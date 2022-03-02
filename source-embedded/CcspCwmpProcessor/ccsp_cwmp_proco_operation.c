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

    module:	ccsp_cwmp_proco_operation.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced operation functions
        of the CCSP CWMP Processor Object.

        *   CcspCwmppoEngage
        *   CcspCwmppoCancel
        *   CcspCwmppoSetupEnv
        *   CcspCwmppoCloseEnv
        *   CcspCwmppoRedeliverEvents
        *   CcspCwmppoGetUnderlivedEvents
        *   CcspCwmppoGetUnderlivedTcEvents
        *   CcspCwmppoGetUnderlivedAtcEvents
        *   CcspCwmppoGetUnderlivedDscEvents
        *   CcspCwmppoGetUnderlivedAdscEvents
        *   CcspCwmppoPeriodicTimerInvoke
        *   CcspCwmppoScheduleTimerInvoke
        *   CcspCwmppoPendingInformTimerInvoke
        *   CcspCwmppoSaveTransferComplete
        *   CcspCwmppoLoadTransferComplete
        *   CcspCwmppoSaveAutonomousTransferComplete
        *   CcspCwmppoLoadAutonomousTransferComplete
        *   CcspCwmppoSaveDUStateChangeComplete
        *   CcspCwmppoLoadDUStateChangeComplete
        *   CcspCwmppoSaveAutonomousDUStateChangeComplete
        *   CcspCwmppoLoadAutonmousDUStateChangeComplete
        *   CcspCwmppoDiscardUndeliveredEvents

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
        07/28/11    Process namespace value changes, including
                    monitoring completeness of RPC such as
                    Download, Upload, ChangeDUState etc.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/

#include "ccsp_cwmp_proco_global.h"

#define  CcspCwmppoMapParamInstNumDmIntToCwmp(pParam)                               \
            {                                                                       \
                CCSP_STRING     pReturnStr  = NULL;                                 \
                                                                                    \
                CcspTr069PaTraceDebug(("%s - Param DmInt to CWMP\n", __FUNCTION__));\
                                                                                    \
                pReturnStr =                                                        \
                    CcspTr069PA_MapInstNumDmIntToCwmp(pParam);                      \
                                                                                    \
                if ( pReturnStr )                                                   \
                {                                                                   \
                    /* we are responsible for releasing the original string */      \
                    AnscFreeMemory(pParam);                                  \
                    pParam = pReturnStr;                                            \
                }                                                                   \
            }

int g_flagToStartCWMP = 0;

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoEngage
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to engage the object activity.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoEngage
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRestart
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject          = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;

    if ( pMyObject->bActive )
    {
        return  ANSC_STATUS_SUCCESS;
    }
    else if ( !pCcspCwmpCpeController )
    {
        return  ANSC_STATUS_NOT_READY;
    }

    /* must mark this flag before calling SetupEnv(),
     * otherwise, 2 instances of AsyncConnectTask 
     * may be created and this may cause side effects
     */    
    pMyObject->bActive = TRUE;

    returnStatus = pMyObject->SetupEnv((ANSC_HANDLE)pMyObject, bRestart);

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->bActive = FALSE;

        return  returnStatus;
    }

       printf("TR69 PA: Waiting for Setup env...\n");
    while( g_flagToStartCWMP==0)
    {
       sleep(5); 
    }

       printf("TR69 PA: Setup env completed\n");
    /*
     * A CPE MUST call the Inform method to initiate a transaction sequence whenever a connection
     * to an ACS is established. The CPE must pass in an array of EventStruct, indicating one or
     * more events that caused the transaction session to be established. If one or more causes
     * exist, the CPE MUST list all such causes.
     */
    if ( TRUE )
    {
        pMyObject->AsyncTaskCount++;
        returnStatus =
            AnscSpawnTask
                (
                    (void*)pMyObject->AsyncConnectTask,
                    (ANSC_HANDLE)pMyObject,
                    "CcspCwmppoAsyncConnectTask"
                 );
    }


    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoCancel
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to cancel the object activity.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoCancel
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject          = (PCCSP_CWMP_PROCESSOR_OBJECT   )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pPeriodicTimerObj  = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hPeriodicTimerObj;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pScheduleTimerObj  = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hScheduleTimerObj;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pPendingInformTimerObj  = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hPendingInformTimerObj;

    if ( !pMyObject->bActive )
    {
        return  ANSC_STATUS_SUCCESS;
    }
    else
    {
        pMyObject->bActive = FALSE;
    }

    pPeriodicTimerObj->Stop((ANSC_HANDLE)pPeriodicTimerObj);
    pScheduleTimerObj->Stop((ANSC_HANDLE)pScheduleTimerObj);
    pPendingInformTimerObj->Stop((ANSC_HANDLE)pPendingInformTimerObj);

    AnscSetEvent(&pMyObject->AsyncConnectEvent);

    while ( pMyObject->AsyncTaskCount > 0 )
    {
        AnscSleep(200);
    }

    returnStatus = pMyObject->CloseEnv((ANSC_HANDLE)pMyObject);

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSetupEnv
            (
                ANSC_HANDLE                 hThisObject,
                BOOL                        bRestart
            );

    description:

        This function is called to setup the operating environment.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoSetupEnv
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRestart
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_PROPERTY    pProperty               = (PCCSP_CWMP_PROCESSOR_PROPERTY)&pMyObject->Property;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    PANSC_ATOM_TABLE_OBJECT         pParamAttrCache         = (PANSC_ATOM_TABLE_OBJECT     )pMyObject->hParamAttrCache;
    char*                           pAcsUrl                 = NULL;

    if ( !bRestart )
    {
        

        /* initialize management server built-in FC */
        CcspManagementServer_Init
            (
                pCcspCwmpCpeController->PANameWithPrefix,
                pCcspCwmpCpeController->SubsysName,
                pCcspCwmpCpeController->hMsgBusHandle,
                pMyObject->MsValueChanged,  /* callback function on parameter value change only on Management Server internal FC */
                CcspCwmppoSysReadySignalCB,
                CcspCwmppoDiagCompleteSignalCB,
                (CCSP_HANDLE)pMyObject,
                (CCSP_HANDLE)pCcspCwmpCpeController,
                pCcspCwmpCpeController->SdmXmlFile ? pCcspCwmpCpeController->SdmXmlFile : NULL
            );

        pMyObject->MsFcInitDone = TRUE;

        /* set callback function on Message Bus to handle parameterValueChangeSignal */
        CcspBaseIf_Register_Event(pCcspCwmpCpeController->hMsgBusHandle, NULL, "parameterValueChangeSignal");

        CcspBaseIf_SetCallback2
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                "parameterValueChangeSignal",
                CcspCwmppoParamValueChangedCB,
                (void*)pMyObject
            );

        if ( !pParamAttrCache )
        {
            pParamAttrCache =
                (PANSC_ATOM_TABLE_OBJECT)AnscCreateAtomTable
                    (
                        (ANSC_HANDLE)NULL,
                        (ANSC_HANDLE)NULL,
                        (ANSC_HANDLE)NULL
                    );

            if ( pParamAttrCache )
            {
                pMyObject->hParamAttrCache  = (ANSC_HANDLE)pParamAttrCache;
            }
            else
            {
                CcspTr069PaTraceError
                    ((
                        "CcspCwmppoSetupEnv - failed to create parameter attribute cache.\n"
                    ));

                return  ANSC_STATUS_RESOURCES;
            }
        }

        pMyObject->InitParamAttrCache((ANSC_HANDLE)pMyObject);
    }
#if 0
    /* set callback function on Message Bus to handle parameterValueChangeSignal */
    CcspBaseIf_Register_Event(pCcspCwmpCpeController->hMsgBusHandle, NULL, "systemReadySignal");
    
    CcspBaseIf_SetCallback2
        (
            pCcspCwmpCpeController->hMsgBusHandle,
            "systemReadySignal",
            CcspCwmppoSysReadySignalCB,
            (void*)pMyObject
        );
#endif

    pProperty->bPeriodicInformEnabled = pMyObject->GetPeriodicInformEnabled ((ANSC_HANDLE)pMyObject);
    pProperty->PeriodicInformInterval = pMyObject->GetPeriodicInformInterval((ANSC_HANDLE)pMyObject);
    pAcsUrl                           = pMyObject->GetAcsUrl                ((ANSC_HANDLE)pMyObject);

    if ( pAcsUrl )
    {
        size_t len = strlen (pAcsUrl);

        if (len < sizeof(pProperty->AcsUrl))
        {
            memcpy (pProperty->AcsUrl, pAcsUrl, len + 1);
        }

        AnscFreeMemory(pAcsUrl);
    }

#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    if ( !pMyObject->bAcsInfoRetrieved )
    {
        PCCSP_CWMP_TCPCR_HANDLER_OBJECT     pCcspCwmpTcpcrHandler   = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT  )pCcspCwmpCpeController->hCcspCwmpTcpConnReqHandler;
        CCSP_CWMP_TCPCR_HANDLER_PROPERTY    tcpCrProperty;
        CCSP_STRING                         tcpCrHostPort           = NULL;
        USHORT                              usTcpCrHostPort         = 0;

        pCcspCwmpTcpcrHandler->GetProperty((ANSC_HANDLE)pCcspCwmpTcpcrHandler, &tcpCrProperty);

        tcpCrHostPort =
            CcspManagementServer_GetConnectionRequestURLPort
                (
                    pCcspCwmpCpeController->PANameWithPrefix
                );

        if ( tcpCrHostPort )
        {
            usTcpCrHostPort = (USHORT)_ansc_atoi(tcpCrHostPort);

            AnscFreeMemory(tcpCrHostPort);

            if ( usTcpCrHostPort!= 0 && usTcpCrHostPort != tcpCrProperty.HostPort )
            {
                tcpCrProperty.HostPort = usTcpCrHostPort;

                pCcspCwmpTcpcrHandler->SetProperty((ANSC_HANDLE)pCcspCwmpTcpcrHandler, (ANSC_HANDLE)&tcpCrProperty);
            }
        }
    }
