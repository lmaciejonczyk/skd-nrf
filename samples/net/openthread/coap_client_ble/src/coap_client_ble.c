/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */
#include <zephyr.h>
#include <coap_client_utils.h>
#include <dk_buttons_and_leds.h>
#include <logging/log.h>

#include "ble_utils.h"

LOG_MODULE_REGISTER(coap_client_ble, CONFIG_COAP_CLIENT_BLE_LOG_LEVEL);

#define COMMAND_REQUEST_UNICAST 'u'
#define COMMAND_REQUEST_MULTICAST 'm'
#define COMMAND_REQUEST_PROVISIONING 'p'

#define CON_STATUS_LED DK_LED2

#define RUN_STATUS_LED DK_LED1
#define RUN_LED_BLINK_INTERVAL 1000

static void on_nus_received(struct bt_conn *conn, const u8_t *const data,
			    u16_t len)
{
	LOG_INF("Received data: %s", log_strdup(data));

	if (len != 1) {
		LOG_WRN("Received invalid data length from NUS");
		return;
	}

	switch (*data) {
	case COMMAND_REQUEST_UNICAST:
		coap_client_toggle_one_light();
		break;

	case COMMAND_REQUEST_MULTICAST:
		coap_client_toggle_mesh_lights();
		break;

	case COMMAND_REQUEST_PROVISIONING:
		coap_client_send_provisioning_request();
		break;

	default:
		LOG_WRN("Received invalid data from NUS");
	}
}

static void on_button_changed(u32_t button_state, u32_t has_changed)
{
	u32_t buttons = button_state & has_changed;

	if (buttons & DK_BTN1_MSK) {
		coap_client_toggle_one_light();
	}

	if (buttons & DK_BTN2_MSK) {
		coap_client_toggle_mesh_lights();
	}

	if (buttons & DK_BTN3_MSK) {
		coap_client_send_provisioning_request();
	}
}

static void on_ble_connect(struct k_work *item)
{
	dk_set_led_on(CON_STATUS_LED);
}

static void on_ble_disconnect(struct k_work *item)
{
	dk_set_led_off(CON_STATUS_LED);
}

void main(void)
{
	int ret;
	int blink_status = 0;

	LOG_INF("Starting Multiprotocol: BLE - OT CoAP example");

	ret = dk_leds_init();
	if (ret) {
		LOG_ERR("Could not initialize leds (error: %d)", ret);
		goto error;
	}

	ret = dk_buttons_init(on_button_changed);
	if (ret) {
		LOG_ERR("Could not initialize buttons (error: %d)", ret);
		goto error;
	}

	ret = ble_utils_init(&on_nus_received, NULL, on_ble_connect,
			     on_ble_disconnect);
	if (ret) {
		LOG_ERR("ble_utils_init failed");
		goto error;
	}

	coap_client_utils_init();

	for (;;) {
		dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);
		k_sleep(RUN_LED_BLINK_INTERVAL);
	}

error:
	dk_set_leds_state(DK_ALL_LEDS_MSK, DK_NO_LEDS_MSK);
}
