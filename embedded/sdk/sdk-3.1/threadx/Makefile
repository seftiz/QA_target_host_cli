
## Copyright (C) 2013 Autotalks Ltd.

# If you'd like to build the example outside of the SDK directory then
# change the value of SDK_DIR to an absolute path of the SDK directory
# or set $SDK_DIR environment variable
#SDK_DIR ?= $(realpath ../..)

#SDK_DIR := $(HOME)/fw_release/pangaea4-sdk-3.0-alpha6-i686-linux-gnu
SDK_DIR := $(HOME)/fw_release/sdk-3.0-rc1-pangaea4-i686-linux-gnu
# SDK_DIR := $(HOME)/fw_release/sdk-3.1-rc2-pangaea4-i686-linux-gnu

#sdk-3.2-alpha1-pangaea4-i686-linux-gnu

CFLAGS := -std=c99 -pedantic -Wall -Wextra -Werror -g

include $(SDK_DIR)/rules.mk
include $(SDK_DIR)/craton/config.mk

.DEFAULT_GOAL := img/v2x-cli.img

bin/v2x-cli: \
    obj/session/session.o \
    obj/wsmp/wsmp.o \
    obj/nav_api/nav_api.o \
    obj/link/link.o \
    obj/v2x_cli/v2x_cli.o
	#obj/management/mng_api.o \
