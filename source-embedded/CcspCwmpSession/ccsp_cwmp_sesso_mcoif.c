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

    module: ccsp_cwmp_sesso_mcoif.c

        For CCSP CWMP protocol implementation


    ---------------------------------------------------------------

    description:

        This module implements the advanced interface functions
        of the CcspCwmp Wmp Session Object.

        *   CcspCwmpsoMcoNotifyAcsStatus
        *   CcspCwmpsoMcoGetNextMethod
        *   CcspCwmpsoMcoProcessSoapResponse
        *   CcspCwmpsoMcoProcessSoapError
        *   CcspCwmpsoMcoInvokeUnknownMethod
        *   CcspCwmpsoMcoGetRpcMethods
        *   CcspCwmpsoMcoSetParameterValues
        *   CcspCwmpsoMcoGetParameterValues
        *   CcspCwmpsoMcoGetParameterNames
        *   CcspCwmpsoMcoSetParameterAttributes
        *   CcspCwmpsoMcoGetParameterAttributes
        *   CcspCwmpsoMcoAddObject
        *   CcspCwmpsoMcoDeleteObject
        *   CcspCwmpsoMcoDownload
        *   CcspCwmpsoMcoReboot
        *   CcspCwmpsoMcoGetQueuedTransfers
        *   CcspCwmpsoMcoScheduleInform
        *   CcspCwmpsoMcoSetVouchers
        *   CcspCwmpsoMcoGetOptions
        *   CcspCwmpsoMcoUpload
        *   CcspCwmpsoMcoFactoryReset
        *   CcspCwmpsoMcoGetParamDataType

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
        06/17/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/

#include "ccsp_cwmp_sesso_global.h"
#include "ccsp_cwmp_acsbo_interface.h"
#include "ccsp_cwmp_acsbo_exported_api.h"

#define  CWMP_MAXI_UPLOAD_DOWNLOAD_DELAY_SECONDS    3600 * 36       /* 36 hours */

/*
 * Define some const values that will be used in the object mapper object definition.
 */
#define  CCSP_CWMPDO_MAX_PARAM_VALUES_IN_RESPONSE   3000


ULONG    g_uMaxParamInResponse   = CCSP_CWMPDO_MAX_PARAM_VALUES_IN_RESPONSE;
#define TR069_QUIRKS

#define  CCSP_CWMP_TRACE_MAX_RESP_LENGTH            1024        

#ifdef DONT_HAVE_RM
void
CcspCwmpsoAsyncReboot();
#endif
/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoNotifyAcsStatus
            (
                ANSC_HANDLE                 hThisObject,
                BOOL                        bNoMoreRequests,
                BOOL                        bHoldRequests
            );

    description:

        This function is called to retrieve the next method in queue
        that is waiting for response.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoNotifyAcsStatus
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bNoMoreRequests,
        BOOL                        bHoldRequests
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;

    if ( !pMyObject->bNoMoreRequests )
    {
        pMyObject->bNoMoreRequests = bNoMoreRequests;
    }

    pMyObject->bHoldRequests = bHoldRequests;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmpsoMcoGetNextMethod
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID
            );

    description:

        This function is called to retrieve the next method in queue
        that is waiting for response.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     method type.

**********************************************************************/

ULONG
CcspCwmpsoMcoGetNextMethod
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq     = (PCCSP_CWMPSO_ASYNC_REQUEST  )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry        = (PSINGLE_LINK_ENTRY         )NULL;
    ULONG                           ulMethodType       = CCSP_CWMP_METHOD_Unknown;
    errno_t rc  = -1;
    int ind = -1;

    AnscAcquireLock(&pMyObject->SavedReqQueueLock);

    if ( pRequestID )
    {
        pSLinkEntry = AnscQueueGetFirstEntry(&pMyObject->SavedReqQueue);

        while ( pSLinkEntry )
        {
            pWmpsoAsyncReq = ACCESS_CCSP_CWMPSO_ASYNC_REQUEST(pSLinkEntry);
            pSLinkEntry    = AnscQueueGetNextEntry(pSLinkEntry);
            rc = strcmp_s(pRequestID, strlen(pRequestID), pWmpsoAsyncReq->RequestID, &ind);
            ERR_CHK(rc);
            if((!ind) && (rc == EOK))
            {
                ulMethodType = pWmpsoAsyncReq->Method;

                break;
            }
        }
    }
    else
    {
        pSLinkEntry = AnscQueueGetFirstEntry(&pMyObject->SavedReqQueue);

        if ( pSLinkEntry )
        {
            pWmpsoAsyncReq = ACCESS_CCSP_CWMPSO_ASYNC_REQUEST(pSLinkEntry);

            ulMethodType = pWmpsoAsyncReq->Method;
        }
    }

    AnscReleaseLock(&pMyObject->SavedReqQueueLock);

    return  ulMethodType;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoProcessSoapResponse
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hSoapResponse
            );

    description:

        This function is called to process a SOAP response envelope.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hSoapResponse
                Specifies the response envelope to be processed.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoProcessSoapResponse
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSoapResponse
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq     = (PCCSP_CWMPSO_ASYNC_REQUEST  )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry        = (PSINGLE_LINK_ENTRY         )NULL;
    PCCSP_CWMP_SOAP_RESPONSE        pSoapResponse      = (PCCSP_CWMP_SOAP_RESPONSE   )hSoapResponse;
    BOOL                            bIDMatched         = FALSE;
    ULONG                           faultCode          = 0;
    errno_t rc  = -1;
    int ind = -1;

    AnscAcquireLock(&pMyObject->SavedReqQueueLock);

    if ( pSoapResponse->Header.ID )
    {
        pSLinkEntry = AnscQueueGetFirstEntry(&pMyObject->SavedReqQueue);

        while ( pSLinkEntry )
        {
            pWmpsoAsyncReq = ACCESS_CCSP_CWMPSO_ASYNC_REQUEST(pSLinkEntry);
            pSLinkEntry    = AnscQueueGetNextEntry(pSLinkEntry);
            rc = strcmp_s(pSoapResponse->Header.ID, strlen(pSoapResponse->Header.ID), pWmpsoAsyncReq->RequestID, &ind);
            ERR_CHK(rc);
             if((!ind) && (rc == EOK))
            {
                bIDMatched = TRUE;
                AnscQueuePopEntryByLink(&pMyObject->SavedReqQueue, &pWmpsoAsyncReq->Linkage);

                /* We need to move the request back to the queue. Otherwise the session will be disconnected */
                if( pSoapResponse->Fault != NULL)
                {
                    AnscAcquireLock          (&pMyObject->AsyncReqQueueLock);
                    AnscQueuePushEntryAtFront(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
                    AnscReleaseLock          (&pMyObject->AsyncReqQueueLock);
                }

                break;

            }
        }

        if ( !bIDMatched )
        {
            if( pSoapResponse )
            {   
                CcspCwmpFreeSoapResponse(pSoapResponse);
                pSoapResponse = NULL;
            }

            AnscReleaseLock(&pMyObject->SavedReqQueueLock);

            return  ANSC_STATUS_INTERNAL_ERROR;
        }
    }
    else
    {
        pSLinkEntry = AnscQueuePopEntry(&pMyObject->SavedReqQueue);

        if ( !pSLinkEntry )
        {
            if( pSoapResponse )
            {   
                CcspCwmpFreeSoapResponse(pSoapResponse);
                pSoapResponse = NULL;
            }

            AnscReleaseLock(&pMyObject->SavedReqQueueLock);

            return  ANSC_STATUS_INTERNAL_ERROR;
        }
        else
        {
            pWmpsoAsyncReq = ACCESS_CCSP_CWMPSO_ASYNC_REQUEST(pSLinkEntry);

            /* We need to move the request back to the queue. Otherwise the session will be disconnected */
            if( pSoapResponse->Fault != NULL)
            {
                AnscAcquireLock          (&pMyObject->AsyncReqQueueLock);
                AnscQueuePushEntryAtFront(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
                AnscReleaseLock          (&pMyObject->AsyncReqQueueLock);
            }
        }
    }

    AnscReleaseLock(&pMyObject->SavedReqQueueLock);

    if( pSoapResponse->Fault)
    {
        faultCode = pSoapResponse->Fault->Fault.FaultCode;

        if( pSoapResponse )
        {
            CcspCwmpFreeSoapResponse(pSoapResponse);
            pSoapResponse = NULL;
        }

        if( faultCode != 8005)
        {
            /* We consider it as a failure. The retry timer will start */
            return ANSC_STATUS_FAILURE;
        }
        else
        {
            /* We will retry without delay in the same session */
            return ANSC_STATUS_SUCCESS;
        }
    }
    else
    {
        pWmpsoAsyncReq->hSoapResponse = (ANSC_HANDLE)pSoapResponse;

        CcspCwmpsoSignalAsyncRequest(pWmpsoAsyncReq, ANSC_STATUS_SUCCESS);
    }

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoProcessSoapError
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                char*                       pMethodName,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to process a SOAP error encoutered by
        the Soap Parser.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                char*                       pMethodName
                Specifies the method name of the request.

                ANSC_HANDLE                 hSoapFault
                Specifies the SOAP fault to be reported.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoProcessSoapError
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pMethodName,
        ANSC_HANDLE                 hSoapFault
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )hSoapFault;

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_Unknown;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_Error
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    pMethodName,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT1;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT2:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoInvokeUnknownMethod
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                char*                       pMethodName
            );

    description:

        This function is called when an unknown method is called by the
        ACS server.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                char*                       pMethodName
                Specifies the unrecognized method name.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoInvokeUnknownMethod
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pMethodName
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;

    pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

    if ( !pCwmpSoapFault )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }
    else
    {
        CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_Unknown;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_Error
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    pMethodName,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoGetRpcMethods
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID
            );

    description:

        This function is called to discover the set of methods
        supported by the CPE.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoGetRpcMethods
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf         = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpCfgIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    SLAP_STRING_ARRAY*              pMethodStringArray = (SLAP_STRING_ARRAY*         )NULL;
    ULONG                           ulNoRPCMethodMask  = 0;
    ULONG                           ulMethodCount      = 17;
    ULONG                           ulIndex            = 0;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NoRPCMethods )
    {
        ulNoRPCMethodMask = pCcspCwmpCfgIf->NoRPCMethods(pCcspCwmpCfgIf->hOwnerContext);

        if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_ChangeDUState )
        {
            ulMethodCount --;
        }

        if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_GetQueuedTransfers )
        {
            ulMethodCount --;
        }

        if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_Upload )
        {
            ulMethodCount --;
        }

        if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_SetVouchers )
        {
            ulMethodCount --;
        }

        if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_GetOptions )
        {
            ulMethodCount --;
        }

        if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_ScheduleInform )
        {
            ulMethodCount --;
        }

        if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_FactoryReset )
        {
            ulMethodCount --;
        }
    }

    if ( ulMethodCount == 0 )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    /*
     * This method may be used by a CPE or Server to discover the set of methods supported by the
     * Server or CPE it is in communication with. This list may include both standard methods and
     * vendor-specific methods, The receiver of the response MUST ignore any unrecognized methods.
     */
    SlapAllocStringArray2(ulMethodCount, pMethodStringArray);

    if ( !pMethodStringArray )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }
    else
    {
        /* required methods */
        pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("GetRPCMethods"         );
        pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("SetParameterValues"    );
        pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("GetParameterValues"    );
        pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("GetParameterNames"     );
        pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("SetParameterAttributes");
        pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("GetParameterAttributes");
        pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("AddObject"             );
        pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("DeleteObject"          );
        pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("Download"              );
        pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("Reboot"                );

        /* optional methods */
        if ( !(ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_ChangeDUState) )
        {
            pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("ChangeDUState");
        }

        if ( !(ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_GetQueuedTransfers) )
        {
            pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("GetQueuedTransfers");
        }

        if ( !(ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_ScheduleInform) )
        {
            pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("ScheduleInform"    );
        }

        if ( !(ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_SetVouchers) )
        {
            pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("SetVouchers"       );
        }

        if ( !(ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_GetOptions) )
        {
            pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("GetOptions"        );
        }

        if ( !(ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_Upload) )
        {
            pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("Upload"            );
        }

        if ( !(ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_FactoryReset) )
        {
            pMethodStringArray->Array.arrayString[ulIndex++] = CcspTr069PaCloneString("FactoryReset"   );
        }
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_GetRPCMethods;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_GetRpcMethods
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    pMethodStringArray
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pMethodStringArray )
    {
        SlapFreeVarArray(pMethodStringArray);
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoSetParameterValues
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hParamValueArray,
                ULONG                       ulArraySize,
                char*                       pParamKey
            );

    description:

        This function is called to modify the value of one or more CPE
        parameters.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                ANSC_HANDLE                 hParamValueArray
                Specifies the array of name-value pairs. CPE needs to
                set the parameter specified by the name to the
                corresponding value.

                ULONG                       ulArraySize
                Specifies the number of name-value pairs in the
                'hParamValueArray' argument.

                char*                       pParamKey
                Specifies the value to set the ParameterKey parameter.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoSetParameterValues
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hParamValueArray,
        ULONG                       ulArraySize,
        char*                       pParamKey
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    int                             iStatus            = 0;
    char*                           pRootObjName       = pCcspCwmpCpeController->GetRootObject((ANSC_HANDLE)pCcspCwmpCpeController);
    char                            paramName[128];

    /*
     *  Record the time receiving SPV request 
     */
    AnscGetLocalTime(&pCcspCwmpCpeController->cwmpStats.LastReceivedSPVTime); 

    returnStatus =
        pCcspCwmpMpaIf->SetParameterValues
            (
                pCcspCwmpMpaIf->hOwnerContext,
                hParamValueArray,
                ulArraySize,
                &iStatus,
                (ANSC_HANDLE*)&pCwmpSoapFault,
                TRUE
            );

    if ( (returnStatus != ANSC_STATUS_SUCCESS) && !pCwmpSoapFault )
    {
        /*
         * Something serious has screwed up (e.g. we have run out of system memory), abort!
         */
        goto  EXIT1;
    }
    else if( returnStatus == ANSC_STATUS_SUCCESS)
    {
        _ansc_sprintf(paramName, "%s%s", pRootObjName, "ManagementServer.ParameterKey");

        returnStatus =
            pCcspCwmpCpeController->SetParameterKey
                (
                    (ANSC_HANDLE)pCcspCwmpCpeController,
                    pParamKey
                );
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_SetParameterValues;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_SetParameterValues
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    iStatus,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoGetParameterValues
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                SLAP_STRING_ARRAY*          pParamNameArray
            );

    description:

        This function is called to obtain the value of one or more CPE
        parameters.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                SLAP_STRING_ARRAY*          pParamNameArray
                Specifies the array of strings, each representing the
                name of a requested parameter.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoGetParameterValues
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        SLAP_STRING_ARRAY*          pParamNameArray
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_PARAM_VALUE          pParamValueArray   = (PCCSP_CWMP_PARAM_VALUE     )NULL;
    ULONG                           ulArraySize        = (ULONG                      )0;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ULONG                           i                  = 0;

    returnStatus =
        pCcspCwmpMpaIf->GetParameterValues
            (
                pCcspCwmpMpaIf->hOwnerContext,
                pParamNameArray,
                g_uMaxParamInResponse,
                (void**)&pParamValueArray,
                &ulArraySize,
                (ANSC_HANDLE*)&pCwmpSoapFault,
                TRUE
            );

    if ( (returnStatus != ANSC_STATUS_SUCCESS) && !pCwmpSoapFault )
    {
        /*
         * Something serious has screwed up (e.g. we have run out of system memory), abort!
         */
        goto  EXIT1;
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_GetParameterValues;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_GetParameterValues
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)pParamValueArray,
                    (ULONG      )ulArraySize,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

