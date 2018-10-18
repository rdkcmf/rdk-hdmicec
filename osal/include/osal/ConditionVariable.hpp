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

/*****************************************************************************/
/*!
\file
\brief This file defines interface of ConditionVariable class.

*/
/*****************************************************************************/



/**
* @defgroup hdmicec
* @{
* @defgroup osal
* @{
**/


#ifndef HDMI_CCEC_OSAL_CONDITION_VARIABLE_HPP_
#define HDMI_CCEC_OSAL_CONDITION_VARIABLE_HPP_

#include "OSAL.hpp"
#include "Mutex.hpp"
#include "Condition.hpp"

CCEC_OSAL_BEGIN_NAMESPACE
/***************************************************************************/
/*!

ConditionVariable factors out the Object monitor methods (wait, notify and notifyAll)
into distinct objects to give the effect of having multiple wait-sets per object, 
by combining them with the use of arbitrary Lock implementations. 

ConditionVariables provide a means for one thread to suspend execution (to "wait") 
until notified by another thread that some state condition may now be true. 
Because access to this shared state information occurs in different threads, 
it must be protected, so a lock of some form is associated with the condition. 
The key property that waiting for a condition provides is that it atomically 
releases the associated lock and suspends the current thread.

A ConditionVariable instance is intrinsically bound to a lock. 
*/
/**************************************************************************/


class ConditionVariable {
public:
/***************************************************************************/
/*!
\brief Constructor.
Creates a ConditionVariable object.

*/
/**************************************************************************/
	ConditionVariable(void);
/***************************************************************************/

/*!
\brief Destructor.
Destroys the ConditionVariable object.

*/
/**************************************************************************/

	~ConditionVariable(void);
/***************************************************************************/
/*!
\brief sets the condition.
Sets the condition assosiated with the ConditionVariable object.

*/
/**************************************************************************/

	void set(void);
/***************************************************************************/
/*!
\brief resets the condition.
Resets the condition assosiated with the ConditionVariable object to default
(false).

*/
/**************************************************************************/

	void reset(void);
/***************************************************************************/
/*!
\brief Checks the status of the condition.
Returns the status of the condition associated. Could be true/false.

\return - boolean value of the associated condition.
*/
/**************************************************************************/
	bool isSet(void);
/***************************************************************************/
/*!
\brief Wait until the conditional variable is signalled.
Causes the current thread to wait until it is signalled or interrupted. 
Calling thread will wait undefinetly until some other thread signals the 
ConditionVariable by calling notify/notifyAll.
*/
/**************************************************************************/

	void wait(void);
/***************************************************************************/
/*!
\brief Wait until the conditional variable is signalled.
Causes the current thread to wait until it is signalled or interrupted . 
Calling thread will wait undefinetly until some other thread signals the 
ConditionVariable by calling notify/notifyAll.
*/
/**************************************************************************/

	long wait(long timeout);
	void notify(void);
	void notifyAll(void);
	void *getNativeHandle(void);
private:
	Condition *cond;
	Mutex *mutex;
	void *nativeHandle;

	ConditionVariable(const ConditionVariable &); /* Not allowed */
	ConditionVariable & operator = (const ConditionVariable &); /* Not allowed */
};

CCEC_OSAL_END_NAMESPACE
#endif


/** @} */
/** @} */
