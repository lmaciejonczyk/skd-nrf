.. _coap_client_multiprotocol:

CoAP client - multiprotocol
###########################

The CoAP client - multiprotocol sample demonstrates concurent working OpenThread stack and :ref:`nRF Bluetooth LE Controller <nrfxlib:ble_controller>`.
The sample is an extension of :ref:`coap_client_sample_readme`.

Overview
********

The sample application uses the :ref:`nus_service_readme` library to control the leds states over Bluetooth LE in the mesh Thread network.
It requires e.g. :ref:`coap_server_sample` for the interaction.

Requirements
************

* One of the following development boards:

  * |nRF52840DK|
  * |nRF52833DK|

* At least one of the development boards programmed with :ref:`coap_server_sample`.
* A phone or tablet running a compatible application.
  The `Testing`_ instructions refer to `nRF Toolbox`_, but similar applications (for example, `nRF Connect for Mobile`_) can be used as well.

User interface
***************

LED 1:
  * Blinking indicates working application.

LED 2:
  * On when Bluetooth LE connection is established.

All LEDs:
  * On when the error occurred during the application initialization.

Button 1:
  * Send a unicast LIGHT_TOGGLE message to the ``/light`` resource on a paired device. 
    If no device is paired with a specific client node, pressing the button has no effect.

Button 2:
  * Send a multicast LIGHT_ON or a LIGHT_OFF message (alternatively) to the ``/light`` resource.
    Sending a LIGHT_ON/LIGHT_OFF message instead of LIGHT_TOGGLE allows to synchronize the state of the LEDs on several server nodes.

Button 4:
  * Send a multicast pairing request to the ``/provisioning`` resource.

UART command assigments:
  * "u" - performs the same operation as Button 1 (send a unicast CoAP message over Thread)
  * "m" - performs the same operation as Button 2 (send a multicast CoAP message over Thread)
  * "p" - performs the same operation as Button 4 (send a pairing request as CoAP message over Thread)

Building and running
********************
.. |sample path| replace:: :file:`samples/net/openthread/coap_client_ble`

.. include:: /includes/build_and_run.txt

Testing
=======

1. Turn on the CoAP client - multiprotocol and at least one CoAP server node.
   They will create the Thread network.
   
   .. note::
     It may take up to 15 seconds for nodes to establish network.
     When the sample application is ready the LED 1 starts blinking

#. Set up nRF Toolbox

   .. tabs::

     .. tab:: Step 1
     
        * Open nRF Toolbox and choose the UART application.

          .. figure:: images/nrftoolbox_uart_default.png
             :alt: Configured nRF Toolbox - UART application

             nRF Toolbox - UART application

     .. tab:: Step 2

        1. Tap EDIT.
	#. When the Create button window appears create three active application buttons:

           * Enter command "u" (EOL = LF)
           * Enter command "m" (EOL = LF)
           * Enter command "p" (EOL = LF)

        #. After all buttons are created, tap DONE.

           .. figure:: images/nrftoolbox_uart_settings.png
              :alt: A window for configuring button in nRF Toolbox - UART application

              nRF Toolbox - UART application - Configure button

     .. tab:: Step 3

        * Tap CONNECT and select the NUS_CoAP_client device.

          .. figure:: images/nrftoolbox_uart_connected.png
             :alt: nRF Toolbox - UART application view after establishing connection

             nRF Toolbox - UART application after establishing the connection

        .. note::
           Observe that LED 2 on your CoAP client - multiprotocol device is solid, which indicates that Bluetooth connection is established.

#. You can control LED 4 on every CoAP server nodes by pressing the middle button ("m" - command) from your smartphone.

#. Pair a client with a server:

   * Press Button 4 on a server node to enable pairing.
   * Press the right button in `nRF Toolbox`_ ("p" command) to pair the two nodes.

#. You can now control the LED4 on this server node by pressing the left button in `nRF Toolbox`_ ("u" - command).
   You can also control CoAP servers directly from the board by pressing Button 1, Button 2, or Button 4

Sample output
==============

The sample logging output can be observed through a serial port.
For more details see :ref:`putty`.

Dependencies
************

This sample uses the following |NCS| libraries:

* :ref:`nRF Bluetooth LE Controller <nrfxlib:ble_controller>`
* :ref:`nus_service_readme`
* :ref:`dk_buttons_and_leds_readme`

In addition, it uses the following Zephyr libraries:

* :ref:`zephyr:coap_sock_interface`

  * ``include/net/coap.h``

* :ref:`zephyr:bluetooth_api`:

  * ``include/bluetooth/bluetooth.h``
  * ``include/bluetooth/gatt.h``
  * ``include/bluetooth/hci.h``
  * ``include/bluetooth/uuid.h``

* :ref:`zephyr:logger`:

  * ``include/logging/log.h``

* :ref:`zephyr:kernel`:

  * ``include/kernel.h``
