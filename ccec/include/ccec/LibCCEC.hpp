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


#ifndef HDMI_CCEC_LIB_HPP_
#define HDMI_CCEC_LIB_HPP_
#include <list>
#include "osal/Mutex.hpp"
#include "ccec/CCEC.hpp"
#include "Operands.hpp"
using CCEC_OSAL::Mutex;

CCEC_BEGIN_NAMESPACE

class PhysicalAddress;

class LibCCEC {
public:
	static LibCCEC &getInstance(void);

	LibCCEC(void);
	void init(const char * name= 0);
	void term(void);
	int getLogicalAddress(int devType);
	void getPhysicalAddress(unsigned int *physicalAddress);

private:
//	int logicalAddresses;
	bool initialized;
	bool connected;
	Mutex mutex;
};

CCEC_END_NAMESPACE

#endif


/** @} */
/** @} */
