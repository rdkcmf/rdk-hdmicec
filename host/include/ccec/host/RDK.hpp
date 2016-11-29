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


#ifndef HDMI_CCEC_HOST_RDK_HPP_
#define HDMI_CCEC_HOST_RDK_HPP_

#ifdef __cplusplus
extern "C"
{
#endif
#define IARM_BUS_CECHOST_NAME 						"CECHost"
#define IARM_BUS_CEC_HOST_EnableDevMgr                      "SetDevMgrStatus"
#define IARM_BUS_CEC_HOST_SetOSDName 			    "SetOSDName"
#define IARM_BUS_CEC_HOST_GetOSDName                        "GetOSDName"
#define IARM_BUS_CEC_HOST_GetDevStatus                     "GetDevStatus"


typedef struct _IARM_Bus_CECHost_SetOSDName_Param_t {
	uint8_t name[14+1];        /*!< [in] OSDName max len + null terminator */
} IARM_Bus_CECHost_SetOSDName_Param_t;

typedef struct _IARM_Bus_CECHost_GetOSDName_Param_t {
        uint8_t name[14+1];        /*!< [in] OSDName max len + null terminator */
} IARM_Bus_CECHost_GetOSDName_Param_t;

typedef struct _IARM_Bus_CECHost_EnableDevMgr_Param_t {
	bool status;
}IARM_Bus_CECHost_EnableDevMgr_Param_t;

typedef struct _IARM_Bus_CECHost_DevMgrStatus_Param_t {
	bool status;
}IARM_Bus_CECHost_DevMgrStatus_Param_t;

typedef struct _IARM_Bus_CECHost_DeviceStatus_t
{
	int logicalAddress;
	int powerState;
	int isConnected;
	char osdName[14+1];
} IARM_Bus_CECHost_DeviceStatus_t;

typedef struct _IARM_Bus_CECHost_GetDeviceStatus_Param_t {
	IARM_Bus_CECHost_DeviceStatus_t devices[14];
}IARM_Bus_CECHost_GetDeviceStatus_Param_t;

typedef enum _IARM_Bus_CECHost_EventId_t
{
	IARM_BUS_CECHost_EVENT_DEVICESTATUSCHANGE = 0,
	IARM_BUS_CECHost_EVENT_DEVMANAGERSTATUS,
	IARM_BUS_CECHost_EVENT_MAX
} IARM_Bus_CECHost_EventId_t;

typedef enum _IARM_Bus_CECHost_StatusType_t{
	IARM_BUS_CECHost_POWER_STATUS = 1,
	IARM_BUS_CECHost_OSD_NAME = 2,
	IARM_BUS_CECHost_CONNECT_STATUS = 3
}IARM_Bus_CECHost_StatusType_t;

typedef struct _IARM_Bus_CECHost_DeviceStatusChanged_EventData_t {
	int logicalAddress;
	IARM_Bus_CECHost_StatusType_t changedStatus;
	union{
		int isConnected;
		int powerState;
		char osdName[14+1];
	}data;
}IARM_Bus_CECHost_DeviceStatusChanged_EventData_t;

#ifdef __cplusplus
}
#endif
#endif


/** @} */
/** @} */
