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

    module: ccsp_cwmp_cpeco_control.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced control functions
        of the CCSP CWMP Cpe Controller Object.

        *   CcspCwmpCpecoLoadEvents
        *   CcspCwmpCpecoStartCWMP
        *   CcspCwmpCpecoStopCWMP
        *   CcspCwmpCpecoInformNow
        *   CcspCwmpCpecoNotifyEvent
        *   CcspCwmpCpecoAcqCrSessionID
        *   CcspCwmpCpecoRelCrSessionID

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang    Quan

    ---------------------------------------------------------------

    revision:

        09/09/05    initial revision.
        06/16/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/13/11    resolve name conflicts with DM library.
        
**********************************************************************/


#include "ccsp_cwmp_cpeco_global.h"
#include "ccsp_management_server.h"

extern int checkIfSystemReady(void);

/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoLoadEvents
            (
                ANSC_HANDLE                 hCcspCwmpMsoIf,
                char*                       pSavedEvents
            );

    description:

        This function is called to load cwmp events

    argument:
                ANSC_HANDLE                 hCcspCwmpMsoIf,
                The CCSP CWMP MSO interface;

                char*                       pSavedEvents
                The saved CWMP event string

    return:     status of operation.

**********************************************************************/
ANSC_STATUS
CcspCwmpCpecoLoadEvents
    (
        ANSC_HANDLE                 hCcspCwmpMsoIf,
        char*                       pSavedEvents
    )
{
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf          = (PCCSP_CWMP_MSO_INTERFACE)hCcspCwmpMsoIf;
    PANSC_TOKEN_CHAIN               pListTokenChain     = (PANSC_TOKEN_CHAIN         )NULL;
    PANSC_STRING_TOKEN              pStringToken        = (PANSC_STRING_TOKEN        )NULL;
    PCHAR                           pString             = NULL;
    PCHAR                           pString2            = NULL;
    ULONG                           length              = 0;
    CHAR                            pEventName[128]     = { 0 };
    CHAR                            pEventKey[128]      = { 0 };

    if( pSavedEvents == NULL)
    {
        return ANSC_STATUS_FAILURE;
    }

    pListTokenChain =
        AnscTcAllocate
            (
                pSavedEvents,
                ","
            );

    if ( !pListTokenChain )
    {
        return  ANSC_STATUS_FAILURE;
    }

    while ( (pStringToken = AnscTcUnlinkToken(pListTokenChain)) )
    {
        AnscZeroMemory(pEventName, 128);
        AnscZeroMemory(pEventKey,  128);

        pString = pStringToken->Name;

        length = AnscSizeOfString(pString);

        if( length > 0)
        {
            pString2 = _ansc_strchr(pString,'+');

            if( pString2 == NULL)
            {
                pCcspCwmpMsoIf->Inform
                    (
                        (ANSC_HANDLE)pCcspCwmpMsoIf->hOwnerContext,
                        pString,
                        "",
                        FALSE
                    );
            }
            else
            {
                AnscCopyMemory(pEventName, pString, (ULONG)(pString2 - pString));

                if( length - 1 == (ULONG)(pString2 - pString))
                {
                    pCcspCwmpMsoIf->Inform
                        (
                            (ANSC_HANDLE)pCcspCwmpMsoIf->hOwnerContext,
                            pEventName,
                            "",
                            FALSE
                        );
                }
                else
                {
                    pString2 += 1;

                    AnscCopyMemory(pEventKey, pString2, length - 1 - (ULONG)(pString2 - pString));

                    pCcspCwmpMsoIf->Inform
                        (
                            (ANSC_HANDLE)pCcspCwmpMsoIf->hOwnerContext,
                            pEventName,
                            pEventKey,
                            FALSE
                        );
                }
            }
        }

        AnscFreeMemory(pStringToken);
    }

    if ( pListTokenChain )
    {
        AnscTcFree((ANSC_HANDLE)pListTokenChain);
    }

    return ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        ccspCwmpCpeFirstInformTask
            (
                ANSC_HANDLE                 hTaskContext
            );

    description:

        This function is called to send boot event.

    argument:   ANSC_HANDLE                 hTaskContext
                Task Context.

    return:     status of operation.

**********************************************************************/

typedef  struct
_CCSP_CWMP_BOOT_INFORM_CTX
{
    ANSC_HANDLE                     hCcspCwmpCpeController;
    ULONG                           ulDelay;
}
CCSP_CWMP_BOOT_INFORM_CTX, *PCCSP_CWMP_BOOT_INFORM_CTX;


ANSC_STATUS
ccspCwmpCpeFirstInformTask
    (
        ANSC_HANDLE                 hTaskContext
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_BOOT_INFORM_CTX           pBITaskContext     = (PCCSP_CWMP_BOOT_INFORM_CTX        )hTaskContext;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )pBITaskContext->hCcspCwmpCpeController;
    PCCSP_CWMP_ACS_BROKER_OBJECT         pCcspCwmpAcsBroker     = (PCCSP_CWMP_ACS_BROKER_OBJECT      )pMyObject->hCcspCwmpAcsBroker;
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf         = (PCCSP_CWMP_MSO_INTERFACE          )pCcspCwmpAcsBroker->hCcspCwmpMsoIf;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    ULONG                           uTimes             = 0;
    ULONG                           ulDelaySec         = pBITaskContext->ulDelay;
    ULONG                           ulTimeStart        = AnscGetTickInSeconds();
    ULONG                           ulTimeNow          = ulTimeStart;

    pMyObject->bBootInformScheduled = TRUE;

    /*
     * We intend to delay the very first Inform message for 10 seconds.
     */
    while( uTimes < 20)
    {
        AnscSleep(500);

        uTimes ++;

        if(!pMyObject->bActive)
        {
            AnscFreeMemory(pBITaskContext);
            pMyObject->bBootInformScheduled = FALSE;

            return ANSC_STATUS_SUCCESS;
        }
    }

    while ( pMyObject->bActive && !pMyObject->bDelayedInformCancelled )
    {
        ulTimeNow = AnscGetTickInSeconds();

        if (
               ( ulTimeNow >= ulTimeStart && (ulTimeNow  - ulTimeStart              >= ulDelaySec) ) ||
               ( ulTimeNow <  ulTimeStart && (0xFFFFFFFF - ulTimeStart + ulTimeNow) >= ulDelaySec  ) )
        {
            break;
        }

        AnscSleep(5000);
    }

    if ( pMyObject->bActive )
    {
        /* wait for BOOTSTRAP to be sent out first */
        while ( pMyObject->bBootstrapInformScheduled )
        {
            AnscSleep(1000);
        }

        returnStatus =
            pCcspCwmpMsoIf->Inform
                (
                    (ANSC_HANDLE)pCcspCwmpMsoIf->hOwnerContext,
                    CCSP_CWMP_INFORM_EVENT_NAME_Boot,
                    NULL,
                    TRUE
                );

        pCcspCwmpProcessor->ConfigPeriodicInform((ANSC_HANDLE)pCcspCwmpProcessor);
    }

    AnscFreeMemory(pBITaskContext);

    pMyObject->bBootInformScheduled = FALSE;

    return 	returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        ccspCwmpCpeBootstrapInformTask
            (
                ANSC_HANDLE                 hTaskContext
            );

    description:

        This function is called to send BOOTSTRAP event to ACS.

    argument:   ANSC_HANDLE                 hTaskContext
                Task context.

    return:     status of operation.


**********************************************************************/

typedef  struct _CCSP_CWMP_BOOT_INFORM_CTX               CCSP_CWMP_BOOTSTRAP_INFORM_CTX, *PCCSP_CWMP_BOOTSTRAP_INFORM_CTX;

ANSC_STATUS
ccspCwmpCpeBootstrapInformTask
    (
        ANSC_HANDLE                 hTaskContext
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_BOOTSTRAP_INFORM_CTX      pBITaskContext     = (PCCSP_CWMP_BOOTSTRAP_INFORM_CTX   )hTaskContext;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )pBITaskContext->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_ACS_BROKER_OBJECT         pCcspCwmpAcsBroker     = (PCCSP_CWMP_ACS_BROKER_OBJECT      )pMyObject->hCcspCwmpAcsBroker;
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf         = (PCCSP_CWMP_MSO_INTERFACE          )pCcspCwmpAcsBroker->hCcspCwmpMsoIf;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf         = (PCCSP_CWMP_CFG_INTERFACE          )pMyObject->GetCcspCwmpCfgIf((ANSC_HANDLE)pMyObject);
    ULONG                           ulDelaySec         = pBITaskContext->ulDelay;
    ULONG                           ulTimeStart        = AnscGetTickInSeconds();
    ULONG                           ulTimeNow          = ulTimeStart;
    BOOL                            bInitialContact    = FALSE;

    pMyObject->bBootstrapInformScheduled = TRUE;

    while ( pMyObject->bActive && !pMyObject->bDelayedInformCancelled && bInitialContact )
    {
        AnscSleep(5000);

        ulTimeNow = AnscGetTickInSeconds();

        if (
               ( ulTimeNow >= ulTimeStart && (ulTimeNow  - ulTimeStart              >= ulDelaySec) ) ||
               ( ulTimeNow <  ulTimeStart && (0xFFFFFFFF - ulTimeStart + ulTimeNow) >= ulDelaySec  ) )
        {
            break;
        }

        if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->IsBootstrap )
        {
            bInitialContact = pCcspCwmpCfgIf->IsBootstrap(pCcspCwmpCfgIf->hOwnerContext);
        }
        else
        {
            bInitialContact = pCcspCwmpProcessor->GetInitialContact((ANSC_HANDLE)pCcspCwmpProcessor);
        }
    }

    if ( pMyObject->bActive && bInitialContact )
    {
        returnStatus =
            pCcspCwmpMsoIf->Inform
                (
                    (ANSC_HANDLE)pCcspCwmpMsoIf->hOwnerContext,
                    CCSP_CWMP_INFORM_EVENT_NAME_Bootstrap,
                    NULL,
                    FALSE
                );
    }

    AnscFreeMemory(pBITaskContext);

    pMyObject->bBootstrapInformScheduled = FALSE;

    return 	returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoStartCWMP
            (
                ANSC_HANDLE                 hThisObject,
                BOOL                        bInformAcs,
                BOOL                        bRestart
            );

    description:

        This function is called to start running CWMP.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                BOOL                        bInformAcs
                Specifies if CPE is going to send Inform to
                ACS.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoStartCWMP
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bInformAcs,
        BOOL                        bRestart
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_ACS_BROKER_OBJECT         pCcspCwmpAcsBroker     = (PCCSP_CWMP_ACS_BROKER_OBJECT      )pMyObject->hCcspCwmpAcsBroker;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf         = (PCCSP_CWMP_MSO_INTERFACE          )pCcspCwmpAcsBroker->hCcspCwmpMsoIf;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf         = (PCCSP_CWMP_CFG_INTERFACE          )pMyObject->GetCcspCwmpCfgIf((ANSC_HANDLE)pMyObject);
    char*                           pAcsUrl            = (char*                        )NULL;
    BOOL                            bInitialContact    = (BOOL                         )FALSE;
    char*                           pTriggerCommand    = (char*                        )NULL;
    char*                           pTriggerCommandKey = (char*                        )NULL;
    char*                           pSavedEvents       = (char*                        )NULL;
    ULONG                           ulBootstrapDelay   = 0;
    ULONG                           ulBootDelay        = 0;
