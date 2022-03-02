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

    module:	ccsp_cwmp_tcpcrho_msg.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements HTTP authentication for CCSP CWMP 
        Connection Request Handler Object.

        *   CcspCwmpTcpcrhoParseCredentials
        *   CcspCwmpTcpcrhoParseBasicCredentials
        *   CcspCwmpTcpcrhoParseDigestCredentials
        *   CcspCwmpTcpcrhoIsRequestComplete
        *   CcspCwmpTcpcrhoIsValidConnRequest
        *   CcspCwmpTcpcrhoMatchSession

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

    caller:     component objects

    prototype:

        BOOL
        CcspCwmpTcpcrhoParseCredentials
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCredentials,
                PUCHAR                      pBuf,
                ULONG                       ulSize
            )

    description:

        This function is called to parse credentials.

    argument:   ANSC_HANDLE                 hThisObject
                Handle to HTTP Simple Message Parser object.

                ANSC_HANDLE                 hCredentials
                Handle to credentials object.

                PUCHAR                      pBuf
                Buffer of credentials string.

                ULONG                       ulSize
                The length of buffer.
    
    return:     status of operation.

**********************************************************************/

BOOL
CcspCwmpTcpcrhoParseCredentials
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCredentials,
        PUCHAR                      pBuf,
        ULONG                       ulSize
    )
{
    /* credentials = basic-credentials | auth-scheme #auth-param */
    /* basic-credentials = "Basic" SP basic-cookie */
    /* auth-scheme = token */
    /* auth-param = token "=" quoted-string */

    PHTTP_AUTH_CREDENTIAL           pCredentials    = (PHTTP_AUTH_CREDENTIAL)hCredentials;
    PUCHAR                          pToken          = pBuf;
    PUCHAR                          pLast           = pBuf + ulSize - 1;
    PUCHAR                          pNext;
    ULONG                           ulTokenSize;
    BOOL                            bSucc           = TRUE;

    /* scheme */
    pNext   = _ansc_memchr(pToken, ' ', pLast - pToken + 1);

    if (!pNext)
    {
        return FALSE;
    }

    ulTokenSize = pNext - pToken;

    pNext ++;

    if (AnscSizeOfString(HTTP_AUTH_TEXT_NONE) == ulTokenSize &&
        AnscEqualString2((char*)pToken, HTTP_AUTH_TEXT_NONE, ulTokenSize, FALSE))
    {
        pCredentials->AuthType  = HTTP_AUTH_TYPE_NONE;
    }
    else
    if (AnscSizeOfString(HTTP_AUTH_TEXT_BASIC) == ulTokenSize &&
        AnscEqualString2((char*)pToken, HTTP_AUTH_TEXT_BASIC, ulTokenSize, FALSE))
    {
        pCredentials->AuthType  = HTTP_AUTH_TYPE_BASIC;

        CcspCwmpTcpcrhoParseBasicCredentials(hThisObject, hCredentials, pNext, pLast - pNext + 1);
    }
    else
    if (AnscSizeOfString(HTTP_AUTH_TEXT_DIGEST) == ulTokenSize &&
        AnscEqualString2((char*)pToken, HTTP_AUTH_TEXT_DIGEST, ulTokenSize, FALSE))
    {
        pCredentials->AuthType  = HTTP_AUTH_TYPE_DIGEST;

        CcspCwmpTcpcrhoParseDigestCredentials(hThisObject, hCredentials, pNext, pLast - pNext + 1);
    }
    else
    {
        pCredentials->AuthType  = HTTP_AUTH_TYPE_RESERVED;
    }

    return bSucc;
}


/**********************************************************************

    caller:     component objects

    prototype:

        BOOL
        CcspCwmpTcpcrhoParseBasicCredentials
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCredentials,
                PUCHAR                      pBuf,
                ULONG                       ulSize
            )

    description:

        This function is called to parse basic credentials.

    argument:   ANSC_HANDLE                 hThisObject
                Handle to HTTP Simple Message Parser object.

                ANSC_HANDLE                 hCredentials
                Handle to credentials object.

                PUCHAR                      pBuf
                Buffer of credentials string.

                ULONG                       ulSize
                The length of buffer.
    
    return:     status of operation.

**********************************************************************/

