#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(openthread_cli)

# NORDIC SDK APP START
target_sources(app PRIVATE src/main.c)
# NORDIC SDK APP END

target_sources_ifdef(CONFIG_BT app PRIVATE src/ble.c)

if (NOT CONFIG_LOG)
  message(WARNING "installing proprietary fatal error handler")
#     target_link_libraries(app INTERFACE "-Wl,--wrap=z_fatal_error")
  # We need to wrap z_arm_fault() so we can get a copy
  # of the fault registers before Zephyr consumes and
  # clears the CFSR. See ports/zephyr/common/memfault_platform_core.c.
  zephyr_ld_options(-Wl,--wrap=z_arm_fault)
endif()
