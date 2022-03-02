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

    module:	ccsp_cwmp_proco_management.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced management functions
        of the CCSP CWMP Processor Object.

        *   CcspCwmppoAcqWmpSession
        *   CcspCwmppoAcqWmpSession2
        *   CcspCwmppoAcqWmpSession3
        *   CcspCwmppoRelWmpSession
        *   CcspCwmppoDelWmpSession
        *   CcspCwmppoDelAllSessions
        *   CcspCwmppoGetActiveWmpSessionCount
        *   CcspCwmppoInitParamAttrCache
        *   CcspCwmppoPushAllVcToBackend
        *   CcspCwmppoUpdateParamAttrCache
        *   CcspCwmppoCheckParamAttrCache
        *   CcspCwmppoSyncNamespacesWithCR
        *   CcspCwmppoHasPendingInform

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/14/05    initial revision.
        07/20/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_proco_global.h"
#include "ccsp_tr069pa_mapper_api.h"
#define  CcspCwmppoMpaMapParamInstNumCwmpToDmInt(pParam)                        \
            /*CWMP_2_DM_INT_INSTANCE_NUMBER_MAPPING*/                           \
            {                                                                   \
                CCSP_STRING     pReturnStr  = NULL;                             \
                                                                                \
                CcspTr069PaTraceWarning(("%s - Param CWMP to DmInt\n", __FUNCTION__));\
                                                                                \
                pReturnStr =                                                    \
                    CcspTr069PA_MapInstNumCwmpToDmInt                           \
                        (                                                       \
                            pParam                                              \
                        );                                                      \
                                                                                \
                if ( pReturnStr )                                               \
                {                                                               \
                    /* Entries in pParamNameArray cannot be freed */            \
                    /* AnscFreeMemory(pParam); */                               \
                    pParam = pReturnStr;                                        \
                }                                                               \
            }
extern int g_flagToStartCWMP;
/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmppoAcqWmpSession
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to acquire the access to the active
        session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     async session.

**********************************************************************/

ANSC_HANDLE
CcspCwmppoAcqWmpSession
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject       = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT    )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = (PSINGLE_LINK_ENTRY          )NULL;

    AnscAcquireLock(&pMyObject->WmpsoQueueLock);

    pSLinkEntry = AnscQueueGetFirstEntry(&pMyObject->WmpsoQueue);

    if ( pSLinkEntry )
    {
        pCcspCwmpSession = ACCESS_CCSP_CWMP_SESSION_OBJECT(pSLinkEntry);

        pCcspCwmpSession->AcqAccess((ANSC_HANDLE)pCcspCwmpSession);
    }

    AnscReleaseLock(&pMyObject->WmpsoQueueLock);

    return  (ANSC_HANDLE)pCcspCwmpSession;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmppoAcqWmpSession2
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to acquire the access to the first
        inactive session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     async session.

**********************************************************************/

ANSC_HANDLE
CcspCwmppoAcqWmpSession2
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject       = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT    )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = (PSINGLE_LINK_ENTRY          )NULL;

    AnscAcquireLock(&pMyObject->WmpsoQueueLock);

    pSLinkEntry = AnscQueueGetFirstEntry(&pMyObject->WmpsoQueue);

    if ( pSLinkEntry )
    {
        pCcspCwmpSession = ACCESS_CCSP_CWMP_SESSION_OBJECT(pSLinkEntry);

        if ( (pCcspCwmpSession->SessionState != CCSP_CWMPSO_SESSION_STATE_idle      ) &&
             (pCcspCwmpSession->SessionState != CCSP_CWMPSO_SESSION_STATE_connectNow) )
        {
            pSLinkEntry = AnscQueueGetNextEntry(pSLinkEntry);

            if ( pSLinkEntry )
            {
                pCcspCwmpSession = ACCESS_CCSP_CWMP_SESSION_OBJECT(pSLinkEntry);

                pCcspCwmpSession->AcqAccess((ANSC_HANDLE)pCcspCwmpSession);
            }
            else
            {
                pCcspCwmpSession = NULL;
            }
        }
        else
        {
            pCcspCwmpSession->AcqAccess((ANSC_HANDLE)pCcspCwmpSession);
        }
    }

    if ( !pCcspCwmpSession )
    {
        pCcspCwmpSession =
            (PCCSP_CWMP_SESSION_OBJECT)CcspCwmpCreateWmpSession
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pCcspCwmpSession )
        {
            AnscReleaseLock(&pMyObject->WmpsoQueueLock);

            return  (ANSC_HANDLE)NULL;
        }
        else
        {
            pCcspCwmpSession->SetCcspCwmpCpeController((ANSC_HANDLE)pCcspCwmpSession, pMyObject->hCcspCwmpCpeController);
            pCcspCwmpSession->SetCcspCwmpProcessor ((ANSC_HANDLE)pCcspCwmpSession, (ANSC_HANDLE)pMyObject       );
        }

        AnscQueuePushEntry(&pMyObject->WmpsoQueue, &pCcspCwmpSession->Linkage);

        pCcspCwmpSession->AcqAccess((ANSC_HANDLE)pCcspCwmpSession);
    }

    AnscReleaseLock(&pMyObject->WmpsoQueueLock);

    return  (ANSC_HANDLE)pCcspCwmpSession;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmppoAcqWmpSession3
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to acquire the access to the first
        session that is either active or idle.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     async session.

