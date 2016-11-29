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
TEST_SRC_ROOT:=../
INCLUDE     := -I$(TEST_SRC_ROOT)/include/\

LDFLAGS     := -L$(IARM_PATH)/install -L$(TEST_SRC_ROOT)/iarmbus/install/lib
LDFLAGS     += -L$(FUSION_PATH) -L$(DFB_LIB) -L$(OPENSOURCE_BASE)/lib -L$(GLIB_LIBRARY_PATH) $(GLIBS) -lIARMBus -lRCECIARMBusHal -lpthread -ldirect -lfusion  


ifeq ($(USE_DBUS),y)
LDFLAGS += -ldbus-1
INCLUDE += -D_USE_DBUS -I$(OPENSOURCE_BASE)/include/dbus-1.0 -I$(OPENSOURCE_BASE)/lib/dbus-1.0/include
INCLUDE += -I$(IARM_PATH)/core/include
else
INCLUDE += -I$(FUSION_PATH)/src -I$(FUSION_PATH)/include
LDFLAGS += -lfusiondale
endif

CFLAGS      +=   -g -fPIC -D_REENTRANT -Wall $(INCLUDE)

.PHONY: clean all $(BINARIES)

#BINARIES    := $(patsubst %.cpp,%,$(wildcard *.cpp))
BINARIES    := $(patsubst %.c,%,$(wildcard *.c))
UNINSTALL   := $(patsubst %,$(PWD)/install/bin/%, $(BINARIES))

all: $(BINARIES)
	@echo "Build $(BINARIES) >>> Finished...."

$(BINARIES):
	@echo "Building $@ ....############################################################################"
	mkdir -p ./install/bin            
	$(CXX) $(CFLAGS) $@.c -o ./install/bin/$@ $(LDFLAGS)
	@echo "Build Finished...."

clean:
	@echo "Cleaning the directory..."
	@$(RM) *.o ./install/bin/
