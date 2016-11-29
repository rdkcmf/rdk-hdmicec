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


#ifndef HDMI_CCEC_MESSAGE_PROCESSOR_HPP_
#define HDMI_CCEC_MESSAGE_PROCESSOR_HPP_

#include <iostream>

#include "Messages.hpp"
#include "Header.hpp"

CCEC_BEGIN_NAMESPACE

class MessageProcessor
{
public:
	MessageProcessor(void) {}

	virtual void process (const ActiveSource &msg, const Header &header)   				{header.print();msg.print();}
	virtual void process (const InActiveSource &msg, const Header &header) 				{header.print();msg.print();}
	virtual void process (const ImageViewOn &msg, const Header &header)    				{header.print();msg.print();}
	virtual void process (const TextViewOn &msg, const Header &header) 	 				{header.print();msg.print();}
	virtual void process (const RequestActiveSource &msg, const Header &header) 		{header.print();msg.print();}
	virtual void process (const Standby &msg, const Header &header)						{header.print();msg.print();}
	virtual void process (const GetCECVersion &msg, const Header &header) 				{header.print();msg.print();}
	virtual void process (const CECVersion &msg, const Header &header) 		 			{header.print();msg.print();}
	virtual void process (const SetMenuLanguage &msg, const Header &header) 			{header.print();msg.print();}
	virtual void process (const GiveOSDName &msg, const Header &header) 		 		{header.print();msg.print();}
	virtual void process (const GivePhysicalAddress &msg, const Header &header) 		{header.print();msg.print();}
	virtual void process (const GiveDeviceVendorID &msg, const Header &header) 		 	{header.print();msg.print();}
	virtual void process (const SetOSDString &msg, const Header &header) 		 		{header.print();msg.print();}
	virtual void process (const SetOSDName &msg, const Header &header) 		 			{header.print();msg.print();}
	virtual void process (const RoutingChange &msg, const Header &header) 			 	{header.print();msg.print();}
	virtual void process (const RoutingInformation &msg, const Header &header) 	 		{header.print();msg.print();}
	virtual void process (const SetStreamPath &msg, const Header &header) 	 			{header.print();msg.print();}
	virtual void process (const GetMenuLanguage &msg, const Header &header) 			{header.print();msg.print();}
	virtual void process (const ReportPhysicalAddress &msg, const Header &header) 		{header.print();msg.print();}
	virtual void process (const DeviceVendorID &msg, const Header &header) 				{header.print();msg.print();}
	//virtual void process (const UserControlReleased &msg, const Header &header) 		{header.print();msg.print();}
	//virtual void process (const UserControlPressed &msg, const Header &header) 		{header.print();msg.print();}
	virtual void process (const GiveDevicePowerStatus &msg, const Header &header) 		{header.print();msg.print();}
	virtual void process (const ReportPowerStatus &msg, const Header &header) 			{header.print();msg.print();}
	virtual void process (const FeatureAbort &msg, const Header &header) 				{header.print();msg.print();}
	virtual void process (const Abort &msg, const Header &header) 						{header.print();msg.print();}
	virtual void process (const Polling &msg, const Header &header) 					{header.print();msg.print();}

	virtual ~MessageProcessor(void) {}

private:
};

CCEC_END_NAMESPACE
#endif


/** @} */
/** @} */
