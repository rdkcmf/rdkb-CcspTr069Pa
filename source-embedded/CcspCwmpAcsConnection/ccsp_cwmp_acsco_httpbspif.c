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

    module: ccsp_cwmp_acsco_httpbspif.c

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This module implements the advanced interface functions
        of the CCSP CWMP ACS Connection Object.

        *   CcspCwmpAcscoHttpBspPolish
        *   CcspCwmpAcscoHttpBspBrowse
        *   CcspCwmpAcscoHttpBspNotify
        *   CcspCwmpAcscoHttpAddCookie
        *   CcspCwmpAcscoHttpRemoveCookies
        *   CcspCwmpAcscoHttpSessionClosed

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/21/05    initial revision.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_acsco_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoHttpBspPolish
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hBmoReq,
                ANSC_HANDLE                 hReqContext
            );

    description:

        This function is called to polish the client request message
        before sending out.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hBmoReq
                Specifies the request message object to be polished.

                ANSC_HANDLE                 hReqContext
                Specifies the request-specific context handle provided
                by the owner.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcscoHttpBspPolish
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hBmoReq,
        ANSC_HANDLE                 hReqContext
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PANSC_ACS_INTERN_HTTP_CONTENT    pHttpGetReq     = (PANSC_ACS_INTERN_HTTP_CONTENT)hReqContext;
    ANSC_STATUS                      returnStatus    = ANSC_STATUS_SUCCESS;
    PHTTP_BMO_REQ_OBJECT             pBmoReqObj      = (PHTTP_BMO_REQ_OBJECT)hBmoReq;
    char                             pBuffer[64]     = { 0 };
    ULONG                            i, len          = 0;
    errno_t                          rc              = -1;

    /* add Authorization header - last good one */
    if ( pMyObject->AuthHeaderValue )
    {
        pBmoReqObj->SetHeaderValueByName((ANSC_HANDLE)pBmoReqObj, "Authorization", pMyObject->AuthHeaderValue);
    }

    /*
     * If there's a cookie returned, add the cookie
     */
    for ( i = 0; i < pMyObject->NumCookies; i ++ )
    {
    	if ( pMyObject->Cookies[i] == NULL )
    		break;

    	len += _ansc_strlen(pMyObject->Cookies[i]);
    }

    if ( pMyObject->NumCookies == 0 )
    {
        CcspTr069PaTraceDebug(("No Cookie will be added.\n"));
    }
    else
    {
        size_t  size_cookies    = len + pMyObject->NumCookies*2;
    	char    *cookies        = (char *)AnscAllocateMemory(size_cookies);

    	if (cookies) {
            for ( i = 0; i < pMyObject->NumCookies; i ++ )
            {
            	if ( pMyObject->Cookies[i] == NULL )
            		break;

                 rc = strcat_s(cookies, size_cookies, pMyObject->Cookies[i]);
                if(rc!=EOK)
               {
                 ERR_CHK(rc);
                 AnscFreeMemory(cookies);
                 return ANSC_STATUS_FAILURE;
               }
            	if ( i < pMyObject->NumCookies - 1 )
                {
                    rc = strcat_s(cookies, size_cookies, "; ");
                    if(rc!=EOK)
                    {
                        ERR_CHK(rc);
                        AnscFreeMemory(cookies);
                        return ANSC_STATUS_FAILURE;
                    }
                }
            }

            pBmoReqObj->SetHeaderValueByName
				(
					(ANSC_HANDLE)pBmoReqObj,
					"Cookie",
					cookies
				);

			CcspTr069PaTraceDebug(("Add Cookie into message: %s\n", cookies));
			AnscFreeMemory(cookies);
    	}
    }

    /* When there is more than one envelope in a single HTTP Request,
     * when there is a SOAP response in an HTTP Request, or when there is a
     * SOAP Fault response in an HTTP Request, the SOAPAction header in the
     * HTTP Request MUST have no value (with no quotes), indicating that this
     * header provides no information as to the intent of the message."
     */
    if( pHttpGetReq ==NULL || pHttpGetReq->SoapMessage == NULL || AnscSizeOfString(pHttpGetReq->SoapMessage) == 0)
    {
        /*
         * An empty HTTP POST MUST NOT contain a SOAPAction header.
         * An empty HTTP POST MUST NOT contain a Content-Type header.
         */
        pBmoReqObj->SetHeaderValueByName((ANSC_HANDLE)pBmoReqObj, "Content-Length", "0");
        /* pBmoReqObj->SetHeaderValueByName((ANSC_HANDLE)pBmoReqObj, "Content-Type", "text/xml;charset=utf-8"); */
    }
    else
    {
        pBmoReqObj->SetHeaderValueByName((ANSC_HANDLE)pBmoReqObj, "Content-Type", "text/xml;charset=utf-8");

        _ansc_sprintf(pBuffer, "%u", (unsigned int)AnscSizeOfString(pHttpGetReq->SoapMessage));
        pBmoReqObj->SetHeaderValueByName((ANSC_HANDLE)pBmoReqObj, "Content-Length", pBuffer);

        if( pHttpGetReq->MethodName == NULL || AnscSizeOfString(pHttpGetReq->MethodName) == 0)
        {
            pBmoReqObj->SetHeaderValueByName((ANSC_HANDLE)pBmoReqObj, "SOAPAction", "");
        }
        else
        {
            _ansc_sprintf(pBuffer, "\"%s\"", pHttpGetReq->MethodName);
            pBmoReqObj->SetHeaderValueByName((ANSC_HANDLE)pBmoReqObj, "SOAPAction", pBuffer);
        }

        returnStatus =
            pBmoReqObj->AppendBody
                (
                    pBmoReqObj,
                    pHttpGetReq->SoapMessage,
                    AnscSizeOfString(pHttpGetReq->SoapMessage)
                );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoHttpBspBrowse
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hBmoReq,
                ANSC_HANDLE                 hBmoRep,
                ANSC_HANDLE                 hReqContext
            );

    description:

        This function is called to browse the response received from
        the web server.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hBmoReq
                Specifies the request message object that triggerred
                the response.

                ANSC_HANDLE                 hBmoRep
                Specifies the response message to be processed.

                ANSC_HANDLE                 hReqContext
                Specifies the request-specific context handle provided
                by the owner.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcscoHttpBspBrowse
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hBmoReq,
        ANSC_HANDLE                 hBmoRep,
        ANSC_HANDLE                 hReqContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject         = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PHTTP_BMO_REQ_OBJECT            pHttpBmoReq       = (PHTTP_BMO_REQ_OBJECT      )hBmoReq;
    PHTTP_BMO_REP_OBJECT            pHttpBmoRep       = (PHTTP_BMO_REP_OBJECT      )hBmoRep;
    PANSC_ACS_INTERN_HTTP_CONTENT   pHttpGetReq       = (PANSC_ACS_INTERN_HTTP_CONTENT )hReqContext;
    ULONG                           ulResponseSize    = (ULONG                     )pHttpBmoRep->GetBodySize((ANSC_HANDLE)pHttpBmoRep);
    char*                           pHttpResponse     = (char*                     )NULL;
    char*                           pHeaderLocation   = NULL;
    char*                           pCookie           = NULL;
    ULONG                           ulCode            = 0;
    char*                           pCookieHeader     = NULL;
    ULONG                           ulCookieIndex     = 0;
