/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr.h>
#include <logging/log.h>
#include <debug/ppi_trace.h>
#include <hal/nrf_radio.h>

#if defined(CONFIG_BT)
#include "ble.h"
#endif

#if defined(CONFIG_USB_UART_CONSOLE)
#include <drivers/uart.h>
#include <usb/usb_device.h>
#endif

LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

static void radio_ppi_trace_setup(void)
{
	uint32_t start_tx;
	uint32_t start_rx;
	uint32_t end_tx_or_rx;
	uint32_t last_bit_on_the_air;
	void *handle;

	end_tx_or_rx = nrf_radio_event_address_get(NRF_RADIO, NRF_RADIO_EVENT_DISABLED);

	start_tx = nrf_radio_event_address_get(NRF_RADIO, NRF_RADIO_EVENT_TXREADY);
	// handle = ppi_trace_config(CONFIG_PPI_TRACE_PIN_START_TX, start_tx);
	handle = ppi_trace_pair_config(CONFIG_PPI_TRACE_PIN_START_TX, start_tx, end_tx_or_rx);
	__ASSERT(handle != NULL, "Failed to configure PPI trace.\n");
	ppi_trace_enable(handle);

	start_rx = nrf_radio_event_address_get(NRF_RADIO, NRF_RADIO_EVENT_RXREADY);
	// handle = ppi_trace_config(CONFIG_PPI_TRACE_PIN_START_RX, start_rx);
	handle = ppi_trace_pair_config(CONFIG_PPI_TRACE_PIN_START_RX, start_rx, end_tx_or_rx);
	__ASSERT(handle != NULL, "Failed to configure PPI trace.\n");
	ppi_trace_enable(handle);

	// handle = ppi_trace_config(CONFIG_PPI_TRACE_PIN_END_RX_OR_TX, end_tx_or_rx);
	// __ASSERT(handle != NULL, "Failed to configure PPI trace.\n");
	// ppi_trace_enable(handle);

	last_bit_on_the_air = nrf_radio_event_address_get(NRF_RADIO, NRF_RADIO_EVENT_PHYEND);
	handle = ppi_trace_config(CONFIG_PPI_TRACE_PIN_LAST_BIT_SEND_ON_THE_AIR,
				  last_bit_on_the_air);
	__ASSERT(handle != NULL, "Failed to configure PPI trace.\n");
	ppi_trace_enable(handle);
}

#define WELLCOME_TEXT \
	"\n\r"\
	"\n\r"\
	"OpenThread Command Line Interface is now running.\n\r" \
	"Use the 'ot' keyword to invoke OpenThread commands e.g. " \
	"'ot thread start.'\n\r" \
	"For the full commands list refer to the OpenThread CLI " \
	"documentation at:\n\r" \
	"https://github.com/openthread/openthread/blob/master/src/cli/README.md\n\r"

void main(void)
{
#if defined(CONFIG_USB_UART_CONSOLE)
	int ret;
	const struct device *dev;
	uint32_t dtr = 0U;

	ret = usb_enable(NULL);
	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return;
	}

	dev = device_get_binding(CONFIG_UART_SHELL_ON_DEV_NAME);
	if (dev == NULL) {
		LOG_ERR("Failed to find specific UART device");
		return;
	}

	LOG_INF("Waiting for host to be ready to communicate");

	/* Data Terminal Ready - check if host is ready to communicate */
	while (!dtr) {
		ret = uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		if (ret) {
			LOG_ERR("Failed to get Data Terminal Ready line state: %d",
				ret);
			continue;
		}
		k_msleep(100);
	}

	/* Data Carrier Detect Modem - mark connection as established */
	(void)uart_line_ctrl_set(dev, UART_LINE_CTRL_DCD, 1);
	/* Data Set Ready - the NCP SoC is ready to communicate */
	(void)uart_line_ctrl_set(dev, UART_LINE_CTRL_DSR, 1);
#endif

	LOG_INF(WELLCOME_TEXT);

	radio_ppi_trace_setup();

#if CONFIG_BT
	ble_enable();
#endif

}
