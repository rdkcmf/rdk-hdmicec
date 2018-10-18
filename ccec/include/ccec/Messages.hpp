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
 * @defgroup HDMI_CEC_MSG_N_FRAME HDMI-CEC Messages and Frames
 * The CECFrame is a byte buffer that provides access to raw CEC bytes. CECFrame is guaranteed to be a
 * complete CEC Message that has the necessary data blocks:
 * @n @n
 * - The Header Data block (The byte that contains the initiator and destination address)
 * - The OpCode Data Block (The byte that contains the opcode).
 * - The Operand Data Block.(The bytes that contains the operands).
 *
 * In most cases application need not access CECFrame directly, but manipulate the raw bytes through the Message API.
 * @n @n
 * The Message API allows the application to send or receive high-level CEC message construct instead of raw bytes.
 * Basically for each CEC message (such as ActiveSource), there is a C++ class implementation representing it.
 * Each message class provides necessary getter and setter methods to access the properties of each message.
 * @ingroup HDMI_CEC
 *
 * @defgroup HDMI_CEC_MSG_N_FRAME_CLASSES HDMI-CEC Messages and Frames Classes
 * Described the details of High Level Class diagram and its functionalities.
 * @ingroup HDMI_CEC_MSG_N_FRAME
 *
 * @defgroup HDMI_CEC_MULTI_SYNC HDMI-CEC Messages - Asynchronous Vs. Synchronous
 * When messages converge on the logical buses, they are queued for sending opportunities on the physical bus.
 * The waiting time for such send to complete, though short in most cases, can be problematic to some interactive
 * real-time applications. It is recommended that the applications always send CEC messages asynchronously via
 * the Connection API and use the listener APIs to monitor response messages or device state changes.
 * The CEC library offers abundant APIs to facilitate such asynchronous implementation and the application is
 * encouraged to make full use of them.
 * @n @n
 * Given the vast variance of HDMI-CEC support from the off-the-self media devices, it is not recommended that
 * application wait for the response from a destination device. Even if the request message is sent out successfully,
 * the destination device may choose to ignore the request. The recommended approach is again to send the request
 * asynchronously and use the listener to monitor responses.
 * @n @n
 * Overall, given the asynchronous nature of HDMI-CEC, application should always opt to use Asynchronous APIs
 * as first choice. And for same reasons, the RDK CEC library offers only limited support for Synchronous APIs.
 * @ingroup HDMI_CEC_MSG_N_FRAME
 */

/**
* @defgroup hdmicec
* @{
* @defgroup ccec
* @{
**/


#ifndef HDMI_CCEC_MESSAGES_HPP_
#define HDMI_CCEC_MESSAGES_HPP_

#include <stdint.h>

#include <iostream>
#include "CCEC.hpp"
#include "Assert.hpp"
#include "ccec/CECFrame.hpp"
#include "DataBlock.hpp"
#include "OpCode.hpp"
#include "Operands.hpp"
#include "ccec/Util.hpp"

CCEC_BEGIN_NAMESPACE
/**
 * @brief The Message API allows the application to send or receive high-level CEC message construct instead of raw bytes.
 *
 * Basically for each CEC message (such as ActiveSource), there is a C++ class implementation representing it.
 * Each message class provides necessary getter and setter methods to access the properties of each message.
 * @ingroup HDMI_CEC_MSG_N_FRAME_CLASSES
 */
class ActiveSource : public DataBlock 
{
public:
    Op_t opCode(void) const {return ACTIVE_SOURCE;}

	ActiveSource(const PhysicalAddress &physicalAddress) : physicalAddress(physicalAddress) {
    }

	ActiveSource(const CECFrame &frame, int startPos = 0) 
    : physicalAddress(frame, startPos)
    {
    }
	CECFrame &serialize(CECFrame &frame) const {
        CCEC_LOG( LOG_DEBUG, "%s \n",physicalAddress.toString().c_str());
        return physicalAddress.serialize(frame);
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG, "%s : %s  : %s \n",GetOpName(opCode()),physicalAddress.name().c_str(),physicalAddress.toString().c_str());
    }
public:
    PhysicalAddress physicalAddress;
};

