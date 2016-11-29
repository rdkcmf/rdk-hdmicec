/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/


/**
* @defgroup hdmicec
* @{
* @defgroup ccec
* @{
**/


#include <stdio.h>
#include "ccec/drivers/hdmi_cec_driver.h"
#include "ccec/drivers/iarmbus/CecIARMBusMgr.h"
#include "libIBus.h"

int main(int argc, char *argv[])
{
    int handle = 0;
    IARM_Bus_Init(IARM_BUS_CECMGR_NAME);
    IARM_Bus_Connect();
    HdmiCecOpen(&handle);
    HdmiCecClose(handle);
    IARM_Bus_Disconnect();
    IARM_Bus_Term();
}




/** @} */
/** @} */
