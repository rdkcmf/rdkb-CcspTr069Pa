/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
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

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/


/**********************************************************************

    module: ccsp_tr069pa_wrapper_api.h

        For CCSP TR-069 Protocol Agent 

    ---------------------------------------------------------------

    description:

        This wrapper file defines all the platform-independent
        functions on memory management.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        06/17/2011    initial revision.

**********************************************************************/

#ifndef  _CCSP_TR069PA_WRAPPER_API_
#define  _CCSP_TR069PA_WRAPPER_API_

#include "ccsp_trace.h"

#ifndef SAFEC_DUMMY_API
#include "safe_str_lib.h"
#include "safe_mem_lib.h"
#endif

#include "ccsp_memory.h"

/*
 * g_Tr069_PA_Name needs to be set when TR-069 PA starts up as the first thing,
 * we expect it to be set as program argument. Since we define the name as global
 * variable, there cannot be more than one TR-069 PA resides in the same process.
 */

extern char *g_Tr069_PA_Name;

/*
 * Logging wrapper APIs
 */
#define  CcspTr069PaTraceEmergency(msg)                         \
    CcspTraceEmergency2(g_Tr069_PA_Name, msg)

#define  CcspTr069PaTraceAlert(msg)                             \
    CcspTraceAlert2(g_Tr069_PA_Name, msg)

#define  CcspTr069PaTraceCritical(msg)                          \
    CcspTraceCritical2(g_Tr069_PA_Name, msg)

#define  CcspTr069PaTraceError(msg)                             \
    CcspTraceError3(g_Tr069_PA_Name, msg)

#define  CcspTr069PaTraceWarning(msg)                           \
    CcspTraceWarning2(g_Tr069_PA_Name, msg)

#define  CcspTr069PaTraceNotice(msg)                            \
    CcspTraceNotice2(g_Tr069_PA_Name, msg)

#define  CcspTr069PaTraceDebug(msg)                             \
    CcspTraceDebug2(g_Tr069_PA_Name, msg)

#define  CcspTr069PaTraceInfo(msg)                              \
    CcspTraceInfo2(g_Tr069_PA_Name, msg)


/*
 * SAFECLIB Erro Handling Logging APIs
 */
#define RDK_SAFECLIB_ERR()  printf("safeclib error at %s %s:%d\n", __FILE__, __FUNCTION__, __LINE__)
 
#define ERR_CHK(rc)                                             \
    if((long)rc !=EOK) {                                              \
        RDK_SAFECLIB_ERR();                                     \
    }

/*This is required to support CiscoXB3 platform which cannot include safeclib due to the image size constraints */
#ifdef SAFEC_DUMMY_API
typedef int errno_t;
#define EOK 0

#define strcpy_s(dst,max,src) EOK; \
 strcpy(dst,src);
#define strncpy_s(dst,max,src,len)  EOK; \
 strncpy(dst,src,len);
#define strcat_s(dst,max,src) EOK; \
 strcat(dst,src);
#define strncat_s(dst,max,src,len) EOK; \
 strncat(dst,src,len);
#define memset_s(dst,max_1,c,max) EOK; \
 memset(dst,c,max);

errno_t strcmp_s(const char *,int,const char *,int *);
errno_t strcasecmp_s(const char * ,int , const char * ,int *);
#endif

#endif


