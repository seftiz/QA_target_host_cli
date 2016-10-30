
## Copyright (C) 2013 Autotalks Ltd.

# If you'd like to build the example outside of the SDK directory then
# change the value of SDK_DIR to an absolute path of the SDK directory
# or set $SDK_DIR environment variable
#SDK_DIR ?= $(realpath ../..)

#SDK_DIR := $(HOME)/fw_release/pangaea4-sdk-3.0-alpha6-i686-linux-gnu
#SDK_DIR := $(HOME)/fw_release/sdk-3.2-beta4-pangaea4-i686-linux-gnu
#COMMON := ../common


CFLAGS := -std=c99 -pedantic -Wall -Wextra -Werror -g

include $(SDK_DIR)/rules.mk
include $(SDK_DIR)/craton/config.mk

.DEFAULT_GOAL := img/v2x-cli.img

bin/v2x-cli: \
	obj/$(COMMON)/session/session.o \
	obj/$(COMMON)/wsmp/wsmp.o \
	obj/$(COMMON)/nav_api/nav_api.o \
	obj/$(COMMON)/management/mng_api.o \
	obj/$(COMMON)/link/link.o \
	obj/phy/phy_cca_control.o \
	obj/cpu/cpu_load.o \
	obj/$(COMMON)/v2x_cli/v2x_cli.o

