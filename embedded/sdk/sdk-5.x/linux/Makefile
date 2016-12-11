
## Copyright (C) 2013 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory
# SDK_DIR := $(HOME)/fw_release/sdk-3.2-beta4-pangaea4-i686-linux-gnu
# COMMON := ../common

ifeq ($(SDK_DIR),)
SDK_DIR := /home/zohars/craton-host/host
COMMON := ../common
APPS := ../apps
endif

# SDK_DIR := ../../..
export SDK_DIR := /home/zohars/craton-host/host
# export COMMON := ../common

#TARGET := remote
#PLATFORM := linux_u
include build/rules.mk
#include $(SDK_DIR)/build/common.mk
#include $(SDK_DIR)/build/rules.mk

include  build/host.mk


SDK_LIB_DIR := $(SDK_DIR)/sdk/remote_linux_u/lib

CFLAGS := -std=c99  -Wextra -g -D_XOPEN_SOURCE=600 -D_BSD_SOURCE

CFLAGS += -I$(SDK_DIR)/include
CFLAGS += -I$(SDK_DIR)/src/core/include
LDFLAGS := -latlkremote_linux_u -lcrypt -L$(SDK_LIB_DIR) 

.DEFAULT_GOAL := $(HOST)/bin/v2x-cli

$(HOST)/bin/v2x-cli: \
	$(HOST)/obj/libcli/libcli.o \
	$(HOST)/obj/$(APPS)/sniffer/sniffer.o \
	$(HOST)/obj/$(COMMON)/v2x_cli/v2x_cli.o \
	$(HOST)/obj/$(COMMON)/general/general.o \
	$(HOST)/obj/$(COMMON)/link/link.o \
	$(HOST)/obj/remote/remote.o

clean: 
	rm -rf $(HOST)