#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pCcspCwmpTcpcrHandler  = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT   )pMyObject->hCcspCwmpTcpConnReqHandler;
#endif
    PANSC_TIMER_DESCRIPTOR_OBJECT   pStartCwmpTimerObj = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hStartCwmpTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pStartCwmpTimerIf  = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hStartCwmpTimerIf;
    BOOL                            bCwmpEnabled       = FALSE;
    char                            psmKeyPrefixed[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN + 16];

    CcspTr069PaTraceInfo(("CcspCwmpCpecoStartCWMP() being called.\n"));

    bCwmpEnabled = pCcspCwmpProcessor->IsCwmpEnabled((ANSC_HANDLE)pCcspCwmpProcessor);

    if ( !bCwmpEnabled )
    {
        CcspTr069PaTraceWarning(("TR-069 PA is disabled!\n"));
        return  ANSC_STATUS_NOT_READY;
    }

    bInitialContact = pCcspCwmpProcessor->GetInitialContact((ANSC_HANDLE)pCcspCwmpProcessor);

    CcspCwmpPrefixPsmKey(psmKeyPrefixed, pMyObject->SubsysName, CCSP_TR069PA_PSM_KEY_TriggerCommand);
    pTriggerCommand = 
        pMyObject->LoadCfgFromPsm
            (
                (ANSC_HANDLE)pMyObject,
                psmKeyPrefixed
            );

    CcspCwmpPrefixPsmKey(psmKeyPrefixed, pMyObject->SubsysName, CCSP_TR069PA_PSM_KEY_TriggerCommandKey);
    pTriggerCommandKey = 
        pMyObject->LoadCfgFromPsm
            (
                (ANSC_HANDLE)pMyObject,
                psmKeyPrefixed
            );

    CcspCwmpPrefixPsmKey(psmKeyPrefixed, pMyObject->SubsysName, CCSP_TR069PA_PSM_KEY_SavedEvents);
    pSavedEvents = 
        pMyObject->LoadCfgFromPsm
            (
                (ANSC_HANDLE)pMyObject,
                psmKeyPrefixed
            );

    if ( pStartCwmpTimerObj )
    {
        pStartCwmpTimerObj->Stop((ANSC_HANDLE)pStartCwmpTimerObj);
    }

    if ( pMyObject->bCWMPStarted )
    {
        if ( pStartCwmpTimerObj )
        {
            pMyObject->hStartCwmpTimerObj = NULL;
            pStartCwmpTimerObj->Remove((ANSC_HANDLE)pStartCwmpTimerObj);
        }

        if ( pStartCwmpTimerIf )
        {
            AnscFreeMemory(pStartCwmpTimerIf);
            pMyObject->hStartCwmpTimerIf = NULL;
        }

        returnStatus = ANSC_STATUS_SUCCESS;
        goto EXIT;
    }

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->CanCwmpStart )
    {
        if ( !pCcspCwmpCfgIf->CanCwmpStart(pCcspCwmpCfgIf->hOwnerContext) )
        {
            CcspTr069PaTraceError(("CcspCwmpCpecoStartCWMP - cannot start CWMP CanCwmpStart() returns FALSE!\n"));

            if ( pStartCwmpTimerObj )
            {
                pStartCwmpTimerObj->Start((ANSC_HANDLE)pStartCwmpTimerObj);
            }

            returnStatus = ANSC_STATUS_NOT_READY;
            goto EXIT;
        }
    }

    if ( pStartCwmpTimerObj )
    {
        pMyObject->hStartCwmpTimerObj = NULL;
        pStartCwmpTimerObj->Remove((ANSC_HANDLE)pStartCwmpTimerObj);
    }

    if ( pStartCwmpTimerIf )
    {
        AnscFreeMemory(pStartCwmpTimerIf);
        pMyObject->hStartCwmpTimerIf = NULL;
    }

