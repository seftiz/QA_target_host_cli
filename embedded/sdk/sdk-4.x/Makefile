

export SDK_DIR := /docs/ATE/fw_releases/sdk-4.11.0-beta1-mc-i686-linux-gnu
export COMMON := ../common
export APPS := ../apps

# Set the board type 

# GM BOARD 
export BOARD := atk22016

# PANAGEA4
export BOARD := atk22016

.DEFAULT_GOAL := all

all: threadx linux
	@echo "Building all"
	$(MAKE) -C threadx/
	$(MAKE) -C linux/
	
linux:
	$(MAKE) -C linux/

threadx:
	$(MAKE) -C threadx/

tg:
	@echo "Building Auto-talks TG"

asn1c:
	@echo ' will do something '

clean:
	$(MAKE) -C threadx/ clean
	$(MAKE) -C linux/ clean
	#rm -rf $(addprefix $(OUTPUT_DIR)/,obj bin lib img)
	rm -rf $(addprefix threadx/,arm arc1 arc2)
	rm -rf ./output

	#$(MAKE) -C $(SDK_DIR)/src clean
	#$(MAKE) -C linux/ clean

# Build PySNMP MIB modules
snmp_mibs_dir := $(SDK_DIR)/mibs
pysnmp_dir := $(snmp_mibs_dir)/pysnmp

mib_modules := $(shell cat mib-modules.txt)

cmd_pysnmp = SMIPATH=$(snmp_mibs_dir) smidump -f python $1 --level=2 --keep-going | \
	libsmi2pysnmp > $(pysnmp_dir)/$1.py

pysnmp:
	mkdir -p $(pysnmp_dir)
	$(foreach m,$(mib_modules),$(call cmd_pysnmp,$m) &&) true

pysnmp-clean:
	rm -rf $(pysnmp_dir)
