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

#ifndef HDMI_CCEC_OPERANDS_HPP_
#define HDMI_CCEC_OPERANDS_HPP_
#include <stdint.h>

#include <cstring>
#include <cstdio>
#include <vector>

#include <sstream>

#include "CCEC.hpp"
#include "Assert.hpp"
#include "Operand.hpp"
#include "Util.hpp"
#include "ccec/CECFrame.hpp"
#include "ccec/Exception.hpp"

CCEC_BEGIN_NAMESPACE

class CECBytes : public Operand
{
protected:
    CECBytes(const uint8_t val) : str(1, val) {}
	CECBytes(const uint8_t *buf, size_t len) {
        if (buf && len) {
            for (size_t i = 0; i < len; i++) {
                str.push_back(buf[i]);
            }
        }
	}

	bool validate(void) const {
		return (str.size() && (str.size() <= getMaxLen()));
	}

	CECBytes(const CECFrame &frame, size_t startPos, size_t len) {
    	/*
    	 * For HDMI CEC definition, the [OSD Name] and [OSD STring] are always the one
    	 * and only one operands in the message. It is not clear if these strings are
    	 * null terminated.  Therefore, consume all remaining bytes in the frame as
    	 * CECBytes
    	 */
        const uint8_t *buf = 0;
        size_t frameLen = 0;
        frame.getBuffer(&buf, &frameLen);
        str.clear();
        len = ((startPos + len) > frameLen) ? frameLen - startPos : len;
        str.insert(str.begin(), buf + startPos, buf + startPos + len);
        if (!validate())
        {
            throw InvalidParamException();
        }
	}

public:
	CECFrame &serialize(CECFrame &frame) const {
		for (size_t i = 0; i < str.size(); i++) {
			frame.append(str[i]);
		}

		return frame;
	}
    using Operand::serialize;

	~CECBytes(void) {}

	virtual const std::string toString(void) const {
		std::stringstream stream;
		for (size_t i = 0; i < str.size(); i++) {
			stream << std::hex << (int)str[i];
		}
		return stream.str();
    };

	bool operator == (const CECBytes &in) const {
		return this->str == in.str;
	}

protected:
    std::vector<uint8_t> str;
    virtual size_t getMaxLen(void) const {
        return CECFrame::MAX_LENGTH;
    }
};

class OSDString : public CECBytes 
{
public:
	enum {
		MAX_LEN = 13,
	};

    OSDString(const char *str) : CECBytes((const uint8_t *)str, strlen(str)) {
        validate();
    }

    OSDString(const CECFrame &frame, size_t startPos) : CECBytes(frame, startPos, MAX_LEN) {
    }

	const std::string toString(void) const {
		return std::string(str.begin(), str.end());
	}
protected:
	size_t getMaxLen() const {return MAX_LEN;}
};

class OSDName : public CECBytes 
{
public:
	enum {
		MAX_LEN = 14,
	};

    OSDName(const char *str) : CECBytes((const uint8_t *)str, strlen(str)) {
        validate();
    }

    OSDName(const CECFrame &frame, size_t startPos) : CECBytes(frame, startPos, MAX_LEN) {
    }

	const std::string toString(void) const {
		return std::string(str.begin(), str.end());
	}
protected:
	size_t getMaxLen() const {return MAX_LEN;}
};

class AbortReason : public CECBytes 
{
public :
    enum {
        MAX_LEN = 1,
    };

	enum  {
		UNRECOGNIZED_OPCODE,
		NOT_IN_CORRECT_MODE_TO_RESPOND,
		CANNOT_OVERIDE_SOURCE,
		INVALID_OPERAND,
		REFUSED,
		UNABLE_TO_DETERMINE,
	};

	AbortReason(int reason) : CECBytes((uint8_t)reason) { }