#ifdef   _DEBUG
    if ( AnscSizeOfString(pWmpsoAsyncRep->SoapEnvelope) <= CCSP_CWMP_TRACE_MAX_RESP_LENGTH )
    {
        CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));
    }
    else
    {
        char                        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+1+8];

        AnscCopyMemory(partResp, pWmpsoAsyncRep->SoapEnvelope, CCSP_CWMP_TRACE_MAX_RESP_LENGTH);
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH] = '\n';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+1] = '.';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+2] = '.';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+3] = '.';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+4] = '\n';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+5] = 0;
        CcspTr069PaTraceDebug(("CPE Response:\n%s\n", partResp));
    }
#endif

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pParamValueArray )
    {
        for ( i = 0; i < ulArraySize; i++ )
        {
            CcspCwmpCleanParamValue((&pParamValueArray[i]));
        }

        CcspTr069PaFreeMemory(pParamValueArray);
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoGetParameterNames
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                char*                       pParamPath,
                BOOL                        bNextLevel
            );

    description:

        This function is called to discover the parameters accessible
        on a particular CPE.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                char*                       pParamPath
                Specifies a string containing either a complete
                parameter name, or a partial path name representing a
                subset of the name hierarchy.

                BOOL                        bNextLevel
                If false, the response lists the full path name of all
                parameters whose name begins with the string given by
                the 'pParamPath' argument. If true, the response lists
                only the partial path one level below the specified
                'pParamPath'.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoGetParameterNames
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pParamPath,
        BOOL                        bNextLevel
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_PARAM_INFO           pParamInfoArray    = (PCCSP_CWMP_PARAM_INFO      )NULL;
    ULONG                           ulArraySize        = (ULONG                      )0;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ULONG                           i                  = 0;

    returnStatus =
        pCcspCwmpMpaIf->GetParameterNames
            (
                pCcspCwmpMpaIf->hOwnerContext,
                pParamPath,
                bNextLevel,
                (void**)&pParamInfoArray,
                &ulArraySize,
                (ANSC_HANDLE*)&pCwmpSoapFault,
                TRUE
            );

    if ( (returnStatus != ANSC_STATUS_SUCCESS) && !pCwmpSoapFault )
    {
        /*
         * Something serious has screwed up (e.g. we have run out of system memory), abort!
         */
        goto  EXIT1;
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_GetParameterNames;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_GetParameterNames
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)pParamInfoArray,
                    (ULONG      )ulArraySize,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

#ifdef   _DEBUG
    if ( AnscSizeOfString(pWmpsoAsyncRep->SoapEnvelope) <= CCSP_CWMP_TRACE_MAX_RESP_LENGTH )
    {
        CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));
    }
    else
    {
        char                        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+1+8];

        AnscCopyMemory(partResp, pWmpsoAsyncRep->SoapEnvelope, CCSP_CWMP_TRACE_MAX_RESP_LENGTH);
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH] = '\n';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+1] = '.';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+2] = '.';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+3] = '.';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+4] = '\n';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+5] = 0;
        CcspTr069PaTraceDebug(("CPE Response:\n%s\n", partResp));
    }
#endif

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pParamInfoArray )
    {
        for ( i = 0; i < ulArraySize; i++ )
        {
            CcspCwmpCleanParamInfo((&pParamInfoArray[i]));
        }

        CcspTr069PaFreeMemory(pParamInfoArray);
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoSetParameterAttributes
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hSetParamAttribArray,
                ULONG                       ulArraySize
            );

    description:

        This function is called to modify attributes associated with
        one or more CPE parameters.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                ANSC_HANDLE                 hSetParamAttribArray
                Specifies a list of changes to be made to the
                attributes for a set of parameters.

                ULONG                       ulArraySize
                Specifies the number of attributes to be set.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoSetParameterAttributes
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSetParamAttribArray,
        ULONG                       ulArraySize
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;

    returnStatus =
        pCcspCwmpMpaIf->SetParameterAttributes
            (
                pCcspCwmpMpaIf->hOwnerContext,
                hSetParamAttribArray,
                ulArraySize,
                (ANSC_HANDLE*)&pCwmpSoapFault,
                TRUE
            );

    if ( (returnStatus != ANSC_STATUS_SUCCESS) && !pCwmpSoapFault )
    {
        /*
         * Something serious has screwed up (e.g. we have run out of system memory), abort!
         */
        goto  EXIT1;
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_SetParameterAttributes;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_SetParameterAttributes
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoGetParameterAttributes
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                SLAP_STRING_ARRAY*          pParamNameArray
            );

    description:

        This function is called to read the attributes associated with
        one or more CPE parameters.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                SLAP_STRING_ARRAY*          pParamNameArray
                Specifies an array of strings, each representing the
                name of a requested paramter.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoGetParameterAttributes
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        SLAP_STRING_ARRAY*          pParamNameArray
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_PARAM_ATTRIB         pParamAttribArray  = (PCCSP_CWMP_PARAM_ATTRIB    )NULL;
    ULONG                           ulArraySize        = (ULONG                      )0;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ULONG                           i                  = 0;

    returnStatus =
        pCcspCwmpMpaIf->GetParameterAttributes
            (
                pCcspCwmpMpaIf->hOwnerContext,
                pParamNameArray,
                g_uMaxParamInResponse,
                (void**)&pParamAttribArray,
                &ulArraySize,
                (ANSC_HANDLE*)&pCwmpSoapFault,
                TRUE
            );

    if ( (returnStatus != ANSC_STATUS_SUCCESS) && !pCwmpSoapFault )
    {
        /*
         * Something serious has screwed up (e.g. we have run out of system memory), abort!
         */
        goto  EXIT1;
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_GetParameterAttributes;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_GetParameterAttributes
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)pParamAttribArray,
                    (ULONG      )ulArraySize,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

