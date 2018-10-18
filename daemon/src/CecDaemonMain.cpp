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
* @defgroup daemon
* @{
**/


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "ccec/CCEC.hpp"
#include "ccec/LibCCEC.hpp"
#include "CecIARMBusMgr.hpp"
#include "ccec/Util.hpp"


using namespace std;

int main(int argc, char *argv[])
{
 	setvbuf(stdout, NULL, _IOLBF, 0);
 	check_cec_log_status();	
    CCEC_LOG( LOG_WARN, "Calling IARMMgr Init\r\n");
	/* Initialize IARM Bus before Accessing the Device settings APIS */
	CECIARMMgr::getInstance().init();
	CECIARMMgr::getInstance().loop();
	CECIARMMgr::getInstance().term();
}


/** @} */
/** @} */