	const std::string toString(void) const {
		static const char *names_[] = {
			"Unrecognized opcode",
			"Not in correct mode to respond",
			"Cannot provide source",
			"Invalid operand",
			"Refused",
			"Unable to determine",
		};

		if (validate())
		{
			return names_[str[0]];
		}
		else
		{
			CCEC_LOG(LOG_WARN,"Unknown abort reason:%x\n", str[0]);
			return "Unknown";
		}
	}

	bool validate(void) const {
		return (/*(str[0]>= UNRECOGNIZED_OPCODE) && */(str[0]<= UNABLE_TO_DETERMINE));
	}

	int toInt(void) const {
        return str[0];
    }

	AbortReason(const CECFrame &frame, size_t startPos) : CECBytes(frame, startPos, MAX_LEN) { } 


protected:
	size_t getMaxLen() const {return MAX_LEN;}

};

class DeviceType : public CECBytes 
{
public :
    enum {
        MAX_LEN = 1,
    };

	enum  {
		TV = 0x0,
		RECORDING_DEVICE,
		RESERVED,
		TUNER,
		PLAYBACK_DEVICE,
		AUDIO_SYSTEM,
		PURE_CEC_SWITCH,
		VIDEO_PROCESSOR,
	};

	DeviceType(int type) : CECBytes((uint8_t)type) {}

	const std::string toString(void) const {
		static const char *names_[] = {
				"TV",
				"Recording Device",
				"Reserved",
				"Tuner",
				"Playback Device",
				"Audio System",
				"Pure CEC Switch",
				"Video Processor",
		};

		if (validate())
		{
			return names_[str[0]];
		}
		else
		{
			CCEC_LOG(LOG_WARN,"Unknown device type:%x\n", str[0]);
			return "Unknown";
		}
	}

	bool validate(void) const {
		return (/*(str[0] >= TV) && */(str[0] <= VIDEO_PROCESSOR));
	}

	DeviceType(const CECFrame &frame, size_t startPos) : CECBytes(frame, startPos, MAX_LEN) {}

	~DeviceType(void) {}

protected:
	size_t getMaxLen() const {return MAX_LEN;}
};

class Language : public CECBytes 
{
public:
	enum {
		MAX_LEN = 3,
	};

    Language(const char *str) : CECBytes((const uint8_t*)str, MAX_LEN) {
        validate();
    }

    Language(const CECFrame &frame, size_t startPos) : CECBytes(frame, startPos, MAX_LEN) {
    }

	const std::string toString(void) const {
		return std::string(str.begin(), str.end());
	}
protected:
	size_t getMaxLen() const {return MAX_LEN;}
};


class VendorID : public CECBytes
{
public:
	enum {
		MAX_LEN = 3,
	};

	VendorID(uint8_t byte0, uint8_t byte1, uint8_t byte2) : CECBytes (NULL, 0) {
        uint8_t bytes[MAX_LEN];
        bytes[0] = byte0;
        bytes[1] = byte1;
        bytes[2] = byte2;
        str.insert(str.begin(), bytes, bytes + MAX_LEN);
	}

	VendorID(const uint8_t *buf, size_t len) : CECBytes (buf, len > MAX_LEN ? MAX_LEN : len) {
    }

	VendorID(const CECFrame &frame, size_t startPos) : CECBytes (frame, startPos, MAX_LEN) {
    };

protected:
	size_t getMaxLen() const {return MAX_LEN;}

};

class PhysicalAddress : public CECBytes
{
    public:
	enum {
		MAX_LEN = 2,
	};

	PhysicalAddress(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3) : CECBytes (NULL, 0) {
        uint8_t bytes[MAX_LEN];
        bytes[0] = (byte0 & 0x0F)<< 4 | (byte1 & 0x0F);
        bytes[1] = (byte2 & 0x0F)<< 4 | (byte3 & 0x0F);
        str.insert(str.begin(), bytes, bytes + MAX_LEN);
    }

    PhysicalAddress(uint8_t *buf, size_t len = MAX_LEN) : CECBytes(buf, MAX_LEN) {
        Assert(len >= MAX_LEN);
    }

