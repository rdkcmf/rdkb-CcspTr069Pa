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

    module:	ccsp_cwmp_proco_internal_api.h

        For CCSP CWMP protocol implementation,

    ---------------------------------------------------------------

    description:

        This header file contains the prototype definition for all
        the internal functions provided by the CCSP CWMP Processor
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

        09/12/05    initial revision.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_PROCO_INTERNAL_API_
#define  _CCSP_CWMP_PROCO_INTERNAL_API_

#include "ccsp_message_bus.h"
#include "ccsp_base_api.h"
#include "ccsp_types.h"
#include "ccsp_tr069pa_mapper_def.h"

/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMPPO_STATES.C
***********************************************************/

ANSC_HANDLE
CcspCwmppoGetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCpeController
    );

BOOL
CcspCwmppoGetInitialContact
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSetInitialContact
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bEnabled
    );
    
BOOL
CcspCwmppoGetInitialContactFactory
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSetInitialContactFactory
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bEnabled
    );

BOOL
CcspCwmppoIsCwmpEnabled
    (
        ANSC_HANDLE                 hThisObject
    );

char*
CcspCwmppoGetAcsUrl
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSetAcsUrl
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pString,
        BOOL                        bAcquiredFromDhcp
    );

char*
CcspCwmppoGetAcsUsername
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSetAcsUsername
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pString
    );

char*
CcspCwmppoGetAcsPassword
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSetAcsPassword
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pString
    );

BOOL
CcspCwmppoGetPeriodicInformEnabled
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSetPeriodicInformEnabled
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bEnabled
    );

ULONG
CcspCwmppoGetPeriodicInformInterval
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSetPeriodicInformInterval
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulValue
    );

ANSC_HANDLE
CcspCwmppoGetPeriodicInformTime
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSetPeriodicInformTime
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hTime
    );

ANSC_STATUS
CcspCwmppoGetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

ANSC_STATUS
CcspCwmppoSetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

ANSC_STATUS
CcspCwmppoResetProperty
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoReset
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmppoGetCcspCwmpMpaIf
    (
        ANSC_HANDLE                 hThisObject
    );

/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMPPO_OPERATION.C
***********************************************************/

ANSC_STATUS
CcspCwmppoEngage
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRestart
    );

ANSC_STATUS
CcspCwmppoCancel
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSetupEnv
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRestart
    );

ANSC_STATUS
CcspCwmppoCloseEnv
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoPeriodicTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoScheduleTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoPendingInformTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSaveTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCommandKey,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        BOOL                        bIsDownload,
        PCCSP_CWMP_FAULT            pFault
    );

ANSC_STATUS
CcspCwmppoSaveValueChanged
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParameterName
    );

ANSC_STATUS
CcspCwmppoDiscardValueChanged
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParameterName
    );

ANSC_STATUS
CcspCwmppoLoadValueChanged
(
        ANSC_HANDLE                 hThisObject
);

ANSC_STATUS
CcspCwmppoLoadTransferComplete
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSaveAutonomousTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        char*                       AnnounceURL,
        char*                       TransferURL,
        char*                       FileType,
        unsigned int                FileSize,
        char*                       TargetFileName,
        ANSC_HANDLE                 hFault,      
        ANSC_HANDLE                 hStartTime,  
        ANSC_HANDLE                 hCompleteTime
    );

ANSC_STATUS
CcspCwmppoLoadAutonomousTransferComplete
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSaveDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hDsccReq
    );

ANSC_STATUS
CcspCwmppoLoadDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoSaveAutonomousDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hAdsccReq
    );

ANSC_STATUS
CcspCwmppoLoadAutonomousDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoRedeliverEvents
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoGetUndeliveredEvents
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

ANSC_STATUS
CcspCwmppoGetUndeliveredTcEvents
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

ANSC_STATUS
CcspCwmppoGetUndeliveredAtcEvents
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

ANSC_STATUS
CcspCwmppoGetUndeliveredDscEvents
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

ANSC_STATUS
CcspCwmppoGetUndeliveredAdscEvents
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

ANSC_STATUS
CcspCwmppoDiscardUndeliveredEvents
    (
        ANSC_HANDLE                 hThisObject
    );

/***********************************************************
      FUNCTIONS IMPLEMENTED IN CCSP_CWMPPO_MANAGEMENT.C
***********************************************************/

