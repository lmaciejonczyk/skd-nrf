#include "coap_utils.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(coap_utils, CONFIG_COAP_UTILS_LOG_LEVEL);

#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <net/socket.h>
#include <net/net_mgmt.h>
#include <net/net_ip.h>
#include <net/udp.h>
#include <net/coap.h>

#define PEER_PORT 5683
#define MAX_COAP_MSG_LEN 256
#define COAP_VER 1
#define COAP_TOKEN_LEN 8
#define COAP_MAX_REPLIES 1
#define COAP_POOL_SLEEP 500
#define COAP_OPEN_SOCKET_SLEEP 200
#define COAP_RECEIVE_STACK_SIZE 500

static K_THREAD_STACK_DEFINE(receive_stack_area, COAP_RECEIVE_STACK_SIZE);
static struct k_thread receive_thread_data;

const static int nfds = 1;
static struct pollfd fds;
static struct coap_reply replies[COAP_MAX_REPLIES];

static void coap_open_socket(int *sock) {
	while (1) {
		*sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		if (*sock < 0) {
			LOG_ERR("Failed to create UDP socket %d", errno);
			k_sleep(COAP_OPEN_SOCKET_SLEEP);
			continue;
		}
		break;
	}
}

static void coap_close_socket(int socket) {
	(void)close(socket);
}

static void coap_receive()
{
	static u8_t buf[MAX_COAP_MSG_LEN + 1];

	struct coap_packet response;
	struct coap_reply *reply = NULL;
	static struct sockaddr from_addr;
	socklen_t from_addr_len;
	int len;
	int ret;

	while (1) {
		if (poll(&fds, nfds, K_FOREVER) < 0) {
			LOG_ERR("Error in poll:%d", errno);
			errno = 0;
			k_sleep(COAP_POOL_SLEEP);
			continue;
		}

		if (fds.revents & POLLERR) {
			LOG_ERR("Error in poll.. waiting a moment.");
			fds.revents = 0;
			k_sleep(COAP_POOL_SLEEP);
			continue;
		}

		if (fds.revents & POLLHUP) {
			LOG_ERR("Error in poll: POLLHUP");
			fds.revents = 0;
			continue;
		}

		if (fds.revents & POLLNVAL) {
			LOG_ERR("Error in poll: POLLNVAL - fd not open");
			fds.revents = 0;

			coap_close_socket(fds.fd);
			coap_open_socket(&(fds.fd));

			LOG_INF("Socket has been re-open");

			continue;
		}

		if (!(fds.revents & POLLIN)) {
			LOG_ERR("Unknown poll error");
			fds.revents = 0;
			continue;
		}

		fds.revents = 0;

		len = recvfrom(fds.fd, buf, sizeof(buf) - 1, 0, &from_addr,
			       &from_addr_len);

		if (len < 0) {
			LOG_ERR("Error reading response: %d", errno);
			errno = 0;
			continue;
		}

		if (len == 0) {
			LOG_ERR("Zero length recv");
			continue;
		}

		buf[len] = 0u;

		ret = coap_packet_parse(&response, buf, len, NULL, 0);
		if (ret < 0) {
			LOG_ERR("Invalid data received");
			continue;
		}

		reply = coap_response_received(&response, &from_addr, replies,
					       COAP_MAX_REPLIES);
		if (reply) {
			coap_reply_clear(reply);
		}
	}
}

static int coap_init_request(enum coap_method method,
			     enum coap_msgtype msg_type,
			     const char *const *uri_path_options, u8_t *payload,
			     u16_t payload_size, struct coap_packet *request,
			     u8_t *buf)
{
	const char *const *opt;
	int ret;

	ret = coap_packet_init(request, buf, MAX_COAP_MSG_LEN, COAP_VER,
			       msg_type, COAP_TOKEN_LEN, coap_next_token(),
			       method, coap_next_id());
	if (ret < 0) {
		LOG_ERR("Failed to init CoAP message");
		goto end;
	}

	for (opt = uri_path_options; opt && *opt; opt++) {
		ret = coap_packet_append_option(request, COAP_OPTION_URI_PATH,
						*opt, strlen(*opt));
		if (ret < 0) {
			LOG_ERR("Unable add option to request");
			goto end;
		}
	}

	if (payload) {
		ret = coap_packet_append_payload_marker(request);
		if (ret < 0) {
			LOG_ERR("Unable to append payload marker");
			goto end;
		}

		ret = coap_packet_append_payload(request, payload,
						 payload_size);
		if (ret < 0) {
			LOG_ERR("Not able to append payload");
			goto end;
		}
	}

end:
	return ret;
}

static int coap_send_message(struct in6_addr *dst_addr,
			     struct coap_packet *request)
{
	struct sockaddr_in6 addr6;

	addr6.sin6_family = AF_INET6;
	addr6.sin6_port = htons(PEER_PORT);
	addr6.sin6_scope_id = 0U;

	memcpy(&addr6.sin6_addr, dst_addr, sizeof(addr6.sin6_addr));

	return sendto(fds.fd, request->data, request->offset, 0,
		      (struct sockaddr *)&addr6, sizeof(addr6));
}

static void coap_set_response_callback(struct coap_packet *request,
				      coap_reply_t reply_cb)
{
	struct coap_reply *reply;

	reply = &replies[0];

	coap_reply_clear(reply);
	coap_reply_init(reply, request);
	reply->reply = reply_cb;
}

void coap_init(void)
{
	fds.events = POLLIN;
	fds.revents = 0;

	coap_open_socket(&(fds.fd));

	/* start sock receive thread */
	k_thread_create(&receive_thread_data, receive_stack_area,
			K_THREAD_STACK_SIZEOF(receive_stack_area),
			(k_thread_entry_t)coap_receive, NULL, NULL, NULL,
			/* Lowest priority cooperative thread */
			K_PRIO_COOP(CONFIG_NUM_COOP_PRIORITIES - 1), 0,
			K_NO_WAIT);
	k_thread_name_set(&receive_thread_data, "CoAP-sock-recv");
	LOG_DBG("CoAP socket receive thread started");
}

int coap_send_non_con_request(enum coap_method method,
			      struct in6_addr *dst_addr,
			      const char *const *uri_path_options,
			      u8_t *payload, u16_t payload_size)
{
	int ret;
	struct coap_packet request;
	u8_t buf[MAX_COAP_MSG_LEN];

	ret = coap_init_request(method, COAP_TYPE_NON_CON, uri_path_options,
				payload, payload_size, &request, buf);
	if (ret < 0) {
		goto end;
	}

	ret = coap_send_message(dst_addr, &request);
	if (ret < 0) {
		LOG_ERR("Transmission failed: %d", errno);
		goto end;
	}

end:
	return ret;
}

int coap_send_con_request(enum coap_method method, struct in6_addr *dst_addr,
			  const char *const *uri_path_options, u8_t *payload,
			  u16_t payload_size, coap_reply_t reply_cb)
{
	int ret;
	struct coap_packet request;
	u8_t buf[MAX_COAP_MSG_LEN];

	ret = coap_init_request(method, COAP_TYPE_NON_CON, uri_path_options,
				payload, payload_size, &request, buf);
	if (ret < 0) {
		goto end;
	}

	coap_set_response_callback(&request, reply_cb);

	ret = coap_send_message(dst_addr, &request);
	if (ret < 0) {
		LOG_ERR("Transmission failed: %d", errno);
		goto end;
	}

end:
	return ret;
}
