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
* @defgroup devmgr
* @{
**/


#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "ccec/LibCCEC.hpp"
#include "ccec/Assert.hpp"
#include "ccec/Messages.hpp"
#include "ccec/Connection.hpp"
#include "ccec/MessageEncoder.hpp"
#include "ccec/MessageDecoder.hpp"
#include "ccec/MessageProcessor.hpp"
#include "ccec/Util.hpp"
#include "ccec/Host.hpp"

static CECHost_Err_t  CECHost_HDMI_HotplugCallback_(int32_t connect);
static CECHost_Err_t  CECHost_PowerStateCallback_ (int32_t curState, int32_t newState);
static CECHost_Err_t CECHost_DevMgrStatusCallback_(bool ipStatus,bool* opStatus);
static CECHost_Err_t CECHost_OSDNameCallback_(uint8_t *name, size_t len);
static CECHost_Err_t CECHost_PolicyCallback_(CECHost_Policy_t policy);


/*Avoiding currently unused variables and function to avoid compilation warnings*/
#if 0
/*Temporary change to allow power sync test*/
static int file_exists(const char *filename)
{
        struct stat st;
        memset(&st,0,sizeof(st));

        if (0 == stat(filename,&st))
        {
                if (S_ISREG(st.st_mode))
                {
                        return 1;
                }
        }
        return 0;
}

static bool devMgrStatus = false;
static int32_t cecClntRefCounter = 0;
static pthread_mutex_t devMgrMutex = PTHREAD_MUTEX_INITIALIZER;
static CECHost_Policy_t policy = {
    /*Default Policy*/
    /*.turnOffTv = */0,
    /*.turnOffSTB = */0,
};
#endif


/*
 * Context/Cache
 */
static Connection *conn_ = 0;
static int32_t isConnected = 0;
static PhysicalAddress phy(0,0,0,0);
static int32_t powerState = CECHost_POWERSTATE_STANDBY;

#define MAX_LOGICAL_ADDRESS 14
#define MAX_LENGTH_OF_OSD_NAME 15

/*
 * *_Simple() are internal helper functions.
 */
static std::string CECHost_GetOSDName_Simple(void)
{
	uint8_t name[OSDName::MAX_LEN + 1] = {0};
	size_t len = OSDName::MAX_LEN;
	if (CECHost_GetOSDName(name, &len) == CECHost_ERR_NONE) {
		/* NULL terminate the name for simplicity */
		name[len] = '\0';
		return (char *)name;
	}
	else {
		return "SettopBox";
	}
}

static int32_t CECHost_IsActive_Simple(void)
{
	int32_t active = 0;
	CECHost_Err_t err = CECHost_IsActive(&active);
	if ((err == CECHost_ERR_NONE) && active) {
		return 1;
	}
	else {
		return 0;
	}
}

static int32_t CECHost_HDMI_IsConnected_Simple(uint8_t *byte0 = 0, uint8_t *byte1 = 0, uint8_t *byte2 = 0, uint8_t *byte3 = 0)
{
	int32_t connected = 0;
	CECHost_Err_t err = CECHost_IsHdmiOutputConnected(&connected);
	if ((err == CECHost_ERR_NONE) && connected) {
		return 1;
	}
	else {
		return 0;
	}
}

static int32_t CECHost_GetPowerState_Simple(void)
{
	int32_t state = 0;
	CECHost_Err_t ret =  CECHost_GetPowerState(&state);
	if (ret == CECHost_ERR_NONE) {
		state = ((state == CECHost_POWERSTATE_ON) ? PowerStatus::ON : PowerStatus::STANDBY);
	}
	else {
		state = PowerStatus::STANDBY;
	}

	return state;
}

class DevMgrProcessor : public MessageProcessor
{
	public:
		DevMgrProcessor(Connection &conn) : conn(conn) {}

		virtual void process (const RequestActiveSource &msg, const Header &header) {
			if (!(header.from == LogicalAddress(LogicalAddress::BROADCAST))) {
				{
					if (CECHost_IsActive_Simple() && (CECHost_HDMI_CONNECTED == isConnected)) {
						CCEC_LOG( LOG_DEBUG, "DevMgrProcessor:: Reporting ActiveSource\r\n");
						conn.sendTo(LogicalAddress(LogicalAddress::BROADCAST), MessageEncoder().encode(ActiveSource(phy)));
					}
				}
			}
		}

		virtual void process (const GiveOSDName &msg, const Header &header) {
			if (!(header.from == LogicalAddress(LogicalAddress::BROADCAST))) {
				CCEC_LOG( LOG_DEBUG, "DevMgrProcessor:: Reporting OSDName\r\n");
                                {
					conn.sendTo(header.from, MessageEncoder().encode(SetOSDName(OSDName("STBSTB"))));
				}   
			}   
		}   

		virtual void process (const GiveDeviceVendorID &msg, const Header &header) {
			if (!(header.from == LogicalAddress(LogicalAddress::BROADCAST))) {
				{
					CCEC_LOG( LOG_DEBUG, "DevMgrProcessor:: Reporting DeviceVendorID\r\n");
					conn.sendTo(LogicalAddress(LogicalAddress::BROADCAST), MessageEncoder().encode(DeviceVendorID(VendorID(0x0F, 0x0A, 0x0B))));
				}
			}   
		}   


		virtual void process (const GiveDevicePowerStatus &msg, const Header &header) {
			if (!(header.from == LogicalAddress(LogicalAddress::BROADCAST))) {
				{
					CCEC_LOG( LOG_DEBUG, "DevMgrProcessor:: Reporting DevicePowerStatus\r\n");
					conn.sendTo(header.from, MessageEncoder().encode(ReportPowerStatus(PowerStatus(powerState))));
				}
			}
		}

		virtual void process (const GetCECVersion &msg, const Header &header) {
			if (!(header.from == LogicalAddress(LogicalAddress::BROADCAST))) {
				CCEC_LOG( LOG_DEBUG, "DevMgrProcessor:: Reporting CECVersion\r\n");
				conn.sendTo(header.from, MessageEncoder().encode(CECVersion(Version::V_1_3a)));
			}
		}

		virtual void process (const GivePhysicalAddress &msg, const Header &header) {
			if (!(header.from == LogicalAddress(LogicalAddress::BROADCAST))) {
				{
					CCEC_LOG( LOG_DEBUG, "DevMgrProcessor:: Reporting PhysicalAddress\r\n");
					conn.sendTo(LogicalAddress(LogicalAddress::BROADCAST), MessageEncoder().encode(ReportPhysicalAddress(phy, DeviceType(DeviceType::TUNER))));
				}
			}
		}

		virtual void process (const Standby &msg, const Header &header)	{
		}

		virtual void process (const SetMenuLanguage &msg, const Header &header) {
		}

		virtual void process (const GetMenuLanguage &msg, const Header &header) {
		}

		virtual void process (const ReportPowerStatus &msg, const Header &header) {
		}

		virtual void process (const SetOSDName &msg, const Header &header) {
		}

	private:
		Connection &conn;

};

class DevMgrFrameListener : public FrameListener
{
public:
	DevMgrFrameListener(MessageProcessor &processor) : processor(processor) {}
	void notify(const CECFrame &in) const {
		MessageDecoder(processor).decode(in);
	}
	~DevMgrFrameListener() {}
private:
	MessageProcessor &processor;
};

int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IOLBF, 0);
	check_cec_log_status();
	CECHost_LoadPlugin();
	CECHost_Init("CECDevMgr");

	CECHost_Callback_t cb = {
		NULL,
		CECHost_PowerStateCallback_,
		NULL,
		NULL,
		NULL,
	};

	CECHost_SetCallback(cb);

	{
		int32_t connected;
		CCEC_LOG( LOG_WARN, "Calling CECHost_IsHdmiOutputConnected\r\n");
		if (CECHost_IsHdmiOutputConnected(&connected) == CECHost_ERR_NONE) {
			if (connected)
				isConnected = CECHost_HDMI_CONNECTED;
			else
				isConnected = CECHost_HDMI_DISCONNECTED;
			uint8_t byte0, byte1, byte2, byte3;
			if (CECHost_GetHdmiOuputPhysicalAddress(&byte0, &byte1, &byte2, &byte3) == CECHost_ERR_NONE) {
				phy = PhysicalAddress(byte0, byte1, byte2, byte3);
				CCEC_LOG( LOG_WARN, "PhysicalAddress : %s\n", phy.toString().c_str());
			}
			else {
				CCEC_LOG( LOG_WARN, "PhysicalAddress cannot be obtained: \n");
			}
		}


		CCEC_LOG( LOG_WARN, "Calling CECHost_GetPowerState\r\n");
		int32_t state;
		if (CECHost_GetPowerState(&state) == CECHost_ERR_NONE) {
			powerState = state;
		}
		else {
			CCEC_LOG( LOG_WARN,"PowerState cannot be obtained: \n");
		}

	}

	LibCCEC::getInstance().init("CECDevMgr");
	Connection conn(LogicalAddress::TUNER_1, false, "CECDevMgr::Connection::");
	conn_ = &conn;
	conn.open();
	{
		DevMgrProcessor proc(conn);
		DevMgrFrameListener frameListener(proc);
		conn.addFrameListener(&frameListener);

		LogicalAddress source;
		//CECHost_DeviceStatus_t devStatus;
		while(1) {
			static int j = 0 ;
			/*Heart Beat in appx 150s*/
			CCEC_LOG( LOG_WARN, "DevMgr Heartbeating %d\r\n", j++);
			sleep(10); //imp: Need to recalculate sleep time once doNotPoll flag is disabled.
		}
	}

	conn.close();

	LibCCEC::getInstance().term();

	CECHost_Term();
	CECHost_UnloadPlugin();

}

