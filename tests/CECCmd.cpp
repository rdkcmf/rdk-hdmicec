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
#include "ccec/drivers/iarmbus/CecIARMBusMgr.h"


//The tool is to convert the hex bytes in command line to CECFrame and send it out via IARM
//CECCmd <hex bytes>
//E.g. CECCmd 3F 82 10 00 /From Tuner To Broadcast, Active_Source

int main(int argc, char *argv[])
{

    int i = 0;
    IARM_Bus_Init("CECClient");
    IARM_Bus_Connect();

#if 1
    LibCCEC::getInstance().init();
    Connection conn(LogicalAddress::UNREGISTERED, false);;
    conn.open();
#endif

    IARM_Result_t ret = IARM_RESULT_SUCCESS;
    IARM_Bus_CECMgr_Send_Param_t dataToSend;
    memset(&dataToSend, 0, sizeof(dataToSend));

    if (0 != argc)
    {
        printf("Count = %d \n Data : ", argc);
        
        for(i = 1; i < argc; i++)
        {
            printf("%x ", strtol(argv[i], NULL, 16));
            dataToSend.data[i-1] = (int)strtol(argv[i], NULL, 16);
        }

        dataToSend.length = (argc - 1);
        ret = IARM_Bus_Call(IARM_BUS_CECMGR_NAME,IARM_BUS_CECMGR_API_Send,(void *)&dataToSend, sizeof(dataToSend));
        if( IARM_RESULT_SUCCESS != ret)
        {
            printf("Iarm call failed retval = %d \n", ret);
        }
    }


#if 1
    conn.close();

    LibCCEC::getInstance().term();
#endif

    IARM_Bus_Disconnect();
    IARM_Bus_Term();
}
//Note: To enable yocto build for the test app, please add the folder name 'tests' in the 
//SUBDIRS & DIST_SUBDIRS parameters in /hdmicec/Makefile.am


/** @} */
/** @} */