//    BOOL                            bCookiesRemoved   = FALSE;

    if ( pHttpGetReq == NULL)
    {
        return ANSC_STATUS_FAILURE;
    }

    ulCode = pHttpBmoRep->GetCode((ANSC_HANDLE)pHttpBmoRep);

    if( HTTP_STATUS_NO_CONTENT == ulCode) /* ACS has nothing to say */
    {
        if( pHttpGetReq != NULL)
        {
            pHttpGetReq->ulContentSize = 0;
            pHttpGetReq->pContent      = NULL;
            pHttpGetReq->bIsRedirect   = FALSE;
        }

        returnStatus = ANSC_STATUS_SUCCESS;

        goto EXIT1;
    }
    else if( HTTP_STATUS_MOVED_PERMANENTLY != ulCode &&
             HTTP_STATUS_FOUND != ulCode             &&
             HTTP_STATUS_OK != ulCode                &&
             HTTP_STATUS_UNAUTHORIZED != ulCode		 &&
			 HTTP_STATUS_TEMP_REDIRECT != ulCode
           )
    {
        returnStatus = ANSC_STATUS_FAILURE;

        CcspTr069PaTraceDebug(("HttpClient returned code: %d\n", (int)ulCode));

        goto  EXIT1;
    }

    /* save Authorization header value */
    if ( TRUE )
    {
        char*                       pAuthHeaderValue   = NULL;

        /* Authorization header in request */
        pAuthHeaderValue = pHttpBmoReq->GetHeaderValueById((ANSC_HANDLE)pHttpBmoReq, HTTP_HEADER_ID_AUTHORIZATION);

        if ( pMyObject->AuthHeaderValue ) AnscFreeMemory(pMyObject->AuthHeaderValue);
        pMyObject->AuthHeaderValue = pAuthHeaderValue ? AnscCloneString(pAuthHeaderValue) : NULL;
    }

    /* look for Set-Cookie headers */
    pCookie = pHttpBmoRep->GetHeaderValueById2((ANSC_HANDLE)pHttpBmoRep, HTTP_HEADER_ID_SET_COOKIE2, ulCookieIndex);

    if ( pCookie && AnscSizeOfString(pCookie) > 0 )
    {
        /*
        pMyObject->RemoveCookies((ANSC_HANDLE)pMyObject);
        bCookiesRemoved = TRUE;
        */

        while ( pCookie != NULL && AnscSizeOfString(pCookie) > 0)
        {
            pCookieHeader = AnscAllocateMemory(AnscSizeOfString(pCookie) + 16);

            if ( pCookieHeader )
                _ansc_sprintf(pCookieHeader, "Set-Cookie2: %s", pCookie);

            pMyObject->AddCookie((ANSC_HANDLE)pMyObject, pCookieHeader);
            AnscFreeMemory(pCookieHeader);

            pCookie = pHttpBmoRep->GetHeaderValueById2((ANSC_HANDLE)pHttpBmoRep, HTTP_HEADER_ID_SET_COOKIE2, ++ulCookieIndex);
        }
    }

    /* If Set-Cookie and Set-Cookie2 co-exist happily, we save them all */
    ulCookieIndex = 0;
    pCookie = pHttpBmoRep->GetHeaderValueById2((ANSC_HANDLE)pHttpBmoRep, HTTP_HEADER_ID_SET_COOKIE, ulCookieIndex);

    if( pCookie != NULL && AnscSizeOfString(pCookie) > 0)
    {
        /*
        if ( !bCookiesRemoved )
        {
            pMyObject->RemoveCookies((ANSC_HANDLE)pMyObject);
            bCookiesRemoved = TRUE;
        }
        */

        while ( pCookie && AnscSizeOfString(pCookie) )
        {
            pCookieHeader = AnscAllocateMemory(AnscSizeOfString(pCookie) + 16);

            if ( pCookieHeader )
                _ansc_sprintf(pCookieHeader, "Set-Cookie: %s", pCookie);

            pMyObject->AddCookie((ANSC_HANDLE)pMyObject, pCookieHeader);
            AnscFreeMemory(pCookieHeader);

            pCookie = pHttpBmoRep->GetHeaderValueById2((ANSC_HANDLE)pHttpBmoRep, HTTP_HEADER_ID_SET_COOKIE, ++ulCookieIndex);
        }
    }

    /*
    if ( !bCookiesRemoved )
    {
        CcspTr069PaTraceDebug(("No Cookie in the response.\n"));

        if( pMyObject->NumCookies != 0)
        {
            for ( i = 0; i < pMyObject->NumCookies; i ++ )
            {
                CcspTr069PaTraceDebug(("Keep the old cookie: %s\n", pMyObject->Cookies[i]));
            }
        }
        else
        {
            CcspTr069PaTraceDebug(("???No cookie exists...\n"));
        }
    }
    */

    if( HTTP_STATUS_UNAUTHORIZED == ulCode)
    {
        /* make sure if server sends back challenge, terminates the session if otherwise */
        char*                       pWwwAuth = NULL;

        pHttpGetReq->bUnauthorized = TRUE;
        
        pWwwAuth = pHttpBmoRep->GetHeaderValueByName((ANSC_HANDLE)pHttpBmoRep, "WWW-Authenticate");
        if ( pWwwAuth )
        {
            returnStatus = ANSC_STATUS_DO_IT_AGAIN; /* ANSC_STATUS_SUCCESS; */
        }
        else
        {
            returnStatus = ANSC_STATUS_FAILURE;
        }

        goto EXIT1;
    }

    if( ulCode == HTTP_STATUS_MOVED_PERMANENTLY || HTTP_STATUS_FOUND == ulCode || HTTP_STATUS_TEMP_REDIRECT == ulCode)
    {
        pHeaderLocation     = pHttpBmoRep->GetHeaderValueByName((ANSC_HANDLE)pHttpBmoRep, "Location"     );

        if( pHeaderLocation == NULL || AnscSizeOfString(pHeaderLocation) == 0)
        {
            returnStatus = ANSC_STATUS_FAILURE;

            goto  EXIT1;
        }

        CcspTr069PaTraceInfo(("ACS URL moved (HTTP code=%lu) to: %s\n", ulCode, pHeaderLocation));

        pHttpGetReq->ulContentSize = AnscSizeOfString(pHeaderLocation);
        pHttpGetReq->pContent      = AnscCloneString(pHeaderLocation);
        pHttpGetReq->bIsRedirect   = TRUE;

        /* notify underlying HTTP Webc Transaction object to clearn request
         * otherwise, in case ACS closes the socket first, we will get notified
         * again and current session will be terminated prematurely.
         */
        returnStatus = ANSC_STATUS_NO_MORE_DATA;
    }
    else
    {
        if ( ulResponseSize == 0 )
        {
             returnStatus = ANSC_STATUS_FAILURE;

            goto  EXIT1;
        }
        else
        {
            pHttpResponse = (char*)AnscAllocateMemory(ulResponseSize + 1);  /* we must leave room for the NULL terminator */

            if ( !pHttpResponse )
            {
                returnStatus = ANSC_STATUS_RESOURCES;

                goto  EXIT1;
            }
            else
            {
                returnStatus =
                    pHttpBmoRep->CopyBodyFrom
                        (
                            (ANSC_HANDLE)pHttpBmoRep,
                            pHttpResponse,
                            &ulResponseSize
                        );
            }
        }

        pHttpGetReq->ulContentSize = ulResponseSize;
        pHttpGetReq->pContent      = (PVOID)pHttpResponse;
        pHttpGetReq->bIsRedirect   = FALSE;
    }

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    if ( pHttpGetReq )
    {
        pHttpGetReq->CompleteStatus = 
            (returnStatus == ANSC_STATUS_DO_IT_AGAIN || returnStatus == ANSC_STATUS_NO_MORE_DATA) ? ANSC_STATUS_SUCCESS : returnStatus;

        AnscSetEvent(&pHttpGetReq->CompleteEvent);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoHttpBspNotify
            (
                ANSC_HANDLE                 hThisObject,
                ULONG                       ulEvent,
                ULONG                       ulError,
                ANSC_HANDLE                 hReqContext,
                ANSC_HANDLE                 hBmoReq,
                ANSC_HANDLE                 hBmoRep
            );

    description:

        This function is called to notify the owner when something
        happens unexpectedly.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ULONG                       ulEvent
                Specifies the nature of the event to be processed.

                ULONG                       ulError
                Specifies the error code associated with the event.

                ANSC_HANDLE                 hReqContext
                Specifies the request-specific context handle provided
                by the owner.

                ANSC_HANDLE                 hBmoReq
                HTTP BMO request object if given.

                ANSC_HANDLE                 hBmoRep
                HTTP BMO response object if given.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcscoHttpBspNotify
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulEvent,
        ULONG                       ulError,
        ANSC_HANDLE                 hReqContext,
        ANSC_HANDLE                 hBmoReq,
        ANSC_HANDLE                 hBmoRep
    )
{
    UNREFERENCED_PARAMETER(ulError);
    UNREFERENCED_PARAMETER(hBmoReq);
    PANSC_ACS_INTERN_HTTP_CONTENT    pHttpGetReq            = (PANSC_ACS_INTERN_HTTP_CONTENT )hReqContext;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject              = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PCCSP_CWMP_SESSION_OBJECT        pWmpSession            = (PCCSP_CWMP_SESSION_OBJECT   )pMyObject->hCcspCwmpSession;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pWmpSession->hCcspCwmpCpeController;
    PCCSP_CWMP_STAT_INTERFACE        pCcspCwmpStatIf        = (PCCSP_CWMP_STAT_INTERFACE)pCcspCwmpCpeController->hCcspCwmpStaIf;
    PHTTP_BMO_REP_OBJECT             pHttpBmoRep            = (PHTTP_BMO_REP_OBJECT      )hBmoRep;

    switch ( ulEvent )
    {
        case    HTTP_BSP_EVENT_BAD_REQUEST :
        case    HTTP_BSP_EVENT_CANNOT_RESOLVE_NAME :
        case    HTTP_BSP_EVENT_SERVER_UNAVAILABLE :
        case    HTTP_BSP_EVENT_NETWORK_FAILURE :
        case    HTTP_BSP_EVENT_TIMEOUT :

                if( pHttpGetReq != NULL && pHttpGetReq->CompleteStatus == ANSC_STATUS_RESET_SESSION)
                {
                    break;
                }

                if( pHttpGetReq && pHttpGetReq->ulContentSize == 0)
                {
                    pHttpGetReq->CompleteStatus = ANSC_STATUS_FAILURE;

                    AnscSetEvent(&pHttpGetReq->CompleteEvent);
                }

                CcspTr069PaTraceDebug(("HttpClient Notify: %d\n", (int)ulEvent));

                if( pCcspCwmpStatIf)
                {
                    pCcspCwmpStatIf->IncTcpFailure(pCcspCwmpStatIf->hOwnerContext);
                }

                CcspCwmpAcscoHttpSessionClosed(hThisObject);

                break;

        case    HTTP_BSP_EVENT_SOCKET_CLOSED:

                /*
                 * During the test with Motive ACS, we observed that the web server
                 * close the connection after sending chanllenge back, which is an option.
                 *
                 * In that case, we need to start a new session and send back the authentication
                 * information.
                 */
                if( pHttpGetReq)
                {
                    char*           pHeaderLocation = NULL;
                    
                    if ( pHttpBmoRep )
                    {
                        pHeaderLocation = pHttpBmoRep->GetHeaderValueByName((ANSC_HANDLE)pHttpBmoRep, "Location");
                    }

                    if ( pHeaderLocation && AnscSizeOfString(pHeaderLocation) != 0 )
                    {
                        ULONG       ulCode = pHttpBmoRep->GetCode((ANSC_HANDLE)pHttpBmoRep);
                        CcspTr069PaTraceInfo(("ACS URL moved (HTTP code=%lu) to: %s\n", ulCode, pHeaderLocation));

                        pHttpGetReq->ulContentSize = AnscSizeOfString(pHeaderLocation);
                        pHttpGetReq->pContent      = AnscCloneString(pHeaderLocation);
                        pHttpGetReq->bIsRedirect   = TRUE;
                    }

                    pHttpGetReq->CompleteStatus = ANSC_STATUS_RESET_SESSION;

                    AnscSetEvent(&pHttpGetReq->CompleteEvent);
                }

                CcspTr069PaTraceDebug(("HttpClient Notify: SOCKET CLOSED\n"));

                /* CcspCwmpAcscoHttpSessionClosed(hThisObject); */

                break;

        case HTTP_BSP_EVENT_TLS_TIMEOUT:
        case HTTP_BSP_EVENT_TLS_ERROR:

                CcspTr069PaTraceDebug(("HttpClient TLS Notify: %lu\n", ulEvent));

                if( pCcspCwmpStatIf)
                {
                    pCcspCwmpStatIf->IncTlsFailure(pCcspCwmpStatIf->hOwnerContext);
                }

                if ( pHttpGetReq )
                {
                    pHttpGetReq->CompleteStatus = ANSC_STATUS_FAILURE;
                    AnscSetEvent(&pHttpGetReq->CompleteEvent);
                }

                break;


        default :

                CcspTr069PaTraceDebug(("HttpClient Notify: %lu\n", ulEvent));

                if ( pHttpGetReq )
                {
                    pHttpGetReq->CompleteStatus = ANSC_STATUS_FAILURE;
                    AnscSetEvent(&pHttpGetReq->CompleteEvent);
                }

                break;
    }

    return  ANSC_STATUS_SUCCESS;
}


