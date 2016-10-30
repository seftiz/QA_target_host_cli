# Generation of the application  from :
#    - the application sources,
#    - the HazardNotificationImpl.cpp to compete
#    - the Marben libraries
#    - The Autotalks SDK
#    - The GCC-ARM
# -----------------------------------------------
EXE = appliPangaea
RAW = appliPangaeaRAW
# IMG = appliPangaeaIMG
IMG = v2x-cli.img

MARBEN_DIR := $(HOME)/fw_release/MarbenV2X-AutotalksPANGAEA4-20140617


# Marben and Autotalks dependencies
# MARBEN_INC = $(addprefix -I, $(dir $(wildcard $(MARBEN_DIR)/Marben/src/*/*/*/*/*.hpp)) $(dir $(wildcard $(MARBEN_DIR)/Marben/src/*/*/*/*.hpp)) $(dir $(wildcard $(MARBEN_DIR)/Marben/src/*/*/*.hpp)) $(dir $(wildcard $(MARBEN_DIR)/Marben/src/*/*/*/incl/*.h)) $(dir $(wildcard $(MARBEN_DIR)/Marben/target/Autotalks/PANGAEA/*/*.hpp)) )
MARBEN_INC = $(addprefix -I, $(dir $(wildcard $(MARBEN_DIR)/Marben/src/*/*/*/*/*.hpp)) $(dir $(wildcard $(MARBEN_DIR)/Marben/src/*/*/*/*.hpp)) $(dir $(wildcard $(MARBEN_DIR)/Marben/src/*/*/*.hpp)) $(dir $(wildcard $(MARBEN_DIR)/Marben/src/*/*/*/incl/*.h)) $(dir $(wildcard $(MARBEN_DIR)/Marben/target/*.hpp)) )

ATLK_SDK = $(MARBEN_DIR)/Autotalks/arm-sdk
# ATLK_LIB = $(ATLK_SDK)/craton/lib/release
ATLK_LIB = $(ATLK_SDK)/release/arm/lib
ATLK_INC = $(addprefix -I, $(sort $(dir $(wildcard $(ATLK_SDK)/include/*.h)) $(dir $(wildcard $(ATLK_SDK)/craton/include/*.h)) ) )

# GCC dependencies
CROSS_COMPILE = /tools/gcc/arm/gcc-arm-none-eabi-4_7-2012q4/bin/arm-none-eabi-
CXX=$(CROSS_COMPILE)g++
CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar

# C++ compilation and linking flags
# For 13/06/2014 version
#CXXFLAGS=-include $(MARBEN_DIR)/Marben/target/Autotalks/PANGAEA/Compatibility/Compatibility.hpp -std=c++11 -Wall -Wextra -pedantic -Wold-style-cast -Os -pipe -I$(ATLK_SDK)/craton/include -I$(ATLK_SDK)/include -mthumb -march=armv7-r -mfloat-abi=hard -mfpu=vfpv3-d16 
#LDFLAGS=-L$(ATLK_LIB) -D__THREADX__=1 -D__CRATON__=1 -mthumb -march=armv7-r -mfloat-abi=hard -mfpu=vfpv3-d16 -Os 
#ATLKLIBS=-lsnmp -lsystem -lcli2 -lregex -lvcad -ljsmn -lgeo -lbsm -lwsmp -lvtpd -lecc-srv -lv2x-srv -lwlan -lwlan-rf -lwlan-if -lhsm-fake -lecc -lrng -lsha -lsl869 -lnav-gpsd-wrapper -lnav-imq-client -lnav-service -lnav-mock-wrapper -lnav-udp-client -lcan-service -lgpsd -lgps -lnx-bsd -lstmeth -lspill -lnetx -lsyslog -lsafeFAT -lsyslog-tcp -lsyslog-udp -lsyslog-rotate -lsyslog-console -lsyslog-imq-server -lsyslog-imq-client -lspi -ldma -lgpio -li2c -limq -lcan -latlk -nostartfiles -T craton.ld
#MARBENLIBS=-lMarbenV2XInit -lMarbenV2

CXXFLAGS=-include $(MARBEN_DIR)/Marben/target/Compatibility.hpp -std=c++11 -Wall -Wextra -pedantic -Wold-style-cast -Os -pipe -I$(ATLK_SDK)/release/arm/include -I$(ATLK_SDK)/include -I$(ATLK_SDK)/include/craton -D__THREADX__=1 -D__CRATON__=1 -D__LINUX_ERRNO_EXTENSIONS__ -D__CRATON_NO_ARC=1 -mthumb -march=armv7-r -mfloat-abi=hard -mfpu=vfpv3-d16 
LDFLAGS=-L$(ATLK_LIB) -D__THREADX__=1 -D__CRATON__=1 -D__LINUX_ERRNO_EXTENSIONS__ -D__CRATON_NO_ARC=1 -mthumb -march=armv7-r -mfloat-abi=hard -mfpu=vfpv3-d16 -Os 
ATLKLIBS=-lsnmp -lsystem -lcli2 -lregex -lvcad -ljsmn -lgeo -lbsm -lwsmp -lvtpd -lv2x-srv -lv2x-imq -lwlan -lwlan-rf -lwlan-if -lhsm-emulator -lcrypto -ltomcrypt -ltommath -lecc-srv -lecc -lrng -lsha -lsl869 -lnav-gpsd-wrapper -lnav-imq-client -lnav-service -lnav-mock-wrapper -lnav-udp-client -lcan-service -lgpsd -lgps -lnx-wlan -lnx-bsd -lstmeth -lspill -lnetx -lsyslog -lsyslog-tcp -lsyslog-udp -lsyslog-rotate -lsyslog-console -lsyslog-imq-server -lsyslog-imq-client -lcan -latlk -lgpio -li2c -limq -Wl,--start-group -lcraton -lthreadx -lspi -ldma -lsafeFAT -Wl,--end-group -lstdc++ -lm -nostartfiles -T craton-arm.ld
MARBENLIBS=-lMarbenV2XInit -lMarbenV2X


# List of the sources 
CPPS:=$(wildcard ./*.cpp) 
OBJS:=$(patsubst %.cpp,%.o,$(notdir $(CPPS)))

# Cdes to generate the raw binary and the image files
OBJCOPY := $(CROSS_COMPILE)objcopy
#MKIMAGE := $(ATLK_SDK)/bin/u-boot-mkimage
#MKIMAGE_FLAGS := $(shell cat $(ATLK_SDK)/craton/u-boot-mkimage.flags)
MKIMAGE := $(ATLK_SDK)/build/u-boot-mkimage
MKIMAGE_FLAGS := $(shell cat $(ATLK_SDK)/build/u-boot-mkimage.flags)


DEPFILE = Make.depend
DEPDIR = .depend
MKDIR = mkdir -p

.PHONY: clean 

$(IMG): $(RAW)
	@echo Image File Generation $(IMG)
	$(MKIMAGE) $(MKIMAGE_FLAGS) -n $(EXE) -d $< $@

$(RAW): $(EXE)
	@echo Raw File Generation $(RAW)
	@$(OBJCOPY) -O binary $< $@

$(EXE): $(OBJS) 
	@echo Linking $(EXE)
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -Wl,-Map=$(EXE).map -o $(EXE) $(ATLK_LIB)/sys-init.o $(OBJS) -L$(MARBEN_DIR)/Marben/lib $(MARBENLIBS)  $(ATLKLIBS)

%.o:
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) $(ATLK_INC) $(MARBEN_INC) -o $@ -c $<


clean:
	@echo Cleaning local directory : `pwd`
	@rm -rf $(OBJS) $(EXE) $(DEPDIR) *.map $(RAW) $(IMG)

depend: $(DEPDIR)/$(DEPFILE)
	@touch $(DEPDIR)/$(DEPFILE)

$(DEPDIR)/$(DEPFILE):
	@echo Makefile - building dependencies in: $@
	@$(MKDIR) $(DEPDIR)
	@$(CXX) -E -MM $(CXXFLAGS) $(ATLK_INC) $(MARBEN_INC) $(CPPS) >> $@

ifeq (,$(findstring clean,$(MAKECMDGOALS)))
-include $(DEPDIR)/$(DEPFILE)
endif
