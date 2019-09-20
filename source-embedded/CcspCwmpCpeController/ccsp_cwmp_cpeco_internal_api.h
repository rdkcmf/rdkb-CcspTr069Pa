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

    module: ccsp_cwmp_cpeco_internal_api.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This header file contains the prototype definition for all
        the internal functions provided by the CCSP CWMP Cpe Controller
        Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin  Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/09/05    initial revision.
        02/12/09    add more statistics apis
        04/25/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP COSA 2.0 CDMMM.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_CPECO_INTERNAL_API_
#define  _CCSP_CWMP_CPECO_INTERNAL_API_


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_CPECO_STATES.C
***********************************************************/

ANSC_HANDLE
CcspCwmpCpecoGetCcspCwmpAcsBroker
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpCpecoGetCcspCwmpProcessor
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpCpecoGetCcspCwmpSoapParser
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpCpecoGetCcspCwmpStunManager
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpCpecoGetCcspCwmpTcpConnReqHandler
    (
        ANSC_HANDLE                 hThisObject
    );

char*
CcspCwmpCpecoGetRootObject
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoSetRootObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRootObject
    );

ANSC_STATUS
CcspCwmpCpecoGetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

ANSC_STATUS
CcspCwmpCpecoSetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

ANSC_STATUS
CcspCwmpCpecoResetProperty
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoReset
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpCpecoGetCcspCwmpCfgIf
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoSetCcspCwmpCfgIf
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hInterface
    );


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_CPECO_ACCESS.C
***********************************************************/

ANSC_HANDLE
CcspCwmpCpecoGetCcspCwmpMcoIf
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpCpecoGetCcspCwmpMsoIf
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoGetParamValues
    (
        ANSC_HANDLE                 hThisObject,
        char**                      ppParamNames,
        int                         NumOfParams,
        PCCSP_VARIABLE              pParamValues
    );

ANSC_STATUS
CcspCwmpCpecoGetParamStringValues
    (
        ANSC_HANDLE                 hThisObject,
        char**                      ppParamNames,
        int                         NumOfParams,
        char**                      pParamValues
    );

ANSC_STATUS
CcspCwmpCpecoGetParamStringValue
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        char**                      pParamValue
    );

ANSC_STATUS
CcspCwmpCpecoSetParamValues
    (
        ANSC_HANDLE                 hThisObject,
        char**                      ppParamNames,
        int                         NumOfParams,
        PCCSP_VARIABLE              pParamValues
    );

ANSC_STATUS
CcspCwmpCpecoAddObjects
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pObjectName,
        int                         NumInstances,
        PULONG                      pInsNumbers,
        PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
    );

ANSC_STATUS
CcspCwmpCpecoDeleteObjects
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pObjectName,
        int                         NumInstances,
        PULONG                      pInsNumbers,
        PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault,
        ULONG                       DelaySeconds
    );

ANSC_STATUS
CcspCwmpCpecoMonitorOpState
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bMonitor,
        char*                       pObjectName,
        int                         NumInstances,
        PULONG                      pInsNumbers,
        char*                       pStateParamName,
        PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
    );

ANSC_STATUS
CcspCwmpCpecoRegisterPA
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRegister
    );

ANSC_STATUS
CcspCwmpCpecoSetPAName
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

char*
CcspCwmpCpecoGetPAName
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoSetSubsysName
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

char*
CcspCwmpCpecoGetSubsysName
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoSetCRName
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

char*
CcspCwmpCpecoGetCRName
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoSetCRBusPath
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pSubsystem,
        char*                       pName
    );

char*
CcspCwmpCpecoGetCRBusPath
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pSubsystem
    );

ANSC_STATUS
CcspCwmpCpecoSetPAMapperFile
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

ANSC_STATUS
CcspCwmpCpecoSetPACustomMapperFile
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

ANSC_STATUS
CcspCwmpCpecoSetSDMXmlFilename
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

char*
CcspCwmpCpecoGetSDMXmlFilename
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoSetOutboundIfName
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

char*
CcspCwmpCpecoGetOutboundIfName
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpCpecoGetMsgBusHandle
    (
        ANSC_HANDLE                 hThisObject
    );

int
CcspCwmpCpecoGetParamNotification
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName
    );

ANSC_STATUS
CcspCwmpCpecoSetParamNotification
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        int                         Notification
    );

int
CcspCwmpCpecoGetParamDataType
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName
    );

ANSC_STATUS
CcspCwmpCpecoSaveCfgToPsm
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCfgKey,
        char*                       pCfgValue
    );

char*
CcspCwmpCpecoLoadCfgFromPsm
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCfgKey
    );

ANSC_STATUS
CcspCwmpCpecoSetParameterKey
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParameterKey
    );

/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_CPECO_OPERATION.C
***********************************************************/

ANSC_STATUS
CcspCwmpCpecoEngage
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoCancel
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoSetupEnv
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoCloseEnv
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoStartCwmpTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_CPECO_CONTROL.C
***********************************************************/

ANSC_STATUS
CcspCwmpCpecoLoadEvents
    (
        ANSC_HANDLE                 hCcspCwmpMsoIf,
        char*                       pSavedEvents
    );

ANSC_STATUS
CcspCwmpCpecoStartCWMP
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bInformAcs,
        BOOL                        bRestart
    );

ANSC_STATUS
CcspCwmpCpecoStopCWMP
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpCpecoRestartCWMP
    (
        ANSC_HANDLE                         hThisObject,
        BOOL                                bInformAcs
    );

ANSC_STATUS
CcspCwmpCpecoInformNow
    (
        ANSC_HANDLE                 hThisObject
    );

void
CcspCwmpCpecoNotifyEvent
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulEvent,
        ANSC_HANDLE                 hEventContext
    );
    
ANSC_STATUS
CcspCwmpCpecoAcqCrSessionID
    (
        ANSC_HANDLE                 hThisObject,
        int                         Priority
    );

ANSC_STATUS
CcspCwmpCpecoRelCrSessionID
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       CrSessionID
    );


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_CPECO_STAIF.C
***********************************************************/
void
CcspCwmpCpecoStatIfConnectAcs
    (
        ANSC_HANDLE                 hThisObject
    );

ULONG
CcspCwmpCpecoStatIfIncTcpFailure
    (
        ANSC_HANDLE                 hThisObject
    );

ULONG
CcspCwmpCpecoStatIfIncTlsFailure
    (
        ANSC_HANDLE                 hThisObject
    );

ULONG
CcspCwmpCpecoStatIfIncTcpSuccess
    (
        ANSC_HANDLE                 hThisObject
    );

ULONG
CcspCwmpCpecoStatIfGetTcpSuccessCount
    (
        ANSC_HANDLE                 hThisObject
    );

ULONG
CcspCwmpCpecoStatIfGetTcpFailureCount
    (
        ANSC_HANDLE                 hThisObject
    );

ULONG
CcspCwmpCpecoStatIfGetTlsFailureCount
    (
        ANSC_HANDLE                 hThisObject
    );

ULONG
CcspCwmpCpecoStatIfResetStats
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpCpecoStatIfGetLastConnectionTime
    (
        ANSC_HANDLE                 hThisObject
    );

char*
CcspCwmpCpecoStatIfGetLastConnectionStatus
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpCpecoStatIfGetLastInformResponseTime
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpCpecoStatIfGetLastReceivedSPVTime
    (
        ANSC_HANDLE                 hThisObject
    );

#endif
