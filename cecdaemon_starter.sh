#!/bin/sh
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

cecdaemonPid=`pidof CecDaemonMain`
if [ ! -z "$cecdaemonPid" ]; then
    echo "Previous running instance of CecDaemonMain with PID $cecdaemonPid is present. Ignore cecdaemon start."
    exit 0
fi

if [ ! -f /lib/rdk/getRFC.sh ]; then
    echo "getRFC.sh not available"
    exit 0
fi

exp_retry=0
while true; do
    _enableHDMICECDaemon=`tr181Set Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HDMICECDaemon.Enable 2>&1 > /dev/null`
    if [ ! -z "$_enableHDMICECDaemon" ]; then
        if [[ $_enableHDMICECDaemon == "true" ]]; then
            touch /tmp/cecdaemon-enabled
                if [ "x${INIT_SYSTEM}" = "xs6" ] ; then
                    s6-ftrig-notify /tmp/.pathfifo path-cecdaemon-enabled
                fi
            echo "RFC HDMICECDaemon.Enabled is set to true"
            break;
        else
            echo "RFC HDMICECDaemon.Enabled is set to false"
            break;
        fi
    else
        if [ "$exp_retry" -ge 30 ]; then
            echo "RFC HDMICECDaemon is not set. Hence by default, feature is disabled.";
            break;
        fi
        echo "RFC HDMICECDaemon is not set. Tring again.."
        sleep 2
    fi
    exp_retry=`expr $exp_retry + 1`
done
