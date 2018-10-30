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

    module:	ccsp_cwmp_ifo_sta.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This wrapper file defines the interface of the statistics of
        CCSP CWMP.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin Zhu

    ---------------------------------------------------------------

    revision:

        11/12/08    initial revision.
        02/12/09    add more statistics apis
        11/24/09    add more apis for "LastInformResponseTime" and "LastReceivedSPVTime"
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_IFO_STA_H
#define  _CCSP_CWMP_IFO_STA_H


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_ifo_interface.h"


/*
 * Define some const values that will be used in the os wrapper object definition.
 */
#define  CCSP_CWMP_STAT_INTERFACE_NAME                "cwmpIfStat"
#define  CCSP_CWMP_STAT_INTERFACE_ID                  0

/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support), we
 * have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  void
(*PFN_CWMPIFSTAT_ACTION)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_HANDLE
(*PFN_CWMPIFSTAT_GET_HANDLE)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  char*
(*PFN_CWMPIFSTAT_GET_STRING)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ULONG
(*PFN_CWMPIFSTAT_GET_UINT32)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ULONG
(*PFN_CWMPIFSTAT_INC_UINT32)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ULONG
(*PFN_CWMPIFSTAT_RESET)
    (
        ANSC_HANDLE                 hThisObject
    );

#define  CCSP_CWMP_STAT_INTERFACE_CLASS_CONTENT                                               \
    /* duplication of the base object class content */                                   \
    ANSCIFO_CLASS_CONTENT                                                                \
    /* start of object class content */                                                  \
    PFN_CWMPIFSTAT_ACTION         ConnectAcs;                                            \
    PFN_CWMPIFSTAT_INC_UINT32     IncTcpSuccess;                                         \
    PFN_CWMPIFSTAT_INC_UINT32     IncTcpFailure;                                          \
    PFN_CWMPIFSTAT_INC_UINT32     IncTlsFailure;                                          \
    PFN_CWMPIFSTAT_RESET          ResetStats;                                             \
    PFN_CWMPIFSTAT_GET_UINT32     GetTcpSuccessCount;                                     \
    PFN_CWMPIFSTAT_GET_UINT32     GetTcpFailureCount;                                     \
    PFN_CWMPIFSTAT_GET_UINT32     GetTlsFailureCount;                                     \
    PFN_CWMPIFSTAT_GET_HANDLE     GetLastConnectionTime;                                  \
    PFN_CWMPIFSTAT_GET_STRING     GetLastConnectionStatus;                                \
    PFN_CWMPIFSTAT_GET_HANDLE     GetLastInformResponseTime;                              \
    PFN_CWMPIFSTAT_GET_HANDLE     GetLastReceivedSPVTime;                                 \
    /* end of object class content */                                                     \

typedef  struct
_CCSP_CWMP_STAT_INTERFACE
{
    CCSP_CWMP_STAT_INTERFACE_CLASS_CONTENT
}
CCSP_CWMP_STAT_INTERFACE,  *PCCSP_CWMP_STAT_INTERFACE;

#define  ACCESS_CCSP_CWMP_STAT_INTERFACE(p)           \
         ACCESS_CONTAINER(p, CCSP_CWMP_STAT_INTERFACE, Linkage)


#endif
