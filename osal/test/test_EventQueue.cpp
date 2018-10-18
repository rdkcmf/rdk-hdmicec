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
#include <stdio.h>
#include <unistd.h>

#include "osal/Thread.hpp"
#include "osal/Runnable.hpp"
#include "osal/EventQueue.hpp"

using namespace CCEC_OSAL;

class Element {
public:
	Element(int i) {
		printf("at [%s][%d\r\n", __FUNCTION__, __LINE__);
		value = i;
	}
	int value;
};

static EventQueue<Element *> *intQueue;

class producer: public Runnable {
    void run(void) {
        while(1) {
            //sleep(2);
            Element *p = intQueue->poll();
            printf("Hello Producer got %d\r\n", p->value);
            delete p;
        }
    }
};

class consumer: public Runnable {
    void run(void) {
        int i = 1000;
        while(i < 2000) {
            i++;
            printf("Hello Consumer sent %d\r\n", i);
            //sleep (2);
            intQueue->offer(new Element(i));
        }
    }
};

int main() 
{
    intQueue = new EventQueue<Element *>();
    consumer *con;
    producer *pro;
    Thread(*(new consumer())).start();
    Thread(*(new producer())).start();
    sleep(10);
    delete intQueue;
}


/** @} */
/** @} */
