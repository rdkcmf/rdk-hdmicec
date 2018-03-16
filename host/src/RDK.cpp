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
* @defgroup host
* @{
**/

/**
 * @defgroup hdmi_host HDMI-CEC Host
 * @ingroup HDMI_CEC
 * @ingroup hdmi_host
 * @{
 */


#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include "libIARM.h"
#include "libIBus.h"
#include "libIBusDaemon.h"
#include "pwrMgr.h"
#include "dsMgr.h"
#include "dsDisplay.h"
#include "videoOutputPort.hpp"
#include "host.hpp"
#include "exception.hpp"
#include "manager.hpp"
#include "ccec/Util.hpp"

#include "ccec/Host.hpp"
#include "ccec/host/RDK.hpp"


#define MAX_LENGTH_OF_OSD_NAME 15

typedef struct _DeviceStatus_t
{
        int logicalAddress;
        int powerState;
        int isConnected;
        char osdName[14+1];
}DeviceStatus_t;

static int powerState;

static CECHost_Callback_t hostCb = {0};
static pthread_mutex_t devStatMutex = PTHREAD_MUTEX_INITIALIZER;
static DeviceStatus_t _deviceStatus[14] = {0};

/*
 * osdName is a properly NULL-terminated cache of the
 * lastly set OSD name.
 */
static char osdName[14+1] = "SettopBox";


static IARM_Result_t _PowerPreChange(void *arg);
static IARM_Result_t _SetDevMgrStatus(void *arg);
static IARM_Result_t _SetOSDName(void *arg);
static IARM_Result_t _GetOSDName(void *arg);
static IARM_Result_t _GetDevStatus(void *arg);


static void _hdmiEventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len);
static void _powerEventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len);

/**
 * @brief This function is used to initialize CEC host device by registering
 * event handlers and calls like status, name etc.
 *
 * @param[in] name OSD name.
 *
 * @retval CECHost_ERR_NONE Returns for successful condition.
 */
CECHost_Err_t CECHost_Init(const char *name)
{
	IARM_Bus_Init(IARM_BUS_CECHOST_NAME);
	IARM_Bus_Connect();

    IARM_Bus_RegisterEventHandler(IARM_BUS_DSMGR_NAME,IARM_BUS_DSMGR_EVENT_HDMI_HOTPLUG, _hdmiEventHandler);
    IARM_Bus_RegisterEventHandler(IARM_BUS_PWRMGR_NAME, IARM_BUS_PWRMGR_EVENT_MODECHANGED, _powerEventHandler);

    IARM_Bus_RegisterCall(IARM_BUS_COMMON_API_PowerPreChange, _PowerPreChange);
    IARM_Bus_RegisterCall(IARM_BUS_CEC_HOST_EnableDevMgr, _SetDevMgrStatus);
    IARM_Bus_RegisterCall(IARM_BUS_CEC_HOST_SetOSDName, _SetOSDName);
    IARM_Bus_RegisterCall(IARM_BUS_CEC_HOST_GetOSDName, _GetOSDName);
    IARM_Bus_RegisterCall(IARM_BUS_CEC_HOST_GetDevStatus, _GetDevStatus);
    CCEC_LOG( LOG_WARN, "RDKHost::CECHost_Init Initialising DeviceStatus Cache \n");
    pthread_mutex_lock(&devStatMutex);
    for( int i=0;i<14;i++)
    {
        _deviceStatus[i].logicalAddress = i;
        _deviceStatus[i].powerState = 1;
        _deviceStatus[i].isConnected = 0;
        strncpy(_deviceStatus[i].osdName,"",MAX_LENGTH_OF_OSD_NAME);
    }
    pthread_mutex_unlock(&devStatMutex);

    device::Manager::Initialize();

    CECHost_GetPowerState(&powerState);

    return CECHost_ERR_NONE;
}

/**
 * @brief This function is used to de-initialize CEC host device, disconnects
 * and terminates the CEC connection.
 *
 * @retval CECHost_ERR_NONE Returns for successful condition.
 */
CECHost_Err_t CECHost_Term(void)
{
    device::Manager::DeInitialize();

	IARM_Bus_UnRegisterEventHandler(IARM_BUS_DSMGR_NAME,IARM_BUS_DSMGR_EVENT_HDMI_HOTPLUG);
	IARM_Bus_Disconnect();
	IARM_Bus_Term();
	return CECHost_ERR_NONE;
}

