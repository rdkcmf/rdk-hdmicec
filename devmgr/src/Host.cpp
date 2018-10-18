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


#include <dlfcn.h>
#include <stdio.h>

#include "ccec/Host.hpp"

static int32_t pluginGen = 0;

static void *plugin = 0;

#define DL_ERROR() printf("dlopen() error : [%s]\r\n", dlerror())

#ifndef CEC_HOST_PLUGIN
#warning "Using Default CEC_HOST_PLUGIN=librdkcechost.so"
#define CEC_HOST_PLUGIN "libRCECHost.so.0"
#endif

CECHost_Err_t CECHost_LoadPlugin(void)
{
	CECHost_Err_t ret = CECHost_ERR_NONE;
	if (plugin == NULL) {
        plugin = dlopen(CEC_HOST_PLUGIN, RTLD_LAZY);
        if (plugin == NULL) {
        	printf("dlopen() error : [%s]\r\n", dlerror());
        	ret = CECHost_ERR_SYMBOL;
        }
        else {
        	pluginGen++;
        }
	}
	else {
		ret = CECHost_ERR_STATE;
	}

	return ret;
}

CECHost_Err_t CECHost_UnloadPlugin(void)
{
	CECHost_Err_t ret = CECHost_ERR_NONE;

	if(plugin){
		dlclose(plugin);
		plugin = NULL;
	}
	else {
		ret = CECHost_ERR_STATE;
	}

	return ret;
}

static CECHost_Err_t _load(const char *funcName, void **func, int32_t *currGen)
{
	CECHost_Err_t ret = CECHost_ERR_NONE;

	if (plugin == NULL) return CECHost_ERR_STATE;

	if (*func == NULL || *currGen != pluginGen) {
		*func = dlsym(plugin, funcName);
		if (*func == NULL) {
			DL_ERROR();
			ret = CECHost_ERR_SYMBOL;
		}
		else {
			*currGen = pluginGen;
		}
	}

    return ret;
}

CECHost_Err_t CECHost_Init(const char* name)
{
	typedef CECHost_Err_t(*CECHost_Init_t)(const char *name);

	static int32_t currGen = pluginGen;
	static void *f = NULL;

	CECHost_Err_t ret = _load(__FUNCTION__, &f, &currGen);
	if (ret == CECHost_ERR_NONE) {
		ret = ((CECHost_Init_t)f)(name);
	}

    return ret;
}

CECHost_Err_t CECHost_Term(void)
{
	typedef CECHost_Err_t(*CECHost_Term_t)(void);

	static int32_t currGen = pluginGen;
	static void *f = NULL;

	CECHost_Err_t ret = _load(__FUNCTION__, &f, &currGen);
	if (ret == CECHost_ERR_NONE) {
		ret = ((CECHost_Term_t)f)();
	}

	return ret;
}
CECHost_Err_t CECHost_SetCallback(CECHost_Callback_t cb)
{
	typedef CECHost_Err_t(*CECHost_SetCallback_t)(CECHost_Callback_t cb);

	static int32_t currGen = pluginGen;
	static void *f = NULL;

	CECHost_Err_t ret = _load(__FUNCTION__, &f, &currGen);
	if (ret == CECHost_ERR_NONE) {
		ret = ((CECHost_SetCallback_t)f)(cb);
	}

	return ret;
}
CECHost_Err_t CECHost_GetHdmiOuputPhysicalAddress(uint8_t *byte0, uint8_t *byte1, uint8_t *byte2, uint8_t *byte3)
{
	typedef CECHost_Err_t(*CECHost_GetHdmiOuputPhysicalAddress_t)(uint8_t *byte0, uint8_t *byte1, uint8_t *byte2, uint8_t *byte3);

	static int32_t currGen = pluginGen;
	static void *f = NULL;

	CECHost_Err_t ret = _load(__FUNCTION__, &f, &currGen);
	if (ret == CECHost_ERR_NONE) {
		ret = ((CECHost_GetHdmiOuputPhysicalAddress_t)f)(byte0, byte1, byte2, byte3);
	}

	return ret;
}

CECHost_Err_t CECHost_IsHdmiOutputConnected(int32_t *connect)
{
	typedef CECHost_Err_t(*CECHost_IsHdmiOutputConnected_t)(int32_t *connect);

	static int32_t currGen = pluginGen;
	static void *f = NULL;

	CECHost_Err_t ret = _load(__FUNCTION__, &f, &currGen);
	if (ret == CECHost_ERR_NONE) {
		ret = ((CECHost_IsHdmiOutputConnected_t)f)(connect);
	}

	return ret;
}

CECHost_Err_t CECHost_GetPowerState(int32_t *state)
{
	typedef CECHost_Err_t(*CECHost_GetPowerState_t)(int32_t *state);

	static int32_t currGen = pluginGen;
	static void *f = NULL;

	CECHost_Err_t ret = _load(__FUNCTION__, &f, &currGen);
	if (ret == CECHost_ERR_NONE) {
		ret = ((CECHost_GetPowerState_t)f)(state);
	}

	return ret;
}
CECHost_Err_t CECHost_SetDeviceStatus(int logicalAddress, CECHost_DeviceStatus_t *deviceStatus)
{
	typedef CECHost_Err_t(*CECHost_SetDeviceStatus_t)(int logicalAddress, CECHost_DeviceStatus_t *deviceStatus);
	static int32_t currGen = pluginGen;
	static void *f = NULL;
	CECHost_Err_t ret = _load(__FUNCTION__, &f ,&currGen);
	if (ret == CECHost_ERR_NONE) {
		ret = ((CECHost_SetDeviceStatus_t)f)(logicalAddress, deviceStatus);
	}
	return ret;

}

CECHost_Err_t CECHost_SetPowerState(int32_t state)
{
	typedef CECHost_Err_t(*CECHost_SetPowerState_t)(int32_t state);

	static int32_t currGen = pluginGen;
	static void *f = NULL;

	CECHost_Err_t ret = _load(__FUNCTION__, &f, &currGen);
	if (ret == CECHost_ERR_NONE) {
		ret = ((CECHost_SetPowerState_t)f)(state);
	}

	return ret;
}

CECHost_Err_t CECHost_GetOSDName(uint8_t *buf, size_t *len)
{
	typedef CECHost_Err_t(*CECHost_GetOSDName_t)(uint8_t *buf, size_t *len);

	static int32_t currGen = pluginGen;
	static void *f = NULL;

	CECHost_Err_t ret = _load(__FUNCTION__, &f, &currGen);
	if (ret == CECHost_ERR_NONE) {
		ret = ((CECHost_GetOSDName_t)f)(buf, len);
	}

	return ret;
}

CECHost_Err_t CECHost_IsActive(int32_t *active)
{
	typedef CECHost_Err_t(*CECHost_IsActive_t)(int32_t *active);

	static int32_t currGen = pluginGen;
	static void *f = NULL;

	CECHost_Err_t ret = _load(__FUNCTION__, &f, &currGen);
	if (ret == CECHost_ERR_NONE) {
		ret = ((CECHost_IsActive_t)f)(active);
	}

	return ret;
}



/** @} */
/** @} */