#ifdef   _DEBUG
    if ( AnscSizeOfString(pWmpsoAsyncRep->SoapEnvelope) <= CCSP_CWMP_TRACE_MAX_RESP_LENGTH )
    {
        CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));
    }
    else
    {
        char                        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+1+8];

        AnscCopyMemory(partResp, pWmpsoAsyncRep->SoapEnvelope, CCSP_CWMP_TRACE_MAX_RESP_LENGTH);
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH] = '\n';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+1] = '.';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+2] = '.';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+3] = '.';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+4] = '\n';
        partResp[CCSP_CWMP_TRACE_MAX_RESP_LENGTH+5] = 0;
        CcspTr069PaTraceDebug(("CPE Response:\n%s\n", partResp));
    }
#endif


    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
    }

EXIT2:

    if ( pParamAttribArray )
    {
        for ( i = 0; i < ulArraySize; i++ )
        {
            CcspCwmpCleanParamAttrib((&pParamAttribArray[i]));
        }

        CcspTr069PaFreeMemory(pParamAttribArray);
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoAddObject
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                char*                       pObjectName,
                char*                       pParamKey
            );

    description:

        This function is called to create a new instance of a multi-
        instance object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                char*                       pObjectName
                Specifies the path name of the collection of objects
                for which a new instance is to be created. The path
                name MUST end with a "." after the last node in the
                hierarchical name of the object.

                char*                       pParamKey
                Specifies the value to set the ParameterKey parameter.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoAddObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pObjectName,
        char*                       pParamKey
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf         = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpCfgIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ULONG                           ulInstanceNumber   = 0;
    int                             iStatus            = 0;
    ULONG                           ulNoRPCMethodMask  = 0;
    char*                           pRootObjName       = pCcspCwmpCpeController->GetRootObject((ANSC_HANDLE)pCcspCwmpCpeController);
    char                            paramName[128];

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NoRPCMethods )
    {
        ulNoRPCMethodMask = pCcspCwmpCfgIf->NoRPCMethods(pCcspCwmpCfgIf->hOwnerContext);
    }

    if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_AddObject )
    {
        pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

        if ( !pCwmpSoapFault )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            goto  EXIT1;
        }
        else
        {
            CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
        }
    }
    else
    {
        returnStatus =
            pCcspCwmpMpaIf->AddObject
                (
                    pCcspCwmpMpaIf->hOwnerContext,
                    pObjectName,
                    &ulInstanceNumber,
                    &iStatus,
                    (ANSC_HANDLE*)&pCwmpSoapFault,
                    TRUE
                );
    }

    if ( (returnStatus != ANSC_STATUS_SUCCESS) && !pCwmpSoapFault )
    {
        /*
         * Something serious has screwed up (e.g. we have run out of system memory), abort!
         */
        goto  EXIT1;
    }
    else if( returnStatus == ANSC_STATUS_SUCCESS)
    {
        _ansc_sprintf(paramName, "%s%s", pRootObjName, "ManagementServer.ParameterKey");

        returnStatus =
            pCcspCwmpCpeController->SetParameterKey
                (
                    (ANSC_HANDLE)pCcspCwmpCpeController,
                    pParamKey
                );
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_AddObject;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_AddObject
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    ulInstanceNumber,
                    iStatus,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoDeleteObject
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                char*                       pObjectName,
                char*                       pParamKey
            );

    description:

        This function is called to remove a particular instance of an
        object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                char*                       pObjectName
                Specifies the path name of the object instance to be
                removed. The path name MUST end with a "." after the
                instance number of the object.

                char*                       pParamKey
                Specifies the value to set the ParameterKey parameter.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoDeleteObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pObjectName,
        char*                       pParamKey
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf         = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpCfgIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    int                             iStatus            = 0;
    ULONG                           ulNoRPCMethodMask  = 0;
    char*                           pRootObjName       = pCcspCwmpCpeController->GetRootObject((ANSC_HANDLE)pCcspCwmpCpeController);
    char                            paramName[128];

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NoRPCMethods )
    {
        ulNoRPCMethodMask = pCcspCwmpCfgIf->NoRPCMethods(pCcspCwmpCfgIf->hOwnerContext);
    }

    if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_DeleteObject )
    {
        pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

        if ( !pCwmpSoapFault )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            goto  EXIT1;
        }
        else
        {
            CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
        }
    }
    else
    {
        returnStatus =
            pCcspCwmpMpaIf->DeleteObject
                (
                    pCcspCwmpMpaIf->hOwnerContext,
                    pObjectName,
                    &iStatus,
                    (ANSC_HANDLE*)&pCwmpSoapFault,
                    TRUE
                );
    }

    if ( (returnStatus != ANSC_STATUS_SUCCESS) && !pCwmpSoapFault )
    {
        /*
         * Something serious has screwed up (e.g. we have run out of system memory), abort!
         */
        goto  EXIT1;
    }
    else if( returnStatus == ANSC_STATUS_SUCCESS)
    {
        _ansc_sprintf(paramName, "%s%s", pRootObjName, "ManagementServer.ParameterKey");

        returnStatus =
            pCcspCwmpCpeController->SetParameterKey
                (
                    (ANSC_HANDLE)pCcspCwmpCpeController,
                    pParamKey
                );
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_DeleteObject;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_DeleteObject
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    iStatus,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoDownload
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hDownloadReq
            );

    description:

        This function is called to cause the CPE to download a
        specified file from the designated location.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                ANSC_HANDLE                 hDownloadReq
                Specifies the parameters used for file downloading.

    return:     status of operation.

**********************************************************************/

#define  CcspCwmpsoMcoConstructDownloadArgName(arg)                                      \
    do {                                                                                \
        _ansc_sprintf                                                                   \
            (                                                                           \
                buf,                                                                    \
                "%s%s",                                                                 \
                CCSP_NS_DOWNLOAD,                                                       \
                arg                                                                     \
            );                                                                          \
        pParamValueArray[i].Name = CcspTr069PaCloneString(buf);                         \
    } while (0) 



