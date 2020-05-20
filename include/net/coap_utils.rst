.. _coap_utils_readme:

CoAP Utils library
##################

CoAP Utils library is a simple module to communicate with CoAP protocol.
It supports sending and receiving non-confirmable CoAP requests.
It utilizes :ref:`CoAP <zephyr:coap_sock_interface>` and :ref:`BSD socket API <bsd_sockets_interface>`.

Limitations
***********

The library supports UDP protocol only.

Configuration
*************

:option:`CONFIG_COAP_UTILS`

   This option enables CoAP Utils library.

API documentation
*****************

| Header file: :file:`include/net/coap_utils.h`
| Source files: :file:`subsys/net/lib/coap_utils/`

.. doxygengroup:: coap_utils
   :project: nrf
   :members:
