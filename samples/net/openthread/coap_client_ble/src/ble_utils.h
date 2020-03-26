#ifndef __BLE_UTILS_H__
#define __BLE_UTILS_H__

#include <bluetooth/services/nus.h>

typedef void (*ble_connection_cb_t)(struct k_work *item);
typedef void (*ble_disconnection_cb_t)(struct k_work *item);

int ble_utils_init(nus_received_cb_t on_nus_received, nus_sent_cb_t on_nus_send,
		   ble_connection_cb_t on_connect,
		   ble_disconnection_cb_t on_disconnect);

#endif
