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

#include "ccec/CECFrame.hpp"
#include "ccec/Header.hpp"
#include "ccec/Operand.hpp"
#include "ccec/Operands.hpp"
#include "ccec/MessageProcessor.hpp"
#include "ccec/MessageDecoder.hpp"
#include "ccec/Exception.hpp"
#include "ccec/Util.hpp" 

CCEC_BEGIN_NAMESPACE

void MessageDecoder::decode(const CECFrame &in_)
{
    const int HEADER_OFFSET = 0;
    const int OPCODE_OFFSET = 1;
    const int OPRAND_OFFSET = 2;

	Header header(in_, HEADER_OFFSET);

    if (in_.length() == 1) {
    	/* This is an Polling Message */
    	processor.process(Polling(), header);
    	return;
    }

	CECFrame in = in_.subFrame(OPRAND_OFFSET);

    try
    {
	switch (OpCode(in_, OPCODE_OFFSET).opCode()) {
	case ACTIVE_SOURCE:
		processor.process(ActiveSource(in), header);
		break;
	case INACTIVE_SOURCE:
		processor.process(InActiveSource(in), header);
		break;
	case IMAGE_VIEW_ON:
		processor.process(ImageViewOn(), header);
		break;
	case TEXT_VIEW_ON:
		processor.process(TextViewOn(), header);
		break;
	case REQUEST_ACTIVE_SOURCE:
		processor.process(RequestActiveSource(), header);
		break;
	case STANDBY:
		CCEC_LOG( LOG_DEBUG, "Decoding STANDBY\r\n");
		processor.process(Standby(), header);
		break;
	case GET_CEC_VERSION:
		processor.process(GetCECVersion(), header);
		break;
	case CEC_VERSION:
		processor.process(CECVersion(in), header);
		break;
	case SET_MENU_LANGUAGE:
		processor.process(SetMenuLanguage(in), header);
		break;
	case GIVE_OSD_NAME:
		processor.process(GiveOSDName(), header);
		break;
	case GIVE_PHYSICAL_ADDRESS:
		processor.process(GivePhysicalAddress(), header);
		break;
	case REPORT_PHYSICAL_ADDRESS:
		processor.process(ReportPhysicalAddress(in), header);
		break;
	case GIVE_DEVICE_VENDOR_ID:
		processor.process(GiveDeviceVendorID(), header);
		break;
	case ROUTING_CHANGE:
		processor.process(RoutingChange(in), header);
		break;
	case ROUTING_INFORMATION:
		processor.process(RoutingInformation(in), header);
		break;
	case SET_STREAM_PATH:
		processor.process(SetStreamPath(in), header);
		break;
	case GET_MENU_LANGUAGE:
		processor.process(GetMenuLanguage(), header);
		break;
	case DEVICE_VENDOR_ID:
		processor.process(DeviceVendorID(in), header);
		break;
	case VENDOR_COMMAND:
		//processor.process(VendorCommand(in), header);
		break;
	case VENDOR_COMMAND_WITH_ID:
		//processor.process(VendorCommandWithID(in), header);
		break;
	case VENDOR_REMOTE_BUTTON_DOWN:
		//processor.process(VendorRemoteButtonDown(in), header);
		break;
	case VENDOR_REMOTE_BUTTON_UP:
		//processor.process(VendorRemoteButtonUp(in), header);
		break;
	case SET_OSD_STRING:
		processor.process(SetOSDString(in), header);
		break;
	case SET_OSD_NAME:
		processor.process(SetOSDName(in), header);
		break;
	case USER_CONTROL_RELEASED:
		//processor.process(UserControlReleased(in), header);
		break;
	case USER_CONTROL_PRESSED:
		//processor.process(UserControlPressed(in), header);
		break;
	case GIVE_DEVICE_POWER_STATUS:
		processor.process(GiveDevicePowerStatus(), header);
		break;
	case REPORT_POWER_STATUS:
		processor.process(ReportPowerStatus(in), header);
		break;
	case FEATURE_ABORT:
		processor.process(FeatureAbort(in), header);
		break;
	case ABORT:
		processor.process(Abort(), header);
		break;
        case INITIATE_ARC:
             processor.process(InitiateArc(), header);
                break;
       case TERMINATE_ARC:
             processor.process(TerminateArc(), header);
             break;
	 case REQUEST_SHORT_AUDIO_DESCRIPTOR:
             processor.process(RequestShortAudioDescriptor(in), header);
             break;

	case REPORT_SHORT_AUDIO_DESCRIPTOR:
	     processor.process(ReportShortAudioDescriptor(in), header);
	     break;
	case SYSTEM_AUDIO_MODE_REQUEST:
             processor.process(SystemAudioModeRequest(in), header);
             break;
	default:
        CCEC_LOG( LOG_DEBUG, "Unhandled Message Received \n");
        OpCode(in_, OPCODE_OFFSET).print();
		break;
	}
    }
    catch(InvalidParamException &e)
    {
	CCEC_LOG( LOG_EXP, "MessageDecoder::decode caught %s \r\n",e.what());
    }
    catch(std::exception &e)
    {
	CCEC_LOG( LOG_EXP, "MessageDecoder::decode caught an %s \r\n",e.what());
        in_.hexDump(LOG_EXP);
    }
}

CCEC_END_NAMESPACE


/** @} */
/** @} */
