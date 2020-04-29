.. _ug_thread:

Working with Thread
###################

The |NCS| provides support for developing applications using the Thread protocol.

.. _thread_ug_intro:

Introduction
************

The |NCS|'s implementation is based on the OpenThread stack, which is integrated into Zephyr.
The integration with the stack and the radio driver is ensured by Zephyr's L2 layer, which acts as intermediary with Thread on the |NCS| side.

OpenThread is a portable and flexible open-source implementation of the Thread networking protocol, created by Nest in active collaboration with Nordic to accelerate the development of products for the connected home.

Among others, OpenThread has the following main advantages:

* A narrow platform abstraction layer that makes the OpenThread platform-agnostic.
* Small memory footprint.
* Support for system-on-chip (SoC), network co-processor (NCP) and radio co-processor (RCP) designs.
* Official Thread certification.

More information about Thread can be found at `OpenThread.io`_ and `Thread Group`_.

Stack architecture
==================

OpenThread's portable nature makes no assumptions about platform features.
OpenThread provides the hooks to use enhanced radio and crypto features, reducing system requirements, such as memory, code, and compute cycles.
This can be done per platform, while retaining the ability to default to a standard configuration.

.. figure:: images/ot-arch_2x.png
   :alt: OpenThread architecture

   OpenThread architecture

.. _thread_ug_supported features:

Supported features
******************

The OpenThread implementation of the Thread protocol supports all features defined in the Thread 1.1.1 specification.
This includes:

* All Thread networking layers:

    * IPv6
    * 6LoWPAN
    * IEEE 802.15.4 with MAC security
    * Mesh Link Establishment
    * Mesh Routing
    
* All device roles
* Border Router support


Required modules
****************

Thread requires the following Zephyr's modules to properly operate in |NCS|:

* :ref:`zephyr:ieee802154` radio driver - This library is automatically enabled when working with OpenThread on Nordic Semiconductor's Development Kits.
* :ref:`zephyr:settings` subsystem - This is required to allow Thread to store settings in the non-volatile memory.

Configuration
*************

To use the Thread protocol in |NCS|, set the following Kconfig options:

* ``CONFIG_NET_L2_OPENTHREAD`` - This option enables the OpenThread stack required for the correct operation of the Thread protocol and allows you to use them.
* ``CONFIG_CPLUSPLUS`` - This option must be enabled, because OpenThread is implemented in C++.
* ``CONFIG_REBOOT`` - This option is needed to ensure safe reboot.
* ``CONFIG_ENTROPY_GENERATOR`` - Required by both OpenThread and radio driver.
* Options related to the Settings subsystem in the ``storage_partition`` partition of the internal flash:

    * ``CONFIG_SETTINGS``
    * ``CONFIG_FLASH``
    * ``CONFIG_FLASH_PAGE_LAYOUT``
    * ``CONFIG_FLASH_MAP``
    * ``CONFIG_MPU_ALLOW_FLASH_WRITE``
    * ``CONFIG_NVS``

* General setting options related to network configuration:

    * ``CONFIG_NETWORKING`` 
    * ``CONFIG_NET_UDP``
    * ``CONFIG_NET_SOCKETS``

IPv6 mandatory configuration
============================

The Thread protocol can only be used with IPv6.
IPv4 is not supported.

Enable the following options to make Thread work over IPv6:

* ``CONFIG_NET_IPV6``
* ``CONFIG_NET_CONFIG_NEED_IPV6``

Additionally, since Thread by default registers a considerable amount of IP addresses, the default IPv6 address count values must be increased.
Set the following options to the provided values:

* ``CONFIG_NET_IF_UNICAST_IPV6_ADDR_COUNT=6``
* ``CONFIG_NET_IF_MCAST_IPV6_ADDR_COUNT=8``

Optional configuration options
==============================

Depending on your configuration needs, you can also set the following options:

* ``CONFIG_NET_SHELL`` - This option enables Zephyr's :ref:`zephyr:shell_label` if you need to access OpenThread CLI.
* ``CONFIG_COAP`` - This option enables Zephyr's :ref:`zephyr:coap_sock_interface` support.
* ``CONFIG_OPENTHREAD_COAP`` - This option enables OpenThread's native CoAP API.

You can also change the default values for the following options:

* ``CONFIG_OPENTHREAD_CHANNEL`` - By default set to ``11``.
  You can set any value ranging from ``11`` to ``26``.
* ``CONFIG_OPENTHREAD_PANID`` - By default set to ``43981``.
  You can set any value ranging from ``0`` to ``65535``.

Thread commissioner
-------------------

Thread commissioner coordinates the process of adding new Thread devices to the network.
It is optional, because the Thread :ref:`samples` in |NCS| use a hardcoded network information.

To enable Thread commissioner, set the following Kconfig options to the provided values:

* ``CONFIG_OPENTHREAD_COMMISSIONER`` to ``y``.
* ``CONFIG_MBEDTLS_HEAP_SIZE`` to ``8192``.
* ``CONFIG_SHELL_STACK_SIZE`` to ``3072``.

OpenThread stack logging options
--------------------------------

The OpenThread stack logging is handled with the following options:

* ``CONFIG_LOG`` - This option enables Zephyr's :ref:`zephyr:logger`.
* ``CONFIG_OPENTHREAD_DEBUG`` - This option enables logging for the OpenThread stack.

Both options must be enabled to allow logging.

This said, enabling logging is optional, because it is enabled by default for all Thread samples.
However, you must set one of the following logging levels to start receiving the logging output:

* ``CONFIG_OPENTHREAD_LOG_LEVEL_ERROR`` - Enables logging only for errors.
* ``CONFIG_OPENTHREAD_LOG_LEVEL_WARNING`` - Enables logging for errors and warnings.
* ``CONFIG_OPENTHREAD_LOG_LEVEL_INFO`` - Enables logging for informational messages, errors, and warnings.
* ``CONFIG_OPENTHREAD_LOG_LEVEL_DEBUG`` - Enables logging for debug messages, informational messages, errors, and warnings.

Zephyr L2 logging options
-------------------------

If you want to get logging output related to the Zephyr's L2 layer, enable one of the following Kconfig options:

* ``CONFIG_OPENTHREAD_L2_LOG_LEVEL_ERR`` - Enables logging only for errors.
* ``CONFIG_OPENTHREAD_L2_LOG_LEVEL_WRN`` - Enables logging for errors and warnings.
* ``CONFIG_OPENTHREAD_L2_LOG_LEVEL_INF`` - Enables logging for informational messages, errors, and warnings.
* ``CONFIG_OPENTHREAD_L2_LOG_LEVEL_DBG`` - Enables logging for debug messages, informational messages, errors, and warnings.

Chosing one of these options will enable writing the appropriate information in the L2 debug log.

Additionally, enabling ``CONFIG_OPENTHREAD_L2_LOG_LEVEL_DBG`` allows you to set the ``CONFIG_OPENTHREAD_L2_DEBUG`` option, which in turn has the following settings:

* ``CONFIG_OPENTHREAD_L2_DEBUG_DUMP_15_4``
* ``CONFIG_OPENTHREAD_L2_DEBUG_DUMP_IPV6``

These options enable dumping 802.15.4 or IPv6 frames (or both) in the debug log output.

You can disable writing to log with the ``CONFIG_OPENTHREAD_L2_LOG_LEVEL_OFF`` option.

Available drivers, libraries, and samples
*****************************************

See :ref:`samples` for the list of available Thread samples.

Copyright disclaimer
********************

|Google_CCLicense|