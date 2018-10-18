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
* @defgroup osal
* @{
**/


#ifndef HDMI_CCEC_OSAL_STOPPABLE_HPP_
#define HDMI_CCEC_OSAL_STOPPABLE_HPP_

#include "OSAL.hpp"

CCEC_OSAL_BEGIN_NAMESPACE

class Stoppable {
	enum {
		RUNNING,
		STOPPING,
		STOPPED,
	};
public:
	Stoppable(int state = RUNNING) : state(state) {}

	virtual void  stop(bool block = false) = 0;
	virtual bool  isStopped(void) {
		return state == STOPPED;
	}
protected:

	virtual void runStarted(void) {
		state = RUNNING;
	}
	virtual void  stopStarted(void) {
		if (state == RUNNING) state = STOPPING;
	}
	virtual bool  isStopping(void) {
		return ((state == STOPPING) || (state == STOPPED));
	}
	virtual bool isRunning(void) {
		return state == RUNNING;
	}
	virtual void stopCompleted(void) {
		state = STOPPED;
	}
private:
	volatile int state;

};

CCEC_OSAL_END_NAMESPACE

#endif


/** @} */
/** @} */