class ImageViewOn : public DataBlock
{
public:
    Op_t opCode(void) const {return IMAGE_VIEW_ON;}
};


class TextViewOn : public DataBlock
{
public:
    Op_t opCode(void) const {return TEXT_VIEW_ON;}
};

class InActiveSource : public DataBlock 
{
public:
    Op_t opCode(void) const {return INACTIVE_SOURCE;}

	InActiveSource(const PhysicalAddress &physicalAddress) : physicalAddress(physicalAddress) {}

	InActiveSource(const CECFrame &frame, int startPos = 0) 
    : physicalAddress(frame, startPos)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
        CCEC_LOG( LOG_DEBUG, "%s \n",physicalAddress.toString().c_str());
        return physicalAddress.serialize(frame);
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG, "%s : %s : %s  \n",GetOpName(opCode()),physicalAddress.name().c_str(),physicalAddress.toString().c_str());
    }
public:
    PhysicalAddress physicalAddress;
};

class RequestActiveSource : public DataBlock
{
public:
    Op_t opCode(void) const {return REQUEST_ACTIVE_SOURCE;}
};

class Standby : public DataBlock
{
public:
    Op_t opCode(void) const {return STANDBY;}
};

class GetCECVersion : public DataBlock
{
public:
    Op_t opCode(void) const {return GET_CEC_VERSION;}
};

class CECVersion : public DataBlock
{
public:
    Op_t opCode(void) const {return CEC_VERSION;}

    CECVersion(const Version &version) : version(version) {}

    CECVersion(const CECFrame &frame, int startPos = 0)
    : version(frame, startPos)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
        return version.serialize(frame);
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG, "Version : %s \n",version.toString().c_str());
    }

    Version version;
};

class SetMenuLanguage : public DataBlock
{

public:
    Op_t opCode(void) const {return SET_MENU_LANGUAGE;}

    SetMenuLanguage(const Language &language) : language(language) {};

    SetMenuLanguage(const CECFrame &frame, int startPos = 0)
    : language(frame, startPos)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
	    return language.serialize(frame);
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG, "Language : %s \n",language.toString().c_str());
    }

    Language language;
};

class GetMenuLanguage: public DataBlock
{
public:
    Op_t opCode(void) const {return GET_MENU_LANGUAGE;}
};

class GiveOSDName: public DataBlock
{
public:
    Op_t opCode(void) const {return GIVE_OSD_NAME;}
};

class SetOSDName: public DataBlock
{
public:
    Op_t opCode(void) const {return SET_OSD_NAME;}

    SetOSDName(const OSDName &osdName) : osdName(osdName) {};

    SetOSDName(const CECFrame &frame, int startPos = 0)
    : osdName(frame, startPos)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
	    return osdName.serialize(frame);
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG,"OSDName : %s\n",osdName.toString().c_str());
    }

    OSDName osdName;
};

class SetOSDString : public DataBlock
{
public:
    Op_t opCode(void) const {return SET_OSD_STRING;}

    SetOSDString(const OSDString &osdString) : osdString(osdString) {};

    SetOSDString(const CECFrame &frame, int startPos = 0)
    : osdString(frame, startPos)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
	    return osdString.serialize(frame);
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG,"OSDString : %s\n",osdString.toString().c_str());
    }

    OSDString osdString;
};

class GivePhysicalAddress : public DataBlock
{
public:
    Op_t opCode(void) const {return GIVE_PHYSICAL_ADDRESS;}
};

class ReportPhysicalAddress : public DataBlock
{
public:
    Op_t opCode(void) const {return REPORT_PHYSICAL_ADDRESS;}

    ReportPhysicalAddress(const PhysicalAddress &physicalAddress, const DeviceType &deviceType)
    : physicalAddress(physicalAddress), deviceType(deviceType) {
    }

    ReportPhysicalAddress(const CECFrame &frame, int startPos = 0)
    : physicalAddress(frame, startPos), deviceType(frame, startPos + PhysicalAddress::MAX_LEN)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
		return deviceType.serialize(physicalAddress.serialize(frame));
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG,"Physical Address : %s\n",physicalAddress.toString().c_str());
        CCEC_LOG( LOG_DEBUG,"Device Type : %s\n",deviceType.toString().c_str());
    }

	PhysicalAddress physicalAddress;
	DeviceType deviceType;
};