static CECHost_Err_t  CECHost_HDMI_HotplugCallback_(int32_t connect)
{
        CCEC_LOG( LOG_WARN, "Dev Manager received Hot plug event \r\n");

	if(CECHost_HDMI_CONNECTED == connect)
	{
		uint8_t byte0, byte1, byte2, byte3;
		if (CECHost_GetHdmiOuputPhysicalAddress(&byte0, &byte1, &byte2, &byte3) == CECHost_ERR_NONE) {
			phy = PhysicalAddress(byte0, byte1, byte2, byte3);
			CCEC_LOG( LOG_WARN, "PhysicalAddress : %s\n", phy.toString().c_str());
		}   
		else {
			CCEC_LOG( LOG_WARN, "PhysicalAddress cannot be obtained: \n");
		}   
            isConnected = 1;
        }
	else {
            isConnected = 0;
	}
	return CECHost_ERR_NONE;

}



static CECHost_Err_t  CECHost_PowerStateCallback_(int32_t curState, int32_t newState)
{
	if (newState == CECHost_POWERSTATE_ON) {
	   CCEC_LOG( LOG_DEBUG, "DevMgrProcessor:: Send ImageViewOn and ActiveSource\r\n");
           conn_->sendTo(LogicalAddress::TV, MessageEncoder().encode(ImageViewOn()), 5000);
           conn_->sendTo(LogicalAddress::TV, MessageEncoder().encode(ActiveSource(phy)), 5000);
	}
	else {
	   CCEC_LOG( LOG_DEBUG, "DevMgrProcessor:: Send InActiveSource\r\n");
           conn_->sendToAsync(LogicalAddress::TV, MessageEncoder().encode(InActiveSource(phy)));
	}

	powerState = newState;
	return CECHost_ERR_NONE;
}

/** @} */

