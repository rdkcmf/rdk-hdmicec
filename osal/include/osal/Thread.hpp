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
\brief This file defines interface of Thread class

*/
/*****************************************************************************/


/**
* @defgroup hdmicec
* @{
* @defgroup osal
* @{
**/


#include <stdint.h>
#include <string>

#ifndef HDMI_CCEC_OSAL_THREAD_HPP_
#define HDMI_CCEC_OSAL_THREAD_HPP_

#include "OSAL.hpp"

#include "Runnable.hpp"


CCEC_OSAL_BEGIN_NAMESPACE


/***************************************************************************/
/*!

A thread is a thread of execution in a program. 
An application could have multiple threads of execution running concurrently.

A class that needs thread functionality shall implement Runnable interface and 
object will be passed to Thread on creation of Thread object. 
On the invocation of start() method of Thread, runnable's run() will be executed 
in a threaded context.
*/
/**************************************************************************/

class Thread : public Runnable {
public:
/**************************************************************************/
/*! 
\brief Constructor

 Allocates a new Thread object
 \param target - Object implements Runnable interface.  
 */
 /************************************************************************/

	Thread(Runnable &target);
/**************************************************************************/
/*! 
\brief Constructor

 Allocates a new Thread object
 \param target - Object implementes Runnable interface.
 \name - Name of the thread context.
 */
 /************************************************************************/

	Thread(Runnable &target, const int8_t* name);
/**************************************************************************/
/*! 
\brief Destructor

 Destroys the Thread object
 */
 /************************************************************************/

	virtual ~Thread(void);
/*! 
\brief Executes the run() method of runnable object.

 If this object is created by passing reference to a runnable object, that 
 object's run() will be executed, otherwise this method does nothing and returns.
 */
 /************************************************************************/

	void run(void);

/************************************************************************/
/*!
\brief Starts excecution of the thread.

 Causes this thread to begin execution;This calls the run method of this thread.
 The result is that two threads are running concurrently: 
 the current thread (which returns from the call to the start method) 
 and the other thread (which executes its run method).
 */
/***********************************************************************/

	void start(void);
/************************************************************************/
/*!
\brief Stops execution of thread.

 Forces the thread to stop executing.
 */
/***********************************************************************/

	void stop(void);
/************************************************************************/
/*!
\brief Detaches the thread.

 Detaches the thread.
 */
/***********************************************************************/

	void detach(void);
/************************************************************************/
/*!
\brief Returns native thread handle.

 Retrieves native thread handle if thread is started other wise returns null.
 \return native thread handle.
 */
/***********************************************************************/

	void *getNativeHandle(void);

private:
	Runnable &runnable;
	std::string name;
	void *nativeHandle;
	static void *CEntry(void * arg);
};

CCEC_OSAL_END_NAMESPACE

#endif


/** @} */
/** @} */
