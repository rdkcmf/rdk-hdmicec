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
AM_CFLAGS = -pthread -Wall -I=/usr/include/rdk/iarmbus -I${top_srcdir}/ccec/drivers/include -I${top_srcdir}/ccec/include
lib_LTLIBRARIES = libRCECIARMBusHal.la
libRCECIARMBusHal_la_SOURCES = hdmi_cec_driver.c
libRCECIARMBusHal_la_LDFLAGS =  -lIARMBus -lpthread