BOOL
CcspCwmpTcpcrhoParseBasicCredentials
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCredentials,
        PUCHAR                      pBuf,
        ULONG                       ulSize
    )
{
    /* basic-credentials = "Basic" SP basic-cookie */
    UNREFERENCED_PARAMETER(hThisObject);
    UNREFERENCED_PARAMETER(pBuf);
    PHTTP_AUTH_CREDENTIAL           pCredentials    = (PHTTP_AUTH_CREDENTIAL)hCredentials;
    PHTTP_CREDENTIAL_BASIC          pBasicCookie    = &pCredentials->Credential.Basic;
    PUCHAR                          pDecodedString;
    PUCHAR                          pString         = NULL;
    ULONG                           ulDecodedStringLen;

    pString = (PUCHAR)AnscAllocateMemory(ulSize + 1);
    
    if (!pString)
    {
        return FALSE;
    }

    AnscCopyMemory(pString, pBuf, ulSize);
    pString[ulSize] = 0;

    pDecodedString  = AnscBase64Decode((PUCHAR)pString, &ulDecodedStringLen);

    if (pDecodedString)
    {
        PUCHAR                      pPos;
        ULONG                       ulCopySize;

        /* parse the decoded string to get user & password */
        pPos = _ansc_memchr(pDecodedString, ':', ulDecodedStringLen);

        if (!pPos)
        {
            ulCopySize  = ulDecodedStringLen;
            if (ulCopySize >= HTTP_MAX_USERNAME_SIZE)
                ulCopySize  = HTTP_MAX_USERNAME_SIZE - 1;

            AnscCopyMemory(pBasicCookie->UserName, pDecodedString, ulCopySize);
            pBasicCookie->UserName[ulCopySize]  = 0;
        }
        else
        {
            PUCHAR                  pUser, pPass;
            ULONG                   ulUser, ulPass;

            pPass   = pPos + 1;
            *pPos   = 0;
            pUser   = pDecodedString;

            ulUser  = AnscSizeOfString((char*)pUser);
            ulPass  = ulDecodedStringLen - ulUser - 1;

            ulCopySize  = ulUser;
            if (ulCopySize >= HTTP_MAX_USERNAME_SIZE)
                ulCopySize  = HTTP_MAX_USERNAME_SIZE - 1;

            AnscCopyMemory(pBasicCookie->UserName, pUser, ulCopySize);
            pBasicCookie->UserName[ulCopySize]  = 0;

            ulCopySize  = ulPass;
            if (ulCopySize >= HTTP_MAX_PASSWORD_SIZE)
                ulCopySize  = HTTP_MAX_PASSWORD_SIZE - 1;

            AnscCopyMemory(pBasicCookie->Password, pPass, ulCopySize);
            pBasicCookie->Password[ulCopySize]  = 0;
        }

        AnscFreeMemory(pDecodedString);
    }

    AnscFreeMemory(pString);

    return TRUE;
}


/**********************************************************************

    caller:     component objects

    prototype:

        BOOL
        CcspCwmpTcpcrhoParseDigestCredentials
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCredentials,
                PUCHAR                      pBuf,
                ULONG                       ulSize
            )

    description:

        This function is called to parse digest credentials.

    argument:   ANSC_HANDLE                 hThisObject
                Handle to HTTP Simple Message Parser object.

                ANSC_HANDLE                 hCredentials
                Handle to credentials object.

                PUCHAR                      pBuf
                Buffer of credentials string.

                ULONG                       ulSize
                The length of buffer.
    
    return:     status of operation.

**********************************************************************/

BOOL
CcspCwmpTcpcrhoParseDigestCredentials
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCredentials,
        PUCHAR                      pBuf,
        ULONG                       ulSize
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    /* auth-scheme = token */
    /* auth-param = token "=" quoted-string */

    PHTTP_AUTH_CREDENTIAL           pCredentials    = (PHTTP_AUTH_CREDENTIAL)hCredentials;
    PHTTP_CREDENTIAL_DIGEST         pDigest         = &pCredentials->Credential.Digest;

    if ( ulSize >= HTTP_MAX_USERNAME_SIZE )
    {
        ulSize = HTTP_MAX_USERNAME_SIZE - 1;
    }

    AnscCopyMemory(pDigest->UserName, pBuf, ulSize);
    pDigest->UserName[ulSize]   = 0;

    return TRUE;
}

/**********************************************************************

    caller:     component objects

    prototype:

        BOOL
        CcspCwmpTcpcrhoIsRequestComplete
            (
                ANSC_HANDLE                 hThisObject,
                PUCHAR                      pBuf,
                ULONG                       ulSize
            )

    description:

        This function is called to determine if the received
        message is a valid connection requst.

    argument:   ANSC_HANDLE                 hThisObject
                Handle to HTTP Simple Message Parser object.

                PUCHAR                      pBuf
                Buffer of credentials string.

                ULONG                       ulSize
                The length of buffer.

                PUCHAR*                     ppUrlPath
                On return, it contains absolute URI path if
                the URL is not a connection request.
  
    return:     status of operation.

**********************************************************************/

