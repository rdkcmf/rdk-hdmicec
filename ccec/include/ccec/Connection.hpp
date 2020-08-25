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


#ifndef HDMI_CCEC_CONNECTION_HPP_
#define HDMI_CCEC_CONNECTION_HPP_

#include <stdlib.h>
#include <list>

#include "osal/Mutex.hpp"

#include "ccec/CCEC.hpp"
#include "ccec/FrameListener.hpp"
#include "ccec/Operands.hpp"
#include "ccec/Driver.hpp"
#include "ccec/LibCCEC.hpp"
#include "ccec/Exception.hpp"

using CCEC_OSAL::Mutex;

CCEC_BEGIN_NAMESPACE
class Bus;
class CECFrame;

/**
 * @brief The connection class provides APIs that allows the application to access CEC Bus.
 * A connection is a tap into the CEC bus. The application can use a connection to send raw bytes
 * (in form of CECFrame) onto CEC bus or receive raw bytes from it.
 * @ingroup HDMI_CEC_CONNECTION
 */
class Connection
{
public:
	Connection(const LogicalAddress &source = LogicalAddress::UNREGISTERED, bool opened = true, const std::string &name="");
	virtual ~Connection(void);

	void open(void);
	void close(void);

    void addFrameListener(FrameListener *listener);
    void removeFrameListener(FrameListener *listener);

	void send(const CECFrame &frame, int timeout, const Throw_e &doThrow);
	void sendTo(const LogicalAddress &to, const CECFrame &frame, int timeout, const Throw_e &doThrow);
	void send(const CECFrame &frame, int timeout = 0);
	void sendTo(const LogicalAddress &to, const CECFrame &frame, int timeout = 0);
	void sendToAsync(const LogicalAddress &to, const CECFrame &frame);
	void poll(const LogicalAddress &from, const Throw_e &doThrow);
	void ping(const LogicalAddress &from, const LogicalAddress &to, const Throw_e &doThrow);
		
	void sendAsync(const CECFrame &frame);

	const LogicalAddress & getSource(void) {
		return source;
	}
	
	const void setSource(LogicalAddress &from) {
		source = from;
	}

private:
	class DefaultFilter : public FrameFilter {
	public:
		DefaultFilter(LogicalAddress &source) : source(source) {}
		bool isFiltered(const CECFrame &frame);
    private:
	    LogicalAddress source;

	};

    class DefaultFrameListener : public FrameListener {
    public:
    	DefaultFrameListener(Connection &connection, FrameFilter &filter) : connection(connection), filter(filter) {
        }
    	void notify(const CECFrame &frame) const;
    private:
    	Connection &connection;
    	FrameFilter &filter;
    };

    void matchSource(const CECFrame &frame);
    std::string name;
    LogicalAddress source;
    Bus &bus;
    DefaultFilter busFrameFilter;
    DefaultFrameListener busFrameListener;
	std::list<FrameListener *> frameListeners;
	Mutex mutex;
};

CCEC_END_NAMESPACE

#endif


/** @} */
/** @} */
