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


#ifndef HDMI_CCEC_MESSAGE_ENCODER_H_
#define HDMI_CCEC_MESSAGE_ENCODER_H_

#include "CCEC.hpp"
#include "ccec/CECFrame.hpp"
#include "DataBlock.hpp"
#include "OpCode.hpp"
#include "Header.hpp"

CCEC_BEGIN_NAMESPACE

/**
 * @brief High-level messages are encoded by the MessageEncoder into raw bytes and placed in a CECFrame.
 *
 * A CECFrame is then sent out via an opened connection.
 * @ingroup HDMI_CEC_MSG_N_FRAME_CLASSES
 */
class MessageEncoder {

public:
	MessageEncoder(void) {};
	~MessageEncoder(void) {};


	static CECFrame & encode(const Header &h, const DataBlock &m, CECFrame &out)
    {
        h.serialize(out);
        return encode(m, out);
    }

	static CECFrame encode(const Header &h, const DataBlock &m)
    {
        CECFrame out;
        h.serialize(out);
        return encode(m, out);
    }

	static CECFrame & encode(const DataBlock &m, CECFrame &out)
	{
        OpCode(m.opCode()).serialize(out);
        m.serialize(out);
        return out;
	}

	static CECFrame encode(const DataBlock &m) {
        CECFrame out;
        return encode(m, out);
    }
};

CCEC_END_NAMESPACE
#endif


/** @} */
/** @} */