#endif

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoCloseEnv
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to close the operating environment.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoCloseEnv
    (
        ANSC_HANDLE                 hThisObject
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoPeriodicTimerInvoke
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called when it's time to call the Inform()
        method periodically.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoPeriodicTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject          = (PCCSP_CWMP_PROCESSOR_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_PROPERTY   pProperty          = (PCCSP_CWMP_PROCESSOR_PROPERTY )&pMyObject->Property;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pPeriodicTimerObj  = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hPeriodicTimerObj;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_SESSION_OBJECT       pCcspCwmpSession   = (PCCSP_CWMP_SESSION_OBJECT)NULL;
    PCCSP_CWMP_EVENT                pCcspCwmpEvent     = (PCCSP_CWMP_EVENT         )NULL;


    if ( pPeriodicTimerObj->TimerType == ANSC_TIMER_TYPE_SPORADIC )
    {
        pPeriodicTimerObj->SetTimerType((ANSC_HANDLE)pPeriodicTimerObj, ANSC_TIMER_TYPE_PERIODIC           );
        pPeriodicTimerObj->SetInterval ((ANSC_HANDLE)pPeriodicTimerObj, pProperty->FollowingInterval * 1000);
        pPeriodicTimerObj->Start       ((ANSC_HANDLE)pPeriodicTimerObj);
    }

    if ( !pCcspCwmpCpeController->bBootInformSent )
    {
        /* do not contact ACS unless PA has sent out BOOT inform */
        return  ANSC_STATUS_SUCCESS;
    }

    pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT     )pMyObject->AcqWmpSession2((ANSC_HANDLE)pMyObject);
    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_FAILURE;
    }

    pCcspCwmpEvent = (PCCSP_CWMP_EVENT)AnscAllocateMemory(sizeof(CCSP_CWMP_EVENT));

    if ( !pCcspCwmpEvent )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }
    else
    {
        pCcspCwmpEvent->EventCode  = AnscCloneString(CCSP_CWMP_INFORM_EVENT_NAME_Peroidic);
        pCcspCwmpEvent->CommandKey = NULL;
    }

    returnStatus =
        pCcspCwmpSession->AddCwmpEvent
            (
                (ANSC_HANDLE)pCcspCwmpSession,
                (ANSC_HANDLE)pCcspCwmpEvent,
                TRUE
            );


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    if ( pCcspCwmpSession )
    {
        pMyObject->RelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoScheduleTimerInvoke
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called when it's time to call the Inform()
        method triggered by a previous ScheduleInform() call.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoScheduleTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject          = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession    = (PCCSP_CWMP_SESSION_OBJECT    )pMyObject->AcqWmpSession2((ANSC_HANDLE)pMyObject);
    PCCSP_CWMP_EVENT                pCcspCwmpEvent     = (PCCSP_CWMP_EVENT            )NULL;

    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_FAILURE;
    }

    pCcspCwmpEvent = (PCCSP_CWMP_EVENT)AnscAllocateMemory(sizeof(CCSP_CWMP_EVENT));

    if ( !pCcspCwmpEvent )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }
    else
    {
        pCcspCwmpEvent->EventCode  = AnscCloneString("3 SCHEDULED");
        pCcspCwmpEvent->CommandKey = pMyObject->SecheduledCommandKey;

        pMyObject->SecheduledCommandKey = NULL;
    }

    returnStatus =
        pCcspCwmpSession->AddCwmpEvent
            (
                (ANSC_HANDLE)pCcspCwmpSession,
                (ANSC_HANDLE)pCcspCwmpEvent,
                TRUE
            );


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    if ( pCcspCwmpSession )
    {
        pMyObject->RelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoPendingInformTimerInvoke
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called when it's time to call the Inform()
        method triggered by a previous ScheduleInform() call.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoPendingInformTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject          = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_SESSION_OBJECT       pCcspCwmpSession   = NULL;

    pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pMyObject->AcqWmpSession((ANSC_HANDLE)pMyObject);

    if ( !pCcspCwmpSession ) return ANSC_STATUS_INTERNAL_ERROR;

    if ( pCcspCwmpSession->SessionState == CCSP_CWMPSO_SESSION_STATE_idle )
    {
        pCcspCwmpSession->SessionState = CCSP_CWMPSO_SESSION_STATE_connectNow;

        returnStatus =
            pMyObject->SignalSession
                (
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)pCcspCwmpSession
                );
    }

    pMyObject->RelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoRedeliverEvents
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to re-deliver events.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoRedeliverEvents
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;

        /* re-deliver TransferComplete events if any */
    returnStatus = pMyObject->LoadTransferComplete((ANSC_HANDLE)pMyObject);

        /* re-deliver AutonomousTransferComplete event if any */
    returnStatus = pMyObject->LoadAutonomousTransferComplete((ANSC_HANDLE)pMyObject);

        /* re-deliver DUStateChangeComplete event if any */
    returnStatus = pMyObject->LoadDUStateChangeComplete((ANSC_HANDLE)pMyObject);

        /* re-deliver AutonomousDUStateChangeComplete event if any */
    returnStatus = pMyObject->LoadAutonDUStateChangeComplete((ANSC_HANDLE)pMyObject);

        /* re-deliver ValueChanged event if any */
    returnStatus = pMyObject->LoadValueChanged((ANSC_HANDLE)pMyObject);

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoGetUndeliveredEvents
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hWmpSession
            );

    description:

        This function is called to add undelivered event codes
        into specified session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hWmpSession
                Specifies the WMP session object to add undelivered
                events in.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoGetUndeliveredEvents
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;

    returnStatus = pMyObject->GetUndeliveredTcEvents((ANSC_HANDLE)pMyObject, hWmpSession);

    returnStatus = pMyObject->GetUndeliveredAtcEvents((ANSC_HANDLE)pMyObject, hWmpSession);

    returnStatus = pMyObject->GetUndeliveredDscEvents((ANSC_HANDLE)pMyObject, hWmpSession);

    returnStatus = pMyObject->GetUndeliveredAdscEvents((ANSC_HANDLE)pMyObject, hWmpSession);

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoGetUndeliveredTcEvents
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hWmpSession
            );

    description:

        This function is called to add undelivered event codes
        into specified session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hWmpSession
                Specifies the WMP session object to add undelivered
                events in.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoGetUndeliveredTcEvents
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    )
{
    ANSC_STATUS                     returnStatus        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT)hThisObject;
    PCCSP_CWMP_SESSION_OBJECT       pCcspCwmpSession    = (PCCSP_CWMP_SESSION_OBJECT  )hWmpSession;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    char                            psmTcName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    char*                           pCommandKey         = NULL;
    ULONG                           i                   = 0;
    int                             psmStatus;
    BOOL                            bIsDownload         = TRUE;
    unsigned int                    numInstances        = 0;
    unsigned int*                   pInsNumbers         = NULL;
    char*                           pValue              = NULL;
    PCCSP_CWMP_EVENT                pCcspCwmpEvent      = NULL;
    BOOL                            bTcSingleEventAdded = FALSE;

    _ansc_snprintf
        (
            psmTcName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_TransferComplete
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    for ( i = 0; i < numInstances; i ++ )
    {
        pCommandKey = NULL; 

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                pInsNumbers[i],
                CCSP_CWMPPO_PARAM_NAME_CommandKey
            );

        psmStatus = 
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    NULL,
                    &pCommandKey
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            continue;
        }

        if ( TRUE )
        {
            bIsDownload = TRUE;

            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                    pInsNumbers[i],
                    CCSP_CWMPPO_PARAM_NAME_IsDownload
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        NULL,
                        &pValue
                    );

            if ( psmStatus == CCSP_SUCCESS && pValue )
            {
                bIsDownload = (BOOL)_ansc_atoi(pValue);
                AnscFreeMemory(pValue);
            }
        }

        if ( !bTcSingleEventAdded )
        {
            pCcspCwmpEvent = (PCCSP_CWMP_EVENT)AnscAllocateMemory(sizeof(CCSP_CWMP_EVENT));
            if ( pCcspCwmpEvent )
            {
                pCcspCwmpEvent->EventCode  = AnscCloneString(CCSP_CWMP_INFORM_EVENT_NAME_TransferComplete);
                pCcspCwmpEvent->CommandKey = NULL;

                pCcspCwmpSession->AddCwmpEvent
                    (
                        (ANSC_HANDLE)pCcspCwmpSession,
                        pCcspCwmpEvent,
                        FALSE
                    );

                bTcSingleEventAdded = TRUE;
            }
        }

        /* add CWMP event code */
        pCcspCwmpEvent = (PCCSP_CWMP_EVENT)AnscAllocateMemory(sizeof(CCSP_CWMP_EVENT));
        if ( pCcspCwmpEvent )
        {
            if ( bIsDownload )
            {
                pCcspCwmpEvent->EventCode  = AnscCloneString(CCSP_CWMP_INFORM_EVENT_NAME_M_Download);
            }
            else
            {
                pCcspCwmpEvent->EventCode  = AnscCloneString(CCSP_CWMP_INFORM_EVENT_NAME_M_Upload);
            }

            pCcspCwmpEvent->CommandKey = AnscCloneString(pCommandKey);

            pCcspCwmpSession->AddCwmpEvent
                (
                    (ANSC_HANDLE)pCcspCwmpSession,
                    pCcspCwmpEvent,
                    FALSE
                );
        }
    }
        
    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoGetUndeliveredAtcEvents
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hWmpSession
            );

    description:

        This function is called to add undelivered event codes
        into specified session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hWmpSession
                Specifies the WMP session object to add undelivered
                events in.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoGetUndeliveredAtcEvents
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    )
{
    ANSC_STATUS                     returnStatus        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT)hThisObject;
    PCCSP_CWMP_SESSION_OBJECT       pCcspCwmpSession    = (PCCSP_CWMP_SESSION_OBJECT  )hWmpSession;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_EVENT                pCcspCwmpEvent      = NULL;
    char                            psmTcName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    int                             psmStatus;
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;

    CcspTr069PaTraceDebug(("ccspCwmppoLoadAutonomousTransferCompleteTask - loading undelivered Autonomous Transfer Complete events...\n"));

    _ansc_snprintf
        (
            psmTcName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    if ( numInstances > 0 )
    {
        pCcspCwmpEvent = (PCCSP_CWMP_EVENT)AnscAllocateMemory(sizeof(CCSP_CWMP_EVENT));
        if ( pCcspCwmpEvent )
        {
            pCcspCwmpEvent->EventCode  = AnscCloneString(CCSP_CWMP_INFORM_EVENT_NAME_AutonomousTransferComplete);
            pCcspCwmpEvent->CommandKey = NULL;

            pCcspCwmpSession->AddCwmpEvent
                (
                    (ANSC_HANDLE)pCcspCwmpSession,
                    pCcspCwmpEvent,
                    FALSE
                );
        }
    }

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoGetUndeliveredDscEvents
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hWmpSession
            );

    description:

        This function is called to add undelivered event codes
        into specified session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hWmpSession
                Specifies the WMP session object to add undelivered
                events in.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoGetUndeliveredDscEvents
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    )
{
    ANSC_STATUS                     returnStatus        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT)hThisObject;
    PCCSP_CWMP_SESSION_OBJECT       pCcspCwmpSession    = (PCCSP_CWMP_SESSION_OBJECT  )hWmpSession;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_EVENT                pCcspCwmpEvent      = NULL;
    char                            psmDsccName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    ULONG                           i;
    int                             psmStatus;
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;
    BOOL                            bTcSingleEventAdded     = FALSE;
    char*                           pCommandKey             = NULL;

    _ansc_snprintf
        (
            psmDsccName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_DSCC
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmDsccName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    for ( i = 0; i < numInstances; i ++ )
    {
        _ansc_snprintf
            (
                psmDsccName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_DSCC,
                (unsigned int)pInsNumbers[i]
            );

        psmStatus = 
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmDsccName,
                    NULL,
                    &pCommandKey
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            continue;
        }

        if ( !bTcSingleEventAdded )
        {
            pCcspCwmpEvent = (PCCSP_CWMP_EVENT)AnscAllocateMemory(sizeof(CCSP_CWMP_EVENT));
            if ( pCcspCwmpEvent )
            {
                pCcspCwmpEvent->EventCode  = AnscCloneString(CCSP_CWMP_INFORM_EVENT_NAME_DUStateChangeComplete);
                pCcspCwmpEvent->CommandKey = NULL;

                pCcspCwmpSession->AddCwmpEvent
                    (
                        (ANSC_HANDLE)pCcspCwmpSession,
                        pCcspCwmpEvent,
                        FALSE
                    );

                bTcSingleEventAdded = TRUE;
            }
        }

        /* add CWMP event code */
        pCcspCwmpEvent = (PCCSP_CWMP_EVENT)AnscAllocateMemory(sizeof(CCSP_CWMP_EVENT));
        if ( pCcspCwmpEvent )
        {
            pCcspCwmpEvent->EventCode  = AnscCloneString(CCSP_CWMP_INFORM_EVENT_NAME_M_ChangeDUState);
            pCcspCwmpEvent->CommandKey = AnscCloneString(pCommandKey);

            pCcspCwmpSession->AddCwmpEvent
                (
                    (ANSC_HANDLE)pCcspCwmpSession,
                    pCcspCwmpEvent,
                    FALSE
                );
        }
    }

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoGetUndeliveredAdscEvents
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hWmpSession
            );

    description:

        This function is called to add undelivered event codes
        into specified session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hWmpSession
                Specifies the WMP session object to add undelivered
                events in.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoGetUndeliveredAdscEvents
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    )
{
    ANSC_STATUS                     returnStatus        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT)hThisObject;
    PCCSP_CWMP_SESSION_OBJECT       pCcspCwmpSession    = (PCCSP_CWMP_SESSION_OBJECT  )hWmpSession;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_EVENT                pCcspCwmpEvent      = NULL;
    char                            psmAdsccName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    int                             psmStatus;
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers         = NULL;

    _ansc_snprintf
        (
            psmAdsccName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_ADSCC
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmAdsccName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    if ( numInstances > 0 )
    {
        pCcspCwmpEvent = (PCCSP_CWMP_EVENT)AnscAllocateMemory(sizeof(CCSP_CWMP_EVENT));
        if ( pCcspCwmpEvent )
        {
            pCcspCwmpEvent->EventCode  = AnscCloneString(CCSP_CWMP_INFORM_EVENT_NAME_AutonomousDUStateChangeComplete);
            pCcspCwmpEvent->CommandKey = NULL;

            pCcspCwmpSession->AddCwmpEvent
                (
                    (ANSC_HANDLE)pCcspCwmpSession,
                    pCcspCwmpEvent,
                    FALSE
                );
        }
    }

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSaveTransferComplete
            (
                ANSC_HANDLE                 hThisObject
                char*                       pCommandKey,
                ANSC_HANDLE                 hStartTime,
                ANSC_HANDLE                 hCompleteTime
            );

    description:

        This function is called to save TransferComplete event 
        into PSM.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pCommandKey,
                The command key;

                ANSC_HANDLE                 hStartTime
                Start time of file transfer.

                ANSC_HANDLE                 hCompleteTime
                Complete time of file transfer.

                PBOOL                       pbIsDownload
                If exists, it specifies if the operation is download
                or upload.

                PCCSP_CWMP_FAULT            pFault
                If exists, it specifies if operation is successful
                or failed.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoSaveTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCommandKey,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        BOOL                        bIsDownload,
        PCCSP_CWMP_FAULT            pFault
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    char                            psmTcName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    char                            buffer[257];
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;
    unsigned int                    InsNumber;
    int                             psmStatus;
    PANSC_UNIVERSAL_TIME            pStartTime              = (PANSC_UNIVERSAL_TIME)hStartTime;
    PANSC_UNIVERSAL_TIME            pCompleteTime           = (PANSC_UNIVERSAL_TIME)hCompleteTime;

    CcspTr069PaTraceDebug(("CcspCwmppoSaveTransferComplete - command key <%s>.\n", pCommandKey)); 

    if ( !pMyObject->bActive )
    {
        return  ANSC_STATUS_FAILURE;
    }

    _ansc_snprintf
        (
            psmTcName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_TransferComplete
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    InsNumber = 
        CcspFindUnusedInsNumber
            (
                numInstances, 
                pInsNumbers, 
                CCSP_CWMPPO_PENDING_TC_MAX_COUNT
            );

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    if ( InsNumber == 0 )
    {
        returnStatus = ANSC_STATUS_RESOURCES;
        goto EXIT;
    }

    _ansc_snprintf
        (
            psmTcName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.%u.%s",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
            InsNumber,
            CCSP_CWMPPO_PARAM_NAME_CommandKey
        );

    psmStatus = 
        PSM_Set_Record_Value2
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                ccsp_string,
                pCommandKey
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        returnStatus = ANSC_STATUS_INTERNAL_ERROR;
        goto EXIT;
    }

    if ( hStartTime && pStartTime->Year >= 1970 )
    {
        _ansc_sprintf(buffer, "%u", (unsigned int)AnscCalendarToSecondFromEpoch(hStartTime));

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                InsNumber,
                CCSP_CWMPPO_PARAM_NAME_StartTime
            );

        psmStatus = 
            PSM_Set_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    ccsp_unsignedInt,
                    buffer
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }
    }
    
    if ( hCompleteTime && pCompleteTime->Year >=1970 )
    {
        _ansc_sprintf(buffer, "%u", (pCompleteTime->Year < 1970)?0:(unsigned int)AnscCalendarToSecondFromEpoch(hCompleteTime));

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                InsNumber,
                CCSP_CWMPPO_PARAM_NAME_CompleteTime
            );

        psmStatus = 
            PSM_Set_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    ccsp_unsignedInt,
                    buffer
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }
    }

    _ansc_snprintf
        (
            psmTcName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.%u.%s",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
            InsNumber,
            CCSP_CWMPPO_PARAM_NAME_IsDownload
        );

    psmStatus = 
        PSM_Set_Record_Value2
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                ccsp_boolean,
                bIsDownload ? "1" : "0"
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        returnStatus = ANSC_STATUS_INTERNAL_ERROR;
        goto EXIT;
    }

    if ( pFault )
    {
        _ansc_sprintf(buffer, "%u", (unsigned int)pFault->FaultCode);

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                InsNumber,
                CCSP_CWMPPO_PARAM_NAME_FaultCode
            );

        psmStatus = 
            PSM_Set_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    ccsp_unsignedInt,
                    buffer
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }

        if ( pFault->FaultString )
        {
            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                    InsNumber,
                    CCSP_CWMPPO_PARAM_NAME_FaultString
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        ccsp_string,
                        pFault->FaultString
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }
    }