static
ANSC_STATUS
CcspCwmpsoMcoDownload_PrepareArgs
    (
        PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController,
        PCCSP_CWMP_MCO_DOWNLOAD_REQ      pDownloadReq,
        PCCSP_CWMP_PARAM_VALUE*     ppParamValueArray,
        PULONG                      pulArraySize
    )
{
    UNREFERENCED_PARAMETER(pCcspCwmpCpeController);
    ANSC_STATUS                     returnStatus        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PARAM_VALUE          pParamValueArray    = NULL;
    int                             i                   = 0;
    PSLAP_VARIABLE                  pSlapVar;
    char                            buf[512];

    *ppParamValueArray  = NULL;
    *pulArraySize       = 0;

    pParamValueArray = 
        (PCCSP_CWMP_PARAM_VALUE)CcspTr069PaAllocateMemory
            (
                sizeof(CCSP_CWMP_PARAM_VALUE) * CCSP_NS_DOWNLOAD_ARG_MAX_COUNT
            );

    if ( !pParamValueArray )
    {
        return  ANSC_STATUS_RESOURCES;
    }
    else
    {
        AnscZeroMemory
            (
                pParamValueArray, 
                CCSP_NS_DOWNLOAD_ARG_MAX_COUNT * sizeof(CCSP_CWMP_PARAM_VALUE)
            );
    }

    /* CommandKey */
    SlapAllocVariable(pSlapVar);
    if ( !pSlapVar ) goto EXIT1;

    CcspCwmpsoMcoConstructDownloadArgName(CCSP_NS_DOWNLOAD_COMMAND_KEY);
    pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
    pParamValueArray[i].Value           = pSlapVar;

    pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
    pSlapVar->Variant.varString = CcspTr069PaCloneString(pDownloadReq->CommandKey);

    i++;

    /* FileType */
    SlapAllocVariable(pSlapVar);
    if ( !pSlapVar ) goto EXIT1;

    CcspCwmpsoMcoConstructDownloadArgName(CCSP_NS_DOWNLOAD_FILE_TYPE);
    pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
    pParamValueArray[i].Value           = pSlapVar;

    pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
    pSlapVar->Variant.varString = CcspTr069PaCloneString(pDownloadReq->FileType);

    i++;

    /* URL */
    SlapAllocVariable(pSlapVar);
    if ( !pSlapVar ) goto EXIT1;

    CcspCwmpsoMcoConstructDownloadArgName(CCSP_NS_DOWNLOAD_URL);
    pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
    pParamValueArray[i].Value           = pSlapVar;

    pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
    pSlapVar->Variant.varString = CcspTr069PaCloneString(pDownloadReq->Url);

    i++;

    /* Username */
    SlapAllocVariable(pSlapVar);
    if ( !pSlapVar ) goto EXIT1;

    CcspCwmpsoMcoConstructDownloadArgName(CCSP_NS_DOWNLOAD_USERNAME);
    pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
    pParamValueArray[i].Value           = pSlapVar;

    pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
    pSlapVar->Variant.varString = CcspTr069PaCloneString(pDownloadReq->Username);

    i++;

    /* Password */
    SlapAllocVariable(pSlapVar);
    if ( !pSlapVar ) goto EXIT1;

    CcspCwmpsoMcoConstructDownloadArgName(CCSP_NS_DOWNLOAD_PASSWORD);
    pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
    pParamValueArray[i].Value           = pSlapVar;

    pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
    pSlapVar->Variant.varString = CcspTr069PaCloneString(pDownloadReq->Password);

    i++;

    /* FileSize */
    SlapAllocVariable(pSlapVar);

    CcspCwmpsoMcoConstructDownloadArgName(CCSP_NS_DOWNLOAD_FILE_SIZE);
    pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_UnsignedInt;
    pParamValueArray[i].Value           = pSlapVar;

    _ansc_sprintf(buf, "%u", (unsigned int)pDownloadReq->FileSize);

    pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
    pSlapVar->Variant.varString = CcspTr069PaCloneString(buf);

    i++;

    /* TargetFileName */
    SlapAllocVariable(pSlapVar);
    if ( !pSlapVar ) goto EXIT1;

    CcspCwmpsoMcoConstructDownloadArgName(CCSP_NS_DOWNLOAD_TARGET_FILE_NAME);
    pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
    pParamValueArray[i].Value           = pSlapVar;

    pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
    pSlapVar->Variant.varString = CcspTr069PaCloneString(pDownloadReq->TargetFileName);

    i++;

    /* DelaySeconds */
    SlapAllocVariable(pSlapVar);
    if ( !pSlapVar ) goto EXIT1;

    CcspCwmpsoMcoConstructDownloadArgName(CCSP_NS_DOWNLOAD_DELAY_SECONDS);
    pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_UnsignedInt;
    pParamValueArray[i].Value           = pSlapVar;

    _ansc_sprintf(buf, "%u", (unsigned int)pDownloadReq->DelaySeconds);

    pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
    pSlapVar->Variant.varString = CcspTr069PaCloneString(buf);

    i++;

    /* SuccessURL */
    SlapAllocVariable(pSlapVar);
    if ( !pSlapVar ) goto EXIT1;

    CcspCwmpsoMcoConstructDownloadArgName(CCSP_NS_DOWNLOAD_SUCCESS_URL);
    pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
    pParamValueArray[i].Value           = pSlapVar;

    pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
    pSlapVar->Variant.varString = CcspTr069PaCloneString(pDownloadReq->SuccessUrl);

    i++;

    /* FailureURL */
    SlapAllocVariable(pSlapVar);
    if ( !pSlapVar ) goto EXIT1;

    CcspCwmpsoMcoConstructDownloadArgName(CCSP_NS_DOWNLOAD_FAILURE_URL);
    pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
    pParamValueArray[i].Value           = pSlapVar;

    pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
    pSlapVar->Variant.varString = CcspTr069PaCloneString(pDownloadReq->FailureUrl);

    i++;

    *pulArraySize = (ULONG)i;
    *ppParamValueArray = pParamValueArray;

    goto EXIT;

EXIT1:

    if ( pParamValueArray )
    {
        int                         k = 0;

        for ( k = 0; k < i; i ++ )
        {
            CcspCwmpCleanParamValue((&pParamValueArray[k]));
        }

        CcspTr069PaFreeMemory(pParamValueArray);
        pParamValueArray = NULL;
    }

EXIT:

    return  returnStatus;
}


ANSC_STATUS
CcspCwmpsoMcoDownload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hDownloadReq
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf         = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpCfgIf;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_MCO_DOWNLOAD_REQ          pMcoDownloadReq    = (PCCSP_CWMP_MCO_DOWNLOAD_REQ     )hDownloadReq;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    PANSC_UNIVERSAL_TIME            pStartTime         = (PANSC_UNIVERSAL_TIME       )NULL;
    PANSC_UNIVERSAL_TIME            pCompleteTime      = (PANSC_UNIVERSAL_TIME       )NULL;
    int                             iStatus            = 0;
    ULONG                           ulNoRPCMethodMask  = 0;
    errno_t rc  = -1;
    int ind = -1;
    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NoRPCMethods )
    {
        ulNoRPCMethodMask = pCcspCwmpCfgIf->NoRPCMethods(pCcspCwmpCfgIf->hOwnerContext);
    }

    /* now we only support type "1 Firmware Upgrade Image" */
    rc = strcmp_s("1 Firmware Upgrade Image", strlen("1 Firmware Upgrade Image"), pMcoDownloadReq->FileType, &ind);
    ERR_CHK(rc);
    if ((ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_Download) || (!(!ind) && (rc == EOK)))
    {
        pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

        if ( !pCwmpSoapFault )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            goto  EXIT1;
        }
        else
        {
            CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
        }
    }
    else 
    {
        /* 
         * map RPC arguments to namespaces and make SPV call to the FC
         * that handles 'Download' - the FC would be "Firmware Upgrade Manager".
         * 
         * Assumption: there will be only one FC that handles all types of
         * Download.
         */
        PCCSP_CWMP_PARAM_VALUE      pParamValueArray= NULL;
        ULONG                       ulArraySize     = 0;

        returnStatus =
            CcspCwmpsoMcoDownload_PrepareArgs
                (
                    pCcspCwmpCpeController,
                    pMcoDownloadReq,
                    &pParamValueArray,
                    &ulArraySize
                );

        if ( !pParamValueArray )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));
            
            if ( !pCwmpSoapFault )
            {
                goto EXIT1;
            }
            else
            {
                CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_resources);
            }
        }
        else
        {
            /* call SPV to start operations */
            returnStatus =
                pCcspCwmpMpaIf->SetParameterValues
                    (
                        pCcspCwmpMpaIf->hOwnerContext,
                        (ANSC_HANDLE)pParamValueArray,
                        ulArraySize,
                        &iStatus,
                        (ANSC_HANDLE*)&pCwmpSoapFault,
                        FALSE
                    );

            if ( pParamValueArray )
            {
                unsigned int                 i;

                for ( i = 0; i < ulArraySize; i++ )
                {
                    CcspCwmpCleanParamValue((&pParamValueArray[i]));
                }

                CcspTr069PaFreeMemory(pParamValueArray);
            }

            if ( returnStatus != ANSC_STATUS_SUCCESS )
            {
                CcspTr069PaTraceError(("Download - SPV failed, status = %d\n", (int)returnStatus));

                if ( pCwmpSoapFault )
                {
                    unsigned int             j;
                    BOOL            bErrorOnCmdKey = FALSE; 

                    for ( j = 0; j < pCwmpSoapFault->SetParamValuesFaultCount; j ++ )
                    {
                        /* temporary solution code starts */
                        bErrorOnCmdKey = (NULL != _ansc_strstr(pCwmpSoapFault->SetParamValuesFaultArray[j].ParameterName, CCSP_NS_DOWNLOAD_COMMAND_KEY));
                        /* temporary solution code ends */

                        CcspTr069PaTraceError(("Download - Download failed to set parameter %s\n", pCwmpSoapFault->SetParamValuesFaultArray[j].ParameterName));
                        CcspCwmpCleanSetParamFault((&pCwmpSoapFault->SetParamValuesFaultArray[j]));
                    }
                    pCwmpSoapFault->SetParamValuesFaultCount = 0;

/* the following code is a temporary solution because SPV D-Bus call
 * won't return expected error code, only CWMP SPV specific error 
 * code may be returned. It must be removed in future if COSA library
 * has improved to let backend returns proper error code to PA.
 */

/* temporary code starts here */

                    if ( bErrorOnCmdKey )
                    {
                        /* if SPV sets fault on CommandKey, means there's a FW upgrade ongoing */
                        CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_resources);
                    }
                    else
                    {
                        char        fuError[256];
                        char*       pValue = NULL;
                        ULONG       ulCwmpError = CCSP_CWMP_CPE_CWMP_FaultCode_resources;

                        _ansc_sprintf
                            (
                                fuError,
                                "%s%s",
                                CCSP_NS_DOWNLOAD,
                                CCSP_NS_DOWNLOAD_ERROR
                            );

                        pCcspCwmpCpeController->GetParamStringValue
                            (
                                (ANSC_HANDLE)pCcspCwmpCpeController,
                                fuError,
                                &pValue
                            );                            

                        if ( pValue ) ulCwmpError = (ULONG)_ansc_atoi(pValue);

                        if ( ulCwmpError < CCSP_ERR_CWMP_BEGINNING ||
                             ulCwmpError > CCSP_ERR_CWMP_ENDING )
                        {
                            ulCwmpError = CCSP_CWMP_CPE_CWMP_FaultCode_resources;
                        }
                        
                        CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, ulCwmpError);
                    }

/* temporary solution ends here */

                }
            }
        }
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pStartTime = (PANSC_UNIVERSAL_TIME)CcspTr069PaAllocateMemory(sizeof(ANSC_UNIVERSAL_TIME));

        if( pStartTime != NULL)
        {
            /* according to TR-069 Spec, and rev 1-4, use unknown time if operation has not finished */
            /* AnscGetSystemTime(pStartTime); */
            AnscZeroMemory(pStartTime, sizeof(ANSC_UNIVERSAL_TIME));

            pStartTime->Year       = 1;
            pStartTime->Month      = 1;
            pStartTime->DayOfMonth = 1;
        }

        pCompleteTime = (PANSC_UNIVERSAL_TIME)CcspTr069PaAllocateMemory(sizeof(ANSC_UNIVERSAL_TIME));

        if( pCompleteTime != NULL)
        {
            /* AnscGetSystemTime(pCompleteTime); */

            /* WT-151 requires to return Unknown Time if the operation is not fully complete */
            AnscZeroMemory(pCompleteTime, sizeof(ANSC_UNIVERSAL_TIME));

            pCompleteTime->Year       = 1;
            pCompleteTime->Month      = 1;
            pCompleteTime->DayOfMonth = 1;
        }

		/* use TransferComplete always */
		iStatus = 1;

        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_Download;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_Download
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    iStatus,
                    pStartTime,
                    pCompleteTime,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pStartTime )
    {
        CcspTr069PaFreeMemory(pStartTime);
    }

    if ( pCompleteTime )
    {
        CcspTr069PaFreeMemory(pCompleteTime);
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


ANSC_STATUS
CcspCwmpsoMcoReboot
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pCommandKey
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    char                            psmKeyPrefixed[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN + 16];

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

    CcspCwmpPrefixPsmKey(psmKeyPrefixed, pCcspCwmpCpeController->SubsysName, CCSP_TR069PA_PSM_KEY_TriggerCommand);
    returnStatus = 
        pCcspCwmpCpeController->SaveCfgToPsm
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                psmKeyPrefixed,
                "M Reboot"
            );

    CcspCwmpPrefixPsmKey(psmKeyPrefixed, pCcspCwmpCpeController->SubsysName, CCSP_TR069PA_PSM_KEY_TriggerCommandKey);
    returnStatus = 
        pCcspCwmpCpeController->SaveCfgToPsm
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                psmKeyPrefixed,
                pCommandKey
            );

    CcspTr069PaTraceInfo(("Write TriggerCommand 'M Reboot' to PSM.\n"));

    pMyObject->SaveCwmpEvent(pMyObject);

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        /*
         * Something serious has screwed up (e.g. we have run out of system memory), abort!
         */
        goto  EXIT1;
    }
    else if ( !pCcspCwmpCpeController->bCpeRebooting )
    {
        /* 
         * map RPC arguments to namespaces and make SPV call to the FC
         * that handles 'Reboot'.
         */
        CCSP_CWMP_PARAM_VALUE       ParamValue      = {0};
        ULONG                       ulArraySize     = 1;
        int                         iStatus         = 0;
        SLAP_VARIABLE               slapVar;
        char                        value[12]        = {"Device\0"};

		CcspCwmpSetRebootReason((ANSC_HANDLE)pCcspCwmpMpaIf);

#ifndef DONT_HAVE_RM
        ParamValue.Name          = CCSP_NS_REBOOT;
        ParamValue.Tr069DataType = CCSP_CWMP_TR069_DATA_TYPE_String;
        ParamValue.Value         = &slapVar;

        SlapInitVariable(&slapVar);
        slapVar.Syntax           = SLAP_VAR_SYNTAX_string;
        slapVar.Variant.varString= value;

        returnStatus =
            pCcspCwmpMpaIf->SetParameterValues
                (
                    pCcspCwmpMpaIf->hOwnerContext,
                    (ANSC_HANDLE)&ParamValue,
                    ulArraySize,
                    &iStatus,
                    (ANSC_HANDLE*)&pCwmpSoapFault,
                    FALSE
                );
#endif
        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCcspCwmpCpeController->bCpeRebooting = TRUE;
        }
        else
        if ( pCwmpSoapFault )
        {
            unsigned int                     j;
            for ( j = 0; j < pCwmpSoapFault->SetParamValuesFaultCount; j ++ )
            {
                CcspTr069PaTraceError(("Reboot - SPV failed on parameter %s\n", pCwmpSoapFault->SetParamValuesFaultArray[j].ParameterName));
                CcspCwmpCleanSetParamFault((&pCwmpSoapFault->SetParamValuesFaultArray[j]));
            }
            pCwmpSoapFault->SetParamValuesFaultCount = 0;
        }
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_Reboot;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_Reboot
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)NULL
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