#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    if ( pCcspCwmpTcpcrHandler )
    {
        pCcspCwmpTcpcrHandler->Engage((ANSC_HANDLE)pCcspCwmpTcpcrHandler);
    }
#endif

    returnStatus = pCcspCwmpProcessor->Engage((ANSC_HANDLE)pCcspCwmpProcessor, bRestart);

if ( bInformAcs )
{

    pAcsUrl = pCcspCwmpProcessor->GetAcsUrl(pCcspCwmpProcessor);

    if ( pMyObject->bDelayedInformCancelled )
    {
        ulBootstrapDelay = ulBootDelay = 0;
    }
    else
    {
        if ( pCcspCwmpCfgIf )
        {
            if ( bInitialContact && pCcspCwmpCfgIf->GetBootstrapInformDelay )
            {
                ulBootstrapDelay = pCcspCwmpCfgIf->GetBootstrapInformDelay(pCcspCwmpCfgIf->hOwnerContext);
                ulBootDelay      = ulBootstrapDelay;
            }
            else if ( !bInitialContact && pCcspCwmpCfgIf->GetBootInformDelay )
            {
                ulBootDelay = pCcspCwmpCfgIf->GetBootInformDelay(pCcspCwmpCfgIf->hOwnerContext);
            }
        }
    }

    if ( bInitialContact )
    {
        CcspTr069PaTraceInfo
            ((
                "CcspCwmpCpecoStartCWMP - CPE reboots from fresh state, BOOTSTRAP Inform delays in %u seconds.\n", 
                (unsigned int)ulBootstrapDelay
            ));
    }
    else
    {
        CcspTr069PaTraceInfo
            ((
                "CcspCwmpCpecoStartCWMP - CPE reboots, BOOT Inform delays in %u seconds.\n", 
                (unsigned int)ulBootDelay
            ));
    }

    if ( !pAcsUrl )
    {
        CcspTr069PaTraceError
            ((
                "CcspCwmpCpecoStartCWMP - ACS URL is not configured, or cannot be read from PSM!\n"
            ));

        returnStatus = ANSC_STATUS_SUCCESS;
        goto EXIT;
    }
    else if ( bInitialContact )
    {
        if ( ulBootstrapDelay == 0 )
        {
            returnStatus =
                pCcspCwmpMsoIf->Inform
                    (
                        (ANSC_HANDLE)pCcspCwmpMsoIf->hOwnerContext,
                        CCSP_CWMP_INFORM_EVENT_NAME_Bootstrap,
                        NULL,
                        FALSE
                    );
        }
        else
        {
            PCCSP_CWMP_BOOTSTRAP_INFORM_CTX  pBITaskContext;

            pBITaskContext = (PCCSP_CWMP_BOOTSTRAP_INFORM_CTX)AnscAllocateMemory(sizeof(CCSP_CWMP_BOOTSTRAP_INFORM_CTX));

            if ( !pBITaskContext )
            {
                returnStatus = ANSC_STATUS_RESOURCES;
                goto EXIT;
            }

            pBITaskContext->hCcspCwmpCpeController  = pMyObject;
            pBITaskContext->ulDelay             = ulBootstrapDelay;

            AnscSpawnTask3
                (
                    (void*)ccspCwmpCpeBootstrapInformTask,
                    (ANSC_HANDLE)pBITaskContext,
                    "CcspCwmpCpeBootstrapInformTask",
                    USER_DEFAULT_TASK_PRIORITY,
                    2*USER_DEFAULT_TASK_STACK_SIZE
                 );
        }

        /*
         * Bin added here:
         *
         * According to WT151, all other undelivered events MUST be discarded on BOOTSTRAP.
         *
         */
    }
    else
    {
        /*
         * After reboot, we must initiate an Inform call the ACS and include the event "M Reboot" in
         * the event list to indicate that the previous Reboot() method was the trigger. For such
         * information to survive reboot, we need to remember the method and corresonding CommandKey
         * in the "invisible" parameters:
         *
         *      "(InternetGateway)Device.ManagementServer.TriggerCommand" and
         *      "(InternetGateway)Device.ManagementServer.TriggerCommandKey"
         *
         * The CcspCwmpAcsBroker will initiate the Inform based on these two parameter values during
         * initialization. These two values will be cleared after the Inform call is made.
         */

        if ( pTriggerCommand && (AnscSizeOfString(pTriggerCommand) > 0) )
        {
            returnStatus =
                pCcspCwmpMsoIf->Inform
                    (
                        (ANSC_HANDLE)pCcspCwmpMsoIf->hOwnerContext,
                        pTriggerCommand,
                        pTriggerCommandKey,
                        FALSE
                    );

            if ( TRUE )
            {
                CcspCwmpPrefixPsmKey(psmKeyPrefixed, pMyObject->SubsysName, CCSP_TR069PA_PSM_KEY_TriggerCommand);
                returnStatus = 
                    pMyObject->SaveCfgToPsm
                        (
                            (ANSC_HANDLE)pMyObject,
                            psmKeyPrefixed,
                            NULL
                        );

                CcspCwmpPrefixPsmKey(psmKeyPrefixed, pMyObject->SubsysName, CCSP_TR069PA_PSM_KEY_TriggerCommandKey);
                returnStatus = 
                    pMyObject->SaveCfgToPsm
                        (
                            (ANSC_HANDLE)pMyObject,
                            psmKeyPrefixed,
                            NULL
                        );
            }
        }

        /*
         * After reboot, we must initiate an Inform call the ACS and include the other undelivered events
         * before reboot.
         * They are saved in the "invisible" parameters:
         *
         *      "(InternetGateway)Device.ManagementServer.SavedEvents"
         *
         * In format "event+key,event+key,...".
         *
         * The CcspCwmpAcsBroker will initiate the Inform based on tthe parameter value during
         * initialization. The value will be cleared after the Inform call is made.
         */

        if ( TRUE )
        {
			/*
            char*                           pRootObjName       = pMyObject->GetRootObject((ANSC_HANDLE)pMyObject);
            BOOL                            bRootDevice        = AnscEqualString(pRootObjName, DM_ROOTNAME, FALSE);
            char*                       pParamNameSE = bRootDevice ? CCSP_CWMP_CFG_HPN_Device_SavedEvents : CCSP_CWMP_CFG_HPN_IGD_SavedEvents;

            if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetHiddenCwmpParamName )
            {
                pParamNameSE = pCcspCwmpCfgIf->GetHiddenCwmpParamName(pCcspCwmpCfgIf->hOwnerContext, CCSP_CWMP_CFG_HPID_SavedEvents);
            }
			*/

            if ( pSavedEvents && (AnscSizeOfString(pSavedEvents) > 0) )
            {
                CcspCwmpCpecoLoadEvents
                    (
                        (ANSC_HANDLE)pCcspCwmpMsoIf,
                        pSavedEvents
                    );

                CcspCwmpPrefixPsmKey(psmKeyPrefixed, pMyObject->SubsysName, CCSP_TR069PA_PSM_KEY_SavedEvents);
                returnStatus = 
                    pMyObject->SaveCfgToPsm
                        (
                            (ANSC_HANDLE)pMyObject,
                            psmKeyPrefixed,
                            NULL
                        );
            }

            if (pSavedEvents) {
                CcspTr069PaTraceDebug(("CcspCwmpCpecoStartCWMP - saved events: %s\n", pSavedEvents));
                AnscFreeMemory(pSavedEvents);
                pSavedEvents = NULL;
            }

        }
    }

    /*
     * Whenever CPE reboots, a "1 BOOT" event should be sent to the ACS as part of the Inform
     * request. This includes initial system reboot, as well as reboot dueo to any cause, including
     * use of the Reboot method.
     */
    if ( TRUE )
    {
        PCCSP_CWMP_BOOT_INFORM_CTX       pBITaskContext;

        pBITaskContext = (PCCSP_CWMP_BOOT_INFORM_CTX)AnscAllocateMemory(sizeof(CCSP_CWMP_BOOT_INFORM_CTX));

        if ( !pBITaskContext )
        {
            returnStatus = ANSC_STATUS_RESOURCES;
            goto EXIT;
        }

        pBITaskContext->hCcspCwmpCpeController  = pMyObject;
        pBITaskContext->ulDelay             = ulBootDelay;

        AnscSpawnTask3
            (
                (void*)ccspCwmpCpeFirstInformTask,
                (ANSC_HANDLE)pBITaskContext,
                "CcspCwmpCpeFirstInformTask",
                USER_DEFAULT_TASK_PRIORITY,
                2*USER_DEFAULT_TASK_STACK_SIZE

             );
    }
}   /* if ( bInformAcs ) */

    returnStatus = ANSC_STATUS_SUCCESS;
    if(checkIfSystemReady())
    {
       pMyObject->bCWMPStarted = TRUE;
    }
    else
    {
	CcspTr069PaTraceInfo(("Checked CR - System is not ready, CWMP is not started\n"));
    }

