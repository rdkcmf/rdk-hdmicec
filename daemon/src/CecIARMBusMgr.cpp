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
* @defgroup daemon
* @{
**/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#include "libIARM.h"
#include "iarmUtil.h"
#include "libIBus.h"

#include "dsMgr.h"
#include "dsDisplay.h"

#include "CecIARMBusMgr.hpp"
#include "osal/Mutex.hpp"
#include "ccec/drivers/iarmbus/CecIARMBusMgr.h"

#include "ccec/CCEC.hpp"
#include "ccec/Assert.hpp"
#include "ccec/Connection.hpp"
#include "ccec/MessageDecoder.hpp"
#include "ccec/MessageProcessor.hpp"

#include "ccec/Util.hpp"
#include "ccec/driver/hdmi_cec_driver.h"

using CCEC_OSAL::AutoLock;

CCEC_BEGIN_NAMESPACE

static Mutex mutex;
static bool enabled;
static volatile Connection *m_connection = 0;

static IARM_Result_t _Send(void *arg);
static IARM_Result_t _GetLogicalAddress(void *arg);
static IARM_Result_t _Enable(void *arg);

static void _iarmMgrHdmiEventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len);


static void _iarmMgrHdmiEventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len)
{
    switch (eventId) {
        case IARM_BUS_DSMGR_EVENT_HDMI_HOTPLUG :{

            IARM_Bus_DSMgr_EventData_t *eventData = (IARM_Bus_DSMgr_EventData_t *)data;
            int hdmi_hotplug_event = eventData->data.hdmi_hpd.event;
            CCEC_LOG( LOG_DEBUG, "I-ARM CEC Mgr:: Received IARM_BUS_DSMGR_EVENT_HDMI_HOTPLUG  event data:%d \r\n", hdmi_hotplug_event);
            if( dsDISPLAY_EVENT_DISCONNECTED == hdmi_hotplug_event)
            {
                CCEC_LOG( LOG_INFO, "I-ARM CEC Mgr:: no Closing soc driver due to HDMI disconnect event.\r\n");
	        try{AutoLock lock_(mutex);
                //CECIARMMgr::getInstance().stop();
	        }
	        catch(Exception &e)
	        {
                    CCEC_LOG( LOG_EXP, "I-ARM CEC Mgr:: Caught Exception while calling LibCCEC::term()\r\n");
                }

            }
            else if(dsDISPLAY_EVENT_CONNECTED == hdmi_hotplug_event)
            {
                CCEC_LOG( LOG_INFO, "I-ARM CEC Mgr:: no Opening soc driver on receiving HDMI connect event\r\n");
	        try{AutoLock lock_(mutex);
                if (enabled) {
                    //CECIARMMgr::getInstance().start();
                }
	        }
	        catch(Exception &e)
	        {
                    CCEC_LOG( LOG_EXP, "I-ARM CEC Mgr:: Caught Exception while calling LibCCEC::init()\r\n");
                }
            }
            else
            {
                //do nothing
            }
      }
      break;
      default:
      //do nothing
      break;
   }
}


static void cecSendEventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len)
{
    Mutex m;
	IARM_Bus_CECMgr_EventData_t *cecEventData = (IARM_Bus_CECMgr_EventData_t*)data;
    CECFrame frameIn;
	frameIn.append(cecEventData->data, cecEventData->length);
    if (m_connection) {
    	CCEC_LOG( LOG_DEBUG, "cecSendEventHandler sending >>>>>>\r\n");
        ((Connection *)m_connection)->sendAsync(frameIn);
    }
}

static void cecMgrEventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len)
{
	IARM_Bus_CECMgr_EventData_t *cecEventData = (IARM_Bus_CECMgr_EventData_t*)data;
    switch (eventId) {
        case IARM_BUS_CECMGR_EVENT_ENABLE: {
            if (cecEventData->length == 1) {
                {AutoLock lock_(mutex); 
                     IARM_Bus_CECMgr_Enable_Param_t param;
                     param.enabled = cecEventData->data[0];
                     _Enable(&param);
                }
            }
        }
        break;
        default:
        /*ignore*/
        break;
    }
}

class CecIARMMgrFrameListener : public FrameListener
{
public:
	CecIARMMgrFrameListener(MessageProcessor *processor) : processor(processor) {}
	void notify(const CECFrame &in) const {
		MessageDecoder(*processor).decode(in);

		/* Signal to CEC IARMMgr's clients */
		IARM_Bus_CECMgr_EventData_t dataRcvd;
		memset(&dataRcvd, 0, sizeof(dataRcvd));
		if (in.length() <= IARM_BUS_CECMGR_MAX_DATA_LENGTH) {
			dataRcvd.length = in.length();
			memcpy(dataRcvd.data, in.getBuffer(), in.length());
			CCEC_LOG( LOG_DEBUG, "Broadcasting msg on IARMBus\r\n");
			IARM_Bus_BroadcastEvent(IARM_BUS_CECMGR_NAME, (IARM_EventId_t) IARM_BUS_CECMGR_EVENT_RECV, (void *)&dataRcvd, sizeof(dataRcvd));
		}
		else {
			Assert(0);
		}
	}
	~CecIARMMgrFrameListener() {processor = 0;}
private:
	MessageProcessor *processor;
};

CECIARMMgr & CECIARMMgr::getInstance(void)
{
	static CECIARMMgr instance;
	return instance;
}

