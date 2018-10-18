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


#include <cstdio>
#include <stdexcept>
#include "ccec/CECFrame.hpp"
#include "ccec/Util.hpp"

namespace {
}

CCEC_BEGIN_NAMESPACE

CECFrame::CECFrame(const uint8_t *buf, size_t len) : len_() {
	reset();
    if (buf && len > 0) append(buf, len);
}

CECFrame CECFrame::subFrame(size_t start, size_t len) const {
    CECFrame frame;
    if (len == 0) len = MAX_LENGTH;
    while(start < len_ && len--) {
        frame.append(at(start));
        start++;
    }
    return frame;
}

void CECFrame::append(uint8_t byte) {
	if (len_ == MAX_LENGTH)
		throw std::out_of_range("Frame grows beyond maximum");
	buf_[len_++] = byte;
}

void CECFrame::append(const uint8_t *buf, size_t len) {
	for (size_t i = 0; i < (len); i++) {
		append(buf[i]);
	}
}

void CECFrame::append(const CECFrame &frame) {
	const uint8_t *buf = NULL;
	size_t len = 0;
	frame.getBuffer(&buf, &len);
	append(buf, len);
}

void CECFrame::reset(void) {
	len_ = 0;
}

void CECFrame::getBuffer(const uint8_t **buf, size_t *len) const {
	*len = this->len_;
	*buf = this->buf_;

}

const uint8_t * CECFrame::getBuffer(void) const {
	return buf_;
}

const uint8_t CECFrame::at(size_t i) const {
	if (i >= len_) {
        CCEC_LOG( LOG_DEBUG, "Frame i=%d, len=%d\r\n", i, len_);
        //int *p = NULL;
        //*p = 0xACACACAC;
		throw std::out_of_range("Frame reads beyond maximum");
	}

	return buf_[i];
}

uint8_t & CECFrame::operator[] (size_t i) {
	if (i >= len_) {
		throw std::out_of_range("Frame access beyond maximum");
	}

    return buf_[i];
}

const size_t CECFrame::length(void) const {
	return len_;
}

void CECFrame::hexDump(int level) const {
	if (1) {
		CCEC_LOG( level, "FRAME DUMP========================: \r\n");
		for (size_t i = 0; i < len_; i++) {
			CCEC_LOG( level, "%02X ", (int) buf_[i]);
		}
		CCEC_LOG( level, "\r\n================================== \r\n");
	}
}


CCEC_END_NAMESPACE



/** @} */
/** @} */
