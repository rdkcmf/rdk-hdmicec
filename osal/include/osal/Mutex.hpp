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
\brief This file defines interface of Mutex class.

*/
/*****************************************************************************/


/**
* @defgroup hdmicec
* @{
* @defgroup osal
* @{
**/


#ifndef HDMI_CCEC_OSAL_MUTEX_HPP_
#define HDMI_CCEC_OSAL_MUTEX_HPP_

#include "OSAL.hpp"

CCEC_OSAL_BEGIN_NAMESPACE

/***************************************************************************/
/*!

This class provides synchronization primitive for implementing critical sections
and protect shared data from concurrent access. This class provides functionaliy
of a recurisive (same thread can take the lock multiple times) mutual exclusive lock.
*/
/**************************************************************************/

class Mutex {
public:
/***************************************************************************/
/*!
\brief Constructor.
Creates Mutex object. 

*/
/**************************************************************************/

	Mutex(void);
	Mutex(const Mutex &);
	Mutex & operator = (const Mutex &);

/***************************************************************************/
/*!
\brief Destructor 
Destroys the Mutex object. 

*/
/**************************************************************************/

	~Mutex(void);

/***************************************************************************/
/*!
\brief Locks the given mutex 

Calling thread will acquire the mutex lock if available and will immediately
return. Subsequent call by other threads before current thread releasing the
mutex will result in those threads to block. 

\note Current thread can call lock() for same object multiple times before 
releasing the mutex. But for releasing the lock to other threads, same number
of unlock calls shall be made by the locking thread.

*/
/**************************************************************************/

	void lock(void);
/***************************************************************************/
/*!
\brief Unlocks the given mutex 

Called by a thread which already acquired lock to release the lock acquired.
If there are other threads waiting on the lock, one of those waiting threads 
will be able to acquire the lock released and will come out of wait state.

*/
/**************************************************************************/


	void unlock(void);

/***************************************************************************/
/*!
\brief Retrieves handle to the underlying native mutex implementation.
 

*/
/**************************************************************************/


	void *getNativeHandle(void);
private:
	void *nativeHandle;
};

class AutoLock
{
public:
	AutoLock(Mutex & mutex) : mutex(mutex) {
		mutex.lock();
	}

	~AutoLock(void) {
		mutex.unlock();
	}
	Mutex &mutex;
};

CCEC_OSAL_END_NAMESPACE

#endif


/** @} */
/** @} */