**********************************************************************/

ANSC_HANDLE
CcspCwmppoAcqWmpSession3
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject       = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT    )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = (PSINGLE_LINK_ENTRY          )NULL;

    AnscAcquireLock(&pMyObject->WmpsoQueueLock);

    pSLinkEntry = AnscQueueGetFirstEntry(&pMyObject->WmpsoQueue);

    if ( pSLinkEntry )
    {
        pCcspCwmpSession = ACCESS_CCSP_CWMP_SESSION_OBJECT(pSLinkEntry);

        if ( pCcspCwmpSession->SessionState != CCSP_CWMPSO_SESSION_STATE_idle )
        {
            pSLinkEntry = AnscQueueGetNextEntry(pSLinkEntry);

            if ( pSLinkEntry )
            {
                pCcspCwmpSession = ACCESS_CCSP_CWMP_SESSION_OBJECT(pSLinkEntry);

                pCcspCwmpSession->AcqAccess((ANSC_HANDLE)pCcspCwmpSession);
            }
            else
            {
                pCcspCwmpSession = NULL;
            }
        }
        else
        {
            pCcspCwmpSession->AcqAccess((ANSC_HANDLE)pCcspCwmpSession);
        }
    }

    if ( !pCcspCwmpSession )
    {
        pCcspCwmpSession =
            (PCCSP_CWMP_SESSION_OBJECT)CcspCwmpCreateWmpSession
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pCcspCwmpSession )
        {
            AnscReleaseLock(&pMyObject->WmpsoQueueLock);

            return  (ANSC_HANDLE)NULL;
        }
        else
        {
            pCcspCwmpSession->SetCcspCwmpCpeController((ANSC_HANDLE)pCcspCwmpSession, pMyObject->hCcspCwmpCpeController);
            pCcspCwmpSession->SetCcspCwmpProcessor ((ANSC_HANDLE)pCcspCwmpSession, (ANSC_HANDLE)pMyObject       );
        }

        AnscQueuePushEntry(&pMyObject->WmpsoQueue, &pCcspCwmpSession->Linkage);

        pCcspCwmpSession->AcqAccess((ANSC_HANDLE)pCcspCwmpSession);
    }

    AnscReleaseLock(&pMyObject->WmpsoQueueLock);

    return  (ANSC_HANDLE)pCcspCwmpSession;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoRelWmpSession
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hWmpSession
            );

    description:

        This function is called to release the access to the active
        session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hWmpSession
                Specifies the session handle to be released.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoRelWmpSession
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT    )hWmpSession;

    if ( pCcspCwmpSession )
    {
        pCcspCwmpSession->RelAccess((ANSC_HANDLE)pCcspCwmpSession);
    }

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoDelWmpSession
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hWmpSession
            );

    description:

        This function is called to delete the specified session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hWmpSession
                Specifies the session handle to be deleted.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoDelWmpSession
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject       = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT    )hWmpSession;

    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_INVALID_HANDLE;
    }
    else
    {
        pCcspCwmpSession->RelAccess((ANSC_HANDLE)pCcspCwmpSession);
    }

    AnscAcquireLock(&pMyObject->WmpsoQueueLock);

    if ( AnscQueuePopEntryByLink(&pMyObject->WmpsoQueue, &pCcspCwmpSession->Linkage) )
    {
        pCcspCwmpSession->AcqAccess      ((ANSC_HANDLE)pCcspCwmpSession);
        pCcspCwmpSession->RelAccess      ((ANSC_HANDLE)pCcspCwmpSession);
        pCcspCwmpSession->CloseConnection((ANSC_HANDLE)pCcspCwmpSession);
        pCcspCwmpSession->Remove         ((ANSC_HANDLE)pCcspCwmpSession);
    }

    AnscReleaseLock(&pMyObject->WmpsoQueueLock);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoDelAllSessions
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to delete all sessions.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoDelAllSessions
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject       = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT    )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = (PSINGLE_LINK_ENTRY          )NULL;

    AnscAcquireLock(&pMyObject->WmpsoQueueLock);

    pSLinkEntry = AnscQueuePopEntry(&pMyObject->WmpsoQueue);

    while ( pSLinkEntry )
    {
        pCcspCwmpSession = ACCESS_CCSP_CWMP_SESSION_OBJECT(pSLinkEntry);
        pSLinkEntry     = AnscQueuePopEntry(&pMyObject->WmpsoQueue);

        pCcspCwmpSession->AcqAccess      ((ANSC_HANDLE)pCcspCwmpSession);
        pCcspCwmpSession->RelAccess      ((ANSC_HANDLE)pCcspCwmpSession);
        pCcspCwmpSession->CloseConnection((ANSC_HANDLE)pCcspCwmpSession);
        pCcspCwmpSession->Remove         ((ANSC_HANDLE)pCcspCwmpSession);
    }

    AnscReleaseLock(&pMyObject->WmpsoQueueLock);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmppoGetActiveWmpSessionCount
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to get the count of sessions.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     session count.

