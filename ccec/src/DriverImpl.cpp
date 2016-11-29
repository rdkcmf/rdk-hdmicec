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


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>

#include "osal/EventQueue.hpp"
#include "osal/Exception.hpp"
#include "ccec/Util.hpp"
#include "ccec/Exception.hpp"
#include "DriverImpl.hpp"

using CCEC_OSAL::AutoLock;

CCEC_BEGIN_NAMESPACE

#include "ccec/driver/hdmi_cec_driver.h"

size_t write(const unsigned char *buf, size_t len);

void DriverImpl::DriverReceiveCallback(int handle, void *callbackData, unsigned char *buf, int len)
{
	CECFrame *frame = new CECFrame();
	frame->append((unsigned char *)buf, (size_t)len);

	CCEC_LOG( LOG_DEBUG, ">>>>>>> >>>>> >>>> >> >> >\r\n");

        dump_buffer((unsigned char*)buf,len);

	CCEC_LOG(LOG_DEBUG, "==========================\r\n");

	try {
		static_cast<DriverImpl &>(Driver::getInstance()).getIncomingQueue(handle).offer(frame);
	}
	catch(...) {
		CCEC_LOG( LOG_EXP, "Exception during frame offer...discarding\r\n");
	}
	CCEC_LOG( LOG_DEBUG, "frame offered\r\n");
}

DriverImpl::DriverImpl() : status(CLOSED), nativeHandle(0)
{
	CCEC_LOG( LOG_DEBUG, "Creating DriverImpl done\r\n");
}

DriverImpl::~DriverImpl()
{
    {AutoLock lock_(mutex);
		if (status != CLOSED) {
			this->close();
		}
    }
}

void DriverImpl::open(void) throw(InvalidStateException, IOException)
{
    {AutoLock lock_(mutex);
		if (status != CLOSED) {
			#if 0
				throw InvalidStateException();
			#else
				return;
			#endif
		}

		int err = HdmiCecOpen(&nativeHandle);
		if (err !=  HDMI_CEC_IO_SUCCESS) {
			throw IOException();
		}

		HdmiCecSetRxCallback(nativeHandle, DriverReceiveCallback, 0);
		status = OPENED;
    }
}

void  DriverImpl::close(void) throw(InvalidStateException, IOException)
{

    {AutoLock lock_(mutex);
		if (status != OPENED) {
			#if 0
				throw InvalidStateException();
			#else
				return;
			#endif
		}
		status = CLOSING;

		/* Use NULL as sentinel */
		rQueue.offer(0);

		int err = HdmiCecClose(nativeHandle);
		if (err != HDMI_CEC_IO_SUCCESS) {
			throw IOException();
		}

		status = CLOSED;
    }
}

void  DriverImpl::read(CECFrame &frame)  throw(InvalidStateException, IOException)
{
    {AutoLock lock_(mutex);
		if (status != OPENED) {
			throw InvalidStateException();
		}
    }

    CCEC_LOG( LOG_DEBUG, "DriverImpl::Read()\r\n");

    bool backToPoll = false;
	do {
		backToPoll = false;

		CECFrame * inFrame = rQueue.poll();

		if (inFrame != 0) {
			frame = *inFrame;
			delete inFrame;
		}
		else {AutoLock lock_(mutex);

			if (status != OPENED) {
				/* Flush and return */
				while (rQueue.size() > 0) {
					inFrame = rQueue.poll();
					frame = *inFrame;
					delete inFrame;
				}
				throw InvalidStateException();
			}
			else {
				backToPoll = true;
			}
		}
    } while(backToPoll);
}

/*
 * Only 1 write is allowed at a time. Queue the write request and wait for response.
 */
