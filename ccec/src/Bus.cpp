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


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "ccec/CECFrame.hpp"
#include "ccec/FrameListener.hpp"
#include "ccec/Driver.hpp"
#include "ccec/Exception.hpp"
#include "ccec/Util.hpp"

#include "Bus.hpp"

using CCEC_OSAL::AutoLock;
using CCEC_OSAL::Thread;

CCEC_BEGIN_NAMESPACE

Bus & Bus::getInstance(void)
{
	static Bus instance;
	return instance;
}

Bus::Bus(void) : reader(*this), writer(*this)
{
	CCEC_LOG( LOG_DEBUG, "Bus Instance Created\r\n");
	Thread(this->reader).start();
	Thread(this->writer).start();
	CCEC_LOG( LOG_DEBUG, "Bus Instance DONE\r\n");
}

void Bus::start(void)
{AutoLock rlock_(rMutex), wlock_(wMutex);
	CCEC_LOG( LOG_INFO, "Bus::start is called \r\n");

        if(reader.isStopped())
        {
             Thread(reader).start();
        }
        if(writer.isStopped())
        {
             Thread(writer).start();
        }

	Driver::getInstance().open();
	started = true;
}

void Bus::stop(void)
{AutoLock rlock_(rMutex), wlock_(wMutex);

CCEC_LOG( LOG_INFO, "Bus::stop is called\r\n");

	started = false;
	reader.stop(false);
	writer.stop(false);

	Driver::getInstance().close();
}

Bus::~Bus(void)
{
	Assert(!started);

	reader.stop(true);
	writer.stop(true);

	CCEC_LOG( LOG_DEBUG, "Bus::Destroyed\r\n");

}

void Bus::Reader::run(void)
{
	CECFrame frame;

        if(isStopped())
        {
             runStarted();
        }

	CCEC_LOG( LOG_INFO, "Bus::Reader::run() started\r\n");
	while (isRunning()) {
		try {
			Driver::getInstance().read(frame);
			{AutoLock lock_(bus.rMutex);
			    if (bus.listeners.size() == 0) CCEC_LOG( LOG_DEBUG, "Bus::Reader discarding msgs for lack of listener\r\n");
				std::list<FrameListener *>::iterator list_it;
				for(list_it = bus.listeners.begin(); list_it!= bus.listeners.end(); list_it++) {
					CCEC_LOG( LOG_DEBUG, "Bus::Reader::run() notify Listener\r\n");
					(*list_it)->notify(frame);
					//CCEC_LOG( LOG_DEBUG, "Bus::Reader::run() notify Listener Done\r\n");
				}
			}
		}
		catch(InvalidStateException &e) {
			CCEC_LOG( LOG_DEBUG, "EOF for reader [%d]\r\n", isRunning());
		}
	}

	stopCompleted();
}

void Bus::Reader::stop(bool block)
{
	CCEC_LOG( LOG_INFO, "Bus::Reader::stop::stop Entering [%d]\r\n", block);

	{AutoLock lock_(bus.rMutex);
		stopStarted();
	}

	Driver::getInstance().close();

	if (block) {
		while (!isStopped()) {
			usleep(50 * 1000);
		}

		CCEC_LOG( LOG_DEBUG, "Bus::Reader::stop::stop completed\r\n");
	}
}

void Bus::addFrameListener(FrameListener *listener)
{
	{AutoLock lock_(rMutex);
		if (!started) throw InvalidStateException();
		listeners.push_back(listener);
	}
}

void Bus::removeFrameListener(FrameListener *listener)
{
	{ AutoLock lock_(rMutex);
	if (!started) throw InvalidStateException();
		listeners.remove(listener);
	}

}

void Bus::Writer::run(void)
{
	CECFrame frame;
	//Driver::getInstance()->open();
	CCEC_LOG( LOG_INFO, "Bus::Writer::run() started\r\n");
	CECFrame * outFrame = NULL;

        if(isStopped())
        {
             runStarted();
        }

	do {
		CCEC_LOG( LOG_DEBUG, "Bus::Writer::run Looping [%d]\r\n", isRunning());

		try {
			outFrame = bus.wQueue.poll();
			if (outFrame != 0) {
				Driver::getInstance().write(*outFrame);
			}
			else {
				CCEC_LOG( LOG_DEBUG, "Bus::Writer::run EOF [%d]\r\n", isRunning());
				/* sentinel value */
				throw InvalidStateException();
			}
		}
		catch(InvalidStateException &e) {
			CCEC_LOG( LOG_EXP, "Driver closed writer[%d]\r\n", isRunning());
		}
		catch(Exception &e) {
			CCEC_LOG( LOG_EXP,"Driver write failed\r\n");
			e.what();
		}

		delete outFrame;
	}

	while (isRunning());

	if (!isRunning()) {
		while(bus.wQueue.size() > 0) {
			outFrame = bus.wQueue.poll();
			delete outFrame;
		}
	}

	stopCompleted();
}

void Bus::Writer::stop(bool block)
{
	CCEC_LOG( LOG_INFO, "Bus::Writer::stop::stop Entering [%d]\r\n", block);

	{AutoLock lock_(bus.wMutex);
		if (isRunning()) {
			stopStarted();
			bus.wQueue.offer(0);
			CCEC_LOG( LOG_DEBUG, "Bus::Writer::stop::stop offer completed [%d]\r\n", isRunning());
		}
	}

	if (block) {
		while (!isStopped()) {
			usleep(50 * 1000);
		}

		CCEC_LOG( LOG_DEBUG, "Bus::Writer::stop::stop completed\r\n");
	}
}

void Bus::send(const CECFrame &frame, int timeout)
{
	{AutoLock rlock_(rMutex), wlock_(wMutex);

        if (timeout <= 0) {
            if (!started) throw InvalidStateException();

            try {
                Driver::getInstance().write(frame);
                CCEC_LOG( LOG_DEBUG, "Bus::send write done\r\n");
            }
            catch (Exception &e){
                CCEC_LOG( LOG_EXP, "Bus::send exp caught [%s] \r\n", e.what());
                throw;
            }
        }
    }

	{AutoLock rlock_(rMutex), wlock_(wMutex);
        if (timeout > 0) {
            /* Retry in 250ms increment till timeout */
            int retry = (timeout / 250);
            do {
                try {
                    if (!started) throw InvalidStateException();
                    send(frame, 0);
                    retry = 0;
                }
                catch (Exception &e){
                    CCEC_LOG( LOG_EXP, "Bus::send exp caught [%s], retry [%d]\r\n", e.what(), retry);
                    if (retry == 0) {
                        throw;
                    }
                    usleep(250000);
                }
            } while (retry--);
        }
	}
}

void Bus::sendAsync(const CECFrame &frame)
{
	{AutoLock lock_(wMutex);

		if (!started) throw InvalidStateException();

		CECFrame *copyFrame = (new CECFrame());
		*copyFrame = frame;

		wQueue.offer((copyFrame));
	}
}

CCEC_END_NAMESPACE


/** @} */
/** @} */