**********************************************************************/

ULONG
CcspCwmppoGetActiveWmpSessionCount
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bQueueLockAcquired
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject       = (PCCSP_CWMP_PROCESSOR_OBJECT)hThisObject;
    PCCSP_CWMP_SESSION_OBJECT       pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT  )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = (PSINGLE_LINK_ENTRY        )NULL;
    ULONG                           ulActiveSessNum = 0;

    if ( !bQueueLockAcquired )
    {
        AnscAcquireLock(&pMyObject->WmpsoQueueLock);
    }

    pSLinkEntry = AnscQueueGetFirstEntry(&pMyObject->WmpsoQueue);

    while ( pSLinkEntry )
    {
        pCcspCwmpSession = ACCESS_CCSP_CWMP_SESSION_OBJECT(pSLinkEntry);
        pSLinkEntry     = AnscQueueGetNextEntry(pSLinkEntry);

        if ( pCcspCwmpSession->SessionState != CCSP_CWMPSO_SESSION_STATE_idle )
        {
            ulActiveSessNum ++;
        }
    }

    if ( !bQueueLockAcquired )
    {
        AnscReleaseLock(&pMyObject->WmpsoQueueLock);
    }

    return  ulActiveSessNum;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoInitParamAttrCache
            (
                ANSC_HANDLE                 hThisObject
            )

    description:

        This function is called to initialize parameter attribute cache.
        All entries are read from PSM.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CcspCwmppoInitParamAttrCache
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PANSC_ATOM_TABLE_OBJECT         pParamAttrCache     = (PANSC_ATOM_TABLE_OBJECT    )pMyObject->hParamAttrCache;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    void*                           hMBusHandle         = (void*                      )pCcspCwmpCpeController->hMsgBusHandle;
    PANSC_ATOM_DESCRIPTOR           pAtomDescriptor;
    ULONG                           i;
    char                            keyPrefix[CCSP_BASE_PARAM_LENGTH];
    char                            key[CCSP_BASE_PARAM_LENGTH*2];
    int                             nCcspError          = CCSP_SUCCESS;
    ULONG                           ulNumRecords        = 0;
    CCSP_BASE_RECORD*               pRecords            = NULL;
    char*                           pParamName;

    pParamAttrCache->DelAllStrAtoms((ANSC_HANDLE)pParamAttrCache);

    /* assumption: PA name is supposed to be the same over reboot */
    _ansc_sprintf
        (
            keyPrefix, 
            "%s%s.%s.", 
            pCcspCwmpCpeController->SubsysName?pCcspCwmpCpeController->SubsysName:"",
            pCcspCwmpCpeController->PAName,
            CCSP_TR069PA_PARAM_NOTIF_ATTR
        );

    nCcspError = 
       PsmEnumRecords 
            (
                hMBusHandle,
                pCcspCwmpCpeController->SubsysName,
                keyPrefix,
                FALSE,          /* next-level is set to FALSE to enumerate all records */
                (unsigned int*)&ulNumRecords,
                &pRecords
            );

    if ( nCcspError != CCSP_SUCCESS )
    {
        returnStatus = ANSC_STATUS_INTERNAL_ERROR;

        CcspTr069PaTraceWarning
            ((
                "Failed to enumerate namespace notification under <%s>, error=%d\n", 
                keyPrefix,
                nCcspError
            ));
    }
    else
    {
        ULONG                       ulNotification;
        char*                       pValue  = NULL;

        for ( i = 0; i < ulNumRecords; i ++ )
        {
            if ( pRecords[i].RecordType == CCSP_BASE_INSTANCE ) 
            {
                continue;
            }

            pParamName = pRecords[i].Instance.Name;
            if ( !pParamName || *pParamName == 0 )
            {
                continue;
            }

            _ansc_sprintf(key, "%s%s", keyPrefix, pParamName);

            nCcspError = 
                PSM_Get_Record_Value2
                    (
                        hMBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        key,
                        NULL,
                        &pValue
                    );

            if ( nCcspError != CCSP_SUCCESS )
            {
                continue;
            }
            ulNotification = (ULONG)(pValue ? _ansc_atoi(pValue) : 0);

            if (pValue)
            {
                AnscFreeMemory(pValue);
                pValue = NULL;
            }

            /* create a cache entry */
            pAtomDescriptor = 
                (PANSC_ATOM_DESCRIPTOR)pParamAttrCache->AddAtomByName
                    (
                        (ANSC_HANDLE)pParamAttrCache,
                        pParamName,
                        0,
                        (ANSC_HANDLE)ulNotification
                    );

            if ( ! pAtomDescriptor )
            {
                CcspTr069PaTraceWarning
                    ((
                        "Failed to add parameter notification cache entry on namespace <%s> due to lack of resources\n", 
                        pParamName
                    ));
            }
        }
    }

    if ( pRecords )
    {
	g_flagToStartCWMP = 1;
        AnscFreeMemory(pRecords);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoCheckParamAttrCache
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to push VC subscription to FCs,
        this is not comforming to orignal CCSP specifications,
        just a workaround. It has side effect on MS FC since
        this FC takes care of it, while other FCs do not.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     parameter attribute, one of CCSP_CWMP_NOTIFICATION_xxx.

**********************************************************************/

#ifdef   _CCSP_TR069PA_PUSH_VC_SIGNAL_ON_LOAD

static
ANSC_STATUS
CcspCwmppoPushVcSubOnLoad
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        ULONG                       ulNotification
    )
{
    UNREFERENCED_PARAMETER(ulNotification);
    ANSC_STATUS                     returnStatus        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault      = NULL;

    returnStatus = 
        pCcspCwmpCpeController->MonitorOpState
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                TRUE,
                pParamName,
                0,
                NULL,
                NULL,
                &pCwmpSoapFault
            );

    if ( returnStatus != ANSC_STATUS_SUCCESS && pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

    return returnStatus;
}


