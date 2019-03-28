#!/bin/sh
# ============================================================================
# RDK MANAGEMENT, LLC CONFIDENTIAL AND PROPRIETARY
# ============================================================================
# This file (and its contents) are the intellectual property of RDK Management, LLC.
# It may not be used, copied, distributed or otherwise  disclosed in whole or in
# part without the express written permission of RDK Management, LLC.
# ============================================================================
# Copyright (c) 2016 RDK Management, LLC. All rights reserved.
# ============================================================================

cecdaemonPid=`pidof CecDaemonMain`
if [ ! -z "$cecdaemonPid" ]; then
    echo "Previous running instance of CecDaemonMain with PID $cecdaemonPid is present. Ignore cecdaemon start."
    exit 0
fi

if [ ! -f /lib/rdk/getRFC.sh ]; then
    echo "getRFC.sh not available"
    exit 0
fi

_enableHDMICECDaemon=`tr181Set Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HDMICECDaemon.Enable 2>&1 > /dev/null`
if [ ! -z "$_enableHDMICECDaemon" ]; then
    if [[ $_enableHDMICECDaemon == "true" ]]; then
        touch /tmp/cecdaemon-enabled
        echo "RFC HDMICECDaemon.Enabled is set to true"
    else
        echo "RFC HDMICECDaemon.Enabled is set to false"
    fi
else
    echo "RFC HDMICECDaemon is not set. Hence by default, feature is disabled"
fi
