
## Copyright (C) 2013 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory
# SDK_DIR := $(HOME)/fw_release/sdk-3.2-beta4-pangaea4-i686-linux-gnu
# COMMON := ../common

ifeq ($(SDK_DIR),)
#SDK_DIR := /media/sf_fw_release/sdk-4.11.1-rc2-sc-i686-linux-gnu/
SDK_DIR := /media/sf_ATE_proj/sdk-4.11.1-rc2-sc-i686-linux-gnu/
COMMON := ../common
endif

# SDK_DIR := ../../..
#export SDK_DIR := /media/sf_fw_release/sdk-4.11.1-alpha4-sectonm-i686-linux-gnu/
#export SDK_DIR := /media/sf_fw_release/sdk-4.11.1-rc2-sc-i686-linux-gnu/
export SDK_DIR := /media/sf_ATE_proj/sdk-4.11.1-rc2-sc-i686-linux-gnu/
# export COMMON := ../common

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/host.mk

SDK_LIB_DIR := $(SDK_DIR)/$(HOST)/lib

CFLAGS := -std=c99  -Wextra -g -D_XOPEN_SOURCE=600

CFLAGS += -I$(SDK_DIR)/include
LDFLAGS := -lsectonhost -lcrypt -L$(SDK_LIB_DIR) 

.DEFAULT_GOAL := $(HOST)/bin/v2x-cli

$(HOST)/bin/v2x-cli: \
	$(HOST)/obj/libcli/libcli.o \
	$(HOST)/obj/$(COMMON)/v2x_cli/v2x_cli.o \
	$(HOST)/obj/$(COMMON)/general/general.o \
	$(HOST)/obj/$(COMMON)/link/link.o \
	$(HOST)/obj/../threadx/ecc/ecc.o \
	$(HOST)/obj/remote/remote.o \
        $(HOST)/obj/$(COMMON)/management/mng_api.o \

clean: 
	rm -rf $(HOST)
