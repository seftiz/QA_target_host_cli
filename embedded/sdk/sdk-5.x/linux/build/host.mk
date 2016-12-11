# Copyright (C) 2013-2016 Autotalks Ltd.

# Make include file for SECTON host software builds.
# Currently supporting only GNU family toolsets (for Linux or QNX).

ifeq ($(TOOLSET),)
$(info TOOLSET not set, using native GNU toolset)
else
$(info Using TOOLSET=$(TOOLSET))
endif

GCC := $(TOOLSET)gcc
HOST := $(shell $(GCC) -dumpmachine 2>/dev/null)
ifeq ($(HOST),)
ifeq ($(shell which $(GCC)),)
$(error $(GCC) not found, not executable, or not in $$PATH)
else
$(error $(GCC) has no support for -dumpmachine flag)
endif
endif

$(eval $(call build_rules,$(HOST)))
$(eval $(call build_commands.gnu,$(HOST)))
TOOLSET.$(HOST) := $(TOOLSET)

ifneq ($(shell echo $(HOST) | grep -- -linux),)
# Necessary Linux libraries
BUILTIN_LDFLAGS.$(HOST) := -lpthread -lrt
else ifneq ($(shell echo $(HOST) | grep -- -qnx),)
# Necessary QNX libraries
BUILTIN_LDFLAGS.$(HOST) := -lsocket
endif
