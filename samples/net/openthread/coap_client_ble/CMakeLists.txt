#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#
cmake_minimum_required(VERSION 3.13.1)

set(NRF_SUPPORTED_BOARDS
	nrf52840_pca10056
	nrf52833_pca10100
)

include($ENV{ZEPHYR_BASE}/../nrf/cmake/boilerplate.cmake)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)

project(openthread_ble_coap_client)

target_include_directories(app PRIVATE ../coap_interface ../coap_utils)

FILE(GLOB app_sources src/*.c ../coap_utils/*.c)
target_sources(app PRIVATE ${app_sources})
