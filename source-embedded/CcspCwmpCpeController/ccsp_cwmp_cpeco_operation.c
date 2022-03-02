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

    module: ccsp_cwmp_cpeco_operation.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced operation functions
        of the CCSP CWMP Cpe Controller Object.

        *   CcspCwmpCpecoEngage
        *   CcspCwmpCpecoCancel
        *   CcspCwmpCpecoSetupEnv
        *   CcspCwmpCpecoCloseEnv

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/09/05    initial revision.
        06/20/11    updated by Kang Quan to migrate TR-069 PA
                    to CCSP architecture.
        10/13/11    remove name conflicts with DM library.

**********************************************************************/

#include "ccsp_cwmp_cpeco_global.h"
#include "ccsp_tr069pa_mapper_api.h"

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoEngage
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
CcspCwmpCpecoEngage
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus          = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     
                                    pMyObject             = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_PROPERTY   
                                    pProperty             = (PCCSP_CWMP_CPE_CONTROLLER_PROPERTY)&pMyObject->Property;
    PCCSP_CWMP_ACS_BROKER_OBJECT    pCcspCwmpAcsBroker    = (PCCSP_CWMP_ACS_BROKER_OBJECT      )pMyObject->hCcspCwmpAcsBroker;
    PCCSP_CWMP_PROCESSOR_OBJECT     pCcspCwmpProcessor    = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT pCcspCwmpTcpcrHandler = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT   )pMyObject->hCcspCwmpTcpConnReqHandler;
#endif
#ifdef   _CCSP_CWMP_STUN_ENABLED
    PCCSP_CWMP_STUN_MANAGER_OBJECT  pCcspCwmpStunManager  = (PCCSP_CWMP_STUN_MANAGER_OBJECT)pMyObject->hCcspCwmpStunManager;
#endif

    CcspTr069PaTraceDebug(("CcspCwmpCpecoEngage - bActive %d!\n", pMyObject->bActive));
    if ( pMyObject->bActive )
    {
        return  ANSC_STATUS_SUCCESS;
    }

    returnStatus = pMyObject->SetupEnv((ANSC_HANDLE)pMyObject);

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        return  returnStatus;
    }

    if ( pCcspCwmpAcsBroker )
    {
        pCcspCwmpAcsBroker->SetCcspCwmpCpeController((ANSC_HANDLE)pCcspCwmpAcsBroker, (ANSC_HANDLE)pMyObject );
        pCcspCwmpAcsBroker->Engage              ((ANSC_HANDLE)pCcspCwmpAcsBroker);
    }

    if ( pCcspCwmpProcessor )
    {
        pCcspCwmpProcessor->SetCcspCwmpCpeController((ANSC_HANDLE)pCcspCwmpProcessor, (ANSC_HANDLE)pMyObject);
        pCcspCwmpProcessor->Engage              ((ANSC_HANDLE)pCcspCwmpProcessor, FALSE);
    }
    
#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    pCcspCwmpTcpcrHandler->Engage((ANSC_HANDLE)pCcspCwmpTcpcrHandler);
#endif

#ifdef   _CCSP_CWMP_STUN_ENABLED
    if ( pCcspCwmpStunManager )
    {
        pCcspCwmpStunManager->Engage((ANSC_HANDLE)pCcspCwmpStunManager);
    }
#endif

    pMyObject->bActive = TRUE;

    if ( pProperty->bStartCwmpRightAway )
    {
        CcspTr069PaTraceDebug(("CcspCwmpCpecoEngage - start CWMP right away!\n"));
        returnStatus = pMyObject->StartCWMP((ANSC_HANDLE)pMyObject, TRUE, FALSE);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoCancel
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
CcspCwmpCpecoCancel
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus          = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     
                                    pMyObject             = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_ACS_BROKER_OBJECT    pCcspCwmpAcsBroker    = (PCCSP_CWMP_ACS_BROKER_OBJECT      )pMyObject->hCcspCwmpAcsBroker;
    PCCSP_CWMP_PROCESSOR_OBJECT     pCcspCwmpProcessor    = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT pCcspCwmpTcpcrHandler = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT   )pMyObject->hCcspCwmpTcpConnReqHandler;
#endif
#ifdef   _CCSP_CWMP_STUN_ENABLED
    PCCSP_CWMP_STUN_MANAGER_OBJECT  pCcspCwmpStunManager  = (PCCSP_CWMP_STUN_MANAGER_OBJECT)pMyObject->hCcspCwmpStunManager;
#endif
    ULONG                           ulTimeStart           = AnscGetTickInSeconds();

    if ( !pMyObject->bActive )
    {
        return  ANSC_STATUS_SUCCESS;
    }
    else
    {
        pMyObject->bActive = FALSE;
    }
    
    returnStatus = pMyObject->CloseEnv((ANSC_HANDLE)pMyObject);

    while ( pMyObject->bBootstrapInformScheduled || pMyObject->bBootInformScheduled )
    {
        AnscSleep(500);

        if ( AnscGetTickInSeconds() - ulTimeStart > 10 )
        {
            /* cancel it anyway */
            break;
        }
    }

    if ( pCcspCwmpAcsBroker )
    {
        pCcspCwmpAcsBroker->Cancel((ANSC_HANDLE)pCcspCwmpAcsBroker);
    }

    if ( pCcspCwmpProcessor )
    {
        pCcspCwmpProcessor->Cancel((ANSC_HANDLE)pCcspCwmpProcessor);
    }

