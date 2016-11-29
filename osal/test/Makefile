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

LDFLAGS     := -L$(TEST_SRC_ROOT)/src/install/lib -lRCECOSHal -lpthread

CFLAGS      +=   -g -fPIC -D_REENTRANT -Wall $(INCLUDE)

.PHONY: $(OUTPUT)

BINARIES    := $(patsubst %.cpp,%,$(wildcard *.cpp))
UNINSTALL   := $(patsubst %,$(PWD)/install/bin/%, $(BINARIES))

all: clean $(BINARIES)  
	@echo "Build Finished...."

#frontPanelTest:
$(BINARIES):
	@echo "Building $@ ...."
	mkdir -p ./install/bin            
	$(CXX) $(CFLAGS) $@.cpp -o ./install/bin/$@ $(LDFLAGS)

clean:
	@echo "Cleaning the directory..."
	@$(RM) $(BINARIES) install



