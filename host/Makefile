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
CFLAGS      := -g -fPIC -D_REENTRANT -Wall
LIBNAME     := RCECHost
LIBNAMEFULL := lib$(LIBNAME).so

CEC_ROOT    :=..

OBJS:= src/RDK.o \

INCLUDE = -I.\
	-I../include\
	-I$(IARM_PATH)/core/include\
	-I$(DS_PATH)/ds/include\
	-I$(DS_PATH)/rpc/include\
	-I$(DS_PATH)/hal/include\
	-I${BUILDS_DIR}/iarmmgrs/generic/hal/include\
	-I$(CEC_ROOT)/ccec/include\
	-I$(CEC_ROOT)/host/include\

CFLAGS+= $(INCLUDE)

LDFLAGS+= -L$(OPENSOURCE_BASE)/lib
LDFLAGS+=-L$(GLIB_LIBRARY_PATH)/
LDFLAGS+=$(GLIBS)
LDFLAGS += -L. -lIARMBus -lpthread -lds

all: clean library
	@echo "Build Finished...."

library: $(OBJS)
	@echo "Building $(LIBNAMEFULL) ...."
	mkdir -p install/lib/
	$(CC) $(OBJS) $(CFLAGS) -shared -o install/lib/$(LIBNAMEFULL)

%.o: %.cpp
	@echo "Building $@ ...."
	$(CC) -c $<  $(CFLAGS) -o $@

clean:
	@echo "Cleaning the directory..."
	@$(RM) *.o install