static BOOL                         s_bPushingAllVcToBackend = FALSE;

ANSC_STATUS
CcspCwmppoPushAllVcToBackend
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject       = (PCCSP_CWMP_PROCESSOR_OBJECT)hThisObject;
    PANSC_ATOM_TABLE_OBJECT         pParamAttrCache = (PANSC_ATOM_TABLE_OBJECT   )pMyObject->hParamAttrCache;
    PANSC_ATOM_DESCRIPTOR           pAtomDescriptor = NULL;
    ULONG                           ulCount, i;

    s_bPushingAllVcToBackend = TRUE;

    /* due to back-end don't support saving VC singal subscription count into PSM
     * as CCSP specifications define, as an alternative, CCSP TR-069 PA will
     * push the subscription into back-end, at least this won't upset ACS */

    ulCount = pParamAttrCache->GetAtomCount2((ANSC_HANDLE)pParamAttrCache);

    for ( i = 0; i < ulCount; i ++ )
    {
        pAtomDescriptor = (PANSC_ATOM_DESCRIPTOR)pParamAttrCache->GetAtomByIndex2((ANSC_HANDLE)pParamAttrCache, i);

        if ( pAtomDescriptor )
        {
            returnStatus =
                CcspCwmppoPushVcSubOnLoad
                    (
                        hThisObject,
                        pAtomDescriptor->AtomName,
                        (ULONG)pAtomDescriptor->hContext
                    ); 

            if ( returnStatus != ANSC_STATUS_SUCCESS )
            {
                CcspTr069PaTraceWarning
                    ((
                        "Failed to subscribe VC signal on namespace <%s>, notif = %lu during CPE reloading!\n", 
                        pAtomDescriptor->AtomName,
                        (ULONG)pAtomDescriptor->hContext
                    ));
            }
            else
            {
                CcspTr069PaTraceWarning
                    ((
                        "VC signal on namespace <%s>, notif = %lu has pushed into backend during CPE reloading!\n", 
                        pAtomDescriptor->AtomName,
                        (ULONG)pAtomDescriptor->hContext
                    ));
            }
        }
    }

    s_bPushingAllVcToBackend = FALSE;

    return  ANSC_STATUS_SUCCESS;
}

#endif /* _CCSP_TR069PA_PUSH_VC_SIGNAL_ON_LOAD */


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoUpdateParamAttrCache
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hParamAttrArray,
                ULONG                       ulParamCount
            )

    description:

        This function is called to update parameter attribute cache.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hParamAttrArray
                Parameter attribute array

                ULONG                       ulParamCount
                Number of parameters

    return:     operation status.

**********************************************************************/

