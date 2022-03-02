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

    module:	ccsp_cwmp_tcpcrho_process.c

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This module implements the advanced state-access functions
        of CCSP CWMP TCP Connection Request Handler Object.

        *   CcspCwmpTcpcrhoProcessRequest
        *   CcspCwmpTcpcrhoVerifyCredential

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/20/08    initial revision.

**********************************************************************/


#include "ccsp_cwmp_tcpcrho_global.h"

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpTcpcrhoProcessRequest
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hSocket,
                PVOID                       buffer,
                ULONG                       ulSize
            );

    description:

        This function is called to process connection request message.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hSocket
                The socket from which the message was received.

                PVOID                       buffer
                Specifies the data buffer to be processed.

                ULONG                       ulSize
                Specifies the size of data buffer to be processed.

    return:     object state.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoProcessRequest
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PVOID                       buffer,
        ULONG                       ulSize
    )
{
    ANSC_STATUS                     status              = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject           = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty           = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;
    PWEB_ACM_INTERFACE              pAcmIf              = (PWEB_ACM_INTERFACE            )pMyObject->hWebAcmIf;
    PCCSP_CWMP_MWS_INTERFACE             pCcspCwmpMwsIf          = (PCCSP_CWMP_MWS_INTERFACE           )NULL;
    PANSC_DAEMON_SOCKET_TCP_OBJECT  pWebSocket          = (PANSC_DAEMON_SOCKET_TCP_OBJECT)hSocket;
    PWEB_AUTH_SERVER_PROPERTY       pAuthProperty       = &pProperty->AuthProperty;
    PCCSP_CWMP_TCPCR_HANDLER_SESSINFO    pSessAuthInfo       = &pMyObject->AuthSessionInfo;
    PUCHAR                          pResponseMsg        = NULL;
    ULONG                           ulResponseMsgLen    = 2048;
    BOOL                            bValidConnReq;
    PUCHAR                          pUrlPath            = NULL;

/*  Turn this off, since we don't need this in CCSP.
    pCcspCwmpMwsIf          
        = (PCCSP_CWMP_MWS_INTERFACE)pCcspCwmpCpeController->GetInterfaceByName(pCcspCwmpCpeController,CCSP_CWMP_MWS_INTERFACE_NAME);
*/

    bValidConnReq = 
        CcspCwmpTcpcrhoIsValidConnRequest
            (
                (ANSC_HANDLE)pMyObject, 
                buffer,
                ulSize,
                &pUrlPath
            );

    if ( bValidConnReq )
    {
        if ( !pAcmIf || pAuthProperty->AuthType == WEB_ROO_AUTH_TYPE_NONE || pAuthProperty->AuthType == WEB_ROO_AUTH_TYPE_RESERVED )
        {
            status = ANSC_STATUS_SUCCESS;
        }
        else
        {
            if ( !CcspCwmpTcpcrhoMatchSession((ANSC_HANDLE)pMyObject, hSocket, (PUCHAR)buffer, ulSize) )
            {
                status = ANSC_STATUS_DO_IT_AGAIN;
            }
            else
            {
                status = 
                    pMyObject->VerifyCredential
                        (
                            (ANSC_HANDLE)pMyObject,
                            buffer,
                            ulSize,
                            (ANSC_HANDLE)&pMyObject->AuthSessionInfo
                        );
            }
        }

        if ( status == ANSC_STATUS_SUCCESS )
        {
            PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
            PCCSP_CWMP_ACS_BROKER_OBJECT     pCcspCwmpAcsBroker      = (PCCSP_CWMP_ACS_BROKER_OBJECT    )pCcspCwmpCpeController->hCcspCwmpAcsBroker;
            PCCSP_CWMP_MSO_INTERFACE         pCcspCwmpMsoIf          = (PCCSP_CWMP_MSO_INTERFACE        )pCcspCwmpAcsBroker->hCcspCwmpMsoIf;

            pResponseMsg = (PUCHAR)AnscAllocateMemory(ulResponseMsgLen);
            if ( !pResponseMsg )
            {
                return  ANSC_STATUS_RESOURCES;
            }

            status =
                CcspCwmpTcpcrhoGenResponse
                    (
                        (ANSC_HANDLE)pMyObject,
                        pResponseMsg,
                        &ulResponseMsgLen,
                        (ANSC_HANDLE)NULL
                    );

            if ( status == ANSC_STATUS_SUCCESS )
            {
                /* send the response back to client */
                status =
                    pWebSocket->Send
                        (
                            (ANSC_HANDLE)pWebSocket,
                            (PVOID)pResponseMsg,
                            AnscSizeOfString((char*)pResponseMsg),
                            (ANSC_HANDLE)NULL
                        );

                AnscFreeMemory(pResponseMsg);

                /* send connection request */

                if ( pCcspCwmpCpeController->bBootInformScheduled || pCcspCwmpCpeController->bBootstrapInformScheduled )
                {
                    status = 
                        pCcspCwmpMsoIf->Inform
                            (
                                pCcspCwmpMsoIf->hOwnerContext,
                                CCSP_CWMP_INFORM_EVENT_NAME_ConnectionRequest,
                                NULL,
                                FALSE
                            );

                    /* wake up bootstrap/boot inform task(s) */
                    pCcspCwmpCpeController->InformNow((ANSC_HANDLE)pCcspCwmpCpeController);
                }
                else
                {
                    /* cancel session backoff retry */
//                  pCcspCwmpCpeController->InformNow((ANSC_HANDLE)pCcspCwmpCpeController); 
                    /* RDKB-28090 - InformNow() prior to Inform() causing race condition and timeout error */

                    status = 
                        pCcspCwmpMsoIf->Inform
                            (
                                pCcspCwmpMsoIf->hOwnerContext,
                                CCSP_CWMP_INFORM_EVENT_NAME_ConnectionRequest,
                                NULL,
                                pCcspCwmpCpeController->bBootInformSent
                            );

                    pCcspCwmpCpeController->InformNow((ANSC_HANDLE)pCcspCwmpCpeController);
                }
            }
        }
        else
        {
            PCCSP_CWMP_TCPCR_HANDLER_SESSINFO    pSessInfo        = &pMyObject->AuthSessionInfo;
            BOOL                            bNonceExpired    = FALSE;

            pMyObject->MagicID ++;
            if ( pMyObject->MagicID == 0 )
            {
                pMyObject->MagicID++;
            }

            if ( status == ANSC_STATUS_DO_IT_AGAIN || pSessInfo->Nonce[0] == 0 )
            {
                if ( pSessInfo->Nonce[0] != 0 )
                {
                    bNonceExpired             = TRUE;
                }

                /* generate a new Nonce */
                status = 
                    CcspCwmpTcpcrhoGenNonce
                        (
                            (ANSC_HANDLE)pMyObject,
                            pSessInfo->Nonce,
                            ANSC_MD5_OUTPUT_SIZE * 2 + 1
                        );

                pSessInfo->NonceTimestamp = AnscGetTickInSeconds();
                pSessInfo->NC             = 0;
            }

            pResponseMsg = (PUCHAR)AnscAllocateMemory(ulResponseMsgLen);
            if ( !pResponseMsg )
            {
                return  ANSC_STATUS_RESOURCES;
            }

            if ( pAuthProperty->AuthType == HTTP_AUTH_TYPE_BASIC )
            {
                status = 
                    CcspCwmpTcpcrhoGenBasicResponse
                        (
                            (ANSC_HANDLE)pMyObject,
                            pResponseMsg,
                            &ulResponseMsgLen,
                            pAuthProperty->Realm
                        );
            }
            else
            {
                status = 
                    CcspCwmpTcpcrhoGenDigestResponse
                        (
                            (ANSC_HANDLE)pMyObject,
                            pResponseMsg,
                            &ulResponseMsgLen,
                            pAuthProperty->Realm,
                            ( pAuthProperty->Domain[0] == 0 ) ? NULL : pAuthProperty->Domain,
                            (PUCHAR)(( pAuthProperty->Algorithm == WEB_AUTH_ALGORITHM_MD5_SESS ) ? HTTP_AUTH_NAME_md5_sess : HTTP_AUTH_NAME_md5),
                            (PUCHAR)(pAuthProperty->bNoQop ? NULL : HTTP_AUTH_NAME_auth),
                            pSessInfo->Nonce,
                            bNonceExpired
                        );
            }

            if ( TRUE )
            {
                PANSC_BUFFER_DESCRIPTOR pBufferDesp    = NULL;

                /* prepare recv buffer to receive following response */
                pBufferDesp =
                    (PANSC_BUFFER_DESCRIPTOR)AnscAllocateBdo
                        (
                            HTTP_SSO_MAX_HEADERS_SIZE,
                            0,
                            0
                        );

                if ( pBufferDesp )
                {
                    pWebSocket->SetBufferContext
                        (
                            (ANSC_HANDLE)pWebSocket,
                            AnscBdoGetEndOfBlock(pBufferDesp),
                            AnscBdoGetLeftSize  (pBufferDesp),
                            (ANSC_HANDLE)pBufferDesp
                        );
                }
            }

            /* send the response back to client */
            status =
                pWebSocket->Send
                    (
                        (ANSC_HANDLE)pWebSocket,
                        (PVOID)pResponseMsg,
                        AnscSizeOfString((char*)pResponseMsg),
                        (ANSC_HANDLE)NULL
                    );

            AnscFreeMemory(pResponseMsg);

            if ( status == ANSC_STATUS_SUCCESS )
            {
                status = ANSC_STATUS_BAD_AUTH_DATA;
            }
            else
            {
                status = ANSC_STATUS_FAILURE;
            }
        }
#ifdef _ANSC_IPV6_COMPATIBLE_
        errno_t rc = -1;
        rc = strcpy_s(pSessAuthInfo->RemoteAddress,sizeof(pSessAuthInfo->RemoteAddress), pWebSocket->PeerAddr);
        if(rc!=EOK)
        {
            ERR_CHK(rc);
            return  ANSC_STATUS_FAILURE;

        }
#else
        pSessAuthInfo->RemoteAddr = pWebSocket->PeerAddress.Value;
#endif
    }
    else if ( pCcspCwmpMwsIf && pUrlPath )
    {
        char                        mediaType[256];
        ULONG                       ulMediaTypeLen      = 255;
        ULONG                       ulResLen;
        char*                       pResBuf;
        char*                       pResponseBuffer     = NULL;
        ULONG                       ulResponseBufferLen = 0;

        pResBuf = 
            pCcspCwmpMwsIf->GetResource
                (
                    pCcspCwmpMwsIf->hOwnerContext,
                    (char*)pUrlPath,
                    mediaType,
                    &ulMediaTypeLen,
                    &ulResLen
                );

        if ( !pResBuf )
        {
            status = ANSC_STATUS_FAILURE;
        }
        else
        {
            ulResponseBufferLen = 2048 + ulResLen;

            pResponseBuffer = (char*)AnscAllocateMemory(ulResponseBufferLen);

            if ( !pResponseBuffer )
            {
                status = ANSC_STATUS_RESOURCES;
            }
            else
            {
                status = 
                    CcspCwmpTcpcrhoGenResResponse
                        (
                            (ANSC_HANDLE)pMyObject,
                            pResponseBuffer,
                            &ulResponseBufferLen,
                            mediaType,
                            pResBuf,
                            ulResLen
                        );

                if ( status == ANSC_STATUS_SUCCESS )
                {
                    /* send the response back to client */
                    status =
                        pWebSocket->Send
                            (
                                (ANSC_HANDLE)pWebSocket,
                                (PVOID)pResponseBuffer,
                                ulResponseBufferLen,
                                (ANSC_HANDLE)NULL
                            );
                }
            }

            if ( pResponseBuffer )
            {
                AnscFreeMemory(pResponseBuffer);
            }
        }

        if ( pResBuf )
        {
            AnscFreeMemory(pResBuf);
        }
    }
    else
    {
        status = ANSC_STATUS_FAILURE;
    }

    if ( pUrlPath )
    {
        AnscFreeMemory(pUrlPath);
    }

    return  status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoVerifyCredential
            (
                ANSC_HANDLE                 hThisObject,
                PVOID                       buffer,
                ULONG                       ulSize,
                ANSC_HANDLE                 hSessAuthInfo
            )

    description:

        This function is called to verify credential of client.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PVOID                       buffer
                Specifies the data buffer to be processed.

                ULONG                       ulSize
                Specifies the size of data buffer to be processed.

                ANSC_HANDLE                 hSessAuthInfo
                Session's authentication info against which the user
                is going to be authenticated.

    return:     object state.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoVerifyCredential
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        ULONG                       ulSize,
        ANSC_HANDLE                 hSessAuthInfo
    )
{
    ANSC_STATUS                     status        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PWEB_AUTH_SESSION_INFO          pSessInfo     = (PWEB_AUTH_SESSION_INFO        )hSessAuthInfo;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty     = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;
    PWEB_ACM_INTERFACE              pAcmIf        = (PWEB_ACM_INTERFACE            )pMyObject->hWebAcmIf;
    PWEB_AUTH_SERVER_PROPERTY       pAuthProperty = &pProperty->AuthProperty;
    PUCHAR                          pHostName     = NULL;
    PUCHAR                          pUriPath      = NULL;
    USHORT                          HostPort      = 0;
    PUCHAR                          pPassword     = NULL;
    PHTTP_AUTHO_INFO                pAuthInfo     = NULL;

    if ( !pAcmIf || pAuthProperty->AuthType == WEB_ROO_AUTH_TYPE_NONE || pAuthProperty->AuthType == WEB_ROO_AUTH_TYPE_RESERVED )
    {
        /* we assume no authentication is needed in case ACM interface is not specified */
        status = ANSC_STATUS_SUCCESS;
        goto EXIT;
    }

    pAuthInfo   = 
        (PHTTP_AUTHO_INFO)CcspCwmpTcpcrhoGetAuthInfo
            (
                (ANSC_HANDLE)pMyObject, 
                buffer, 
                ulSize
            );

    if ( !pAuthInfo )
    {
        status = ANSC_STATUS_DO_IT_AGAIN;
        goto EXIT;
    }

    status = 
        CcspCwmpTcpcrhoGetRequestHostUri
            (
                (ANSC_HANDLE)pMyObject, 
                buffer,
                ulSize, 
                &pHostName, 
                &HostPort, 
                &pUriPath
            );

    if ( status != ANSC_STATUS_SUCCESS )
    {
        status = ANSC_STATUS_BAD_AUTH_DATA;
        goto EXIT;
    }

    if ( pAuthInfo->AuthType == HTTP_AUTH_TYPE_BASIC )
    {
        status = 
            pAcmIf->GetCredential
                (
                    pAcmIf->hOwnerContext,
                    pHostName,
                    HostPort,
                    pUriPath,
                    pAuthInfo->pUserName,
                    pAuthInfo->pRealm,
                    &pPassword
                );
    }
    else
    {
        status = 
            pAcmIf->GetCredential
                (
                    pAcmIf->hOwnerContext,
                    pHostName,
                    HostPort,
                    pAuthInfo->pDigest->pDigestUri,    /* use digest uri instead of requst uri since proxy may change it */
                    pAuthInfo->pUserName,
                    pAuthInfo->pRealm,
                    &pPassword
                );
    }

    if ( status == ANSC_STATUS_PASS_THROUGH )
    {
        /* in this case, we assume authentication will be handled by application, could it be BSP page e.g. */
        status = ANSC_STATUS_SUCCESS;
        goto EXIT;
    }

    if ( status != ANSC_STATUS_SUCCESS )
    {
        /* user does not exist */
        status = ANSC_STATUS_BAD_AUTH_DATA;
        goto EXIT;
    }

    status =
        CcspCwmpTcpcrhoVerify
            (
                (ANSC_HANDLE)pMyObject,
                buffer,
                ulSize,
                (ANSC_HANDLE)pAuthInfo,
                pPassword,
                pAuthProperty->AuthType,
                pAuthProperty->Realm,
                pSessInfo->Nonce,
                pAuthProperty->NonceTimeout,
                pSessInfo->NonceTimestamp
            );
 
    if ( pPassword )
    {
        AnscFreeMemory(pPassword);
    }

EXIT:

    if ( pAuthInfo )
    {
        HttpAuthInfoRemove(pAuthInfo);
    }

    if ( pHostName )
    {
        AnscFreeMemory(pHostName);
    }

    if ( pUriPath )
    {
        AnscFreeMemory(pUriPath);
    }

    return  status;
}