	PhysicalAddress(const CECFrame &frame, size_t startPos) : CECBytes (frame, startPos, MAX_LEN) {
    };

	PhysicalAddress(std::string &addr)         : CECBytes (NULL, 0) {
		uint8_t byte[4];
		uint8_t bytes[MAX_LEN];
		size_t dotposition = 0;
		int i = 0;

		Assert((addr.length() != 0 && addr.length() == 7));
		
		while (addr.length()    && i < 4)
		{
		  byte[i++] = stoi(addr,&dotposition,16);
		  if (addr.length() > 1)
		  {
		   	 addr = addr.substr(dotposition+1);
		  }
		  else
		  {
		 	 break; 	
		  }
		}
		
        bytes[0] = (byte[0] & 0x0F)<< 4 | (byte[1] & 0x0F);
        bytes[1] = (byte[2] & 0x0F)<< 4 | (byte[3] & 0x0F);
        str.insert(str.begin(), bytes, bytes + MAX_LEN);
    }

	const uint8_t getByteValue( int index) const {
		uint8_t val;

		Assert(index < 4);

		switch(index)
		{
			case 0: 
			{
				val = (int) ((str[0] & 0xF0) >> 4);
			}
				break;
			case 1: 
			{
				val = (int) (str[0] & 0x0F);
			}
				break;
			
			case 2: 
			{
				val = (int) ((str[1] & 0xF0) >> 4);
			}
				break;

			case 3: 
			{
				val = (int) (str[1] & 0x0F);
			}
				break;
		}

		return val;
    }

    const std::string toString(void) const {
		std::stringstream stream;
        stream << (int)((str[0] & 0xF0) >> 4)<< "." << (int)(str[0] & 0x0F) << "." << (int)((str[1] & 0xF0) >> 4) << "." << (int)(str[1] & 0x0F);
		return stream.str();
    }

    const std::string name(void) const {
        return "PhysicalAddress";
    }

protected:
	size_t getMaxLen() const {return MAX_LEN;}
};

class  LogicalAddress : public CECBytes 
{
public:
    enum {
        MAX_LEN = 1,
    };

    static const LogicalAddress kTv;

    enum {
    	TV 						= 0,
    	RECORDING_DEVICE_1 		= 1,
    	RECORDING_DEVICE_2		= 2,
        TUNER_1 				= 3,
        PLAYBACK_DEVICE_1 		= 4,
        AUDIO_SYSTEM 			= 5,
        TUNER_2 				= 6,
        TUNER_3 				= 7,
        PLAYBACK_DEVICE_2 		= 8,
        RECORDING_DEVICE_3 		= 9,
        TUNER_4 				= 10,
        PLAYBACK_DEVICE_3 		= 11,
        RESERVED_12 			= 12,
        RESERVED_13 			= 13,
        SPECIFIC_USE 			= 14,
        UNREGISTERED 			= 15,
        BROADCAST				= UNREGISTERED,
    };

    LogicalAddress(int addr = UNREGISTERED) : CECBytes((uint8_t)addr) { };

    const std::string toString(void) const
    {
    	static const char *names_[] = {
    	"TV",
    	"Recording Device 1",
    	"Recording Device 2",
    	"Tuner 1",
    	"Playback Device 1",
    	"Audio System",
    	"Tuner 2",
    	"Tuner 3",
    	"Playback Device 2",
    	"Recording Device 3",
    	"Tuner 4",
    	"Playback Device 3",
    	"Reserved 12",
    	"Reserved 13",
    	"Specific Use",
    	"Broadcast/Unregistered",
    	};

	if (validate())
	{
		return names_[str[0]];
	}
	else
	{
		CCEC_LOG(LOG_WARN,"Unknown logical address:%x\n", str[0]);
		return "Unknown";
	}
    }

	int toInt(void) const {
		return str[0];
	}

	bool validate(void) const {
		return ((str[0] <= BROADCAST) && (str[0] >= TV));
	}

