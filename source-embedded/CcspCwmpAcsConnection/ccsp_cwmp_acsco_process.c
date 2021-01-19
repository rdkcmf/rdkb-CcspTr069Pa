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

    module: ccsp_cwmp_acsco_process.c

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This module implements the advanced operation functions
        of the CCSP CWMP ACS Connection Object.

            *   CcspCwmpAcscoConnect
            *   CcspCwmpAcscoRequest
            *   CcspCwmpAcscoRequestOnly
            *   CcspCwmpAcscoClose
    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin  Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/21/05    initial revision.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_acsco_global.h"

#ifdef   _DEBUG
#define  CCSP_CWMP_TRACE_MAX_SOAP_MSG_LENGTH        1024
#endif

#ifdef _ANSC_USE_OPENSSL_
#include <openssl/ssl.h>
#include "linux/user_openssl.h"
extern char* openssl_client_ca_certificate_files;
extern int openssl_load_ca_certificates(int who_calls);
#endif /* _ANSC_USE_OPENSSL_ */

#include "ssp_ccsp_cwmp_cfg.h"

#ifdef SAFEC_DUMMY_API
/*This is required to support CiscoXB3 platform which cannot include safeclib due to the image size constraints */

//adding strcmp_s definition
errno_t strcmp_s(const char * d,int max ,const char * src,int *r)
{
	UNREFERENCED_PARAMETER(max);
	if(d)
	{
		if(src)
		{
			*r= strcmp(d,src);
			return EOK;
		}
		else
		{
			CcspTr069PaTraceError(("source string is null\n"));
		}
	}
	else
	{
		CcspTr069PaTraceError(("destination string is null\n"));
	}
	return -1;
}

//adding strcasecmp_s definition
errno_t strcasecmp_s(const char * d,int max ,const char * src,int *r)
{
	UNREFERENCED_PARAMETER(max);
	if(d)
	{
		if(src)
		{
			*r= strcasecmp(d,src);
			return EOK;
		}
		else
		{
			CcspTr069PaTraceError(("source string is null\n"));
		}
	}
	else
	{
		CcspTr069PaTraceError(("destination string is null\n"));
	}
	return -1;
}

#endif

/*
 *  RDKB-12305  Adding method to check whether comcast device or not
 *  Procedure     : bIsComcastImage
 *  Purpose       : return True for Comcast build.
 *  Parameters    :
 *  Return Values :
 *  1             : 1 for comcast images
 *  2             : 0 for other images
 */
#define DEVICE_PROPERTIES    "/etc/device.properties" 
static int bIsComcastImage( void)
{
	char PartnerId[255] = {'\0'};
	int isComcastImg = 1;
        errno_t rc       = -1;
        int     ind      = -1;
	
	getPartnerId ( PartnerId ) ;
        rc = strcmp_s("comcast", strlen("comcast"), PartnerId, &ind);
        ERR_CHK(rc);
        if((rc == EOK) && (ind != 0))
        {
		isComcastImg = 0;
	}

        return isComcastImg;
}

