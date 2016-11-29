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


#ifndef HDMI_CCECHOST_HPP_
#define HDMI_CCECHOST_HPP_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Host Plugin must implement the following.
 */

typedef enum {
	CECHost_ERR_GENERAL = -1,
	CECHost_ERR_NONE,
	CECHost_ERR_INVALID,
	CECHost_ERR_SYMBOL,
	CECHost_ERR_HOST,
	CECHost_ERR_STATE,
} CECHost_Err_t;

#define CECHost_HDMI_CONNECTED 	0
#define CECHost_HDMI_DISCONNECTED 	1

#define CECHost_POWERSTATE_ON        0
#define CECHost_POWERSTATE_STANDBY   1

typedef struct _CECHost_Policy_t {
	int32_t turnOffTv;
	int32_t turnOffSTB;
} CECHost_Policy_t;

typedef enum _CECHost_DeviceStatusType_t{
	CECHost_POWER_STATUS = 1,
	CECHost_OSD_NAME = 2,
	CECHost_CONNECTED_STATUS =3
}CECHost_DeviceStatusType_t;

typedef struct _CECHost_DeviceStatus_t
{
	CECHost_DeviceStatusType_t statusType;
	union{
		int powerState;
		int isConnected;
		char osdName[14+1];
	}data;
}CECHost_DeviceStatus_t;

/*
 * The following are a set of callbacks that the host module
 * use to notify CEC about its state change.
 */

typedef CECHost_Err_t (*CECHost_HdmiHotplugCallback_t)	(int32_t connect);
typedef CECHost_Err_t (*CECHost_PowerStateCallback_t)	(int32_t curState, int32_t newState);
typedef CECHost_Err_t (*CECHost_DevMgrStatusCallback_t) (bool ipStatus,bool* opStatus);
/*
 * Description: Notify CEC of the latest OSD name.
 *
 * The 'name' need not be null terminated. if it is, the 'len' does
 * not include the 'null' termintator.
 *
 * @param name: the ASCII bytes of the OSD name.
 * @param len:  the number of ASCII bytes.
 */
typedef CECHost_Err_t (*CECHost_OSDNameCallback_t)		(uint8_t *name, size_t len);
typedef CECHost_Err_t (*CECHost_PolicyCallback_t)		(CECHost_Policy_t policy);

/*
 * A Set of callback to notify CEC that the host state has changed.
 */
typedef struct _CECHost_Callback_t {
	CECHost_HdmiHotplugCallback_t 	hotplugCb;
	CECHost_PowerStateCallback_t  	pwrStateCb;
        CECHost_DevMgrStatusCallback_t  devMgrStatusCb;
	CECHost_OSDNameCallback_t 		osdCb;
	CECHost_PolicyCallback_t 		policyCb;
} CECHost_Callback_t;

CECHost_Err_t CECHost_Init(const char *name);
CECHost_Err_t CECHost_Term(void);

CECHost_Err_t CECHost_SetCallback(CECHost_Callback_t cb);

/* HDMI  */
CECHost_Err_t CECHost_GetHdmiOuputPhysicalAddress(uint8_t *byte0, uint8_t *byte1, uint8_t *byte2, uint8_t *byte3);
CECHost_Err_t CECHost_IsHdmiOutputConnected(int32_t *connect);

/* Power */
CECHost_Err_t CECHost_GetPowerState(int32_t *state);
CECHost_Err_t CECHost_SetPowerState(int32_t state);

/* Device Status */
CECHost_Err_t CECHost_SetDeviceStatus(int logicalAddress, CECHost_DeviceStatus_t *deviceStatus);

/* CEC Control */

/*
 * Description: Get the OSD name from Host module.
 *
 * The 'name' need not be null terminated. if it is, the 'len' does
 * not include the 'null' termintator.
 *
 * @param name: the ASCII bytes of the OSD name.
 * @param len:  the number of ASCII bytes.
 */

CECHost_Err_t CECHost_GetOSDName(uint8_t *name, size_t *len);
CECHost_Err_t CECHost_GetPolicy(CECHost_Policy_t *policy);


/*
 * If box is an active source is different from its power state.
 * I.e. a PowerOn STB in lightsleep may not claim to be an
 * active source.
 *
 * STANDBY implies Inactive
 * ON does not imply Active
 * Active imples ON
 * Inactive does not imply STANDBY
 *
 */
CECHost_Err_t CECHost_IsActive(int32_t *active);


/*
 * Host Plugin Need not implement these two APIs.
 */
CECHost_Err_t CECHost_LoadPlugin(void);
CECHost_Err_t CECHost_UnloadPlugin(void);



#ifdef __cplusplus
}
#endif

#endif


/** @} */
/** @} */
