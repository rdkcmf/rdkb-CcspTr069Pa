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

    module:	ccsp_cwmp_proco_base.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the basic container object functions
        of the CcspCwmp Wmp Processor Object.

        *   CcspCwmppoCreate
        *   CcspCwmppoRemove
        *   CcspCwmppoEnrollObjects
        *   CcspCwmppoInitialize

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
        06/21/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_proco_global.h"


/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CcspCwmppoCreate
            (
                ANSC_HANDLE                 hContainerContext,
                ANSC_HANDLE                 hOwnerContext,
                ANSC_HANDLE                 hAnscReserved
            );

    description:

        This function constructs the CcspCwmp Wmp Processor Object and
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
CcspCwmppoCreate
    (
        ANSC_HANDLE                 hContainerContext,
        ANSC_HANDLE                 hOwnerContext,
        ANSC_HANDLE                 hAnscReserved
    )
{
    UNREFERENCED_PARAMETER(hAnscReserved);
    PANSC_COMPONENT_OBJECT          pBaseObject  = NULL;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCCSP_CWMP_PROCESSOR_OBJECT)AnscAllocateMemory(sizeof(CCSP_CWMP_PROCESSOR_OBJECT));

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
    /* AnscCopyString(pBaseObject->Name, CCSP_CWMP_PROCESSOR_NAME); */

    pBaseObject->hContainerContext = hContainerContext;
    pBaseObject->hOwnerContext     = hOwnerContext;
    pBaseObject->Oid               = CCSP_CWMP_PROCESSOR_OID;
    pBaseObject->Create            = CcspCwmppoCreate;
    pBaseObject->Remove            = CcspCwmppoRemove;
    pBaseObject->EnrollObjects     = CcspCwmppoEnrollObjects;
    pBaseObject->Initialize        = CcspCwmppoInitialize;

    pBaseObject->EnrollObjects((ANSC_HANDLE)pBaseObject);
    pBaseObject->Initialize   ((ANSC_HANDLE)pBaseObject);

    return  (ANSC_HANDLE)pMyObject;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoRemove
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
CcspCwmppoRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject         = (PCCSP_CWMP_PROCESSOR_OBJECT   )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pPeriodicTimerObj = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hPeriodicTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pPeriodicTimerIf  = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hPeriodicTimerIf;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pScheduleTimerObj = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hScheduleTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pScheduleTimerIf  = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hScheduleTimerIf;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pPendingInformTimerObj = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hPendingInformTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pPendingInformTimerIf  = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hPendingInformTimerIf;
    PANSC_ATOM_TABLE_OBJECT         pParamAttrCache   = (PANSC_ATOM_TABLE_OBJECT      )pMyObject->hParamAttrCache;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf        = (PCCSP_CWMP_MPA_INTERFACE          )pMyObject->hCcspCwmpMpaIf;
    PCCSP_NAMESPACE_MGR_OBJECT      pCcspNsMgr        = (PCCSP_NAMESPACE_MGR_OBJECT   )pMyObject->hCcspNamespaceMgr;

    pMyObject->Cancel((ANSC_HANDLE)pMyObject);
    pMyObject->Reset ((ANSC_HANDLE)pMyObject);

    if ( pPeriodicTimerObj )
    {
        pPeriodicTimerObj->Remove((ANSC_HANDLE)pPeriodicTimerObj);
    }

    if ( pPeriodicTimerIf )
    {
        AnscFreeMemory(pPeriodicTimerIf);
    }

    if ( pScheduleTimerObj )
    {
        pScheduleTimerObj->Remove((ANSC_HANDLE)pScheduleTimerObj);
    }

    if ( pScheduleTimerIf )
    {
        AnscFreeMemory(pScheduleTimerIf);
    }

    if ( pPendingInformTimerObj )
    {
        pPendingInformTimerObj->Remove((ANSC_HANDLE)pPendingInformTimerObj);
    }

    if ( pPendingInformTimerIf )
    {
        AnscFreeMemory(pPendingInformTimerIf);
    }

    if ( pCcspCwmpMpaIf )
    {
        AnscFreeMemory(pCcspCwmpMpaIf);

        pMyObject->hCcspCwmpMpaIf = (ANSC_HANDLE)NULL;
    }

    if ( pCcspNsMgr )
    {
        pCcspNsMgr->CleanAll((ANSC_HANDLE)pCcspNsMgr);
        AnscFreeMemory(pCcspNsMgr);
    }

    if ( pMyObject->SecheduledCommandKey ) 
    {
        AnscFreeMemory(pMyObject->SecheduledCommandKey);
    }

    AnscFreeLock (&pMyObject->WmpsoQueueLock   );
    AnscFreeEvent(&pMyObject->AsyncConnectEvent);
    AnscCoRemove ((ANSC_HANDLE)pMyObject       );

    if ( pParamAttrCache )
    {
        pParamAttrCache->Remove((ANSC_HANDLE)pParamAttrCache);
    }

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoEnrollObjects
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
CcspCwmppoEnrollObjects
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject         = (PCCSP_CWMP_PROCESSOR_OBJECT   )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pPeriodicTimerObj = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hPeriodicTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pPeriodicTimerIf  = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hPeriodicTimerIf;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pScheduleTimerObj = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hScheduleTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pScheduleTimerIf  = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hScheduleTimerIf;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pPendingInformTimerObj = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hPendingInformTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pPendingInformTimerIf  = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hPendingInformTimerIf;
    PCCSP_CWMP_MPA_INTERFACE        pCcspCwmpMpaIf        = (PCCSP_CWMP_MPA_INTERFACE          )pMyObject->hCcspCwmpMpaIf;

    if ( !pPeriodicTimerObj )
    {
        pPeriodicTimerObj =
            (PANSC_TIMER_DESCRIPTOR_OBJECT)AnscCreateTimerDescriptor
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pPeriodicTimerObj )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hPeriodicTimerObj = (ANSC_HANDLE)pPeriodicTimerObj;
        }

        pPeriodicTimerObj->SetTimerType((ANSC_HANDLE)pPeriodicTimerObj, ANSC_TIMER_TYPE_PERIODIC      );
        pPeriodicTimerObj->SetInterval ((ANSC_HANDLE)pPeriodicTimerObj, CCSP_CWMPPO_DEF_INFORM_INTERVAL);
        /* _ansc_strcpy(pPeriodicTimerObj->Name, "CcspCwmppoPeriodicTimer"); */
    }

    if ( !pPeriodicTimerIf )
    {
        pPeriodicTimerIf = (PANSC_TDO_CLIENT_OBJECT)AnscAllocateMemory(sizeof(ANSC_TDO_CLIENT_OBJECT));

        if ( !pPeriodicTimerIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hPeriodicTimerIf = (ANSC_HANDLE)pPeriodicTimerIf;
        }

        pPeriodicTimerIf->hClientContext = (ANSC_HANDLE)pMyObject;
        pPeriodicTimerIf->Invoke         = CcspCwmppoPeriodicTimerInvoke;

        pPeriodicTimerObj->SetClient((ANSC_HANDLE)pPeriodicTimerObj, (ANSC_HANDLE)pPeriodicTimerIf);
    }

    if ( !pScheduleTimerObj )
    {
        pScheduleTimerObj =
            (PANSC_TIMER_DESCRIPTOR_OBJECT)AnscCreateTimerDescriptor
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pScheduleTimerObj )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hScheduleTimerObj = (ANSC_HANDLE)pScheduleTimerObj;
        }

        pScheduleTimerObj->SetTimerType((ANSC_HANDLE)pScheduleTimerObj, ANSC_TIMER_TYPE_SPORADIC   );
        pScheduleTimerObj->SetInterval ((ANSC_HANDLE)pScheduleTimerObj, CCSP_CWMPPO_DEF_INFORM_DELAY);
        /* _ansc_strcpy(pScheduleTimerObj->Name, "CcspCwmppoScheduleTimer"); */

    }

    if ( !pScheduleTimerIf )
    {
        pScheduleTimerIf = (PANSC_TDO_CLIENT_OBJECT)AnscAllocateMemory(sizeof(ANSC_TDO_CLIENT_OBJECT));

        if ( !pScheduleTimerIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hScheduleTimerIf = (ANSC_HANDLE)pScheduleTimerIf;
        }

        pScheduleTimerIf->hClientContext = (ANSC_HANDLE)pMyObject;
        pScheduleTimerIf->Invoke         = CcspCwmppoScheduleTimerInvoke;

        pScheduleTimerObj->SetClient((ANSC_HANDLE)pScheduleTimerObj, (ANSC_HANDLE)pScheduleTimerIf);
    }

    if ( !pPendingInformTimerObj )
    {
        pPendingInformTimerObj =
            (PANSC_TIMER_DESCRIPTOR_OBJECT)AnscCreateTimerDescriptor
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pPendingInformTimerObj )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hPendingInformTimerObj = (ANSC_HANDLE)pPendingInformTimerObj;
        }

        pPendingInformTimerObj->SetTimerType((ANSC_HANDLE)pPendingInformTimerObj, ANSC_TIMER_TYPE_SPORADIC   );
        pPendingInformTimerObj->SetInterval ((ANSC_HANDLE)pPendingInformTimerObj, CCSP_CWMPPO_DEF_PENDING_INFORM_DELAY);
        /* _ansc_strcpy(pPendingInformTimerObj->Name, "CcspCwmppoPendingInformTimer"); */
    }

    if ( !pPendingInformTimerIf )
    {
        pPendingInformTimerIf = (PANSC_TDO_CLIENT_OBJECT)AnscAllocateMemory(sizeof(ANSC_TDO_CLIENT_OBJECT));

        if ( !pPendingInformTimerIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hPendingInformTimerIf = (ANSC_HANDLE)pPendingInformTimerIf;
        }

        pPendingInformTimerIf->hClientContext = (ANSC_HANDLE)pMyObject;
        pPendingInformTimerIf->Invoke         = CcspCwmppoPendingInformTimerInvoke;

        pPendingInformTimerObj->SetClient((ANSC_HANDLE)pPendingInformTimerObj, (ANSC_HANDLE)pPendingInformTimerIf);
    }

    if ( !pCcspCwmpMpaIf )
    {
        pCcspCwmpMpaIf = (PCCSP_CWMP_MPA_INTERFACE)AnscAllocateMemory(sizeof(CCSP_CWMP_MPA_INTERFACE));

        if ( !pCcspCwmpMpaIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hCcspCwmpMpaIf = (ANSC_HANDLE)pCcspCwmpMpaIf;
        }

        AnscCopyString(pCcspCwmpMpaIf->Name, CCSP_CWMP_MPA_INTERFACE_NAME);

        pCcspCwmpMpaIf->InterfaceId            = CCSP_CWMP_MPA_INTERFACE_ID;
        pCcspCwmpMpaIf->hOwnerContext          = (ANSC_HANDLE)pMyObject;
        pCcspCwmpMpaIf->Size                   = sizeof(CCSP_CWMP_MPA_INTERFACE);

        pCcspCwmpMpaIf->LockWriteAccess        = CcspCwmppoMpaLockWriteAccess;
        pCcspCwmpMpaIf->UnlockWriteAccess      = CcspCwmppoMpaUnlockWriteAccess;

        pCcspCwmpMpaIf->SetParameterValues     = CcspCwmppoMpaSetParameterValues;
        pCcspCwmpMpaIf->SetParameterValuesWithWriteID     
                                           = CcspCwmppoMpaSetParameterValuesWithWriteID;
        pCcspCwmpMpaIf->GetParameterValues     = CcspCwmppoMpaGetParameterValues;
        pCcspCwmpMpaIf->GetParameterNames      = CcspCwmppoMpaGetParameterNames;
        pCcspCwmpMpaIf->SetParameterAttributes = CcspCwmppoMpaSetParameterAttributes;
        pCcspCwmpMpaIf->GetParameterAttributes = CcspCwmppoMpaGetParameterAttributes;
        pCcspCwmpMpaIf->AddObject              = CcspCwmppoMpaAddObject;
        pCcspCwmpMpaIf->DeleteObject           = CcspCwmppoMpaDeleteObject;
    }

    AnscCoEnrollObjects((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoInitialize
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
CcspCwmppoInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT)hThisObject;

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
    pMyObject->Oid                       = CCSP_CWMP_PROCESSOR_OID;
    pMyObject->Create                    = CcspCwmppoCreate;
    pMyObject->Remove                    = CcspCwmppoRemove;
    pMyObject->EnrollObjects             = CcspCwmppoEnrollObjects;
    pMyObject->Initialize                = CcspCwmppoInitialize;

    pMyObject->SecheduledCommandKey      = NULL;
    pMyObject->hCcspCwmpCpeController        = (ANSC_HANDLE)NULL;
    pMyObject->AsyncTaskCount            = 0;
    pMyObject->bActive                   = FALSE;
    
    pMyObject->MsFcInitDone              = FALSE;
    pMyObject->bAcsInfoRetrieved         = FALSE;

    pMyObject->GetCcspCwmpCpeController  = CcspCwmppoGetCcspCwmpCpeController;
    pMyObject->SetCcspCwmpCpeController  = CcspCwmppoSetCcspCwmpCpeController;
    pMyObject->GetInitialContact         = CcspCwmppoGetInitialContact;
    pMyObject->SetInitialContact         = CcspCwmppoSetInitialContact;
    pMyObject->GetInitialContactFactory  = CcspCwmppoGetInitialContactFactory;
    pMyObject->SetInitialContactFactory  = CcspCwmppoSetInitialContactFactory;
    pMyObject->IsCwmpEnabled             = CcspCwmppoIsCwmpEnabled;
    pMyObject->GetAcsUrl                 = CcspCwmppoGetAcsUrl;
    pMyObject->GetAcsUsername            = CcspCwmppoGetAcsUsername;
    pMyObject->GetAcsPassword            = CcspCwmppoGetAcsPassword;
    pMyObject->GetPeriodicInformEnabled  = CcspCwmppoGetPeriodicInformEnabled;
    pMyObject->GetPeriodicInformInterval = CcspCwmppoGetPeriodicInformInterval;
    pMyObject->GetPeriodicInformTime     = CcspCwmppoGetPeriodicInformTime;

    pMyObject->GetCcspCwmpMpaIf          = CcspCwmppoGetCcspCwmpMpaIf;
    pMyObject->GetProperty               = CcspCwmppoGetProperty;
    pMyObject->SetProperty               = CcspCwmppoSetProperty;
    pMyObject->ResetProperty             = CcspCwmppoResetProperty;
    pMyObject->Reset                     = CcspCwmppoReset;

    pMyObject->Engage                    = CcspCwmppoEngage;
    pMyObject->Cancel                    = CcspCwmppoCancel;
    pMyObject->SetupEnv                  = CcspCwmppoSetupEnv;
    pMyObject->CloseEnv                  = CcspCwmppoCloseEnv;
    pMyObject->PeriodicTimerInvoke       = CcspCwmppoPeriodicTimerInvoke;
    pMyObject->ScheduleTimerInvoke       = CcspCwmppoScheduleTimerInvoke;

    pMyObject->AcqWmpSession             = CcspCwmppoAcqWmpSession;
    pMyObject->AcqWmpSession2            = CcspCwmppoAcqWmpSession2;
    pMyObject->AcqWmpSession3            = CcspCwmppoAcqWmpSession3;
    pMyObject->RelWmpSession             = CcspCwmppoRelWmpSession;
    pMyObject->DelWmpSession             = CcspCwmppoDelWmpSession;
    pMyObject->DelAllSessions            = CcspCwmppoDelAllSessions;
    pMyObject->GetActiveWmpSessionCount  = CcspCwmppoGetActiveWmpSessionCount;
    pMyObject->InitParamAttrCache        = CcspCwmppoInitParamAttrCache;
#ifdef   _CCSP_TR069PA_PUSH_VC_SIGNAL_ON_LOAD
    pMyObject->PushAllVcToBackend        = CcspCwmppoPushAllVcToBackend;
#endif
    pMyObject->SyncNamespacesWithCR      = CcspCwmppoSyncNamespacesWithCR;
    pMyObject->UpdateParamAttrCache      = CcspCwmppoUpdateParamAttrCache;
    pMyObject->CheckParamAttrCache       = CcspCwmppoCheckParamAttrCache;
    pMyObject->HasPendingInform          = CcspCwmppoHasPendingInform;

    pMyObject->ConfigPeriodicInform      = CcspCwmppoConfigPeriodicInform;
    pMyObject->ScheduleInform            = CcspCwmppoScheduleInform;
    pMyObject->SignalSession             = CcspCwmppoSignalSession;
    pMyObject->AsyncConnectTask          = CcspCwmppoAsyncConnectTask;

    pMyObject->GetAcsInfo                = CcspCwmppoGetAcsInfo;
    pMyObject->MsValueChanged            = (PFN_CCSPMS_VALUECHANGE)CcspCwmppoMsValueChanged;

    pMyObject->LoadValueChanged          = CcspCwmppoLoadValueChanged;
    pMyObject->SaveValueChanged          = CcspCwmppoSaveValueChanged;
    pMyObject->DiscardValueChanged       = CcspCwmppoDiscardValueChanged;

    pMyObject->RedeliverEvents           = CcspCwmppoRedeliverEvents;
    pMyObject->GetUndeliveredEvents      = CcspCwmppoGetUndeliveredEvents;
    pMyObject->GetUndeliveredTcEvents    = CcspCwmppoGetUndeliveredTcEvents;
    pMyObject->GetUndeliveredAtcEvents   = CcspCwmppoGetUndeliveredAtcEvents;
    pMyObject->GetUndeliveredDscEvents   = CcspCwmppoGetUndeliveredDscEvents;
    pMyObject->GetUndeliveredAdscEvents  = CcspCwmppoGetUndeliveredAdscEvents;
    pMyObject->SaveTransferComplete      = CcspCwmppoSaveTransferComplete;
    pMyObject->LoadTransferComplete      = CcspCwmppoLoadTransferComplete;
    pMyObject->SaveAutonomousTransferComplete
                                         = CcspCwmppoSaveAutonomousTransferComplete;
    pMyObject->LoadAutonomousTransferComplete      
                                         = CcspCwmppoLoadAutonomousTransferComplete;
    pMyObject->SaveDUStateChangeComplete = CcspCwmppoSaveDUStateChangeComplete;
    pMyObject->LoadDUStateChangeComplete = CcspCwmppoLoadDUStateChangeComplete;
    pMyObject->SaveAutonDUStateChangeComplete
                                         = CcspCwmppoSaveAutonomousDUStateChangeComplete;
    pMyObject->LoadAutonDUStateChangeComplete      
                                         = CcspCwmppoLoadAutonomousDUStateChangeComplete;
    pMyObject->DiscardUndeliveredEvents  = CcspCwmppoDiscardUndeliveredEvents;

    AnscQueueInitializeHeader(&pMyObject->WmpsoQueue       );
    AnscInitializeLock       (&pMyObject->WmpsoQueueLock   );
    AnscInitializeEvent      (&pMyObject->AsyncConnectEvent);
    AnscResetEvent           (&pMyObject->AsyncConnectEvent);

    /*
     * We shall initialize the configuration properties to the default values, which may be changed
     * later via the set_property() member function. Note that this call may not guarantee a valid
     * and legtimate configuration.
     */
    pMyObject->ResetProperty((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}