/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoConnect
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to connect to ACS.

    argument:

                ANSC_HANDLE                 hThisObject
                The caller object.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpAcscoConnect
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject      = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PHTTP_SIMPLE_CLIENT_OBJECT      pHttpClient     = (PHTTP_SIMPLE_CLIENT_OBJECT)pMyObject->hHttpSimpleClient;
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT       pWmpSession     = (PCCSP_CWMP_SESSION_OBJECT   )pMyObject->hCcspCwmpSession;
    errno_t rc = -1;

    if ( pWmpSession )
    {
        PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pWmpSession->hCcspCwmpCpeController;
		
		if ( pCcspCwmpCpeController )
		{
    	    PCCSP_CWMP_CFG_INTERFACE    pCcspCwmpCfgIf  = (PCCSP_CWMP_CFG_INTERFACE)pCcspCwmpCpeController->hCcspCwmpCfgIf;
	        PHTTP_SIMPLE_CLIENT_OBJECT	pHttpClient     = (PHTTP_SIMPLE_CLIENT_OBJECT)pMyObject->hHttpSimpleClient;
        	ULONG                       ulSessionIdleTimeout = 0;

    	    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetHttpSessionIdleTimeout )
	        {
        	    ulSessionIdleTimeout = pCcspCwmpCfgIf->GetHttpSessionIdleTimeout(pCcspCwmpCfgIf->hOwnerContext);
    	    }

	        pHttpClient->SetSessionIdleTimeout((ANSC_HANDLE)pHttpClient, ulSessionIdleTimeout);

            // fprintf(stderr, "<RT> %s: OutboundIfName = '%s'\n", __FUNCTION__, pCcspCwmpCpeController->OutboundIfName ? pCcspCwmpCpeController->OutboundIfName : "NULL");
            if ( pCcspCwmpCpeController->OutboundIfName && pCcspCwmpCpeController->OutboundIfName[0] 
                    &&  AnscSizeOfString(pCcspCwmpCpeController->OutboundIfName) < HTTP_MAX_DEVICE_NAME_SIZE)
            {
               rc = strcpy_s(pHttpClient->Property.BindToDevice.DeviceName, sizeof(pHttpClient->Property.BindToDevice.DeviceName) ,pCcspCwmpCpeController->OutboundIfName);
               if(rc != EOK)
               {
                  ERR_CHK(rc);
                  return  ANSC_STATUS_FAILURE;
               }
            }
            else
            {
                pHttpClient->Property.BindToDevice.DeviceName[0] = '\0';
            }
		}
    }

    if( pMyObject->AcsUrl == NULL || AnscSizeOfString(pMyObject->AcsUrl) <= 10)
    {
        return ANSC_STATUS_NOT_READY;
    }

    returnStatus = pHttpClient->Engage  ((ANSC_HANDLE)pHttpClient);

    if( returnStatus == ANSC_STATUS_SUCCESS)
    {
        pMyObject->bActive          = TRUE;
    }

    return returnStatus;
}

/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoRequest
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pSoapMessage,
                char*                       pMethodName,
                ULONG                       ulReqEnvCount,
                ULONG                       ulRepEnvCount
            );

    description:

        This function is called to send the request to ACS.

    argument:

                ANSC_HANDLE                 hThisObject
                The caller object.

                char*                       pSoapMessage
                The SOAP Request messages

                char*                       pMethodName,
                The first SOAP method name;

                ULONG                       ulReqEnvCount,
                The request envelope count;

                ULONG                       ulRepEnvCount
                The response envelope count;

    return:     the status of the operation;

**********************************************************************/
ANSC_ACS_INTERN_HTTP_CONTENT    intHttpContent  = { 0 };

ANSC_STATUS
CcspCwmpAcscoRequest
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pSoapMessage,
        char*                       pMethodName,
        ULONG                       ulReqEnvCount,
        ULONG                       ulRepEnvCount
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject      = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PHTTP_SIMPLE_CLIENT_OBJECT      pHttpClient     = (PHTTP_SIMPLE_CLIENT_OBJECT)pMyObject->hHttpSimpleClient;
    PCCSP_CWMP_SESSION_OBJECT       pWmpSession     = (PCCSP_CWMP_SESSION_OBJECT   )pMyObject->hCcspCwmpSession;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pWmpSession->hCcspCwmpCpeController;
    PCCSP_CWMP_STAT_INTERFACE       pCcspCwmpStatIf = (PCCSP_CWMP_STAT_INTERFACE)pCcspCwmpCpeController->hCcspCwmpStaIf;
	PCCSP_CWMP_CFG_INTERFACE		pCcspCwmpCfgIf	= (PCCSP_CWMP_CFG_INTERFACE)pCcspCwmpCpeController->hCcspCwmpCfgIf;
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf  = (PCCSP_CWMP_MCO_INTERFACE        )pWmpSession->hCcspCwmpMcoIf;
    PHTTP_HFP_INTERFACE             pHttpHfpIf      = (PHTTP_HFP_INTERFACE)pHttpClient->GetHfpIf((ANSC_HANDLE)pHttpClient);
    PHTTP_CAS_INTERFACE             pHttpCasIf      = NULL;
    PHTTP_REQUEST_URI               pHttpReqInfo    = NULL;
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    PANSC_ACS_INTERN_HTTP_CONTENT   pHttpGetReq     = &intHttpContent;
    BOOL                            bApplyTls       = FALSE;
    PCHAR                           pRequestURL     = NULL;
    PCHAR                           pTempString     = NULL;
    PHTTP_AUTH_CLIENT_OBJECT        pAuthClientObj  = NULL;
    char                            pNewUrl[257]    = { 0 };
    ULONG                           uRedirect       = 0;
    ULONG                           uMaxRedirect    = 5;
	ULONG							ulRpcCallTimeout= CCSP_CWMPSO_RPCCALL_TIMEOUT; 

    /* If the response is 401 authentication required, we need to try again */
    int                             nMaxAuthRetries = 2;

    if( pMyObject->AcsUrl == NULL || AnscSizeOfString(pMyObject->AcsUrl) <= 10 || pHttpHfpIf == NULL)
    {
        return ANSC_STATUS_NOT_READY;
    }

    AnscZeroMemory(pHttpGetReq, sizeof(ANSC_ACS_INTERN_HTTP_CONTENT));

    CcspTr069PaTraceDebug(("CcspCwmpAcscoRequest -- AcsUrl = '%s'\n", pMyObject->AcsUrl));

    pHttpCasIf  = (PHTTP_CAS_INTERFACE)pHttpClient->GetCasIf((ANSC_HANDLE)pHttpClient);

    if ( pHttpCasIf != NULL)
    {
        if( pMyObject->Username == NULL || AnscSizeOfString(pMyObject->Username) == 0)
        {
            pHttpCasIf->EnableAuth(pHttpCasIf->hOwnerContext, FALSE);
        }
        else
        {
            pHttpCasIf->EnableAuth(pHttpCasIf->hOwnerContext, TRUE);

            pAuthClientObj  = (PHTTP_AUTH_CLIENT_OBJECT)pHttpClient->GetClientAuthObj((ANSC_HANDLE)pHttpClient);

            if ( pAuthClientObj != NULL)
            {
                pAuthClientObj->SetAcmIf((ANSC_HANDLE)pAuthClientObj, (ANSC_HANDLE)pMyObject->hHttpAcmIf);
            }
            else
            {
                CcspTr069PaTraceError(("Failed to Get HttpAuthClient object.\n"));
            }
        }
    }