EXIT:
    if (pAcsUrl)
    {
        AnscFreeMemory(pAcsUrl);
    }
    if ( pTriggerCommand )
    {
        AnscFreeMemory(pTriggerCommand);
    }

    if ( pTriggerCommandKey )
    {
        AnscFreeMemory(pTriggerCommandKey);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoStopCWMP
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to stop running CWMP, the data model
        is not going to be unloaded.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoStopCWMP
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pCcspCwmpTcpcrHandler  = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT   )pMyObject->hCcspCwmpTcpConnReqHandler;
#endif

    if ( !pMyObject->bCWMPStarted )
    {
        return  ANSC_STATUS_SUCCESS;
    }

    CcspTr069PaTraceInfo(("CcspCwmpCpecoStopCWMP - \n"));

    if ( pCcspCwmpProcessor )
    {
        pCcspCwmpProcessor->Cancel((ANSC_HANDLE)pCcspCwmpProcessor);
        pCcspCwmpProcessor->Reset ((ANSC_HANDLE)pCcspCwmpProcessor);
    }

#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    if ( pCcspCwmpTcpcrHandler )
    {
        pCcspCwmpTcpcrHandler->Cancel((ANSC_HANDLE)pCcspCwmpTcpcrHandler);
    }
#endif

    pMyObject->bCWMPStarted             = FALSE;
    pMyObject->bDelayedInformCancelled  = FALSE;

    return  returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoRestartCWMP
            (
                ANSC_HANDLE                 hThisObject,
                BOOL                        bInformAcs
            );

    description:

        This function is called to restart CWMP(start from bootstrap),
        the data model is not going to be unloaded.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoRestartCWMP
    (
        ANSC_HANDLE                         hThisObject,
        BOOL                                bInformAcs
    )
{
    ANSC_STATUS                             returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT        pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_STAT_INTERFACE               pCcspCwmpStatIf    = (PCCSP_CWMP_STAT_INTERFACE       )pMyObject->hCcspCwmpStaIf;
    PCCSP_CWMP_PROCESSOR_OBJECT             pCcspCwmpProcessor = (PCCSP_CWMP_PROCESSOR_OBJECT       )pMyObject->hCcspCwmpProcessor;

    CcspTraceWarning(("CcspCwmpCpecoRestartCWMP - bInformAcs %d!\n", bInformAcs));

    pMyObject->StopCWMP((ANSC_HANDLE)pMyObject);
    pCcspCwmpProcessor->SetInitialContact((ANSC_HANDLE)pCcspCwmpProcessor, TRUE);
    pCcspCwmpProcessor->SetInitialContactFactory((ANSC_HANDLE)pCcspCwmpProcessor, TRUE);
    returnStatus = CcspManagementServer_RestoreDefaultValues();
    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTraceWarning(("!!! CcspManagementServer_RestoreDefaultValues Error !!!\n"));
    }

    if ( pCcspCwmpStatIf )
    {
        pCcspCwmpStatIf->ResetStats(pCcspCwmpStatIf->hOwnerContext);
    }
    pMyObject->bBootInformSent = FALSE;
    pMyObject->bRestartCWMP    = TRUE;

    returnStatus =
        pMyObject->StartCWMP
            (
                (ANSC_HANDLE)pMyObject,
                bInformAcs, /* Should be Inform now */
                TRUE  /* It's a restart */
            );

    return returnStatus;
}

