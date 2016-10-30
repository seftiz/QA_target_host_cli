
## Copyright (C) 2013 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory

#SDK_DIR := $(HOME)/fw_release/sdk-3.2-beta4-pangaea4-i686-linux-gnu
#COMMON := ../common

flthdr = $(CC:%-gcc=%-flthdr)

CFLAGS := -std=c99 -pedantic -Wall -Wextra -Werror -g -D__LINUX__ -D_XOPEN_SOURCE=600
CFLAGS += -I$(SDK_DIR)/include -L$(SDK_DIR)/lib
CFLAGS += -lpthread -lcrypt -lvtpc -lnavc -latlk -lrt 

# -lv2x -latlk -lpthread -lcrypt -lnav -lgps -lm

CC := gcc
AR := ar


include $(SDK_DIR)/rules.mk

.DEFAULT_GOAL := bin/v2x-cli

bin/v2x-cli: \
	obj/libcli/libcli.o \
    obj/$(COMMON)/session/session.o \
    obj/$(COMMON)/wsmp/wsmp.o \
    obj/$(COMMON)/nav_api/nav_api.o \
    obj/$(COMMON)/link/link.o \
    obj/$(COMMON)/v2x_cli/v2x_cli.o
