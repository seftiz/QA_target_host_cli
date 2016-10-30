

SDK_DIR := $(HOME)/fw_release/sdk-3.3.1-rc2-pangaea4-i686-linux-gnu
COMMON := ../common
export

.DEFAULT_GOAL := all


all:
	$(MAKE) -C $(SDK_DIR)/src 
	$(MAKE) -C $(SDK_DIR)/src install

	$(MAKE) -C linux/
	$(MAKE) -C threadx/
	
linux:
	$(MAKE) -C linux/

threadx:
	$(MAKE) -C threadx/

clean:
	#rm -rf $(addprefix $(OUTPUT_DIR)/,obj bin lib img)
	$(MAKE) -C $(SDK_DIR)/src clean

	$(MAKE) -C linux/ clean
	$(MAKE) -C threadx/ clean



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



