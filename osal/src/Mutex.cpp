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

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "safec_lib.h"
#include "osal/Mutex.hpp"
#include "osal/Util.hpp"

CCEC_OSAL_BEGIN_NAMESPACE

Mutex::Mutex(void) : nativeHandle(NULL)
{
    pthread_mutexattr_t attr;
    nativeHandle = Malloc(sizeof(pthread_mutex_t));
    pthread_mutexattr_init( &attr);
    pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init( (pthread_mutex_t *) nativeHandle, &attr );
    pthread_mutexattr_destroy(&attr);
}

Mutex::Mutex(const Mutex &rhs)
{
    nativeHandle = Malloc(sizeof(pthread_mutex_t));
    MEMCPY_S(nativeHandle,sizeof(pthread_mutex_t), rhs.nativeHandle, sizeof(pthread_mutex_t));
}

Mutex & Mutex::operator = (const Mutex &rhs)
{
	Mutex temp(rhs);
	void *tempHandle = temp.nativeHandle;
	temp.nativeHandle = nativeHandle;
	nativeHandle = tempHandle;

	return *this;
}

Mutex::~Mutex(void)
{
	if (nativeHandle != NULL) {
		pthread_mutex_destroy((pthread_mutex_t *) nativeHandle);
		Free(nativeHandle);
	}
}

void Mutex::lock(void)
{
	pthread_mutex_lock((pthread_mutex_t *)nativeHandle);
}

void Mutex::unlock(void)
{
	pthread_mutex_unlock((pthread_mutex_t *)nativeHandle);
}

void * Mutex::getNativeHandle(void)
{
	return nativeHandle;
}

CCEC_OSAL_END_NAMESPACE


/** @} */
/** @} */