IARM_Result_t CECIARMMgr::init(void)
{
    started = false;
    IARM_Bus_Init(IARM_BUS_CECMGR_NAME);
    IARM_Bus_Connect();
    IARM_Bus_RegisterEvent(IARM_BUS_CECMGR_EVENT_MAX);
    IARM_Bus_RegisterCall(IARM_BUS_CECMGR_API_Send,_Send);
    IARM_Bus_RegisterCall(IARM_BUS_CECMGR_API_GetLogicalAddress,_GetLogicalAddress);
    IARM_Bus_RegisterCall(IARM_BUS_CECMGR_API_Enable, _Enable);
    IARM_Bus_RegisterEventHandler(IARM_BUS_CECMGR_NAME, IARM_BUS_CECMGR_EVENT_ENABLE, cecMgrEventHandler);
    IARM_Bus_RegisterEventHandler(IARM_BUS_CECMGR_NAME, IARM_BUS_CECMGR_EVENT_SEND, cecSendEventHandler);
    IARM_Bus_RegisterEventHandler(IARM_BUS_DSMGR_NAME,IARM_BUS_DSMGR_EVENT_HDMI_HOTPLUG, _iarmMgrHdmiEventHandler);
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t CECIARMMgr::term(void)
{
	IARM_Bus_Disconnect();
    IARM_Bus_Term();
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t CECIARMMgr::start(void)
{AutoLock lock_(mutex);
    
    if  (started) return IARM_RESULT_INVALID_STATE;

    CCEC_LOG( LOG_INFO, "Inside CECIARMMgr::start\r\n");
    LibCCEC::getInstance().init("CECIARMMgr");
    processor = new MessageProcessor();
    frameListener = new CecIARMMgrFrameListener(processor);
    connection = new Connection(LogicalAddress::UNREGISTERED, false, "CECIARMMgr::Connection::");

    connection->open();
    connection->addFrameListener(frameListener);

    m_connection = connection;
    /* Listen for incoming Packet */
    started = true;
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t CECIARMMgr::loop(void)
{
    time_t curr = 0;
    while(1)
    {
        time(&curr);
        CCEC_LOG( LOG_WARN, "I-ARM CEC Mgr: HeartBeat at %s\r\n", ctime(&curr));
        sleep(300);
    }
	return IARM_RESULT_SUCCESS;
}


IARM_Result_t CECIARMMgr::stop(void)
{
    {AutoLock lock_(mutex);

        if (!started) return IARM_RESULT_INVALID_STATE;

        CCEC_LOG( LOG_INFO, "Inside CECIARMMgr::stop\r\n");
        connection->removeFrameListener(frameListener);
        connection->close();

        delete frameListener;
        frameListener = 0;

        m_connection = 0;

        delete connection;
        connection = 0;

        delete processor;
        processor = 0;

        LibCCEC::getInstance().term();
        started = false;
    }

    return IARM_RESULT_SUCCESS;
}


static IARM_Result_t _Send(void *arg)
{
	IARM_Bus_CECMgr_Send_Param_t *param = (IARM_Bus_CECMgr_Send_Param_t *)arg;
	CECFrame frameIn;
	frameIn.append(param->data,param->length);
	if (m_connection) {
		try{
			//CCEC_LOG( LOG_DEBUG, "_Send sending >>>>>>\r\n");
			((Connection *)m_connection)->send(frameIn, 0, Throw_e());
			param->retVal = HDMI_CEC_IO_SENT_AND_ACKD;
		}
		catch(CECNoAckException &e )
		{
			//CCEC_LOG( LOG_EXP, "_Send caught %s \r\n",e.what());
			param->retVal = HDMI_CEC_IO_SENT_BUT_NOT_ACKD;
		}
		catch(Exception &e)
		{
			//CCEC_LOG(LOG_EXP, "_Send caught %s \r\n",e.what());
			param->retVal = HDMI_CEC_IO_SENT_FAILED;
		}
	}
	else
	{
		CCEC_LOG( LOG_DEBUG, "Connection not ready >>>>>>\r\n");
		return IARM_RESULT_INVALID_STATE;
	}
	return IARM_RESULT_SUCCESS;
}

static IARM_Result_t _GetLogicalAddress(void *arg)
{
	IARM_Result_t retCode = IARM_RESULT_SUCCESS;
	IARM_Bus_CECMgr_GetLogicalAddress_Param_t *param = (IARM_Bus_CECMgr_GetLogicalAddress_Param_t *) arg;

	CCEC_LOG( LOG_DEBUG, "Inside _GetLogicalAddress : for DevType : %d  >>>>>>\r\n",param->devType);
	try{
		param->logicalAddress = LibCCEC::getInstance().getLogicalAddress(param->devType);
	}
	catch(Exception &e)
	{
		CCEC_LOG(LOG_EXP, "_GetLogicalAddress caught %s \r\n",e.what());
		retCode = IARM_RESULT_INVALID_STATE;
	}

	CCEC_LOG( LOG_DEBUG, "Inside _GetLogicalAddress : Logical Address : %d  >>>>>>\r\n",param->logicalAddress);

	return retCode;
}


static IARM_Result_t _Enable(void *arg)
{
    IARM_Result_t retCode = IARM_RESULT_SUCCESS;

    {AutoLock lock_(mutex);
        IARM_Bus_CECMgr_Enable_Param_t *param = (IARM_Bus_CECMgr_Enable_Param_t *) arg;
        CCEC_LOG( LOG_INFO, "Inside _Enable : %d, %d  >>>>>>\r\n", enabled, param->enabled);

        enabled = param->enabled;

        if (param->enabled) {
           CECIARMMgr::getInstance().start();
        }
        else {
           CECIARMMgr::getInstance().stop();
        }
    }
	return retCode;
}


CCEC_END_NAMESPACE




/** @} */
/** @} */