#ifdef   _DEBUG
    if ( !pSoapMessage )
    {
        CcspTr069PaTraceDebug(("CPE Request:\n<EMPTY>\n"));
    }
    else if ( AnscSizeOfString(pSoapMessage) <= CCSP_CWMP_TRACE_MAX_SOAP_MSG_LENGTH )
    {
        CcspTr069PaTraceDebug(("CPE Request:\n%s\n", pSoapMessage));
    }
    else
    {
        char                        partSoap[CCSP_CWMP_TRACE_MAX_SOAP_MSG_LENGTH+1+8];

        AnscCopyMemory(partSoap, pSoapMessage, CCSP_CWMP_TRACE_MAX_SOAP_MSG_LENGTH);
        partSoap[CCSP_CWMP_TRACE_MAX_SOAP_MSG_LENGTH] = '\n';
        partSoap[CCSP_CWMP_TRACE_MAX_SOAP_MSG_LENGTH+1] = '.';
        partSoap[CCSP_CWMP_TRACE_MAX_SOAP_MSG_LENGTH+2] = '.';
        partSoap[CCSP_CWMP_TRACE_MAX_SOAP_MSG_LENGTH+3] = '.';
        partSoap[CCSP_CWMP_TRACE_MAX_SOAP_MSG_LENGTH+4] = '\n';
        partSoap[CCSP_CWMP_TRACE_MAX_SOAP_MSG_LENGTH+5] = 0;
        CcspTr069PaTraceDebug(("CPE Request:\n%s\n", partSoap));
    }
#endif
    
