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
AM_CXXFLAGS = -pthread -Wall -D_USE_DBUS -I../include \
	      -I$(top_srcdir)/daemon/include \
              -I${top_srcdir}/osal/include \
              -I${top_srcdir}/host/include \
              -I${top_srcdir}/ccec/drivers/include -I${top_srcdir}/ccec/include \
              -I${top_srcdir}/ccec/drivers/include/ccec/drivers/iarmbus/CecIARMBusMgr.h \
              -I=/usr/include/rdk/iarmbus -I=/usr/include/rdk/ds -I=/usr/include/rdk/ds-hal

bin_PROGRAMS = BasicTest CECCmd CECMonitor CECCmdTest

BasicTest_SOURCES = BasicTest.cpp
BasicTest_LDADD = -lIARMBus -lds -ldshalcli -ldbus-1 \
                      ${top_builddir}/ccec/src/libRCEC.la \
                      ${top_builddir}/osal/src/libRCECOSHal.la \
                      ${top_builddir}/ccec/drivers/iarmbus/libRCECIARMBusHal.la

CECCmd_SOURCES = CECCmd.cpp
CECCmd_LDADD = -lIARMBus -lds -ldshalcli -ldbus-1 \
                      ${top_builddir}/ccec/src/libRCEC.la \
                      ${top_builddir}/osal/src/libRCECOSHal.la \
                      ${top_builddir}/ccec/drivers/iarmbus/libRCECIARMBusHal.la

CECMonitor_SOURCES = CECMonitor.cpp
CECMonitor_LDADD = ${top_builddir}/ccec/src/libRCEC.la \
                   ${top_builddir}/osal/src/libRCECOSHal.la \
                   ${top_builddir}/ccec/drivers/iarmbus/libRCECIARMBusHal.la

CECCmdTest_SOURCES = CECCmdTest.cpp
CECCmdTest_LDADD = -lIARMBus -lds -ldshalcli -ldbus-1 \
                      ${top_builddir}/ccec/src/libRCEC.la \
                      ${top_builddir}/osal/src/libRCECOSHal.la \
                      ${top_builddir}/ccec/drivers/iarmbus/libRCECIARMBusHal.la