EXIT:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTr069PaTraceError
            ((
                "CcspCwmppoSaveTransferComplete - failed to create TC with ins number %u, returnStatus <%d>.\n", 
                InsNumber, 
                (int)returnStatus
            )); 

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                InsNumber
            );

        PSM_Del_Record
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName
            );
    }
    else
    {
        CcspTr069PaTraceInfo
            ((
                "CcspCwmppoSaveTransferComplete - TC with ins number %u has been created in PSM.\n", 
                InsNumber
            )); 
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoLoadTransferComplete
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to load transfer complete event and send out
        to ACS server

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
ccspCwmppoLoadTransferCompleteTask
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     status                  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf              = (PCCSP_CWMP_MSO_INTERFACE        )pCcspCwmpCpeController->GetCcspCwmpMsoIf(pCcspCwmpCpeController);
    char                            psmTcName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    char*                           pCommandKey             = NULL;
    ULONG                           uStartTime              = 0;
    ULONG                           uEndTime                = 0;
    ANSC_UNIVERSAL_TIME             timeStart               = { 0 };
    ANSC_UNIVERSAL_TIME             timeEnd                 = { 0 };
    ULONG                           i                       = 0;
    int                             psmStatus;
    CCSP_CWMP_FAULT                 Fault                   = {0};
    BOOL                            bIsDownload             = TRUE;
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;
    char*                           pValue                  = NULL;

    CcspTr069PaTraceDebug(("ccspCwmppoLoadTransferCompleteTask - loading completed transfers ...\n"));

    _ansc_snprintf
        (
            psmTcName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_TransferComplete
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    if ( numInstances == 0 )
    {
        CcspTr069PaTraceDebug(("ccspCwmppoLoadTransferCompleteTask - no pending TransferComplete events.\n"));

        status = ANSC_STATUS_CANT_FIND;
        goto EXIT;
    }

#if 0
    /* wait for 30 seconds first and send the transfer complete events if have */
    while( i < 60)
    {
        AnscSleep(500);

        if(!pMyObject->bActive)
        {
            goto EXIT;
        }

        i ++;
    }
#endif 

    for ( i = 0; i < numInstances; i ++ )
    {
        pCommandKey = NULL; 

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                pInsNumbers[i],
                CCSP_CWMPPO_PARAM_NAME_CommandKey
            );

        psmStatus = 
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    NULL,
                    &pCommandKey
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            CcspTr069PaTraceError
                ((
                    "ccspCwmppoLoadTransferCompleteTask - failed to load TC with Ins Number <%u>.\n", 
                    pInsNumbers[i]
                ));

            continue;
        }

        if ( TRUE )
        {
            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                    pInsNumbers[i],
                    CCSP_CWMPPO_PARAM_NAME_StartTime
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        NULL,
                        &pValue
                    );

            if ( psmStatus == CCSP_SUCCESS && pValue )
            {
                uStartTime = (ULONG)_ansc_atoi(pValue);
                AnscZeroMemory(&timeStart, sizeof(ANSC_UNIVERSAL_TIME));
                AnscSecondToCalendar(uStartTime, &timeStart);

                AnscFreeMemory(pValue);
            }
            else
            {
                /* unknown time */
                timeStart.Year  = 1;
                timeStart.Month = 1;
                timeStart.DayOfMonth = 1;
            }
        }

        if ( TRUE )
        {
            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                    pInsNumbers[i],
                    CCSP_CWMPPO_PARAM_NAME_CompleteTime
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        NULL,
                        &pValue
                    );

            if ( psmStatus == CCSP_SUCCESS && pValue )
            {
                uEndTime = (ULONG)_ansc_atoi(pValue);
                AnscZeroMemory(&timeStart, sizeof(ANSC_UNIVERSAL_TIME));
                AnscSecondToCalendar(uEndTime, &timeEnd);

                AnscFreeMemory(pValue);
            }
            else
            {
                /* unknown time */
                timeEnd.Year  = 1;
                timeEnd.Month = 1;
                timeEnd.DayOfMonth = 1;
            }
        }

        if ( TRUE )
        {
            bIsDownload = TRUE;

            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                    pInsNumbers[i],
                    CCSP_CWMPPO_PARAM_NAME_IsDownload
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        NULL,
                        &pValue
                    );

            if ( psmStatus == CCSP_SUCCESS && pValue )
            {
                bIsDownload = (BOOL)_ansc_atoi(pValue);
                AnscFreeMemory(pValue);
            }
        }

        if ( TRUE )
        {
            Fault.FaultCode = 0;

            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                    pInsNumbers[i],
                    CCSP_CWMPPO_PARAM_NAME_FaultCode
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        NULL,
                        &pValue
                    );

            if ( psmStatus == CCSP_SUCCESS && pValue )
            {
                Fault.FaultCode = (ULONG)_ansc_atoi(pValue);
                AnscFreeMemory(pValue);
            }
        }

        if ( Fault.FaultCode != 0 )
        {
            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                    pInsNumbers[i],
                    CCSP_CWMPPO_PARAM_NAME_FaultString
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        NULL,
                        &Fault.FaultString
                    );
        }

        CcspTr069PaTraceDebug(("ccspCwmppoLoadTransferCompleteTask - sending TransferComplete, command key <%s>.\n", pCommandKey));

        status = 
            pCcspCwmpMsoIf->TransferComplete
                (
                    pCcspCwmpMsoIf->hOwnerContext, 
                    bIsDownload,
                    pCommandKey,
                    &Fault,
                    &timeStart,
                    &timeEnd,
                    FALSE   /* use current session to deliver this */
                );

        if ( status != ANSC_STATUS_SUCCESS )
        {
            CcspTr069PaTraceWarning(("Failed to send out 'TransferComplete' whose command key is '%s'\n", pCommandKey));
        }
        else
        {
            /* remove delivered TC from PSM */
            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_TransferComplete,
                    pInsNumbers[i]
                );

            PSM_Del_Record
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName
                );
        }

        CcspCwmpCleanFault((&Fault));

        if ( pCommandKey != NULL )
        {
            AnscFreeMemory(pCommandKey);
        }

        if ( !pMyObject->bActive )
        {
            goto EXIT;
        }
    }

    status = ANSC_STATUS_SUCCESS;

