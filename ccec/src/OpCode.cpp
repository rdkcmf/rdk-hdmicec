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


#include "ccec/OpCode.hpp"
#include "ccec/Operand.hpp"
#include "ccec/Assert.hpp"

CCEC_BEGIN_NAMESPACE

extern "C" const char *GetOpName(Op_t op);

const char *GetOpName(Op_t op)
{
	const char *name = NULL;

	switch(op) {
	case ACTIVE_SOURCE:
		name = "Active Source";
		break;
	case IMAGE_VIEW_ON:
		name = "Image View On";
		break;
	case TEXT_VIEW_ON:
		name = "Text View On";
		break;
	case INACTIVE_SOURCE:
		name = "InActive Source";
		break;
	case REQUEST_ACTIVE_SOURCE:
		name = "Request Active Source";
		break;
	case ROUTING_CHANGE:
		name = "Routing Change";
		break;
	case ROUTING_INFORMATION:
		name = "Routing Information";
		break;
	case SET_STREAM_PATH:
		name = "Set Stream Path";
		break;
	case STANDBY:
		name = "Stand by";
		break;
	case RECORD_OFF:
		name = "Record Off";
		break;
	case RECORD_ON:
		name = " Record On";
		break;
	case RECORD_STATUS:
		name = "Record Status";
		break;
	case RECORD_TV_SCREEN:
		name = "Record TV Screen";
		break;
	case CLEAR_ANALOGUE_TIMER:
		name = "Clear Analogue Timer";
		break;
	case CLEAR_DIGITAL_TIMER:
		name = "Clear Digital Timer";
		break;
	case CLEAR_EXTERNAL_TIMER:
		name = "Clear External Timer";
		break;
	case SET_ANALOG_TIMER:
		name = "Set Analog Timer";
		break;
	case SET_DIGITAL_TIMER:
		name = " Set Digital Timer";
		break;
	case SET_EXTERNAL_TIMER:
		name = "Set External Timer";
		break;
	case SET_TIMER_PROGRAM_TITLE:
		name = "Set Timer Program Title";
		break;
	case TIMER_CLEARED_STATUS:
		name = "Timer Cleared Status";
		break;
	case TIMER_STATUS:
		name = "Timer Status";
		break;
	case CEC_VERSION:
		name = "CEC Version";
		break;
	case GIVE_PHYSICAL_ADDRESS:
		name = "Give Physical Address";
		break;
	case GET_MENU_LANGUAGE:
		name = "Get Menu Language";
		break;
	case POLLING:
		name = "Polling ";
		break;
	case REPORT_PHYSICAL_ADDRESS:
		name = "Report Physical Address";
		break;
	case SET_MENU_LANGUAGE:
		name = "Set Menu Language";
		break;
	case DECK_CONTROL:
		name = "Deck control";
		break;
	case DECK_STATUS:
		name = "deck Status";
		break;
	case PLAY:
		name = "Play";
		break;
	case GIVE_TUNER_DEVICE_STATUS:
		name = "Give Tuner Device Status";
		break;
	case SELECT_ANALOGUE_SERVICE:
		name = "Select Analogue service";
		break;
	case TUNER_DEVICE_STATUS:
		name = "Tuner Device Status";
		break;
	case TUNER_STEP_DECREMENT:
		name = "Tuner Step Decrement";
		break;
	case TUNER_STEP_INCREMENT:
		name = "Tuner Step Increment";
		break;
   	case DEVICE_VENDOR_ID:
		name = "Device Vendor Id";
		break;
	case GET_CEC_VERSION:
		name = "Get CEC Version";
		break;
	case GIVE_DEVICE_VENDOR_ID:
		name = "Give Ddevice Vendor ID";
		break;
	case VENDOR_COMMAND:
		name = "Vendor Command";
		break;
	case VENDOR_COMMAND_WITH_ID:
		name = "Vendor command With ID";
		break;
	case VENDOR_REMOTE_BUTTON_DOWN:
		name = "Vendor Remote Button Down";
		break;
	case VENDOR_REMOTE_BUTTON_UP:
		name = "Vendor Remote Button Up";
		break;
	case SET_OSD_STRING:
		name = "Set OSD String";
		break;
	case GIVE_OSD_NAME:
		name = "Give OSD Name";
		break;
	case SET_OSD_NAME:
		name = "Set OSD Name";
		break;
	case MENU_REQUEST:
		name = "Menu Request";
		break;
	case MENU_STATUS:
		name = "Menu Status";
		break;
	case USER_CONTROL_PRESSED:
		name = "User control Pressed";
		break;
	case USER_CONTROL_RELEASED:
		name = "User Control released";
		break;
	case GIVE_DEVICE_POWER_STATUS:
		name = "Give Device Power Status";
		break;
	case REPORT_POWER_STATUS:
		name = "Report power Status";
		break;
	case FEATURE_ABORT:
		name = "Feature Abort";
		break;
	case ABORT:
		name = "Abort";
		break;
	case GIVE_AUDIO_STATUS:
		name = "Give Aduio Status";
		break;
	case GIVE_SYSTEM_AUDIO_MODE_STATUS:
		name = "Give System Audio Mode Status";
		break;
	case REPORT_AUDIO_STATUS:
		name = "Report Audio Status";
		break;
	case REQUEST_SHORT_AUDIO_DESCRIPTOR:
		name = "Request Short Audio Descriptor";
		break;
	case REPORT_SHORT_AUDIO_DESCRIPTOR:
		name = "Report Short Audio Descriptor";
		break;
	case SET_SYSTEM_AUDIO_MODE:
		name = "Set System Audio Mode";
		break;
	case SYSTEM_AUDIO_MODE_REQUEST:
		name = "System Audio mode request";
		break;
	case SET_AUDIO_RATE:
		name = "Set Audio rate";
		break;
	case INITIATE_ARC:
		name = "Initiate ARC";
		break;
	case REPORT_ARC_INITIATED:
		name = "Report ARC Initiated";
		break;
	case REPORT_ARC_TERMINATED:
		name = "Report ARC Terminated";
		break;
	case REQUEST_ARC_INITIATION:
		name = "Report ARC Initiation";
		break;
	case REQUEST_ARC_TERMINATION:
		name = "Request ARC Termination";
		break;
	case TERMINATE_ARC:
		name = "Terminate ARC";
		break;
	case CDC_MESSAGE:
		name = "CDC Message";
		break;
	default:
		name = "Unrecognized Message";
		break;
	}

	return name;
}

CCEC_END_NAMESPACE


/** @} */
/** @} */