BOOL
CcspCwmpTcpcrhoIsRequestComplete
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pBuf,
        ULONG                       ulSize
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    char                            eom[][5]            = { "\n\r\n", "\n\n", "\r\r" };
    int                             num_eom             = sizeof(eom)/sizeof(eom[0]);
    int                             i;
    char*                           pEndHeaders;

    for ( i = 0; i < num_eom; i ++ )
    {
        pEndHeaders =   
            CcspMemorySearch
                (
                    pBuf,
                    ulSize,
                    (PUCHAR)eom[i],
                    AnscSizeOfString(eom[i]),
                    TRUE
                );

        if ( pEndHeaders )
        {
            return  TRUE;
        }
    }

    return  FALSE;
}


/**********************************************************************

    caller:     component objects

    prototype:

        BOOL
        CcspCwmpTcpcrhoIsValidConnRequest
            (
                ANSC_HANDLE                 hThisObject,
                PUCHAR                      pBuf,
                ULONG                       ulSize,
                PUCHAR*                     pUrlPath
            );

    description:

        This function is called to determine if the received
        message is a valid connection requst.

    argument:   ANSC_HANDLE                 hThisObject
                Handle to HTTP Simple Message Parser object.

                PUCHAR                      pBuf
                Buffer of credentials string.

                ULONG                       ulSize
                The length of buffer.

                PUCHAR*                     ppUrlPath
                On return, it contains absolute URI path if
                the URL is not a connection request.
  
    return:     status of operation.

**********************************************************************/

BOOL
CcspCwmpTcpcrhoIsValidConnRequest
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pBuf,
        ULONG                       ulSize,
        PUCHAR*                     ppUrlPath
    )
{
    /* the message has to be a valid HTTP request */
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject           = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PUCHAR                          pMsg                = pBuf;
    PUCHAR                          pMsgEnd             = pMsg + ulSize - 1;
    BOOL                            bValid              = TRUE;
    PUCHAR                          pUrlPath           = NULL;
    PUCHAR                          pUrlPathOrg        = NULL;
    PUCHAR                          pPath;
    PUCHAR                          pPathEnd, pLWS, pProto;
    PUCHAR                          pReqLineEnd        = NULL;
    ANSC_STATUS                     status;

    *ppUrlPath  = NULL;

    pMsg = CcspCwmpTcpcrhoSkipWS(pMsg, ulSize);

    if ( pMsgEnd - pMsg + 1 < 16 )
    {
        return FALSE;
    }

    if ( !AnscEqualString2((char*)pMsg, "GET ", 4, FALSE) )
    {
        return  FALSE;
    }

    pPath = pMsg + 3;
    pPath = CcspCwmpTcpcrhoSkipWS(pPath, pMsgEnd - pPath + 1);

    if ( !pPath )
    {
        return  FALSE;
    }

    pReqLineEnd = _ansc_memchr(pPath, '\r', pMsgEnd - pPath + 1);
    pPathEnd    = _ansc_memchr(pPath, '\n', pMsgEnd - pPath + 1);
    if ( pPathEnd < pReqLineEnd )
    {
        pReqLineEnd = pPathEnd;
    }

    pLWS = _ansc_memchr(pPath, ' ',  pMsgEnd - pPath + 1);
    if ( pLWS && pLWS < pPathEnd )
    {
        pPathEnd = pLWS;
    }
    pLWS = _ansc_memchr(pPath, '\t', pMsgEnd - pPath + 1);
    if ( pLWS && pLWS < pPathEnd )
    {
        pPathEnd = pLWS;
    }
    if ( pPathEnd > pReqLineEnd )
    {
        return  FALSE;      /* HTTP/1.1 is missing */
    }

    /* check path segment */
    status = 
        pCcspCwmpCpeController->GetParamStringValue
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                DM_ROOTNAME"ManagementServer.ConnectionRequestURL",
                (char**)&pUrlPathOrg
            );

    /*RDKB-7328, CID-32947, perform null check before use*/
    if (( status != ANSC_STATUS_SUCCESS ) || (!pUrlPathOrg))
    {
        return FALSE;       /* somehow CR Url is not ready yet */
    }

    pUrlPath = pUrlPathOrg;

    pProto      = (PUCHAR)_ansc_strstr((char*)pUrlPath, "http://");

    if ( pProto )
    {
        pUrlPath = _ansc_memchr((char*)pUrlPath+7, '/', AnscSizeOfString((char*)pUrlPath)-7);
    }

    if ( (pProto = (PUCHAR)_ansc_strstr((char*)pPath, "http://")) >= pReqLineEnd )
    {
        bValid = FALSE;
        goto EXIT;
    }

    if ( !pUrlPath && (pPathEnd - pPath) == 1 && *pPath == '/' )
    {
        /* no path specified, it's valid then */
        bValid = TRUE;
        goto EXIT;
    }
    else if ( !pUrlPath )
    {
        bValid = FALSE;
        goto EXIT;
    }

    if ( pProto )
    {
        pPath = _ansc_memchr(pPath+7, '/', pReqLineEnd - pUrlPath - 6);
    }

    if ( !pPath || AnscSizeOfString((char*)pUrlPath) != (ULONG)(pPathEnd - pPath) || 
         !AnscEqualMemory((char*)pPath, (char*)pUrlPath, AnscSizeOfString((char*)pUrlPath)) )
    {
        bValid = FALSE;
    }


