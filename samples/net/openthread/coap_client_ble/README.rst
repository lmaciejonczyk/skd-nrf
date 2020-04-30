.. _coap_client_multiprotocol_sample:

Thread: CoAP Multiprotocol Client 
#################################

The CoAP Multiprotocol Client sample demonstrates the OpenThread stack and :ref:`nRF Bluetooth LE Controller <nrfxlib:ble_controller>` working concurrently.
This sample is a multiprotocol extension of :ref:`coap_client_sample`.

Overview
********

The sample application uses the :ref:`nus_service_readme` library to control the LED states over Bluetooth LE in a Thread network.
To show this interaction, the sample requires a server sample that is compatible with the OpenThread network.
The recommended server sample referenced on this page is :ref:`coap_server_sample`.

Requirements
************

* One of the following development kits:

  * |nRF52840DK|
  * |nRF52833DK|

* One or more compatible development kits programmed with the :ref:`coap_server_sample` sample.
* A phone or tablet with the `nRF Toolbox`_ application installed.

  .. note::
    The `Testing`_ instructions refer to nRF Toolbox, but similar applications can be used as well, for example `nRF Connect for Mobile`_.

User interface
***************

All LEDs:
    On when an error occurred during the application initialization.

LED 1:
    Blinking indicates working application.

LED 2:
    On when Bluetooth LE connection is established.

Button 1:
    Send a unicast ``LIGHT_TOGGLE`` message to the ``/light`` resource on a paired device. 
    If no device is paired with the specific client node, pressing the button has no effect.

Button 2:
    Send a multicast ``LIGHT_ON`` or ``LIGHT_OFF`` message (alternatively) to the ``/light`` resource.
    Sending this multicast message instead of ``LIGHT_TOGGLE`` allows to synchronize the state of the LEDs on several server nodes.

Button 4:
    Send a multicast pairing request to the ``/provisioning`` resource.

UART command assigments:
    The following command assignments are configured and used in nRF Toolbox:

    * ``u`` - Send a unicast CoAP message over Thread (the same operation as Button 1).
    * ``m`` - Send a multicast CoAP message over Thread (the same operation as Button 2).
    * ``p`` - Send a pairing request as CoAP message over Thread (the same operation as Button 4).

Building and running
********************
.. |sample path| replace:: :file:`samples/net/openthread/coap_client_ble`

.. include:: /includes/build_and_run.txt

Testing
=======

After building the sample and programming it to your development kit, test it by performing the following steps:

1. Program at least one development kit with :ref:`coap_server_sample` and reset it.
#. Turn on the CoAP Multiprotocol Client and at least one CoAP server node.
   They will create the Thread network.
   
   .. note::
     It may take up to 15 seconds for nodes to establish the network.
     When the sample application is ready, the LED 1 starts blinking.

#. Set up nRF Toolbox by completing the following steps:

   .. tabs::

      .. tab:: 1. Start UART
     
         Tap :guilabel:`UART` to open the UART application in nRF Toolbox.
 
         .. figure:: images/nrftoolbox_uart_default.png
            :alt: UART application in nRF Toolbox
 
            UART application in nRF Toolbox 
 
      .. tab:: 2. Configure commands
  
         Configure the UART commands by completing the following steps:
         
         1. Tap the :guilabel:`EDIT` button in the top right corner of the application.
            The button configuration window appears.
         #. Create the active application buttons by completing the following steps:
  
            a. Bind the top left button to the ``u`` command, with EOL set to LF and an icon of your choice.
               For this testing procedure, the :guilabel:`>` icon is used.
            #. Bind the top middle button to the ``m`` command, with EOL set to LF and an icon of your choice.
               For this testing procedure, the play button icon is used.
            #. Bind the top left button to the ``p`` command, with EOL set to LF and an icon of your choice.
               For this testing procedure, the settings gear icon is used.
  
            .. figure:: images/nrftoolbox_uart_settings.png
               :alt: Configuring buttons in nRF Toolbox - UART application
  
               Configuring buttons in the UART application of nRF Toolbox
            
         #. Tap the :guilabel:`DONE` button in the top right corner of the application.      
  
      .. tab:: 3. Connect to device
  
         Tap :guilabel:`CONNECT` and select the ``NUS_CoAP_client`` device from the list of devices.
  
         .. figure:: images/nrftoolbox_uart_connected.png
            :alt: nRF Toolbox - UART application view after establishing connection
  
            The UART application of nRF Toolbox after establishing the connection
  
         .. note::
            Observe that LED 2 on your CoAP Multiprotocol Client node is solid, which indicates that the Bluetooth connection is established.
   ..

#. In nRF Toolbox, press the middle button to control LED 4 on all CoAP server nodes. 
#. Pair a client with a server by completing the following steps:

   a. Press Button 4 on a server node to enable pairing.
   #. In nRF Toolbox, press the left button to pair the two nodes.

#. In nRF Toolbox, press the right button to control LED 4 on the paired server node.
   You can also control CoAP servers directly by pressing Button 1, Button 2, or Button 4 on the client development kit.

Sample output
==============

The sample logging output can be observed through a serial port.
For more details, see :ref:`putty`.

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