START:
    pRequestURL = pMyObject->AcsUrl;

    pHttpReqInfo =
        (PHTTP_REQUEST_URI)pHttpHfpIf->ParseHttpUrl
            (
                pHttpHfpIf->hOwnerContext,
                pRequestURL,
                AnscSizeOfString(pRequestURL)
            );

    if ( !pHttpReqInfo )
    {
        return ANSC_STATUS_INTERNAL_ERROR;
    }

    pHttpReqInfo->Type = HTTP_URI_TYPE_ABS_PATH;

    /* init the request */
    AnscZeroMemory(pHttpGetReq, sizeof(ANSC_ACS_INTERN_HTTP_CONTENT));
    pHttpGetReq->bIsRedirect    = FALSE;
    pHttpGetReq->SoapMessage    = pSoapMessage;

    /* When there is more than one envelope in a single HTTP Request,
     * when there is a SOAP response in an HTTP Request, or when there is a
     * SOAP Fault response in an HTTP Request, the SOAPAction header in the
     * HTTP Request MUST have no value (with no quotes), indicating that this
     * header provides no information as to the intent of the message."
     */
    if( ulReqEnvCount == 1 && ulRepEnvCount == 0)
    {
        pHttpGetReq->MethodName     = pMethodName;
    }

    AnscInitializeEvent(&pHttpGetReq->CompleteEvent);

    while ( nMaxAuthRetries > 0 )
    {
        CcspTr069PaTraceInfo(("ACS Request now at: %u\n", (unsigned int)AnscGetTickInSeconds()));

        if ( AnscEqualString2(pRequestURL, "https", 5, FALSE) )
        {
            bApplyTls = TRUE;
        }
        else if ( AnscEqualString2(pRequestURL, "http", 4, FALSE) )
        {
            if ( bIsComcastImage() ){                
#ifdef _SUPPORT_HTTP
               CcspTr069PaTraceInfo(("HTTP request from ACS is supported\n"));
               bApplyTls = FALSE;
#else
               CcspTr069PaTraceInfo(("TR-069 blocked unsecured traffic from ACS\n"));
               pHttpGetReq->CompleteStatus = ANSC_STATUS_NOT_SUPPORTED;
               pHttpGetReq->bUnauthorized = TRUE;
               pHttpGetReq->bIsRedirect = FALSE;
               break;
#endif
            }
            else {
               bApplyTls = FALSE; 
            }
        }
        else
        {
             pHttpGetReq->CompleteStatus = ANSC_STATUS_NOT_SUPPORTED;
             pHttpGetReq->bUnauthorized = FALSE;
             pHttpGetReq->bIsRedirect = FALSE;
             break;
        }

        if(pHttpGetReq->pContent != NULL)
        {
            CcspTr069PaFreeMemory(pHttpGetReq->pContent);

            pHttpGetReq->pContent = NULL;
        }

        pHttpGetReq->CompleteStatus = ANSC_STATUS_FAILURE;
        pHttpGetReq->bUnauthorized  = FALSE;
        AnscResetEvent     (&pHttpGetReq->CompleteEvent);

        returnStatus =
            pHttpClient->Request
                (
                    (ANSC_HANDLE)pHttpClient,
                    (ULONG      )HTTP_METHOD_CODE_POST,
                    (ANSC_HANDLE)pHttpReqInfo,
                    (ANSC_HANDLE)pHttpGetReq,
                    bApplyTls
                );

        if( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CcspTr069PaTraceError(("ACS Request failed: returnStatus = %.X\n", (unsigned int)returnStatus));
			break;
        }

		if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCwmpRpcTimeout )
		{
			ulRpcCallTimeout = pCcspCwmpCfgIf->GetCwmpRpcTimeout(pCcspCwmpCfgIf->hOwnerContext);	
			if ( ulRpcCallTimeout < CCSP_CWMPSO_RPCCALL_TIMEOUT )
			{
				ulRpcCallTimeout = CCSP_CWMPSO_RPCCALL_TIMEOUT;
			}
		}

        AnscWaitEvent(&pHttpGetReq->CompleteEvent, ulRpcCallTimeout * 1000);
        if ( pHttpGetReq->CompleteStatus == ANSC_STATUS_SUCCESS && pHttpGetReq->bUnauthorized && nMaxAuthRetries > 0 )
        {
            CcspTr069PaTraceError(("ACS Request is not authenticated, try again.\n"));
            nMaxAuthRetries --;
			
#ifdef _ANSC_USE_OPENSSL_
            if( bApplyTls )
        	{
	        	if ( ANSC_STATUS_SUCCESS == CcspTr069PaSsp_GetTr069CertificateLocationForSyndication( &openssl_client_ca_certificate_files ) )
        		{
					openssl_load_ca_certificates( SSL_CLIENT_CALLS );
        		}
        	}
#endif /* _ANSC_USE_OPENSSL_ */
        }
        else
        {
            CcspTr069PaTraceInfo(("ACS Request has completed with status code %lu, at %lu\n", pHttpGetReq->CompleteStatus, AnscGetTickInSeconds()));

            break;
        }
    }

    /* AnscResetEvent (&pHttpGetReq->CompleteEvent); */
    AnscFreeEvent(&pHttpGetReq->CompleteEvent);

    CcspTr069PaFreeMemory(pHttpReqInfo);

    if ( pHttpGetReq->CompleteStatus != ANSC_STATUS_SUCCESS )
    {
        if ( pHttpGetReq->CompleteStatus == ANSC_STATUS_RESET_SESSION )
        {
            goto  REDIRECTED;
        }
        else
        {
            returnStatus = pHttpGetReq->CompleteStatus;

            goto  EXIT;
        }
    }
    else if( pHttpGetReq->bUnauthorized)
    {
        returnStatus = ANSC_STATUS_FAILURE;

        if( pCcspCwmpStatIf)
        {
            pCcspCwmpStatIf->IncTcpFailure(pCcspCwmpStatIf->hOwnerContext);
        }

        goto EXIT;
    }

