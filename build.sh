#!/bin/bash
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
#
set -x

if [ -z $PLATFORM_SOC ]; then
    export PLATFORM_SOC=intel
fi

if [ $PLATFORM_SOC = "intel" ]; then
	export TOOLCHAIN_DIR=$COMBINED_ROOT/sdk/toolchain/staging_dir
	export CROSS_TOOLCHAIN=$TOOLCHAIN_DIR
	export CROSS_COMPILE=$CROSS_TOOLCHAIN/bin/i686-cm-linux
	export CC=$CROSS_COMPILE-gcc
	export CXX=$CROSS_COMPILE-g++
	export OPENSOURCE_BASE=$COMBINED_ROOT/opensource
	export DFB_ROOT=$TOOLCHAIN_DIR 
	export DFB_LIB=$TOOLCHAIN_DIR/lib 
	export FUSION_PATH=$OPENSOURCE_BASE/src/FusionDale
	export SDK_FSROOT=$COMBINED_ROOT/sdk/fsroot/ramdisk
	export FSROOT=$COMBINED_ROOT/sdk/fsroot/ramdisk
	export MFR_PATH=$COMBINED_ROOT/ri/mpe_os/platforms/intel/groveland/mfrlibs
	export MFR_FPD_PATH=$COMBINED_ROOT/mfrlibs
	export GLIB_INCLUDE_PATH=$CROSS_TOOLCHAIN/include/glib-2.0/
	export GLIB_LIBRARY_PATH=$CROSS_TOOLCHAIN/lib/
	export GLIB_CONFIG_INCLUDE_PATH=$GLIB_LIBRARY_PATH/glib-2.0/
	export GLIBS='-lglib-2.0'
	export _ENABLE_WAKEUP_KEY=-D_ENABLE_WAKEUP_KEY
	export RF4CE_PATH=$COMBINED_ROOT/rf4ce/
	export USE_GREEN_PEAK_RF4CE_INTERFACE=-DUSE_GREEN_PEAK_RF4CE_INTERFACE
elif [ $PLATFORM_SOC = "broadcom" ]; then 
    rm -rf build_soc.sh
    cp soc/$PLATFORM_SOC/common/build.sh ./build_soc.sh
    sh ./build_soc.sh
fi