ANSC_HANDLE
CcspCwmppoAcqWmpSession
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmppoAcqWmpSession2
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmppoAcqWmpSession3
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoRelWmpSession
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

ANSC_STATUS
CcspCwmppoDelWmpSession
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

ANSC_STATUS
CcspCwmppoDelAllSessions
    (
        ANSC_HANDLE                 hThisObject
    );

ULONG
CcspCwmppoGetActiveWmpSessionCount
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bQueueLockAcquired
    );

ANSC_STATUS
CcspCwmppoInitParamAttrCache
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoPushAllVcToBackend
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoUpdateParamAttrCache
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hParamAttrArray,
        ULONG                       ulParamCount
    );

ULONG
CcspCwmppoCheckParamAttrCache
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName
    );

ANSC_STATUS
CcspCwmppoSyncNamespacesWithCR
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRemoteOnly
    );

CCSP_BOOL
CcspCwmppoSyncRemoteNamespace
    (
        CCSP_HANDLE                 pContext,
        PCCSP_TR069_PARAM_INFO      pParamInfo
    );

BOOL
CcspCwmppoHasPendingInform
    (
        ANSC_HANDLE                 hThisObject
    );

/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMPPO_PROCESS.C
***********************************************************/

ANSC_STATUS
CcspCwmppoConfigPeriodicInform
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoScheduleInform
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulDelaySeconds,
        char*                       pCommandKey
    );

ANSC_STATUS
CcspCwmppoSignalSession
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

ANSC_STATUS
CcspCwmppoAsyncConnectTask
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMPPO_MPAIF.C
***********************************************************/

BOOL
CcspCwmppoMpaLockWriteAccess
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoMpaUnlockWriteAccess
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoMpaSetParameterValues
    (
        ANSC_HANDLE                 hThisObject,
        void*                       pParamValueArray,
        ULONG                       ulArraySize,
        int*                        piStatus,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

ANSC_STATUS
CcspCwmppoMpaSetParameterValuesWithWriteID
    (
        ANSC_HANDLE                 hThisObject,
        void*                       pParamValueArray,
        ULONG                       ulArraySize,
        ULONG                       ulWriteID,
        int*                        piStatus,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

ANSC_STATUS
CcspCwmppoMpaGetParameterValues
    (
        ANSC_HANDLE                 hThisObject,
        SLAP_STRING_ARRAY*          pParamNameArray,
		ULONG						uMaxEntry,
        void**                      ppParamValueArray,
        PULONG                      pulArraySize,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

ANSC_STATUS
CcspCwmppoMpaGetParameterNames
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamPath,
        BOOL                        bNextLevel,
        void**                      ppParamInfoArray,
        PULONG                      pulArraySize,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

ANSC_STATUS
CcspCwmppoMpaSetParameterAttributes
    (
        ANSC_HANDLE                 hThisObject,
        void*                       pSetParamAttribArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

ANSC_STATUS
CcspCwmppoMpaGetParameterAttributes
    (
        ANSC_HANDLE                 hThisObject,
        SLAP_STRING_ARRAY*          pParamNameArray,
		ULONG						uMaxEntry,
        void**                      ppParamAttribArray,
        PULONG                      pulArraySize,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

ANSC_STATUS
CcspCwmppoMpaAddObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pObjName,
        PULONG                      pulObjInsNumber,
        int*                        piStatus,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

ANSC_STATUS
CcspCwmppoMpaDeleteObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pObjName,
        int*                        piStatus,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMPPO_PROCESS.C
***********************************************************/

ANSC_STATUS
CcspCwmppoGetAcsInfo
    (
        ANSC_HANDLE                 hThisObject
    );

void
CcspCwmppoMsValueChanged
    (
        ANSC_HANDLE                 hThisObject,
        int                         hMsvcObjectID
    );

/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMPPO_EVENT.C
***********************************************************/

void 
CcspCwmppoParamValueChangedCB
    (
        parameterSigStruct_t*       val,
        int                         size,
        void*                       user_data
    );

void 
CcspCwmppoSysReadySignalCB
    (
        void*                       user_data
    );

void 
CcspCwmppoDiagCompleteSignalCB
    (
        void*                       user_data
    );

void 
CcspCwmppoProcessSysReadySignal
    (
		void*	cbContext
    );

void waitUntilSystemReady(	void*	cbContext);

int checkIfSystemReady(void);

#endif