REDIRECTED:
    if( pHttpGetReq->bIsRedirect)
    {
        if( _ansc_strstr((PCHAR)pHttpGetReq->pContent, "http") == pHttpGetReq->pContent)
        {
            if ( pMyObject->AcsUrl ) CcspTr069PaFreeMemory(pMyObject->AcsUrl);
            pMyObject->AcsUrl = CcspTr069PaCloneString(pHttpGetReq->pContent);
        }
        else
        {
            /* if it's partial path */
            pTempString = _ansc_strstr(pRequestURL, "//");

            if( pTempString == NULL)
            {
                returnStatus = ANSC_STATUS_FAILURE;

                goto EXIT;
            }

            pTempString += AnscSizeOfString("//");
            pTempString  = _ansc_strstr(pTempString, "/");

            if( pTempString == NULL)
            {
                returnStatus = ANSC_STATUS_FAILURE;

                goto EXIT;
            }

            AnscCopyMemory(pNewUrl, pRequestURL, (ULONG)(pTempString - pRequestURL));
            AnscCatString(pNewUrl, (PCHAR)pHttpGetReq->pContent);

            if ( pMyObject->AcsUrl ) CcspTr069PaFreeMemory(pMyObject->AcsUrl);
            pMyObject->AcsUrl = CcspTr069PaCloneString(pNewUrl);
        }

        uRedirect ++;

        if( uRedirect >= uMaxRedirect)
        {
            CcspTr069PaTraceDebug(("Maximum Redirection reached. Give up!\n"));

            returnStatus = ANSC_STATUS_FAILURE;

            goto EXIT;
        }
        else
        {
            CcspTr069PaTraceDebug(("Acs connection redirection #%u: '%s'\n", (unsigned int)uRedirect, pMyObject->AcsUrl));

            /* in case redirected ACS challenges CPE again */
            nMaxAuthRetries = 2;

            /* tear down current HTTP session before redirecting to new ACS,
             * otherwise, there might be case that ACS sends out redirection
             * response and immediately closes the socket, CWMP may be 
             * confused by closing CWMP session prematurely. 
             */
            pHttpClient->DelAllWcsos((ANSC_HANDLE)pHttpClient);

            goto START;
        }
    }

    if(pWmpSession != NULL)
    {
        if( pHttpGetReq->ulContentSize > 0 && pHttpGetReq->pContent != NULL)
        {
            CcspTr069PaTraceDebug(("Response:\n%s\n", (char*)pHttpGetReq->pContent));

            returnStatus =
                pWmpSession->RecvSoapMessage
                    (
                       pWmpSession,
                       (PCHAR)pHttpGetReq->pContent
                    );
        }
        else
        {
            CcspTr069PaTraceDebug(("Response: <EMPTY>\n"));

            returnStatus =
                pCcspCwmpMcoIf->NotifyAcsStatus
                    (
                        pCcspCwmpMcoIf->hOwnerContext,
                        TRUE,           /* no more requests */
                        FALSE
                    );
        }
    }

EXIT:

    if(pHttpGetReq->pContent != NULL)
    {
        CcspTr069PaFreeMemory(pHttpGetReq->pContent);

        pHttpGetReq->pContent = NULL;
    }

    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

    return returnStatus;
}


/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoRequestOny
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to send the last empty request to ACS.

    argument:

                ANSC_HANDLE                 hThisObject
                The caller object.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpAcscoRequestOnly
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject      = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;

    return  pMyObject->Request((ANSC_HANDLE)pMyObject, NULL, NULL, 0, 0);

