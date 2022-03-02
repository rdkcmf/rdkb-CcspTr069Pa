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

    module: ccsp_cwmp_cpeco_base.c

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This module implements the basic container object functions
        of the CCSP CWMP Cpe Controller Object.

        *   CcspCwmpCpecoCreate
        *   CcspCwmpCpecoRemove
        *   CcspCwmpCpecoEnrollObjects
        *   CcspCwmpCpecoInitialize

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Bin Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/09/05    initial revision.
        02/12/09    add more statistics apis
        06/21/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_cpeco_global.h"


/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoCreate
            (
                ANSC_HANDLE                 hContainerContext,
                ANSC_HANDLE                 hOwnerContext,
                ANSC_HANDLE                 hAnscReserved
            );

    description:

        This function constructs the CcspCwmp Cpe Controller Object and
        initializes the member variables and functions.

    argument:   ANSC_HANDLE                 hContainerContext
                This handle is used by the container object to interact
                with the outside world. It could be the real container
                or an target object.

                ANSC_HANDLE                 hOwnerContext
                This handle is passed in by the owner of this object.

                ANSC_HANDLE                 hAnscReserved
                This handle is passed in by the owner of this object.

    return:     newly created container object.

**********************************************************************/

ANSC_HANDLE
CcspCwmpCpecoCreate
    (
        ANSC_HANDLE                 hContainerContext,
        ANSC_HANDLE                 hOwnerContext,
        ANSC_HANDLE                 hAnscReserved
    )
{
    UNREFERENCED_PARAMETER(hAnscReserved);
    PANSC_COMPONENT_OBJECT          pBaseObject  = NULL;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject    = NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)AnscAllocateMemory(sizeof(CCSP_CWMP_CPE_CONTROLLER_OBJECT));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }
    else
    {
        pBaseObject = (PANSC_COMPONENT_OBJECT)pMyObject;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    /* AnscCopyString(pBaseObject->Name, CCSP_CWMP_CPE_CONTROLLER_NAME); */

    pBaseObject->hContainerContext = hContainerContext;
    pBaseObject->hOwnerContext     = hOwnerContext;
    pBaseObject->Oid               = CCSP_CWMP_CPE_CONTROLLER_OID;
    pBaseObject->Create            = CcspCwmpCpecoCreate;
    pBaseObject->Remove            = CcspCwmpCpecoRemove;
    pBaseObject->EnrollObjects     = CcspCwmpCpecoEnrollObjects;
    pBaseObject->Initialize        = CcspCwmpCpecoInitialize;

    pBaseObject->EnrollObjects((ANSC_HANDLE)pBaseObject);
    pBaseObject->Initialize   ((ANSC_HANDLE)pBaseObject);

    return  (ANSC_HANDLE)pMyObject;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function destroys the object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     
                                    pMyObject               = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;
    PCCSP_CWMP_ACS_BROKER_OBJECT    pCcspCwmpAcsBroker      = (PCCSP_CWMP_ACS_BROKER_OBJECT    )pMyObject->hCcspCwmpAcsBroker;
    PCCSP_CWMP_PROCESSOR_OBJECT     pCcspCwmpProcessor      = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SOAP_PARSER_OBJECT   pCcspCwmpSoapParser     = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pMyObject->hCcspCwmpSoapParser;
    PCCSP_CWMP_STAT_INTERFACE       pCcspCwmpStatIf         = (PCCSP_CWMP_STAT_INTERFACE       )pMyObject->hCcspCwmpStaIf;
#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT pCcspCwmpTcpcrHandler   = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT )pMyObject->hCcspCwmpTcpConnReqHandler;
#endif
    PANSC_TIMER_DESCRIPTOR_OBJECT   pStartCwmpTimerObj      = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hStartCwmpTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pStartCwmpTimerIf       = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hStartCwmpTimerIf;
    int                             i;
#ifdef   _CCSP_CWMP_STUN_ENABLED
    PCCSP_CWMP_STUN_MANAGER_OBJECT  pCcspCwmpStunManager    = (PCCSP_CWMP_STUN_MANAGER_OBJECT)pMyObject->hCcspCwmpStunManager;
#endif

    for ( i = 0; i < pMyObject->NumSubsystems; i ++ )
    {
        if ( pMyObject->Subsystem[i] )
        {
            AnscFreeMemory(pMyObject->Subsystem[i]);
        }
        if ( pMyObject->MBusPath[i] )
        {
            AnscFreeMemory(pMyObject->MBusPath[i]);
        }
    }

    pMyObject->Cancel((ANSC_HANDLE)pMyObject);
    pMyObject->Reset ((ANSC_HANDLE)pMyObject);

    if ( pCcspCwmpAcsBroker )
    {
        pCcspCwmpAcsBroker->Remove((ANSC_HANDLE)pCcspCwmpAcsBroker);

        pMyObject->hCcspCwmpAcsBroker = (ANSC_HANDLE)NULL;
    }

    if ( pCcspCwmpProcessor )
    {
        pCcspCwmpProcessor->Remove((ANSC_HANDLE)pCcspCwmpProcessor);

        pMyObject->hCcspCwmpProcessor = (ANSC_HANDLE)NULL;
    }

    if ( pCcspCwmpSoapParser )
    {
        pCcspCwmpSoapParser->Remove((ANSC_HANDLE)pCcspCwmpSoapParser);

        pMyObject->hCcspCwmpSoapParser = (ANSC_HANDLE)NULL;
    }

#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    if ( pCcspCwmpTcpcrHandler )
    {
        pCcspCwmpTcpcrHandler->Remove((ANSC_HANDLE)pCcspCwmpTcpcrHandler);

        pMyObject->hCcspCwmpTcpConnReqHandler = (ANSC_HANDLE)NULL;
    }
#endif

#ifdef   _CCSP_CWMP_STUN_ENABLED
    if ( pCcspCwmpStunManager )
    {
        pCcspCwmpStunManager->Remove((ANSC_HANDLE)pCcspCwmpStunManager);
    }
#endif

    if( pCcspCwmpStatIf != NULL)
    {
        AnscFreeMemory(pCcspCwmpStatIf);
        pMyObject->hCcspCwmpStaIf = NULL;
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

    if ( pMyObject->PAName )
    {
        AnscFreeMemory(pMyObject->PAName);
    }

    if ( pMyObject->PANameWithPrefix )
    {
        AnscFreeMemory(pMyObject->PANameWithPrefix);
    }

    if ( pMyObject->CRName )
    {
        AnscFreeMemory(pMyObject->CRName);
    }

    if ( pMyObject->CRNameWithPrefix )
    {
        AnscFreeMemory(pMyObject->CRNameWithPrefix);
    }

    if ( pMyObject->PAMapperFile )
    {
        AnscFreeMemory(pMyObject->PAMapperFile);
    }

    if ( pMyObject->PACustomMapperFile )
    {
        AnscFreeMemory(pMyObject->PACustomMapperFile);
    }
    
    if ( pMyObject->SdmXmlFile )
    {
        AnscFreeMemory(pMyObject->SdmXmlFile);
    }

    if ( pMyObject->OutboundIfName )
    {
        AnscFreeMemory(pMyObject->OutboundIfName);
    }

    AnscCoRemove((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoEnrollObjects
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function enrolls all the objects required by this object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoEnrollObjects
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject           = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;
    PCCSP_CWMP_ACS_BROKER_OBJECT         pCcspCwmpAcsBroker      = (PCCSP_CWMP_ACS_BROKER_OBJECT    )pMyObject->hCcspCwmpAcsBroker;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor   = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser     = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pMyObject->hCcspCwmpSoapParser;
    PCCSP_CWMP_STAT_INTERFACE            pCcspCwmpStatIf       = (PCCSP_CWMP_STAT_INTERFACE       )pMyObject->hCcspCwmpStaIf;
#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pCcspCwmpTcpcrHandler = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT )pMyObject->hCcspCwmpTcpConnReqHandler;
#endif
#ifdef   _CCSP_CWMP_STUN_ENABLED
    PCCSP_CWMP_STUN_MANAGER_OBJECT  pCcspCwmpStunManager    = (PCCSP_CWMP_STUN_MANAGER_OBJECT)pMyObject->hCcspCwmpStunManager;
#endif
    PANSC_TIMER_DESCRIPTOR_OBJECT   pStartCwmpTimerObj= (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hStartCwmpTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pStartCwmpTimerIf = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hStartCwmpTimerIf;

    if ( !pCcspCwmpAcsBroker )
    {
        pCcspCwmpAcsBroker =
            (PCCSP_CWMP_ACS_BROKER_OBJECT)CcspCwmpCreateAcsBroker
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pCcspCwmpAcsBroker )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hCcspCwmpAcsBroker = (ANSC_HANDLE)pCcspCwmpAcsBroker;
        }
    }

    if ( !pCcspCwmpProcessor )
    {
        pCcspCwmpProcessor =
            (PCCSP_CWMP_PROCESSOR_OBJECT)CcspCwmpCreateProcessor
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pCcspCwmpProcessor )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hCcspCwmpProcessor = (ANSC_HANDLE)pCcspCwmpProcessor;
        }
    }

    if ( !pCcspCwmpSoapParser )
    {
        pCcspCwmpSoapParser =
            (PCCSP_CWMP_SOAP_PARSER_OBJECT)CcspCwmpCreateSoapParser
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pCcspCwmpSoapParser )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hCcspCwmpSoapParser = (ANSC_HANDLE)pCcspCwmpSoapParser;
        }
    }

