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

#include "ccec/CECFrame.hpp"
#include "ccec/FrameListener.hpp"
#include "ccec/Connection.hpp"
#include "ccec/Util.hpp"
#include "ccec/Header.hpp"

#include "Bus.hpp"

using CCEC_OSAL::AutoLock;

CCEC_BEGIN_NAMESPACE

Connection::Connection(const LogicalAddress &source, bool opened, const std::string &name)
: name(name), source(source), bus(Bus::getInstance()), busFrameFilter(this->source), busFrameListener(*this, busFrameFilter)
{
	if (opened) open();
}

Connection::~Connection(void) {

}

/**
 * @brief Open a connection to receive CEC packets from the bus.
 *
 * When a connection is opened without a given logical address this connection will pick up all messages destined
 * to the host device, regardless what roles the device has. This is useful if the application wants to sniff all available
 * CEC packets from the bus.
 *
 * @return None.
 */
void Connection::open(void)
{
	CCEC_LOG( LOG_DEBUG, "Connection::open with source [%s]\r\n", source.toString().c_str());
	bus.addFrameListener(&busFrameListener);
}

/**
 * brief Close the CEC connection from bus so that the application will stop receiving CEC packets from the bus.
 *
 * @return None.
 */
void Connection::close(void)
{
	{AutoLock lock_(mutex);
		frameListeners.clear();
	}
	bus.removeFrameListener(&busFrameListener);
}

/**
 * @brief This function is used to listen for CECFrame, which is a byte stream that contains raw bytes received from CEC bus.
 *
 * @param[in] listener A listener structure which need be added in the frameListeners Queue.
 *
 * @return None.
 */
void Connection::addFrameListener(FrameListener *listener)
{
	{AutoLock lock_(mutex);
		frameListeners.push_back(listener);
	}
	CCEC_LOG( LOG_DEBUG, "Connection::addFrameListener::done\r\n");

}

/**
 * @brief This function is used to remove the listener information from the queue.
 *
 * @param[in] listener Address of the pointer which need to be removed from the Queue.
 *
 * @return None.
 */
void Connection::removeFrameListener(FrameListener *listener)
{
	{ AutoLock lock_(mutex);
	    frameListeners.remove(listener);
	}

}

/**
 * @brief This function is used to send CEC frame to CEC Bus.
 *
 * @param[in] to Logical address of the connection where CEC frame can be sent.
 * @param[in] frame CEC Frame which is a byte stream that contains raw bytes.
 * @param[in] timeout It is an upper bound on the amount of time to wait so that the application will not hang during sending.
 *
 * @return None.
 */
void Connection::sendTo(const LogicalAddress &to, const CECFrame &frame, int timeout)
{
	CCEC_LOG( LOG_DEBUG, "Sending out from Connection\r\n");
	CECFrame fullFrame;
	Header header(source, to);
	header.serialize(fullFrame);
	fullFrame.append(frame);
	send(fullFrame, timeout);
}

/**
 * @brief This function is used to send CEC frame to CEC Bus.
 *
 * @param[in] to Logical address of the connection where CEC frame can be sent.
 * @param[in] frame CEC Frame which is a byte stream that contains raw bytes.
 * @param[in] timeout It is an upper bound on the amount of time to wait so that the application will not hang during sending.
 * @param[in] doThrow Throw an exception if the CEC frame is unable to send.
 *
 * @return None.
 */
void Connection::sendTo(const LogicalAddress &to, const CECFrame &frame, int timeout, const Throw_e &doThrow)
{
	CCEC_LOG( LOG_DEBUG, "Sending out from Connection\r\n");
	CECFrame fullFrame;
	Header header(source, to);
	header.serialize(fullFrame);
	fullFrame.append(frame);
	try
	{
		send(fullFrame, timeout, doThrow);
	}
	catch(Exception &e)
	{
		//CCEC_LOG( LOG_EXP, "Catch and re-throw Exception from sendTo\r\n");
                throw;
	}
}

/**
 * @brief sends HDMI-CEC frame to CEC Bus using asynchronized method.
 *
 * @param[in] to Logical address of the connection where CEC frame can be sent.
 * @param[in] frame CEC Frame which is a byte stream that contains raw bytes.
 *
 * @return None.
 */
void Connection::sendToAsync(const LogicalAddress &to, const CECFrame &frame)
{
	CECFrame fullFrame;
	Header header(source, to);
	header.serialize(fullFrame);
	fullFrame.append(frame);
	sendAsync(fullFrame);
}

/**
 * @brief This function is used to send CEC frame to CEC Bus.
 *
 * @param[in] frame CEC Frame which is a byte stream that contains raw bytes.
 * @param[in] timeout It is an upper bound on the amount of time to wait so that the application will not hang during sending.
 *
 * @return None.
 */