#if 0
static BOOLEAN
copyStringAfterTrim
    (
        PCHAR                       pDest,
        PCHAR                       pSource
    )
{
    ULONG                           begin    = 0;
    ULONG                           end      = 0;
    ULONG                           length;

    if( pDest == NULL || pSource == NULL)
    {
        return FALSE;
    }

    length = AnscSizeOfString(pSource);

    if( length == 0)
    {
        return FALSE;
    }

    /* left */
    while(pSource[begin] == ' ')
    {
        begin ++;

        if( begin >= length)
        {
            return FALSE;
        }
    }

    /* right */
    end = length - 1;

    while( pSource[end] == ' ')
    {
        if( end == 0)
        {
            return FALSE;
        }

        end --;
    }

    if( begin > end )
    {
        return FALSE;
    }

    AnscCopyMemory
        (
            pDest,
            (PVOID)(pSource + begin),
            (end - begin + 1)
        );

    return TRUE;
}
#endif


/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoHttpAddCookie
            (
                ANSC_HANDLE                 hThisObject,
                PCHAR                       pCookie
            )

    description:

        This function is called to add a cookie which is going to
        be included in the future requests.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PCHAR                       pCookie
                Cookie to be added.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcscoHttpAddCookie
    (
        ANSC_HANDLE                 hThisObject,
        PCHAR                       pCookie
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject         = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PHTTP_SIMPLE_CLIENT_OBJECT      pHttpSimpleClient = (PHTTP_SIMPLE_CLIENT_OBJECT)pMyObject->hHttpSimpleClient;
    PHTTP_HFP_INTERFACE             pHttpHfpIf        = (PHTTP_HFP_INTERFACE)pHttpSimpleClient->GetHfpIf((ANSC_HANDLE)pHttpSimpleClient);
    char*                           pCookieValue      = NULL;
    PHTTP_HFO_SET_COOKIE            pHfoSetCookie     = NULL;

    if ( !pCookie || AnscSizeOfString(pCookie) <= 5)
    {
        CcspTr069PaTraceDebug(("!!!Empty Cookie, ignored.\n"));

        return ANSC_STATUS_SUCCESS;
    }

    if ( pMyObject->NumCookies >= CCSP_CWMP_ACSCO_MAX_COOKIE )
    {
        CcspTr069PaTraceDebug(("!!!Too many cookies, over the limit %d.\n", CCSP_CWMP_ACSCO_MAX_COOKIE));

        return ANSC_STATUS_DISCARD;
    }

    pCookieValue = (PCHAR)AnscAllocateMemory(AnscSizeOfString(pCookie) + 64);

    if( NULL == pCookieValue )
    {
        return ANSC_STATUS_RESOURCES;
    }

    pHfoSetCookie = (PHTTP_HFO_SET_COOKIE)pHttpHfpIf->ParseHeader(pHttpHfpIf->hOwnerContext, pCookie, AnscSizeOfString(pCookie));

    if ( pHfoSetCookie )
    {
        ULONG                       ulCookieSize = 0;
        int                         nIndex;
        ULONG                       i;

        /* play a trick here - suppose the definitions of Cookie and Set-Cookie/2 are the same */
        pHfoSetCookie->HeaderId = HTTP_HEADER_ID_COOKIE;
        pHfoSetCookie->Flags &= ~HTTP_FIELD_FLAG_VALUE_PRESENT;
        pHfoSetCookie->Flags &= ~HTTP_FIELD_FLAG_LINE_PRESENT;

        ulCookieSize = pHttpHfpIf->GetHeaderSize(pHttpHfpIf->hOwnerContext, (ANSC_HANDLE)pHfoSetCookie);
        pHttpHfpIf->BuildHeader(pHttpHfpIf->hOwnerContext, (ANSC_HANDLE)pHfoSetCookie, pCookieValue, ulCookieSize);
        pCookieValue[ulCookieSize] = 0;

        /* remove old cookies */
        for ( i = 0; i < pHfoSetCookie->CookieCount; i ++ )
        {
            nIndex = pMyObject->FindCookie((ANSC_HANDLE)pMyObject, pHfoSetCookie->CookieArray[i].Name);
            if ( nIndex >= 0 )
            {
                pMyObject->DelCookie((ANSC_HANDLE)pMyObject, (ULONG)nIndex);
            }
        }

        pMyObject->Cookies[pMyObject->NumCookies++] = AnscCloneString(pCookieValue + 8);

        AnscFreeMemory(pHfoSetCookie);
    }

    AnscFreeMemory(pCookieValue);

    return returnStatus;
}


