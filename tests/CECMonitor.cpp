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


#include<stdio.h>
#include<unistd.h>
#include "ccec/Connection.hpp"
#include "ccec/CECFrame.hpp"
#include "libIBus.h"
#include "ccec/Messages.hpp"
#include "ccec/MessageDecoder.hpp"
#include "ccec/MessageProcessor.hpp"


static int libcecInitStatus;
class TestFrameListener : public FrameListener
{
public:
    TestFrameListener(MessageProcessor &processor) : processor(processor) {}
    void notify(const CECFrame &in) const {
        const uint8_t *buf = NULL;
        size_t len = 0;

        printf("\n==================================================\n");
        printf("Received CEC Frame: \n");
        in.getBuffer(&buf, &len);

        for (int i = 0; i < len; i++) {
           printf("%02X ", (int) *(buf + i));
        }
        printf("\n");
        MessageDecoder(processor).decode(in);
        printf("==================================================\n");
    }
    ~TestFrameListener() {}
private:
    MessageProcessor &processor;
};

class TestCecProcessor : public MessageProcessor
{
public:
    TestCecProcessor(Connection &conn) : conn(conn) {}
        void process (const ActiveSource &msg, const Header &header)
	{
            printHeader(header);
            printf("Command: ActiveSource\n");
            printf("%s : %s  : %s \n",GetOpName(msg.opCode()),msg.physicalAddress.name().c_str(),msg.physicalAddress.toString().c_str());
        }
	void process (const InActiveSource &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: InActiveSource\n");
            printf("%s : %s : %s  \n",GetOpName(msg.opCode()),msg.physicalAddress.name().c_str(),msg.physicalAddress.toString().c_str());
        }
	void process (const ImageViewOn &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: ImageViewOn \n");
        }
	void process (const TextViewOn &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: TextViewOn\n");
        }
	void process (const RequestActiveSource &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: RequestActiveSource\n");
        }
	void process (const Standby &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: Standby\n");
        }
	void process (const GetCECVersion &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: GetCECVersion\n");
        }
	void process (const CECVersion &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: CECVersion\n");
            printf("Version : %s \n",msg.version.toString().c_str());
        }
	void process (const SetMenuLanguage &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: SetMenuLanguage\n");
            printf("Language : %s \n",msg.language.toString().c_str());
        }
	void process (const GiveOSDName &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: GiveOSDName\n");
        }
	void process (const GivePhysicalAddress &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: GivePhysicalAddress\n");
        }
	void process (const GiveDeviceVendorID &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: GiveDeviceVendorID\n");
        }
	void process (const SetOSDString &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: SetOSDString\n");
            printf("OSDString : %s\n",msg.osdString.toString().c_str());
        }
	void process (const SetOSDName &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: SetOSDName\n");
            printf("OSDName : %s\n",msg.osdName.toString().c_str());
        }
	void process (const RoutingChange &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: RoutingChange\n");
            printf("Routing Change From : %s\n",msg.from.toString().c_str());
            printf("Routing Change to : %s\n",msg.to.toString().c_str());
        }
	void process (const RoutingInformation &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: RoutingInformation\n");
            printf("Routing Information to Sink : %s\n",msg.toSink.toString().c_str());
        }
	void process (const SetStreamPath &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: SetStreamPath\n");
            printf("Set Stream Path to Sink : %s\n",msg.toSink.toString().c_str());
        }
	void process (const GetMenuLanguage &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: GetMenuLanguage\n");
        }
	void process (const ReportPhysicalAddress &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: ReportPhysicalAddress\n");
        }
        void process (const DeviceVendorID &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: DeviceVendorID\n");
            printf("VendorID : %s\n",msg.vendorId.toString().c_str());
        }

	//virtual void process (const UserControlReleased &msg, const Header &header) 		{header.print();msg.print();}
	//virtual void process (const UserControlPressed &msg, const Header &header) 		{header.print();msg.print();}
	void process (const GiveDevicePowerStatus &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: GiveDevicePowerStatus\n");
        }
	void process (const ReportPowerStatus &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: ReportPowerStatus\n");
            printf("Power Status: %s\n",msg.status.toString().c_str());
        }
	void process (const FeatureAbort &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: FeatureAbort\n");
        }
	void process (const Abort &msg, const Header &header)
        {
            printHeader(header);
            printf("Command: Abort\n");
        }
	void process (const Polling &msg, const Header &header) 			        {
            printHeader(header);
            printf("Command: Polling\n");
        }
private:
    Connection conn;
    void printHeader(const Header &header)
    {
        printf("Header : From : %s \n", header.from.toString().c_str());
        printf("Header : to   : %s \n", header.to.toString().c_str());
    }

};



int main()
{
    char test_status = 'r';
    IARM_Bus_Init("CECMonitorClient");
    IARM_Bus_Connect();
    if(0 == libcecInitStatus)
    {
        try
        {
            LibCCEC::getInstance().init();
            libcecInitStatus++;
        }
        catch (const std::exception e)
        {
            printf("Caught LibCCEC exception");
        }
    }
    Connection testConnection(LogicalAddress::UNREGISTERED,false, "CEC Monitor connection");
    testConnection.open();
    TestCecProcessor proc(testConnection);
    TestFrameListener frameListener(proc);
    testConnection.addFrameListener(&frameListener);

    while(test_status != 'q')
    {
        printf("CECMonitor Test App Started.Please enter 'q' to quit the test app \n");
        test_status = getchar();
    }


    testConnection.close();

    if(1 == libcecInitStatus)
    {
        LibCCEC::getInstance().term();
    }
    libcecInitStatus--;

    IARM_Bus_Disconnect();
    IARM_Bus_Term();

    return 1;
}

//Note: To enable yocto build for the test app, please add the folder name 'tests' in the 
//SUBDIRS & DIST_SUBDIRS parameters in /hdmicec/Makefile.am


/** @} */
/** @} */