#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
    if ( !pCcspCwmpTcpcrHandler )
    {
        pCcspCwmpTcpcrHandler =
            (PCCSP_CWMP_TCPCR_HANDLER_OBJECT)CcspCwmpCreateTcpConnReqHandler
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pCcspCwmpTcpcrHandler )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hCcspCwmpTcpConnReqHandler = (ANSC_HANDLE)pCcspCwmpTcpcrHandler;

            pCcspCwmpTcpcrHandler->SetCcspCwmpCpeController((ANSC_HANDLE)pCcspCwmpTcpcrHandler, (ANSC_HANDLE)pMyObject);
        }
    }
#endif

#ifdef   _CCSP_CWMP_STUN_ENABLED
    if ( !pCcspCwmpStunManager )
    {
        pCcspCwmpStunManager = 
            (PCCSP_CWMP_STUN_MANAGER_OBJECT)CcspCwmpCreateStunManager
                (
                    pMyObject->hContainerContext, 
                    (ANSC_HANDLE)pMyObject, 
                    (ANSC_HANDLE)NULL
                );

        if ( !pCcspCwmpStunManager )
        {
            return  ANSC_STATUS_RESOURCES;
        }

        pMyObject->hCcspCwmpStunManager = (ANSC_HANDLE)pCcspCwmpStunManager;

        pCcspCwmpStunManager->SetCcspCwmpCpeController((ANSC_HANDLE)pCcspCwmpStunManager, (ANSC_HANDLE)pMyObject);
    }
