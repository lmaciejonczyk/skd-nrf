/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */
#include <zephyr.h>
#include <logging/log.h>
#include <net/socket.h>
#include <net/coap.h>
#include <coap_server_client_interface.h>

#include "coap_utils.h"

LOG_MODULE_REGISTER(coap_client_utils, CONFIG_COAP_CLIENT_UTILS_LOG_LEVEL);

static struct k_work unicast_light_work;
static struct k_work multicast_light_work;
static struct k_work provisioning_work;

/* Options supported by the server */
static const char *const light_option[] = { LIGHT_URI_PATH, NULL };
static const char *const provisioning_option[] = { PROVISIONING_URI_PATH,
						   NULL };

/* Thread multicast mesh local address */
static struct sockaddr_in6 multicast_local_addr = {
	.sin6_family = AF_INET6,
	.sin6_port = htons(COAP_PORT),
	.sin6_addr.s6_addr = { 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
	.sin6_scope_id = 0U
};

/* Variable for storing server address acquiring in provisioning handshake */
static char unique_local_addr_str[INET6_ADDRSTRLEN];
static struct sockaddr_in6 unique_local_addr = { .sin6_family = AF_INET6,
						 .sin6_port = htons(COAP_PORT),
						 .sin6_addr.s6_addr = {0, },
						 .sin6_scope_id = 0U
};

static int on_provisioning_reply(const struct coap_packet *response,
				 struct coap_reply *reply,
				 const struct sockaddr *from)
{
	const u8_t *payload;
	u16_t payload_size = 0u;

	ARG_UNUSED(reply);
	ARG_UNUSED(from);

	payload = coap_packet_get_payload(response, &payload_size);

	if (payload_size != sizeof(unique_local_addr.sin6_addr)) {
		LOG_ERR("Received data size is invalid");
	}

	memcpy(&unique_local_addr.sin6_addr, payload, payload_size);

	if (!inet_ntop(AF_INET6, payload, unique_local_addr_str,
		       INET6_ADDRSTRLEN)) {
		LOG_ERR("Received data is not IPv6 address: %d", errno);
	}

	LOG_INF("Received peer address: %s", log_strdup(unique_local_addr_str));

	return errno;
}

static void toggle_one_light(struct k_work *item)
{
	u8_t payload = (u8_t)THREAD_COAP_UTILS_LIGHT_CMD_TOGGLE;

	ARG_UNUSED(item);

	if (unique_local_addr.sin6_addr.s6_addr16[0] == 0) {
		LOG_WRN("Peer address not set. Activate 'provisioning' option "
			"on the server side");
		return;
	}

	LOG_INF("Send 'light' request to: %s",
		log_strdup(unique_local_addr_str));
	coap_send_request(COAP_METHOD_PUT, &unique_local_addr, light_option,
			  &payload, sizeof(payload), NULL);
}

static void toggle_mesh_lights(struct k_work *item)
{
	static u8_t command = (u8_t)THREAD_COAP_UTILS_LIGHT_CMD_OFF;

	ARG_UNUSED(item);

	command = ((command == THREAD_COAP_UTILS_LIGHT_CMD_OFF) ?
			   THREAD_COAP_UTILS_LIGHT_CMD_ON :
			   THREAD_COAP_UTILS_LIGHT_CMD_OFF);

	LOG_INF("Send multicast mesh 'light' request");
	coap_send_request(COAP_METHOD_PUT, &multicast_local_addr, light_option,
			  &command, sizeof(command), NULL);
}

static void send_provisioning_request(struct k_work *item)
{
	ARG_UNUSED(item);

	LOG_INF("Send 'provisioning' request");
	coap_send_request(COAP_METHOD_GET, &multicast_local_addr,
			  provisioning_option, NULL, 0u, on_provisioning_reply);
}

void coap_client_utils_init(void)
{
	k_work_init(&unicast_light_work, toggle_one_light);
	k_work_init(&multicast_light_work, toggle_mesh_lights);
	k_work_init(&provisioning_work, send_provisioning_request);

	coap_init();
}

void coap_client_toggle_one_light(void)
{
	k_work_submit(&unicast_light_work);
}

void coap_client_toggle_mesh_lights(void)
{
	k_work_submit(&multicast_light_work);
}

void coap_client_send_provisioning_request(void)
{
	k_work_submit(&provisioning_work);
}
