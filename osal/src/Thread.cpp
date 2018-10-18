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


#include <stdlib.h>
#include <pthread.h>

#include "osal/Runnable.hpp"
#include "osal/Thread.hpp"

CCEC_OSAL_BEGIN_NAMESPACE

void *Thread::CEntry(void * arg)
{
	Runnable *target = static_cast<Runnable *>(arg);
	target->run();
	return NULL;
}

Thread::Thread(Runnable &target) : runnable(target)
{
}

Thread::Thread(Runnable &target, const int8_t* name) : runnable(target), name((const char *)name), nativeHandle(0)
{
}

Thread::~Thread(void)
{
}

void Thread::run(void)
{
	runnable.run();
}

void Thread::start()
{
	pthread_t tid;
	pthread_attr_t attr;
	int ret = 0;
    (void) pthread_attr_init(&attr);
    (void) pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    (void) pthread_attr_setschedpolicy(&attr, SCHED_OTHER);

    /*@TODO: Set priority */
	ret = pthread_create(&tid, &attr, Thread::CEntry, static_cast<void *>(&runnable));
	pthread_attr_destroy(&attr);

	if (ret != 0) {
		/*@TODO: Throw Exception */
	}
	else {
		nativeHandle = (void *)tid;
	}
}

void Thread::detach(void)
{
	pthread_detach((pthread_t)nativeHandle);
}

CCEC_OSAL_END_NAMESPACE


/** @} */
/** @} */
