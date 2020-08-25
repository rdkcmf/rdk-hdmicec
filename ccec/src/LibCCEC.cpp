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
 * @defgroup hdmi_ccec HDMI-CEC CCEC
 * @ingroup HDMI_CEC
 * @ingroup hdmi_ccec
 * @{
 */


#include <string.h>
#include <iostream>

#include "osal/Thread.hpp"
#include "osal/Mutex.hpp"

#include "ccec/LibCCEC.hpp"
#include "ccec/Exception.hpp"

#include "ccec/Driver.hpp"
#include "Bus.hpp"

using CCEC_OSAL::AutoLock;
using CCEC_OSAL::Thread;

CCEC_BEGIN_NAMESPACE

extern char _CEC_LOG_PREFIX[64];

/**
 * @brief This function is used to create the instance for CEC.
 *
 * @return libCCEC Instance of CEC.
 */
LibCCEC &LibCCEC::getInstance()
{
	static LibCCEC libCCEC;
	return libCCEC;
}

/**
 * @brief This is Constructor for LibCEC class. It initializes variables with
 * default values.
 */
LibCCEC::LibCCEC()
: initialized(false), connected(false)
{
}

/**
 * @brief This function is used to initialize CEC by starting the driver and
 * doing host-specific initialization.
 *
 * @param[in] name Name of CEC log prefix.
 *
 * @return None
 */
void LibCCEC::init(const char *name)
{AutoLock lock_(mutex);

	if (initialized) {
		throw InvalidStateException();
	}

    if (name != NULL) {
        strncpy(_CEC_LOG_PREFIX, name, sizeof(_CEC_LOG_PREFIX) - 2);
        _CEC_LOG_PREFIX[sizeof(_CEC_LOG_PREFIX) - 1] = '\0';
    }
    else {
        _CEC_LOG_PREFIX[0] = '\0';
    }

	/* Add Host-specific Initialization*/
	Driver::getInstance().open();
	Bus::getInstance().start();
	initialized = true;
}

/**
 * @brief This function is used to stop CEC by terminating the connection and
 * stoping the driver.
 *
 * @return None
 */
void LibCCEC::term()
{AutoLock lock_(mutex);

	if (!initialized) {
		throw InvalidStateException();
	}

	Bus::getInstance().stop();
	Driver::getInstance().close();
	initialized = false;
}

/**
 * @brief This function is used to add logical address
 * to the driver, so that it can ACK if there a direct messages received.
 *
 * @param[in] logical address of device.
 *
 * @return success or throw exceptions if there is error.
 */
int LibCCEC::addLogicalAddress(const LogicalAddress &source)
{
	//printf("Entered LibCCEC::getLogicalAddress \n");
	if (!initialized) {
		throw InvalidStateException();
	}

	Driver::getInstance().addLogicalAddress(source);

	return true;
}


/**
 * @brief This function is used to get CEC device logical address
 * starting the connection.
 *
 * @param[in] devType Device whose logical address has to be found.
 *
 * @return logicalAddress Logical address of the device.
 */
int LibCCEC::getLogicalAddress(int devType)
{
	//printf("Entered LibCCEC::getLogicalAddress \n");
	int logicalAddress = 0;
	if (!initialized) {
		throw InvalidStateException();
	}

	logicalAddress = Driver::getInstance().getLogicalAddress(devType);

	if(0 == logicalAddress)
	{
		throw InvalidStateException();
	}

	//printf("LibCCEC::getLogicalAddress got logicalAddress: %d \n",logicalAddress);
	return logicalAddress;
}

void LibCCEC::getPhysicalAddress(unsigned int *physicalAddress)
{
        //printf("Entered LibCCEC::getPhysicalAddress\n");
        if (!initialized) {
                throw InvalidStateException();
        }

        Driver::getInstance().getPhysicalAddress(physicalAddress);

        //printf("LibCCEC::getPhysicalAddress got getPhysicallAddress : %x \n",physicalAddress);
        return;
}

CCEC_END_NAMESPACE


/** @} */
/** @} */
/** @} */