static ANSC_STATUS
CcspCwmppoUpdateSingleParamAttr
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        ULONG                       Notification
    )
{
    ANSC_STATUS                     returnStatus        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PANSC_ATOM_TABLE_OBJECT         pParamAttrCache     = (PANSC_ATOM_TABLE_OBJECT    )pMyObject->hParamAttrCache;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    void*                           hMBusHandle         = (void*                      )pCcspCwmpCpeController->hMsgBusHandle;
    PANSC_ATOM_DESCRIPTOR           pAtomDescriptor;
    char                            key[CCSP_BASE_PARAM_LENGTH*2];
    int                             nCcspError = CCSP_SUCCESS;

	CcspCwmppoMpaMapParamInstNumCwmpToDmInt(pParamName);
    _ansc_sprintf
        (
            key, 
            "%s%s.%s.%s", 
            pCcspCwmpCpeController->SubsysName?pCcspCwmpCpeController->SubsysName:"",
            pCcspCwmpCpeController->PAName,
            CCSP_TR069PA_PARAM_NOTIF_ATTR,
            pParamName
        );

    /* remove parameter notification attribute from PSM */
    pParamAttrCache->DelAtomByName
        (
            (ANSC_HANDLE)pParamAttrCache,
            pParamName
        );

    PSM_Del_Record
        (
            hMBusHandle,
            pCcspCwmpCpeController->SubsysName,
            key
        );

    if ( nCcspError != CCSP_SUCCESS )
    {
        CcspTr069PaTraceError
            ((
                "Failed to remove namespace notification on key <%s>, error=%d\n", 
                key,
                nCcspError
            ));

        returnStatus = ANSC_STATUS_INTERNAL_ERROR;
    }
    
    if ( Notification != CCSP_CWMP_NOTIFICATION_off )
    {
        pAtomDescriptor = 
            (PANSC_ATOM_DESCRIPTOR)pParamAttrCache->AddAtomByName
                (
                    (ANSC_HANDLE)pParamAttrCache,
                    pParamName,
                    0,
                    (ANSC_HANDLE)Notification
                );

        if ( !pAtomDescriptor )
        {
            returnStatus = ANSC_STATUS_RESOURCES;
        }
        else
        {
            char                    value[16];

            /* save parameter notification into from PSM */
            _ansc_sprintf(value, "%u", (unsigned int)Notification);

            nCcspError = 
                PSM_Set_Record_Value2
                    (
                        hMBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        key,
                        ccsp_unsignedInt,
                        value
                    );

            if ( nCcspError != CCSP_SUCCESS )
            {
                CcspTr069PaTraceError
                    ((
                        "Failed to save notification (%u) attribute with key <%s>, error=%d\n", 
                        (unsigned int)Notification,
                        key,
                        nCcspError
                    ));

                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
            }
        }
    }

    return  returnStatus;
}


ANSC_STATUS
CcspCwmppoUpdateParamAttrCache
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hParamAttrArray,
        ULONG                       ulParamCount
    )
{
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    ULONG                           i;
    PCCSP_CWMP_SET_PARAM_ATTRIB     pParamAttrArray = (PCCSP_CWMP_SET_PARAM_ATTRIB)hParamAttrArray;
    PCCSP_CWMP_SET_PARAM_ATTRIB     pSetParamAttr;

#ifdef   _CCSP_TR069PA_PUSH_VC_SIGNAL_ON_LOAD
    if ( s_bPushingAllVcToBackend )
    {
        return ANSC_STATUS_SUCCESS;
    }
#endif

    /* we use a separate cache for maintaining parameter attributes base on
     * assumptions that in field not many parameters will be set as
     * passive or active notification. If this assumption is not right,
     * we have to incorporate notification attribute into namespace cache.
     */

    for ( i = 0; i < ulParamCount; i ++ )
    {
        pSetParamAttr = &pParamAttrArray[i];

        if ( !pSetParamAttr->bNotificationChange )
        {
            continue;
        }

        returnStatus = 
            CcspCwmppoUpdateSingleParamAttr
                (
                    hThisObject,
                    pSetParamAttr->Name,
                    pSetParamAttr->Notification
                );

        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            return  returnStatus;
        }
    }

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmppoCheckParamAttrCache
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pParamName
            );

    description:

        This function is called to retrieve parameter attribute 
        from cache.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hParamAttrArray
                Parameter attribute array

                ULONG                       ulParamCount
                Number of parameters

    return:     parameter attribute, one of CCSP_CWMP_NOTIFICATION_xxx.

**********************************************************************/

static
char*
CcspCwmppoGetParentParamName
    (
        char*                       pParamName
    )
{
    char*                           pParentParamName = NULL;
    char*                           pLast            = NULL;

    if ( !pParamName ) return NULL;

    pParentParamName = AnscCloneString(pParamName);

    if ( pParentParamName )
    {
        pLast = pParentParamName + AnscSizeOfString(pParentParamName) - 1;

        if ( *pLast == '.' ) pLast --;

        while ( pLast >= pParentParamName && *pLast != '.'  ) pLast --;

        if ( pLast < pParentParamName )
        {
            AnscFreeMemory(pParentParamName);
            pParentParamName = NULL;
        }  
        else
        {
            *(pLast+1) = 0;
        }
    }

    return pParentParamName;
}