/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoInformNow
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to force inform to happen right
        away.

    argument:
                ANSC_HANDLE                 hThisObject,
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoInformNow
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pMyObject         = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT     pCcspCwmpProcessor = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SESSION_OBJECT       pCcspCwmpSession   = (PCCSP_CWMP_SESSION_OBJECT   )NULL;
    BOOL                            bInformScheduled   =  pMyObject->bBootInformScheduled || pMyObject->bBootstrapInformScheduled;

    /* set the flag to terminate any delayed BOOTSTRAP or BOOT inform */
    pMyObject->bDelayedInformCancelled  = TRUE;

    if ( pMyObject->bCWMPStarted )
    {
        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession((ANSC_HANDLE)pCcspCwmpProcessor);

        if ( pCcspCwmpSession )
        {
            returnStatus = pCcspCwmpSession->CancelRetryDelay((ANSC_HANDLE)pCcspCwmpSession, !bInformScheduled);
            pCcspCwmpSession->RelAccess((ANSC_HANDLE)pCcspCwmpSession);
        }
    }
    else
    {
        returnStatus = ANSC_STATUS_NOT_READY;
    }

    return  returnStatus;
}


static ANSC_STATUS
CcspCwmpCpecoCancelRetryDelayTask
    (
        ANSC_HANDLE                 hContext                    
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )hContext;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession    = (PCCSP_CWMP_SESSION_OBJECT   )NULL;

    /* reset active session retry and send inform right away */
    pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession((ANSC_HANDLE)pCcspCwmpProcessor);

    /* we don't send out unsolicited Inform if there is no waiting session */
    if ( pCcspCwmpSession )
    {
        returnStatus = pCcspCwmpSession->CancelRetryDelay((ANSC_HANDLE)pCcspCwmpSession, TRUE);
        pCcspCwmpSession->RelAccess((ANSC_HANDLE)pCcspCwmpSession);
    }

    return  returnStatus;
}