EXIT:

    if ( !bValid )
    {
        int                         nPathLen = pPathEnd - pPath;

        *ppUrlPath = AnscAllocateMemory(nPathLen + 1);

        if ( *ppUrlPath )
        {
            AnscCopyMemory(*ppUrlPath, pPath, nPathLen);
            (*ppUrlPath)[nPathLen] = 0;
        }
    }

    if ( pUrlPathOrg )
    {
        AnscFreeMemory(pUrlPathOrg);
    }

    return  bValid;
}


/**********************************************************************

    caller:     component objects

    prototype:

        BOOL
        CcspCwmpTcpcrhoMatchSession
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hSocket,
                PUCHAR                      pBuf,
                ULONG                       ulSize
            );

    description:

        This function is called to determine if the received
        message is a valid connection requst.

    argument:   ANSC_HANDLE                 hThisObject
                Handle to HTTP Simple Message Parser object.

                ANSC_HANDLE                 hSocket
                The socket from which request was received.

                PUCHAR                      pBuf
                Buffer of credentials string.

                ULONG                       ulSize
                The length of buffer.

    return:     status of operation.

**********************************************************************/

BOOL
CcspCwmpTcpcrhoMatchSession
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PUCHAR                      pBuf,
        ULONG                       ulSize
    )
{
    /* the message has to be a valid HTTP request */
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PANSC_DAEMON_SOCKET_TCP_OBJECT  pWebSocket   = (PANSC_DAEMON_SOCKET_TCP_OBJECT)hSocket;
    PUCHAR                          pCookie, pSID, pVEnd;
    ULONG                           ulVOffset;
    ULONG                           ulVLen;
    ULONG                           ulSID = 0;
#ifdef _ANSC_IPV6_COMPATIBLE_
    errno_t                          rc = -1;
    int                              ind = -1;
    rc = strcasecmp_s(pMyObject->AuthSessionInfo.RemoteAddress,sizeof(pMyObject->AuthSessionInfo.RemoteAddress),pWebSocket->PeerAddr,&ind);
    ERR_CHK(rc);
    if ( pMyObject->AuthSessionInfo.RemoteAddress != NULL &&
        (!(!ind) && (rc == EOK)))
#else

    if ( pMyObject->AuthSessionInfo.RemoteAddr != 0 &&
         pWebSocket->PeerAddress.Value != pMyObject->AuthSessionInfo.RemoteAddr )
#endif
    {
        /* this address is different from the peer which established current session */
        return  FALSE;
    }

    /* check if the message contains Cookie with current magic number specified */
    pCookie = 
        CcspCwmpTcpcrhoFindHeader
            (
                pBuf,
                ulSize,
                (PUCHAR)"Cookie",
                &ulVOffset,
                &ulVLen
            );

    if ( !pCookie )
    {
        /* we can not force client to send cookie as we sent, 
         * however if client sends cookie, we have to verify SID.
         */
        return  TRUE;
    }

    pCookie += ulVOffset;
    pVEnd    = pCookie + ulVLen;

    pSID    = (PUCHAR) 
        CcspMemorySearch
            (
                pCookie,
                ulVLen,
                (PUCHAR)CCSP_CWMP_TCPCR_COOKIE_MAGIC_NUMBER,
                AnscSizeOfString(CCSP_CWMP_TCPCR_COOKIE_MAGIC_NUMBER),
                FALSE
            );

    if ( !pSID )
    {
        return  FALSE;
    }

    pSID += AnscSizeOfString(CCSP_CWMP_TCPCR_COOKIE_MAGIC_NUMBER) + 1;
    if ( pSID <= pVEnd && *pSID == '"' )
    {
        pSID ++;
    }

    if ( pSID > pVEnd )
    {
        return  FALSE;
    }

    ulSID = (ULONG)_ansc_atol((char*)pSID);

    return  ( ulSID == pMyObject->MagicID );
}