#if 0
    PHTTP_SIMPLE_CLIENT_OBJECT      pHttpClient     = (PHTTP_SIMPLE_CLIENT_OBJECT)pMyObject->hHttpSimpleClient;
    PCCSP_CWMP_SESSION_OBJECT       pWmpSession     = (PCCSP_CWMP_SESSION_OBJECT   )pMyObject->hCcspCwmpSession;
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf  = (PCCSP_CWMP_MCO_INTERFACE    )pWmpSession->hCcspCwmpMcoIf;
    PHTTP_HFP_INTERFACE             pHttpHfpIf      = (PHTTP_HFP_INTERFACE)pHttpClient->GetHfpIf((ANSC_HANDLE)pHttpClient);
    PHTTP_CAS_INTERFACE             pHttpCasIf      = NULL;
    PHTTP_REQUEST_URI               pHttpReqInfo    = NULL;
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    ANSC_ACS_INTERN_HTTP_CONTENT    intHttpContent  = { 0 };
    PANSC_ACS_INTERN_HTTP_CONTENT   pHttpGetReq     = &intHttpContent;
    BOOL                            bApplyTls       = FALSE;
    PCHAR                           pRequestURL     = NULL;
    PCHAR                           pTempString     = NULL;
    PHTTP_AUTH_CLIENT_OBJECT        pAuthClientObj  = NULL;
    char                            pNewUrl[257]    = { 0 };
    ULONG                           uRedirect       = 0;
    ULONG                           uMaxRedirect    = 5;

    if( pMyObject->AcsUrl == NULL || AnscSizeOfString(pMyObject->AcsUrl) <= 10 || pHttpHfpIf == NULL)
    {
        return ANSC_STATUS_NOT_READY;
    }

    CcspTr069PaTraceDebug(("CcspCwmpAcscoRequest -- AcsUrl = '%s'\n", pMyObject->AcsUrl));

    pRequestURL = pMyObject->AcsUrl;

    if ( AnscEqualString2(pRequestURL, "https", 5, FALSE) )
    {
        bApplyTls = TRUE;
    }
    else if ( AnscEqualString2(pRequestURL, "http", 4, FALSE) )
    {
        bApplyTls = FALSE;
    }
    else
    {
        return ANSC_STATUS_NOT_SUPPORTED;
    }

    pHttpCasIf  = (PHTTP_CAS_INTERFACE)pHttpClient->GetCasIf((ANSC_HANDLE)pHttpClient);

    if ( pHttpCasIf != NULL)
    {
        if( pMyObject->Username == NULL || AnscSizeOfString(pMyObject->Username) == 0)
        {
            pHttpCasIf->EnableAuth(pHttpCasIf->hOwnerContext, FALSE);
        }
        else
        {
            pHttpCasIf->EnableAuth(pHttpCasIf->hOwnerContext, TRUE);

            pAuthClientObj  = (PHTTP_AUTH_CLIENT_OBJECT)pHttpClient->GetClientAuthObj((ANSC_HANDLE)pHttpClient);

            if ( pAuthClientObj != NULL)
            {
                pAuthClientObj->SetAcmIf((ANSC_HANDLE)pAuthClientObj, (ANSC_HANDLE)pMyObject->hHttpAcmIf);
            }
            else
            {
                CcspTr069PaTraceError(("Failed to Get HttpAuthClient object.\n"));
            }
        }
    }

    pHttpReqInfo =
        (PHTTP_REQUEST_URI)pHttpHfpIf->ParseHttpUrl
            (
                pHttpHfpIf->hOwnerContext,
                pRequestURL,
                AnscSizeOfString(pRequestURL)
            );

    if ( !pHttpReqInfo )
    {
        return ANSC_STATUS_INTERNAL_ERROR;
    }

    pHttpReqInfo->Type = HTTP_URI_TYPE_ABS_PATH;


    CcspTr069PaTraceInfo(("Send empty request to now at: %u\n", (unsigned int)AnscGetTickInSeconds()));

    returnStatus =
        pHttpClient->Request
            (
                (ANSC_HANDLE)pHttpClient,
                (ULONG      )HTTP_METHOD_CODE_POST,
                (ANSC_HANDLE)pHttpReqInfo,
                (ANSC_HANDLE)NULL,
                bApplyTls
            );

    AnscSleep(500);

    CcspTr069PaFreeMemory(pHttpReqInfo);

    return returnStatus;
#endif
}

/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoClose
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to close the connection to ACS

    argument:

                ANSC_HANDLE                 hThisObject
                The caller object.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpAcscoClose
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PHTTP_SIMPLE_CLIENT_OBJECT      pHttpClient     = (PHTTP_SIMPLE_CLIENT_OBJECT)pMyObject->hHttpSimpleClient;
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;

    returnStatus = pHttpClient->Cancel((ANSC_HANDLE)pHttpClient);

	/*
    pMyObject->RemoveCookies((ANSC_HANDLE)pMyObject);
	*/

    pMyObject->bActive = FALSE;

    return returnStatus;
}

