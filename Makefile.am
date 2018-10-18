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
SUBDIRS = osal ccec host devmgr daemon
DIST_SUBDIRS = cfg osal ccec host devmgr daemon

nobase_includedir = ${includedir}/hdmicec
nobase_include_HEADERS = ${top_srcdir}/ccec/include/ccec/Assert.hpp \
                        ${top_srcdir}/ccec/include/ccec/Connection.hpp \
                        ${top_srcdir}/ccec/include/ccec/Driver.hpp \
                        ${top_srcdir}/ccec/include/ccec/Header.hpp \
                        ${top_srcdir}/ccec/include/ccec/MessageDecoder.hpp \
                       	${top_srcdir}/ccec/include/ccec/Messages.hpp \
                        ${top_srcdir}/ccec/include/ccec/Operands.hpp \
                        ${top_srcdir}/ccec/include/ccec/CCEC.hpp \
                        ${top_srcdir}/ccec/include/ccec/DataBlock.hpp \
                        ${top_srcdir}/ccec/include/ccec/Exception.hpp \
                        ${top_srcdir}/ccec/include/ccec/Host.hpp \
                        ${top_srcdir}/ccec/include/ccec/MessageEncoder.hpp \
                        ${top_srcdir}/ccec/include/ccec/OpCode.hpp \
                        ${top_srcdir}/ccec/include/ccec/Util.hpp \
                        ${top_srcdir}/ccec/include/ccec/CECFrame.hpp \
                        ${top_srcdir}/ccec/include/ccec/FrameListener.hpp \
                        ${top_srcdir}/ccec/include/ccec/LibCCEC.hpp \
                        ${top_srcdir}/ccec/include/ccec/MessageProcessor.hpp \
                        ${top_srcdir}/ccec/include/ccec/Operand.hpp \
                        ${top_srcdir}/ccec/include/ccec/driver/hdmi_cec_driver.h \
                        ${top_srcdir}/ccec/drivers/include/ccec/drivers/iarmbus/CecIARMBusMgr.h \
			${top_srcdir}/osal/include/osal/Condition.hpp \
                        ${top_srcdir}/osal/include/osal/EventQueue.hpp \
                        ${top_srcdir}/osal/include/osal/Mutex.hpp \
                        ${top_srcdir}/osal/include/osal/Runnable.hpp \
                        ${top_srcdir}/osal/include/osal/Thread.hpp \
                        ${top_srcdir}/osal/include/osal/ConditionVariable.hpp \
                        ${top_srcdir}/osal/include/osal/Exception.hpp \
                        ${top_srcdir}/osal/include/osal/OSAL.hpp \
                        ${top_srcdir}/osal/include/osal/Stoppable.hpp \
                        ${top_srcdir}/osal/include/osal/Util.hpp \
			${top_srcdir}/host/include/ccec/host/RDK.hpp

distcleancheck_listfiles = *-libtool
