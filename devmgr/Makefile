##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
RM          := rm -rf 
CEC_ROOT    :=..
INCLUDE     := -I$(CEC_ROOT)/ccec/include/\
	-I$(CEC_ROOT)/ccec/drivers/include \
	-I$(CEC_ROOT)/osal/include \
	-I$(IARM_PATH)/core/include \
	-I$(DS_PATH)/hal/include	\
	-I$(DS_PATH)/ds/include	

CFLAGS +=-DCEC_HOST_PLUGIN='"libRCECHost.so"'

LDFLAGS     := -L$(CEC_ROOT)/ccec/drivers/iarmbus/install/lib -L$(CEC_ROOT)/osal/src/install/lib -L$(CEC_ROOT)/soc/$(PLATFORM_SOC)/common/install/lib -L$(CEC_ROOT)/ccec/src/install/lib
LDFLAGS     += -L$(IARM_PATH)/install -L$(CEC_ROOT)/ccec/iarmbus 
LDFLAGS      += -L$(FUSION_PATH) -L$(DFB_LIB) -L$(OPENSOURCE_BASE)/lib -L$(GLIB_LIBRARY_PATH) $(GLIBS) -lIARMBus -lRCECIARMBusHal -lRCECOSHal -lRCEC -lpthread -ldirect -lfusion 
LDFLAGS     += -lnexus
LDFLAGS     += -L$(DS_PATH)/install/lib -lds -ldshalcli

ifeq ($(PLATFORM_SOC),broadcom)
    LDFLAGS+=-L$(COMBINED_ROOT)/sdk/fsroot/ramdisk/usr/local/lib -lnexus
    LDFLAGS += -lnxclient 
ifeq ($(RDK_USE_NXCLIENT),y)
    LDFLAGS += -lnxclient 
endif
endif

ifeq ($(USE_DBUS),y)
LDFLAGS += -ldbus-1
INCLUDE += -D_USE_DBUS -I$(OPENSOURCE_BASE)/include/dbus-1.0 -I$(OPENSOURCE_BASE)/lib/dbus-1.0/include
else
LDFLAGS += -lfusiondale
endif


LDFLAGS     += -lnexus
CFLAGS      +=   -g -fPIC -D_REENTRANT -Wall $(INCLUDE)

LDFLAGS += -o install/bin/CECDevMgr

.PHONY: $(OUTPUT)
BINARIES := CECDevMgr
_BINARIES    := $(patsubst %.cpp,%,$(wildcard *.cpp))
UNINSTALL   := $(patsubst %,$(PWD)/install/bin/%, $(BINARIES))

all: clean $(BINARIES)  
	@echo "Build Finished...."

$(BINARIES):
	@echo "Building $@ ...."
	mkdir -p ./install/bin            
	$(CXX) $(CFLAGS) src/DeviceManager.cpp src/Host.cpp -o ./install/bin/$@ $(LDFLAGS)

clean:
	@echo "Cleaning the directory..."
	@$(RM) $(BINARIES) install