void  DriverImpl::write(const CECFrame &frame)  throw(InvalidStateException, IOException, CECNoAckException)
{

	const uint8_t *buf = NULL;
	size_t length = 0;

	frame.getBuffer(&buf, &length);

    {AutoLock lock_(mutex);
    	if (status != OPENED) {
    		throw InvalidStateException();
    	}
		int sendResult = HDMI_CEC_IO_SUCCESS;
		CCEC_LOG( LOG_DEBUG, "DriverImpl::write to call HdmiCecTx\r\n");

		int err = HdmiCecTx(nativeHandle, buf, length, &sendResult);

		CCEC_LOG( LOG_DEBUG, ">>>>>>> >>>>> >>>> >> >> >\r\n");

		dump_buffer((unsigned char*)buf,length);

		CCEC_LOG(LOG_DEBUG, "==========================\r\n");

		CCEC_LOG( LOG_DEBUG, "DriverImpl:: call HdmiCecTx DONE %x, result %x\r\n", err, sendResult);

		if (err != HDMI_CEC_IO_SUCCESS) {
			throw IOException();
		}

        if (sendResult != HDMI_CEC_IO_SUCCESS) {
            if ((sendResult == HDMI_CEC_IO_INVALID_STATE) ||
                (sendResult == HDMI_CEC_IO_INVALID_ARGUMENT) || 
                (sendResult == HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE) || 
                (sendResult == HDMI_CEC_IO_SENT_FAILED) || 
                (sendResult == HDMI_CEC_IO_GENERAL_ERROR) )
            {
                throw IOException();
            }
        }

		if (((frame.at(0) & 0x0F) != 0x0F) && sendResult == HDMI_CEC_IO_SENT_BUT_NOT_ACKD) {
			throw CECNoAckException();
		}

		/* Send a copy to the incoming queue as echo back */
		CECFrame *frameCopy = new CECFrame(frame);
		rQueue.offer(frameCopy);
    }

    CCEC_LOG( LOG_DEBUG, "Send Completed\r\n");
}

int DriverImpl::getLogicalAddress(int devType)
{
    {AutoLock lock_(mutex);
	int logicalAddress = 0;
	CCEC_LOG( LOG_DEBUG, "DriverImpl::getLogicalAddress called for devType : %d \r\n", devType);

	HdmiCecGetLogicalAddress(nativeHandle,devType,&logicalAddress);

	CCEC_LOG( LOG_DEBUG, "DriverImpl::getLogicalAddress got logical Address : %d \r\n", logicalAddress);
	return logicalAddress;
    }
}

void DriverImpl::removeLogicalAddress(const LogicalAddress &source)
{
	int LA[15] = {0};
    {AutoLock lock_(mutex);
		if (status != OPENED) {
			throw InvalidStateException();
		}

		logicalAddresses.remove(source);
		HdmiCecRemoveLogicalAddress(nativeHandle, source.toInt());
    }
}

bool DriverImpl::addLogicalAddress(const LogicalAddress &source)
{
    {AutoLock lock_(mutex);

		if (status != OPENED) {
			throw InvalidStateException();
		}

		int retErr =  HdmiCecAddLogicalAddress(nativeHandle, source.toInt());

		if (retErr == HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE) {
			throw AddressNotAvailableException();
		}
		else if (retErr == HDMI_CEC_IO_GENERAL_ERROR) {
			throw IOException();
		}
		else {
			logicalAddresses.push_back(source);
		}
    }

    return true;
}

bool DriverImpl::isValidLogicalAddress(const LogicalAddress & source) const
{
	bool found = false;
	std::list<LogicalAddress>::const_iterator it;
	for (it = logicalAddresses.begin(); it != logicalAddresses.end(); it++) {
		if(*it == source) {
			found = true;
			break;
		}
	}

	return found;
}

void DriverImpl::poll(const LogicalAddress &from, const LogicalAddress &to)
     	 	 	 	  throw (InvalidStateException, IOException, CECNoAckException)
{
	uint8_t firstByte = (((from.toInt() & 0x0F) << 4) | (to.toInt() & 0x0F));
	CCEC_LOG( LOG_DEBUG, "$$$$$$$$$$$$$$$$$$$$ POST POLL [%s] $$$$$$$$$$$$$$$$$$$$$\r\n", from.toString().c_str());

	{
		CECFrame frame;
		frame.append(firstByte);
		write(frame);
	}
	{
		/* Send a Poll so indicate there is a device present */
		CECFrame *frame = new CECFrame();
		frame->append(firstByte);
		rQueue.offer(frame);
	}
}

DriverImpl::IncomingQueue & DriverImpl::getIncomingQueue(int nativeHandle)
{
	if (status != OPENED) {
		throw InvalidStateException();
	}

	return rQueue;
}

CCEC_END_NAMESPACE


/** @} */
/** @} */