ULONG
CcspCwmppoCheckParamAttrCache
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject       = (PCCSP_CWMP_PROCESSOR_OBJECT)hThisObject;
    PANSC_ATOM_TABLE_OBJECT         pParamAttrCache = (PANSC_ATOM_TABLE_OBJECT   )pMyObject->hParamAttrCache;
    PANSC_ATOM_DESCRIPTOR           pAtomDescriptor = NULL;
    char*                           pCurName        = AnscCloneString(pParamName);
    char*                           pParentName     = NULL;

    while ( pCurName )
    {
        pAtomDescriptor = (PANSC_ATOM_DESCRIPTOR)pParamAttrCache->GetAtomByName((ANSC_HANDLE)pParamAttrCache, pCurName);

        if ( pAtomDescriptor )
        {
            AnscFreeMemory(pCurName);
            return  (ULONG)pAtomDescriptor->hContext;
        }

        pParentName = CcspCwmppoGetParentParamName(pCurName);
        AnscFreeMemory(pCurName);
        pCurName = pParentName;
    }

    return  CCSP_CWMP_NOTIFICATION_off;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSyncNamespacesWithCR
            (
                ANSC_HANDLE                 hThisObject,
                BOOL                        bRemoteOnly
            )

    description:

        This function is called to synchronize namespaces with
        Component Registrar (CR).

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                BOOL                        bRemoteOnly
                To indicate if PA synchronizes namespaces on
                remote sub-systems only.

    return:     operation status.

**********************************************************************/

CCSP_BOOL
CcspCwmppoSyncRemoteNamespace
    (
        CCSP_HANDLE                 pContext,
        PCCSP_TR069_PARAM_INFO      pParamInfo
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT )pContext;
    PCCSP_NAMESPACE_MGR_OBJECT      pCcspNsMgr          = (PCCSP_NAMESPACE_MGR_OBJECT )pMyObject->hCcspNamespaceMgr;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor   = (PCCSP_CWMP_PROCESSOR_OBJECT )pCcspCwmpCpeController->hCcspCwmpProcessor;
    void*                           hMBusHandle         = (void*                      )pCcspCwmpCpeController->hMsgBusHandle;
    const char*                     pCrName             = pCcspCwmpCpeController->GetCRName((ANSC_HANDLE)pCcspCwmpCpeController);
    componentStruct_t**             ppComp;
    CCSP_INT                        i, NumComp;
    CCSP_INT                        ret;
    char                            paramFullName[CCSP_TR069_RPC_Namespace_Length+1];
    char*                           pCrNameWithPrefix   = NULL;
    int                             nLen = (pCcspCwmpCpeController->SubsysName?AnscSizeOfString(pCcspCwmpCpeController->SubsysName):0) + AnscSizeOfString(pCrName) + 1;

    pCrNameWithPrefix = (char*)AnscAllocateMemory(nLen);

    if ( !pCrNameWithPrefix )
    {
        CcspTr069PaTraceError(("Out of resource while synchronizing namespace with remote CR!\n"));
        return  CCSP_FALSE;
    }

    _ansc_sprintf(pCrNameWithPrefix, "%s%s", pCcspCwmpCpeController->SubsysName ? pCcspCwmpCpeController->SubsysName:"", pCrName);

    ret = CcspTr069PA_GetPiFullName(pParamInfo, paramFullName, CCSP_TR069_RPC_Namespace_Length);
    if ( ret < 0 )
    {
        CcspTr069PaTraceError
            ((
                "CcspBaseIf_discComponentSupportingNamespace() failed on namespace whose last name <%s>, partial full name <%s> returns %d!\n",
                pParamInfo->Name,
                paramFullName,
                ret
            ));

        goto EXIT;
    }

    ret = 
        CcspBaseIf_discComponentSupportingNamespace
            (
                hMBusHandle,
                pCrNameWithPrefix,
                paramFullName,
                pParamInfo->Subsystem,
                &ppComp,
                &NumComp
            );

    if ( ret != CCSP_SUCCESS )
    {
        CcspTr069PaTraceError
            ((
                "CcspBaseIf_discComponentSupportingNamespace() failed on namespace <%s> returns %d!\n", 
                paramFullName,
                ret
            ));

        pCcspCwmpProcessor->bRemoteCRsSyncDone = FALSE;
    }
    else
    {
        name_spaceType_t            Ns;
        name_spaceType_t*           pNs = &Ns;
        char                        fn[CCSP_TR069_RPC_Namespace_Length+1] = {0};
        int                         ret;

        ret = CcspTr069PA_GetPiFullName(pParamInfo, fn, CCSP_TR069_RPC_Namespace_Length);

        Ns.name_space = fn;
        Ns.dataType   = ppComp[0]->type;

        for ( i = 0; i < NumComp; i ++ )
        {
            ret = 
                pCcspNsMgr->BuildNamespaces
                    (
                        (ANSC_HANDLE)pCcspNsMgr,
                        ppComp[i]->componentName,
                        ppComp[i]->dbusPath,
                        pParamInfo->Subsystem,
                        (PVOID*)&pNs,
                        1
                    );

            if ( ret != CCSP_SUCCESS )
            {
                CcspTr069PaTraceError
                    ((
                        "Failed to build namespace <%s> in cache on FC <%s>, error=%d\n", 
                        paramFullName,
                        ppComp[i]->componentName,
                        ret
                    ));
            }
        }
    
        free_componentStruct_t(hMBusHandle, NumComp, ppComp);
    }