#endif

    if( !pCcspCwmpStatIf)
    {
        pCcspCwmpStatIf = (PCCSP_CWMP_STAT_INTERFACE)AnscAllocateMemory(sizeof(CCSP_CWMP_STAT_INTERFACE));

        if ( !pCcspCwmpStatIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hCcspCwmpStaIf = (ANSC_HANDLE)pCcspCwmpStatIf;
        }

        AnscCopyString(pCcspCwmpStatIf->Name, CCSP_CWMP_STAT_INTERFACE_NAME);

        pCcspCwmpStatIf->InterfaceId   = CCSP_CWMP_STAT_INTERFACE_ID;
        pCcspCwmpStatIf->hOwnerContext = (ANSC_HANDLE)pMyObject;
        pCcspCwmpStatIf->Size          = sizeof(CCSP_CWMP_STAT_INTERFACE);

        pCcspCwmpStatIf->ConnectAcs         = CcspCwmpCpecoStatIfConnectAcs;
        pCcspCwmpStatIf->IncTcpSuccess      = CcspCwmpCpecoStatIfIncTcpSuccess;
        pCcspCwmpStatIf->IncTcpFailure      = CcspCwmpCpecoStatIfIncTcpFailure;
        pCcspCwmpStatIf->IncTlsFailure      = CcspCwmpCpecoStatIfIncTlsFailure;
        pCcspCwmpStatIf->GetTcpSuccessCount = CcspCwmpCpecoStatIfGetTcpSuccessCount;
        pCcspCwmpStatIf->GetTcpFailureCount = CcspCwmpCpecoStatIfGetTcpFailureCount;
        pCcspCwmpStatIf->GetTlsFailureCount = CcspCwmpCpecoStatIfGetTlsFailureCount;
        pCcspCwmpStatIf->ResetStats         = CcspCwmpCpecoStatIfResetStats;
        pCcspCwmpStatIf->GetLastConnectionTime
                                        = CcspCwmpCpecoStatIfGetLastConnectionTime;
        pCcspCwmpStatIf->GetLastConnectionStatus
                                        = CcspCwmpCpecoStatIfGetLastConnectionStatus;
        pCcspCwmpStatIf->GetLastInformResponseTime
                                        = CcspCwmpCpecoStatIfGetLastInformResponseTime;
        pCcspCwmpStatIf->GetLastReceivedSPVTime
                                        = CcspCwmpCpecoStatIfGetLastReceivedSPVTime;
    }

    if ( !pStartCwmpTimerObj )
    {
        pStartCwmpTimerObj =
            (PANSC_TIMER_DESCRIPTOR_OBJECT)AnscCreateTimerDescriptor
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pStartCwmpTimerObj )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hStartCwmpTimerObj = (ANSC_HANDLE)pStartCwmpTimerObj;
        }

        pStartCwmpTimerObj->SetTimerType((ANSC_HANDLE)pStartCwmpTimerObj, ANSC_TIMER_TYPE_SPORADIC   );
        pStartCwmpTimerObj->SetInterval ((ANSC_HANDLE)pStartCwmpTimerObj, CCSP_CWMP_CPECO_CWMP_START_INTERVAL);
    }

    if ( !pStartCwmpTimerIf )
    {
        pStartCwmpTimerIf = (PANSC_TDO_CLIENT_OBJECT)AnscAllocateMemory(sizeof(ANSC_TDO_CLIENT_OBJECT));

        if ( !pStartCwmpTimerIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hStartCwmpTimerIf = (ANSC_HANDLE)pStartCwmpTimerIf;
        }

        pStartCwmpTimerIf->hClientContext = (ANSC_HANDLE)pMyObject;
        pStartCwmpTimerIf->Invoke         = CcspCwmppoStartCwmpTimerInvoke;

        pStartCwmpTimerObj->SetClient((ANSC_HANDLE)pStartCwmpTimerObj, (ANSC_HANDLE)pStartCwmpTimerIf);
    }

    AnscCoEnrollObjects((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function first calls the initialization member function
        of the base class object to set the common member fields
        inherited from the base class. It then initializes the member
        fields that are specific to this object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject    = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;

    /*
     * Until you have to simulate C++ object-oriented programming style with standard C, you don't
     * appreciate all the nice little things come with C++ language and all the dirty works that
     * have been done by the C++ compilers. Member initialization is one of these things. While in
     * C++ you don't have to initialize all the member fields inherited from the base class since
     * the compiler will do it for you, such is not the case with C.
     */
    AnscCoInitialize((ANSC_HANDLE)pMyObject);

    /*
     * Although we have initialized some of the member fields in the "create" member function, we
     * repeat the work here for completeness. While this simulation approach is pretty stupid from
     * a C++/Java programmer perspective, it's the best we can get for universal embedded network
     * programming. Before we develop our own operating system (don't expect that to happen any
     * time soon), this is the way things gonna be.
     */
    pMyObject->Oid                      = CCSP_CWMP_CPE_CONTROLLER_OID;
    pMyObject->Create                   = CcspCwmpCpecoCreate;
    pMyObject->Remove                   = CcspCwmpCpecoRemove;
    pMyObject->EnrollObjects            = CcspCwmpCpecoEnrollObjects;
    pMyObject->Initialize               = CcspCwmpCpecoInitialize;

    pMyObject->bActive                  = FALSE;

    pMyObject->bBootEventAdded          = FALSE;
    pMyObject->pRootObject              = AnscCloneString(DM_ROOTNAME);

    pMyObject->PAName                   = AnscCloneString(CCSP_TR069PA_DEF_NAME);
    pMyObject->CRName                   = AnscCloneString(CCSP_CR_DEF_NAME);
    pMyObject->PAMapperFile             = NULL;
    pMyObject->PACustomMapperFile       = NULL; 
    pMyObject->SdmXmlFile               = NULL;
    pMyObject->OutboundIfName           = NULL;

    pMyObject->GetCcspCwmpAcsBroker         = CcspCwmpCpecoGetCcspCwmpAcsBroker;
    pMyObject->GetCcspCwmpProcessor      = CcspCwmpCpecoGetCcspCwmpProcessor;
    pMyObject->GetCcspCwmpSoapParser        = CcspCwmpCpecoGetCcspCwmpSoapParser;
    pMyObject->GetCcspCwmpTcpConnReqHandler = CcspCwmpCpecoGetCcspCwmpTcpConnReqHandler;
    pMyObject->GetRootObject            = CcspCwmpCpecoGetRootObject;
    pMyObject->SetRootObject            = CcspCwmpCpecoSetRootObject;

    pMyObject->GetProperty              = CcspCwmpCpecoGetProperty;
    pMyObject->SetProperty              = CcspCwmpCpecoSetProperty;
    pMyObject->ResetProperty            = CcspCwmpCpecoResetProperty;
    pMyObject->Reset                    = CcspCwmpCpecoReset;

    pMyObject->GetCcspCwmpMcoIf             = CcspCwmpCpecoGetCcspCwmpMcoIf;
    pMyObject->GetCcspCwmpMsoIf             = CcspCwmpCpecoGetCcspCwmpMsoIf;

    pMyObject->Engage                   = CcspCwmpCpecoEngage;
    pMyObject->Cancel                   = CcspCwmpCpecoCancel;
    pMyObject->SetupEnv                 = CcspCwmpCpecoSetupEnv;
    pMyObject->CloseEnv                 = CcspCwmpCpecoCloseEnv;

    pMyObject->StartCWMP                = CcspCwmpCpecoStartCWMP;
    pMyObject->StopCWMP                 = CcspCwmpCpecoStopCWMP;
    pMyObject->RestartCWMP              = CcspCwmpCpecoRestartCWMP;
    pMyObject->AcqCrSessionID           = CcspCwmpCpecoAcqCrSessionID;
    pMyObject->RelCrSessionID           = CcspCwmpCpecoRelCrSessionID;

    pMyObject->GetCcspCwmpCfgIf             = CcspCwmpCpecoGetCcspCwmpCfgIf;
    pMyObject->SetCcspCwmpCfgIf             = CcspCwmpCpecoSetCcspCwmpCfgIf;

    pMyObject->InformNow                = CcspCwmpCpecoInformNow;
    pMyObject->NotifyEvent              = CcspCwmpCpecoNotifyEvent;
    pMyObject->bDelayedInformCancelled  = FALSE;
    pMyObject->bBootInformSent          = FALSE;
    pMyObject->bRestartCWMP             = FALSE;

    pMyObject->GetParamValues           = CcspCwmpCpecoGetParamValues;
    pMyObject->GetParamStringValues     = CcspCwmpCpecoGetParamStringValues;
    pMyObject->GetParamStringValue      = CcspCwmpCpecoGetParamStringValue;
    /*pMyObject->SetParamValues           = CcspCwmpCpecoSetParamValues;*/
    pMyObject->AddObjects               = CcspCwmpCpecoAddObjects;
    pMyObject->DeleteObjects            = CcspCwmpCpecoDeleteObjects;
    pMyObject->MonitorOpState           = CcspCwmpCpecoMonitorOpState;
    pMyObject->RegisterPA               = CcspCwmpCpecoRegisterPA;
    pMyObject->SetPAName                = CcspCwmpCpecoSetPAName;
    pMyObject->GetPAName                = CcspCwmpCpecoGetPAName;
    pMyObject->SetSubsysName            = CcspCwmpCpecoSetSubsysName;
    pMyObject->GetSubsysName            = CcspCwmpCpecoGetSubsysName;
    pMyObject->SetCRName                = CcspCwmpCpecoSetCRName;
    pMyObject->GetCRName                = CcspCwmpCpecoGetCRName;
    pMyObject->SetCRBusPath             = CcspCwmpCpecoSetCRBusPath;
    pMyObject->GetCRBusPath             = CcspCwmpCpecoGetCRBusPath;
    pMyObject->SetPAMapperFile          = CcspCwmpCpecoSetPAMapperFile;
    pMyObject->SetPACustomMapperFile    = CcspCwmpCpecoSetPACustomMapperFile;
    pMyObject->SetSDMXmlFilename        = CcspCwmpCpecoSetSDMXmlFilename;
    pMyObject->GetSDMXmlFilename        = CcspCwmpCpecoGetSDMXmlFilename;
    pMyObject->SetOutboundIfName        = CcspCwmpCpecoSetOutboundIfName;
    pMyObject->GetOutboundIfName        = CcspCwmpCpecoGetOutboundIfName;
    pMyObject->GetMsgBusHandle          = CcspCwmpCpecoGetMsgBusHandle;
    pMyObject->GetParamNotification     = CcspCwmpCpecoGetParamNotification;
    pMyObject->SetParamNotification     = CcspCwmpCpecoSetParamNotification;
    pMyObject->GetParamDataType         = CcspCwmpCpecoGetParamDataType;
    pMyObject->SaveCfgToPsm             = CcspCwmpCpecoSaveCfgToPsm;
    pMyObject->LoadCfgFromPsm           = CcspCwmpCpecoLoadCfgFromPsm;
    pMyObject->SetParameterKey          = CcspCwmpCpecoSetParameterKey;

    AnscZeroMemory(&pMyObject->cwmpStats, sizeof(CCSP_CWMP_STATISTICS));

    /*
     * We shall initialize the configuration properties to the default values, which may be changed
     * later via the set_property() member function. Note that this call may not guarantee a valid
     * and legtimate configuration.
     */
    pMyObject->ResetProperty((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}