class GiveDeviceVendorID : public DataBlock
{
public:
    Op_t opCode(void) const {return GIVE_DEVICE_VENDOR_ID;}
};

class DeviceVendorID : public DataBlock
{
public:
    Op_t opCode(void) const {return DEVICE_VENDOR_ID;}

    DeviceVendorID(const VendorID &vendorId) : vendorId(vendorId) {}

    DeviceVendorID(const CECFrame &frame, int startPos = 0)
    : vendorId(frame, startPos)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
		return vendorId.serialize(frame);
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG,"VendorID : %s\n",vendorId.toString().c_str());
    }

    VendorID vendorId;
};

class GiveDevicePowerStatus : public DataBlock
{
public:
    Op_t opCode(void) const {return GIVE_DEVICE_POWER_STATUS;}
};

class ReportPowerStatus : public DataBlock
{
public:
    Op_t opCode(void) const {return REPORT_POWER_STATUS;}

    ReportPowerStatus(const PowerStatus &status) : status(status) {}

    ReportPowerStatus(const CECFrame &frame, int startPos = 0)
    : status(frame, startPos)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
		return status.serialize(frame);
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG,"Power Status: %s",status.toString().c_str());
    }

    PowerStatus status;
};

class Abort : public DataBlock
{
public:
    Op_t opCode(void) const {return ABORT;}
};

class FeatureAbort: public DataBlock
{
public:
    Op_t opCode(void) const {return FEATURE_ABORT;}

    FeatureAbort(const OpCode &feature, const AbortReason &reason) : feature(feature), reason(reason) {}

    FeatureAbort(const CECFrame &frame, int startPos = 0)
    : feature(frame, startPos), reason(frame, startPos + OpCode::MAX_LEN)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
		return reason.serialize(feature.serialize(frame));
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG,"Abort For Feature : %s\n",feature.toString().c_str());
        CCEC_LOG( LOG_DEBUG,"Abort Reason : %s\n",reason.toString().c_str());
    }

    OpCode feature;
    AbortReason reason;
};

class RoutingChange: public DataBlock
{
public:
    Op_t opCode(void) const {return ROUTING_CHANGE;}

    RoutingChange(const PhysicalAddress &from, const PhysicalAddress &to) : from(from), to(to) {}

    RoutingChange(const CECFrame &frame, int startPos = 0)
    : from(frame, startPos), to(frame, startPos + PhysicalAddress::MAX_LEN)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
		return to.serialize(from.serialize(frame));
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG,"Routing Change From : %s\n",from.toString().c_str());
        CCEC_LOG( LOG_DEBUG,"Routing Change to : %s\n",to.toString().c_str());
    }

    PhysicalAddress from;
    PhysicalAddress to;
};

class RoutingInformation: public DataBlock
{
public:
    Op_t opCode(void) const {return ROUTING_INFORMATION;}

    RoutingInformation(const PhysicalAddress &toSink) : toSink(toSink) {}

    RoutingInformation(const CECFrame &frame, int startPos = 0)
    : toSink(frame, startPos)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
		return toSink.serialize(frame);
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG,"Routing Information to Sink : %s\n",toSink.toString().c_str());
    }

    PhysicalAddress toSink;
};


class SetStreamPath: public DataBlock
{
public:
    Op_t opCode(void) const {return SET_STREAM_PATH;}

    SetStreamPath(const PhysicalAddress &toSink) : toSink(toSink) {}

    SetStreamPath(const CECFrame &frame, int startPos = 0)
    : toSink(frame, startPos)
    {
    }

	CECFrame &serialize(CECFrame &frame) const {
		return toSink.serialize(frame);
	}

    void print(void) const {
        CCEC_LOG( LOG_DEBUG,"Set Stream Path to Sink : %s\n",toSink.toString().c_str());
    }

    PhysicalAddress toSink;
};

class Polling : public DataBlock
{
public:
    Op_t opCode(void) const {return POLLING;}
};
#endif


/** @} */
/** @} */