#ifdef DONT_HAVE_RM
    AnscSpawnTask3
        (
            CcspCwmpsoAsyncReboot,
            NULL,
            "CcspCwmpsoAsyncRebootTask",
            USER_DEFAULT_TASK_PRIORITY,
            USER_DEFAULT_TASK_STACK_SIZE
            );
#endif

    return  returnStatus;
}

ANSC_STATUS CcspCwmpSetRebootReason(ANSC_HANDLE                 hThisObject)
{
	PCCSP_CWMP_MPA_INTERFACE	pCcspCwmpMpaIf  = (PCCSP_CWMP_MPA_INTERFACE      )hThisObject;
    PCCSP_CWMP_SOAP_FAULT       pCwmpSoapFault  = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    CCSP_CWMP_PARAM_VALUE       ParamValue      = {0};
    ANSC_STATUS                 returnStatus    = ANSC_STATUS_SUCCESS;
    ULONG                       ulArraySize     = 1;
    int                         iStatus         = 0;
    SLAP_VARIABLE               slapVar;
    char                        value[16]        = {"tr069-reboot\0"};

    ParamValue.Name          = CCSP_NS_REBOOT_REASON;
    ParamValue.Tr069DataType = CCSP_CWMP_TR069_DATA_TYPE_String;
    ParamValue.Value         = &slapVar;

    SlapInitVariable(&slapVar);
    slapVar.Syntax           = SLAP_VAR_SYNTAX_string;
    slapVar.Variant.varString= value;

    returnStatus =
        pCcspCwmpMpaIf->SetParameterValues
            (
                pCcspCwmpMpaIf->hOwnerContext,
                (ANSC_HANDLE)&ParamValue,
                ulArraySize,
                &iStatus,
                (ANSC_HANDLE*)&pCwmpSoapFault,
                FALSE
            );

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
	    if ( pCwmpSoapFault )
	    {
	        unsigned int                     j;
	        for ( j = 0; j < pCwmpSoapFault->SetParamValuesFaultCount; j ++ )
	        {
	            CcspTr069PaTraceError(("RebootReason - SPV failed on parameter %s\n", pCwmpSoapFault->SetParamValuesFaultArray[j].ParameterName));
	            CcspCwmpCleanSetParamFault((&pCwmpSoapFault->SetParamValuesFaultArray[j]));
	        }
	        pCwmpSoapFault->SetParamValuesFaultCount = 0;
	    }
    }
	
    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

	return returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoChangeDUState
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hCdsReq
            );

    description:

        This function is called to cause the CPE to change DU state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                ANSC_HANDLE                 hCdsReq
                Specifies the parameters used for ChangeDUState.

    return:     status of operation.

**********************************************************************/

#define  CcspCwmpsoMcoConstructCdsArgName(arg)                                          \
    do {                                                                                \
        _ansc_sprintf                                                                   \
            (                                                                           \
                buf,                                                                    \
                "%s%u.%s",                                                              \
                CCSP_NS_CHANGEDUSTATE,                                                  \
                (unsigned int)pInsNumbers[k],                                           \
                arg                                                                     \
            );                                                                          \
        pParamValueArray[i].Name = CcspTr069PaCloneString(buf);                         \
    } while (0) 


