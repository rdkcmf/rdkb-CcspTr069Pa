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

    module:	ccsp_cwmp_wmppo_acs_access.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced configuration functions
        of the CCSP CWMP Wmp Processor Object.

        *   CcspCwmppoGetAcsInfo

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        10/10/05    initial revision.
        06/21/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/13/11    remove name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_proco_global.h"


static
void 
mgsrv_InformNowTask
    (
        void *                      context
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)context;
    PCCSP_CWMP_ACS_BROKER_OBJECT         pCcspCwmpAcsBroker     = (PCCSP_CWMP_ACS_BROKER_OBJECT    )pCcspCwmpCpeController->hCcspCwmpAcsBroker;
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf         = (PCCSP_CWMP_MSO_INTERFACE        )pCcspCwmpAcsBroker->hCcspCwmpMsoIf;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pCcspCwmpCpeController->hCcspCwmpProcessor;
    BOOL                            bCwmpInSession     = TRUE;

    AnscTrace("ACS URL has changed, wait for current session is over to initiate a new 'Inform'...\n");

    while ( bCwmpInSession )
    {
        if ( pCcspCwmpProcessor->IsCwmpEnabled((ANSC_HANDLE)pCcspCwmpProcessor) )
        {
            if ( pCcspCwmpProcessor->GetActiveWmpSessionCount((ANSC_HANDLE)pCcspCwmpProcessor, FALSE) != 0 )
            {
                bCwmpInSession = TRUE;
            }
            else
            {
                bCwmpInSession = FALSE;
            }
        }
        else
        {
            break;
        }
    }

    pCcspCwmpMsoIf->Inform
        (
            (ANSC_HANDLE)pCcspCwmpMsoIf->hOwnerContext,
            CCSP_CWMP_INFORM_EVENT_NAME_Bootstrap,
            NULL,
            TRUE
        );
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoGetAcsInfo
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve CcspCwmp policy parameters.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoGetAcsInfo
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject              = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_PROPERTY    pProperty              = (PCCSP_CWMP_PROCESSOR_PROPERTY)&pMyObject->Property;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT pCcspCwmpTcpcrHandler   = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT  )pCcspCwmpCpeController->hCcspCwmpTcpConnReqHandler;
    CCSP_CWMP_TCPCR_HANDLER_PROPERTY     tcpCrProperty;
    CCSP_STRING                     tcpCrHostPort           = NULL;
    USHORT                          usTcpCrHostPort         = 0;
#endif
    char*                           pPeriodicInformTime     = NULL;
    BOOL                            bPiEnabled              = pProperty->bPeriodicInformEnabled;
    ULONG                           ulOldInterval           = pProperty->PeriodicInformInterval;
    BOOL                            bStartCWMP              = FALSE;
    BOOL                            bStopCWMP               = FALSE;
    BOOL                            bAcsUrlChanged          = FALSE;
    PANSC_UNIVERSAL_TIME            pCalendarTime;
    char*                           pValue;
    BOOL                            bCwmpStarted            = pCcspCwmpCpeController->bCWMPStarted;
    BOOL                            bCwmpEnabled            = FALSE;
    BOOL                            bPiChanged              = FALSE;
#ifdef   _CCSP_CWMP_STUN_ENABLED
    PCCSP_CWMP_STUN_MANAGER_OBJECT  pCcspCwmpStunManager    = pCcspCwmpCpeController->hCcspCwmpStunManager;
    PCCSP_CWMP_STUN_INFO            pCcspCwmpStunInfo       = NULL;
#endif

    CcspTr069PaTraceDebug(("GetAcsInfo-bCWMPstarted=%s\n", bCwmpStarted?"Y":"N"));

