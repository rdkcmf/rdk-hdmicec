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


#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "libIBus.h"
#include "ccec/drivers/hdmi_cec_driver.h"
#include "ccec/drivers/iarmbus/CecIARMBusMgr.h"


#define CECDriverAssert(cond) do\
{\
	if (!(cond)) {\
		printf("Assert Failed at [%s][%d]\r\n", __FUNCTION__, __LINE__);\
	}\
}while(0)

typedef struct DriverContext_t
{
	pthread_mutex_t lock;
	HdmiCecTxCallback_t txCB;
	void * txCBdata;
	HdmiCecRxCallback_t rxCB;
	void * rxCBdata;

} DriverContext_t;

static DriverContext_t *driverCtx = 0;
pthread_mutex_t DriverMutex = PTHREAD_MUTEX_INITIALIZER;
#define DRIVER_LOCK()   do{/*printf("LOCKING DRV [%s]\r\n", __FUNCTION__);*/    pthread_mutex_lock(&DriverMutex);}while(0)
#define DRIVER_UNLOCK() do{/*printf("UNLOCKING DRV [%s]\r\n", __FUNCTION__);*/pthread_mutex_unlock(&DriverMutex);}while(0)

static void cecRecvEventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len);

int HdmiCecOpen(int *handle)
{
	DRIVER_LOCK();
	int retErr = HDMI_CEC_IO_SUCCESS;

	if (driverCtx == 0) {
        {
#if 0
            printf("Sending SYNC ENABLE to Daemon\r\n");
            IARM_Result_t ret = IARM_RESULT_SUCCESS;
            IARM_Bus_CECMgr_Enable_Param_t data;
            memset(&data, 0, sizeof(data));
            data.enabled =  1;
            IARM_Bus_Call(IARM_BUS_CECMGR_NAME,IARM_BUS_CECMGR_API_Enable,(void *)&data, sizeof(data));
#else
            printf("Sending ASYNC ENABLE to Daemon\r\n");
            IARM_Result_t ret = IARM_RESULT_SUCCESS;
            IARM_Bus_CECMgr_EventData_t data;
            memset(&data, 0, sizeof(data));
            data.length =  1;
            data.data[0]=  1;
            /*To Do - Return values checking of IARM Calls*/
            ret = IARM_Bus_BroadcastEvent(IARM_BUS_CECMGR_NAME, (IARM_EventId_t) IARM_BUS_CECMGR_EVENT_ENABLE, (void *)&data, sizeof(data));
#endif
        }
		IARM_Bus_RegisterEventHandler(IARM_BUS_CECMGR_NAME, IARM_BUS_CECMGR_EVENT_RECV, cecRecvEventHandler);
		driverCtx = (DriverContext_t *)malloc(sizeof(DriverContext_t));
		*handle = (int)driverCtx;
		retErr =  HDMI_CEC_IO_SUCCESS;
	}
	else {
		retErr =  HDMI_CEC_IO_INVALID_STATE;
	}

	DRIVER_UNLOCK();
	return retErr;
}

int HdmiCecClose(int handle)
{
	DRIVER_LOCK();
	CECDriverAssert(handle == ((int)driverCtx));

	int retErr = HDMI_CEC_IO_SUCCESS;
	if (driverCtx != 0) {
        {
#if 0
            printf("Sending SYNC DISABLE to Daemon\r\n");
            IARM_Result_t ret = IARM_RESULT_SUCCESS;
            IARM_Bus_CECMgr_Enable_Param_t data;
            memset(&data, 0, sizeof(data));
            data.enabled =  0;
            IARM_Bus_Call(IARM_BUS_CECMGR_NAME,IARM_BUS_CECMGR_API_Enable,(void *)&data, sizeof(data));
#else
            printf("Sending ASYNC DISABLE to Daemon\r\n");
            IARM_Result_t ret = IARM_RESULT_SUCCESS;
            IARM_Bus_CECMgr_EventData_t data;
            memset(&data, 0, sizeof(data));
            data.length =  1;
            data.data[0]=  0;
            /*To Do - Return values checking of IARM Calls*/
            ret = IARM_Bus_BroadcastEvent(IARM_BUS_CECMGR_NAME, (IARM_EventId_t) IARM_BUS_CECMGR_EVENT_ENABLE, (void *)&data, sizeof(data));
#endif
        }
		IARM_Bus_UnRegisterEventHandler(IARM_BUS_CECMGR_NAME, IARM_BUS_CECMGR_EVENT_RECV);
		free(driverCtx);
		driverCtx = 0;
		retErr =  HDMI_CEC_IO_SUCCESS;
	}
	else {
		retErr =  HDMI_CEC_IO_INVALID_STATE;
	}

	DRIVER_UNLOCK();
	return retErr;

}

int HdmiCecAddLogicalAddress(int handle, int logicalAddresses)
{
	//@TODO: IARM Call.
    return HDMI_CEC_IO_SUCCESS;
}

int HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses)
{
	//@TODO: IARM Call.
    return HDMI_CEC_IO_SUCCESS;
}

//int HdmiCecGetLogicalAddress(int handle, int *logicalAddresses, int *num)
int HdmiCecGetLogicalAddress(int handle, int devType,  int *logicalAddresses)
{
	DRIVER_LOCK();
	CECDriverAssert(handle == ((int)driverCtx));
	int retErr = HDMI_CEC_IO_SUCCESS;

	//printf("HdmiCecGetLogicalAddress from IARM driver DevType set  = %d \n", devType);

	IARM_Result_t ret = IARM_RESULT_SUCCESS;
	IARM_Bus_CECMgr_GetLogicalAddress_Param_t data;

	memset(&data, 0, sizeof(data));
	data.devType = devType;

	ret = IARM_Bus_Call(IARM_BUS_CECMGR_NAME,IARM_BUS_CECMGR_API_GetLogicalAddress,(void *)&data, sizeof(data));
	if( IARM_RESULT_SUCCESS != ret)
	{
		//printf("Iarm call failed retval = %d \n", ret);
		retErr = HDMI_CEC_IO_INVALID_STATE;
	}
        else
        {
		*logicalAddresses = data.logicalAddress;
        }

	//printf("HdmiCecGetLogicalAddress from IARM driver  Logical Address obtained  = %d \n", data.logicalAddress);

	DRIVER_UNLOCK();

	return retErr;
}

int HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void *data)
{
	DRIVER_LOCK();
	CECDriverAssert(handle == ((int)driverCtx));

	driverCtx->rxCB = cbfunc;
	driverCtx->rxCBdata = data;

	DRIVER_UNLOCK();

    return HDMI_CEC_IO_SUCCESS;
}

int HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void *data)
{
	DRIVER_LOCK();
	CECDriverAssert(handle == ((int)driverCtx));

	driverCtx->txCB = cbfunc;
	driverCtx->txCBdata = data;

	DRIVER_UNLOCK();

    return HDMI_CEC_IO_SUCCESS;
}

int HdmiCecTxAsync(int handle, const unsigned char *buf, int len)
{
	DRIVER_LOCK();
	CECDriverAssert(handle == ((int)driverCtx));

	IARM_Bus_CECMgr_EventData_t dataToSend;
	memset(&dataToSend, 0, sizeof(dataToSend));
	dataToSend.length = len;
	memcpy(dataToSend.data, buf, len);
    IARM_Bus_BroadcastEvent(IARM_BUS_CECMGR_NAME, (IARM_EventId_t) IARM_BUS_CECMGR_EVENT_SEND, (void *)&dataToSend, sizeof(dataToSend));

	DRIVER_UNLOCK();

	return HDMI_CEC_IO_SUCCESS;
}

int HdmiCecTx(int handle, const unsigned char *buf, int len, int *result)
{
	DRIVER_LOCK();
	CECDriverAssert(handle == ((int)driverCtx));
	IARM_Result_t ret = IARM_RESULT_SUCCESS;
	IARM_Bus_CECMgr_Send_Param_t dataToSend;

	memset(&dataToSend, 0, sizeof(dataToSend));
	dataToSend.length = len;
	memcpy(dataToSend.data, buf, len);
	ret = IARM_Bus_Call(IARM_BUS_CECMGR_NAME,IARM_BUS_CECMGR_API_Send,(void *)&dataToSend, sizeof(dataToSend));
	if( IARM_RESULT_SUCCESS != ret)
	{
		//printf("Iarm call failed retval = %d \n", ret);
		DRIVER_UNLOCK();
		return HDMI_CEC_IO_INVALID_STATE; 
	}
	*result = dataToSend.retVal;

	DRIVER_UNLOCK();

	return HDMI_CEC_IO_SUCCESS;
}


static void cecRecvEventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len)
{
	IARM_Bus_CECMgr_EventData_t *cecEventData = (IARM_Bus_CECMgr_EventData_t*)data;

	/* call the callback */
	if (driverCtx->rxCB) {
		//printf("CEC IARM Driver GOT messages =============\r\n");

		driverCtx->rxCB((int)driverCtx, driverCtx->rxCBdata, cecEventData->data, cecEventData->length);
	}
	else {
		//printf("CEC IARM Driver Dropping messages for lack of listeners\r\n");
	}
}

int HdmiCecSetLogicalAddress(int handle, int *logicalAddresses, int num)
{
    return 0;
}

//int main()
//{
//
//	int handle = 0;
//	int result = 0;
//	IARM_Bus_Init("CECClient");
//	IARM_Bus_Connect();
//	HdmiCecOpen(&handle);
//	unsigned char buf[2] = {0x30, 0x36};
//	HdmiCecTxAsync(handle, buf, 2);
//
//	sleep(10);
//
//	HdmiCecClose(handle);
//
//	IARM_Bus_Disconnect();
//	IARM_Bus_Term();
//}




/** @} */
/** @} */
