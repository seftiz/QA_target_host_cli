

## Copyright (C) 2013-2014 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory

#SDK_DIR := ../../..

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/craton.mk
include $(SDK_DIR)/build/u-boot.mk

CFLAGS := -std=c99 -pedantic -Wall -Wextra -Werror -g -Os

.DEFAULT_GOAL := arm/img/v2x-cli.img

arm/bin/v2x-cli: \
    arm/obj/$(COMMON)/nav_api/nav_api.o \
    arm/obj/$(COMMON)/link/link.o \
    arm/obj/$(COMMON)/management/mng_api.o \
    arm/obj/phy/phy_cca_control.o \
    arm/obj/cpu/cpu_load.o \
    arm/obj/ecc/ecc.o \
    arm/obj/can/can.o \
    arm/obj/$(COMMON)/v2x_cli/v2x_cli.o


ifneq ($(CRATON_NO_ARC),y)
LDFLAGS := -ldefault.arc1 -ldefault.arc2
endif # CRATON_NO_ARC


