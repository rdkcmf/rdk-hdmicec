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
    bool inited = false;

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
        cout << "Please enter your selection : \n";
        if (!(cin >> j)) {
            cout << "Please enter numbers only." << endl;
            cin.clear();
            cin.ignore(10000,'\n');
            continue;
        }
       
        switch (j)
        {
            case 1:
                if(false == inited)
                {
                    cout << "LibCCEC init....................\n";
                    LibCCEC::getInstance().init();
                    inited = true;
                }
            break;
            case 2:
                if(true == inited)
                {
                    cout << "LibCCEC term....................\n";
                    LibCCEC::getInstance().term();
                    inited = false;
                }
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
                if(inited != false)
                    LibCCEC::getInstance().term();

                running = false;
            break;
            default:
               cout << "Invalid Entry" << endl;
               j = 0;
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