    int getType(void) const {

        if (!validate()) {
            throw InvalidParamException();
        }

        static int _type[] = {
            DeviceType::TV,
            DeviceType::RECORDING_DEVICE,
            DeviceType::RECORDING_DEVICE,
            DeviceType::TUNER,
            DeviceType::PLAYBACK_DEVICE,
            DeviceType::AUDIO_SYSTEM,
            DeviceType::TUNER,
            DeviceType::TUNER,
            DeviceType::PLAYBACK_DEVICE,
            DeviceType::RECORDING_DEVICE,
            DeviceType::TUNER,
            DeviceType::PLAYBACK_DEVICE,
            DeviceType::RESERVED,
            DeviceType::RESERVED,
            DeviceType::RESERVED,
            DeviceType::RESERVED,
        };

        return _type[str[0]];
    }

	LogicalAddress(const CECFrame &frame, size_t startPos) : CECBytes (frame, startPos, MAX_LEN) {
    };
protected:
	size_t getMaxLen() const {return MAX_LEN;}
};

class Version : public CECBytes 
{
public:
    enum {
        MAX_LEN = 1,
    };

    enum {
    	V_RESERVED_0,
    	V_RESERVED_1,
    	V_RESERVED_2,
    	V_RESERVED_3,
    	V_1_3a,
    	V_1_4,
    };

	Version(int version) : CECBytes((uint8_t)version) { };

	bool validate(void) const {
		return ((str[0] <= V_1_4) && (str[0] >= V_1_3a));
	}

	const std::string toString(void) const
	{
		static const char *names_[] = {
				"Reserved",
				"Reserved",
				"Reserved",
				"Reserved",
				"Version 1.3a",
				"Version 1.4",
		};

		if (validate())
		{
			return names_[str[0]];
		}
		else
		{
			CCEC_LOG(LOG_WARN,"Unknown version:%x\n", str[0]);
			return "Unknown";
		}
	}

	Version (const CECFrame &frame, size_t startPos) : CECBytes (frame, startPos, MAX_LEN) {
    };
protected:
	size_t getMaxLen() const {return MAX_LEN;}
};

class PowerStatus : public CECBytes 
{
public:
    enum {
        MAX_LEN = 1,
    };

    enum {
        ON = 0,
        STANDBY = 0x01,
        IN_TRANSITION_STANDBY_TO_ON = 0x02,
        IN_TRANSITION_ON_TO_STANDBY = 0x03,
        POWER_STATUS_NOT_KNOWN = 0x4, 
        POWER_STATUS_FEATURE_ABORT = 0x05,
   };

	PowerStatus(int status) : CECBytes((uint8_t)status) { };

	bool validate(void) const {
		return ((str[0] <= IN_TRANSITION_ON_TO_STANDBY)/* && (str[0] >= ON)*/);
	}

	const std::string toString(void) const
	{
		static const char *names_[] = {
			"On",
			"Standby",
			"In transition Standby to On",
			"In transition On to Standby",
			"Not Known",
			"Feature Abort"
		};

		if (validate())
		{
			return names_[str[0]];
		}
		else
		{
			CCEC_LOG(LOG_WARN,"Unknown powerstatus:%x\n", str[0]);
			return "Unknown";
		}
	}

	int toInt(void) const {
		return str[0];
	}

	PowerStatus (const CECFrame &frame, size_t startPos) : CECBytes (frame, startPos, MAX_LEN) {
	};

protected:
	size_t getMaxLen() const {return MAX_LEN;}
};

class UICommand : public CECBytes 
{
public:
    enum {
        MAX_LEN = 1,
    };

    enum {
		UI_COMMAND_POWER_OFF_FUNCTION = 0x6C,
        UI_COMMAND_POWER_ON_FUNCTION = 0x6D,
    };

    UICommand(int command) : CECBytes((uint8_t)command) { };

    int toInt(void) const {
        return str[0];
    }

protected:
	size_t getMaxLen() const {return MAX_LEN;}
};


CCEC_END_NAMESPACE

#endif


/** @} */
/** @} */
