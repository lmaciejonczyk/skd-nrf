#ifndef __COAP_UTILS_H__
#define __COAP_UTILS_H__

#include <net/coap.h>

void coap_init(void);

int coap_send_non_con_request(enum coap_method method, struct in6_addr *dst_addr,
			      const char *const *uri_path_option,
			      u8_t *payload, u16_t payload_size);

int coap_send_con_request(enum coap_method method, struct in6_addr *dst_addr,
			  const char *const *uri_path_options,
			  u8_t *payload, u16_t payload_size, coap_reply_t reply_cb);

#endif
