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
\brief This file defines interface of EventQueue class.

*/
/*****************************************************************************/



/**
* @defgroup hdmicec
* @{
* @defgroup osal
* @{
**/


#ifndef HDMI_CCEC_OSAL_EVENTQUEUE_HPP_
#define HDMI_CCEC_OSAL_EVENTQUEUE_HPP_

#include <stdlib.h>
#include <deque>

#include "OSAL.hpp"
#include "Mutex.hpp"
#include "ConditionVariable.hpp"

CCEC_OSAL_BEGIN_NAMESPACE

/***************************************************************************/
/*!

This is a collection class which provides the event queue functionality.
consumer threads could wait on the queue and will be signalled when 
queue is populated.

\param E - type of elements held in this collection.
*/
/**************************************************************************/

template <class E>
class EventQueue  {
public:

/***************************************************************************/
/*!
\brief Constructor.
Creates an EventQueue with the provided capacity.

\param cap - Number of elements that could be held in the queue.
*/
/**************************************************************************/

	EventQueue(size_t cap = 32) {
		this->cap = cap;
		events = new std::deque<E>(0);
	}
/***************************************************************************/
/*!

\brief Destructor
Destructor - Destroys the EventQueue object.
*/
/**************************************************************************/

	~EventQueue(void)
	{AutoLock lock_(mutex);
		if (!events->empty()) {
			printf("WARNING:  There are [%d] elements left in queue\r\n", events->size());
		}
		delete events;
	}
/***************************************************************************/
/*!
\brief method for polling events. This will block if queue is empty.

Consumer will be returned with the object from front of the queue if queue is
not empty. If queue is empty, consumer threads will wait until an event is 
posted to the queue.

If waiting thread is awakened by there is no element on queue, a default
value is returned. This allows the waiting thread to close out the queue.

\return event from the front of the queue.
*/
/**************************************************************************/

	E poll(void) {
		typedef E ET;
		E front;
		do {
			cond.wait();
			{AutoLock lock_(mutex);
				if (!cond.isSet() || events->empty()) {
					return ET();
				}

				if (cond.isSet()) {
					if (!events->empty()) {
						front = events->front();
						events->pop_front();
						if (events->empty()) {
							cond.reset();
						}
					}
					else {
						cond.reset();
					}
				} else {

				}
			}

		} while (0 && front == NULL);

		return front;
	}
	
/***************************************************************************/
/*!
\brief returns size of queue.

Retrieves number of events currently available in the queue

\return number of events in queue.
*/
/**************************************************************************/

    size_t size(void)  {
    	AutoLock lock_(mutex);
    	return events->size();
    }
	
/***************************************************************************/
/*!
\brief send an event to the queue.

Post a event to the queue and signals threads waiting on the queue.
On receiving the signal (event), if there is any consumer thread waiting
on the queue will come out of wait state and will consume the event.

\exception - if queue is full, method will throw an exception.
\param E - Object that is to be posted to the queue.
*/
/**************************************************************************/

	void offer(E element) {
    	AutoLock lock_(mutex);

    	if (events->size() == cap) {
			/* @TODO Throw Exception */
		}
		else {
			events->push_back(element);
			cond.set();
			cond.notifyAll();
		}
	}

private:
	std::deque<E> *events;
	size_t cap;
	Mutex mutex;
	ConditionVariable cond;
};

CCEC_OSAL_END_NAMESPACE

#endif


/** @} */
/** @} */
