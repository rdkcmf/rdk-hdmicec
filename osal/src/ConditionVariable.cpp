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


#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

#include "osal/ConditionVariable.hpp"
#include "osal/Condition.hpp"
#include "osal/Mutex.hpp"
#include "osal/Util.hpp"

CCEC_OSAL_BEGIN_NAMESPACE

ConditionVariable::ConditionVariable() : cond(NULL), mutex(NULL), nativeHandle(NULL)
{
	cond = new  Condition(false);
    mutex = new Mutex();
    nativeHandle = Malloc(sizeof(pthread_cond_t));
    pthread_cond_init( (pthread_cond_t *) nativeHandle, NULL );
}

ConditionVariable::~ConditionVariable()
{
	if (nativeHandle != NULL) {
		pthread_cond_destroy((pthread_cond_t *) nativeHandle);
		Free(nativeHandle);
	}
	delete mutex;
	delete cond;
}

void ConditionVariable::set(void)
{
	mutex->lock();
	cond->set();
	mutex->unlock();
}

void ConditionVariable::reset(void)
{
	mutex->lock();
	cond->reset();
	mutex->unlock();
}

bool ConditionVariable::isSet(void)
{
	mutex->lock();
	bool set = cond->isSet();
	mutex->unlock();
	return set;
}

void ConditionVariable::wait(void)
{
	wait(0);
}

long ConditionVariable::wait(long timeout)
{
	long timeLeft = 1;
	mutex->lock();
	if (!cond->isSet()) {
		int ret = 0;
		if (timeout == 0) {
			ret = pthread_cond_wait((pthread_cond_t *)nativeHandle,
								    (pthread_mutex_t *)mutex->getNativeHandle());
			if (ret < 0) {
				/* @TODO Throw Exception */
			}
		}
		else {
            struct timeval  curTime;
            memset(&curTime, 0, sizeof(curTime));
            gettimeofday(&curTime, NULL);

            struct timespec wakeTime;
            memset(&wakeTime, 0, sizeof(wakeTime));
            wakeTime.tv_nsec = curTime.tv_usec * 1000 + (timeout % 1000) * 1000000;
            wakeTime.tv_sec = curTime.tv_sec + (timeout / 1000);
            if (wakeTime.tv_nsec > 1000000000)
            {
            	wakeTime.tv_nsec -= 1000000000;
            	wakeTime.tv_sec++;
            }

            ret = pthread_cond_timedwait((pthread_cond_t *)nativeHandle,
            							 (pthread_mutex_t *)mutex->getNativeHandle(),
            							 &wakeTime);

            if ((ret != 0) && !cond->isSet() && ret == ETIMEDOUT) {
            	timeLeft = 0;
            }
            else {
            	/* @TODO: Handle wait interruption. Return actual left over time */
            	timeLeft =  1;
            }

		}
    }
	mutex->unlock();

	return timeLeft;
}

void ConditionVariable::notify(void)
{
	mutex->lock();
	cond->set();
	pthread_cond_signal((pthread_cond_t *)nativeHandle);
	mutex->unlock();
}

void ConditionVariable::notifyAll(void)
{
	mutex->lock();
	cond->set();
	pthread_cond_broadcast((pthread_cond_t *)nativeHandle);
	mutex->unlock();
}

void * ConditionVariable::getNativeHandle(void)
{
	return nativeHandle;
}

CCEC_OSAL_END_NAMESPACE


/** @} */
/** @} */