#ifdef   _CCSP_CWMP_STUN_ENABLED
    if ( pCcspCwmpStunManager )
    {
        BOOL                        bStunSettingsChanged = FALSE;
        PSTUN_SIMPLE_CLIENT_OBJECT  pStunSimpleClient    = (PSTUN_SIMPLE_CLIENT_OBJECT )pCcspCwmpStunManager->hStunSimpleClient;
        ULONG                       ulServerAddr         = pStunSimpleClient->GetServerAddr((ANSC_HANDLE)pStunSimpleClient);
        USHORT                      ulServerPort         = pStunSimpleClient->GetServerPort((ANSC_HANDLE)pStunSimpleClient);
        char*                       pUsername            = pStunSimpleClient->GetUsername((ANSC_HANDLE)pStunSimpleClient);
        char*                       pPassword            = pStunSimpleClient->GetPassword((ANSC_HANDLE)pStunSimpleClient);
        int                         maxKeepAlive         = pStunSimpleClient->GetMaxKeepAliveInterval((ANSC_HANDLE)pStunSimpleClient);
        ULONG                       minKeepAlive         = pStunSimpleClient->GetMinKeepAliveInterval((ANSC_HANDLE)pStunSimpleClient);

        pCcspCwmpStunInfo = &pCcspCwmpStunManager->CcspCwmpStunInfo;

        pCcspCwmpStunManager->RegGetStunInfo
            (
                (ANSC_HANDLE)pCcspCwmpStunManager
            );

        if ( pCcspCwmpStunInfo->STUNServerPort != ulServerPort )
        {
            bStunSettingsChanged = TRUE;
        }
        else if ( pCcspCwmpStunInfo->STUNMaximumKeepAlivePeriod != maxKeepAlive )
        {
            bStunSettingsChanged = TRUE;
        }
        else if ( pCcspCwmpStunInfo->STUNMinimumKeepAlivePeriod != minKeepAlive )
        {
            bStunSettingsChanged = TRUE;
        }
        else if ( !AnscEqualString(pCcspCwmpStunInfo->STUNUsername, pUsername, TRUE) )
        {
            bStunSettingsChanged = TRUE;
        }
        else if ( !AnscEqualString(pCcspCwmpStunInfo->STUNPassword, pPassword, TRUE) )
        {
            bStunSettingsChanged = TRUE;
        }
        else
        {
             ANSC_IPV4_ADDRESS      server_ip4_addr;

			server_ip4_addr.Value = 0;

            if ( pCcspCwmpStunInfo->STUNServerAddress )
            {
                AnscResolveHostName(pCcspCwmpStunInfo->STUNServerAddress, server_ip4_addr.Dot);
            }

            if ( server_ip4_addr.Value != ulServerAddr )
            {
                bStunSettingsChanged = TRUE;
            }
        }

        /* compare if any STUN settings have been changed */
        if ( bStunSettingsChanged )
        {
            pCcspCwmpStunManager->ApplyStunSettings((ANSC_HANDLE)pCcspCwmpStunManager);
        }
    }
#endif

#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
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

            pCcspCwmpTcpcrHandler->Cancel((ANSC_HANDLE)pCcspCwmpTcpcrHandler);

            pCcspCwmpTcpcrHandler->SetProperty((ANSC_HANDLE)pCcspCwmpTcpcrHandler, (ANSC_HANDLE)&tcpCrProperty);
            pCcspCwmpTcpcrHandler->Engage((ANSC_HANDLE)pCcspCwmpTcpcrHandler);
        }
    }
#endif

