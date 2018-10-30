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

    module:	ccsp_cwmp_sesso_internal_api.h

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This header file contains the prototype definition for all
        the internal functions provided by the CCSP CWMP Session
        Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/13/05    initial revision.
        06/21/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_SESSO_INTERNAL_API_
#define  _CCSP_CWMP_SESSO_INTERNAL_API_


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMPSO_STATES.C
***********************************************************/

ANSC_HANDLE
CcspCwmpsoGetCcspCwmpAcsConnection
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpsoGetCcspCwmpMcoIf
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpsoGetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoSetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCpeController
    );

ANSC_HANDLE
CcspCwmpsoGetCcspCwmpProcessor
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoSetCcspCwmpProcessor
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpProcessor
    );

ANSC_STATUS
CcspCwmpsoReset
    (
        ANSC_HANDLE                 hThisObject
    );

/***********************************************************
       FUNCTIONS IMPLEMENTED IN CCSP_CWMPSO_OPERATION.C
***********************************************************/

ANSC_STATUS
CcspCwmpsoAcqAccess
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoRelAccess
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoSessionTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoSessionRetryTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoDelayedActiveNotifTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoStartRetryTimer
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoStopRetryTimer
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoCancelRetryDelay
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bConnectAgain
    );

ANSC_STATUS
CcspCwmpsoStopDelayedActiveNotifTimer
	(
		ANSC_HANDLE					hThisObject
	);

/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMPSO_CONTROL.C
***********************************************************/

BOOL
CcspCwmpsoIsAcsConnected
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoConnectToAcs
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoCloseConnection
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
      FUNCTIONS IMPLEMENTED IN CCSP_CWMPSO_MANAGEMENT.C
***********************************************************/

ANSC_STATUS
CcspCwmpsoAddCwmpEvent
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCwmpEvent,
        BOOL                        bConnectNow
    );

ANSC_STATUS
CcspCwmpsoDiscardCwmpEvent
    (
        ANSC_HANDLE                 hThisObject,
        int                         EventCode
    );

ANSC_STATUS
CcspCwmpsoDelAllEvents
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoAddModifiedParameter
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        char*                       pParamValue,
        ULONG                       CwmpDataType,
        BOOL                        bConnectNow
    );

ANSC_STATUS
CcspCwmpsoDelAllParameters
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoSaveCwmpEvent
    (
        ANSC_HANDLE                 hThisObject
    );

BOOL
CcspCwmpsoInformPending
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMPSO_INVOKE.C
***********************************************************/

ANSC_STATUS
CcspCwmpsoGetRpcMethods
    (
        ANSC_HANDLE                 hThisObject,
        SLAP_STRING_ARRAY**         ppMethodList,
        PULONG                      pulErrorCode
    );

ANSC_STATUS
CcspCwmpsoInform
    (
        ANSC_HANDLE                 hThisObject,
        PULONG                      pulErrorCode
    );

ANSC_STATUS
CcspCwmpsoTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        char*                       pCommandKey,
        ANSC_HANDLE                 hFault,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode
    );

ANSC_STATUS
CcspCwmpsoKicked
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCommand,
        char*                       pReferer,
        char*                       pArg,
        char*                       pNext,
        char**                      ppNextUrl,
        PULONG                      pulErrorCode
    );

ANSC_STATUS
CcspCwmpsoRequestDownload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pFileType,
        ANSC_HANDLE                 hFileTypeArgArray,
        ULONG                       ulArraySize,
        PULONG                      pulErrorCode
    );

ANSC_STATUS
CcspCwmpsoAutonomousTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        ANSC_HANDLE                 hFault,             
        ANSC_HANDLE                 hStartTime,         
        ANSC_HANDLE                 hCompleteTime,      
        char*                       AnnounceURL,
        char*                       TransferURL,
        char*                       FileType,
        unsigned int                FileSize,
        char*                       TargetFileName,
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode
    );

ANSC_STATUS
CcspCwmpsoDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hDsccReq,
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode
    );

ANSC_STATUS
CcspCwmpsoAutonomousDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hAdsccReq,
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode
    );


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMPSO_PROCESS.C
***********************************************************/

ANSC_STATUS
CcspCwmpsoRecvSoapMessage
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pMessage
    );

ANSC_STATUS
CcspCwmpsoNotifySessionClosed
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpsoAsyncProcessTask
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
         FUNCTIONS IMPLEMENTED IN CCSP_CWMPSO_MCOIF.C
***********************************************************/

ANSC_STATUS
CcspCwmpsoMcoNotifyAcsStatus
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bNoMoreRequests,
        BOOL                        bHoldRequests
    );

ULONG
CcspCwmpsoMcoGetNextMethod
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID
    );

ANSC_STATUS
CcspCwmpsoMcoProcessSoapResponse
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSoapResponse
    );

ANSC_STATUS
CcspCwmpsoMcoProcessSoapError
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pMethodName,
        ANSC_HANDLE                 hSoapFault
    );

ANSC_STATUS
CcspCwmpsoMcoInvokeUnknownMethod
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pMethodName
    );

ANSC_STATUS
CcspCwmpsoMcoGetRpcMethods
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID
    );

ANSC_STATUS
CcspCwmpsoMcoSetParameterValues
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hParamValueArray,
        ULONG                       ulArraySize,
        char*                       pParamKey
    );

ANSC_STATUS
CcspCwmpsoMcoGetParameterValues
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        SLAP_STRING_ARRAY*          pParamNameArray
    );

ANSC_STATUS
CcspCwmpsoMcoGetParameterNames
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pParamPath,
        BOOL                        bNextLevel
    );

ANSC_STATUS
CcspCwmpsoMcoSetParameterAttributes
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSetParamAttribArray,
        ULONG                       ulArraySize
    );

ANSC_STATUS
CcspCwmpsoMcoGetParameterAttributes
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        SLAP_STRING_ARRAY*          pParamNameArray
    );

ANSC_STATUS
CcspCwmpsoMcoAddObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pObjectName,
        char*                       pParamKey
    );

ANSC_STATUS
CcspCwmpsoMcoDeleteObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pObjectName,
        char*                       pParamKey
    );

ANSC_STATUS
CcspCwmpsoMcoDownload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hDownloadReq
    );

ANSC_STATUS
CcspCwmpsoMcoReboot
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pCommandKey
    );

ANSC_STATUS CcspCwmpSetRebootReason(ANSC_HANDLE                 hThisObject);

ANSC_STATUS
CcspCwmpsoMcoChangeDUState
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hDownloadReq
    );

ANSC_STATUS
CcspCwmpsoMcoGetQueuedTransfers
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID
    );

ANSC_STATUS
CcspCwmpsoMcoScheduleInform
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ULONG                       ulDelaySeconds,
        char*                       pCommandKey
    );

ANSC_STATUS
CcspCwmpsoMcoSetVouchers
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        SLAP_STRING_ARRAY*          pVoucherList
    );

ANSC_STATUS
CcspCwmpsoMcoGetOptions
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pOptionName
    );

ANSC_STATUS
CcspCwmpsoMcoUpload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hUploadReq
    );

ANSC_STATUS
CcspCwmpsoMcoFactoryReset
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID
    );


ULONG
CcspCwmpsoMcoGetParamDataType
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName
    );

#endif
