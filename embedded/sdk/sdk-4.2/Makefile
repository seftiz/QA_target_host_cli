


# export SDK_DIR := $(HOME)/fw_release/sdk-4.2-rc20-pangaea4-i686-linux-gnu
export SDK_DIR := /docs/ATE/fw_releases/sdk-4.2.3-pangaea4-i686-linux-gnu
export COMMON := ../common
.DEFAULT_GOAL := all

ifdef NEW
	TYPE := _$(NEW)
	MK = _new
endif

all:
	$(MAKE) -C threadx -f Makefile$(MK)$(TYPE)
linux:
	$(MAKE) -C linux/

threadx:
	$(MAKE) -C threadx -f Makefile$(MK)$(TYPE)

tg:
	@echo "Building Auto-talks TG"

asn1c:
	@echo ' will do something '

clean:
	#rm -rf $(addprefix $(OUTPUT_DIR)/,obj bin lib img)
	$(MAKE) -C threadx -f Makefile$(MK)$(TYPE) clean
	$(MAKE) -C $(SDK_DIR)/src clean
	$(MAKE) -C linux/ clean

# Build PySNMP MIB modules
snmp_mibs_dir := $(SDK_DIR)/mibs
pysnmp_dir := $(snmp_mibs_dir)/pysnmp

snmp_mib_files := $(wildcard $(snmp_mibs_dir)/*.mib)
pysnmp_files := $(snmp_mib_files:$(snmp_mibs_dir)/%.mib=$(pysnmp_dir)/%.py)

snmp_extern_mibs := IF-MIB IANAifType-MIB
pysnmp_files += $(snmp_extern_mibs:%=$(pysnmp_dir)/%.py)

cmd_pysnmp = SMIPATH=$(CONFIG_MIBS_DIR):$(snmp_mibs_dir) \
	smidump -f python $< | libsmi2pysnmp > $@
$(pysnmp_dir)/%.py: $(snmp_mibs_dir)/%.mib | $(pysnmp_dir)
	$(cmd_pysnmp)
$(pysnmp_dir)/%.py: $(CONFIG_MIBS_DIR)/%.mib | $(pysnmp_dir)
	$(cmd_pysnmp)

$(pysnmp_dir): 
	mkdir -p $@

pysnmp: $(pysnmp_files)	



