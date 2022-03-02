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

    module:	ccsp_cwmp_sesso_process.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced process functions
        of the CCSP CWMP Session Object.

        *   CcspCwmpsoRecvSoapMessage
        *   CcspCwmpsoNotifySessionClosed
        *   CcspCwmpsoAsyncProcessTask

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
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_sesso_global.h"

#define TR069_QUIRKS

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoRecvSoapMessage
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pMessage
            );

    description:

        This function is called to process a CWMP SOAP message.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pMessage
                Specifies the SOAP message to be processed.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoRecvSoapMessage
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pMessage
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT     )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pSessionTimerObj   = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hSessionTimerObj;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT     )pCcspCwmpCpeController->hCcspCwmpSoapParser;

    pSessionTimerObj->Stop((ANSC_HANDLE)pSessionTimerObj);

    if ( !pMessage || (AnscSizeOfString(pMessage) == 0) )
    {
        pMyObject->bNoMoreRequests = TRUE;
        pMyObject->bHoldRequests   = FALSE;
    }
    else
    {
        returnStatus =
            pCcspCwmpSoapParser->ProcessSoapEnvelopes
                (
                    (ANSC_HANDLE)pCcspCwmpSoapParser,
                    pMessage,
                    pMyObject->CpeMaxEnvelopes,
                    pMyObject->hCcspCwmpMcoIf
                );

        /*
         * If the CPE receives an HTTP response from the ACS for which the XML (for any contained
         * envelope) does not properly parse, or for which the SOAP structure is deemed invalid,
         * the CPE must consider the session terminated unsuccessfully.
         */
        if ( returnStatus != ANSC_STATUS_SUCCESS && 
             ((pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_connected) ||
             (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_informed )) )
        {
            pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;

            pCcspCwmpProcessor->SignalSession
                (
                    (ANSC_HANDLE)pCcspCwmpProcessor,
                    (ANSC_HANDLE)pMyObject
                );
        }
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoNotifySessionClosed
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called by the AcsConnection when the ACS session
        drops out unexpectedly.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoNotifySessionClosed
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT     )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pSessionTimerObj   = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hSessionTimerObj;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    
    CcspTr069PaTraceDebug(("CcspCwmpsoNotifySessionClosed -- session state %lu.\n", pMyObject->SessionState));

    pSessionTimerObj->Stop((ANSC_HANDLE)pSessionTimerObj);

/*
    if ( (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_connected) ||
         (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_informed ) )
    {
*/

        /* we should always retry if there's anything wrong with this session */
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
        returnStatus            =
            pCcspCwmpProcessor->SignalSession
                (
                    (ANSC_HANDLE)pCcspCwmpProcessor,
                    (ANSC_HANDLE)pMyObject
                );
