.. _coap_client_sample:

openthread: coap_client
##################

The openthread coap_client sample demonstrates interactions between nodes performing different Thread roles with the use of OpenThread and CoAP protocol.

Overview
********

The client node can control the state of BSP_LED_3 on server nodes. It can turn on the LED either on every server node in the network with a multicast message, or on a single specific server node that is paired with the client node.


Requirements
************

* One of the following development boards:

  * |nRF5340DK|
  

User interface
**************

The following board buttons are used by this sample:

BUTTON 1:
   Pressing results in sending a unicast LIGHT_TOGGLE message to the /light resource on a paired device. If no device is paired with the specific client node, pressing the button has no effect.

BUTTON 2:
    Pressing results in sending a multicast LIGHT_ON or a LIGHT_OFF message (alternatively) to the /light resource. Sending a LIGHT_ON/LIGHT_OFF message instead of LIGHT_TOGGLE allows to synchronize the state of the LEDs on several server nodes.

BUTTON 3:
    Pressing results in sending a multicast pairing request to the /provisioning resource.


Testing
=======

After programming the sample to your board, test it by performing the following steps:

1. Build the projects
2. Program one or more boards with the Thread Simple CoAP Client, the Thread Simple CoAP Server.
3. Turn on the Simple CoAP Server or Simple CoAP Client node and wait for a while.
4. Turn on any of the other nodes. They will enter the network as Children, and Simple CoAP Client or Simple CoAP Server nodes will gradually become Routers.
5. You can control LED 4 on every server node by pressing BUTTON 2 on any client node.
6. Pair a client with a server. Press BUTTON 4 on a server node to enable pairing. Then, press BUTTON 3 on any client node to pair the two nodes. You can now control the LED 4 on this server node by pressing BUTTON 1 on the client node.
7. Optionally, you can connect to any of the Simple CoAP Server or Simple CoAP Client nodes through a serial port and run OT CLI commands. The application uses the following UART settings:
        Baud rate: 115.200
        8 data bits
        1 stop bit
        No parity
        HW flow control: None
    For complete CLI documentation, refer to OpenThread CLI Reference.


Dependencies
************

This sample uses the following |NCS| libraries:

* :ref:`dk_buttons_and_leds_readme`

In addition, it uses the following Zephyr libraries:

* :ref:`zephyr:kernel`:

  * ``include/kernel.h``