EXIT:

    pMyObject->AsyncTaskCount--;

    CcspTr069PaTraceDebug(("ccspCwmppoLoadTransferCompleteTask - exit.\n"));

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    return status;
}


ANSC_STATUS
CcspCwmppoLoadTransferComplete
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     status                  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;

#if 0
    /*
     * If there's transfer complete event in PSM, send it out.
     */
    if ( TRUE )
    {
        pMyObject->AsyncTaskCount++;

        AnscSpawnTask
            (
                (void*)ccspCwmppoLoadTransferCompleteTask,
                (ANSC_HANDLE)pMyObject,
                "LoadTransferCompleteTask"
             );
    }
#endif

    if ( TRUE )
    {
        pMyObject->AsyncTaskCount++;

        /* Try to send transfer complete along with BOOT event if any */
        status = ccspCwmppoLoadTransferCompleteTask((ANSC_HANDLE)pMyObject);
    }

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSaveAutonomousTransferComplete
            (
                ANSC_HANDLE                 hThisObject
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

    description:

        This function is called to save transfer complete to PSM.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

		        BOOL						bIsDownload
                Specifies if operation is download or upload.

                char*                       AnnounceURL
                Announce URL.

                char*                       TransferURL
                Transfer URL.

                char*                       FileType
                File type.

                unsigned int                FileSize
                File size.

                char*                       TargetFileName
                Target file name.

                ANSC_HANDLE                 hFault
                Specifies fault.

                ANSC_HANDLE                 hStartTime
                Start time of file transfer.

                ANSC_HANDLE                 hCompleteTime
                Complete time of file transfer.

    return:     status of operation.

**********************************************************************/

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
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_SOAP_FAULT           pCwmpFault              = (PCCSP_CWMP_SOAP_FAULT       )hFault;
    char                            psmTcName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    char                            buffer[257];
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;
    unsigned int                    InsNumber;
    int                             psmStatus;
    PANSC_UNIVERSAL_TIME            pStartTime              = (PANSC_UNIVERSAL_TIME)hStartTime;
    PANSC_UNIVERSAL_TIME            pCompleteTime           = (PANSC_UNIVERSAL_TIME)hCompleteTime;

    CcspTr069PaTraceDebug(("CcspCwmppoSaveAutonomousTransferComplete being called ...\n")); 

    if ( !pMyObject->bActive )
    {
        return  ANSC_STATUS_FAILURE;
    }

    _ansc_snprintf
        (
            psmTcName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    InsNumber = 
        CcspFindUnusedInsNumber
            (
                numInstances, 
                pInsNumbers, 
                CCSP_CWMPPO_PENDING_ATC_MAX_COUNT
            );

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    if ( InsNumber == 0 )
    {
        returnStatus = ANSC_STATUS_RESOURCES;
        goto EXIT;
    }

    _ansc_snprintf
        (
            psmTcName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.%u.%s",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
            (unsigned int)InsNumber,
            CCSP_CWMPPO_PARAM_NAME_IsDownload
        );

    psmStatus = 
        PSM_Set_Record_Value2
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                ccsp_boolean,
                bIsDownload ? "1" : "0"
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        returnStatus = ANSC_STATUS_INTERNAL_ERROR;
        goto EXIT;
    }

    if ( AnnounceURL )
    {
        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                (unsigned int)InsNumber,
                CCSP_CWMPPO_PARAM_NAME_AnnounceURL
            );

        psmStatus = 
            PSM_Set_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    ccsp_string,
                    AnnounceURL
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }
    }

    if ( TransferURL )
    {
        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                (unsigned int)InsNumber,
                CCSP_CWMPPO_PARAM_NAME_TransferURL
            );

        psmStatus = 
            PSM_Set_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    ccsp_string,
                    TransferURL
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }
    }

    if ( FileType )
    {
        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                (unsigned int)InsNumber,
                CCSP_CWMPPO_PARAM_NAME_FileType
            );

        psmStatus = 
            PSM_Set_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    ccsp_string,
                    FileType
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }
    }

    if ( FileSize != 0 )
    {
        _ansc_sprintf(buffer, "%u", (unsigned int)FileSize);        

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                (unsigned int)InsNumber,
                CCSP_CWMPPO_PARAM_NAME_FileSize
            );

        psmStatus = 
            PSM_Set_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    ccsp_unsignedInt,
                    buffer
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }
    }

    if ( TargetFileName )
    {
        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                (unsigned int)InsNumber,
                CCSP_CWMPPO_PARAM_NAME_TargetFileName
            );

        psmStatus = 
            PSM_Set_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    ccsp_string,
                    TargetFileName
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }
    }

    if ( pCwmpFault )
    {
        _ansc_sprintf(buffer, "%u", (unsigned int)pCwmpFault->Fault.FaultCode);

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                (unsigned int)InsNumber,
                CCSP_CWMPPO_PARAM_NAME_FaultCode
            );

        psmStatus = 
            PSM_Set_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    ccsp_unsignedInt,
                    buffer
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }

        if ( pCwmpFault->Fault.FaultString )
        {
            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                    (unsigned int)InsNumber,
                    CCSP_CWMPPO_PARAM_NAME_FaultString
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        ccsp_string,
                        pCwmpFault->Fault.FaultString
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }
    }

    if ( hStartTime && pStartTime->Year >=1970  )
    {
        _ansc_sprintf(buffer, "%u", (unsigned int)AnscCalendarToSecondFromEpoch(hStartTime));

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                (unsigned int)InsNumber,
                CCSP_CWMPPO_PARAM_NAME_StartTime
            );

        psmStatus = 
            PSM_Set_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    ccsp_unsignedInt,
                    buffer
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }
    }
    
    if ( hCompleteTime && pCompleteTime->Year >= 1970 )
    {
        _ansc_sprintf(buffer, "%u", (unsigned int)AnscCalendarToSecondFromEpoch(hCompleteTime));

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                (unsigned int)InsNumber,
                CCSP_CWMPPO_PARAM_NAME_CompleteTime
            );

        psmStatus = 
            PSM_Set_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    ccsp_unsignedInt,
                    buffer
                );

        if ( psmStatus != CCSP_SUCCESS )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }
    }


EXIT:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTr069PaTraceError
            ((
                "CcspCwmppoSaveAutonomousTransferComplete - failed to create TC with ins number %u, returnStatus <%d>.\n", 
                (unsigned int)InsNumber, 
                (int)returnStatus
            )); 

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                (unsigned int)InsNumber
            );

        PSM_Del_Record
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName
            );
    }
    else
    {
        CcspTr069PaTraceInfo
            ((
                "CcspCwmppoSaveAutonomousTransferComplete - TC with ins number %u has been created in PSM.\n", 
                (unsigned int)InsNumber
            )); 
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoLoadAutonomousTransferComplete
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to load transfer complete event and send out
        to ACS server

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
ccspCwmppoLoadAutonomousTransferCompleteTask
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     status                  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf              = (PCCSP_CWMP_MSO_INTERFACE        )pCcspCwmpCpeController->GetCcspCwmpMsoIf(pCcspCwmpCpeController);
    char                            psmTcName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    ULONG                           i                       = 0;
    int                             psmStatus;
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;
    char*                           pValue                  = NULL;
    CCSP_CWMP_FAULT                 Fault                   = {0};
    BOOL                            bIsDownload             = TRUE;
    ULONG                           uStartTime              = 0;
    ULONG                           uEndTime                = 0;
    char*                           pAnnounceURL            = NULL;
    char*                           pTransferURL            = NULL;
    char*                           pFileType               = NULL;
    unsigned int                    FileSize                = 0;
    char*                           pTargetFileName         = NULL;
    ANSC_UNIVERSAL_TIME             timeStart, timeEnd;

    CcspTr069PaTraceDebug(("ccspCwmppoLoadAutonomousTransferCompleteTask - loading undelivered Autonomous Transfer Complete events...\n"));

    _ansc_snprintf
        (
            psmTcName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    if ( numInstances == 0 )
    {
        CcspTr069PaTraceDebug(("ccspCwmppoLoadAutonomousTransferCompleteTask - no pending TransferComplete events.\n"));

        status = ANSC_STATUS_CANT_FIND;
        goto EXIT;
    }

#if 0
    /* wait for 30 seconds first and send the transfer complete events if have */
    while( i < 60)
    {
        AnscSleep(500);

        if(!pMyObject->bActive)
        {
            goto EXIT;
        }

        i ++;
    }
#endif 

    for ( i = 0; i < numInstances; i ++ )
    {
        /* get IsDownload */
        if ( TRUE )
        {
            bIsDownload = TRUE;

            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                    pInsNumbers[i],
                    CCSP_CWMPPO_PARAM_NAME_IsDownload
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        NULL,
                        &pValue
                    );

            if ( psmStatus == CCSP_SUCCESS && pValue )
            {
                bIsDownload = (BOOL)_ansc_atoi(pValue);
                AnscFreeMemory(pValue);
            }
        }

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                pInsNumbers[i],
                CCSP_CWMPPO_PARAM_NAME_AnnounceURL
            );

        psmStatus = 
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    NULL,
                    &pAnnounceURL
                );

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                pInsNumbers[i],
                CCSP_CWMPPO_PARAM_NAME_TransferURL
            );

        psmStatus = 
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    NULL,
                    &pTransferURL
                );

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                pInsNumbers[i],
                CCSP_CWMPPO_PARAM_NAME_FileType
            );

        psmStatus = 
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    NULL,
                    &pFileType
                );

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                pInsNumbers[i],
                CCSP_CWMPPO_PARAM_NAME_FileSize
            );

        psmStatus = 
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    NULL,
                    &pValue
                );

        FileSize = 0;

        if ( pValue )
        {
            FileSize = (ULONG)_ansc_atoi(pValue);
            AnscFreeMemory(pValue);
        }

        _ansc_snprintf
            (
                psmTcName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                pInsNumbers[i],
                CCSP_CWMPPO_PARAM_NAME_TargetFileName
            );

        psmStatus = 
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    NULL,
                    &pTargetFileName
                );

        if ( TRUE )
        {
            Fault.FaultCode = 0;

            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                    pInsNumbers[i],
                    CCSP_CWMPPO_PARAM_NAME_FaultCode
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        NULL,
                        &pValue
                    );

            if ( psmStatus == CCSP_SUCCESS && pValue )
            {
                Fault.FaultCode = (ULONG)_ansc_atoi(pValue);
                AnscFreeMemory(pValue);
            }
        }

        if ( Fault.FaultCode != 0 )
        {
            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                    pInsNumbers[i],
                    CCSP_CWMPPO_PARAM_NAME_FaultString
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        NULL,
                        &Fault.FaultString
                    );
        }

        if ( TRUE )
        {
            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                    pInsNumbers[i],
                    CCSP_CWMPPO_PARAM_NAME_StartTime
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        NULL,
                        &pValue
                    );

            if ( psmStatus == CCSP_SUCCESS && pValue )
            {
                uStartTime = (ULONG)_ansc_atoi(pValue);
                AnscZeroMemory(&timeStart, sizeof(ANSC_UNIVERSAL_TIME));
                AnscSecondToCalendar(uStartTime, &timeStart);

                AnscFreeMemory(pValue);
            }
            else
            {
                /* unknown time */
                timeStart.Year  = 1;
                timeStart.Month = 1;
                timeStart.DayOfMonth = 1;
            }
        }

        if ( TRUE )
        {
            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                    pInsNumbers[i],
                    CCSP_CWMPPO_PARAM_NAME_CompleteTime
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmTcName,
                        NULL,
                        &pValue
                    );

            if ( psmStatus == CCSP_SUCCESS && pValue )
            {
                uEndTime = (ULONG)_ansc_atoi(pValue);
                AnscZeroMemory(&timeStart, sizeof(ANSC_UNIVERSAL_TIME));
                AnscSecondToCalendar(uEndTime, &timeEnd);

                AnscFreeMemory(pValue);
            }
            else
            {
                /* unknown time */
                timeEnd.Year  = 1;
                timeEnd.Month = 1;
                timeEnd.DayOfMonth = 1;
            }
        }

        CcspTr069PaTraceDebug(("ccspCwmppoLoadAutonomousTransferCompleteTask - sending AutonomousTransferComplete...\n"));

        status =
            pCcspCwmpMsoIf->AutonomousTransferComplete
                            (
                                pCcspCwmpMsoIf->hOwnerContext, 
		                        bIsDownload,
                                pAnnounceURL,
                                pTransferURL,
                                pFileType,
                                FileSize,
                                pTargetFileName,
                                &Fault,
                                &timeStart,
                                &timeEnd,
                                FALSE       /* use current session to deliver this */
                            );

        if ( status != ANSC_STATUS_SUCCESS )
        {
            CcspTr069PaTraceWarning(("Failed to send out 'AutonomousTransferComplete'!\n"));
        }
        else
        {
            /* remove delivered TC from PSM */
            _ansc_snprintf
                (
                    psmTcName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete,
                    pInsNumbers[i]
                );

            PSM_Del_Record
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName
                );
        }

        CcspCwmpCleanFault((&Fault));

        if ( pAnnounceURL != NULL )
        {
            AnscFreeMemory(pAnnounceURL);
        }

        if ( pTransferURL != NULL )
        {
            AnscFreeMemory(pTransferURL);
        }

        if ( pFileType != NULL )
        {
            AnscFreeMemory(pFileType);
        }

        if ( pTargetFileName != NULL )
        {
            AnscFreeMemory(pTargetFileName);
        }
    }

    status = ANSC_STATUS_SUCCESS;