static
ANSC_STATUS
CcspCwmpsoMcoChangeDUState_PrepareArgs
    (
        PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController,
        PCCSP_TR069_CDS_REQ         pCdsReq,
        PCCSP_CWMP_PARAM_VALUE*     ppParamValueArray,
        PULONG                      pulArraySize,
        PULONG                      pInsNumbers
    )
{
    ANSC_STATUS                     returnStatus        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PARAM_VALUE          pParamValueArray    = NULL;
    int                             i                   = 0;
    PSLAP_VARIABLE                  pSlapVar;
    char*                           pValue;
    PCCSP_TR069_CDS_Operation       pOperation;
    int                             k                   = 0;
    char                            buf[512];
    PCCSP_CWMP_SOAP_FAULT           pCwmpFault      = NULL;

    *ppParamValueArray  = NULL;
    *pulArraySize       = 0;

    pParamValueArray = 
        (PCCSP_CWMP_PARAM_VALUE)CcspTr069PaAllocateMemory
            (
                sizeof(CCSP_CWMP_PARAM_VALUE) * CCSP_NS_CDS_OPERATION_ARG_MAX_COUNT * pCdsReq->NumOperations
            );

    if ( !pParamValueArray )
    {
        return  ANSC_STATUS_RESOURCES;
    }
    else
    {
        AnscZeroMemory
            (
                pParamValueArray, 
                CCSP_NS_CDS_OPERATION_ARG_MAX_COUNT * sizeof(CCSP_CWMP_PARAM_VALUE) * pCdsReq->NumOperations
            );
    }

    returnStatus =
        pCcspCwmpCpeController->AddObjects
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                CCSP_NS_CHANGEDUSTATE,
                pCdsReq->NumOperations,
                pInsNumbers,
                &pCwmpFault
            );

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTr069PaTraceError(("CDS - failed to make a request to AppEnvMgr, nRet = %d\n", (int)returnStatus));

        returnStatus = ANSC_STATUS_INTERNAL_ERROR;

        goto EXIT1;
    }

    for ( k = 0; k < pCdsReq->NumOperations; k ++ )
    {
        pOperation = &pCdsReq->Operations[k];

        /* ID */
        SlapAllocVariable(pSlapVar);

        CcspCwmpsoMcoConstructCdsArgName(CCSP_NS_CDS_ID);
        pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
        pParamValueArray[i].Value           = pSlapVar;

        pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
        pSlapVar->Variant.varString = CcspTr069PaCloneString(pCdsReq->CommandKey);

        i++;

        /* Operation */
        SlapAllocVariable(pSlapVar);
        if ( !pSlapVar ) goto EXIT1;

        CcspCwmpsoMcoConstructCdsArgName(CCSP_NS_CDS_OPERATION);
        pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
        pParamValueArray[i].Value           = pSlapVar;

        pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
        switch ( pOperation->Op )
        {
            case    CCSP_TR069_CDS_OP_Install:
        
                    pSlapVar->Variant.varString = CcspTr069PaCloneString(CCSP_NS_CDS_OPERATION_Install);

                    break;
            case    CCSP_TR069_CDS_OP_Update:
        
                    pSlapVar->Variant.varString = CcspTr069PaCloneString(CCSP_NS_CDS_OPERATION_Update);

                    break;
            case    CCSP_TR069_CDS_OP_Uninstall:
        
                    pSlapVar->Variant.varString = CcspTr069PaCloneString(CCSP_NS_CDS_OPERATION_Remove);

                    break;

            default:

                    goto EXIT1;
        }

        i++;

        /* OperationState */
        SlapAllocVariable(pSlapVar);
        if ( !pSlapVar ) goto EXIT1;

        CcspCwmpsoMcoConstructCdsArgName(CCSP_NS_CDS_OPERATION_STATE);
        pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
        pParamValueArray[i].Value           = pSlapVar;

        pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
        pSlapVar->Variant.varString = CcspTr069PaCloneString(CCSP_NS_CDS_OPERATION_STATE_Requested);

        i++;

        /* URL */
        if ( pOperation->Op == CCSP_TR069_CDS_OP_Install ||
             pOperation->Op == CCSP_TR069_CDS_OP_Update )
        {
            pValue = 
                (pOperation->Op == CCSP_TR069_CDS_OP_Install) ?
                    pOperation->op.Install.Url : pOperation->op.Update.Url;

            SlapAllocVariable(pSlapVar);
            if ( !pSlapVar ) goto EXIT1;

            CcspCwmpsoMcoConstructCdsArgName(CCSP_NS_CDS_URL);
            pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
            pParamValueArray[i].Value           = pSlapVar;

            pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
            pSlapVar->Variant.varString = CcspTr069PaCloneString(pValue);

            i++;
        }

        /* UUID */
        if ( TRUE )
        {
            if ( pOperation->Op == CCSP_TR069_CDS_OP_Install )
            {
                pValue = pOperation->op.Install.Uuid;
            }
            else if ( pOperation->Op == CCSP_TR069_CDS_OP_Update )
            {
                pValue = pOperation->op.Update.Uuid;
            }
            else
            {
                pValue = pOperation->op.Uninstall.Uuid;
            }

            SlapAllocVariable(pSlapVar);
            if ( !pSlapVar ) goto EXIT1;

            CcspCwmpsoMcoConstructCdsArgName(CCSP_NS_CDS_UUID);
            pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
            pParamValueArray[i].Value           = pSlapVar;

            pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
            pSlapVar->Variant.varString = CcspTr069PaCloneString(pValue);

            i++;
        }

        /* Username */
        if ( pOperation->Op == CCSP_TR069_CDS_OP_Install ||
             pOperation->Op == CCSP_TR069_CDS_OP_Update )
        {
            pValue = 
                (pOperation->Op == CCSP_TR069_CDS_OP_Install) ?
                    pOperation->op.Install.Username : pOperation->op.Update.Username;

            SlapAllocVariable(pSlapVar);
            if ( !pSlapVar ) goto EXIT1;

            CcspCwmpsoMcoConstructCdsArgName(CCSP_NS_CDS_Username);
            pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
            pParamValueArray[i].Value           = pSlapVar;

            pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
            pSlapVar->Variant.varString = CcspTr069PaCloneString(pValue);

            i++;
        }

        /* Password */
        if ( pOperation->Op == CCSP_TR069_CDS_OP_Install ||
             pOperation->Op == CCSP_TR069_CDS_OP_Update )
        {
            pValue = 
                (pOperation->Op == CCSP_TR069_CDS_OP_Install) ?
                    pOperation->op.Install.Password : pOperation->op.Update.Password;

            SlapAllocVariable(pSlapVar);
            if ( !pSlapVar ) goto EXIT1;

            CcspCwmpsoMcoConstructCdsArgName(CCSP_NS_CDS_Password);
            pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
            pParamValueArray[i].Value           = pSlapVar;

            pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
            pSlapVar->Variant.varString = CcspTr069PaCloneString(pValue);

            i++;
        }

        /* ExecEnvRef */
        if ( pOperation->Op == CCSP_TR069_CDS_OP_Install )
        {
            pValue = pOperation->op.Install.ExecEnvRef;

            SlapAllocVariable(pSlapVar);
            if ( !pSlapVar ) goto EXIT1;

            CcspCwmpsoMcoConstructCdsArgName(CCSP_NS_CDS_EXECUTION_ENV_REF);
            pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
            pParamValueArray[i].Value           = pSlapVar;

            pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
            pSlapVar->Variant.varString = CcspTr069PaCloneString(pValue);

            i++;
        }

        /* Version */
        if ( pOperation->Op == CCSP_TR069_CDS_OP_Uninstall ||
             pOperation->Op == CCSP_TR069_CDS_OP_Update )
        {
            pValue = 
                (pOperation->Op == CCSP_TR069_CDS_OP_Uninstall) ?
                    pOperation->op.Uninstall.Version: pOperation->op.Update.Version;

            SlapAllocVariable(pSlapVar);
            if ( !pSlapVar ) goto EXIT1;

            CcspCwmpsoMcoConstructCdsArgName(CCSP_NS_CDS_VERSION);
            pParamValueArray[i].Tr069DataType   = CCSP_CWMP_TR069_DATA_TYPE_String;
            pParamValueArray[i].Value           = pSlapVar;

            pSlapVar->Syntax            = SLAP_VAR_SYNTAX_string;
            pSlapVar->Variant.varString = CcspTr069PaCloneString(pValue);

            i++;
        }
    }

    *pulArraySize = (ULONG)i;
    *ppParamValueArray = pParamValueArray;

    goto EXIT;

EXIT1:

    if ( pCwmpFault )
    {
        CcspCwmpFreeSoapFault(pCwmpFault);
        pCwmpFault = NULL;
    }

    if ( pParamValueArray )
    {
        CcspTr069PaTraceError(("CDS - abnormally exit, k = %d\n", k));

        for ( i = 0; i < k; i ++ )
        {
            CcspCwmpCleanParamValue((&pParamValueArray[i]));
        }

        CcspTr069PaFreeMemory(pParamValueArray);
        pParamValueArray = NULL;
    }

EXIT:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pCcspCwmpCpeController->DeleteObjects
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                CCSP_NS_CHANGEDUSTATE,
                pCdsReq->NumOperations,
                pInsNumbers,
                &pCwmpFault,
                0
            );

        if ( pCwmpFault )
        {
            CcspCwmpFreeSoapFault(pCwmpFault);
            pCwmpFault = NULL;
        }
    }

    return  returnStatus;
}


static
ANSC_STATUS
CcspCwmpsoMcoChangeDUState_SaveStateMonitors
    (
        PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController,
        PCCSP_TR069_CDS_REQ         pCdsReq,
        int                         NumInstances,
        PULONG                      pInsNumbers,
        BOOL                        bSaveMonitors
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    int                             i;
    char                            buf[512];
    char                            vbuf[16];
    int                             nRet;

    if ( !bSaveMonitors )
    {
        for ( i = 0; i < NumInstances; i ++ )
        {
            if ( !pCcspCwmpCpeController->SubsysName )
            {
                _ansc_sprintf(buf, "%s%u.", CCSP_NS_CHANGEDUSTATE, (unsigned int)pInsNumbers[i]);
            }
            else
            {
                _ansc_sprintf(buf, "%s%s%u.", pCcspCwmpCpeController->SubsysName, CCSP_NS_CHANGEDUSTATE, (unsigned int)pInsNumbers[i]);
            }

            CcspTr069PaTraceDebug
                ((
                    "CDS - remove state monitor <%s> from PSM, value <%s>.\n", 
                    buf, 
                    pCdsReq->CommandKey
                ));

            PSM_Del_Record
                (
                    pCcspCwmpCpeController->hMsgBusHandle,
                    pCcspCwmpCpeController->SubsysName,
                    buf
                );
        }
    }
    else
    {
        for ( i = 0; i < NumInstances; i ++ )
        {
            if ( !pCcspCwmpCpeController->SubsysName )
            {
                _ansc_sprintf(buf, "%s%u.", CCSP_NS_CHANGEDUSTATE, (unsigned int)pInsNumbers[i]);
            }
            else
            {
                _ansc_sprintf(buf, "%s%s%u.", pCcspCwmpCpeController->SubsysName, CCSP_NS_CHANGEDUSTATE, (unsigned int)pInsNumbers[i]);
            }

            CcspTr069PaTraceDebug
                ((
                    "CDS - saves state monitor <%s> into PSM, value <%s>.\n", 
                    buf, 
                    pCdsReq->CommandKey
                ));

            nRet = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        buf,
                        ccsp_string,
                        pCdsReq->CommandKey
                    );

            if ( nRet != CCSP_SUCCESS )
            {
                CcspTr069PaTraceError(("CDS - failed to save state monitor into PSM, nRet = %d\n", nRet));

                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                break;
            }

            if ( !pCcspCwmpCpeController->SubsysName )
            {
                _ansc_sprintf(buf, "%s%u.%s", CCSP_NS_CHANGEDUSTATE, (unsigned int)pInsNumbers[i], CCSP_NS_CDS_PSM_NODE_COMPLETE);
            }
            else
            {
                _ansc_sprintf(buf, "%s%s%u.%s", pCcspCwmpCpeController->SubsysName, CCSP_NS_CHANGEDUSTATE, (unsigned int)pInsNumbers[i], CCSP_NS_CDS_PSM_NODE_COMPLETE);
            }

            CcspTr069PaTraceDebug
                ((
                    "CDS - saves state monitor <%s> into PSM, value <%s>.\n", 
                    buf, 
                    "0"
                ));

            nRet = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        buf,
                        ccsp_boolean,
                        "0"
                    );

            if ( nRet != CCSP_SUCCESS )
            {
                CcspTr069PaTraceError(("CDS - failed to save state monitor into PSM, nRet = %d\n", nRet));

                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                break;
            }

            if ( !pCcspCwmpCpeController->SubsysName )
            {
                _ansc_sprintf(buf, "%s%u.%s", CCSP_NS_CHANGEDUSTATE, (unsigned int)pInsNumbers[i], CCSP_NS_CDS_PSM_NODE_ORDER);
            }
            else
            {
                _ansc_sprintf(buf, "%s%s%u.%s", pCcspCwmpCpeController->SubsysName, CCSP_NS_CHANGEDUSTATE, (unsigned int)pInsNumbers[i], CCSP_NS_CDS_PSM_NODE_ORDER);
            }

            _ansc_sprintf(vbuf, "%u", i);

            CcspTr069PaTraceDebug
                ((
                    "CDS - saves state monitor <%s> into PSM, value <%s>.\n", 
                    buf, 
                    vbuf
                ));

            nRet = 
                PSM_Set_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        buf,
                        ccsp_unsignedInt,
                        vbuf
                    );

            if ( nRet != CCSP_SUCCESS )
            {
                CcspTr069PaTraceError(("CDS - failed to save state monitor into PSM, nRet = %d\n", nRet));

                returnStatus = ANSC_STATUS_INTERNAL_ERROR;
                break;
            }
        }

        /* remove saved monitors if error occurs */
        if ( i < pCdsReq->NumOperations )
        {
            CcspCwmpsoMcoChangeDUState_SaveStateMonitors
                (
                    pCcspCwmpCpeController,
                    pCdsReq,
                    i,
                    pInsNumbers,
                    FALSE
                );
        }
    }

    return  returnStatus;
}