/**
 * @brief This function is used to set the callback function. It is called by
 * Device Manager application to set Dev manager status, OSD name, hot plug event
 * handler, power state event handler etc.
 *
 * @param[in] cb CECHost_Callback_t type structure.
 *
 * @retval CECHost_ERR_NONE Returns for successful condition.
 */
CECHost_Err_t CECHost_SetCallback(CECHost_Callback_t cb)
{
	hostCb = cb;
	return CECHost_ERR_NONE;
}

/**
 * @brief This function is used to get the physical address of the HDMI output
 * port byte by byte.
 *
 * @param[out] byte0 First byte of physical address.
 * @param[out] byte1 Second byte of physical address.
 * @param[out] byte2 Third byte of physical address.
 * @param[out] byte3 Fourth byte of physical address.
 *
 * @retval CECHost_ERR_NONE Returns for successful condition.
 * @retval CECHost_ERR_STATE Returns on error condition.
 */
CECHost_Err_t CECHost_GetHdmiOuputPhysicalAddress(uint8_t *byte0, uint8_t *byte1, uint8_t *byte2, uint8_t *byte3)
{
	/* Use DS */
	CCEC_LOG( LOG_DEBUG, "CECHost_GetHdmiOuputPhysicalAddress entering\r\n");
	try {
		device::VideoOutputPort vPort = device::Host::getInstance().getVideoOutputPort("HDMI0");
		if (vPort.isDisplayConnected()) {
			vPort.getDisplay().getPhysicallAddress(*byte0, *byte1, *byte2, *byte3);
			CCEC_LOG( LOG_WARN, "Geting connected PHY %d %d %d %d\r\n", *byte0, *byte1, *byte2, *byte3);
		}
		else {
			return CECHost_ERR_STATE;
		}
	}
	catch(device::Exception &e) {
		CCEC_LOG( LOG_EXP, "exception thrown %s\r\n", e.what());
		return CECHost_ERR_HOST;
	}

	return CECHost_ERR_NONE;
}

/**
 * @brief This function is used to check whether the HDMI output is connected
 * or not.
 *
 * @param[out] connect State of connection, 1 for connected and 0 for not connected..
 *
 * @retval CECHost_ERR_NONE Returns for successful condition.
 * @retval CECHost_ERR_HOST Returns on fail condition.
 */
CECHost_Err_t CECHost_IsHdmiOutputConnected(int32_t *connect)
{
	/* Use DS */
	CCEC_LOG( LOG_DEBUG, "CECHost_IsHdmiOutputConnected entering\r\n");
	try {
		device::VideoOutputPort vPort = device::Host::getInstance().getVideoOutputPort("HDMI0");
		if (vPort.isDisplayConnected()) {
			*connect = 1;
		}
		else {
			*connect = 0;
		}
	}
	catch(device::Exception &e) {
		CCEC_LOG( LOG_EXP, "exceptin thrown %s\r\n", e.what());
		return CECHost_ERR_HOST;
	}

	return CECHost_ERR_NONE;
}

/**
 * @brief This function is used to get the state of power whether it is ON or
 * in STANDBY state.
 *
 * @param[out] state State of power, ON or STANDBY.
 *
 * @retval CECHost_ERR_NONE Returns for successful condition.
 * @retval CECHost_ERR_HOST Returns on fail condition.
 */
CECHost_Err_t CECHost_GetPowerState(int32_t *state)
{
	IARM_Bus_PWRMgr_GetPowerState_Param_t param;
	param.curState = IARM_BUS_PWRMGR_POWERSTATE_ON;
	IARM_Result_t ret =  IARM_Bus_Call(IARM_BUS_PWRMGR_NAME, IARM_BUS_PWRMGR_API_GetPowerState, (void *)&param, sizeof(param));
	int isPowerOn = ((ret == IARM_RESULT_SUCCESS) && (param.curState == IARM_BUS_PWRMGR_POWERSTATE_ON));
	*state = isPowerOn ? CECHost_POWERSTATE_ON : CECHost_POWERSTATE_STANDBY;
	powerState = *state;
	CCEC_LOG( LOG_WARN, "CECHost_GetPowerState has state %d\r\n", *state);
	return ret == IARM_RESULT_SUCCESS ? CECHost_ERR_NONE : CECHost_ERR_HOST;
}