EXIT:

    pMyObject->AsyncTaskCount--;

    CcspTr069PaTraceDebug(("ccspCwmppoLoadAutonomousTransferCompleteTask - exit.\n"));

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    return status;
}


ANSC_STATUS
CcspCwmppoLoadAutonomousTransferComplete
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     status                  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;

#if 0
    /*
     * If there's autonomous transfer complete event in PSM, send it out.
     */
    if ( TRUE )
    {
        pMyObject->AsyncTaskCount++;

        AnscSpawnTask
            (
                (void*)ccspCwmppoLoadAutonomousTransferCompleteTask,
                (ANSC_HANDLE)pMyObject,
                "LoadAutonomousTransferCompleteTask"
             );
    }
#endif

    if ( TRUE )
    {
        pMyObject->AsyncTaskCount++;

        /* Try to send autonomous transfer complete along with BOOT event if any */
        status = ccspCwmppoLoadAutonomousTransferCompleteTask((ANSC_HANDLE)pMyObject);
    }

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSaveDUStateChangeComplete
            (
                ANSC_HANDLE                 hThisObject
                ANSC_HANDLE                 hDsccReq
            );

    description:

        This function is called to save DUStateChangeComplete into PSM.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hDsccReq
                DSCC request.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoSaveDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hDsccReq
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    PCCSP_TR069_DSCC_REQ            pDsccReq                = (PCCSP_TR069_DSCC_REQ        )hDsccReq;
    char                            psmDsccName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    char                            buffer[257];
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;
    unsigned int                    InsNumber;
    int                             psmStatus;
    int                             i;
    PCCSP_TR069_CDS_OpResult        pDsccOpResult;

    CcspTr069PaTraceDebug(("CcspCwmppoSaveDUStateChangeComplete being called ...\n")); 

    if ( !pMyObject->bActive )
    {
        return  ANSC_STATUS_FAILURE;
    }

    _ansc_snprintf
        (
            psmDsccName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_DSCC
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmDsccName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    InsNumber = 
        CcspFindUnusedInsNumber
            (
                numInstances, 
                pInsNumbers, 
                CCSP_CWMPPO_PENDING_ATC_MAX_COUNT
            );

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    if ( InsNumber == 0 )
    {
        returnStatus = ANSC_STATUS_RESOURCES;
        goto EXIT;
    }

    _ansc_snprintf
        (
            psmDsccName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.%u.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_DSCC,
            InsNumber
        );

    psmStatus = 
        PSM_Set_Record_Value2
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmDsccName,
                ccsp_string,
                pDsccReq->CommandKey
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        returnStatus = ANSC_STATUS_INTERNAL_ERROR;
        goto EXIT;
    }

    for ( i = 0; i < pDsccReq->NumResults; i ++ )
    {
        pDsccOpResult = pDsccReq->Results + i;

        if ( pDsccOpResult->FaultCode != 0 )
        {
            _ansc_sprintf(buffer, "%u", pDsccOpResult->FaultCode);

            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    InsNumber,
                    i + 1,
                    CCSP_CWMPPO_PARAM_NAME_FaultCode
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        ccsp_unsignedInt,
                        buffer
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }

            if ( pDsccOpResult->FaultString )
            {
                _ansc_snprintf
                    (
                        psmDsccName, 
                        CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                        "%s.%s.%u.%u.%s",
                        pCcspCwmpCpeController->PANameWithPrefix,
                        CCSP_CWMPPO_FOLDER_NAME_DSCC,
                        InsNumber,
                        i+1,
                        CCSP_CWMPPO_PARAM_NAME_FaultString
                    );

                psmStatus = 
                    PSM_Set_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            psmDsccName,
                            ccsp_string,
                            pDsccOpResult->FaultString
                        );

                if ( psmStatus != CCSP_SUCCESS )
                {
                    returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                    goto EXIT;
                }
            }
        }

        if ( pDsccOpResult->Uuid )
        {
            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_Uuid
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        ccsp_string,
                        pDsccOpResult->Uuid
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pDsccOpResult->DeploymentUnitRef )
        {
            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_DeploymentUnitRef
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        ccsp_string,
                        pDsccOpResult->DeploymentUnitRef
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pDsccOpResult->Version )
        {
            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_Version
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        ccsp_string,
                        pDsccOpResult->Version
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pDsccOpResult->CurrentState )
        {
            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_CurrentState
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        ccsp_string,
                        pDsccOpResult->CurrentState
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pDsccOpResult->Resolved )
        {
            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_Resolved
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        ccsp_string,
                        pDsccOpResult->Resolved ? "1" : "0"
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pDsccOpResult->ExecutionUnitRefList )
        {
            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_ExecutionUnitRefList
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        ccsp_string,
                        pDsccOpResult->ExecutionUnitRefList
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pDsccOpResult->StartTime )
        {
            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_StartTime
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        ccsp_unsignedInt,
                        pDsccOpResult->StartTime
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }
    
        if ( pDsccOpResult->CompleteTime )
        {
            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_CompleteTime
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        ccsp_unsignedInt,
                        pDsccOpResult->CompleteTime
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }
    }

