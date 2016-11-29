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


#ifndef HDMI_CCEC_OPCODE_HPP_
#define HDMI_CCEC_OPCODE_HPP_

#include <iostream>

#include "CCEC.hpp"
#include "ccec/CECFrame.hpp"
#include "Assert.hpp"

#include "DataBlock.hpp"


CCEC_BEGIN_NAMESPACE

extern "C" const char *GetOpName(Op_t op);

enum
{
	ACTIVE_SOURCE 					= 0x82,
	IMAGE_VIEW_ON 					= 0x04,
	TEXT_VIEW_ON 					= 0x0D,
	INACTIVE_SOURCE 				= 0x9D,
	REQUEST_ACTIVE_SOURCE 			= 0x85,
	ROUTING_CHANGE                  = 0x80,
	ROUTING_INFORMATION             = 0x81,
	SET_STREAM_PATH                 = 0x86,
	STANDBY 						= 0x36,
	RECORD_OFF                      = 0X0B,
	RECORD_ON                       = 0X09,
	RECORD_STATUS                   = 0X0A,
	RECORD_TV_SCREEN                = 0X0F,
	CLEAR_ANALOGUE_TIMER            = 0X33,
	CLEAR_DIGITAL_TIMER             = 0X99,
	CLEAR_EXTERNAL_TIMER            = 0XA1,
	SET_ANALOG_TIMER                = 0X34,
	SET_DIGITAL_TIMER               = 0X97,
	SET_EXTERNAL_TIMER              = 0XA2,
	SET_TIMER_PROGRAM_TITLE         = 0X67,
	TIMER_CLEARED_STATUS            = 0X43,
	TIMER_STATUS                    = 0X35,
	GET_CEC_VERSION 				= 0x9F,
	CEC_VERSION 					= 0x9E,
	GIVE_PHYSICAL_ADDRESS 			= 0x83,
	GET_MENU_LANGUAGE               = 0X91,
	REPORT_PHYSICAL_ADDRESS 		= 0x84,
	SET_MENU_LANGUAGE               = 0X32,
	DECK_CONTROL                    = 0X42,
	DECK_STATUS                     = 0X1B,
	GIVE_DECK_STATUS                = 0X1A,
	PLAY                            = 0X41,
	GIVE_TUNER_DEVICE_STATUS        = 0X08,
	SELECT_ANALOGUE_SERVICE         = 0X92,
	SELECT_DIGITAL_SERVICE          = 0X93,
	TUNER_DEVICE_STATUS             = 0X07,
	TUNER_STEP_DECREMENT            = 0X06,
	TUNER_STEP_INCREMENT            = 0X05,
	DEVICE_VENDOR_ID 				= 0x87,
	GIVE_DEVICE_VENDOR_ID 			= 0x8C,
	VENDOR_COMMAND                  = 0X89,
	VENDOR_COMMAND_WITH_ID          = 0XA0,
	VENDOR_REMOTE_BUTTON_DOWN       = 0X8A,
	VENDOR_REMOTE_BUTTON_UP         = 0X8B,
	SET_OSD_STRING 					= 0x64,
	GIVE_OSD_NAME 					= 0x46,
	SET_OSD_NAME 					= 0x47,
	MENU_REQUEST                    = 0X8D,
	MENU_STATUS                     = 0X8E,
	USER_CONTROL_PRESSED            = 0X44,
	USER_CONTROL_RELEASED           = 0X45,
	GIVE_DEVICE_POWER_STATUS 		= 0x8F,
	REPORT_POWER_STATUS 			= 0x90,
	FEATURE_ABORT 					= 0x00,
	ABORT 							= 0xFF,
	GIVE_AUDIO_STATUS               = 0X71,
	GIVE_SYSTEM_AUDIO_MODE_STATUS   = 0X7D,
	REPORT_AUDIO_STATUS             = 0X7A,
	REPORT_SHORT_AUDIO_DESCRIPTOR   = 0XA3,
	REQUEST_SHORT_AUDIO_DESCRIPTOR  = 0XA4,
	SET_SYSTEM_AUDIO_MODE           = 0X72,
	SYSTEM_AUDIO_MODE_REQUEST       = 0X70,
	SYSTEM_AUDIO_MODE_STATUS        = 0X7E,
	SET_AUDIO_RATE                  = 0X9A,
	INITIATE_ARC                    = 0XC0,
	REPORT_ARC_INITIATED            = 0XC1,
	REPORT_ARC_TERMINATED           = 0XC2,
	REQUEST_ARC_INITIATION          = 0XC3,
	REQUEST_ARC_TERMINATION         = 0XC4,
	TERMINATE_ARC                   = 0XC5,
	CDC_MESSAGE                     = 0XF8,
	POLLING 						= 0x200, // Special Code for Polling Msg.

	UNKNOWN                         = 0xFFFF
};

class OpCode : public DataBlock 
{
public:
    enum {
        MAX_LEN = 1,
    };

    OpCode(Op_t opCode) : opCode_(opCode) {};
	OpCode(const CECFrame &frame, int startPos) : opCode_(frame.at(startPos)) {
    }
	CECFrame &serialize(CECFrame &frame) const {
        if (opCode_ != POLLING) {
            frame.append(opCode_);
        }
        return frame;
    }
    virtual Op_t opCode(void) const {return opCode_;};
    virtual std::string toString(void) const {return GetOpName(opCode_);}
    virtual void print(void) const {
    	CCEC_LOG( LOG_DEBUG , "Opcode : %s \n",toString().c_str());
    };
private:
    Op_t opCode_;
};



CCEC_END_NAMESPACE;


#endif


/** @} */
/** @} */
