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
# common flasg     
IARM_BUS_LIB_FLAGS:=-lfusion -ldirect -lIARMBus -lglib
# List of Executable
exe_lib            := src/
exe_drivers        := drivers/

executable := $(exe_lib) $(exe_drivers)  

.PHONY: clean all $(executable) 

all: clean $(executable) 

$(executable):
	$(MAKE) -C $@
