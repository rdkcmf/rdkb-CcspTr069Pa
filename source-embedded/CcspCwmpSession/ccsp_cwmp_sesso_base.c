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

    module:	ccsp_cwmp_sesso_base.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the basic container object functions
        of the CCSP CWMP Session Object.

        *   CcspCwmpsoCreate
        *   CcspCwmpsoRemove
        *   CcspCwmpsoEnrollObjects
        *   CcspCwmpsoInitialize

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
        06/22/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_sesso_global.h"


/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CcspCwmpsoCreate
            (
                ANSC_HANDLE                 hContainerContext,
                ANSC_HANDLE                 hOwnerContext,
                ANSC_HANDLE                 hAnscReserved
            );

    description:

        This function constructs the CCSP CWMP Session Object and
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
CcspCwmpsoCreate
    (
        ANSC_HANDLE                 hContainerContext,
        ANSC_HANDLE                 hOwnerContext,
        ANSC_HANDLE                 hAnscReserved
    )
{
    UNREFERENCED_PARAMETER(hAnscReserved);
    PANSC_COMPONENT_OBJECT          pBaseObject  = NULL;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject    = NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCCSP_CWMP_SESSION_OBJECT)AnscAllocateMemory(sizeof(CCSP_CWMP_SESSION_OBJECT));

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
    /* AnscCopyString(pBaseObject->Name, CCSP_CWMP_SESSION_NAME); */

    pBaseObject->hContainerContext = hContainerContext;
    pBaseObject->hOwnerContext     = hOwnerContext;
    pBaseObject->Oid               = CCSP_CWMP_SESSION_OID;
    pBaseObject->Create            = CcspCwmpsoCreate;
    pBaseObject->Remove            = CcspCwmpsoRemove;
    pBaseObject->EnrollObjects     = CcspCwmpsoEnrollObjects;
    pBaseObject->Initialize        = CcspCwmpsoInitialize;

    pBaseObject->EnrollObjects((ANSC_HANDLE)pBaseObject);
    pBaseObject->Initialize   ((ANSC_HANDLE)pBaseObject);

    return  (ANSC_HANDLE)pMyObject;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoRemove
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
CcspCwmpsoRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT       pMyObject          = (PCCSP_CWMP_SESSION_OBJECT     )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pSessionTimerObj   = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hSessionTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pSessionTimerIf    = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hSessionTimerIf;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pRetryTimerObj     = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hRetryTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pRetryTimerIf      = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hRetryTimerIf;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pDelayedActiveNotifTimerObj     
													   = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hDelayedActiveNotifTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pDelayedActiveNotifTimerIf      
													   = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hDelayedActiveNotifTimerIf;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pCcspCwmpAcsConnection = (PCCSP_CWMP_ACS_CONNECTION_OBJECT  )pMyObject->hCcspCwmpAcsConnection;
    PCCSP_CWMP_MCO_INTERFACE             pCcspCwmpMcoIf         = (PCCSP_CWMP_MCO_INTERFACE          )pMyObject->hCcspCwmpMcoIf;

    pMyObject->Reset((ANSC_HANDLE)pMyObject);

    if ( pSessionTimerObj )
    {
        pSessionTimerObj->Remove((ANSC_HANDLE)pSessionTimerObj);
    }

    if ( pSessionTimerIf )
    {
        AnscFreeMemory(pSessionTimerIf);
    }

    if ( pRetryTimerObj )
    {
        pRetryTimerObj->Remove((ANSC_HANDLE)pRetryTimerObj);
    }

    if ( pRetryTimerIf )
    {
        AnscFreeMemory(pRetryTimerIf);
    }

    if ( pDelayedActiveNotifTimerObj )
    {
        pDelayedActiveNotifTimerObj->Remove((ANSC_HANDLE)pDelayedActiveNotifTimerObj);
    }

    if ( pDelayedActiveNotifTimerIf )
    {
        AnscFreeMemory(pDelayedActiveNotifTimerIf);
    }

    if ( pCcspCwmpAcsConnection )
    {
        pCcspCwmpAcsConnection->Remove((ANSC_HANDLE)pCcspCwmpAcsConnection);

        pMyObject->hCcspCwmpAcsConnection = (ANSC_HANDLE)NULL;
    }

    if ( pCcspCwmpMcoIf )
    {
        AnscFreeMemory(pCcspCwmpMcoIf);

        pMyObject->hCcspCwmpMcoIf = (ANSC_HANDLE)NULL;
    }

    AnscFreeLock (&pMyObject->AccessLock       );
    AnscFreeLock (&pMyObject->AsyncReqQueueLock);
    AnscFreeLock (&pMyObject->SavedReqQueueLock);
    AnscFreeLock (&pMyObject->AsyncRepQueueLock);
    AnscFreeEvent(&pMyObject->AsyncProcessEvent);
    AnscCoRemove ((ANSC_HANDLE)pMyObject       );

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoEnrollObjects
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
CcspCwmpsoEnrollObjects
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT       pMyObject          = (PCCSP_CWMP_SESSION_OBJECT    )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pSessionTimerObj   = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hSessionTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pSessionTimerIf    = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hSessionTimerIf;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pCcspCwmpAcsConnection 
													   = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)pMyObject->hCcspCwmpAcsConnection;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pRetryTimerObj     = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hRetryTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pRetryTimerIf      = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hRetryTimerIf;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pDelayedActiveNotifTimerObj     
													   = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hDelayedActiveNotifTimerObj;
    PANSC_TDO_CLIENT_OBJECT         pDelayedActiveNotifTimerIf      
													   = (PANSC_TDO_CLIENT_OBJECT      )pMyObject->hDelayedActiveNotifTimerIf;
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf     = (PCCSP_CWMP_MCO_INTERFACE     )pMyObject->hCcspCwmpMcoIf;

    errno_t rc = -1;
    if ( !pSessionTimerObj )
    {
        pSessionTimerObj =
            (PANSC_TIMER_DESCRIPTOR_OBJECT)AnscCreateTimerDescriptor
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pSessionTimerObj )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hSessionTimerObj = (ANSC_HANDLE)pSessionTimerObj;
        }

        pSessionTimerObj->SetTimerType((ANSC_HANDLE)pSessionTimerObj, ANSC_TIMER_TYPE_SPORADIC         );
        pSessionTimerObj->SetInterval ((ANSC_HANDLE)pSessionTimerObj, CCSP_CWMPSO_SESSION_TIMEOUT * 1000);
        /* _ansc_strcpy(pSessionTimerObj->Name, "CcspCwmpsoSessionTimer"); */

    }

    if ( !pSessionTimerIf )
    {
        pSessionTimerIf = (PANSC_TDO_CLIENT_OBJECT)AnscAllocateMemory(sizeof(ANSC_TDO_CLIENT_OBJECT));

        if ( !pSessionTimerIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hSessionTimerIf = (ANSC_HANDLE)pSessionTimerIf;
        }

        pSessionTimerIf->hClientContext = (ANSC_HANDLE)pMyObject;
        pSessionTimerIf->Invoke         = CcspCwmpsoSessionTimerInvoke;

        pSessionTimerObj->SetClient((ANSC_HANDLE)pSessionTimerObj, (ANSC_HANDLE)pSessionTimerIf);
    }

    if ( !pRetryTimerObj )
    {
        /* use separate timer scehduler to run retry alg, otherwise, we will observe delays of timer expiration */
        pRetryTimerObj =
            (PANSC_TIMER_DESCRIPTOR_OBJECT)AnscCreateTimerDescriptor
                (
                    (ANSC_HANDLE)NULL, /* pMyObject->hContainerContext,*/ 
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pRetryTimerObj )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hRetryTimerObj = (ANSC_HANDLE)pRetryTimerObj;
        }

        pRetryTimerObj->SetTimerType((ANSC_HANDLE)pRetryTimerObj, ANSC_TIMER_TYPE_SPORADIC         );
        pRetryTimerObj->SetInterval ((ANSC_HANDLE)pRetryTimerObj, 1000 * 1000);
        /* _ansc_strcpy(pSessionTimerObj->Name, "CcspCwmpsoSessionRetryTimer"); */

    }

    if ( !pRetryTimerIf )
    {
        pRetryTimerIf = (PANSC_TDO_CLIENT_OBJECT)AnscAllocateMemory(sizeof(ANSC_TDO_CLIENT_OBJECT));

        if ( !pRetryTimerIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hRetryTimerIf = (ANSC_HANDLE)pRetryTimerIf;
        }

        pRetryTimerIf->hClientContext = (ANSC_HANDLE)pMyObject;
        pRetryTimerIf->Invoke         = CcspCwmpsoSessionRetryTimerInvoke;

        pRetryTimerObj->SetClient((ANSC_HANDLE)pRetryTimerObj, (ANSC_HANDLE)pRetryTimerIf);
    }

    if ( !pDelayedActiveNotifTimerObj )
    {
        /* use separate timer scehduler to run retry alg, otherwise, we will observe delays of timer expiration */
        pDelayedActiveNotifTimerObj =
            (PANSC_TIMER_DESCRIPTOR_OBJECT)AnscCreateTimerDescriptor
                (
                    (ANSC_HANDLE)NULL, /* pMyObject->hContainerContext,*/ 
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pDelayedActiveNotifTimerObj )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hDelayedActiveNotifTimerObj = (ANSC_HANDLE)pDelayedActiveNotifTimerObj;
        }

        pDelayedActiveNotifTimerObj->SetTimerType((ANSC_HANDLE)pDelayedActiveNotifTimerObj, ANSC_TIMER_TYPE_SPORADIC);
        pDelayedActiveNotifTimerObj->SetInterval ((ANSC_HANDLE)pDelayedActiveNotifTimerObj, 5000 * 1000);
    }

    if ( !pDelayedActiveNotifTimerIf )
    {
        pDelayedActiveNotifTimerIf = (PANSC_TDO_CLIENT_OBJECT)AnscAllocateMemory(sizeof(ANSC_TDO_CLIENT_OBJECT));

        if ( !pDelayedActiveNotifTimerIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hDelayedActiveNotifTimerIf = (ANSC_HANDLE)pDelayedActiveNotifTimerIf;
        }

        pDelayedActiveNotifTimerIf->hClientContext = (ANSC_HANDLE)pMyObject;
        pDelayedActiveNotifTimerIf->Invoke         = CcspCwmpsoDelayedActiveNotifTimerInvoke;

        pDelayedActiveNotifTimerObj->SetClient((ANSC_HANDLE)pDelayedActiveNotifTimerObj, (ANSC_HANDLE)pDelayedActiveNotifTimerIf);
    }


    if ( !pCcspCwmpAcsConnection )
    {
        pCcspCwmpAcsConnection =
            (PCCSP_CWMP_ACS_CONNECTION_OBJECT)CcspCwmpCreateAcsConnection
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pCcspCwmpAcsConnection )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hCcspCwmpAcsConnection = (ANSC_HANDLE)pCcspCwmpAcsConnection;

            pCcspCwmpAcsConnection->SetCcspCwmpSession((ANSC_HANDLE)pCcspCwmpAcsConnection, (ANSC_HANDLE)pMyObject);
        }
    }

    if ( !pCcspCwmpMcoIf )
    {
        pCcspCwmpMcoIf = (PCCSP_CWMP_MCO_INTERFACE)AnscAllocateMemory(sizeof(CCSP_CWMP_MCO_INTERFACE));

        if ( !pCcspCwmpMcoIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hCcspCwmpMcoIf = (ANSC_HANDLE)pCcspCwmpMcoIf;
        }

        rc = strcpy_s(pCcspCwmpMcoIf->Name, sizeof(pCcspCwmpMcoIf->Name), CCSP_CWMP_MCO_INTERFACE_NAME);
        if(rc != EOK)
        {
                ERR_CHK(rc);
                return ANSC_STATUS_FAILURE;
        }
        pCcspCwmpMcoIf->InterfaceId            = CCSP_CWMP_MCO_INTERFACE_ID;
        pCcspCwmpMcoIf->hOwnerContext          = (ANSC_HANDLE)pMyObject;
        pCcspCwmpMcoIf->Size                   = sizeof(CCSP_CWMP_MCO_INTERFACE);

        pCcspCwmpMcoIf->NotifyAcsStatus        = CcspCwmpsoMcoNotifyAcsStatus;
        pCcspCwmpMcoIf->GetNextMethod          = CcspCwmpsoMcoGetNextMethod;
        pCcspCwmpMcoIf->ProcessSoapResponse    = CcspCwmpsoMcoProcessSoapResponse;
        pCcspCwmpMcoIf->ProcessSoapError       = CcspCwmpsoMcoProcessSoapError;
        pCcspCwmpMcoIf->InvokeUnknownMethod    = CcspCwmpsoMcoInvokeUnknownMethod;

        pCcspCwmpMcoIf->GetRpcMethods          = CcspCwmpsoMcoGetRpcMethods;
        pCcspCwmpMcoIf->SetParameterValues     = CcspCwmpsoMcoSetParameterValues;
        pCcspCwmpMcoIf->GetParameterValues     = CcspCwmpsoMcoGetParameterValues;
        pCcspCwmpMcoIf->GetParameterNames      = CcspCwmpsoMcoGetParameterNames;
        pCcspCwmpMcoIf->SetParameterAttributes = CcspCwmpsoMcoSetParameterAttributes;
        pCcspCwmpMcoIf->GetParameterAttributes = CcspCwmpsoMcoGetParameterAttributes;
        pCcspCwmpMcoIf->AddObject              = CcspCwmpsoMcoAddObject;
        pCcspCwmpMcoIf->DeleteObject           = CcspCwmpsoMcoDeleteObject;
        pCcspCwmpMcoIf->Download               = CcspCwmpsoMcoDownload;
        pCcspCwmpMcoIf->Reboot                 = CcspCwmpsoMcoReboot;

        pCcspCwmpMcoIf->GetQueuedTransfers     = CcspCwmpsoMcoGetQueuedTransfers;
        pCcspCwmpMcoIf->ScheduleInform         = CcspCwmpsoMcoScheduleInform;
        pCcspCwmpMcoIf->SetVouchers            = CcspCwmpsoMcoSetVouchers;
        pCcspCwmpMcoIf->GetOptions             = CcspCwmpsoMcoGetOptions;
        pCcspCwmpMcoIf->Upload                 = CcspCwmpsoMcoUpload;
        pCcspCwmpMcoIf->FactoryReset           = CcspCwmpsoMcoFactoryReset;
        pCcspCwmpMcoIf->ChangeDUState          = CcspCwmpsoMcoChangeDUState;

        pCcspCwmpMcoIf->GetParamDataType       = CcspCwmpsoMcoGetParamDataType;
    }

    AnscCoEnrollObjects((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoInitialize
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
CcspCwmpsoInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject    = (PCCSP_CWMP_SESSION_OBJECT)hThisObject;

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
    pMyObject->Oid                       = CCSP_CWMP_SESSION_OID;
    pMyObject->Create                    = CcspCwmpsoCreate;
    pMyObject->Remove                    = CcspCwmpsoRemove;
    pMyObject->EnrollObjects             = CcspCwmpsoEnrollObjects;
    pMyObject->Initialize                = CcspCwmpsoInitialize;

    pMyObject->hCcspCwmpCpeController        = (ANSC_HANDLE)NULL;
    pMyObject->hCcspCwmpProcessor         = (ANSC_HANDLE)NULL;
    pMyObject->AsyncTaskCount            = 0;
    pMyObject->bActive                   = FALSE;

    pMyObject->bInformWhenActive         = FALSE;

    pMyObject->GlobalRequestID           = 1;
    pMyObject->SessionState              = CCSP_CWMPSO_SESSION_STATE_idle;
    pMyObject->CpeMaxEnvelopes           = CCSP_CWMP_CPE_MAX_ENVELOPES;
    pMyObject->AcsMaxEnvelopes           = 1;
    pMyObject->ActiveResponses           = 0;
    pMyObject->bHoldRequests             = FALSE;
    pMyObject->bNoMoreRequests           = FALSE;
    pMyObject->bLastEmptyRequestSent     = FALSE;
    pMyObject->EventCount                = 0;
    pMyObject->RetryCount                = 0;
    pMyObject->ModifiedParamCount        = 0;

    pMyObject->GetCcspCwmpAcsConnection  = CcspCwmpsoGetCcspCwmpAcsConnection;
    pMyObject->GetCcspCwmpMcoIf          = CcspCwmpsoGetCcspCwmpMcoIf;
    pMyObject->GetCcspCwmpCpeController  = CcspCwmpsoGetCcspCwmpCpeController;
    pMyObject->SetCcspCwmpCpeController  = CcspCwmpsoSetCcspCwmpCpeController;
    pMyObject->GetCcspCwmpProcessor      = CcspCwmpsoGetCcspCwmpProcessor;
    pMyObject->SetCcspCwmpProcessor      = CcspCwmpsoSetCcspCwmpProcessor;
    pMyObject->Reset                     = CcspCwmpsoReset;

    pMyObject->AcqAccess                 = CcspCwmpsoAcqAccess;
    pMyObject->RelAccess                 = CcspCwmpsoRelAccess;
    pMyObject->SessionTimerInvoke        = CcspCwmpsoSessionTimerInvoke;
    pMyObject->SessionRetryTimerInvoke   = CcspCwmpsoSessionRetryTimerInvoke;
    pMyObject->DelayedActiveNotifTimerInvoke   
										 = CcspCwmpsoDelayedActiveNotifTimerInvoke;
    pMyObject->StartRetryTimer           = CcspCwmpsoStartRetryTimer;
    pMyObject->StopRetryTimer            = CcspCwmpsoStopRetryTimer;
    pMyObject->StopDelayedActiveNotifTimer 
							             = CcspCwmpsoStopDelayedActiveNotifTimer;

    pMyObject->IsAcsConnected            = CcspCwmpsoIsAcsConnected;
    pMyObject->ConnectToAcs              = CcspCwmpsoConnectToAcs;
    pMyObject->CloseConnection           = CcspCwmpsoCloseConnection;

    pMyObject->AddCwmpEvent              = CcspCwmpsoAddCwmpEvent;
    pMyObject->DiscardCwmpEvent          = CcspCwmpsoDiscardCwmpEvent;
    pMyObject->DelAllEvents              = CcspCwmpsoDelAllEvents;
    pMyObject->AddModifiedParameter      = CcspCwmpsoAddModifiedParameter;
    pMyObject->DelAllParameters          = CcspCwmpsoDelAllParameters;
    pMyObject->SaveCwmpEvent             = CcspCwmpsoSaveCwmpEvent;

    pMyObject->GetRpcMethods             = CcspCwmpsoGetRpcMethods;
    pMyObject->Inform                    = CcspCwmpsoInform;
    pMyObject->TransferComplete          = CcspCwmpsoTransferComplete;
    pMyObject->AutonomousTransferComplete= CcspCwmpsoAutonomousTransferComplete;
    pMyObject->Kicked                    = CcspCwmpsoKicked;
    pMyObject->RequestDownload           = CcspCwmpsoRequestDownload;
    pMyObject->DUStateChangeComplete     = CcspCwmpsoDUStateChangeComplete;
    pMyObject->AutonomousDUStateChangeComplete
                                         = CcspCwmpsoAutonomousDUStateChangeComplete;

    pMyObject->RecvSoapMessage           = CcspCwmpsoRecvSoapMessage;
    pMyObject->NotifySessionClosed       = CcspCwmpsoNotifySessionClosed;
    pMyObject->AsyncProcessTask          = CcspCwmpsoAsyncProcessTask;

    pMyObject->CancelRetryDelay          = CcspCwmpsoCancelRetryDelay;

    pMyObject->InformPending             = CcspCwmpsoInformPending;

    AnscInitializeLock       (&pMyObject->AccessLock       );
    AnscQueueInitializeHeader(&pMyObject->AsyncReqQueue    );
    AnscInitializeLock       (&pMyObject->AsyncReqQueueLock);
    AnscQueueInitializeHeader(&pMyObject->SavedReqQueue    );
    AnscInitializeLock       (&pMyObject->SavedReqQueueLock);
    AnscQueueInitializeHeader(&pMyObject->AsyncRepQueue    );
    AnscInitializeLock       (&pMyObject->AsyncRepQueueLock);
    AnscInitializeEvent      (&pMyObject->AsyncProcessEvent);
    AnscResetEvent           (&pMyObject->AsyncProcessEvent);

    return  ANSC_STATUS_SUCCESS;
}
