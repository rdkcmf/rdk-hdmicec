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
SUBDIRS =
AM_CXXFLAGS = -pthread -Wall -I${top_srcdir}/osal/include -I${top_srcdir}/drivers/include -I${top_srcdir}/ccec/include

lib_LTLIBRARIES = libRCEC.la

libRCEC_la_SOURCES = CECFrame.cpp \
                     Util.cpp \
                     DriverImpl.cpp \
                     LibCCEC.cpp \
                     Bus.cpp \
                     OpCode.cpp \
                     Connection.cpp \
                     Driver.cpp \
                     MessageDecoder.cpp

libRCEC_la_LDFLAGS = -lpthread
libRCEC_la_LIBADD = -lRCECOSHal -L${top_builddir}/osal/src/.libs
