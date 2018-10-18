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

/**
* @addtogroup HDMI_DAEMON
* @{
**/


#include "libIARM.h"

#include "ccec/CCEC.hpp"

CCEC_BEGIN_NAMESPACE

class Connection;
class FrameListener;
class MessageProcessor;

class CECIARMMgr {
public:
	static CECIARMMgr & getInstance(void);
public:
	IARM_Result_t init(void);
	IARM_Result_t start(void);
	IARM_Result_t stop(void);
	IARM_Result_t loop(void);
	IARM_Result_t term(void);
private:
    bool started;
	Connection *connection;
	FrameListener *frameListener;
	MessageProcessor *processor;
};

CCEC_END_NAMESPACE


/** @} */
/** @} */
/** @} */