/*
    }
*/

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoAsyncProcessTask
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This task is created to stream-line SOAP requests and responses
        and construct SOAP messages sent to the ACS.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoAsyncProcessTask
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT     )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pSessionTimerObj   = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hSessionTimerObj;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pCcspCwmpAcsConnection = (PCCSP_CWMP_ACS_CONNECTION_OBJECT  )pMyObject->hCcspCwmpAcsConnection;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq     = (PCCSP_CWMPSO_ASYNC_REQUEST    )NULL;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep     = (PCCSP_CWMPSO_ASYNC_RESPONSE   )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry        = (PSINGLE_LINK_ENTRY           )NULL;
    ULONG                           ulReqEnvelopeCount = (ULONG                        )0;
    ULONG                           ulRepEnvelopeCount = (ULONG                        )0;
    char*                           pSoapMessage       = (char*                        )NULL;
    char*                           pMethodName        = (char*                        )NULL;
    ULONG                           ulMessageSize      = 20480;
    ULONG                           ulAvailableSize    = 20480;
    PCCSP_CWMP_CFG_INTERFACE        pCcspCwmpCfgIf     = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpCfgIf;
 
    CcspTr069PaTraceDebug(("CcspCwmpsoAsyncProcessTask -- This is the beginning.\n"));
    errno_t rc       = -1;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCwmpRpcTimeout )
    {
        ULONG						ulRpcTimeout = pCcspCwmpCfgIf->GetCwmpRpcTimeout(pCcspCwmpCfgIf->hOwnerContext);

        if ( ulRpcTimeout > CCSP_CWMPSO_SESSION_TIMEOUT )
        {
        	pSessionTimerObj->SetInterval ((ANSC_HANDLE)pSessionTimerObj, ulRpcTimeout * 1000);
		}
    }

    while ( pMyObject->bActive )
    {
        AnscWaitEvent(&pMyObject->AsyncProcessEvent, 200);

        /*
         * The CPE MUST terminate the transaction session when all of the following conditions are
         * met:
         *
         *      (1) The ACS has no further requests to send the CPE. The CPE concludes
         *          this if either one of the following is true:
         *          a. The most recent HTTP response from the ACS contains no envelopes.
         *          b. The most recent envelope received from the ACS includes a NoMore-
         *             Requests header equal true. Use of this header by a CPE is
         *             OPTIONAL.
         *      (2) The CPE has no further requests to send to the ACS.
         *      (3) The CPE has received all outstanding response messages from the ACS.
         *      (4) The CPE has sent all outstanding response message to the ACS
         *          resulting from prior requests.
         */
        if ( pMyObject->bNoMoreRequests                            &&
             (AnscQueueQueryDepth(&pMyObject->AsyncReqQueue) == 0) &&
             /* (AnscQueueQueryDepth(&pMyObject->SavedReqQueue) == 0) && */
             (AnscQueueQueryDepth(&pMyObject->AsyncRepQueue) == 0) )
        {
            /*
             *  This behavior has been changed for a couple of times:
             *      1) Initially we do not an empty request
             *      2) Upon WT-151 requirement, we have to send out an empty request to allow
             *          ACS server to gracefully tear down the session
             *      3) Then we changed it to only send out one empty request
             *      4) Still, we ran into a few issues in plugfest when ACS server receives our empty request:
             *          a. Some ACS server sends back 'No Content' response
             *          b. Some ACS server senss the next request
             *         To solve all these, this sending 'Empty Request' is disabled.
             *
             */
            if( AnscQueueQueryDepth(&pMyObject->SavedReqQueue) > 0)
            {
                CcspTr069PaTraceWarning(("There's a CPE request waiting for the response but ACS returned empty message already.\n"));
            }

            if ( !pMyObject->bLastEmptyRequestSent)
            {
                pCcspCwmpAcsConnection->RequestOnly
                    (
                        (ANSC_HANDLE)pCcspCwmpAcsConnection
                    );

                pMyObject->bLastEmptyRequestSent = TRUE;

                AnscSleep(500);
            }

            pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_disconnectNow;
            pCcspCwmpProcessor->SignalSession
                (
                    (ANSC_HANDLE)pCcspCwmpProcessor,
                    (ANSC_HANDLE)pMyObject
                );

            break;
        }

        ulReqEnvelopeCount = 0;
        ulRepEnvelopeCount = 0;

        /*
         * CWMP allows the pipeline and aggregation of multiple requests and responses in the same
         * message. The number of SOAP envelopes is limited by the corresponding "MaxEnvelopes"
         * value exchanged in Inform() call. On the CPE side, we send a HTTP POST message with a
         * mix of requests and responses whenever one of following conditions is met:
         *
         *      - There has been a request made by the CPE to the ACS.
         *      - The number of responses exceeds or equals ACS's "MaxEnvelopes" limit.
         *      - There's at least one queued response and no active response.
         */
        if (  (AnscQueueQueryDepth(&pMyObject->AsyncReqQueue) >  0                         ) ||
              (AnscQueueQueryDepth(&pMyObject->AsyncRepQueue) >= pMyObject->AcsMaxEnvelopes) ||
             ((AnscQueueQueryDepth(&pMyObject->AsyncRepQueue) >  0                         ) &&
              (pMyObject->ActiveResponses                     == 0                         )) )
        {
            if ( pMyObject->AcsMaxEnvelopes > 1 )
            {
                pSoapMessage = (char*)AnscAllocateMemory(ulMessageSize);
            }
            else
            {
                pSoapMessage = NULL;
            }
            pMethodName  = NULL;

            if ( !pSoapMessage && pMyObject->AcsMaxEnvelopes > 1 )
            {
                continue;
            }
            else
            {
                ulAvailableSize = ulMessageSize - 3;    /* count out the NULL terminator and "\r\n" */
            }

            /*
             * We first add all queued responses into the message, then we add the pending requests
             * until we reach the "MaxEnvelopes" limit...
             */
            while ( pMyObject->bActive && ((ulReqEnvelopeCount + ulRepEnvelopeCount) < pMyObject->AcsMaxEnvelopes) )
            {
                AnscAcquireLock(&pMyObject->AsyncRepQueueLock);
                pSLinkEntry = AnscQueuePopEntry(&pMyObject->AsyncRepQueue);
                AnscReleaseLock(&pMyObject->AsyncRepQueueLock);

                if ( !pSLinkEntry )
                {
                    break;
                }
                else
                {
                    pWmpsoAsyncRep = ACCESS_CCSP_CWMPSO_ASYNC_RESPONSE(pSLinkEntry);
                }

                if ( pMyObject->AcsMaxEnvelopes <= 1 )
                {
                    pSoapMessage = pWmpsoAsyncRep->SoapEnvelope; 
                    pWmpsoAsyncRep->SoapEnvelope = NULL;

                    ulRepEnvelopeCount++;
                }
                else
                if ( AnscSizeOfString(pWmpsoAsyncRep->SoapEnvelope) > ulAvailableSize )
                {
                    if ( (ulReqEnvelopeCount + ulRepEnvelopeCount) == 0 )
                    {
                        ulMessageSize *= 2;
                    }

                    AnscAcquireLock          (&pMyObject->AsyncRepQueueLock);
                    AnscQueuePushEntryAtFront(&pMyObject->AsyncRepQueue, &pWmpsoAsyncRep->Linkage);
                    AnscReleaseLock          (&pMyObject->AsyncRepQueueLock);

                    break;
                }
                else
                {
                     rc = strcat_s(pSoapMessage,ulMessageSize,pWmpsoAsyncReq->SoapEnvelope);
                     if(rc!=EOK)
                    {
                       ERR_CHK(rc);
                       goto EXIT;
                    }
                    rc = strcat_s(pSoapMessage,ulMessageSize,"\r\n");
                     if(rc!=EOK)
                    {
                       ERR_CHK(rc);
                       goto EXIT;
                    }

                    ulAvailableSize -= AnscSizeOfString(pWmpsoAsyncRep->SoapEnvelope);
                    ulAvailableSize -= 2;
                    ulRepEnvelopeCount++;
                }

                if( pWmpsoAsyncRep )
                {
                    CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
                    pWmpsoAsyncRep = NULL;
                }
            }

            while ( pMyObject->bActive && ((ulReqEnvelopeCount + ulRepEnvelopeCount) < pMyObject->AcsMaxEnvelopes) && !pMyObject->bHoldRequests )
            {
                AnscAcquireLock(&pMyObject->AsyncReqQueueLock);
                pSLinkEntry = AnscQueuePopEntry(&pMyObject->AsyncReqQueue);
                AnscReleaseLock(&pMyObject->AsyncReqQueueLock);

                if ( !pSLinkEntry )
                {
                    break;
                }
                else
                {
                    pWmpsoAsyncReq = ACCESS_CCSP_CWMPSO_ASYNC_REQUEST(pSLinkEntry);
                }

                if ( pMyObject->AcsMaxEnvelopes <= 1 )
                {
                    /* We have to keep "SoapEnvelope" in order to resend if requires */
                    pSoapMessage = AnscCloneString(pWmpsoAsyncReq->SoapEnvelope);

                    ulReqEnvelopeCount++;

                    if ( ulReqEnvelopeCount == 1 )
                    {
                        pMethodName = AnscCloneString(pWmpsoAsyncReq->MethodName);
                    }
                }
                else
                if ( AnscSizeOfString(pWmpsoAsyncReq->SoapEnvelope) > ulAvailableSize )
                {
                    if ( (ulReqEnvelopeCount + ulRepEnvelopeCount) == 0 )
                    {
                        ulMessageSize *= 2;
                    }

                    AnscAcquireLock          (&pMyObject->AsyncReqQueueLock);
                    AnscQueuePushEntryAtFront(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
                    AnscReleaseLock          (&pMyObject->AsyncReqQueueLock);

                    break;
                }
                else
                {
                     rc = strcat_s(pSoapMessage,ulMessageSize,pWmpsoAsyncReq->SoapEnvelope);
                     if(rc!=EOK)
                    {
                       ERR_CHK(rc);
                       goto EXIT;
                    }
                    rc = strcat_s(pSoapMessage,ulMessageSize,"\r\n");
                     if(rc!=EOK)
                    {
                       ERR_CHK(rc);
                       goto EXIT;
                    }

                    if ( ulReqEnvelopeCount == 0 )
                    {
                        pMethodName = AnscCloneString(pWmpsoAsyncReq->MethodName);
                    }

                    ulAvailableSize -= AnscSizeOfString(pWmpsoAsyncReq->SoapEnvelope);
                    ulAvailableSize -= 2;
                    ulReqEnvelopeCount++;
                }

                AnscAcquireLock   (&pMyObject->SavedReqQueueLock);
                AnscQueuePushEntry(&pMyObject->SavedReqQueue, &pWmpsoAsyncReq->Linkage);
                AnscReleaseLock   (&pMyObject->SavedReqQueueLock);
            }

            /*
             * Now it's time to send out the SOAP message. The CcspCwmpAcsConnection object will fill
             * in the HTTP header information...
             */
            CcspTr069PaTraceDebug(("CcspCwmpsoAsyncProcessTask -- Calling AcsConnection->Request.\n"));

            if ( (ulReqEnvelopeCount + ulRepEnvelopeCount) > 0 )
            {
                returnStatus =
                    pCcspCwmpAcsConnection->Request
                        (
                            (ANSC_HANDLE)pCcspCwmpAcsConnection,
                            pSoapMessage,
                            pMethodName,
                            ulReqEnvelopeCount,
                            ulRepEnvelopeCount
                        );
            }
            else
            {
                returnStatus =
                    pCcspCwmpAcsConnection->Request
                        (
                            (ANSC_HANDLE)pCcspCwmpAcsConnection,
                            NULL,
                            NULL,
                            ulReqEnvelopeCount,
                            ulRepEnvelopeCount
                        );

                pMyObject->bLastEmptyRequestSent = TRUE;
            }

            if ( pMethodName )
            {
                AnscFreeMemory(pMethodName);
            }

            if ( pSoapMessage )
            {
                AnscFreeMemory(pSoapMessage);
                pSoapMessage = NULL;
            }

            if ( returnStatus != ANSC_STATUS_SUCCESS )
            {
                break;
            }

            pSessionTimerObj->Start((ANSC_HANDLE)pSessionTimerObj);
        }
        else
        {
            returnStatus =
                pCcspCwmpAcsConnection->Request
                    (
                        (ANSC_HANDLE)pCcspCwmpAcsConnection,
                        NULL,
                        NULL,
                        0,
                        0
                    );

            if ( returnStatus != ANSC_STATUS_SUCCESS )
            {
                break;
            }

            pMyObject->bLastEmptyRequestSent = TRUE;

            pSessionTimerObj->Start((ANSC_HANDLE)pSessionTimerObj);
        }

        AnscResetEvent(&pMyObject->AsyncProcessEvent);
    }

    /*
     * Release all the async requests...
     */
    AnscAcquireLock(&pMyObject->AsyncReqQueueLock);

    pSLinkEntry = AnscQueuePopEntry(&pMyObject->AsyncReqQueue);

    while ( pSLinkEntry )
    {
        pWmpsoAsyncReq = ACCESS_CCSP_CWMPSO_ASYNC_REQUEST(pSLinkEntry);
        pSLinkEntry    = AnscQueuePopEntry(&pMyObject->AsyncReqQueue);

        CcspCwmpsoSignalAsyncRequest(pWmpsoAsyncReq, ANSC_STATUS_FAILURE);
    }

    AnscReleaseLock(&pMyObject->AsyncReqQueueLock);

    /*
     * Release all the saved requests...
     */
    AnscAcquireLock(&pMyObject->SavedReqQueueLock);

    pSLinkEntry = AnscQueuePopEntry(&pMyObject->SavedReqQueue);

    while ( pSLinkEntry )
    {
        pWmpsoAsyncReq = ACCESS_CCSP_CWMPSO_ASYNC_REQUEST(pSLinkEntry);
        pSLinkEntry    = AnscQueuePopEntry(&pMyObject->SavedReqQueue);

        CcspCwmpsoSignalAsyncRequest(pWmpsoAsyncReq, ANSC_STATUS_FAILURE);
    }

    AnscReleaseLock(&pMyObject->SavedReqQueueLock);

    /*
     * Release all the async responses...
     */
    AnscAcquireLock(&pMyObject->AsyncRepQueueLock);

    pSLinkEntry = AnscQueuePopEntry(&pMyObject->AsyncRepQueue);

    while ( pSLinkEntry )
    {
        pWmpsoAsyncRep = ACCESS_CCSP_CWMPSO_ASYNC_RESPONSE(pSLinkEntry);
        pSLinkEntry    = AnscQueuePopEntry(&pMyObject->AsyncRepQueue);

        if( pWmpsoAsyncRep )
        {
            CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
            pWmpsoAsyncRep = NULL;

        }
    }

    AnscReleaseLock(&pMyObject->AsyncRepQueueLock);

    pMyObject->AsyncTaskCount--;

    CcspTr069PaTraceDebug(("CcspCwmpsoAsyncProcessTask -- Exit.\n"));

    pMyObject->bLastEmptyRequestSent = FALSE;

    return  ANSC_STATUS_SUCCESS;

EXIT:

    if ( pSoapMessage )
    {
        AnscFreeMemory(pSoapMessage);
    }
    return  ANSC_STATUS_FAILURE;
}

