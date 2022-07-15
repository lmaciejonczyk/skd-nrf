/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <dk_buttons_and_leds.h>

#if defined(CONFIG_BT)
#include "ble.h"
#endif

#include <nrfx.h>
#include <hal/nrf_power.h>
#if !NRF_POWER_HAS_RESETREAS
#include <hal/nrf_reset.h>
#endif

#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>

LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

#define WELLCOME_TEXT                                                                              \
	"\n\r"                                                                                     \
	"\n\r"                                                                                     \
	"OpenThread Command Line Interface is now running.\n\r"                                    \
	"Use the 'ot' keyword to invoke OpenThread commands e.g. "                                 \
	"'ot thread start.'\n\r"                                                                   \
	"For the full commands list refer to the OpenThread CLI "                                  \
	"documentation at:\n\r"                                                                    \
	"https://github.com/openthread/openthread/blob/master/src/cli/README.md\n\r"

/**
 * @brief  Helper function for printing the reason of the last reset.
 * Can be used to confirm that NCF field actually woke up the system.
 */
static void print_reset_reason(void)
{
	uint32_t reas;

#if NRF_POWER_HAS_RESETREAS

	reas = nrf_power_resetreas_get(NRF_POWER);
	nrf_power_resetreas_clear(NRF_POWER, reas);
	if (reas & NRF_POWER_RESETREAS_NFC_MASK) {
		LOG_ERR("Wake up by NFC field detect\n");
	} else if (reas & NRF_POWER_RESETREAS_RESETPIN_MASK) {
		LOG_ERR("Reset by pin-reset\n");
	} else if (reas & NRF_POWER_RESETREAS_SREQ_MASK) {
		LOG_ERR("Reset by soft-reset\n");
	} else if (reas) {
		LOG_ERR("Reset by a different source (0x%08X)\n", reas);
	} else {
		LOG_ERR("Power-on-reset\n");
	}

#else

	reas = nrf_reset_resetreas_get(NRF_RESET);
	nrf_reset_resetreas_clear(NRF_RESET, reas);
	if (reas & NRF_RESET_RESETREAS_NFC_MASK) {
		LOG_ERR("Wake up by NFC field detect\n");
	} else if (reas & NRF_RESET_RESETREAS_RESETPIN_MASK) {
		LOG_ERR("Reset by pin-reset\n");
	} else if (reas & NRF_RESET_RESETREAS_SREQ_MASK) {
		LOG_ERR("Reset by soft-reset\n");
	} else if (reas) {
		LOG_ERR("Reset by a different source (0x%08X)\n", reas);
	} else {
		LOG_ERR("Power-on-reset\n");
	}

#endif
}

void buggy_function(void)
{
	*(uint32_t *)0xbadcafe = 0x0;
}

static void on_button_changed(uint32_t button_state, uint32_t has_changed)
{
	uint32_t buttons = button_state & has_changed;

	if (buttons & DK_BTN1_MSK) {
		buggy_function();
	} else if (buttons & DK_BTN2_MSK) {
		__ASSERT(false, "typical assert");
	}
}

#if !defined(CONFIG_RESET_ON_FATAL_ERROR)
// Zephy's z_arm_fault() function consumes and clears
// the SCB->CFSR register so we must wrap their function
// so we can preserve the pristine fault register values.
void __wrap_z_arm_fault(uint32_t msp, uint32_t psp, uint32_t exc_return,
			_callee_saved_t *callee_regs)
{
	arch_irq_lock();

	while (1) {
		__ASM("nop");
	}
}
#endif

void main(void)
{
#if DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_shell_uart), zephyr_cdc_acm_uart)
	int ret;
	const struct device *dev;
	uint32_t dtr = 0U;

	ret = usb_enable(NULL);
	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return;
	}

	dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));
	if (dev == NULL) {
		LOG_ERR("Failed to find specific UART device");
		return;
	}

	LOG_INF("Waiting for host to be ready to communicate");

	/* Data Terminal Ready - check if host is ready to communicate */
	while (!dtr) {
		ret = uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		if (ret) {
			LOG_ERR("Failed to get Data Terminal Ready line state: %d", ret);
			continue;
		}
		k_msleep(100);
	}

	/* Data Carrier Detect Modem - mark connection as established */
	(void)uart_line_ctrl_set(dev, UART_LINE_CTRL_DCD, 1);
	/* Data Set Ready - the NCP SoC is ready to communicate */
	(void)uart_line_ctrl_set(dev, UART_LINE_CTRL_DSR, 1);
#endif

	print_reset_reason();

	int ret = dk_buttons_init(on_button_changed);
	if (ret) {
		LOG_ERR("Cannot init buttons (error: %d)", ret);
		return;
	}

#if CONFIG_BT
	ble_enable();
#endif
}
