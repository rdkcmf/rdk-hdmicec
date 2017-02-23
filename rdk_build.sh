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

#######################################
#
# Build Framework standard script for
#
# HDMICEC  component

# use -e to fail on any shell issue
# -e is the requirement from Build Framework
set -e


# default PATHs - use `man readlink` for more info
# the path to combined build
export RDK_PROJECT_ROOT_PATH=${RDK_PROJECT_ROOT_PATH-`readlink -m ..`}
export COMBINED_ROOT=$RDK_PROJECT_ROOT_PATH

# path to build script (this script)
export RDK_SCRIPTS_PATH=${RDK_SCRIPTS_PATH-`readlink -m $0 | xargs dirname`}

# path to components sources and target
export RDK_SOURCE_PATH=${RDK_SOURCE_PATH-`readlink -m .`}
export RDK_TARGET_PATH=${RDK_TARGET_PATH-$RDK_SOURCE_PATH}

# fsroot and toolchain (valid for all devices)
export RDK_FSROOT_PATH=${RDK_FSROOT_PATH-`readlink -m $RDK_PROJECT_ROOT_PATH/sdk/fsroot/ramdisk`}
export RDK_TOOLCHAIN_PATH=${RDK_TOOLCHAIN_PATH-`readlink -m $RDK_PROJECT_ROOT_PATH/sdk/toolchain/staging_dir`}


# default component name
export RDK_COMPONENT_NAME=${RDK_COMPONENT_NAME-`basename $RDK_SOURCE_PATH`}

# parse arguments
INITIAL_ARGS=$@

function usage()
{
    set +x
    echo "Usage: `basename $0` [-h|--help] [-v|--verbose] [action]"
    echo "    -h    --help                  : this help"
    echo "    -v    --verbose               : verbose output"
    echo "    -p    --platform  =PLATFORM   : specify platform for HDMICEC "
    echo
    echo "Supported actions:"
    echo "      configure, clean, build (DEFAULT), rebuild, install"
}

# options may be followed by one colon to indicate they have a required argument
if ! GETOPT=$(getopt -n "build.sh" -o hvp: -l help,verbose,platform: -- "$@")
then
    usage
    exit 1
fi

eval set -- "$GETOPT"

while true; do
  case "$1" in
    -h | --help ) usage; exit 0 ;;
    -v | --verbose ) set -x ;;
    -p | --platform ) CC_PLATFORM="$2" ; shift ;;
    -- ) shift; break;;
    * ) break;;
  esac
  shift
done

ARGS=$@


# component-specific vars
export RDK_PLATFORM_SOC=${RDK_PLATFORM_SOC-broadcom}
export PLATFORM_SOC=$RDK_PLATFORM_SOC
export CC_PLATFORM=${CC_PLATFORM-$RDK_PLATFORM_SOC}
CC_PATH=$RDK_SOURCE_PATH
export FSROOT=${RDK_FSROOT_PATH}
export TOOLCHAIN_DIR=${RDK_TOOLCHAIN_PATH}
export BUILDS_DIR=$RDK_PROJECT_ROOT_PATH


# functional modules

function configure()
{
   true #use this function to perform any pre-build configuration
}

function clean()
{
    true #use this function to provide instructions to clean workspace
}

function build()
{
    CURR_DIR=`pwd`
    cd ${CC_PATH}
    export DS_PATH=$BUILDS_DIR/devicesettings
    export HDMICEC_PATH=$BUILDS_DIR/hdmicec
    export IARM_PATH=$BUILDS_DIR/iarmbus
    export FUSION_PATH=$OPENSOURCE_BASE/src/FusionDale
    export SDK_FSROOT=$COMBINED_ROOT/sdk/fsroot/ramdisk
    export FSROOT=$COMBINED_ROOT/sdk/fsroot/ramdisk
    export DFB_LIB=$TOOLCHAIN_DIR/lib
    export OPENSOURCE_BASE=$COMBINED_ROOT/opensource
    export GLIB_LIBRARY_PATH=$CROSS_TOOLCHAIN/lib/
    export GLIBS='-lglib-2.0 -lz'
	if [ $RDK_PLATFORM_SOC = "intel" ]; then
		export CC=$CROSS_COMPILE-gcc
		export CXX=$CROSS_COMPILE-g++
		export OPENSOURCE_BASE=$COMBINED_ROOT/opensource
	elif [ $RDK_PLATFORM_SOC = "broadcom" ]; then
        echo "building for ${RDK_PLATFORM_DEVICE}..."
        export WORK_DIR=$BUILDS_DIR/work${RDK_PLATFORM_DEVICE^^}
	source $BUILDS_DIR/build_scripts/setBCMenv.sh
        export MFR_PATH=$WORK_DIR/svn/sdk/mfrlib
    	export OPENSOURCE_BASE=$BUILDS_DIR/opensource
        export HDMICEC_PATH=$BUILDS_DIR/hdmicec
        export FSROOT=$COMBINED_ROOT/sdk/fsroot/ramdisk
    	CROSS_COMPILE=mipsel-linux
        export CC=$CROSS_COMPILE-gcc
    	export CXX=$CROSS_COMPILE-g++
        export GLIB_INCLUDE_PATH=$APPLIBS_TARGET_DIR/usr/local/include/glib-2.0/
    	export GLIB_LIBRARY_PATH=$APPLIBS_TARGET_DIR/usr/local/lib/
        export GLIBS='-lglib-2.0 -lintl -lz'
	fi
    make
    cd $CURR_DIR
}

function rebuild()
{
    clean
    build
}

function install()
{
    CURR_DIR=`pwd`
    
	cd ${CC_PATH}
	
        mkdir -p ${RDK_FSROOT_PATH}/usr/local/lib
	    mkdir -p ${RDK_FSROOT_PATH}/mnt/nfs/env
        mkdir -p ${RDK_FSROOT_PATH}/usr/local/include/ccec
        mkdir -p ${RDK_FSROOT_PATH}/usr/local/include/osal
        mkdir -p ${RDK_FSROOT_PATH}/usr/local/include/host
	
	if [ $RDK_PLATFORM_SOC = "intel" ]; then
		export CC=$CROSS_COMPILE-gcc
		export CXX=$CROSS_COMPILE-g++
		export OPENSOURCE_BASE=$COMBINED_ROOT/opensource
	elif [ $RDK_PLATFORM_SOC = "broadcom" ]; then
		cp -v ${CC_PATH}/install/bin/CecDaemonMain ${RDK_FSROOT_PATH}mnt/nfs/env
		cp -v ${CC_PATH}/install/lib/libRCEC*.so ${RDK_FSROOT_PATH}/usr/local/lib
		cp -r ${CC_PATH}/ccec/include/ ${RDK_FSROOT_PATH}/usr/include/ccec/
		cp -r ${CC_PATH}/osal/include/ ${RDK_FSROOT_PATH}/usr/include/osal/
		cp -r ${CC_PATH}/host/include/ ${RDK_FSROOT_PATH}/usr/include/host/	
	fi
    
	cd $CURR_DIR
}


# run the logic

#these args are what left untouched after parse_args
HIT=false

for i in "$ARGS"; do
    case $i in
        configure)  HIT=true; configure ;;
        clean)      HIT=true; clean ;;
        build)      HIT=true; build ;;
        rebuild)    HIT=true; rebuild ;;
        install)    HIT=true; install ;;
        *)
            #skip unknown
        ;;
    esac
done

# if not HIT do build by default
if ! $HIT; then
  build
fi
