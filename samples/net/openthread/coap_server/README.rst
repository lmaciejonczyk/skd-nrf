.. _coap_server_sample:

Thread: CoAP Server
#######################

The Thread CoAP Server sample demonstrates controlling light resources of other nodes within an OpenThread network.
This sample exposes resorces in the network and requires another sample to access them, for example :ref:`coap_client_sample`.


Overview
********
This example demnstrates how to expose resources that can be accessed by other devices in the network.
You can use this sample application as a starting point to implement a :ref:`CoAP <zephyr:coap_sock_interface>` application.

Following CoAP resources are exposed on the network by this sample":

* ``/light`` - used to control LED 4
* ``/provisioning`` - used to perform provisioning


This sample uses native `OpenThread CoAP API`_ for communication.

For example usage of Zephyr CoAp API refer to :ref:`coap_client_sample`

Requirements
************

* At least one of the following development boards for use as the Thread CoAP Server:

  * |nRF52840DK|
  * |nRF52833DK|

* At least one of the development boards programmed with :ref:`coap_client_sample`.

User interface
**************

Button 4:
    Pressing results in entering the pairing mode for a the limited period of time.

LED 3:
    Blinks when the pairing mode is enabled.

LED 4:
    Turned on and off by messages sent from the client nodes.

Building and running
********************
.. |sample path| replace:: :file:`samples/net/openthread/coap_server`

.. include:: /includes/build_and_run.txt

Testing
=======

After building the sample, test it by performing the following steps:

#. Program one or more boards with the Thread Simple CoAP Client and the Thread Simple CoAP Server.
#. Turn on the Simple CoAP Server or Simple CoAP Client node.
#. Turn on any of the other nodes.
   They will enter the network as Children, and Simple CoAP Client or Simple CoAP Server nodes will gradually become Routers.

   .. note::
      It may take up to 15 seconds for the Thread to establish network.

#. Press Button 2 on the client node to control LED 4 on all server nodes.
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

* :ref:`zephyr:kernel`:

  * ``include/kernel.h``

OpenThread CoAP API is used in this sample:

* `OpenThread CoAP API`_
