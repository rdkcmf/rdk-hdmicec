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


#ifndef _IARM_BUS_CECMGR_H
#define _IARM_BUS_CECMGR_H

#include <stdint.h>
#include "libIARM.h"

#ifdef __cplusplus
extern "C" 
{
#endif
#define IARM_BUS_CECMGR_NAME 						"CECMgr" /*!< IR Manager IARM -bus name */
#define IARM_BUS_CECMGR_MAX_DATA_LENGTH              62
#define IARM_BUS_CECMGR_API_Enable                   "Enable"
#define IARM_BUS_CECMGR_API_Send                     "Send"
#define IARM_BUS_CECMGR_API_GetLogicalAddress        "GetLogicalAddress"

/*! Events published from IR Mananger */
typedef enum _CECMgr_EventId_t {
	IARM_BUS_CECMGR_EVENT_SEND,           /*!< Request to Send CEC Data         */
	IARM_BUS_CECMGR_EVENT_RECV,           /*!< Notify Availability of CEC Data  */
	IARM_BUS_CECMGR_EVENT_ENABLE,         /*!< Notify Enable/Disable of CEC Driver  */
    IARM_BUS_CECMGR_EVENT_MAX,             /*!< Maximum event id*/
} IARM_Bus_IRMgr_EventId_t;

typedef enum _CECMgr_DataDirection_t {
	IAMR_BUS_CECMGR_DATADIRECTION_SEND,
	IARM_BUS_CECMGR_DATADIRECTION_RECV,
} IARM_Bus_CECMgr_DataDirection_t;

/*! Key Event Data */
typedef struct _CECMgr_EventData_t {
	uint8_t length;
	uint8_t data[IARM_BUS_CECMGR_MAX_DATA_LENGTH + 1];
}IARM_Bus_CECMgr_EventData_t;

typedef struct _IARM_Bus_CECMgr_Send_Param_t {
	uint8_t length;
	uint8_t data[IARM_BUS_CECMGR_MAX_DATA_LENGTH + 1];
	uint8_t retVal;
}IARM_Bus_CECMgr_Send_Param_t;

typedef struct _IARM_Bus_CECMgr_GetLogicalAddress_Param_t {
	int devType;
	int logicalAddress;
}IARM_Bus_CECMgr_GetLogicalAddress_Param_t;

typedef struct _IARM_Bus_CECMgr_Enable_Param_t {
    int enabled;
}IARM_Bus_CECMgr_Enable_Param_t;


#ifdef __cplusplus
}
#endif
#endif

/* End of IARM_BUS_CECMGR_API doxygen group */
/**
 */


/** @} */
/** @} */