void Connection::send(const CECFrame &frame, int timeout)
{
	CCEC_LOG( LOG_DEBUG, "Sending out from Connection with timeout %d ms\r\n", timeout);
	//@TODO: Need to enforce frame's source == connection.source?
	matchSource(frame);
        try
        {
		bus.send(frame, timeout);
        }
	catch(Exception &e)
        {
		//CCEC_LOG( LOG_EXP, "Capturing Exception in send and Not sending upwards :: %s\r\n",e.what());
        }
}

/**
 * @brief This function is used to send CEC frame to CEC Bus.
 *
 * @param[in] frame CEC Frame which is a byte stream that contains raw bytes.
 * @param[in] timeout It is an upper bound on the amount of time to wait so that the application will not hang during sending.
 * @param[in] doThrow Throw an exception if the CEC frame is unable to send.
 *
 * @return None.
 */
void Connection::send(const CECFrame &frame, int timeout, const Throw_e &doThrow)
{
	CCEC_LOG( LOG_DEBUG, "Sending out from Connection\r\n");
	//@TODO: Need to enforce frame's source == connection.source?
	matchSource(frame);
        try
	{
		bus.send(frame, timeout);
        }
	catch(Exception &e)
        {
		//CCEC_LOG(LOG_EXP, "Catch and re-throw Exception from send\r\n");
		throw;
	}
}

/**
 * @brief This function is used to send the CEC frame to physical CEC Bus using asynchronous method.
 *
 * @param[in] frame CEC Frame which is a byte stream that contains raw bytes.
 *
 * @return None.
 */
void Connection::sendAsync(const CECFrame &frame)
{
	CCEC_LOG( LOG_DEBUG, "Sending out from Connection\r\n");
	matchSource(frame);
	bus.sendAsync(frame);
}

/**
 * @brief Check the filtered is set for this connection.
 *
 * All incoming CECFrame arrived at the Physical CEC bus will be dispatched to all opened connections,
 * based on the filtering criteria of the connection.
 *
 * @param[in] frame CEC Frame which is a byte stream that contains raw bytes.
 *
 * @return TRUE if the filter is set for the CEC Connection, otherwise return FALSE.
 */
bool Connection::DefaultFilter::isFiltered(const CECFrame &frame)
{
	bool filtered = true;

	if (source == LogicalAddress(LogicalAddress::UNREGISTERED)) {
		filtered = false;
	}
	else {
		Header header(frame);
		if ((header.to == LogicalAddress(LogicalAddress::BROADCAST)) ||
			(header.to == source)) {
			filtered = false;
		}
		else {
			filtered = true;
		}
	}

	return filtered;
}

/**
 * @brief Notify to the application if CECFrame is received. The CEC frame contains the raw bytes.
 *
 * @param[in] frame CEC Frame which is a byte stream that contains raw bytes.
 *
 * @return None.
 */
void Connection::DefaultFrameListener::notify(const CECFrame &frame) const
{
	{AutoLock lock_(connection.mutex);
		if (!filter.isFiltered(frame)) {
			std::list<FrameListener *>::iterator list_it;
			for(list_it = connection.frameListeners.begin(); list_it!= connection.frameListeners.end(); list_it++) {
				CCEC_LOG( LOG_DEBUG, "connection [%s] frame Listeners notify Listener\r\n", connection.name.c_str());
				(*list_it)->notify(frame);
			}
		}
		else {
			Header header(frame);
		}
	}
}

/**
 * @brief Match the source address and update the logical address for the Connection.
 *
 * @param[in] frame CEC Frame which is a byte stream that contains raw bytes.
 *
 * @return None.
 */
void Connection::matchSource(const CECFrame &frame)
{
	//@TODO: use Header.from/to to do filtering, instead of raw bytes
	if (Driver::getInstance().isValidLogicalAddress(source)) {
		CCEC_LOG( LOG_ERROR, "Mathing source to %s\r\n", source.toString().c_str());

		const uint8_t *buf = frame.getBuffer();
		CCEC_LOG( LOG_DEBUG, "Has source to %x\r\n", (buf[0] >> 4) & 0x0F);

		if (((buf[0] >> 4) & 0x0F) != (source.toInt() & 0x0F)) {
			CCEC_LOG( LOG_ERROR, "Altering Initiator to match connection source\r\n");
			((uint8_t *)buf)[0] = ((buf[0] & 0x0F) | ((source.toInt() & 0x0F) << 4));
		}
	}
}

CCEC_END_NAMESPACE


/** @} */
/** @} */