/**
 * @brief This function is used to set and update the device status like power
 * status, logical address, osd name etc based on status type.
 *
 * @param[in] logicalAddress Logical address of cec device.
 * @param[in] devices CECHost_DeviceStatus_t type pointer containing info about
 * device.
 * @retval CECHost_ERR_NONE Returns for successful condition.
 * @retval CECHost_ERR_HOST Returns on fail condition.
 */
CECHost_Err_t CECHost_SetDeviceStatus(int logicalAddress, CECHost_DeviceStatus_t *devices)
{
	IARM_Bus_CECHost_DeviceStatusChanged_EventData_t eventData;
	memset(&eventData,0,sizeof(eventData));
	IARM_Result_t ret = IARM_RESULT_SUCCESS;
	pthread_mutex_lock(&devStatMutex);
	if (devices->statusType == CECHost_POWER_STATUS)
	{
		if( _deviceStatus[logicalAddress].powerState != devices->data.powerState)
		{
			_deviceStatus[logicalAddress].powerState = devices->data.powerState;
			eventData.logicalAddress = logicalAddress;
			eventData.changedStatus = IARM_BUS_CECHost_POWER_STATUS;
			eventData.data.powerState = devices->data.powerState;
			CCEC_LOG( LOG_WARN, "RDKHost BroadcastingChangedPowerStatus on Iarm Bus \n");
			ret = IARM_Bus_BroadcastEvent(IARM_BUS_CECHOST_NAME,(IARM_EventId_t)IARM_BUS_CECHost_EVENT_DEVICESTATUSCHANGE,(void *)&eventData,sizeof(eventData));
		}
	}
	else if (devices->statusType == CECHost_OSD_NAME)
	{
		if( 0 != strncmp(_deviceStatus[logicalAddress].osdName,(const char *)devices->data.osdName,MAX_LENGTH_OF_OSD_NAME))
		{
			strncpy(_deviceStatus[logicalAddress].osdName,(const char *)devices->data.osdName,MAX_LENGTH_OF_OSD_NAME);
			eventData.logicalAddress = logicalAddress;
			eventData.changedStatus = IARM_BUS_CECHost_OSD_NAME;
			strncpy(eventData.data.osdName,(const char *)devices->data.osdName,MAX_LENGTH_OF_OSD_NAME);
			CCEC_LOG( LOG_DEBUG, "RDKHost BroadcastingChangedOsdNameStatus on Iarm Bus \n");
			ret = IARM_Bus_BroadcastEvent(IARM_BUS_CECHOST_NAME,(IARM_EventId_t)IARM_BUS_CECHost_EVENT_DEVICESTATUSCHANGE,(void *)&eventData,sizeof(eventData));
		}
	}
	else if (devices->statusType == CECHost_CONNECTED_STATUS)
	{
		if( _deviceStatus[logicalAddress].isConnected != devices->data.isConnected)
		{
			_deviceStatus[logicalAddress].isConnected = devices->data.isConnected;
			if( !devices->data.isConnected)
			{
				_deviceStatus[logicalAddress].powerState = 1;
				strncpy(_deviceStatus[logicalAddress].osdName,"",MAX_LENGTH_OF_OSD_NAME);
			}
			eventData.logicalAddress = logicalAddress;
			eventData.changedStatus = IARM_BUS_CECHost_CONNECT_STATUS;
			eventData.data.isConnected = devices->data.isConnected;
			CCEC_LOG( LOG_DEBUG, "RDKHost BroadcastingChanged ConnectStatus on Iarm Bus for %d \n",logicalAddress);
			ret = IARM_Bus_BroadcastEvent(IARM_BUS_CECHOST_NAME,(IARM_EventId_t)IARM_BUS_CECHost_EVENT_DEVICESTATUSCHANGE,(void *)&eventData,sizeof(eventData));
		}
	}
	else
        {
            /*Do nothing*/
	}
	pthread_mutex_unlock(&devStatMutex);
	return ret == IARM_RESULT_SUCCESS ? CECHost_ERR_NONE : CECHost_ERR_HOST;
}

/**
 * @brief This function is used to set the device power state to be ON or STANDBY.
 *
 * @param[in] state Power state value to be updated.

 * @retval CECHost_ERR_NONE Returns for successful condition.
 * @retval CECHost_ERR_HOST Returns on fail condition.
 */
