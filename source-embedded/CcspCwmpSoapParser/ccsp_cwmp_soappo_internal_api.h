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

    module: ccsp_cwmp_soappo_internal_api.h

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This header file contains the prototype definition for all
        the internal functions provided by the CCSP CWMP SoapParser
        Parser Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/06/05    initial revision.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/
#ifndef  _CCSP_CWMP_SOAPPO_INTERNAL_API_
#define  _CCSP_CWMP_SOAPPO_INTERNAL_API_

#include "ccsp_cwmp_definitions_soap.h"

/***********************************************************
         FUNCTIONS IMPLEMENTED IN CCSP_CWMP_SOAPPO_STATES.C
***********************************************************/

ANSC_STATUS
CcspCwmpSoappoReset
    (
        ANSC_HANDLE                 hThisObject
    );

/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_SOAPPO_BUILD.C
***********************************************************/
char*
CcspCwmpSoappoBuildSoapReq_GetRpcMethods
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID
    );

char*
CcspCwmpSoappoBuildSoapReq_Inform
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

char*
CcspCwmpSoappoBuildSoapReq_TransferComplete
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pCommandKey,
        ANSC_HANDLE                 hFault,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime
    );

char*
CcspCwmpSoappoBuildSoapReq_Kicked
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pCommand,
        char*                       pReferer,
        char*                       pArg,
        char*                       pNext
    );

char*
CcspCwmpSoappoBuildSoapReq_RequestDownload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pFileType,
        ANSC_HANDLE                 hFileTypeArgArray,
        ULONG                       ulArraySize
    );

char*
CcspCwmpSoappoBuildSoapReq_AutonomousTransferComplete
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

char*
CcspCwmpSoappoBuildSoapReq_DUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hDsccReq,
        char*                       pCommandKey
    );

char*
CcspCwmpSoappoBuildSoapReq_AutonomousDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hAdsccReq
    );


char*
CcspCwmpSoappoBuildSoapRep_Error
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pMethodName,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_GetRPCMethods
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        SLAP_STRING_ARRAY*          pMethodList
    );

char*
CcspCwmpSoappoBuildSoapRep_SetParameterValues
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_GetParameterValues
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hParamValueArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_GetParameterNames
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hParamInfoArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_SetParameterAttributes
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_GetParameterAttributes
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hParamAttribArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_AddObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ULONG                       ulInstanceNumber,
        int                         iStatus,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_DeleteObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_Download
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_Reboot
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_ChangeDUState
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_GetQueuedTransfers
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hQueuedTransferArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_ScheduleInform
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_SetVouchers
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_GetOptions
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hOptionArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_Upload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRep_FactoryReset
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

char*
CcspCwmpSoappoBuildSoapRepFault
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    );

/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_SOAPPO_PROCESS.C
***********************************************************/

ANSC_STATUS
CcspCwmpSoappoProcessSoapHeader
    (
        ANSC_HANDLE                 hCwmpSoapHeader,
        PCHAR                       pNameSpace,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_SetParameterValues
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_GetParameterValues
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_GetParameterNames
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_SetParameterAttributes
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_GetParameterAttributes
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_AddObject
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_DeleteObject
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_Reboot
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_Download
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_Upload
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_ScheduleInform
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_SetVouchers
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessRequest_GetOptions
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    );


ANSC_STATUS
CcspCwmpSoappoProcessRequest
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        PCHAR                       pNameSpace,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_HANDLE
CcspCwmpSoappoProcessResponse_GetRPCMethods
    (
        ANSC_HANDLE                 hXmlHandle
    );

ULONG
CcspCwmpSoappoProcessResponse_Inform
    (
        ANSC_HANDLE                 hXmlHandle
    );

PCHAR
CcspCwmpSoappoProcessResponse_Kicked
    (
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessResponse
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        PCHAR                       pNameSpace,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessFault
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        PCHAR                       pNameSpace,
        ANSC_HANDLE                 hXmlHandle
    );

ANSC_STATUS
CcspCwmpSoappoProcessSingleEnvelope
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        PCHAR                       pNameSpace,
        PCHAR                       pEnvelopeMessage,
        ULONG                       uMsgSize,
        PBOOLEAN                    pIsFault
    );

ANSC_STATUS
CcspCwmpSoappoProcessSoapEnvelopes
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pSoapEnvelopes,      /* may contain multiple SOAP envelopes */
        ULONG                       uMaxEnvelope,
        ANSC_HANDLE                 hCcspCwmpMcoIf
    );

/***********************************************************
         FUNCTIONS IMPLEMENTED IN CCSP_CWMP_SOAPPO_UTILITY.C
***********************************************************/
char*
CcspCwmpSoappoUtilGetNodeNameWithoutNS
    (
        PCHAR                       pNodeName
    );

ANSC_HANDLE
CcspCwmpSoappoUtilFindChildNode
    (
        ANSC_HANDLE                 hXmlNode,
        PCHAR                       pInputName
    );

ANSC_STATUS
CcspCwmpSoappoUtilGetSoapNamespace
    (
        PCHAR                       pEnvelopeMessage,
        PCHAR                       pOutNamespaceName
    );

ULONG
CcspCwmpSoappoUtilGetCwmpMethod
    (
        PCHAR                       pMethodName,
        BOOL                        bFromServer
    );

ANSC_STATUS
CcspCwmpSoappoUtilGetCwmpMethodName
    (
        ULONG                       uMethod,
        BOOL                        bFromServer,
        PCHAR                       pOutBuffer
    );

ANSC_STATUS
CcspCwmpSoappoUtilGetParamValue
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        ANSC_HANDLE                 hXmlHandle,
        ANSC_HANDLE                 hParamHandle
    );

ANSC_STATUS
CcspCwmpSoappoUtilGetParamAttribute
    (
        ANSC_HANDLE                 hXmlHandle,
        ANSC_HANDLE                 hAttrHandle
    );

ANSC_STATUS
CcspCwmpSoappoUtilAddSoapHeader
    (
        ANSC_HANDLE                 hSoapEnvelopeHandle,
        char*                       pRequestID
    );

ANSC_HANDLE
CcspCwmpSoappoUtilCreateSoapEnvelopeHandle
    (
        char*                       pRequestID
    );

ANSC_STATUS
CcspCwmpSoappoUtilProcessInvalidArgumentRequest
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        char*                       pMethodName
    );

ANSC_STATUS
CcspCwmpSoappoUtilProcessInvalidArgumentSPVRequest
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        char*                       pParamName,
        ULONG                       CwmpSpvFaultCode
    );

ANSC_STATUS
CcspCwmpSoappoUtilProcessTooManyEnvelopes
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf
    );

#endif