#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    if ( pCcspCwmpTcpcrHandler )
    {
        pCcspCwmpTcpcrHandler->Cancel((ANSC_HANDLE)pCcspCwmpTcpcrHandler);
    }
#endif

#ifdef   _CCSP_CWMP_STUN_ENABLED    
    if ( pCcspCwmpStunManager )
    {
        pCcspCwmpStunManager->Cancel((ANSC_HANDLE)pCcspCwmpStunManager);
    }
#endif

    pMyObject->bDelayedInformCancelled  = FALSE;

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetupEnv
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to setup the operating environment.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

/*
static 
void*
CcspCwmpCpecoAllocateMemory
    ( 
        size_t                      size 
    )
{
    return  AnscAllocateMemory(size);
}

void 
CcspCwmpCpecoFreeMemory
   ( 
        void *                      ptr 
   )
{
    AnscFreeMemory(ptr);
}
*/


ANSC_STATUS
CcspCwmpCpecoSetupEnv
    (
        ANSC_HANDLE                 hThisObject
    )
{
    /*
     *  set up underlying message bus environment
     *  read persistent configuration data - PA-ID, CR/CDMM IDs
     *  register PA to CDMM
     *  register TCP URL Handling service into CR for secondary PAs
     */
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject         = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
//    int                             nRet = 0;

    if ( !pMyObject->PAMapperFile )
    {
        CcspTr069PaTraceError(("TR-069 PA mapper file is not specified!\n"));

        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    /* load PA mapper file */
    pMyObject->hTr069PaMapper = 
        CcspTr069PA_LoadMappingFile
            (
                pMyObject->PAMapperFile
            );

    if (pMyObject->PACustomMapperFile != NULL)
    {
        CcspTr069PA_LoadCustomMappingFile
            (
                pMyObject->hTr069PaMapper,
                pMyObject->PACustomMapperFile
            );
    }

    if ( TRUE )
    {
        CCSP_INT                nSubsystems = 0;
        CCSP_INT                i;
        CCSP_STRING             pSubsysName;

        nSubsystems = CcspTr069PA_GetSubsystemCount(pMyObject->hTr069PaMapper);

        for ( i = 0; i < nSubsystems; i ++ )
        {
            pSubsysName = CcspTr069PA_GetSubsystemByIndex(pMyObject->hTr069PaMapper, i);

            returnStatus = 
                pMyObject->SetCRBusPath
                    (
                        (ANSC_HANDLE)pMyObject, 
                        pSubsysName, 
                        CCSP_DBUS_PATH_CR
                    );
        }
    }

    /* initialize message bus */
#ifdef DBUS_INIT_SYNC_MODE
//    nRet = 
        CCSP_Message_Bus_Init_Synced
            (
                pMyObject->PANameWithPrefix,
                CCSP_MSG_BUS_CFG,
                (void**)&pMyObject->hMsgBusHandle,
                (CCSP_MESSAGE_BUS_MALLOC)Ansc_AllocateMemory_Callback, 
                (CCSP_MESSAGE_BUS_FREE)Ansc_FreeMemory_Callback
            );
#else
//    nRet = 
        CCSP_Message_Bus_Init
            (
                pMyObject->PANameWithPrefix,
                CCSP_MSG_BUS_CFG,
                (void**)&pMyObject->hMsgBusHandle,
                (CCSP_MESSAGE_BUS_MALLOC)Ansc_AllocateMemory_Callback, 
                (CCSP_MESSAGE_BUS_FREE)Ansc_FreeMemory_Callback
            );
#endif

/*
    returnStatus = (nRet == CCSP_SUCCESS)? ANSC_STATUS_SUCCESS : ANSC_STATUS_FAILURE;

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTr069PaTraceError(("TR-069 PA fails to connect to message bus!\n"));
    }
*/

    /* register PA to CR */
    returnStatus = pMyObject->RegisterPA((ANSC_HANDLE)pMyObject, TRUE);

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoCloseEnv
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
CcspCwmpCpecoCloseEnv
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject         = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    /* 
     * unregister PA from CR
     */
    returnStatus = pMyObject->RegisterPA((ANSC_HANDLE)pMyObject, FALSE);

    /* disconnect from message bus */
    CCSP_Message_Bus_Exit(pMyObject->hMsgBusHandle);
    pMyObject->hMsgBusHandle = NULL;

    CcspTr069PaTraceInfo(("TR-069 PA has disconnected from message bus!\n"));

    /* unload PA mapper file */
    CcspTr069PA_UnloadMappingFile(pMyObject->hTr069PaMapper);

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

    ANSC_STATUS
    CcspCwmppoStartCwmpTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    );

    description:

        This function is called when it's time to call to reattempt
        engaging CWMP.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoStartCwmpTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject         = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    CcspTr069PaTraceDebug(("CcspCwmppoStartCwmpTimerInvoke - time to start CWMP again!\n"));
    pMyObject->StartCWMP((ANSC_HANDLE)pMyObject, TRUE, FALSE);

    return  returnStatus;
}