CECHost_Err_t CECHost_SetPowerState(int32_t state)
{
	IARM_Bus_PWRMgr_SetPowerState_Param_t param;
	param.newState = ((state == CECHost_POWERSTATE_ON) ? IARM_BUS_PWRMGR_POWERSTATE_ON :IARM_BUS_PWRMGR_POWERSTATE_STANDBY);
	IARM_Result_t ret = IARM_Bus_Call(IARM_BUS_PWRMGR_NAME, IARM_BUS_PWRMGR_API_SetPowerState, (void *)&param, sizeof(param));
	return ret == IARM_RESULT_SUCCESS ? CECHost_ERR_NONE : CECHost_ERR_HOST;
}

/**
 * @brief This function is used to get OSD name of Host module. Here 'buf' need not
 * to be null terminated. If it is, the 'len' does not include the 'null' termintator.
 *
 * @param[in] buf The ASCII bytes of the OSD name
 * @param[in] len The number of ASCII bytes.
 *
 * @retval CECHost_ERR_NONE Returns for success condition.
 * @retval CECHost_ERR_INVALID Returns for failure condition.
 */
CECHost_Err_t CECHost_GetOSDName(uint8_t *buf, size_t *len)
{
	if (!buf || !len || *len == 0) {
		return CECHost_ERR_INVALID;
	}

	size_t nameLen = strlen(osdName);
	if (*len > nameLen) *len = nameLen;
    memcpy((char *)buf, (char *)osdName, *len);
    return CECHost_ERR_NONE;
}

/**
 * @brief This function is used to check whether host device is active or not.
 *
 * @param[out] active Variable that stores the state of box.
 *
 * @retval CECHost_ERR_NONE By default it returns success condition.
 */
CECHost_Err_t CECHost_IsActive(int32_t *active)
{
	/* Logic to determine if a box is activeSource
	 * For example, a User-Key PRess on a PowerON
	 * STB indicate that this box is active source.
	 *
	 * Simplest logic:
	 *     active =  (powerState == CECHost_POWERSTATE_ON)
	 */
	*active = (powerState == CECHost_POWERSTATE_ON);
	return CECHost_ERR_NONE;
}


static IARM_Result_t _PowerPreChange(void *arg)
{
    IARM_Bus_CommonAPI_PowerPreChange_Param_t *param = (IARM_Bus_CommonAPI_PowerPreChange_Param_t *)arg;
    //IARM_Result_t retCode = IARM_RESULT_SUCCESS;

    int curState = (param->curState == IARM_BUS_PWRMGR_POWERSTATE_ON) ? CECHost_POWERSTATE_ON : CECHost_POWERSTATE_STANDBY;
    int newState = (param->newState == IARM_BUS_PWRMGR_POWERSTATE_ON) ? CECHost_POWERSTATE_ON : CECHost_POWERSTATE_STANDBY;
    CCEC_LOG( LOG_DEBUG, "RDKHost::_PowerPreChange() Old State %d, New State: %d", curState , newState);

    return IARM_RESULT_SUCCESS;
}

static IARM_Result_t _SetDevMgrStatus(void *arg)
{
    bool opStatus = false;
    IARM_Result_t ret = IARM_RESULT_SUCCESS;
    IARM_Bus_CECHost_DevMgrStatus_Param_t opParam;
    IARM_Bus_CECHost_EnableDevMgr_Param_t *param = (IARM_Bus_CECHost_EnableDevMgr_Param_t *)arg;
    CCEC_LOG( LOG_DEBUG, "RDKHost::_SetDevMgrStatus() Dev Manager status set by client to : %s \n", (param->status)?"true":"false");

    if(hostCb.devMgrStatusCb){
        hostCb.devMgrStatusCb(param->status, &opStatus);
    }

    opParam.status = opStatus;

    ret = IARM_Bus_BroadcastEvent( IARM_BUS_CECHOST_NAME, (IARM_EventId_t)IARM_BUS_CECHost_EVENT_DEVMANAGERSTATUS, (void *)&opParam, sizeof(opParam));
    CCEC_LOG( LOG_DEBUG, "RDKHost::_SetDevMgrStatus() IARM Broadcast status : %s \n", (ret == IARM_RESULT_SUCCESS) ? "Success" : "Failure");

    return IARM_RESULT_SUCCESS;
}

static IARM_Result_t _SetOSDName(void *arg)
{
	IARM_Bus_CECHost_SetOSDName_Param_t *param = (IARM_Bus_CECHost_SetOSDName_Param_t *)arg;
        CCEC_LOG( LOG_DEBUG, "RDKHost::_SetOSDName() : %s::%s", param->name, osdName);

	if (strncmp(osdName, (char *)param->name, sizeof(param->name)) != 0) {
		osdName[14] = '\0';
       
         	strncpy((char *)osdName, (const char *) param->name, sizeof(osdName) - 1);
		if (hostCb.osdCb) {
			hostCb.osdCb((uint8_t *)osdName, strlen(osdName));
		}
	}
    return IARM_RESULT_SUCCESS;
}