ANSC_STATUS
CcspCwmpsoMcoChangeDUState
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hCdsReq
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf         = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpCfgIf;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_TR069_CDS_REQ             pCdsReq            = (PCCSP_TR069_CDS_REQ        )hCdsReq;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ULONG                           ulNoRPCMethodMask  = 0;
    PULONG                          pInsNumbers        = NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpFault         = NULL;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NoRPCMethods )
    {
        ulNoRPCMethodMask = pCcspCwmpCfgIf->NoRPCMethods(pCcspCwmpCfgIf->hOwnerContext);
    }

    if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_Download )
    {
        pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

        if ( !pCwmpSoapFault )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            goto  EXIT1;
        }
        else
        {
            CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
        }
    }
    else
    {
        BOOL                        bCdsSupported = TRUE;

        if ( !bCdsSupported )
        {
            pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

            if ( !pCwmpSoapFault )
            {
                returnStatus = ANSC_STATUS_RESOURCES;

                goto  EXIT1;
            }
            else
            {
                CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
            }
        }
        else
        {
            /* 
             * map RPC arguments to namespaces and make SPV call to the FC
             * that handles 'ChangeDUState'.
             * Assumption: there will be only one FC that handles all types of
             * ChangeDUState, in CCSP, this module is Application Environment
             * Manager (AEM).
             *
             * Per current AEM spec, TR-069 PA needs to map a CDS operation (install/update/
             * uninstall) into CCSP internal namespace. TR-069 PA needs to maintain
             * the orders of these operations and keep them into PSM. Once PA receives
             * "DUStateChangeComplete" events by monitoring CCSP internal namespace defined
             * by AEM, PA re-order operation results and send them back to ACS once all 
             * operation results have been collected.
             */
            int                     iStatus         = 0;
            PCCSP_CWMP_PARAM_VALUE  pParamValueArray= NULL;
            ULONG                   ulArraySize     = 0;

            pInsNumbers = 
                (PULONG)CcspTr069PaAllocateMemory
                    (
                        sizeof(ULONG) * pCdsReq->NumOperations
                    );

            returnStatus = 
                CcspCwmpsoMcoChangeDUState_PrepareArgs
                    (
                        pCcspCwmpCpeController,
                        pCdsReq,
                        &pParamValueArray,
                        &ulArraySize,
                        pInsNumbers
                    );

            if ( !pParamValueArray )
            {
                pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));
                
                if ( !pCwmpSoapFault )
                {
                    goto EXIT1;
                }
                else
                {
                    if ( returnStatus == ANSC_STATUS_RESOURCES )
                    {
                        CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_resources);
                    }
                    else
                    {
                        CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_internalError);
                    }
                }
            }
            else
            {
                /* set up state monitor on all operations */

                /* According to AEM SAS spec, PA only needs to subscribe to 
                   value change on the table object "CCSP.COMMAND.PACKAGE.MANAGEMENT.".
                   AEM only signals ValueChange on "OperationState" namespace when
                   its value changes to 'Complete' or 'Error'.

                returnStatus =
                    pCcspCwmpCpeController->MonitorOpState
                        (
                            (ANSC_HANDLE)pCcspCwmpCpeController,
                            TRUE,
                            CCSP_NS_CHANGEDUSTATE,
                            pCdsReq->NumOperations,
                            pInsNumbers,
                            CCSP_NS_CDS_OPERATION_STATE,
                            &pCwmpSoapFault
                        );

                if ( returnStatus != ANSC_STATUS_SUCCESS )
                {
                    CcspTr069PaTraceError(("CDS - failed to monitor on operations, status = %u\n", returnStatus));
                }
                else
                */
                {
                    if ( TRUE )
                    {
                        /* call SPV to start operations */
                        returnStatus =
                            pCcspCwmpMpaIf->SetParameterValues
                                (
                                    pCcspCwmpMpaIf->hOwnerContext,
                                    (ANSC_HANDLE)pParamValueArray,
                                    ulArraySize,
                                    &iStatus,
                                    (ANSC_HANDLE*)&pCwmpSoapFault,
                                    FALSE
                                );

                        if ( pParamValueArray )
                        {
                            unsigned int                 i;

                            for ( i = 0; i < ulArraySize; i++ )
                            {
                                CcspCwmpCleanParamValue((&pParamValueArray[i]));
                            }

                            CcspTr069PaFreeMemory(pParamValueArray);
                        }

                        if ( returnStatus != ANSC_STATUS_SUCCESS )
                        {
                            PCCSP_CWMP_SOAP_FAULT   pCwmpFault  = NULL;
                            char                    objInstance[256];

                            memset(objInstance, 0, sizeof(objInstance));
                            if ( pCwmpSoapFault )
                            {
                                unsigned int j;
                                for ( j = 0; j < pCwmpSoapFault->SetParamValuesFaultCount; j ++ )
                                {
                                    CcspTr069PaTraceError(("CDS - SPV failed on parameter %s\n", pCwmpSoapFault->SetParamValuesFaultArray[j].ParameterName));
                                    CcspCwmpCleanSetParamFault((&pCwmpSoapFault->SetParamValuesFaultArray[j]));
                                }
                                pCwmpSoapFault->SetParamValuesFaultCount = 0;
                            }

                            /* delete created object instance(s) */
                            pCcspCwmpCpeController->DeleteObjects
                                (
                                    (ANSC_HANDLE)pCcspCwmpCpeController,
                                    CCSP_NS_CHANGEDUSTATE,
                                    pCdsReq->NumOperations,
                                    pInsNumbers,
                                    &pCwmpFault,
                                    0
                                );

                            CcspTr069PaTraceError(("CDS - SPV failed, status = %d\n", (int)returnStatus));

                            if ( pCwmpFault )
                            {
                                CcspCwmpFreeSoapFault(pCwmpFault);
                                pCwmpFault = NULL;
                            }
                        }
                        else
                        {
                            /* save requests (object instances, CommandKey, complete etc for 
                             * monitoring operation status) into PSM 
                             */
                            returnStatus =
                                CcspCwmpsoMcoChangeDUState_SaveStateMonitors
                                    (
                                        pCcspCwmpCpeController,
                                        pCdsReq,
                                        pCdsReq->NumOperations,
                                        pInsNumbers,
                                        TRUE
                                    );
                        }
                    }
                }
            }
        }
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_ChangeDUState;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_ChangeDUState
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pCcspCwmpCpeController->DeleteObjects
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                CCSP_NS_CHANGEDUSTATE,
                pCdsReq->NumOperations,
                pInsNumbers,
                &pCwmpFault,
                0
            );

        if ( pCwmpFault )
        {
            CcspCwmpFreeSoapFault(pCwmpFault);
            pCwmpFault = NULL;
        }

        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    if ( pInsNumbers )
    {
        CcspTr069PaFreeMemory(pInsNumbers);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoGetQueuedTransfers
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID
            );

    description:

        This function is called to determine the status of previously
        requested downloads or uploads.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoGetQueuedTransfers
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID
    )
{
    ANSC_STATUS                     returnStatus         = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject            = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor    = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController   = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf           = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpCfgIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser      = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep       = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_QUEUED_TRANSFER      pQueuedTransferArray = (PCCSP_CWMP_QUEUED_TRANSFER )NULL;
    ULONG                           ulArraySize          = (ULONG                      )0;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault       = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ULONG                           ulNoRPCMethodMask    = 0;

    /* no need to support this RPC for now */
    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NoRPCMethods )
    {
        ulNoRPCMethodMask = pCcspCwmpCfgIf->NoRPCMethods(pCcspCwmpCfgIf->hOwnerContext);
    }

    if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_GetQueuedTransfers )
    {
        pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

        if ( !pCwmpSoapFault )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            goto  EXIT1;
        }
        else
        {
            CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
        }
    }

    if ( (returnStatus != ANSC_STATUS_SUCCESS) && !pCwmpSoapFault )
    {
        /*
         * Something serious has screwed up (e.g. we have run out of system memory), abort!
         */
        goto  EXIT1;
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_GetQueuedTransfers;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_GetQueuedTransfers
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)pQueuedTransferArray,
                    (ULONG      )ulArraySize,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pQueuedTransferArray )
    {
        CcspTr069PaFreeMemory(pQueuedTransferArray);
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoScheduleInform
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ULONG                       ulDelaySeconds,
                char*                       pCommandKey
            );

    description:

        This function is called to request the CPE to schedule a
        one-time Inform method call (separate from its periodic Inform
        method calls) sometime in the future.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                ULONG                       ulDelaySeconds
                Specifies the number of seconds from the time this
                method is called to the time the CPE is requested to
                initiate a one-time Inform method call.

                char*                       pCommandKey
                Specifies the string to return in the CommandKey
                element of the InformStruct when the CPE calls the
                Inform method.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoScheduleInform
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ULONG                       ulDelaySeconds,
        char*                       pCommandKey
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf         = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpCfgIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ULONG                           ulNoRPCMethodMask  = 0;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NoRPCMethods )
    {
        ulNoRPCMethodMask = pCcspCwmpCfgIf->NoRPCMethods(pCcspCwmpCfgIf->hOwnerContext);
    }

    if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_ScheduleInform )
    {
        pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

        if ( !pCwmpSoapFault )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            goto  EXIT1;
        }
        else
        {
            CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
        }
    }
    else
    {
        pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

        if ( !pCwmpSoapFault )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            goto  EXIT1;
        }

        /*
         * The 'ulDelaySeconds" argument specifies the number of seconds from the time this method is
         * called to the time the CPE is requested to initiate a one-time Inform method call. The CPE
         * sends a response, and then DelaySeconds later calls the Inform method. This argument must be
         * greater than zero.
         */
        if ( ulDelaySeconds == 0 )
        {
            pCwmpSoapFault->soap_faultcode           = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_invalidArgs);
            pCwmpSoapFault->soap_faultstring         = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_invalidArgs);
            pCwmpSoapFault->Fault.FaultCode          = CCSP_CWMP_CPE_CWMP_FaultCode_invalidArgs;
            pCwmpSoapFault->Fault.FaultString        = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_invalidArgs);
            pCwmpSoapFault->SetParamValuesFaultCount = 0;
        }
        else
        {
            returnStatus =
                pCcspCwmpProcessor->ScheduleInform
                    (
                        (ANSC_HANDLE)pCcspCwmpProcessor,
                        ulDelaySeconds,
                        pCommandKey
                    );

            if ( returnStatus != ANSC_STATUS_SUCCESS )
            {
                CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_requestDenied);
            }
            else
            {
                if( pCwmpSoapFault )
                {
                    CcspCwmpFreeSoapFault(pCwmpSoapFault);
                    pCwmpSoapFault = NULL;
                }
            }
        }
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_ScheduleInform;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_ScheduleInform
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoSetVouchers
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                SLAP_STRING_ARRAY*          pVoucherList
            )

    description:

        This function is called to set one or more option Vouchers in
        the CPE.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                SLAP_STRING_ARRAY*          pVoucherList
                Specifies the array of Vouchers, where each Voucher is
                represented as a Base64 encoded octet string.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoSetVouchers
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        SLAP_STRING_ARRAY*          pVoucherList
    )
{
    UNREFERENCED_PARAMETER(pVoucherList);
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;

    /* this RPC has been deprecated by TR-069 spec */
    pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

    if ( !pCwmpSoapFault )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }
    else
    {
        CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_SetVouchers;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_SetVouchers
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoGetOptions
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                char*                       pOptionName
            )

    description:

        This function is called to obtain a list of the options
        currently set in a CPE, and their associated state information.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                char*                       pOptionName
                Specifies a string representing either the name of a
                particular Option, or an empty string indicating the
                method should return the state of all Options supported
                by the CPE (whether or not they are currently enabled).

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoGetOptions
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pOptionName
    )
{
    UNREFERENCED_PARAMETER(pOptionName);
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;

    /* this RPC has been deprecated */
    pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

    if ( !pCwmpSoapFault )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }
    else
    {
        CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_GetOptions;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_GetOptions
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)NULL,
                    (ULONG      )0,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoUpload
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hUploadReq
            );

    description:

        This function is called to cause the CPE to upload a specified
        file to the designated location.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

                ANSC_HANDLE                 hUploadReq
                Specifies the parameters used for file uploading.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoUpload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hUploadReq
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf         = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpCfgIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_MCO_UPLOAD_REQ            pMcoUploadReq      = (PCCSP_CWMP_MCO_UPLOAD_REQ       )hUploadReq;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    PANSC_UNIVERSAL_TIME            pStartTime         = (PANSC_UNIVERSAL_TIME       )NULL;
    PANSC_UNIVERSAL_TIME            pCompleteTime      = (PANSC_UNIVERSAL_TIME       )NULL;
    int                             iStatus            = 0;
    ULONG                           ulNoRPCMethodMask  = 0;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NoRPCMethods )
    {
        ulNoRPCMethodMask = pCcspCwmpCfgIf->NoRPCMethods(pCcspCwmpCfgIf->hOwnerContext);
    }

    if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_Upload )
    {
        pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

        if ( !pCwmpSoapFault )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            goto  EXIT1;
        }
        else
        {
            CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
        }
    }
    else
    {
        BOOL                        bUploadSupported = TRUE;

        if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NoRPCMethods )
        {
            ULONG                   ulNoRPCMethodMask = pCcspCwmpCfgIf->NoRPCMethods(pCcspCwmpCfgIf->hOwnerContext);

            if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_Upload )
            {
                bUploadSupported = FALSE;
            }
        }

        if ( bUploadSupported )
        {
            pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

            if ( !pCwmpSoapFault )
            {
                returnStatus = ANSC_STATUS_RESOURCES;

                goto  EXIT1;
            }
            else
            {
                CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_requestDenied);
            }
        }
        else if ( _ansc_strstr(pMcoUploadReq->Url, "@") != NULL )  /* userinfo included */
        {
            pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

            if ( !pCwmpSoapFault )
            {
                returnStatus = ANSC_STATUS_RESOURCES;

                goto  EXIT1;
            }
            else
            {
                CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_invalidArgs);
            }
        }
        else
        {
            /* 
             * map RPC arguments to namespaces and make SPV call to the FC
             * that handles 'Upload'.
             * Assumption: there will be only one FC that handles all types of
             * Upload.
             */

            returnStatus = ANSC_STATUS_NOT_SUPPORTED;

            goto  EXIT1;
        }
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pStartTime = (PANSC_UNIVERSAL_TIME)CcspTr069PaAllocateMemory(sizeof(ANSC_UNIVERSAL_TIME));

        if( pStartTime != NULL)
        {
            /* according to TR-069 Spec, and rev 1-4, use unknown time if operation has not finished */
            /* AnscGetSystemTime(pStartTime); */
            AnscZeroMemory(pStartTime, sizeof(ANSC_UNIVERSAL_TIME));

            pStartTime->Year       = 1;
            pStartTime->Month      = 1;
            pStartTime->DayOfMonth = 1;
        }

        pCompleteTime = (PANSC_UNIVERSAL_TIME)CcspTr069PaAllocateMemory(sizeof(ANSC_UNIVERSAL_TIME));

        if( pCompleteTime != NULL)
        {
            /* AnscGetSystemTime(pCompleteTime); */

            /* WT-151 requires to return Unknown Time if the operation is not fully complete */
            AnscZeroMemory(pCompleteTime, sizeof(ANSC_UNIVERSAL_TIME));

            pCompleteTime->Year       = 1;
            pCompleteTime->Month      = 1;
            pCompleteTime->DayOfMonth = 1;
        }

        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_Upload;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_Upload
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    iStatus,
                    pStartTime,
                    pCompleteTime,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pStartTime )
    {
        CcspTr069PaFreeMemory(pStartTime);
    }

    if ( pCompleteTime )
    {
        CcspTr069PaFreeMemory(pCompleteTime);
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoMcoFactoryReset
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID
            );

    description:

        This function is called to reset the CPE to its factory default
        state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRequestID
                If not NULL, it represents the "ID" header element used
                to associate SOAP requests and responses using a unique
                identifier for each request.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoMcoFactoryReset
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf         = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpCfgIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE )NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ULONG                           ulNoRPCMethodMask  = 0;

    CcspTraceWarning(("%s -- %d Entering CcspCwmpsoMcoFactoryReset \n", __FUNCTION__, __LINE__));

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NoRPCMethods )
    {
        ulNoRPCMethodMask = pCcspCwmpCfgIf->NoRPCMethods(pCcspCwmpCfgIf->hOwnerContext);
    }

    if ( ulNoRPCMethodMask & CCSP_CWMP_CFG_RPC_METHOD_NO_FactoryReset )
    {
        pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

        if ( !pCwmpSoapFault )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            goto  EXIT1;
        }
        else
        {
            CCSP_CWMP_SET_SOAP_FAULT(pCwmpSoapFault, CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported);
        }
    }
    else
    {
        /* 
         * map RPC arguments to namespaces and make SPV call to the FC
         * that handles 'FactoryReset'.
         */
        CCSP_CWMP_PARAM_VALUE       ParamValue      = {0};
        ULONG                       ulArraySize     = 1;
        int                         iStatus         = 0;
        SLAP_VARIABLE               slapVar;
        char                       value[] = "Router,Wifi,VoIP,Dect,MoCA";

        ParamValue.Name          = CCSP_NS_FACTORYRESET;
        ParamValue.Tr069DataType = CCSP_CWMP_TR069_DATA_TYPE_String;
        ParamValue.Value         = &slapVar;

        SlapInitVariable(&slapVar);
        slapVar.Syntax           = SLAP_VAR_SYNTAX_string;
        slapVar.Variant.varString= value;

        returnStatus =
            pCcspCwmpMpaIf->SetParameterValues
                (
                    pCcspCwmpMpaIf->hOwnerContext,
                    (ANSC_HANDLE)&ParamValue,
                    ulArraySize,
                    &iStatus,
                    (ANSC_HANDLE*)&pCwmpSoapFault,
                    FALSE
                );

        if ( returnStatus != ANSC_STATUS_SUCCESS && pCwmpSoapFault )
        {
            unsigned int j;
            for ( j = 0; j < pCwmpSoapFault->SetParamValuesFaultCount; j ++ )
            {
                CcspTr069PaTraceError(("FactoryReset - SPV failed on parameter %s\n", pCwmpSoapFault->SetParamValuesFaultArray[j].ParameterName));
                CcspCwmpCleanSetParamFault((&pCwmpSoapFault->SetParamValuesFaultArray[j]));
            }
            pCwmpSoapFault->SetParamValuesFaultCount = 0;
        }
    }

    /*
     * Instead of sending back the response SOAP envelope right away in the same context, we create
     * an Asynchronous Response structure and let another dedicated task manage the message flow.
     */
    pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPSO_ASYNC_RESPONSE));

    if ( !pWmpsoAsyncRep )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }
    else
    {
        pWmpsoAsyncRep->Method       = CCSP_CWMP_METHOD_FactoryReset;
        pWmpsoAsyncRep->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapRep_FactoryReset
                (
                    pCcspCwmpSoapParser->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)pCwmpSoapFault
                );
    }

    if ( !pWmpsoAsyncRep->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT3;
    }

    CcspTr069PaTraceDebug(("CPE Response:\n%s\n", pWmpsoAsyncRep->SoapEnvelope));

    AnscAcquireLock   (&pMyObject->AsyncRepQueueLock);
    AnscQueuePushEntry(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
    AnscReleaseLock   (&pMyObject->AsyncRepQueueLock);

    AnscSetEvent(&pMyObject->AsyncProcessEvent);

    returnStatus = ANSC_STATUS_SUCCESS;

    goto  EXIT2;

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT3:

    if ( pWmpsoAsyncRep )
    {
        CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
        pWmpsoAsyncRep = NULL;
    }

EXIT2:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

EXIT1:

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


ULONG
CcspCwmpsoMcoGetParamDataType
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;

    return pCcspCwmpCpeController->GetParamDataType((ANSC_HANDLE)pCcspCwmpCpeController, pParamName);
}

#ifdef DONT_HAVE_RM
void
CcspCwmpsoAsyncReboot()
{
    AnscSleep(3*1000);
    system("/rdklogger/backupLogs.sh");
    //system("reboot");
}
#endif

