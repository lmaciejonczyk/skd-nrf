.. _coap_client_sample:

Thread: CoAP Client
#######################

The Thread CoAP Client sample demonstrates controlling light resources of other nodes within an OpenThread network.
To show this interaction, the sample requires another sample with a light resource available, for example :ref:`coap_server_sample`.


Overview
********

This example demonstrates how to access resources available on the server node.
After Thread network is established the client node can control the state of LED 4 on server nodes.
It can turn on the LED either on every server node in the network with a multicast message, or on a single specific server node that is paired with the client node.

Following CoAP resources are accessed on the server side":

* ``/light`` - used to control LED 4
* ``/provisioning`` - used to perform provisioning

This sample uses :ref:`Zephyr CoAP API<zephyr:coap_sock_interface>` for communication.
For example usage of native Thread CoAp API refer to :ref:`coap_server_sample`


Requirements
************

* At least one of the following development boards for use as the Thread CoAP Client:

  * |nRF52840DK|
  * |nRF52833DK|

* At least one of the development boards programmed with :ref:`coap_server_sample`.


User interface
**************

The following board buttons are used by this sample:

Button 1:
  Pressing results in sending a unicast LIGHT_TOGGLE message to the ``/light`` resource on a paired server node.
  If no server node is paired with the specific client node, pressing the button has no effect.

Button 2:
  Pressing results in sending a multicast LIGHT_ON or a multicast LIGHT_OFF message (alternatively) to the ``/light`` resource on server nodes.
  Sending this kind of message instead of a LIGHT_TOGGLE one allows to synchronize the state of the LEDs on several server nodes at once.

Button 3:
  Pressing results in sending a multicast pairing request to the ``/provisioning`` resource on the other nodes.

Building and running
********************
.. |sample path| replace:: :file:`samples/net/openthread/coap_client`

.. include:: /includes/build_and_run.txt


Testing
=======

After building the sample, test it by performing the following steps:

#. Program one or more boards with the Thread Simple CoAP Client and the Thread Simple CoAP Server.
#. Turn on the Simple CoAP Server or Simple CoAP Client node.
#. Turn on any of the other nodes. They will enter the network as Children, and Simple CoAP Client or Simple CoAP Server nodes will gradually become Routers.

   .. note::
      It may take up to 15 seconds for the Thread to establish network.

#. Press Button 2 on the client node to control LED 4 all on server nodes.
#. Pair a client with a server.

   * Press Button 4 on a server node to enable pairing.
   * Press Button 3 on any client node to pair the two nodes.

#. Press Button 1 on the client node to control the LED 4 on paired server node.

.. tip::
    You can connect to any of the Simple CoAP Server or Simple CoAP Client nodes through a serial port and run OpenThread CLI commands.
    For more details see :ref:`putty`

      .. note::
          In Zephyr shell every OpenThread command needs to be prepended with the `ot` keyword e.g. ``ot channel 20``.

      .. note::
          For complete CLI documentation, refer to `OpenThread CLI Reference`_.




Dependencies
************

This sample uses the following |NCS| libraries:

* :ref:`dk_buttons_and_leds_readme`

In addition, it uses the following Zephyr libraries:

* :ref:`CoAP <zephyr:coap_sock_interface>`

* :ref:`zephyr:kernel`:

  * ``include/kernel.h``