EXIT:
    AnscFreeMemory(pCrNameWithPrefix);

    return CCSP_TRUE;
}


ANSC_STATUS
CcspCwmppoSyncNamespacesWithCR
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRemoteOnly
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PCCSP_NAMESPACE_MGR_OBJECT      pCcspNsMgr          = (PCCSP_NAMESPACE_MGR_OBJECT )pMyObject->hCcspNamespaceMgr;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    CCSP_HANDLE                     hCcspTr069Mapper    = (CCSP_HANDLE                )pCcspCwmpCpeController->hTr069PaMapper;
    void*                           hMBusHandle         = (void*                      )pCcspCwmpCpeController->hMsgBusHandle;
    int                             NumCRs              = 0;
    const char*                     pCrName             = NULL;
    char*                           pSubsystem          = NULL;
    char*                           pCrMBusPath         = NULL;
    int                             ret;
	registeredComponent_t**			ppComp				= NULL;
    int                             NumComp, i, j;
    name_spaceType_t**              pNsArray;
    int                             nNsArraySize;

    /*
     * This API builds namespace caches on all namespaces managed
     * by this PA. 
     *
     * Note that in case multiple sub-systems relate to PA, this API must be
     * call after all related sub-systems have started. Specifically,
     * when PA receives SystemReady signal from local CR, it means
     * all related sub-systems must be ready as well.
     *
     * Regarding error handling, the initial implementation will
     * not do recovery. Since we build cache at system bootup, 
     * assume traffic is not heavy and as long as synchronization 
     * does not take too long, we probably can assume this process
     * should be finished without problems. However if it turns out
     * otherwise, we may have to maintain a un-sync'ed namespace list
     * that we can build cache on missing namespaces in the runtime.
     */
    pMyObject->bRemoteCRsSyncDone = TRUE;

    pCrName = pCcspCwmpCpeController->GetCRName((ANSC_HANDLE)pCcspCwmpCpeController);

    if ( pCcspNsMgr )
    {
        pCcspNsMgr->CleanAll((ANSC_HANDLE)pCcspNsMgr);
    }
    else
    {
        pCcspNsMgr = 
            (PCCSP_NAMESPACE_MGR_OBJECT)CcspCreateNamespaceMgr
                (
                    pCcspCwmpCpeController->PANameWithPrefix
                );

        if ( !pCcspNsMgr )
        {
            return  ANSC_STATUS_RESOURCES;
        }

        /* set up sub-system prefix */
        if ( pCcspNsMgr->SubsysPrefix )
        {
            AnscFreeMemory(pCcspNsMgr->SubsysPrefix);
        }
        
        pCcspNsMgr->SubsysPrefix = AnscCloneString(pCcspCwmpCpeController->SubsysName);       
        pMyObject->hCcspNamespaceMgr = (ANSC_HANDLE)pCcspNsMgr;
    }

    /* sync namespaces with CRs that related to this PA
     * 1. retrieve all registered components
     * 2. retrieve all supported namespaces for each component
     */

    NumCRs = CcspTr069PA_GetSubsystemCount(hCcspTr069Mapper);

    for ( i = 0; i < NumCRs; i ++ )
    {
        pSubsystem  = (char*)CcspTr069PA_GetSubsystemByIndex(hCcspTr069Mapper, i);

        pCrMBusPath = (char*)pCcspCwmpCpeController->GetCRBusPath((ANSC_HANDLE)pCcspCwmpCpeController, pSubsystem);
        if ( !pCrMBusPath )
        {
            continue;
        }

/*
        if ( (!pCcspCwmpCpeController->SubsysName && pSubsystem) ||
             (pCcspCwmpCpeController->SubsysName && !pSubsystem) )
*/
        if ( !pCcspCwmpCpeController->SubsysName && pSubsystem )
        {
            continue;
        }

        if ( (!pSubsystem && !pCcspCwmpCpeController->SubsysName) ||
             (pCcspCwmpCpeController->SubsysName && !pSubsystem)  ||
             AnscEqualString(pCcspCwmpCpeController->SubsysName, pSubsystem, TRUE) )
        {
            BOOL                    bNoSubsys         = !pCcspCwmpCpeController->SubsysName;
            char*                   pCrNameWithPrefix = (char*)pCrName;
 
            if ( bRemoteOnly ) 
            {
                continue;
            }

            if ( !bNoSubsys )
            {
                int                 nLen = AnscSizeOfString(pCcspCwmpCpeController->SubsysName) + AnscSizeOfString(pCrName) + 1;

                pCrNameWithPrefix = (char*)AnscAllocateMemory(nLen);

                if ( !pCrNameWithPrefix )
                {
                    CcspTr069PaTraceError(("Out of resource while synchronizing namespace with local CR!\n"));
                    continue;
                }
            
                _ansc_sprintf(pCrNameWithPrefix, "%s%s", pCcspCwmpCpeController->SubsysName ? pCcspCwmpCpeController->SubsysName:"", pCrName);
            }

            /* this CR is on the same sub-system as PA is running on - full synchronization */
            ret = 
                CcspBaseIf_getRegisteredComponents
                    (
                        hMBusHandle,
                        pCrNameWithPrefix,
                        &ppComp,
                        &NumComp
                    );

            if ( ret != CCSP_SUCCESS )
            {
                CcspTr069PaTraceError(("CcspBaseIf_getRegisteredComponents() returns %d!\n", ret));
            }
            else
            {
                CcspTr069PaTraceDebug(("CcspCwmppoSyncNamespacesWithCR - CcspBaseIf_getRegisteredComponents() returns %d components\n", NumComp));

                for ( j = 0; j < NumComp; j ++ )
                {
                    /* go through each component and enumerate all supported namespaces */

                    ret = 
                        CcspBaseIf_discNamespaceSupportedByComponent
                            (
                                hMBusHandle,
                                pCrNameWithPrefix,
                                ppComp[j]->componentName,
                                &pNsArray,
                                &nNsArraySize
                            );

                    if ( ret != CCSP_SUCCESS )
                    {
                        CcspTr069PaTraceError
                            ((
                                "Failed to retrieve namespaces on FC <%s> from local CR, error=%d\n", 
                                ppComp[j]->componentName,
                                ret
                            ));
                    }
                    else
                    {
                        ret = 
                            pCcspNsMgr->BuildNamespaces
                                (
                                    (ANSC_HANDLE)pCcspNsMgr,
                                    ppComp[j]->componentName,
                                    ppComp[j]->dbusPath,
                                    pSubsystem,
                                    (PVOID*)pNsArray,
                                    nNsArraySize
                                );

                        if ( ret != CCSP_SUCCESS )
                        {
                            CcspTr069PaTraceError
                                ((
                                    "Failed to build namespaces in cache on FC <%s>, error=%d\n", 
                                    ppComp[j]->componentName,
                                    ret
                                ));
                        }

                        free_name_spaceType_t(hMBusHandle, nNsArraySize, pNsArray);
                    }
                }

                if ( !bNoSubsys && pCrNameWithPrefix )
                {
                    AnscFreeMemory(pCrNameWithPrefix);
                }

                free_registeredComponent_t(hMBusHandle, NumComp, ppComp);
            }
        }
        else
        {
            /* this CR is on different sub-system than PA is running on - only partial 
             * synchronization needed for those supported namespaces on the sub-system
             * CR is running.
             */

            /* go through supported namespaces on this sub-system and 
             * find out all owning FCs.
             */
            CcspTr069PA_TraverseRemotePiTree
                (
                    pCcspCwmpCpeController->hTr069PaMapper,
                    TRUE,
                    pSubsystem,
                    CcspCwmppoSyncRemoteNamespace,
                    (CCSP_HANDLE)pMyObject
                );
        }
    }

    pMyObject->LastRemoteCrSyncTime = AnscGetTickInSeconds();

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        CcspCwmppoHasPendingInform
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to check if there's a pending inform.
        It is only used INTERNALLY by CWMP state machine when a CWMP
        session is closed, and the state machine checks if there's
        any pending events to be sent to ACS immediately.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     TRUE if a new session needs to be created immediately,
                FALSE otherwise.

**********************************************************************/

BOOL
CcspCwmppoHasPendingInform
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject       = (PCCSP_CWMP_PROCESSOR_OBJECT)hThisObject;
    PCCSP_CWMP_SESSION_OBJECT       pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT  )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = (PSINGLE_LINK_ENTRY        )NULL;

    AnscAcquireLock(&pMyObject->WmpsoQueueLock);

    pSLinkEntry = AnscQueueGetFirstEntry(&pMyObject->WmpsoQueue);

    if ( pSLinkEntry )
    {
        pCcspCwmpSession = ACCESS_CCSP_CWMP_SESSION_OBJECT(pSLinkEntry);
    }

    AnscReleaseLock(&pMyObject->WmpsoQueueLock);

    if ( pCcspCwmpSession && pCcspCwmpSession->InformPending((ANSC_HANDLE)pCcspCwmpSession) )
    {
        return  TRUE;
    }

    return  FALSE;
}



