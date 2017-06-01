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

#include <iostream>
#include <string>

#include "libIBus.h"
#include "ccec/Messages.hpp"
#include "ccec/Assert.hpp"
#include "ccec/Connection.hpp"
#include "ccec/host/RDK.hpp"
#include "ccec/MessageEncoder.hpp"
#include "ccec/LibCCEC.hpp"
#include "ccec/drivers/iarmbus/CecIARMBusMgr.h"

using namespace std;


int main(int argc, char *argv[])
{
    int i = 0, j = 0;
    string command;
    size_t pos = 0;
    std::string token;
    string delimiter = ":";
    bool running = true;

    Connection *testConnection = NULL;
    IARM_Bus_Init("CECClient");
    IARM_Bus_Connect();

    IARM_Result_t ret = IARM_RESULT_SUCCESS;
    IARM_Bus_CECMgr_Send_Param_t dataToSend;
    memset(&dataToSend, 0, sizeof(dataToSend));

    cout << "********Entered CECCmd tool***********\n";
    cout << "Options : " << endl;
    cout << "1 - To enable CEC\n2 - To disable CEC\n";
    cout << "3 - Send CEC Command\n4 - Exit test application \n";
    while(running)
    {
        cout << "Please enter your selection\n";
        scanf("%d",&j);
       
        switch (j)
        {
            case 1:
                if(NULL == testConnection)
                {
                    cout << "ENABLING CEC ....................\n";
                    LibCCEC::getInstance().init();
                    testConnection = new Connection(LogicalAddress::UNREGISTERED,false,"TestConnection");     
                    testConnection->open();
                }
                else
                    cout << "CEC is already enabled!!" << endl;
            break;
            case 2:
                if(NULL != testConnection)
                {
                   cout << "DISABLING CEC ...................." << endl;
                   testConnection->close();
                   delete testConnection;
                   testConnection = NULL;
                   LibCCEC::getInstance().term();
                }
                else
                    cout << "CEC is already disabled!!" << endl;
            break;
            case 3:
                 cout << "Please enter your CEC Command - eg: 3F:82:10:00" << endl;
                 cin >> command;
                 cout << "Command is : " << command << endl;
                 i = 0;
                 while ((pos = command.find(delimiter)) != string::npos) {
                         token = command.substr(0, pos);
                         dataToSend.data[i++] = (int) strtol(token.c_str(), NULL, 16);
                         command.erase(0, pos + delimiter.length());
                 }
                 dataToSend.data[i++] = (int) strtol(command.c_str(), NULL, 16);
                 dataToSend.length = i;
                 ret = IARM_Bus_Call(IARM_BUS_CECMGR_NAME,IARM_BUS_CECMGR_API_Send,(void *)&dataToSend, sizeof(dataToSend));
                 if( IARM_RESULT_SUCCESS != ret)
                 {
                      cout << "Iarm call failed retval " << ret << endl;
                 }
            break;
            case 4:
               running = false;
            break;
            default:
               cout << "Invalid Entry" << endl;
            break;
        }
    }

    IARM_Bus_Disconnect();
    IARM_Bus_Term();
}
//Note: To enable yocto build for the test app, please add the folder name 'tests' in the 
//SUBDIRS & DIST_SUBDIRS parameters in /hdmicec/Makefile.am


/** @} */
/** @} */
