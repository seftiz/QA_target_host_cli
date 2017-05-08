## Copyright (C) 2013-2015 Autotalks Ltd.

-include $(SDK_DIR)/build/config.mk
-include $(SDK_DIR)/build/paths.mk

ifeq ($(SRC_DIR),)
SRC_DIR := .
endif

ifeq ($(BUILD_DIR),)
BUILD_DIR := .
endif

try_include = -include $1

define run_cmd
@echo "# $(notdir $(if $(DISP.$1),$(DISP.$1),$(word 1,$(CMD.$1)))) => $@"
mkdir -p $(@D)
$(CMD.$1)
endef

.PHONY: clean

# Define common build rules for environment $1
define build_rules
$(BUILD_DIR)/$1/obj/%.o: $(SRC_DIR)/%.S
	$$(call run_cmd,ASM.$1)

$(BUILD_DIR)/$1/obj/%.o: $(SRC_DIR)/%.c
	$$(call run_cmd,CC.$1)

$(BUILD_DIR)/$1/obj/%.o: $(SRC_DIR)/%.cc
	$$(call run_cmd,CXX.$1)

$(BUILD_DIR)/$1/obj/%.o: $(SRC_DIR)/%.cpp
	$$(call run_cmd,CXX.$1)

$(BUILD_DIR)/$1/obj/%.o: $(SRC_DIR)/%.cxx
	$$(call run_cmd,CXX.$1)

$(BUILD_DIR)/$1/bin/%:
	$$(call run_cmd,LINK.$1)

$(BUILD_DIR)/$1/lib/%.o:
	$$(call run_cmd,ILINK.$1)

$(BUILD_DIR)/$1/lib/lib%.a:
	$$(call run_cmd,ARCHIVE.$1)

$(BUILD_DIR)/$1/img/%.bin: $(BUILD_DIR)/$1/bin/%
	$$(call run_cmd,EXE_TO_BIN.$1)

# Add build environment cleanup to 'clean' goal
.PHONY: clean.$1

clean.$1:
	rm -rf $(addprefix $(BUILD_DIR)/$1/,obj bin lib img)

clean: clean.$1

# Include any dependency files that exist
$(if $(realpath $(BUILD_DIR)/$1/obj),\
	$(eval $(call try_include,\
		$(shell find $(BUILD_DIR)/$1/obj -name '*.d'))))
endef # build_rules

# C/C++ compilation flags for GNU toolchain
build_cflags.gnu = -pipe -MD -MP -I$(SDK_DIR)/include $$(BUILTIN_CFLAGS.$1) \
	$$(CFLAGS) $$(CFLAGS.$1) $$(CFLAGS.$$<) $$(CFLAGS.$$@)

# Define build commands using GNU toolchain for environment $1
define build_commands.gnu
CMD.ASM.$1 = $$(TOOLSET.$1)gcc -c -o $$@ $$< $(build_cflags.gnu)

CMD.CC.$1 = $$(CCACHE) $$(TOOLSET.$1)gcc -c -o $$@ $$< \
	$$(CCFLAGS) $(build_cflags.gnu)

CMD.CXX.$1 = $$(CCACHE) $$(TOOLSET.$1)g++ -c -o $$@ $$< \
	$$(CXXFLAGS) $(build_cflags.gnu)

CMD.LINK.$1 = $$(TOOLSET.$1)gcc -o $$@ $$(filter %.o %.a,$$^) \
	$$(LDFLAGS.$$@) $$(LDFLAGS.$1) $$(LDFLAGS) $$(BUILTIN_LDFLAGS.$1)
DISP.LINK.$1 = $$(TOOLSET.$1)ld

CMD.ILINK.$1 = $$(TOOLSET.$1)ld -i -o $$@ $$(filter %.o,$$^)

CMD.ARCHIVE.$1 = rm -f $$@ ; $$(TOOLSET.$1)ar rcs $$@ $$(filter %.o,$$^)
DISP.ARCHIVE.$1 = $$(TOOLSET.$1)ar

CMD.EXE_TO_BIN.$1 = $$(TOOLSET.$1)objcopy -O binary $$< $$@
endef # build_commands.gnu

# Copy file command
CMD.COPY = cp -f $< $@

# Shorthand for copy recipe
COPY = $(call run_cmd,COPY)
