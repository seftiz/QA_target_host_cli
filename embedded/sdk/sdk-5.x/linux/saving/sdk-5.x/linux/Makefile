# Autotalks SDK directory
ATLK_SDK_DIR := /media/sf_fw_release/dbg-secton-sdk-5.3.0-alpha2
#ATLK_SDK_DIR := /docs/ATE/fw_releases/craton2-sdk-5.3.0-alpha1

COMMON := ../common
APPS := ../apps


# Autotalks SDK library directory
ATLK_SDK_LIB_DIR := $(ATLK_SDK_DIR)/lib

# Autotalks include directory
ATLK_SDK_INCLUDE_DIR := $(ATLK_SDK_DIR)/include

# Autotalks library name
ATLK_SDK_LIB := atlkremote_linux_u

# Common C compilation flags
ATLK_CFLAGS := -std=gnu99 -Wall -Wextra -Werror

# Target application name
TARGET_APP := v2x_cli


# Target source files
TARGET_SRCS := libcli/libcli.c
TARGET_SRCS += link_layer/*.c  
TARGET_SRCS += $(APPS)/sniffer/sniffer.c
TARGET_SRCS += $(COMMON)/v2x_cli/v2x_cli.c
TARGET_SRCS += $(COMMON)/general/general.c
TARGET_SRCS += $(COMMON)/link/link.c
TARGET_SRCS += remote/remote.c

# Additional common libraries
ADDITIONAL_LIBS:= -lpthread -lcrypt -lrt
               
$(TARGET_APP):$(TARGET_SRCS)
	$(CC) $(ATLK_CFLAGS) $(TARGET_SRCS) -o $(TARGET_APP) \
	-I$(ATLK_SDK_INCLUDE_DIR) -L$(ATLK_SDK_LIB_DIR) \
	-l$(ATLK_SDK_LIB) $(ADDITIONAL_LIBS) \

all: $(TARGET_APP)

clean:
	rm -rf $(TARGET_APP)

