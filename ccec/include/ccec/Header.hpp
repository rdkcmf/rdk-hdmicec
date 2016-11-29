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


#ifndef HDMI_CCEC_HEADER_BLOCK_
#define HDMI_CCEC_HEADER_BLOCK_

#include "CCEC.hpp"
#include "Assert.hpp"
#include "DataBlock.hpp"
#include "Operands.hpp"
#include "Util.hpp"

CCEC_BEGIN_NAMESPACE

class Header 
{
public:
	enum {
		MAX_LEN = 1,
	};

	Header(const LogicalAddress &from, const LogicalAddress &to) : from(from), to(to) {
        print();
    };

	Header(const CECFrame &frame, size_t startPos = 0) {
        from = LogicalAddress((frame.at(startPos) & 0xF0) >> 4);
        to   = LogicalAddress((frame.at(startPos) & 0x0F) >> 0);
	}

	CECFrame &serialize(CECFrame &frame) const {
		CCEC_LOG( LOG_DEBUG, "Serialing header  %s   %s \n",from.toString().c_str(),to.toString().c_str());
        /* serialize to same byte */
        to.serialize(frame);
        uint8_t &byte = frame[frame.length() - 1];
        Assert(byte == to.toInt());
        byte = from.toInt() << 4 | byte;
        return frame;
	}

	virtual ~Header(void) {};

    void print(void) const {
        CCEC_LOG( LOG_DEBUG, "Header : From : %s \n", from.toString().c_str());
        CCEC_LOG( LOG_DEBUG, "Header : to   : %s \n", to.toString().c_str());
    }


public:
	LogicalAddress from;
	LogicalAddress to;
};


CCEC_END_NAMESPACE

#endif



/** @} */
/** @} */