/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoHttpRemoveCookies
            (
                ANSC_HANDLE                 hThisObject
            )

    description:

        This function is called to remove saved cookies.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcscoHttpRemoveCookies
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject         = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    ULONG                           i;

    for ( i = 0; i < pMyObject->NumCookies; i ++ )
    {
        if ( pMyObject->Cookies[i] )
        {
            AnscFreeMemory(pMyObject->Cookies[i]);
            pMyObject->Cookies[i] = NULL;
        }
    }
    pMyObject->NumCookies = 0;

    return returnStatus;
}


/**********************************************************************

    prototype:

        int
        CcspCwmpAcscoHttpFindCookie
            (
                ANSC_HANDLE                 hThisObject,
                PCHAR                       pCookieName
            )

    description:

        This function is called to find cookie with specified name.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PCHAR                       pCookieName

    return:     index in cookie array, -1 if cookie with
                specified name is not found.

**********************************************************************/

int
CcspCwmpAcscoHttpFindCookie
    (
        ANSC_HANDLE                 hThisObject,
        PCHAR                       pCookieName
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject         = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    ULONG                           i;
    PCHAR                           pValue;
    ULONG                           ulCookieNameLen   = AnscSizeOfString(pCookieName);

    for ( i = 0; i < pMyObject->NumCookies; i ++ )
    {
        if ( pMyObject->Cookies[i] )
        {
            pValue = _ansc_strstr(pMyObject->Cookies[i], pCookieName);

			if ( !pValue ) continue;

            if ( ( pValue == pMyObject->Cookies[i] || *(pValue-1) == ' ' || *(pValue-1) == ';' ) && 
                 *(pValue + ulCookieNameLen) == '=' )
            {
                return  (int)i;
            }
        }
    }

    return -1;
}


