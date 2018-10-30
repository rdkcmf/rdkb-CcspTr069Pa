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

    module:	ccsp_cwmp_soappo_interface.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This wrapper file defines all the platform-independent
        functions and macros for the CCSP CWMP Soap Parser Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/02/05    initial revision.
        01/02/11    Kang added support of following RPCs 
                        AutonomousTransferComplete
                        DUStateChangeComplete 
                        AutonomousDUStateChangeComplete
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_SOAPPO_INTERFACE_
#define  _CCSP_CWMP_SOAPPO_INTERFACE_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_co_interface.h"
#include "ansc_co_external_api.h"
#include "ccsp_cwmp_properties.h"


/***********************************************************
         CCSP CWMP SOAP PARSER COMPONENT OBJECT DEFINITION
***********************************************************/

/*
 * Define some const values that will be used in the object mapper object definition.
 */

/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support),
 * we have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  ANSC_HANDLE
(*PFN_CWMPSOAPPO_GET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSOAPPO_SET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hContext
    );

typedef  ANSC_HANDLE
(*PFN_CWMPSOAPPO_GET_IF)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSOAPPO_SET_IF)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hInterface
    );

typedef  ANSC_STATUS
(*PFN_CWMPSOAPPO_GET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPSOAPPO_SET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPSOAPPO_RESET)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSOAPPO_PROCESS)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pSoapEnvelopes,     /* may contain multiple SOAP envelopes */
        ULONG                       ulMaxEnvelopes,
        ANSC_HANDLE                 hCcspCwmpMcoIf
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REQ1)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REQ2)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hDeviceId,
        ANSC_HANDLE                 hEventArray,
        ULONG                       ulEventCount,
        ULONG                       ulMaxEnvelopes,
        ANSC_HANDLE                 hCurrentTime,
        int                         iTimeZoneOffset,
        ULONG                       ulRetryCount,
        ANSC_HANDLE                 hParamValueArray,
        ULONG                       ulArraySize
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REQ3)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pCommandKey,
        ANSC_HANDLE                 hFault,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REQ4)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pCommand,
        char*                       pReferer,
        char*                       pArg,
        char*                       pNext
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REQ5)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pFileType,
        ANSC_HANDLE                 hFileTypeArgArray,
        ULONG                       ulArraySize
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REQ6)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hFault,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
		BOOL						bIsDownload,
        char*                       AnnounceURL,
        char*                       TransferURL,
        char*                       FileType,
        unsigned int                FileSize,
        char*                       TargetFileName
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REQ7)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hDsccReq,
        char*                       pCommandKey
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REQ8)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hAdsccReq
    );


typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP0)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pMethodName,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP1)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        SLAP_STRING_ARRAY*          pMethodList
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP2)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP3)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hParamValueArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP4)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hParamInfoArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP5)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP6)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hParamAttribArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP7)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ULONG                       ulInstanceNumber,
        int                         iStatus,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP8)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP9)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP10)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP11)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hQueuedTransferArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP12)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP13)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP14)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hOptionArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP15)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        ANSC_HANDLE                 hSoapFault
    );

typedef  char*
(*PFN_CWMPSOAPPO_BUILD_REP16)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

/*
 * The CPE WAN Management Protocol defines SOAP 1.1 as the encoding syntax to transport the RPC
 * method calls and responses. The following describes the mapping of RPC methods to SOAP encoding:
 *
 *      - The encoding must use the standard SOAP 1.1 envelope and serialization
 *        namespaces.
 *      - All elements and attributes defined as part of this version of the CPE WAN
 *        Management Protocol are associated with the following namespace identifier:
 *        "urn:dslforum-org:cwmp-1-0".
 *      - The data types used correspond directly to the data types defined in the SOAP
 *        1.1 serialization namespace.
 *      - For an array argument, the given argument name corresponds to the name of the
 *        overall array element.
 *      - .....
 */