/*
    char                            psmKeyPrefixed[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN + 16];
    CcspCwmpPrefixPsmKey(psmKeyPrefixed, pCcspCwmpCpeController->SubsysName, CCSP_TR069PA_PSM_KEY_InitialContact);
    pValue =
        pCcspCwmpCpeController->LoadCfgFromPsm
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                psmKeyPrefixed
            );

    pProperty->bInitialContact = pValue ? _ansc_atoi(pValue) : TRUE;

    if ( pValue )
    {
        AnscFreeMemory(pValue);
    }
*/

    /* 
     * sync with ManagementServer functional component
     */
    bCwmpEnabled = 
        CcspManagementServer_GetEnableCWMP
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    pProperty->bPeriodicInformEnabled = 
        CcspManagementServer_GetPeriodicInformEnable
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    pProperty->PeriodicInformInterval = 
        CcspManagementServer_GetPeriodicInformInterval
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    pPeriodicInformTime = 
        CcspManagementServer_GetPeriodicInformTimeStr
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    if ( pPeriodicInformTime && AnscSizeOfString(pPeriodicInformTime) != 0 )
    {
        pCalendarTime = CcspStringToCalendarTime(pPeriodicInformTime);

        if ( pCalendarTime )
        {
            pProperty->PeriodicInformTime = *pCalendarTime;
            AnscFreeMemory(pCalendarTime);
        }

        bPiChanged = TRUE;
    }
    else
    {
        AnscZeroMemory(&pProperty->PeriodicInformTime, sizeof(ANSC_UNIVERSAL_TIME));
    }

    if ( pPeriodicInformTime )
    {
        AnscFreeMemory(pPeriodicInformTime);
    }

    /* Configure Period Inform */
    if ( (bPiEnabled && ! pProperty->bPeriodicInformEnabled) ||
         (!bPiEnabled && pProperty->bPeriodicInformEnabled ) ||
         (ulOldInterval != pProperty->PeriodicInformInterval) )
    {
        bPiChanged = TRUE;
    }

	/* DefaultActiveNotificationThrottle */
	pProperty->DefActiveNotifThrottle = 
		CcspManagementServer_GetDefaultActiveNotificationThrottle
			(
				pCcspCwmpCpeController->PANameWithPrefix
			);

    /* ACS URL */
    pValue = CcspManagementServer_GetURL(pCcspCwmpCpeController->PANameWithPrefix);

    if ( pValue )
    {
        size_t len = strlen (pValue);

        /*
           If the new URL is too long or the same as the current URL then ignore it.
        */
        if ((len < sizeof(pProperty->AcsUrl)) && (memcmp (pValue, pProperty->AcsUrl, len + 1) != 0))
        {
            memcpy (pProperty->AcsUrl, pValue, len + 1);
            pMyObject->SetInitialContact((ANSC_HANDLE)pMyObject, TRUE);
            bAcsUrlChanged = TRUE;
        }

        AnscFreeMemory(pValue);
    }

    if ( !bCwmpStarted && bCwmpEnabled )
    {
        bStartCWMP = TRUE;
    }
    else if ( bCwmpStarted && !bCwmpEnabled )
    {
        bStopCWMP = TRUE;
    }

    CcspTr069PaTraceDebug(("CcspCwmppoGetAcsInfo-bStartCWMP=%s\n", bStartCWMP?"Y":"N"));
    CcspTr069PaTraceDebug(("CcspCwmppoGetAcsInfo-bStopCWMP=%s\n", bStopCWMP?"Y":"N"));
    CcspTr069PaTraceDebug(("CcspCwmppoGetAcsInfo-bPiChanged=%s\n", bPiChanged?"Y":"N"));

    if ( bStartCWMP )
    {
        pCcspCwmpCpeController->StartCWMP((ANSC_HANDLE)pCcspCwmpCpeController, !pCcspCwmpCpeController->bBootInformSent, pCcspCwmpCpeController->bRestartCWMP);
    }

    if ( !bStopCWMP && bPiChanged && pCcspCwmpCpeController->bCWMPStarted )
    {
        pMyObject->ConfigPeriodicInform((ANSC_HANDLE)pMyObject);
    }

    if ( bAcsUrlChanged && !bStopCWMP && bCwmpStarted )
    {
        /* we need to spawn a task and wait for the end of current session  */
        AnscSpawnTask(mgsrv_InformNowTask, (ANSC_HANDLE)pCcspCwmpCpeController, "InformNow");
    }

    returnStatus = ANSC_STATUS_SUCCESS;

    pMyObject->bAcsInfoRetrieved = TRUE;

    if ( bStopCWMP )
    {
        if ( pMyObject->GetActiveWmpSessionCount((ANSC_HANDLE)pMyObject, TRUE) == 0 )
        {
            /* not triggered by ACS */
            pCcspCwmpCpeController->StopCWMP((ANSC_HANDLE)pCcspCwmpCpeController);
        }
        else
        {
            pCcspCwmpCpeController->bCWMPStopping = TRUE;
        }
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        void
        CcspCwmppoMsValueChanged
            (
                ANSC_HANDLE                 hThisObject,
                ULONG                       hMsvcObjectID
            );

    description:

        This function is called by Management Server FC when
        the value of a parameter has changed.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ULONG                       hMsvcObjectID
                Indicate the object ID that at least one parameter 
                underneath has changed.

    return:     status of operation.

**********************************************************************/

void
CcspCwmppoMsValueChanged
    (
        ANSC_HANDLE                 hThisObject,
        int                         ulMsvcObjectID
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_ACS_BROKER_OBJECT    pCcspCwmpAcsBroker  = (PCCSP_CWMP_ACS_BROKER_OBJECT)pCcspCwmpCpeController->hCcspCwmpAcsBroker;
    PCCSP_CWMP_MSO_INTERFACE        pCcspCwmpMsoIf      = (PCCSP_CWMP_MSO_INTERFACE    )pCcspCwmpAcsBroker->hCcspCwmpMsoIf;

    if ( ulMsvcObjectID & CCSP_MSVC_OBJECT_ManagementServer )
    {
        CcspTr069PaTraceDebug(("### ManagementServer configuration changed! ###\n"));
        pMyObject->GetAcsInfo((ANSC_HANDLE)pMyObject);
    }
    
    if ( ulMsvcObjectID & CCSP_MSVC_OBJECT_DiagComplete )
    {
        /* PA has received diagnostics complete signal over D-Bus,
         * need to include it in CWMP session to notify ACS
         */
        pCcspCwmpMsoIf->Inform
            (
                (ANSC_HANDLE)pCcspCwmpMsoIf->hOwnerContext,
                CCSP_CWMP_INFORM_EVENT_NAME_DiagnosticsComplete,
                NULL,
                TRUE
            );
    }

    /* we do not need to sync with other objects such as 
     * autonomous transfer complete policy and autonomous
     * DUStateChange complete policy, we can retrieve those
     * always at run-time.
     */
}

