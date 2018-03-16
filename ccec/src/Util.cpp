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


#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "ccec/Util.hpp"

static int cec_log_level = LOG_INFO;
#define MAX_LOG_BUFF 500


#define __TIMESTAMP() do { /*YYMMDD-HH:MM:SS:usec*/               \
        struct tm __tm;                                             \
        struct timeval __tv;                                        \
        gettimeofday(&__tv, NULL);                                  \
        localtime_r(&__tv.tv_sec, &__tm);                           \
        printf("\r\n%02d%02d%02d-%02d:%02d:%02d:%06d ",                 \
                            __tm.tm_year+1900-2000,                             \
                            __tm.tm_mon+1,                                      \
                            __tm.tm_mday,                                       \
                            __tm.tm_hour,                                       \
                            __tm.tm_min,                                        \
                            __tm.tm_sec,                                        \
                            (int)__tv.tv_usec);                                      \
} while(0)

static const char *logLevel[][2] =
{   {"FATAL","0"},
    {"ERROR","1"},
    {"WARN","2"},
    {"EXP","3"},
    {"NOTICE","4"},
    {"INFO","5"},
    {"DEBUG","6"},
    {"TRACE","7"}
};

/**
 * @brief This function is used to get the cec log from the log file and checks
 * the level of log received.
 *
 * @return None
 */
void check_cec_log_status(void)
{
    struct stat st;
    FILE *fp;
    const int buffer_length = 256;
    char cecBuffer[buffer_length];
    memset(&st,0,sizeof(st));
    if(0 == stat("/tmp/cec_log_enabled",&st))
    {
        if(!S_ISREG(st.st_mode))
        {
            return;
        }
        if((fp = fopen("/tmp/cec_log_enabled","r")) == NULL)
        {
            printf("Error in opening cec_log_enabled filee \n");
            return;
        }
        if ((fgets(cecBuffer,buffer_length,fp)) != NULL)
        {
            for (int i =0; i< LOG_MAX;i++)
            {
                if (strncmp(cecBuffer,logLevel[i][0],strlen(logLevel[i][0])) == 0)
                {
                    cec_log_level = atoi(logLevel[i][1]);
					break;
                }
            }
        }
        fclose(fp);
        }
        return;
}

char _CEC_LOG_PREFIX[64];

/**
 * @brief This function is used to gets the logs depending on the level of log
 * and print these to standard output.
 *
 * @param[in] level CEC Log level
 * @param[in] format Format of the received data.
 *
 * @return None
 */
void CCEC_LOG(int level, const char * format ...)
{
    if ((level < LOG_MAX) && (level <= cec_log_level))
    {
        char tmp_buff[MAX_LOG_BUFF];
        va_list args;
        va_start(args, format);
        vsnprintf(tmp_buff,MAX_LOG_BUFF-1,format, args);
        va_end(args);
         __TIMESTAMP();printf("[%s]%s", _CEC_LOG_PREFIX, tmp_buff);
    }
}

/**
 * @brief This function is used to print the content of log buffer in hexadecimal
 * format.
 *
 * @param[in] buf Buffer where CEC log is stored.
 * @param[in] len Length of the buffer to be printed.
 *
 * @return None.
 */
void dump_buffer(unsigned char * buf, int len)
{
    if((cec_log_level < LOG_MAX) && (cec_log_level >= LOG_DEBUG))
    {
        for (int ii = 0; ii < len; ii++) {
            printf("%02X ", buf[ii]);
        }
    }
}


/** @} */
/** @} */
