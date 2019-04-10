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


#ifndef _HDMI_CCEC_DRIVER_HPP_
#define _HDMI_CCEC_DRIVER_HPP_

#include <queue>
#include <list>

#include "osal/Mutex.hpp"
#include "ccec/Exception.hpp"
#include "CECFrame.hpp"
#include "Operands.hpp"

using CCEC_OSAL::Mutex;

CCEC_BEGIN_NAMESPACE

class Driver {
public:
	static Driver &getInstance(void);

	enum  {
		SENT_AND_ACKD,     //On the bus and destination device ack'd
		SENT_FAILED = 1,   //Not getting on the bus.
		SENT_BUT_NOT_ACKD, //On the bus but no destination device.
	};


	Driver(void) {};

	virtual void  open(void) throw(InvalidStateException, IOException) = 0;
	virtual void  close(void) throw(InvalidStateException, IOException)= 0;
	virtual void  read(CECFrame &frame)  throw(InvalidStateException, IOException) = 0;
	virtual void  write(const CECFrame &frame) throw(InvalidStateException, IOException, CECNoAckException) = 0;
	virtual void  removeLogicalAddress(const LogicalAddress &source)  = 0;
	virtual bool  addLogicalAddress   (const LogicalAddress &source) = 0;
//	virtual void  getLogicalAddress(int devType, int *logicalAddress) = 0;
	virtual int  getLogicalAddress(int devType) = 0;
	virtual void getPhysicalAddress(unsigned int *physicalAddress) = 0;
	virtual bool isValidLogicalAddress(const LogicalAddress &source) const = 0;
	virtual void poll(const LogicalAddress &from, const LogicalAddress &to) throw(InvalidStateException, IOException, CECNoAckException) = 0;

	virtual ~Driver(void) {};
protected:

private:
	static Mutex instanceMutex;
};

CCEC_END_NAMESPACE
#endif


/** @} */
/** @} */