static ANSC_STATUS
CcspCwmpCpecoInformNowTask
    (
        ANSC_HANDLE                 hContext                    
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hContext;

    pCcspCwmpCpeController->InformNow((ANSC_HANDLE)pCcspCwmpCpeController);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    prototype:

        void
        CcspCwmpCpecoNotifyEvent
            (
                ANSC_HANDLE                 hThisObject,
                ULONG                       ulEvent,
                ANSC_HANDLE                 hEventContext
            );

    description:

        This function is called to notify CPE Controller the specified
        event has happened.

    argument:
                ANSC_HANDLE                 hThisObject,
                This handle is actually the pointer of this object
                itself.

                ULONG                       ulEvent
                Event code.

                ANSC_HANDLE                 hEventContext
                Event context.

    return:     status of operation.

**********************************************************************/

void
CcspCwmpCpecoNotifyEvent
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulEvent,
        ANSC_HANDLE                 hEventContext
    )
{
    UNREFERENCED_PARAMETER(hEventContext);
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;

    CcspTr069PaTraceDebug(("CcspCwmpCpecoNotifyEvent - event %u.\n", (unsigned int)ulEvent));

    switch ( ulEvent )
    {
        case    CCSP_CWMP_CPECO_EVENT_InterfaceUp:

                if ( ( pMyObject->bBootInformScheduled || pMyObject->bBootstrapInformScheduled ) && 
                     !pMyObject->bDelayedInformCancelled )
                {
                    /* BOOT or BOOTSTRAP inform has been scheduled, so we don't need to do anything now */
                }
                else
                {
                    /* we are most likely in context of IPC call and acquiring current 
                     * session may be blocked if there's an active ACS session 
                     */

                    AnscSpawnTask3
                        (
                            (void*)CcspCwmpCpecoCancelRetryDelayTask,
                            (ANSC_HANDLE)pCcspCwmpProcessor,
                            "CcspCwmpCpecoCancelRetryDelayTask",
                            USER_DEFAULT_TASK_PRIORITY,
                            USER_DEFAULT_TASK_STACK_SIZE
                         );
                }

                break;

        case    CCSP_CWMP_CPECO_EVENT_InterfaceIpAddrChanged:

                /* we need to notify ACS server since connection request URL is very likely changed, 
                 * and ACS server usually wants to catch this 
                 */
                if ( pMyObject->bBootInformScheduled && !pMyObject->bBootstrapInformScheduled && 
                     !pMyObject->bDelayedInformCancelled )
                {
                    /* cancel delay and inform now */
                    AnscSpawnTask3
                        (
                            (void*)CcspCwmpCpecoInformNowTask,
                            (ANSC_HANDLE)pMyObject,
                            "CcspCwmpCpecoInformNowTask",
                            USER_DEFAULT_TASK_PRIORITY,
                            USER_DEFAULT_TASK_STACK_SIZE
                         );
                }

                break;

        case    CCSP_CWMP_CPECO_EVENT_InterfaceDown:

                /* 
                 * nothing needs to be done - minus side is CWMP state machine is still running.
                 * Plus side is if we don't get notified once interface is up, communication between
                 * CPE and ACS still can be recovered.
                 */

                break;

        default:

                /* do nothing */

                break;
    }
}


