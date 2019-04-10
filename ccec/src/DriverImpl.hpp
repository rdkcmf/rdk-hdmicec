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


#ifndef HDMI_CCEC_DRIVER_IMPL_HPP_
#define HDMI_CCEC_DRIVER_IMPL_HPP_

#include <list>

#include "osal/Mutex.hpp"
#include "osal/EventQueue.hpp"

#include "osal/ConditionVariable.hpp"
#include "ccec/Driver.hpp"

using CCEC_OSAL::EventQueue;
using CCEC_OSAL::Mutex;

CCEC_BEGIN_NAMESPACE

class IncomingQueue;

class DriverImpl : public Driver
{
public:
	static void DriverReceiveCallback(int handle, void *callbackData, unsigned char *buf, int len);
	typedef EventQueue<CECFrame *> IncomingQueue;

	enum {
		CLOSED = 0,
		CLOSING,
		OPENED,
	};
	DriverImpl(void);
	virtual ~DriverImpl();

	virtual void  open(void) throw(InvalidStateException, IOException);
	virtual void  close(void) throw(InvalidStateException, IOException);
	virtual void  read(CECFrame &frame)  throw(InvalidStateException, IOException);
	virtual void  write(const CECFrame &frame) throw(InvalidStateException, IOException, CECNoAckException);
	virtual void  removeLogicalAddress(const LogicalAddress &source);
	virtual bool  addLogicalAddress   (const LogicalAddress &source);
	virtual int   getLogicalAddress(int devType);
	virtual void  getPhysicalAddress(unsigned int *physicalAddress);
//	virtual const std::list<LogicalAddress> & getLogicalAddresses(void);
	virtual bool isValidLogicalAddress(const LogicalAddress &source) const;
	virtual void poll(const LogicalAddress &from, const LogicalAddress &to) throw(InvalidStateException, IOException, CECNoAckException);

private:
	IncomingQueue & getIncomingQueue(int nativeHandle);

	int status;
	int nativeHandle;
	IncomingQueue rQueue;
	Mutex mutex;
	std::list<LogicalAddress> logicalAddresses;

	DriverImpl(const DriverImpl &); /* Not allowed */
	DriverImpl & operator = (const DriverImpl &); /* Not allowed */

};

CCEC_END_NAMESPACE

#endif


/** @} */
/** @} */
