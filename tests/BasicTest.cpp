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
* @defgroup tests
* @{
**/


#include <stdio.h>
#include <unistd.h>
#include "libIBus.h"

#include "ccec/Messages.hpp"
#include "ccec/Assert.hpp"
#include "ccec/Connection.hpp"
#include "ccec/host/RDK.hpp"
#include "ccec/MessageEncoder.hpp"
#include "ccec/LibCCEC.hpp"


int main(int argc, char *argv[])
{

    IARM_Bus_Init("CECClient");
    IARM_Bus_Connect();

    LibCCEC::getInstance().init();
    Connection conn(LogicalAddress::UNREGISTERED, false);;
    conn.open();

    try{
        int logicalAddress;
        logicalAddress = LibCCEC::getInstance().getLogicalAddress(3);
        printf("Got logical Address here logicalAddress : %d !!\n", logicalAddress);
    }
    catch (const std::exception e)  {
        printf("getLogicalAddress caught exception %s \r\n",e.what());
    }

    IARM_Bus_CECHost_EnableDevMgr_Param_t param1;
    IARM_Result_t ret = IARM_RESULT_SUCCESS;
    IARM_Bus_CECHost_GetDeviceStatus_Param_t param;
    PhysicalAddress phy(1, 0, 0, 0);

    unsigned int option,i,j = 20;
    printf("\n Enter test option : \n 1 - Connected Device Test \n 2 - Logical Address Test \n 3 - Default test\n");
    scanf("%d", &option);

    switch(option)
    {

        case 1:
            param1.status = true;
            IARM_Bus_Call(IARM_BUS_CECHOST_NAME,IARM_BUS_CEC_HOST_EnableDevMgr,(void *)&param1,sizeof(param1));

            while(j)
            {    
                try
                {
                    ret = IARM_Bus_Call(IARM_BUS_CECHOST_NAME, IARM_BUS_CEC_HOST_GetDevStatus, (void*)&param, sizeof(param));
                    if (ret != IARM_RESULT_SUCCESS )
                    {
                        printf("%s :: IARM_BUS_CEC_HOST_GetDevStatus failes \n",__FUNCTION__);
                    }
                    printf("Connected Devices : \n");
                    for (i = 0; i < 14; i++)
                    {
                        if (0 != param.devices[i].isConnected)
                        {
                            printf("Logical Address : %d \n", param.devices[i].logicalAddress);
                        }
                    }
                }
                catch (const std::exception e)  {
                    printf("%s failed \n", __FUNCTION__);
                }
                sleep(5);

                j--;
            }
            break;
        case 2:
            while(j)
            {
                /*Can try hdmi hot plug events during this test*/
                try{
                    int logicalAddress;
                    logicalAddress = LibCCEC::getInstance().getLogicalAddress(3);
                    printf("Got logical Address here logicalAddress : %d !!\n", logicalAddress);
                }
                catch (const std::exception e)  {
                   printf("getLogicalAddress caught exception %s \r\n",e.what());
                }

                sleep(5);
                j--;
            }
            break;

        case 3:
            {
                conn.sendTo(LogicalAddress::TV, MessageEncoder().encode(ActiveSource(phy)));
                printf("Send ActiveSource completed\r\n");
            }

            sleep(1);
            {
                conn.sendTo(LogicalAddress::TV, MessageEncoder().encode(ImageViewOn()));
                printf("===================Sending  ImageViewOn completed\r\n");
            }
            sleep(1);
            {
                conn.sendTo(LogicalAddress::TV, MessageEncoder().encode(TextViewOn()));
                printf("===================Sending  TextViewOn completed\r\n");
            }
            sleep(15);
            {
                conn.sendTo(LogicalAddress::TV, MessageEncoder().encode(InActiveSource(phy)));
                printf("===================Sending Standby completed...\r\n");
            }
            sleep(25);
            break;
        default:
            break;
    }

    conn.close();

    LibCCEC::getInstance().term();

    IARM_Bus_Disconnect();
    IARM_Bus_Term();
}
//Note: To enable yocto build for the test app, please add the folder name 'tests' in the 
//SUBDIRS & DIST_SUBDIRS parameters in /hdmicec/Makefile.am


/** @} */
/** @} */