/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoHttpDelCookie
            (
                ANSC_HANDLE                 hThisObject,
                ULONG                       ulIndex
            )

    description:

        This function is called to delete the specified cookie.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ULONG                       ulIndex
                The index to the cookie to be deleted.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcscoHttpDelCookie
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulIndex
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject         = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    ULONG                           i;
    PCHAR                           pCookie           = NULL;

    if ( ulIndex >= pMyObject->NumCookies )
    {
        return  ANSC_STATUS_BAD_PARAMETER;
    }

    pCookie = pMyObject->Cookies[ulIndex];

    for ( i = ulIndex; i < pMyObject->NumCookies - 1; i ++ )
    {
        pMyObject->Cookies[i] = pMyObject->Cookies[i+1]; 
    }

    pMyObject->NumCookies --;

    if ( pCookie )
    {
        AnscFreeMemory(pCookie);
    }

    return ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoHttpSessionClosed
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called whenever the http session is closed

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/
ANSC_STATUS
CcspCwmpAcscoHttpSessionClosed
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PCCSP_CWMP_SESSION_OBJECT        pWmpSession     = (PCCSP_CWMP_SESSION_OBJECT   )pMyObject->hCcspCwmpSession;

    CcspTr069PaTraceDebug(("CcspCwmpAcscoHttpSessionClosed...\n"));

    if( pWmpSession != NULL)
    {
        pWmpSession->NotifySessionClosed(pWmpSession);
    }

    return ANSC_STATUS_SUCCESS;
}