/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoAcqCrSessionID
            (
                ANSC_HANDLE                 hThisObject,
                int                         Priority
            );

    description:

        This function is called to acquire CR session ID.

    argument:
                ANSC_HANDLE                 hThisObject,
                This handle is actually the pointer of this object
                itself.

                int                         Priority
                Indicate session priority.

    return:     status of operation.

**********************************************************************/

ULONG
CcspCwmpCpecoAcqCrSessionID
    (
        ANSC_HANDLE                 hThisObject,
        int                         Priority
    )
{
    int                             sid                = 0;

#ifdef   _CCSP_TR069PA_USE_CR_SESSION_CONTROL
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;
    int                             nRet;
    nRet = 
        CcspBaseIf_requestSessionID
            (
                pMyObject->hMsgBusHandle,
                pMyObject->CRNameWithPrefix,
                Priority,
                &sid
            );

    nRet = CcspTr069PA_MapCcspErrCode(pMyObject->hTr069PaMapper, nRet);

    if ( nRet != CCSP_CWMP_CPE_CWMP_FaultCode_noError )
    {
        CcspTr069PaTraceError(("TR-069 PA failed to acquire session ID from CR, error code = %d.\n", nRet));
        sid = 0;
    }
    else
    {
        CcspTr069PaTraceDebug(("TR-069 PA has acquired session ID %u from CR.\n", (ULONG)sid));
    }
#else
    UNREFERENCED_PARAMETER(hThisObject);
    UNREFERENCED_PARAMETER(Priority);
#endif

    return  (ULONG)sid;
}