EXIT:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTr069PaTraceError
            ((
                "CcspCwmppoSaveDUStateChangeComplete - failed to create DSCC with ins number %u, returnStatus <%d>.\n", 
                (unsigned int)InsNumber, 
                (int)returnStatus
            )); 

        _ansc_snprintf
            (
                psmDsccName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_DSCC,
                InsNumber
            );

        PSM_Del_Record
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmDsccName
            );
    }
    else
    {
        CcspTr069PaTraceInfo
            ((
                "CcspCwmppoSaveDUStateChangeComplete - DSCC with ins number %u has been created in PSM.\n", 
                InsNumber
            )); 
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoLoadDUStateChangeComplete
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to load DUStateChangeComplete events
        and send them to ACS server.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
ccspCwmppoLoadDUStateChangeCompleteTask
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     status                  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf              = (PCCSP_CWMP_MSO_INTERFACE        )pCcspCwmpCpeController->GetCcspCwmpMsoIf(pCcspCwmpCpeController);
    char                            psmDsccName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    ULONG                           i,j;
    int                             psmStatus;
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;
    char*                           pValue                  = NULL;
    PCCSP_TR069_DSCC_REQ            pDsccReq                = NULL;
    unsigned int                    NumReqInstances         = 0;
    unsigned int*                   pReqInsNumbers          = NULL;
    PCCSP_TR069_CDS_OpResult        pDsccOpResult           = NULL;
    ULONG                           uStartTime, uEndTime;
    ANSC_UNIVERSAL_TIME             timeStart, timeEnd;

    CcspTr069PaTraceDebug(("ccspCwmppoLoadDUStateChangeCompleteTask - loading undelivered DUStateChangeComplete events...\n"));

    _ansc_snprintf
        (
            psmDsccName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_DSCC
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmDsccName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    if ( numInstances == 0 )
    {
        CcspTr069PaTraceDebug(("ccspCwmppoLoadDUStateChangeCompleteTask - no pending TransferComplete events.\n"));

        status = ANSC_STATUS_CANT_FIND;
        goto EXIT;
    }

#if 0
    /* wait for 30 seconds first and send the transfer complete events if have */
    while( i < 60)
    {
        AnscSleep(500);

        if(!pMyObject->bActive)
        {
            goto EXIT;
        }

        i ++;
    }
#endif 

    for ( i = 0; i < numInstances; i ++ )
    {
        pDsccReq = (PCCSP_TR069_DSCC_REQ)AnscAllocateMemory(sizeof(CCSP_TR069_DSCC_REQ));

        if ( !pDsccReq )
        {
            status = ANSC_STATUS_RESOURCES;
            goto EXIT;
        }

        _ansc_snprintf
            (
                psmDsccName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_DSCC,
                (unsigned int)pInsNumbers[i]
            );

        psmStatus = 
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmDsccName,
                    NULL,
                    &pDsccReq->CommandKey
                );

        psmStatus = 
            PsmGetNextLevelInstances
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmDsccName,
                    &NumReqInstances,
                    &pReqInsNumbers
                );

        if ( psmStatus != CCSP_SUCCESS || NumReqInstances == 0 )
        {
            CcspTr069DsccReqRemove(pDsccReq);
            status = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }

        pDsccReq->Results = 
            (PCCSP_TR069_CDS_OpResult)AnscAllocateMemory
                (
                    sizeof(CCSP_TR069_CDS_OpResult) * NumReqInstances
                );

        if ( !pDsccReq->Results )
        {
            CcspTr069DsccReqRemove(pDsccReq);
            status = ANSC_STATUS_RESOURCES;
            goto EXIT;
        }

        pDsccReq->NumResults = NumReqInstances;

        for ( j = 0; j < NumReqInstances; j ++ )
        {
            pDsccOpResult = pDsccReq->Results + j;

            if ( TRUE )
            {
                pDsccOpResult->FaultCode = 0;

                _ansc_snprintf
                    (
                        psmDsccName, 
                        CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                        "%s.%s.%u.%u.%s",
                        pCcspCwmpCpeController->PANameWithPrefix,
                        CCSP_CWMPPO_FOLDER_NAME_DSCC,
                        (unsigned int)pInsNumbers[i],
                        (unsigned int)j+1,
                        CCSP_CWMPPO_PARAM_NAME_FaultCode
                    );

                psmStatus = 
                    PSM_Get_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            psmDsccName,
                            NULL,
                            &pValue
                        );

                if ( psmStatus == CCSP_SUCCESS && pValue )
                {
                    pDsccOpResult->FaultCode = (ULONG)_ansc_atoi(pValue);
                    AnscFreeMemory(pValue);
                }
            }

            if ( pDsccOpResult->FaultCode != 0 )
            {
                _ansc_snprintf
                    (
                        psmDsccName, 
                        CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                        "%s.%s.%u.%s",
                        pCcspCwmpCpeController->PANameWithPrefix,
                        CCSP_CWMPPO_FOLDER_NAME_DSCC,
                        (unsigned int)pInsNumbers[i],
                        CCSP_CWMPPO_PARAM_NAME_FaultString
                    );

                psmStatus = 
                    PSM_Get_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            psmDsccName,
                            NULL,
                            &pDsccOpResult->FaultString
                        );
            }

            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_Uuid
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        NULL,
                        &pDsccOpResult->Uuid
                    );

            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_DeploymentUnitRef
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        NULL,
                        &pDsccOpResult->DeploymentUnitRef
                    );

            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_Version
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        NULL,
                        &pDsccOpResult->Version
                    );

            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_CurrentState
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        NULL,
                        &pDsccOpResult->CurrentState
                    );

            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_Resolved
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        NULL,
                        &pValue
                    );

            pDsccOpResult->Resolved = FALSE;

            if ( psmStatus == CCSP_SUCCESS && pValue )
            {
                pDsccOpResult->Resolved = (BOOL)_ansc_atoi(pValue);
                AnscFreeMemory(pValue);
            }

            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_ExecutionUnitRefList
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmDsccName,
                        NULL,
                        &pDsccOpResult->ExecutionUnitRefList
                    );

            if ( TRUE )
            {
                _ansc_snprintf
                    (
                        psmDsccName, 
                        CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                        "%s.%s.%u.%u.%s",
                        pCcspCwmpCpeController->PANameWithPrefix,
                        CCSP_CWMPPO_FOLDER_NAME_DSCC,
                        (unsigned int)pInsNumbers[i],
                        (unsigned int)j+1,
                        CCSP_CWMPPO_PARAM_NAME_StartTime
                    );

                psmStatus = 
                    PSM_Get_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            psmDsccName,
                            NULL,
                            &pValue
                        );

                if ( psmStatus == CCSP_SUCCESS && pValue )
                {
                    uStartTime = (ULONG)_ansc_atoi(pValue);
                    AnscZeroMemory(&timeStart, sizeof(ANSC_UNIVERSAL_TIME));
                    AnscSecondToCalendar(uStartTime, &timeStart);

                    AnscFreeMemory(pValue);
                }
                else
                {
                    /* unknown time */
                    timeStart.Year  = 1;
                    timeStart.Month = 1;
                    timeStart.DayOfMonth = 1;
                }
            }

            if ( TRUE )
            {
                _ansc_snprintf
                    (
                        psmDsccName, 
                        CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                        "%s.%s.%u.%u.%s",
                        pCcspCwmpCpeController->PANameWithPrefix,
                        CCSP_CWMPPO_FOLDER_NAME_DSCC,
                        (unsigned int)pInsNumbers[i],
                        (unsigned int)j+1,
                        CCSP_CWMPPO_PARAM_NAME_CompleteTime
                    );

                psmStatus = 
                    PSM_Get_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            psmDsccName,
                            NULL,
                            &pValue
                        );

                if ( psmStatus == CCSP_SUCCESS && pValue )
                {
                    uEndTime = (ULONG)_ansc_atoi(pValue);
                    AnscZeroMemory(&timeStart, sizeof(ANSC_UNIVERSAL_TIME));
                    AnscSecondToCalendar(uEndTime, &timeEnd);

                    AnscFreeMemory(pValue);
                }
                else
                {
                    /* unknown time */
                    timeEnd.Year  = 1;
                    timeEnd.Month = 1;
                    timeEnd.DayOfMonth = 1;
                }
            }
        }

        CcspTr069PaTraceDebug(("ccspCwmppoLoadDUStateChangeCompleteTask - sending DUStateChangeComplete, command key <%s>.\n", pDsccReq->CommandKey));

        status = 
            pCcspCwmpMsoIf->DuStateChangeComplete
                            (
                                pCcspCwmpMsoIf->hOwnerContext, 
                                (ANSC_HANDLE)pDsccReq,
                                FALSE           /* use current session to deliver this */
                            );

        if ( status != ANSC_STATUS_SUCCESS )
        {
            CcspTr069PaTraceWarning(("Failed to send out 'DUStateChangeComplete' whose command key is '%s'\n", pDsccReq->CommandKey));
        }
        else
        {
            /* remove delivered TC from PSM */
            _ansc_snprintf
                (
                    psmDsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_DSCC,
                    pInsNumbers[i]
                );

            PSM_Del_Record
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmDsccName
                );
        }

        CcspTr069DsccReqRemove(pDsccReq);
        pDsccReq = NULL;

        if ( !pMyObject->bActive )
        {
            goto EXIT;
        }
    }

    status = ANSC_STATUS_SUCCESS;

EXIT:

    pMyObject->AsyncTaskCount--;

    CcspTr069PaTraceDebug(("ccspCwmppoLoadDUStateChangeCompleteTask - exit.\n"));

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    return status;
}


ANSC_STATUS
CcspCwmppoLoadDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     status                  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;

#if 0
    /*
     * If there's transfer complete event in PSM, send it out.
     */
    if ( TRUE )
    {
        pMyObject->AsyncTaskCount++;

        AnscSpawnTask
            (
                (void*)ccspCwmppoLoadDUStateChangeCompleteTask,
                (ANSC_HANDLE)pMyObject,
                "LoadDUStateChangeCompleteTask"
             );
    }
#endif

    if ( TRUE )
    {
        pMyObject->AsyncTaskCount++;

        /* Try to send transfer complete along with BOOT event if any */
        status = ccspCwmppoLoadDUStateChangeCompleteTask((ANSC_HANDLE)pMyObject);
    }

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSaveAutonomousDUStateChangeComplete
            (
                ANSC_HANDLE                 hThisObject
                ANSC_HANDLE                 hAdsccReq
            );

    description:

        This function is called to save AutonomousDUStateChangeComplete into PSM.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hAdsccReq
                DSCC request.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoSaveAutonomousDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hAdsccReq
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    PCCSP_TR069_ADSCC_REQ           pAdsccReq               = (PCCSP_TR069_ADSCC_REQ       )hAdsccReq;
    char                            psmAdsccName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    char                            buffer[257];
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;
    unsigned int                    InsNumber;
    int                             psmStatus;
    int                             i;
    PCCSP_TR069_ADSCC_OpResult      pAdsccOpResult;

    CcspTr069PaTraceDebug(("CcspCwmppoSaveAutonomousDUStateChangeComplete being called ...\n")); 

    if ( !pMyObject->bActive )
    {
        return  ANSC_STATUS_FAILURE;
    }

    _ansc_snprintf
        (
            psmAdsccName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_ADSCC
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmAdsccName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    InsNumber = 
        CcspFindUnusedInsNumber
            (
                numInstances, 
                pInsNumbers, 
                CCSP_CWMPPO_PENDING_ATC_MAX_COUNT
            );

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    if ( InsNumber == 0 )
    {
        returnStatus = ANSC_STATUS_RESOURCES;
        goto EXIT;
    }

    for ( i = 0; i < pAdsccReq->NumResults; i ++ )
    {
        pAdsccOpResult = pAdsccReq->Results + i;

        if ( pAdsccOpResult->FaultCode != 0 )
        {
            _ansc_sprintf(buffer, "%u", pAdsccOpResult->FaultCode);

            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    InsNumber,
                    i + 1,
                    CCSP_CWMPPO_PARAM_NAME_FaultCode
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        ccsp_unsignedInt,
                        buffer
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }

            if ( pAdsccOpResult->FaultString )
            {
                _ansc_snprintf
                    (
                        psmAdsccName, 
                        CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                        "%s.%s.%u.%u.%s",
                        pCcspCwmpCpeController->PANameWithPrefix,
                        CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                        InsNumber,
                        i+1,
                        CCSP_CWMPPO_PARAM_NAME_FaultCode
                    );

                psmStatus = 
                    PSM_Set_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            psmAdsccName,
                            ccsp_string,
                            pAdsccOpResult->FaultString
                        );

                if ( psmStatus != CCSP_SUCCESS )
                {
                    returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                    goto EXIT;
                }
            }
        }

        if ( pAdsccOpResult->Uuid )
        {
            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_Uuid
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        ccsp_string,
                        pAdsccOpResult->Uuid
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pAdsccOpResult->DeploymentUnitRef )
        {
            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_DeploymentUnitRef
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        ccsp_string,
                        pAdsccOpResult->DeploymentUnitRef
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pAdsccOpResult->Version )
        {
            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_Version
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        ccsp_string,
                        pAdsccOpResult->Version
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pAdsccOpResult->OperationPerformed )
        {
            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_OperationPerformed
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        ccsp_string,
                        pAdsccOpResult->OperationPerformed
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pAdsccOpResult->CurrentState )
        {
            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_CurrentState
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        ccsp_string,
                        pAdsccOpResult->CurrentState
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pAdsccOpResult->Resolved )
        {
            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_Resolved
                );

			if ( pAdsccOpResult->Resolved )
			{
            	_ansc_sprintf(buffer, "1");
			}
			else
			{
            	_ansc_sprintf(buffer, "0");
			}

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        ccsp_boolean,
                        buffer
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pAdsccOpResult->ExecutionUnitRefList )
        {
            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_ExecutionUnitRefList
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        ccsp_string,
                        pAdsccOpResult->ExecutionUnitRefList
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }

        if ( pAdsccOpResult->StartTime )
        {
            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_StartTime
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        ccsp_unsignedInt,
                        pAdsccOpResult->StartTime
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }
    
        if ( pAdsccOpResult->CompleteTime )
        {
            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    InsNumber,
                    i+1,
                    CCSP_CWMPPO_PARAM_NAME_CompleteTime
                );

            psmStatus = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        ccsp_unsignedInt,
                        pAdsccOpResult->CompleteTime
                    );

            if ( psmStatus != CCSP_SUCCESS )
            {
                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                goto EXIT;
            }
        }
    }

