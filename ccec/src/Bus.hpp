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
* @defgroup ccec
* @{
**/


#ifndef _HDMI_CCEC_BUS_HPP_
#define _HDMI_CCEC_BUS_HPP_

#include <list>

#include "osal/Mutex.hpp"
#include "osal/Runnable.hpp"
#include "osal/Stoppable.hpp"
#include "osal/Thread.hpp"
#include "osal/EventQueue.hpp"

#include "ccec/CCEC.hpp"

using CCEC_OSAL::Runnable;
using CCEC_OSAL::Stoppable;
using CCEC_OSAL::EventQueue;
using CCEC_OSAL::Mutex;

CCEC_BEGIN_NAMESPACE

class CECFrame;
class FrameListener;

class Bus {
public:
	static Bus & getInstance(void);
    void addFrameListener(FrameListener *listener);
    void removeFrameListener(FrameListener *listener);
    void send(const CECFrame &frame, int timeout = 0);
    void sendAsync(const CECFrame &frame);

	void start(void);
	void stop(void);

private:
    class Reader : public Runnable, public Stoppable {
    public:
    	Reader(Bus &bus) : bus(bus) {}
    	void run(void);
    	void stop(bool block = true);
    private:
    	Bus &bus;
    } reader;

    class Writer : public Runnable, public Stoppable {
    public:
    	Writer(Bus &bus) : bus(bus) {}
    	void run(void);
    	void stop(bool block = true);
    private:
    	Bus &bus;
    } writer;

	Bus(void);
	Bus(const Bus &); /* Not allowed */
	Bus & operator = (const Bus &);  /* Not allowed */
	~Bus(void);

private:
	std::list<FrameListener *> listeners;
	Mutex rMutex;
	Mutex wMutex;
	EventQueue<CECFrame * > wQueue;
	volatile bool started;
};

CCEC_END_NAMESPACE


#endif


/** @} */
/** @} */