/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoRelCrSessionID
            (
                ANSC_HANDLE                 hThisObject,
                ULONG                       CrSessionID
            );

    description:

        This function is called to inform CR PA session has
        ended.

    argument:
                ANSC_HANDLE                 hThisObject,
                This handle is actually the pointer of this object
                itself.

                ULONG                       CrSessionID
                CR Session ID.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoRelCrSessionID
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       CrSessionID
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;

#ifdef   _CCSP_TR069PA_USE_CR_SESSION_CONTROL
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;
    int                             nRet;
    nRet = 
        CcspBaseIf_informEndOfSession 
            (
                pMyObject->hMsgBusHandle,
                pMyObject->CRNameWithPrefix,
                CrSessionID
            );

    nRet = CcspTr069PA_MapCcspErrCode(pMyObject->hTr069PaMapper, nRet);

    if ( nRet != CCSP_CWMP_CPE_CWMP_FaultCode_noError )
    {
        CcspTr069PaTraceError(("TR-069 PA failed to release session ID %u acquired from CR.\n", CrSessionID));
        returnStatus = ANSC_STATUS_FAILURE;
    }
    else
    {
        CcspTr069PaTraceDebug(("TR-069 PA has released session ID %u acquired from CR.\n", CrSessionID));
    }
#else
    UNREFERENCED_PARAMETER(hThisObject);
    UNREFERENCED_PARAMETER(CrSessionID);
#endif

    return  returnStatus;
}