EXIT:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTr069PaTraceError
            ((
                "CcspCwmppoSaveAutonomousDUStateChangeComplete - failed to create DSCC with ins number %u, returnStatus <%d>.\n", 
                InsNumber, 
                (int)returnStatus
            )); 

        _ansc_snprintf
            (
                psmAdsccName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                InsNumber
            );

        PSM_Del_Record
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmAdsccName
            );
    }
    else
    {
        CcspTr069PaTraceInfo
            ((
                "CcspCwmppoSaveAutonomousDUStateChangeComplete - DSCC with ins number %u has been created in PSM.\n", 
                InsNumber
            )); 
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoLoadAutonomousDUStateChangeComplete
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to load AutonomousDUStateChangeComplete events
        and send them to ACS server.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
ccspCwmppoLoadAutonomousDUStateChangeCompleteTask
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     status                  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf              = (PCCSP_CWMP_MSO_INTERFACE        )pCcspCwmpCpeController->GetCcspCwmpMsoIf(pCcspCwmpCpeController);
    char                            psmAdsccName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    ULONG                           i,j;
    int                             psmStatus;
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;
    char*                           pValue                  = NULL;
    PCCSP_TR069_ADSCC_REQ           pAdsccReq                = NULL;
    unsigned int                    NumReqInstances         = 0;
    unsigned int*                   pReqInsNumbers          = NULL;
    PCCSP_TR069_ADSCC_OpResult      pAdsccOpResult           = NULL;
    ULONG                           uStartTime, uEndTime;
    ANSC_UNIVERSAL_TIME             timeStart, timeEnd;

    CcspTr069PaTraceDebug(("ccspCwmppoLoadAutonomousDUStateChangeCompleteTask - loading undelivered AutonomousDUStateChangeComplete events...\n"));

    _ansc_snprintf
        (
            psmAdsccName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_ADSCC
        );

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmAdsccName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    if ( numInstances == 0 )
    {
        CcspTr069PaTraceDebug(("ccspCwmppoLoadAutonomousDUStateChangeCompleteTask - no pending TransferComplete events.\n"));

        status = ANSC_STATUS_CANT_FIND;
        goto EXIT;
    }

#if 0
    /* wait for 30 seconds first and send the transfer complete events if have */
    while( i < 60)
    {
        AnscSleep(500);

        if(!pMyObject->bActive)
        {
            goto EXIT;
        }

        i ++;
    }
#endif 

    for ( i = 0; i < numInstances; i ++ )
    {
        pAdsccReq = (PCCSP_TR069_ADSCC_REQ)AnscAllocateMemory(sizeof(CCSP_TR069_ADSCC_REQ));

        if ( !pAdsccReq )
        {
            status = ANSC_STATUS_RESOURCES;
            goto EXIT;
        }

        _ansc_snprintf
            (
                psmAdsccName, 
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                "%s.%s.%u.",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                (unsigned int)pInsNumbers[i]
            );

        psmStatus = 
            PsmGetNextLevelInstances
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmAdsccName,
                    &NumReqInstances,
                    &pReqInsNumbers
                );

        if ( psmStatus != CCSP_SUCCESS || NumReqInstances == 0 )
        {
            CcspTr069AdsccReqRemove(pAdsccReq);
            status = ANSC_STATUS_INTERNAL_ERROR;
            goto EXIT;
        }

        pAdsccReq->Results = 
            (PCCSP_TR069_ADSCC_OpResult)AnscAllocateMemory
                (
                    sizeof(CCSP_TR069_ADSCC_OpResult) * NumReqInstances
                );

        if ( !pAdsccReq->Results )
        {
            CcspTr069AdsccReqRemove(pAdsccReq);
            status = ANSC_STATUS_RESOURCES;
            goto EXIT;
        }

        pAdsccReq->NumResults = NumReqInstances;

        for ( j = 0; j < NumReqInstances; j ++ )
        {
            pAdsccOpResult = pAdsccReq->Results + j;

            if ( TRUE )
            {
                pAdsccOpResult->FaultCode = 0;

                _ansc_snprintf
                    (
                        psmAdsccName, 
                        CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                        "%s.%s.%u.%u.%s",
                        pCcspCwmpCpeController->PANameWithPrefix,
                        CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                        (unsigned int)pInsNumbers[i],
                        (unsigned int)j+1,
                        CCSP_CWMPPO_PARAM_NAME_FaultCode
                    );

                psmStatus = 
                    PSM_Get_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            psmAdsccName,
                            NULL,
                            &pValue
                        );

                if ( psmStatus == CCSP_SUCCESS && pValue )
                {
                    pAdsccOpResult->FaultCode = (ULONG)_ansc_atoi(pValue);
                    AnscFreeMemory(pValue);
                }
            }

            if ( pAdsccOpResult->FaultCode != 0 )
            {
                _ansc_snprintf
                    (
                        psmAdsccName, 
                        CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                        "%s.%s.%u.%s",
                        pCcspCwmpCpeController->PANameWithPrefix,
                        CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                        (unsigned int)pInsNumbers[i],
                        CCSP_CWMPPO_PARAM_NAME_FaultString
                    );

                psmStatus = 
                    PSM_Get_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            psmAdsccName,
                            NULL,
                            &pAdsccOpResult->FaultString
                        );
            }

            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_Uuid
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        NULL,
                        &pAdsccOpResult->Uuid
                    );

            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_DeploymentUnitRef
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        NULL,
                        &pAdsccOpResult->DeploymentUnitRef
                    );

            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_Version
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        NULL,
                        &pAdsccOpResult->Version
                    );

            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_OperationPerformed
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        NULL,
                        &pAdsccOpResult->OperationPerformed
                    );

            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_CurrentState
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        NULL,
                        &pAdsccOpResult->CurrentState
                    );

            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_Resolved
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        NULL,
                        &pValue
                    );

            pAdsccOpResult->Resolved = FALSE;

            if ( psmStatus == CCSP_SUCCESS && pValue )
            {
                pAdsccOpResult->Resolved = (BOOL)_ansc_atoi(pValue);
                AnscFreeMemory(pValue);
            }

            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.%u.%s",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    (unsigned int)pInsNumbers[i],
                    (unsigned int)j+1,
                    CCSP_CWMPPO_PARAM_NAME_ExecutionUnitRefList
                );

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        psmAdsccName,
                        NULL,
                        &pAdsccOpResult->ExecutionUnitRefList
                    );

            if ( TRUE )
            {
                _ansc_snprintf
                    (
                        psmAdsccName, 
                        CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                        "%s.%s.%u.%u.%s",
                        pCcspCwmpCpeController->PANameWithPrefix,
                        CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                        (unsigned int)pInsNumbers[i],
                        (unsigned int)j+1,
                        CCSP_CWMPPO_PARAM_NAME_StartTime
                    );

                psmStatus = 
                    PSM_Get_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            psmAdsccName,
                            NULL,
                            &pValue
                        );

                if ( psmStatus == CCSP_SUCCESS && pValue )
                {
                    uStartTime = (ULONG)_ansc_atoi(pValue);
                    AnscZeroMemory(&timeStart, sizeof(ANSC_UNIVERSAL_TIME));
                    AnscSecondToCalendar(uStartTime, &timeStart);

                    AnscFreeMemory(pValue);
                }
                else
                {
                    /* unknown time */
                    timeStart.Year  = 1;
                    timeStart.Month = 1;
                    timeStart.DayOfMonth = 1;
                }
            }

            if ( TRUE )
            {
                _ansc_snprintf
                    (
                        psmAdsccName, 
                        CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                        "%s.%s.%u.%u.%s",
                        pCcspCwmpCpeController->PANameWithPrefix,
                        CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                        (unsigned int)pInsNumbers[i],
                        (unsigned int)j+1,
                        CCSP_CWMPPO_PARAM_NAME_CompleteTime
                    );

                psmStatus = 
                    PSM_Get_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            psmAdsccName,
                            NULL,
                            &pValue
                        );

                if ( psmStatus == CCSP_SUCCESS && pValue )
                {
                    uEndTime = (ULONG)_ansc_atoi(pValue);
                    AnscZeroMemory(&timeStart, sizeof(ANSC_UNIVERSAL_TIME));
                    AnscSecondToCalendar(uEndTime, &timeEnd);

                    AnscFreeMemory(pValue);
                }
                else
                {
                    /* unknown time */
                    timeEnd.Year  = 1;
                    timeEnd.Month = 1;
                    timeEnd.DayOfMonth = 1;
                }
            }
        }

        CcspTr069PaTraceDebug(("ccspCwmppoLoadAutonomousDUStateChangeCompleteTask - sending AutonomousDUStateChangeComplete...\n"));

        status =
            pCcspCwmpMsoIf->AutonomousDuStateChangeComplete
                (
                    pCcspCwmpMsoIf->hOwnerContext, 
                    (ANSC_HANDLE)pAdsccReq,
                    FALSE           /* use current session to deliver this */
                );

        if ( status != ANSC_STATUS_SUCCESS )
        {
            CcspTr069PaTraceWarning(("Failed to send out 'AutonomousDUStateChangeComplete'!\n"));
        }
        else
        {
            /* remove delivered TC from PSM */
            _ansc_snprintf
                (
                    psmAdsccName, 
                    CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
                    "%s.%s.%u.",
                    pCcspCwmpCpeController->PANameWithPrefix,
                    CCSP_CWMPPO_FOLDER_NAME_ADSCC,
                    pInsNumbers[i]
                );

            PSM_Del_Record
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmAdsccName
                );
        }

        CcspTr069AdsccReqRemove(pAdsccReq);
        pAdsccReq = NULL;

        if ( !pMyObject->bActive )
        {
            goto EXIT;
        }
    }

    status = ANSC_STATUS_SUCCESS;

EXIT:

    pMyObject->AsyncTaskCount--;

    CcspTr069PaTraceDebug(("ccspCwmppoLoadAutonomousDUStateChangeCompleteTask - exit.\n"));

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
    }

    return status;
}


ANSC_STATUS
CcspCwmppoLoadAutonomousDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     status                  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;

#if 0
    /*
     * If there's transfer complete event in PSM, send it out.
     */
    if ( TRUE )
    {
        pMyObject->AsyncTaskCount++;

        AnscSpawnTask
            (
                (void*)ccspCwmppoLoadAutonomousDUStateChangeCompleteTask,
                (ANSC_HANDLE)pMyObject,
                "LoadAutonomousDUStateChangeCompleteTask"
             );
    }
#endif

    if ( TRUE )
    {
        pMyObject->AsyncTaskCount++;

        /* Try to send transfer complete along with BOOT event if any */
        status = ccspCwmppoLoadAutonomousDUStateChangeCompleteTask((ANSC_HANDLE)pMyObject);
    }

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoDiscardUndeliverEvents
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to discard undelivered events.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoDiscardUndeliveredEvents
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT)hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     
                                    pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    char                            psmRecordName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];

    _ansc_snprintf
        (
            psmRecordName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_TransferComplete
        );

    PSM_Del_Record
        (
            pCcspCwmpCpeController->hMsgBusHandle,
            pCcspCwmpCpeController->SubsysName,
            psmRecordName
        );

    _ansc_snprintf
        (
            psmRecordName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete
        );

    PSM_Del_Record
        (
            pCcspCwmpCpeController->hMsgBusHandle,
            pCcspCwmpCpeController->SubsysName,
            psmRecordName
        );

    _ansc_snprintf
        (
            psmRecordName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_DSCC
        );

    PSM_Del_Record
        (
            pCcspCwmpCpeController->hMsgBusHandle,
            pCcspCwmpCpeController->SubsysName,
            psmRecordName
        );

    _ansc_snprintf
        (
            psmRecordName, 
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN, 
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_ADSCC
        );

    PSM_Del_Record
        (
            pCcspCwmpCpeController->hMsgBusHandle,
            pCcspCwmpCpeController->SubsysName,
            psmRecordName
        );

    _ansc_snprintf
        (
            psmRecordName,
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN,
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_ValueChanged
        );

    PSM_Del_Record
        (
            pCcspCwmpCpeController->hMsgBusHandle,
            pCcspCwmpCpeController->SubsysName,
            psmRecordName
        );
    
    return  returnStatus;
}

