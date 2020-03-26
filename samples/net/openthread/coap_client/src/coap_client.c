/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */
#include <logging/log.h>
LOG_MODULE_REGISTER(coap_client, CONFIG_COAP_CLIENT_LOG_LEVEL);

#include <logging/log_ctrl.h>
#include <errno.h>
#include <zephyr.h>
#include <net/socket.h>
#include <dk_buttons_and_leds.h>

#include "coap_utils.h"

#define SLEEP_TIME 500
#define IPV6_ADDR_BYTE_LENGTH 16

/**@brief Enumeration describing light commands. */
typedef enum {
	THREAD_COAP_UTILS_LIGHT_CMD_OFF = '0',
	THREAD_COAP_UTILS_LIGHT_CMD_ON = '1',
	THREAD_COAP_UTILS_LIGHT_CMD_TOGGLE = '2'
} thread_coap_utils_light_command_t;

/* Options supported by the server */
static const char *const light_option[] = { "light", NULL };
static const char *const provisioning_option[] = { "provisioning", NULL };

/* Thread multicast mesh local address */
static const char *multicast_mesh_local_str_addr = "ff03::1";
static struct in6_addr multicast_mesh_local_addr = { .s6_addr = 0 };

/* Variables for storing a server address required in a provisioning handshake */
static char peer_addr_str[INET6_ADDRSTRLEN];
struct in6_addr peer_addr = { .s6_addr = 0 };

static struct k_work unicast_light_work;
static struct k_work multicast_light_work;
static struct k_work provisioning_work;

static int set_multicast_addr(const char *addr)
{
	int ret = 0;

	if (multicast_mesh_local_addr.s6_addr16[0] == 0) {
		ret = inet_pton(AF_INET6, multicast_mesh_local_str_addr,
				&multicast_mesh_local_addr);
		if (ret == 0) {
			LOG_ERR("Invalid IPv6 address");
			ret = -1;
		}
	}

	return ret;
}

static int on_provisioning_reply(const struct coap_packet *response,
				 struct coap_reply *reply,
				 const struct sockaddr *from)
{
	const u8_t *payload;
	u16_t payload_size = 0u;

	ARG_UNUSED(reply);
	ARG_UNUSED(from);

	payload = coap_packet_get_payload(response, &payload_size);

	if (payload_size != sizeof(peer_addr)) {
		LOG_ERR("Received data size is invalid");
	}

	memcpy(&peer_addr, payload, payload_size);

	if (!inet_ntop(AF_INET6, payload, peer_addr_str, INET6_ADDRSTRLEN)) {
		LOG_ERR("Received data is not IPv6 address: %d", errno);
	}

	LOG_INF("Received peer address: %s", log_strdup(peer_addr_str));

	return errno;
}

static void toggle_one_light(struct k_work *item)
{
	u8_t payload = (u8_t)THREAD_COAP_UTILS_LIGHT_CMD_TOGGLE;

	ARG_UNUSED(item);

	if (peer_addr.s6_addr16[0] == 0) {
		LOG_WRN("Peer address not set. Activate 'provisioning' option on the server side");
		return;
	}

	LOG_INF("Send 'light' request to: %s", log_strdup(peer_addr_str));
	coap_send_non_con_request(COAP_METHOD_PUT, &peer_addr, light_option,
				  &payload, sizeof(payload));
}

static void toggle_mesh_lights(struct k_work *item)
{
	static u8_t command = (u8_t)THREAD_COAP_UTILS_LIGHT_CMD_OFF;

	ARG_UNUSED(item);

	command = ((command == THREAD_COAP_UTILS_LIGHT_CMD_OFF) ?
			     THREAD_COAP_UTILS_LIGHT_CMD_ON :
			     THREAD_COAP_UTILS_LIGHT_CMD_OFF);

	if (set_multicast_addr(multicast_mesh_local_str_addr) < 0) {
		return;
	}

	LOG_INF("Send multicast mesh 'light' request");
	coap_send_non_con_request(COAP_METHOD_PUT, &multicast_mesh_local_addr,
				  light_option, &command, sizeof(command));
}

static void send_provisioning_request(struct k_work *item)
{
	ARG_UNUSED(item);

	if (set_multicast_addr(multicast_mesh_local_str_addr) < 0) {
		return;
	}

	LOG_INF("Send 'provisioning' request");
	coap_send_con_request(COAP_METHOD_GET, &multicast_mesh_local_addr,
			      provisioning_option, NULL, 0u,
			      on_provisioning_reply);
}

static void on_button_changed(u32_t button_state, u32_t has_changed)
{
	u32_t buttons = button_state & has_changed;

	if (buttons & DK_BTN1_MSK) {
		k_work_submit(&unicast_light_work);
	}

	if (buttons & DK_BTN2_MSK) {
		k_work_submit(&multicast_light_work);
	}

	if (buttons & DK_BTN3_MSK) {
		k_work_submit(&provisioning_work);
	}
}

void main(void)
{
	int ret;

	LOG_INF("Start CoAP-client sample");

	k_work_init(&unicast_light_work, toggle_one_light);
	k_work_init(&multicast_light_work, toggle_mesh_lights);
	k_work_init(&provisioning_work, send_provisioning_request);

	ret = dk_buttons_init(on_button_changed);
	if (ret) {
		LOG_ERR("Cannot init buttons (error: %d)\n", ret);
		return;
	}

	coap_init();
}