static IARM_Result_t _GetDevStatus(void *arg)
{
    pthread_mutex_lock(&devStatMutex);
    if(NULL != arg)
    {
        IARM_Bus_CECHost_GetDeviceStatus_Param_t *param = (IARM_Bus_CECHost_GetDeviceStatus_Param_t *)arg;
        if(param->devices)
        {
            memcpy(param->devices,_deviceStatus,sizeof(_deviceStatus));
            CCEC_LOG( LOG_DEBUG, "RDKHOST::_GetDevStatus() success \n");
            pthread_mutex_unlock(&devStatMutex);
            return IARM_RESULT_SUCCESS;
        }
    }
    pthread_mutex_unlock(&devStatMutex);
    return IARM_RESULT_INVALID_PARAM;
}

static IARM_Result_t _GetOSDName(void *arg)
{
    if(NULL != arg)
    {
        IARM_Bus_CECHost_GetOSDName_Param_t *param = (IARM_Bus_CECHost_GetOSDName_Param_t *)arg;
        if(NULL != param->name)
        {
            memcpy(param->name,osdName,sizeof(osdName));
            CCEC_LOG( LOG_DEBUG, "RDKHost::_GetOSDName() : %s::%s\n",param->name,osdName);
            return IARM_RESULT_SUCCESS;
        }
    }
    return IARM_RESULT_INVALID_PARAM;
}

static void _hdmiEventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len)
{
   switch (eventId) {
      case IARM_BUS_DSMGR_EVENT_HDMI_HOTPLUG :{
         IARM_Bus_DSMgr_EventData_t *eventData = (IARM_Bus_DSMgr_EventData_t *)data;
         int hdmi_hotplug_event = eventData->data.hdmi_hpd.event;
         CCEC_LOG( LOG_DEBUG, "RDKHost::_hdmiEventHandler: IARM_BUS_DSMGR_EVENT_HDMI_HOTPLUG  event data:%d \r\n", hdmi_hotplug_event);
         if (hostCb.hotplugCb)
        	 hostCb.hotplugCb(
        			 ((hdmi_hotplug_event == dsDISPLAY_EVENT_CONNECTED) ?
        					 CECHost_HDMI_CONNECTED : CECHost_HDMI_DISCONNECTED));

        if( dsDISPLAY_EVENT_DISCONNECTED == hdmi_hotplug_event)
        {
            CCEC_LOG( LOG_DEBUG, "RDKHost::_hdmiEventHandler: Clearing device status on receiving disconnect event\r\n");
            pthread_mutex_lock(&devStatMutex);
            for( int i=0;i<14;i++)
            {
                _deviceStatus[i].logicalAddress = i;
                _deviceStatus[i].powerState = 1;
                _deviceStatus[i].isConnected = 0;
                strncpy(_deviceStatus[i].osdName,"",MAX_LENGTH_OF_OSD_NAME);
            }
            pthread_mutex_unlock(&devStatMutex);
        }

      }
      break;
   }
}

static void _powerEventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len)
{
   switch (eventId) {
      case  IARM_BUS_PWRMGR_EVENT_MODECHANGED :{
    	  IARM_Bus_PWRMgr_EventData_t *eventData = (IARM_Bus_PWRMgr_EventData_t *)data;
         //IARM_Result_t retCode = IARM_RESULT_SUCCESS;

         int curState = (eventData->data.state.curState == IARM_BUS_PWRMGR_POWERSTATE_ON) ? CECHost_POWERSTATE_ON : CECHost_POWERSTATE_STANDBY;
         int newState = (eventData->data.state.newState == IARM_BUS_PWRMGR_POWERSTATE_ON) ? CECHost_POWERSTATE_ON : CECHost_POWERSTATE_STANDBY;

         CCEC_LOG( LOG_DEBUG, "RDKHost::_powerEventHandler() Old State %d, New State: %d", curState , newState);
     	if (hostCb.pwrStateCb) hostCb.pwrStateCb(curState, newState);
     	powerState = newState;
      }
      break;
   }
}



/** @} */
/** @} */
/** @} */
