/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#if defined(CONFIG_BT)
#include "ble.h"
#endif

#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <dk_buttons_and_leds.h>

LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

#define WELLCOME_TEXT \
	"\n\r"\
	"\n\r"\
	"OpenThread Command Line Interface is now running.\n\r" \
	"Use the 'ot' keyword to invoke OpenThread commands e.g. " \
	"'ot thread start.'\n\r" \
	"For the full commands list refer to the OpenThread CLI " \
	"documentation at:\n\r" \
	"https://github.com/openthread/openthread/blob/master/src/cli/README.md\n\r"

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

#if !defined(CONFIG_LOG)
// Zephy's z_arm_fault() function consumes and clears
// the SCB->CFSR register so we must wrap their function
// so we can preserve the pristine fault register values.
void __wrap_z_arm_fault(uint32_t msp, uint32_t psp, uint32_t exc_return,
			_callee_saved_t *callee_regs)
{
	// z_arch_esf_t *esf;
	// bool nested_exc = false;

	// // psp was active prior to exception if bit 2 is set
	// // otherwise, the msp was active
	// if (exc_return & (1 << 2)) {
	// 	/* Returning to thread mode */
	// 	esf = (z_arch_esf_t *)psp;
	// } else {
	// 	/* Returning to handler mode */
	// 	esf = (z_arch_esf_t *)msp;
	// 	nested_exc = true;
	// }

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

	int ret = dk_buttons_init(on_button_changed);
	if (ret) {
		LOG_ERR("Cannot init buttons (error: %d)", ret);
		return;
	}

#if CONFIG_BT
	ble_enable();
#endif

}
