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
# List of Libraries
install_dir := ./install/bin
install_lib_dir := ./install/lib

# List of Executable
exe_osal            := osal/
exe_ccec            := ccec/
exe_soc             := soc/$(PLATFORM_SOC)/common
exe_daemon          := daemon
exe_host            := host
exe_devmgr          := devmgr
exe_tests           := tests

executable := $(exe_soc) $(exe_osal) $(exe_ccec) $(exe_daemon) $(exe_host) $(exe_devmgr) $(exe_tests)

.PHONY: clean all $(executable) install

all: clean $(executable) install 

$(executable):
	$(MAKE) -C $@

install:
	echo "Creating directory.."
	mkdir -p $(install_dir)
	mkdir -p $(install_lib_dir)
	echo "Copying files now.."	
	cp $(exe_osal)/src/install/lib/*.so $(install_lib_dir)
	cp $(exe_ccec)/src/install/lib/*.so $(install_lib_dir)
	cp $(exe_ccec)/drivers/iarmbus/install/lib/*.so $(install_lib_dir)
	cp soc/$(PLATFORM_SOC)/common/install/lib/*.so $(install_lib_dir)
	cp $(exe_ccec)/drivers/test/install/bin/* $(install_dir)
	cp $(exe_daemon)/install/bin/* $(install_dir)
	cp $(exe_devmgr)/install/bin/* $(install_dir)
	#cp $(exe_host)/install/lib/* $(install_lib_dir)

ifneq ($(PLATFORM_SOC),entropic)
endif
ifeq ($(PLATFORM_SOC),broadcom)
endif
ifeq ($(PLATFORM_SOC),intel)
endif
clean:
	rm -rf build_soc.sh
	rm -rf install
	rm -rf $(exe_osal)/src/install
	rm -rf $(exe_osal)/test/install
	rm -rf $(exe_ccec)/lib/install
	rm -rf $(exe_ccec)/drivers/iarmbus/install
	rm -rf soc/$(PLATFORM_SOC)/common/install
	rm -rf $(exe_ccec)/drivers/test/install
	rm -rf $(exe_daemon)/install
	rm -rf $(exe_devmgr)/install
	rm -rf $(exe_host)/install
	rm -rf $(exe_tests)/install
#delete all temp objects
	rm -rf $(exe_osal)/src/*.o
	rm -rf $(exe_ccec)/lib/*.o
	rm -rf $(exe_ccec)/drivers/iarmbus/*.o
	rm -rf soc/$(PLATFORM_SOC)/common/*.o
	rm -rf $(exe_ccec)/drivers/test/*.o
	rm -rf $(exe_tests)/*.o