/***********************************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoLoadValueChanged
            (
                ANSC_HANDLE                 hThisObject,
            );

    description:

        This function is called to load undelivered value changed events
        4 VALUE CHANGE event should not be discarded in case agent of crash or unexpected reboot
        CcspCwmppoLoadValueChangedTask reads the event entries (eRT.com.cisco.spvtg.ccsp.tr069pa.Undelivered_VC.x)
        from psm and triggers new value change event based on each parameter attribute
        PSM entries will contain only parameter name

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**************************************************************************************/
ANSC_STATUS
CcspCwmppoLoadValueChangedTask
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject                  = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf         = (PCCSP_CWMP_MSO_INTERFACE        )pCcspCwmpCpeController->GetCcspCwmpMsoIf(pCcspCwmpCpeController);
    ULONG                           i                           = 0;
    ULONG                           Notification                = 0;
    int                             dataType                    = 0;
    char                            psmTcName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    int                             psmStatus;
    int                             status;
    unsigned int                    numInstances                = 0;
    unsigned int*                   pInsNumbers                 = NULL;
    char*                           pValue                      = NULL;
    char*                           pParameterName              = NULL;
    char*                           pParameterValue             = NULL;

    CcspTr069PaTraceDebug((" Load Undelivered ValueChangeEvent Task\n"));
    _ansc_snprintf
        (
            psmTcName,
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN,
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_ValueChanged
        );

    CcspTr069PaTraceInfo(("CcspCwmppoLoadValueChanged - ValueChanged <%s>\n", psmTcName));
    psmStatus =
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                &numInstances,
                &pInsNumbers
            );

    CcspTr069PaTraceInfo(("CcspCwmppoLoadeValueChanged - number of instances <%d>\n", numInstances));
    if ( psmStatus != CCSP_SUCCESS )
    {
        CcspTr069PaTraceError(("CcspCwmppoLoadeValueChanged - failed to get number of instances\n"));
        status = ANSC_STATUS_INTERNAL_ERROR;
        goto EXIT;
    }

    if ( numInstances == 0 )
    {
        CcspTr069PaTraceWarning(("ccspCwmppoLoadValueChanged - no pending ValueChanged events.\n"));
        status = ANSC_STATUS_CANT_FIND;
        goto EXIT;
    }

    for ( i = 0; i < numInstances; i ++ )
    {
        _ansc_snprintf
            (
                psmTcName,
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN,
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_ValueChanged,
                pInsNumbers[i],
                CCSP_CWMPPO_PARAM_NAME_ValueChanged
            );

        psmStatus =
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    NULL,
                    &pValue
                );

        if ( psmStatus == CCSP_SUCCESS )
        {
            /* Storing the original Parameter name */
            pParameterName = AnscCloneString(pValue);
            CcspCwmppoMapParamInstNumDmIntToCwmp(pValue);

            pCcspCwmpCpeController->GetParamStringValue
                (
                    (ANSC_HANDLE)pCcspCwmpCpeController,
                    pValue,
                    &pParameterValue
                );
            Notification = pMyObject->CheckParamAttrCache((ANSC_HANDLE)pMyObject, pParameterName);
            dataType = pCcspCwmpCpeController->GetParamDataType
                (
                    (ANSC_HANDLE)pCcspCwmpCpeController,
                    pParameterName
                );
            if ( (pParameterValue != NULL) && (pParameterName != NULL) )
            {
                CcspTr069PaTraceInfo(("CcspCwmppoLoadValueChanged - ParameterName:%s, ParameterValue:%s, Notification:%lu, datatype:%d\n", pParameterName, pParameterValue, Notification, dataType));
                /* notify value change */
                pCcspCwmpMsoIf->ValueChanged( pCcspCwmpMsoIf->hOwnerContext, pParameterName, pParameterValue, dataType, Notification == CCSP_CWMP_NOTIFICATION_active );
            }
            else
            {
                CcspTr069PaTraceWarning(("CcspCwmppoLoadValueChanged - Something went wrong with entry %s,deleting it....\n", psmTcName));
                PSM_Del_Record
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName
                );
            }
        }
        if ( pValue != NULL )
        {
            AnscFreeMemory(pValue);
            pValue = NULL;
        }

        if ( pParameterValue != NULL )
        {
            AnscFreeMemory(pParameterValue);
            pParameterValue = NULL;
        }

        if ( pParameterName != NULL )
        {
            AnscFreeMemory(pParameterName);
            pParameterName = NULL;
        }

    }

    status = ANSC_STATUS_SUCCESS;

EXIT:

    pMyObject->AsyncTaskCount--;

    CcspTr069PaTraceDebug(("ccspCwmppoLoadValueChangedTask - exit.\n"));

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
        pInsNumbers = NULL;
    }

    return status;
}

ANSC_STATUS
CcspCwmppoLoadValueChanged
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject                  = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;

    CcspTr069PaTraceDebug((" Load Undelivered Value Changed Event\n"));
    pMyObject->AsyncTaskCount++;
    CcspCwmppoLoadValueChangedTask((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}

/***********************************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSaveValueChanged
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pParameterName
            );

    description:

        This function is called to save value change event
        CcspCwmppoSaveValueChanged creates new entries with record names
        (eRT.com.cisco.spvtg.ccsp.tr069pa.Undelivered_VC.x.ValueChanged) into psm
        <Record name="eRT.com.cisco.spvtg.ccsp.tr069pa.Undelivered_VC.1.ValueChanged" type="astr">Device.WiFi.SSID.2.SSID</Record>
        Even if value change is triggered multiple times for a parameter, single entry will be maintained

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pParameterName
                ParameterName which needs to be saved in to psm

    return:     status of operation.

**************************************************************************************/
ANSC_STATUS
CcspCwmppoSaveValueChanged
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParameterName
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    char                            psmTcName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    char*                           pValue                  = NULL;
    ULONG                           i                       = 0;
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;
    unsigned int                    InsNumber               = 0;
    int                             psmStatus;

    CcspTr069PaTraceDebug(("CcspCwmppoSaveValueChanged - Parameter name <%s>.\n", pParameterName));

    if ( !pMyObject->bActive )
    {
        return  ANSC_STATUS_FAILURE;
    }
    _ansc_snprintf
        (
            psmTcName,
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN,
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_ValueChanged
        );

    psmStatus =
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        CcspTr069PaTraceError(("CcspCwmppoSaveValueChanged - failed to get number of instances\n"));
        returnStatus = ANSC_STATUS_INTERNAL_ERROR;
        goto EXIT;
    }

    for ( i = 0; i < numInstances; i ++ )
    {
        _ansc_snprintf
            (
                psmTcName,
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN,
                "%s.%s.%u.%s",
                pCcspCwmpCpeController->PANameWithPrefix,
                CCSP_CWMPPO_FOLDER_NAME_ValueChanged,
                pInsNumbers[i],
                CCSP_CWMPPO_PARAM_NAME_ValueChanged
            );

        psmStatus =
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    NULL,
                    &pValue
                );

        if ( psmStatus == CCSP_SUCCESS && ( 0 == _ansc_strcmp(pValue, pParameterName ) ) )
        {
            CcspTr069PaTraceWarning(("CcspCwmppoSaveValueChanged - Parameter %s entry already is available in psm\n",pParameterName));
            AnscFreeMemory(pValue);
            pValue = NULL;
            returnStatus = ANSC_STATUS_SUCCESS;
            goto EXIT;
        }

        if ( pValue != NULL )
        {
            AnscFreeMemory(pValue);
            pValue = NULL;
        }

    }

    InsNumber =
        CcspFindUnusedInsNumber
            (
                numInstances,
                pInsNumbers,
                CCSP_CWMPPO_PENDING_TC_MAX_COUNT
            );

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
        pInsNumbers = NULL;
    }

    if ( InsNumber == 0 )
    {
        returnStatus = ANSC_STATUS_RESOURCES;
        goto EXIT;
    }

    _ansc_snprintf
        (
            psmTcName,
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN,
            "%s.%s.%u.%s",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_ValueChanged,
            InsNumber,
            CCSP_CWMPPO_PARAM_NAME_ValueChanged
        );

    psmStatus =
        PSM_Set_Record_Value2
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                ccsp_string,
                pParameterName
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        returnStatus = ANSC_STATUS_INTERNAL_ERROR;
        goto EXIT;
    }

EXIT:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTr069PaTraceError
            ((
                "CcspCwmppoSaveValueChanged - failed to create Entry with ins number %u, returnStatus <%d>.\n",
                InsNumber,
                (int)returnStatus
            ));
    }
    else
    {
        CcspTr069PaTraceInfo
            ((
                "CcspCwmppoSaveValueChanged - Entry with ins number %u has been created in PSM.\n",
                InsNumber
            ));
    }

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
        pInsNumbers = NULL;
    }

    return  returnStatus;
}

/***********************************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoDiscardValueChanged
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pParameterName
            );

    description:

        This function is called to discard value changed event
        CcspCwmppoDiscardValueChanged deletes corresponding parameter entry from the psm

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pParameterName
                ParameterName which needs to be removed from the psm entries

    return:     status of operation.

**************************************************************************************/
ANSC_STATUS
CcspCwmppoDiscardValueChanged
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParameterName
    )
{
    ANSC_STATUS                     status                  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    char                            psmTcName[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN];
    ULONG                           i                       = 0;
    int                             psmStatus;
    unsigned int                    numInstances            = 0;
    unsigned int*                   pInsNumbers             = NULL;
    char*                           pValue                  = NULL;

    CcspTr069PaTraceWarning(("CcspCwmppoDiscardValueChanged - Discarding Value Changed events...\n"));
    _ansc_snprintf
        (
            psmTcName,
            CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN,
            "%s.%s.",
            pCcspCwmpCpeController->PANameWithPrefix,
            CCSP_CWMPPO_FOLDER_NAME_ValueChanged
        );

    psmStatus =
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus != CCSP_SUCCESS )
    {
        CcspTr069PaTraceError(("CcspCwmppoDiscardValueChanged - failed to get number of instances\n"));
        status = ANSC_STATUS_INTERNAL_ERROR;
        goto EXIT;
    }

    if ( numInstances == 0 )
    {
        CcspTr069PaTraceWarning(("ccspCwmppoDiscardValueChanged - no pending ValueChanged events to discard.\n"));
        status = ANSC_STATUS_CANT_FIND;
        goto EXIT;
    }

    for ( i = 0; i < numInstances; i ++ )
    {
        _ansc_snprintf
            (
                psmTcName,
                CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN,
                "%s.%s.%u.%s",
                 pCcspCwmpCpeController->PANameWithPrefix,
                 CCSP_CWMPPO_FOLDER_NAME_ValueChanged,
                 pInsNumbers[i],
                 CCSP_CWMPPO_PARAM_NAME_ValueChanged
            );

        psmStatus =
            PSM_Get_Record_Value2
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    psmTcName,
                    NULL,
                    &pValue
                );

        if ( psmStatus == CCSP_SUCCESS && (0 == _ansc_strcmp(pValue, pParameterName )) )
        {
            PSM_Del_Record
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                psmTcName
            );
        }

        if ( pValue != NULL )
        {
            AnscFreeMemory(pValue);
            pValue = NULL;
        }

    }

    status = ANSC_STATUS_SUCCESS;

EXIT:

    CcspTr069PaTraceDebug(("ccspCwmppoDiscardValueChanged - exit.\n"));

    if ( pInsNumbers )
    {
        AnscFreeMemory(pInsNumbers);
        pInsNumbers = NULL;
    }

    return status;
}