#define  CCSP_CWMP_SOAP_PARSER_CLASS_CONTENT                                                     \
    /* duplication of the base object class content */                                      \
    ANSCCO_CLASS_CONTENT                                                                    \
    /* start of object class content */                                                     \
    PFN_CWMPSOAPPO_RESET            Reset;                                                  \
                                                                                            \
    PFN_CWMPSOAPPO_PROCESS          ProcessSoapEnvelopes;                                   \
                                                                                            \
    PFN_CWMPSOAPPO_BUILD_REQ1       BuildSoapReq_GetRpcMethods;                             \
    PFN_CWMPSOAPPO_BUILD_REQ2       BuildSoapReq_Inform;                                    \
    PFN_CWMPSOAPPO_BUILD_REQ3       BuildSoapReq_TransferComplete;                          \
    PFN_CWMPSOAPPO_BUILD_REQ4       BuildSoapReq_Kicked;                                    \
    PFN_CWMPSOAPPO_BUILD_REQ5       BuildSoapReq_RequestDownload;                           \
    PFN_CWMPSOAPPO_BUILD_REQ6       BuildSoapReq_AutonomousTransferComplete;                \
    PFN_CWMPSOAPPO_BUILD_REQ7       BuildSoapReq_DUStateChangeComplete;                     \
    PFN_CWMPSOAPPO_BUILD_REQ8       BuildSoapReq_AutonomousDUStateChangeComplete;           \
                                                                                            \
    PFN_CWMPSOAPPO_BUILD_REP0       BuildSoapRep_Error;                                     \
    PFN_CWMPSOAPPO_BUILD_REP1       BuildSoapRep_GetRpcMethods;                             \
    PFN_CWMPSOAPPO_BUILD_REP2       BuildSoapRep_SetParameterValues;                        \
    PFN_CWMPSOAPPO_BUILD_REP3       BuildSoapRep_GetParameterValues;                        \
    PFN_CWMPSOAPPO_BUILD_REP4       BuildSoapRep_GetParameterNames;                         \
    PFN_CWMPSOAPPO_BUILD_REP5       BuildSoapRep_SetParameterAttributes;                    \
    PFN_CWMPSOAPPO_BUILD_REP6       BuildSoapRep_GetParameterAttributes;                    \
    PFN_CWMPSOAPPO_BUILD_REP7       BuildSoapRep_AddObject;                                 \
    PFN_CWMPSOAPPO_BUILD_REP8       BuildSoapRep_DeleteObject;                              \
    PFN_CWMPSOAPPO_BUILD_REP9       BuildSoapRep_Download;                                  \
    PFN_CWMPSOAPPO_BUILD_REP10      BuildSoapRep_Reboot;                                    \
    PFN_CWMPSOAPPO_BUILD_REP10      BuildSoapRep_ChangeDUState;                             \
    PFN_CWMPSOAPPO_BUILD_REP11      BuildSoapRep_GetQueuedTransfers;                        \
    PFN_CWMPSOAPPO_BUILD_REP12      BuildSoapRep_ScheduleInform;                            \
    PFN_CWMPSOAPPO_BUILD_REP13      BuildSoapRep_SetVouchers;                               \
    PFN_CWMPSOAPPO_BUILD_REP14      BuildSoapRep_GetOptions;                                \
    PFN_CWMPSOAPPO_BUILD_REP15      BuildSoapRep_Upload;                                    \
    PFN_CWMPSOAPPO_BUILD_REP16      BuildSoapRep_FactoryReset;                              \
    /* end of object class content */                                                       \

typedef  struct
_CCSP_CWMP_SOAP_PARSER_OBJECT
{
    CCSP_CWMP_SOAP_PARSER_CLASS_CONTENT
}
CCSP_CWMP_SOAP_PARSER_OBJECT,  *PCCSP_CWMP_SOAP_PARSER_OBJECT;

#define  ACCESS_CCSP_CWMP_SOAP_PARSER_OBJECT(p)          \
         ACCESS_CONTAINER(p, CCSP_CWMP_SOAP_PARSER_OBJECT, Linkage)


#endif
