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

/**
 * @defgroup hdmi_bus HDMI-CEC BUS
 * @ingroup HDMI_CEC
 * @addtogroup hdmi_bus
 * @{
 */


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

/**
 * @brief This function is used to create the instance of Bus class.
 *
 * @return instance Instance of the Bus class.
 */
Bus & Bus::getInstance(void)
{
	static Bus instance;
	return instance;
}

/**
 * @brief This function is a constructor for the class Bus. It is used to starts
 * the read and write thread which creates its instance.
 *
 * @return None
 */
Bus::Bus(void) : reader(*this), writer(*this)
{
	CCEC_LOG( LOG_DEBUG, "Bus Instance Created\r\n");
	Thread(this->reader).start();
	Thread(this->writer).start();
	CCEC_LOG( LOG_DEBUG, "Bus Instance DONE\r\n");
}

/**
 * @brief This function starts the threads and gets the instance for Bus.
 *
 * @return None
 */
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

/**
 * @brief This function stops the reader & writer threads and removes the
 * instance for Bus.
 *
 * @return None
 */
void Bus::stop(void)
{AutoLock rlock_(rMutex), wlock_(wMutex);

CCEC_LOG( LOG_INFO, "Bus::stop is called\r\n");

	started = false;
	reader.stop(false);
	writer.stop(false);

	Driver::getInstance().close();
}

/**
 * @brief This is a destructor for class BUS. This function initiates the closing
 * of threads and the instance for Bus will be freed.
 *
 * @return None
 */
Bus::~Bus(void)
{
	Assert(!started);

	reader.stop(true);
	writer.stop(true);

	CCEC_LOG( LOG_DEBUG, "Bus::Destroyed\r\n");

}

/**
 * @brief This function is used to read CECFrame from the driver. This gets
 * notified to the frameListener which is listening for frames in CEC bus.
 *
 * @return None
 */
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

/**
 * @brief This function is used to stop the reader to read frames from the bus.
 *
 * @param[in] block State of Reader.
 *
 * @return None
 */
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

/**
 * @brief This function is used to add new listener for reading frames.
 *
 * @param[in] listener Struct pointer for the addition of listener.
 *
 * @return None
 */
void Bus::addFrameListener(FrameListener *listener)
{
	{AutoLock lock_(rMutex);
		if (!started) throw InvalidStateException();
		listeners.push_back(listener);
	}
}

/**
 * @brief This function is used to remove the listener.
 *
 * @param[in] listener Struct pointer of a listener which is to be removed.
 *
 * @return None
 */
void Bus::removeFrameListener(FrameListener *listener)
{
	{ AutoLock lock_(rMutex);
	if (!started) throw InvalidStateException();
		listeners.remove(listener);
	}

}

/**
 * @brief This function is used to poll the bus for frame availability and it
 * writes the CEC frame to the driver.
 *
 * @return None
 */
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

/**
 * @brief This function is used to stop the writer for polling the bus and writing
 * to the driver.
 *
 * @param[in] block State of Writer.
 *
 * @return None
 */
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

/**
 * @brief This function is used to write the frame to the driver. If it fails,
 * as it is a synchronous function, it retries every 250ms till the retry count lapse.
 *
 * @param[in] frame CEC frame to be sent.
 * @param[in] timeout Time period for retrying.
 *
 * @return None
 */
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

/**
 * @brief This function is used to keep asynchronously sending the frame by
 * keeping copy of cec frame in the queue of the driver.
 *
 * @param[in] frame CEC frame which need to be sent asynchronously.
 *
 * @return None
 */
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
/** @} */
