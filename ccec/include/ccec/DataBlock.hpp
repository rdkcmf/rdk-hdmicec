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


#ifndef HDMI_CCEC_DATABLOCK_HPP_
#define HDMI_CCEC_DATABLOCK_HPP_

#include <iostream>

#include "CCEC.hpp"
#include "ccec/CECFrame.hpp"
#include "Assert.hpp"
#include "ccec/Util.hpp"

CCEC_BEGIN_NAMESPACE

typedef uint32_t Op_t;

extern "C" const char *GetOpName(Op_t);

class DataBlock
{
public:
	/* Base is empty block */
    virtual std::string toString(void) const {return "[No Operands]";}
    virtual void print(void) const {
        CCEC_LOG( LOG_DEBUG, "%s : %s \n",GetOpName(opCode()),toString().c_str());
    }
    virtual CECFrame &serialize(CECFrame &frame) const {
        /* Default: Empty Datablock */
    	return frame;
    };
    virtual Op_t opCode(void) const = 0;
};

CCEC_END_NAMESPACE;


#endif


/** @} */
/** @} */
