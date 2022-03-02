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

    module:	ccsp_cwmp_tcpcrho_auth.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements HTTP authentication for CCSP CWMP
        Connection Request Handler Object.

        *   CcspCwmpTcpcrhoCalcDigResponse
        *   CcspCwmpTcpcrhoCalcDigestHA1
        *   CcspCwmpTcpcrhoCalcDigestHA2
        *   CcspCwmpTcpcrhoGetRequestHostUri
        *   CcspCwmpTcpcrhoGetDigestAuthInfo

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/20/08    initial revision.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/

/* #define  _TIME_FUNC_SUPPORTED */

#include "ccsp_cwmp_tcpcrho_global.h"


static const char                   s_httpAuthMsgBody[] =
"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
"<html><head>"
"<title>401 Unauthorized</title>"
"</head><body>"
"<h1>Authorization Required</h1>"
"<p>"
"This server could not verify that you "
"are authorized to access the document requested.  "
"Either you supplied the wrong credentials (e.g., bad password), or your "
"browser doesn't understand how to supply the credentials required"
"</p>"
"<hr>"
"</body>"
"</html>"
;




/**********************************************************************

    caller:     owner of this object

    prototype:

        PUCHAR
        CcspCwmpTcpcrhoSkipWS
            (
                PUCHAR                  pMsg,
                ULONG                   ulSize
            )

    description:

        This function is called to skip leading white spaces.

    argument:   PUCHAR                      pMsg
                The message to be processed.

                ULONG                       ulSize
                Message size.

    return:     the first non white space character in the given
                buffer.

**********************************************************************/

PUCHAR
CcspCwmpTcpcrhoSkipWS
    (
        PUCHAR                  pMsg,
        ULONG                   ulSize
    )
{
    ULONG                       i;

    for ( i = 0; i < ulSize; i ++ )
    {
        if ( *(pMsg+i) != ' ' && *(pMsg+i) != '\t' )
        {
            break;
        }
    }

    return  (i < ulSize) ? (pMsg+i) : NULL;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        PUCHAR
        CcspCwmpTcpcrhoSkipWS
            (
                PUCHAR                  pMsg,
                ULONG                   ulSize
            )

    description:

        This function is called to skip trailing white spaces.

    argument:   PUCHAR                      pMsg
                The message to be processed.

                ULONG                       ulSize
                Message size.

    return:     the first non white space character in the given
                buffer from the end of message.

**********************************************************************/

PUCHAR
CcspCwmpTcpcrhoSkipTrailingWS
    (
        PUCHAR                  pMsg,
        ULONG                   ulSize
    )
{
    ULONG                       i;

    for ( i = 0; i < ulSize; i ++ )
    {
        if ( *(pMsg-i) != ' ' && *(pMsg-i) != '\t' )
        {
            break;
        }
    }

    return  (i < ulSize) ? (pMsg-i) : NULL;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        void
        CcspCwmpTcpcrhoBinToHex
            (
                PUCHAR                  Bin,
                ULONG                   ulBinLen,
                PUCHAR                  Hex
            )

    description:

        This function is called to convert binary string to
        hexidecimal string.

    argument:   PUCHAR                      Bin
                Binary string.

                ULONG                       ulBinLen
                The size of binary string.

                PUCHAR                      He
                Buffer to the hexidecimal string.

    return:     none.

**********************************************************************/

void
CcspCwmpTcpcrhoBinToHex
    (
        PUCHAR                  Bin,
        ULONG                   ulBinLen,
        PUCHAR                  Hex
    )
{
    ULONG                       i;
    USHORT                      j;

    for ( i = 0; i < ulBinLen; i++ )
    {
        j = (Bin[i] >> 4) & 0x0F;
        if (j <= 9)
            Hex[i*2] = (j + '0');
         else
            Hex[i*2] = (j + 'a' - 10);
        j = Bin[i] & 0x0F;
        if (j <= 9)
            Hex[i*2+1] = (j + '0');
         else
            Hex[i*2+1] = (j + 'a' - 10);
    }

    Hex[ulBinLen * 2] = '\0';
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        PUCHAR
        CcspCwmpTcpcrhoFindHeader
            (
                PUCHAR                  pMsg,
                ULONG                   ulSize,
                PUCHAR                  pHeaderName,
                PULONG                  pulHdrValueOffset,
                PULONG                  pulHdrValueLen
            )

    description:

        This function is called to find the HTTP header
        by the specified name.

    argument:   PUCHAR                      pMsg
                HTTP request message.

                ULONG                       ulSize
                The size of HTTP request message.

                PUCHAR                      pHeaderName
                Header's name.

                PULONG                      pulHdrValueOffset
                The offset to the returned header where the
                header value begins.

                PULONG                      pulHdrValueLen
                Length of header value.

    return:     where the header appears.

**********************************************************************/

PUCHAR
CcspCwmpTcpcrhoFindHeader
    (
        PUCHAR                  pMsg,
        ULONG                   ulSize,
        PUCHAR                  pHeaderName,
        PULONG                  pulHdrValueOffset,
        PULONG                  pulHdrValueLen
    )
{
    PUCHAR                      pHeader     = pMsg;
    PUCHAR                      pMsgEnd     = pMsg + ulSize - 1;
    PUCHAR                      pNext       = pHeader;
    ULONG                       ulHdrLen    = 0;
    PUCHAR                      pHeaderEnd, pHeaderValue;
    ULONG                       ulHeaderLen = AnscSizeOfString((char*)pHeaderName);
    PUCHAR                      pCR, pLF, pCRLF;

    pHeader = CcspCwmpTcpcrhoSkipWS(pHeader, ulSize);
    if ( !pHeader )
    {
        return  NULL;
    }

    /* skip request line */
    pCRLF = (PUCHAR)CcspMemorySearch(pHeader, (ULONG)(pMsgEnd - pHeader + 1), (PUCHAR)"\r\n", 2, TRUE);
    pCR   = (PUCHAR)CcspMemorySearch(pHeader, (ULONG)(pMsgEnd - pHeader + 1), (PUCHAR)"\r",   1, TRUE);
    pLF   = (PUCHAR)CcspMemorySearch(pHeader, (ULONG)(pMsgEnd - pHeader + 1), (PUCHAR)"\n",   1, TRUE);

    pNext = pCRLF;
    if ( !pNext ) pNext = pCR;
    else if ( pCR && pCR < pNext ) pNext = pCR;
    if ( !pNext ) pNext = pLF;
    else if ( pLF && pLF < pNext ) pNext = pLF;

    pHeader = pNext;

    if ( !pHeader )
    {
        return  NULL;
    }

    if ( pHeader == pCRLF ) 
        pHeader += 2; 
    else 
        pHeader ++;

    while ( pHeader )
    {
        pCRLF = (PUCHAR)CcspMemorySearch(pHeader, (ULONG)(pMsgEnd - pHeader + 1), (PUCHAR)"\r\n", 2, TRUE);
        pCR   = (PUCHAR)CcspMemorySearch(pHeader, (ULONG)(pMsgEnd - pHeader + 1), (PUCHAR)"\r",   1, TRUE);
        pLF   = (PUCHAR)CcspMemorySearch(pHeader, (ULONG)(pMsgEnd - pHeader + 1), (PUCHAR)"\n",   1, TRUE);

        pNext = pCRLF;
        if ( !pNext ) pNext = pCR;
        else if ( pCR && pCR < pNext ) pNext = pCR;
        if ( !pNext ) pNext = pLF;
        else if ( pLF && pLF < pNext ) pNext = pLF;

        ulHdrLen     = pNext? pNext - pHeader + 1 : pMsgEnd - pHeader + 1;
        pHeaderValue = _ansc_memchr(pHeader, ':', ulHdrLen);

        if ( !pHeaderValue )
        {
            return  NULL;   /* invalid header */
        }

        pHeaderEnd = CcspCwmpTcpcrhoSkipTrailingWS(pHeaderValue - 1, pHeaderValue - pHeader);

        if ( ulHeaderLen == (ULONG)(pHeaderEnd - pHeader + 1) &&
             AnscEqualString2((char*)pHeader, (char*)pHeaderName, ulHeaderLen, FALSE) )
        {
            pHeaderValue += 1;

            pHeaderValue = CcspCwmpTcpcrhoSkipWS(pHeaderValue, pNext ? pNext - pHeaderValue : pMsgEnd - pHeaderValue + 1);

            *pulHdrValueLen     = pNext ? pNext - pHeaderValue : pMsgEnd - pHeaderValue + 1;
            *pulHdrValueOffset  = pHeaderValue - pHeader;

            return pHeader;
        }

        pHeader = pNext;
        if ( pHeader != NULL )
        {
            if ( pHeader == pCRLF ) pHeader += 2; else pHeader ++;
        }
    }

    return  NULL;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        PUCHAR
        CcspCwmpTcpcrhoCalcDigResponse
            (
                ANSC_HANDLE                 hThisObject,
                PUCHAR                      pUserName,
                PUCHAR                      pPassword,
                PUCHAR                      pRealm,
                PUCHAR                      pAlgorithm,
                PUCHAR                      pNonce,
                PUCHAR                      pCNonce,
                PUCHAR                      pQop,
                PUCHAR                      pNC,
                PUCHAR                      pEntityDigest
            )

    description:

        This function is called to calculate digest response.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PUCHAR                      pMethodName
                HTTP request method name.

                PUCHAR                      pUserName
                User name.

                PUCHAR                      pPassword
                User's password.

                PUCHAR                      pRealm
                Realm name.

                PUCHAR                      pAlgorithm
                The name of algorithm.

                PUCHAR                      pNonce
                The Nonce value.

                PUCHAR                      pCNonce
                CNonce value.

                PUCHAR                      pQop
                QOP value.

                PUCHAR                      pNC
                Nonce-Count value.

                PUCHAR                      pEntityDigest
                Specifies the digest of entity body. It must be
                specified if algorithm is specified as 'md5-sess'.

    return:     digest response.

**********************************************************************/

extern
ULONG
AnscCryptoMd5Digest 
    (
        PVOID                       buffer,
        ULONG                       size,
        PANSC_CRYPTO_HASH           hash
    );

PUCHAR
CcspCwmpTcpcrhoCalcDigResponse
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pMethodName,
        PUCHAR                      pUserName,
        PUCHAR                      pPassword,
        PUCHAR                      pRealm,
        PUCHAR                      pAlgorithm,
        PUCHAR                      pNonce,
        PUCHAR                      pCNonce,
        PUCHAR                      pQop,
        PUCHAR                      pNC,
        PUCHAR                      pEntityDigest,
        PUCHAR                      pDigestUri
    )
{
    ANSC_STATUS                     status       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT)hThisObject;
    char*                           pBuf         = NULL;
    ULONG                           ulSize       = 0;
    PUCHAR                          pDigRep      = NULL;
    ANSC_CRYPTO_HASH                MD5Hash;
    PUCHAR                          pHA1, pHA2;

    pDigRep = (PUCHAR)AnscAllocateMemory(ANSC_MD5_OUTPUT_SIZE * 2 + 1);
    if ( !pDigRep )
    {
        return NULL;
    }

    pHA1    = (PUCHAR)AnscAllocateMemory(ANSC_MD5_OUTPUT_SIZE * 2 + 1);
    pHA2    = (PUCHAR)AnscAllocateMemory(ANSC_MD5_OUTPUT_SIZE * 2 + 1);

    if ( !pHA1 || !pHA2 )
    {
        goto EXIT2;
    }

    status =
        CcspCwmpTcpcrhoCalcDigestHA1
            (
                (ANSC_HANDLE)pMyObject,
                pUserName,
                pPassword,
                pRealm,
                pAlgorithm,
                pNonce,
                pCNonce,
                pHA1
            );

    if ( status != ANSC_STATUS_SUCCESS )
    {
        goto EXIT2;
    }

    CcspTr069PaTraceDebug(("pHA1 = %s\n", pHA1));

    status =
        CcspCwmpTcpcrhoCalcDigestHA2
            (
                (ANSC_HANDLE)pMyObject,
                pMethodName,
                pQop,
                pEntityDigest,
                pDigestUri,
                pHA2
            );

    if ( status != ANSC_STATUS_SUCCESS )
    {
        goto EXIT2;
    }

    CcspTr069PaTraceDebug(("pHA2 = %s\n", pHA2));

    ulSize  = ANSC_MD5_OUTPUT_SIZE * 2 + 1 + AnscSizeOfString((char*)pNonce) + 1;
    if ( pQop )
    {
        ulSize  +=
            8 + 1 +                                             /* Nonce Count */
            AnscSizeOfString((char*)pCNonce) + 1 +
            AnscSizeOfString((char*)pQop)   + 1;
    }
    ulSize  += ANSC_MD5_OUTPUT_SIZE * 2;

    pBuf    = (char *)AnscAllocateMemory(ulSize + 16);

    if ( !pBuf )
    {
        goto EXIT2;
    }
    else
    {
        _ansc_sprintf(pBuf, "%s:%s:", pHA1, pNonce);

        if ( pQop )
        {
            _ansc_sprintf(pBuf + AnscSizeOfString(pBuf), "%s:%s:%s:", pNC,  pCNonce, pQop);
        }

        _ansc_sprintf(pBuf + AnscSizeOfString(pBuf), "%s", pHA2);

        AnscCryptoMd5Digest((PVOID)pBuf, AnscSizeOfString(pBuf), &MD5Hash);

        AnscFreeMemory(pBuf);
    }

    CcspCwmpTcpcrhoBinToHex(MD5Hash.Value, ANSC_MD5_OUTPUT_SIZE, pDigRep);

    CcspTr069PaTraceDebug(("pDigRep = %s\n", pDigRep));

    goto EXIT1;

EXIT2:

    if ( pDigRep )
    {
        AnscFreeMemory(pDigRep);
        pDigRep = NULL;
    }

EXIT1:
    if ( pHA1 )
    {
        AnscFreeMemory(pHA1);
    }

    if ( pHA2)
    {
        AnscFreeMemory(pHA2);
    }

    return pDigRep;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoCalcDigestHA1
            (
                ANSC_HANDLE                 hThisObject,
                PUCHAR                      pUserName,
                PUCHAR                      pPassword,
                PUCHAR                      pRealm,
                PUCHAR                      pAlgorithm,
                PUCHAR                      pNonce,
                PUCHAR                      pCNonce,
                PUCHAR                      pHA1
            )

    description:

        This function is called to calculate HA1.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PUCHAR                      pUserName
                User name.

                PUCHAR                      pPassword
                User's password.

                PUCHAR                      pRealm
                Realm name.

                PUCHAR                      pAlgorithm
                The name of algorithm.

                PUCHAR                      pNonce
                The Nonce value.

                PUCHAR                      pCNonce
                CNonce value.

                PUCHAR                      pHA1
                On successful return, it contains HA1 value.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoCalcDigestHA1
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pUserName,
        PUCHAR                      pPassword,
        PUCHAR                      pRealm,
        PUCHAR                      pAlgorithm,
        PUCHAR                      pNonce,
        PUCHAR                      pCNonce,
        PUCHAR                      pHA1
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    ANSC_STATUS                     status       = ANSC_STATUS_SUCCESS;
    char*                           pBuf         = NULL;
    ULONG                           ulSize       = 0;
    ANSC_CRYPTO_HASH                MD5Hash;
    errno_t rc = -1 ;
    int ind = -1;

    ulSize  =
        AnscSizeOfString((char*)pUserName) + 1 +
        AnscSizeOfString((char*)pRealm)    + 1 +
        (pPassword?AnscSizeOfString((char*)pPassword):0);

     if (pAlgorithm)
     {
        rc = strcasecmp_s(HTTP_AUTH_NAME_md5_sess,strlen(HTTP_AUTH_NAME_md5_sess),(char*)pAlgorithm,&ind);
        ERR_CHK(rc);
        if ((!ind) && (rc == EOK))
        {
            if ( ulSize < ANSC_MD5_OUTPUT_SIZE * 2 )
            {
               ulSize  = ANSC_MD5_OUTPUT_SIZE * 2;
            }

            ulSize    += 1 + AnscSizeOfString((char*)pNonce) + 1;
            if ( pCNonce )
            {
                ulSize  += AnscSizeOfString((char*)pCNonce);
            }
      
        }
      }

    pBuf  = (char *)AnscAllocateMemory(ulSize + 16);
    if ( !pBuf )
    {
        status = ANSC_STATUS_RESOURCES;
    }
    /*
     * The following code which complies with RFC 2617 doesn't work with IE and Mozilla for some reason
     * when algorithm "MD5-sess" is specified. The reason is these browsers give the ASCII format (32 bytes) of
     * hashed value over user:realm:password instead of hex value (16 bytes). My guess is all web server will
     * follow these rules for compatibility reasons. Hopefully all browsers and HTTP clients which support
     * MD5-sess will follow this rule.
     */
    /*
    else
    {
        _ansc_sprintf(pBuf, "%s:%s:%s", pUserName, pRealm, pPassword);

        AnscCryptoMd5Digest((PVOID)pBuf, AnscSizeOfString(pBuf), &MD5Hash);

        if ( pAlgorithm && AnscEqualString(pAlgorithm, HTTP_AUTH_NAME_md5_sess, FALSE) )
        {
            ULONG                   ulMsgSize = ANSC_MD5_OUTPUT_SIZE;

            AnscCopyMemory(pBuf, MD5Hash.Value, ANSC_MD5_OUTPUT_SIZE);

            _ansc_sprintf(pBuf + ANSC_MD5_OUTPUT_SIZE, ":%s:%s", pNonce, pCNonce);

            ulMsgSize += AnscSizeOfString(pBuf + ANSC_MD5_OUTPUT_SIZE);

            AnscCryptoMd5Digest((PVOID)pBuf, ulMsgSize, &MD5Hash);
        }

        AnscFreeMemory(pBuf);

        CcspCwmpTcpcrhoBinToHex(MD5Hash.Value, ANSC_MD5_OUTPUT_SIZE, pHA1);
    }
    */
    else
    {
        _ansc_sprintf(pBuf, "%s:%s:%s", (char*)pUserName, (char*)pRealm, pPassword?(char*)pPassword:"");

        AnscCryptoMd5Digest((PVOID)pBuf, AnscSizeOfString(pBuf), &MD5Hash);
        CcspCwmpTcpcrhoBinToHex(MD5Hash.Value, ANSC_MD5_OUTPUT_SIZE, pHA1);
        if (pAlgorithm)
        {
            rc = strcasecmp_s(HTTP_AUTH_NAME_md5_sess,strlen(HTTP_AUTH_NAME_md5_sess),(char*)pAlgorithm,&ind);
            ERR_CHK(rc);
            if ((!ind) && (rc == EOK))
            {
               _ansc_sprintf(pBuf, "%s:%s:%s", pHA1, pNonce, pCNonce);

               AnscCryptoMd5Digest((PVOID)pBuf, AnscSizeOfString(pBuf), &MD5Hash);
               CcspCwmpTcpcrhoBinToHex(MD5Hash.Value, ANSC_MD5_OUTPUT_SIZE, pHA1);
            }
        
       }
        AnscFreeMemory(pBuf);
    }

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoCalcDigestHA2
            (
                ANSC_HANDLE                 hThisObject,
                PUCHAR                      pMethodName,
                PUCHAR                      pQop,
                PUCHAR                      pEntityDigest,
                PUCHAR                      pDigestUri,
                PUCHAR                      pHA2
            )

    description:

        This function is called to calculate HA2.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PUCHAR                      pMethodName
                HTTP request method name.

                PUCHAR                      pQop
                QOP value.

                PUCHAR                      pEntityDigest
                Specifies the digest of entity body. It must be
                specified if algorithm is specified as 'md5-sess'.

                PUCHAR                      pDigestUri
                Specifies the digest URI.

                PUCHAR                      pHA2
                On successful return, it contains HA2 value.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoCalcDigestHA2
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pMethodName,
        PUCHAR                      pQop,
        PUCHAR                      pEntityDigest,
        PUCHAR                      pDigestUri,
        PUCHAR                      pHA2
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    ANSC_STATUS                     status       = ANSC_STATUS_SUCCESS;
    char*                           pBuf         = NULL;
    ULONG                           ulSize       = 0;
    BOOL                            bAuthInt     = FALSE;
    PUCHAR                          pUriPath     = NULL;
    ANSC_CRYPTO_HASH                MD5Hash      = {0}; /*RDKB-7327, CID-32923, initialize before use */
    errno_t rc       = -1;
    int     ind      = -1;

    pUriPath = pDigestUri;

    ulSize  = AnscSizeOfString((char*)pMethodName) + 1 + AnscSizeOfString((char*)pUriPath);
    if(pQop)
    {
        rc = strcasecmp_s(HTTP_AUTH_NAME_auth_int,strlen(HTTP_AUTH_NAME_auth_int),(char*)pQop,&ind);
        ERR_CHK(rc);
      if ((!ind) && (rc == EOK))
    {
       
        bAuthInt    = TRUE;
        ulSize     += 1 + pEntityDigest?AnscSizeOfString((char*)pEntityDigest):0;
    }
    }

    pBuf    = (char *)AnscAllocateMemory(ulSize + 16);
    if ( !pBuf )
    {
        status = ANSC_STATUS_RESOURCES;
    }
    else
    {
        _ansc_sprintf(pBuf, "%s:%s", pMethodName, pUriPath);

        if ( bAuthInt )
        {
            _ansc_sprintf
                (
                    pBuf + AnscSizeOfString(pBuf),
                    ":%s",
                    pEntityDigest?(char*)pEntityDigest:""
                );
        }

        if ( status == ANSC_STATUS_SUCCESS )
        {
        	AnscCryptoMd5Digest((PVOID)pBuf, AnscSizeOfString(pBuf), &MD5Hash);
        }

        AnscFreeMemory(pBuf);
    }

    CcspCwmpTcpcrhoBinToHex(MD5Hash.Value, ANSC_MD5_OUTPUT_SIZE, pHA2);

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoGetRequestHostUri
            (
                ANSC_HANDLE                 hThisObject,
                PVOID                       buffer,
                ULONG                       ulSize,
                PUCHAR                      *ppHostName,
                PUSHORT                     pHostPort,
                PUCHAR                      *ppUriPath
            )

    description:

        This function is called to get request host, port and
        URI path.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PVOID                       buffer
                HTTP request message buffer.

                ULONG                       ulSize
                The size of HTTP request message.

                PUCHAR                      *ppHostName
                On return, it contains host name.

                PUSHORT                     pHostPort
                On return, it contains host port.

                PUCHAR                      *ppUriPath
                On return, it contains URI path.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoGetRequestHostUri
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        ULONG                       ulSize,
        PUCHAR                      *ppHostName,
        PUSHORT                     pHostPort,
        PUCHAR                      *ppUriPath
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    ANSC_STATUS                     status       = ANSC_STATUS_BAD_PAYLOAD;
    PUCHAR                          pMsg         = (PUCHAR                    )buffer;
    PUCHAR                          pMsgEnd      = pMsg + ulSize - 1;
    char*                           pHost        = NULL;
    PUCHAR                          pReqInfo     = NULL;
    ULONG                           ulHdrVSize   = 0;
    ULONG                           ulHdrVOffset = 0;

    *ppHostName = NULL;
    *ppUriPath  = NULL;
    *pHostPort  = 0;

    pReqInfo = CcspCwmpTcpcrhoSkipWS(pMsg, ulSize);
    if ( !pReqInfo )
    {
        goto EXIT;
    }

    pHost = (char*)CcspCwmpTcpcrhoFindHeader(pMsg, ulSize, (PUCHAR)"Host", &ulHdrVOffset, &ulHdrVSize);
    if ( pHost )
    {
        pHost = pHost + ulHdrVOffset;
    }

    if ( pHost && ulHdrVSize != 0 )
    {
        char*                       pPort       = NULL;
        ULONG                       ulHostLen   = ulHdrVSize;

        pPort = _ansc_memchr((char*)pHost, ':', ulHdrVSize);

        if ( pPort )
        {
            *pHostPort      = (USHORT)_ansc_atol(pPort+1);
            ulHostLen       = pPort - pHost;
        }

        if ( ulHostLen == 0 )
        {
            goto EXIT;
        }

        *ppHostName      = (PUCHAR)AnscAllocateMemory(ulHostLen + 1);
        if ( *ppHostName )
        {
            AnscCopyMemory(*ppHostName, pHost, ulHostLen);
            (*ppHostName)[ulHostLen] = 0;
        }
    }

    if ( TRUE )
    {
        PUCHAR                      pUri        = pReqInfo;
        PUCHAR                      pPath       = NULL;
        PUCHAR                      pUriEnd     = NULL;
        PUCHAR                      pReqInfoEnd = NULL;
        ULONG                       ulLen;

        pReqInfoEnd = (PUCHAR)CcspMemorySearch(pReqInfo, pMsgEnd - pReqInfo + 1, (PUCHAR)"\r\n", 2, TRUE);

        if ( !pReqInfoEnd )
        {
            goto EXIT;
        }

        pUri = _ansc_memchr(pReqInfo, ' ', pReqInfoEnd - pReqInfo + 1);
        if ( !pUri )
        {
            goto EXIT;
        }

        pUri ++;
        pUriEnd = _ansc_memchr(pUri, ' ', pReqInfoEnd - pUri + 1);

        if ( !pUriEnd )
        {
            goto EXIT;
        }

        pUriEnd --;
        ulLen = pUriEnd - pUri + 1;

        if ( ulLen >= 7 && AnscEqualString2((char*)pUri, "http://", 7, FALSE) )
        {
            char*                  	pPort   = NULL;

            /* absolute URI */
            pUri    += 7;
            ulLen   -= 7;

            pPort   = _ansc_memchr((char*)pUri, ':', pUriEnd - pUri + 1);
            pPath   = _ansc_memchr((char*)pUri, '/', pUriEnd - pUri + 1);

            if ( pPort && pPath && (PUCHAR)pPort > pPath )
            {
                pPort = NULL;
            }

            if ( pPort )
            {
                *pHostPort = (USHORT)_ansc_atol(pPort+1);
            }
        }
        else
        {
            /* relative path or absolute path */
            pPath   = pUri;
        }

        if ( pPath )
        {
            ulLen = pUriEnd - pPath + 1;

            *ppUriPath = (PUCHAR)AnscAllocateMemory(ulLen + 1);

            if ( *ppUriPath )
            {
                AnscCopyMemory(*ppUriPath, pPath, ulLen);
                (*ppUriPath)[ulLen] = 0;
            }
        }
    }

    status = ANSC_STATUS_SUCCESS;

EXIT:

    if ( status != ANSC_STATUS_SUCCESS )
    {
        if ( *ppHostName )
        {
            AnscFreeMemory(*ppHostName);
            *ppHostName = NULL;
        }

        if ( *ppUriPath )
        {
            AnscFreeMemory(*ppUriPath);
            *ppUriPath = NULL;
        }
    }

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpTcpcrhoGetAuthInfo
            (
                ANSC_HANDLE                 hThisObject,
                PVOID                       buffer,
                ULONG                       ulSize
            );

    description:

        This function is called to get authentication info from
        the given request object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PVOID                       buffer
                HTTP request message buffer.

                ULONG                       ulSize
                Size of HTTP request message.

    return:     status of operation.

**********************************************************************/

ANSC_HANDLE
CcspCwmpTcpcrhoGetAuthInfo
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        ULONG                       ulSize
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT)hThisObject;
    PHTTP_HFO_AUTHORIZATION         pHfoAuth     = (PHTTP_HFO_AUTHORIZATION   )NULL;
    PHTTP_AUTHO_INFO                pAuthInfo    = (PHTTP_AUTHO_INFO          )NULL;
    PHTTP_AUTH_CREDENTIAL           pCredential  = NULL;
    BOOL                            bDigestAuth  = FALSE;
    PUCHAR                          pHdrAuth     = NULL;
    ULONG                           ulHdrValueOffset, ulHdrValueLen;

    pHdrAuth =
        CcspCwmpTcpcrhoFindHeader
            (
                (PUCHAR)buffer,
                ulSize,
                (PUCHAR)HTTP_HEADER_NAME_AUTHORIZATION,
                &ulHdrValueOffset,
                &ulHdrValueLen
            );

    if ( !pHdrAuth || ulHdrValueLen == 0 )
    {
        return NULL;
    }

    pHfoAuth  = (PHTTP_HFO_AUTHORIZATION)AnscAllocateMemory(sizeof(HTTP_HFO_AUTHORIZATION));

    if ( !pHfoAuth )
    {
        return NULL;
    }

    pCredential = &pHfoAuth->Credential;

    if ( CcspCwmpTcpcrhoParseCredentials((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCredential, pHdrAuth + ulHdrValueOffset, ulHdrValueLen) == FALSE )
    {
        AnscFreeMemory(pHfoAuth);
        return NULL;
    }

    switch ( pCredential->AuthType )
    {
        case    HTTP_AUTH_TYPE_BASIC:

                bDigestAuth = FALSE;

                break;

        case    HTTP_AUTH_TYPE_DIGEST:

                bDigestAuth = TRUE;

                break;

        default:

                AnscFreeMemory(pHfoAuth);

                return NULL;    /* scheme not supported */

                break;
    }

    HttpAuthInfoCreate(pAuthInfo);

    if ( !pAuthInfo )
    {
        AnscFreeMemory(pHfoAuth);
        return NULL;
    }

    pAuthInfo->AuthType = pCredential->AuthType;

    if ( bDigestAuth )
    {
        HttpAuthDigestInfoCreate(pAuthInfo->pDigest);

        if ( !pAuthInfo->pDigest )
        {
            HttpAuthInfoRemove(pAuthInfo);
            AnscFreeMemory(pHfoAuth);

            return NULL;
        }
    }

    if ( !bDigestAuth )
    {
        PHTTP_CREDENTIAL_BASIC      pBasicCred  = &pCredential->Credential.Basic;

        pAuthInfo->pUserName    = (PUCHAR)AnscCloneString(pBasicCred->UserName);
        pAuthInfo->pPassword    = (PUCHAR)AnscCloneString(pBasicCred->Password);
    }
    else
    {
        PHTTP_CREDENTIAL_DIGEST     pDigestCred = &pCredential->Credential.Digest;
        PUCHAR                      pChal       = (PUCHAR)pDigestCred->UserName;

        CcspCwmpTcpcrhoGetDigestAuthInfo
            (
                (ANSC_HANDLE)pMyObject,
                (ANSC_HANDLE)pAuthInfo,
                pChal
            );
    }

    AnscFreeMemory(pHfoAuth);

    return pAuthInfo;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoGetDigestAuthInfo
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hAuthInfo,
                PUCHAR                      pDigData
            )

    description:

        This function is called to extract digest authentication
        info from the given digest data.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hAuthInfo
                On successful return, it contains the digest
                authentication info.

                PUCHAR                      pDigData
                Specifies the digest data to be parsed.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoGetDigestAuthInfo
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hAuthInfo,
        PUCHAR                      pDigData
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    ANSC_STATUS                     status       = ANSC_STATUS_SUCCESS;
    PHTTP_AUTHO_INFO                pAuthInfo    = (PHTTP_AUTHO_INFO          )hAuthInfo;
    char*                           pChal        = (char*)pDigData;
    char*                           pNext = NULL, *pValue= NULL, *pValueLast= NULL;
    ULONG                           ulLen, ulNameLen;

    while ( pChal )
    {
        /*RDKB-7327, CID-33475, null check before use*/
        while ( (pChal) && (*pChal == ' ') )
        {
            pChal ++;
        }
        if ( !pChal )
        {
            break;
        }

        pNext       = _ansc_strchr(pChal, ',');
        ulLen       = pNext ? (ULONG)(pNext - pChal) : AnscSizeOfString(pChal);

        pValue      = _ansc_memchr(pChal, '=', ulLen);
        ulNameLen   = pValue ? (ULONG)(pValue - pChal) : AnscSizeOfString(pChal);
        pValue ++;
        pValueLast  = pNext ? pNext - 1 : pChal + AnscSizeOfString(pChal) - 1;

        if ( pValueLast >= pValue && *pValue == '"' )
        {
            pValue ++;

            if ( *pValueLast == '"' )
            {
                pValueLast --;
            }
        }

        if ( pValueLast < pValue )
        {
            pChal   = pNext ? pNext + 1 : NULL;
            continue;
        }

        if ( ulNameLen == AnscSizeOfString(HTTP_AUTH_NAME_realm) &&
             AnscEqualString2(pChal, HTTP_AUTH_NAME_realm, ulNameLen, FALSE) )
        {
            HttpAuthCloneMemory(pAuthInfo->pRealm, pValue, pValueLast - pValue + 1);
        }
        else if ( ulNameLen == AnscSizeOfString(HTTP_AUTH_NAME_domain) &&
                  AnscEqualString2(pChal, HTTP_AUTH_NAME_domain, ulNameLen, FALSE) )
        {
            HttpAuthCloneMemory(pAuthInfo->pDigest->pDomain, pValue, pValueLast - pValue + 1);
        }
        else if ( ulNameLen == AnscSizeOfString(HTTP_AUTH_NAME_nonce) &&
                  AnscEqualString2(pChal, HTTP_AUTH_NAME_nonce, ulNameLen, FALSE) )
        {
            HttpAuthCloneMemory(pAuthInfo->pDigest->pNonce, pValue, pValueLast - pValue + 1);
        }
        else if ( ulNameLen == AnscSizeOfString(HTTP_AUTH_NAME_opaque) &&
                  AnscEqualString2(pChal, HTTP_AUTH_NAME_opaque, ulNameLen, FALSE) )
        {
            HttpAuthCloneMemory(pAuthInfo->pDigest->pOpaque, pValue, pValueLast - pValue + 1);
        }
        else if ( ulNameLen == AnscSizeOfString(HTTP_AUTH_NAME_algorithm) &&
                  AnscEqualString2(pChal, HTTP_AUTH_NAME_algorithm, ulNameLen, FALSE) )
        {
            HttpAuthCloneMemory(pAuthInfo->pDigest->pAlgorithm, pValue, pValueLast - pValue + 1);
        }
        else if ( ulNameLen == AnscSizeOfString(HTTP_AUTH_NAME_qop) &&
                  AnscEqualString2(pChal, HTTP_AUTH_NAME_qop, ulNameLen, FALSE) )
        {
            HttpAuthCloneMemory(pAuthInfo->pDigest->pQop, pValue, pValueLast - pValue + 1);
        }
        else if ( ulNameLen == AnscSizeOfString(HTTP_AUTH_NAME_response) &&
                  AnscEqualString2(pChal, HTTP_AUTH_NAME_response, ulNameLen, FALSE) )
        {
            HttpAuthCloneMemory(pAuthInfo->pDigest->pResponse, pValue, pValueLast - pValue + 1);
        }
        else if ( ulNameLen == AnscSizeOfString(HTTP_AUTH_NAME_digest_uri) &&
                  AnscEqualString2(pChal, HTTP_AUTH_NAME_digest_uri, ulNameLen, FALSE) )
        {
            HttpAuthCloneMemory(pAuthInfo->pDigest->pDigestUri, pValue, pValueLast - pValue + 1);
        }
        else if ( ulNameLen == AnscSizeOfString(HTTP_AUTH_NAME_nc) &&
                  AnscEqualString2(pChal, HTTP_AUTH_NAME_nc, ulNameLen, FALSE) )
        {
            HttpAuthCloneMemory(pAuthInfo->pDigest->pNonceCount, pValue, pValueLast - pValue + 1);
        }
        else if ( ulNameLen == AnscSizeOfString(HTTP_AUTH_NAME_cnonce) &&
                  AnscEqualString2(pChal, HTTP_AUTH_NAME_cnonce, ulNameLen, FALSE) )
        {
            HttpAuthCloneMemory(pAuthInfo->pDigest->pCNonce, pValue, pValueLast - pValue + 1);
        }
        else if ( ulNameLen == AnscSizeOfString(HTTP_AUTH_NAME_username) &&
                  AnscEqualString2(pChal, HTTP_AUTH_NAME_username, ulNameLen, FALSE) )
        {
            HttpAuthCloneMemory(pAuthInfo->pUserName, pValue, pValueLast - pValue + 1);
        }

        pChal   = pNext ? pNext + 1 : NULL;
    }

    if ( pAuthInfo->pDigest->pDomain )
    {
        PUCHAR                  pDomain = pAuthInfo->pDigest->pDomain;
        PUCHAR                  pNext;
        ULONG                   ulCount = 0;

        pNext   = (PUCHAR)_ansc_strchr((char*)pDomain, ' ');

        if ( pNext )
        {
            while ( pDomain )
            {
                while (*pDomain == ' ')
                {
                    pDomain ++;
                }

                if ( *pDomain == 0 )
                {
                    break;
                }

                pNext   = (PUCHAR)_ansc_strchr((char*)pDomain, ' ');

                ulCount ++;

                pDomain = pNext ? pNext + 1 : NULL;
            }

            if ( ulCount > 1 )
            {
                pAuthInfo->pDigest->pDomainURIs =
                    (PHTTP_AUTHO_DIGEST_DOMAIN_URIS)AnscAllocateMemory(sizeof(HTTP_AUTHO_DIGEST_DOMAIN_URIS));

                if ( pAuthInfo->pDigest->pDomainURIs )
                {
                    PUCHAR      *pURIs  = (PUCHAR *)AnscAllocateMemory(sizeof(PUCHAR) * ulCount);
                    ULONG       ulLen;

                    if ( !pURIs )
                    {
                        AnscFreeMemory(pAuthInfo->pDigest->pDomainURIs);
                        pAuthInfo->pDigest->pDomainURIs = NULL;
                    }
                    else
                    {
                        pAuthInfo->pDigest->pDomainURIs->NumURIs    = ulCount;
                        pAuthInfo->pDigest->pDomainURIs->pURIs      = pURIs;

        	            pDomain = pAuthInfo->pDigest->pDomain;
    	                ulCount = 0;

	                    while ( pDomain )
                    	{
                	        while (*pDomain == ' ')
            	            {
        	                    pDomain ++;
    	                    }

	                        if ( *pDomain == 0 )
                        	{
                    	        break;
                	        }

            	            pNext   = (PUCHAR)_ansc_strchr((char*)pDomain, ' ');
        	                ulLen   = pNext ? (ULONG)(pNext - pDomain) : AnscSizeOfString((char*)pDomain);

    	                    HttpAuthCloneMemory(pURIs[ulCount], pDomain, ulLen);
	                        ulCount ++;

                        	pDomain = pNext ? pNext + 1 : NULL;
                    	}
		    		}
                }
            }
        }
    }

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        PUCHAR
        CcspCwmpTcpcrhoGenBasicChallenge
            (
                ANSC_HANDLE                 hThisObject,
                PUCHAR                      pRealm
            )

    description:

        This function is called to generate basic challenge value.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PUCHAR                      pRealm
                Authentication realm.

    return:     status of operation.

**********************************************************************/

PUCHAR
CcspCwmpTcpcrhoGenBasicChallenge
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pRealm
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    return (PUCHAR)AnscCloneString((char*)pRealm);
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        PUCHAR
        CcspCwmpTcpcrhoGenDigestChallenge
            (
                ANSC_HANDLE                 hThisObject,
                PUCHAR                      pRealm,
                PUCHAR                      pDomain,
                PUCHAR                      pAlgorithm,
                PUCHAR                      pQop,
                PUCHAR                      pNonce,
                BOOL                        bNonceExpired
            )

    description:

        This function is called to generate digest challenge value.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PUCHAR                      pRealm
                Authentication realm.

                PUCHAR                      pDomain
                Authentication domain.

                PUCHAR                      pAlgorithm
                Algorithm name, be it either "MD5" or "MD5-sess" as
                RFC 2167 specifies.

                PUCHAR                      pQop
                Qop value, be it either NULL or "auth". "auth-int"
                is not supported in this version.

                PUCHAR                      pNonce
                Nonce value to be used.

                BOOL                        bNonceExpired
                If true, "stale=true" will be present in the
                challenge.

    return:     status of operation.

**********************************************************************/

PUCHAR
CcspCwmpTcpcrhoGenDigestChallenge
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pRealm,
        PUCHAR                      pDomain,
        PUCHAR                      pAlgorithm,
        PUCHAR                      pQop,
        PUCHAR                      pNonce,
        BOOL                        bNonceExpired
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    char*							pDigChal     = NULL;
    ULONG                           ulSize       = 0;

    ulSize  = AnscSizeOfString(HTTP_AUTH_NAME_realm) + 1 + AnscSizeOfString((char*)pRealm) + 2;
    ulSize += 2;

    ulSize += AnscSizeOfString(HTTP_AUTH_NAME_nonce) + 1 + AnscSizeOfString((char*)pNonce) + 2;
    ulSize += 2;

    ulSize += AnscSizeOfString(HTTP_AUTH_NAME_algorithm) + 1 + AnscSizeOfString((char*)pAlgorithm) + 2;
    ulSize += 2;

    if ( pDomain )
    {
        ulSize += AnscSizeOfString(HTTP_AUTH_NAME_domain) + 1 + AnscSizeOfString((char*)pDomain) + 2;
        ulSize += 2;
    }

    if ( pQop )
    {
        ulSize += AnscSizeOfString(HTTP_AUTH_NAME_qop) + 1 + AnscSizeOfString((char*)pQop) + 2;
        ulSize += 2;
    }

    if ( bNonceExpired )
    {
        ulSize += AnscSizeOfString(HTTP_AUTH_NAME_stale) + 1 + AnscSizeOfString(HTTP_AUTH_NAME_true) + 2;
        ulSize += 2;
    }

    ulSize += 16;

    pDigChal    = (char*)AnscAllocateMemory(ulSize);

    if ( !pDigChal )
    {
        return NULL;
    }

    /* realm */
    _ansc_sprintf
        (
            pDigChal,
            "%s=\"%s\"",
            HTTP_AUTH_NAME_realm,
            pRealm
        );

    /* nonce */
    _ansc_sprintf
        (
            pDigChal + AnscSizeOfString(pDigChal),
            ", %s=\"%s\"",
            HTTP_AUTH_NAME_nonce,
            pNonce
        );

    /* algorithm */
    _ansc_sprintf
        (
            pDigChal + AnscSizeOfString(pDigChal),
            ", %s=\"%s\"",
            HTTP_AUTH_NAME_algorithm,
            pAlgorithm
        );

    /* domain */
    if ( pDomain )
    {
        _ansc_sprintf
            (
                pDigChal + AnscSizeOfString(pDigChal),
                ", %s=\"%s\"",
                HTTP_AUTH_NAME_domain,
                pDomain
            );
    }

    /* qop */
    if ( pQop )
    {
        _ansc_sprintf
            (
                pDigChal + AnscSizeOfString(pDigChal),
                ", %s=\"%s\"",
                HTTP_AUTH_NAME_qop,
                pQop
            );
    }

    /* stale if neccessary */
    if ( bNonceExpired )
    {
        _ansc_sprintf
            (
                pDigChal + AnscSizeOfString(pDigChal),
                ", %s=\"%s\"",
                HTTP_AUTH_NAME_stale,
                HTTP_AUTH_NAME_true
            );
    }

    return (PUCHAR)pDigChal;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoGenBasicResponse
            (
                ANSC_HANDLE                 hThisObject,
                PVOID                       buffer,
                PULONG                      pulSize,
                PUCHAR                      pRealm
            )

    description:

        This function is called to generate basic challenge response.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PVOID                       buffer
                HTTP response message buffer.

                PULONG                      pulSize
                The size of HTTP response message buffer.

                PUCHAR                      pRealm
                Authentication realm.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoGenBasicResponse
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        PULONG                      pulSize,
        PUCHAR                      pRealm
    )
{
    ANSC_STATUS                     status       = ANSC_STATUS_RESOURCES;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT)hThisObject;
    PHTTP_HFO_WWW_AUTHENTICATE      pHfoWwwAuth  = NULL;
    PHTTP_AUTH_CHALLENGE            pAuthChal    = NULL;
    PUCHAR                          pChalReq     = NULL;
    PHTTP_CHALLENGE_BASIC           pBasicAuth   = NULL;
    errno_t  rc = -1;


    pChalReq    = CcspCwmpTcpcrhoGenBasicChallenge((ANSC_HANDLE)pMyObject, pRealm);
    if ( !pChalReq )
    {
        return status;
    }

    /* construct WWW-Authenticate header */
    pHfoWwwAuth     = (PHTTP_HFO_WWW_AUTHENTICATE)AnscAllocateMemory(sizeof(HTTP_HFO_WWW_AUTHENTICATE));

    if ( pHfoWwwAuth )
    {
        pHfoWwwAuth->Flags          = 0;
        pHfoWwwAuth->HeaderId       = HTTP_HEADER_ID_WWW_AUTHENTICATE;

        pHfoWwwAuth->ChallengeCount = 1;
        pAuthChal                   = &pHfoWwwAuth->ChallengeArray[0];

        pAuthChal->AuthType = HTTP_AUTH_TYPE_BASIC;
        pBasicAuth  = &pAuthChal->Challenge.Basic;
        rc = strcpy_s((char*)pBasicAuth->Realm, sizeof(pBasicAuth->Realm), (char*)pChalReq);
        if(rc!=EOK)
        {
           ERR_CHK(rc);
           return  ANSC_STATUS_FAILURE;
        }
        status = CcspCwmpTcpcrhoGenResponse((ANSC_HANDLE)pMyObject, buffer, pulSize, (ANSC_HANDLE)pHfoWwwAuth);

        AnscFreeMemory(pHfoWwwAuth);
    }

    if ( pChalReq )
    {
        AnscFreeMemory(pChalReq);
    }

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        PUCHAR
        CcspCwmpTcpcrhoGenDigestChallenge
            (
                ANSC_HANDLE                 hThisObject,
                PVOID                       buffer,
                PULONG                      pulSize,
                PUCHAR                      pRealm,
                PUCHAR                      pDomain,
                PUCHAR                      pAlgorithm,
                PUCHAR                      pQop,
                PUCHAR                      pNonce,
                BOOL                        bNonceExpired
            )

    description:

        This function is called to generate digest challenge response.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PVOID                       buffer
                HTTP response message buffer.

                PULONG                      pulSize
                Size of HTTP response message.

                PUCHAR                      pRealm
                Authentication realm.

                PUCHAR                      pDomain
                Authentication domain.

                PUCHAR                      pAlgorithm
                Algorithm name, be it either "MD5" or "MD5-sess" as
                RFC 2167 specifies.

                PUCHAR                      pQop
                Qop value, be it either NULL or "auth". "auth-int"
                is not supported in this version.

                PUCHAR                      pNonce
                Nonce value to be used.

                BOOL                        bNonceExpired
                If true, "stale=true" will be present in the
                challenge.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoGenDigestResponse
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        PULONG                      pulSize,
        PUCHAR                      pRealm,
        PUCHAR                      pDomain,
        PUCHAR                      pAlgorithm,
        PUCHAR                      pQop,
        PUCHAR                      pNonce,
        BOOL                        bNonceExpired
    )
{
    ANSC_STATUS                     status       = ANSC_STATUS_RESOURCES;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT)hThisObject;
    PHTTP_HFO_WWW_AUTHENTICATE      pHfoWwwAuth  = NULL;
    PHTTP_AUTH_CHALLENGE            pAuthChal    = NULL;
    PUCHAR                          pChalReq     = NULL;
    PHTTP_CHALLENGE_DIGEST          pDigestAuth  = NULL;
    errno_t                         rc           = -1;

    pChalReq    =
        CcspCwmpTcpcrhoGenDigestChallenge
            (
                (ANSC_HANDLE)pMyObject,
                pRealm,
                pDomain,
                pAlgorithm,
                pQop,
                pNonce,
                bNonceExpired
            );

    if ( !pChalReq )
    {
        return status;
    }

    /* construct WWW-Authenticate header */
    pHfoWwwAuth     = (PHTTP_HFO_WWW_AUTHENTICATE)AnscAllocateMemory(sizeof(HTTP_HFO_WWW_AUTHENTICATE));

    if ( pHfoWwwAuth )
    {
        pHfoWwwAuth->Flags          = 0;
        pHfoWwwAuth->HeaderId       = HTTP_HEADER_ID_WWW_AUTHENTICATE;

        pHfoWwwAuth->ChallengeCount = 1;
        pAuthChal                   = &pHfoWwwAuth->ChallengeArray[0];

        pAuthChal->AuthType = HTTP_AUTH_TYPE_DIGEST;
        pDigestAuth = &pAuthChal->Challenge.Digest;
         rc = strcpy_s((char*)pDigestAuth->Realm, sizeof(pDigestAuth->Realm), (char*)pChalReq);
         if(rc != EOK)
         { 
             ERR_CHK(rc);
              return ANSC_STATUS_FAILURE;
         }

        status = CcspCwmpTcpcrhoGenResponse((ANSC_HANDLE)pMyObject, buffer, pulSize, (ANSC_HANDLE)pHfoWwwAuth);

        AnscFreeMemory(pHfoWwwAuth);
    }

    if ( pChalReq )
    {
        AnscFreeMemory(pChalReq);
    }

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoGenResponse
            (
                ANSC_HANDLE                 hThisObject,
                PVOID                       buffer,
                PULONG                      pulSize,
                ANSC_HANDLE                 hAuthHeader
            )

    description:

        This function is called to generate response.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PVOID                       buffer
                Http response message buffer.

                PULONG                      pulSize
                size of response message.

                ANSC_HANDLE                 hAuthHeader
                Authentication header.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoGenResponse
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        PULONG                      pulSize,
        ANSC_HANDLE                 hAuthHeader
    )
{
    ANSC_STATUS                     status       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT)hThisObject;
    char*                           pMsg         = (char*                          )buffer;
    ULONG                           ulSize       = *pulSize;
    PHTTP_HFO_WWW_AUTHENTICATE      pHfoWwwAuth  = (PHTTP_HFO_WWW_AUTHENTICATE)hAuthHeader;

    *pMsg = 0;

    /* status line */
    if ( pHfoWwwAuth )
    {
        _ansc_snprintf
            (
                pMsg+AnscSizeOfString(pMsg),
                ulSize - AnscSizeOfString(pMsg),
                "HTTP/1.1 401 Unauthorized\r\n"
            );
    }
    else
    {
        _ansc_snprintf
            (
                pMsg+AnscSizeOfString(pMsg),
                ulSize - AnscSizeOfString(pMsg),
                "HTTP/1.1 200 OK\r\n"
            );
    }

    /* construct Content-Type header */
    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "Content-Type: text/html;charset=iso-8859-1\r\n"
        );

    /* construct Connection header */
    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "Connection: Keep-Alive\r\n"
        );

    /* Cookie */
    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "Set-Cookie: %s=\"%u/%lu\"; Version=\"1\"; Path=\"/\"\r\n",
            CCSP_CWMP_TCPCR_COOKIE_MAGIC_NUMBER,
            (unsigned int)pMyObject->MagicID,
            (long unsigned)buffer
        );

    /* WWW-Authenticate */
    if ( pHfoWwwAuth )
    {
        PHTTP_AUTH_CHALLENGE        pAuthChal = &pHfoWwwAuth->ChallengeArray[0];

        if ( pAuthChal->AuthType == HTTP_AUTH_TYPE_BASIC )
        {
            _ansc_snprintf
                (
                    pMsg+AnscSizeOfString(pMsg),
                    ulSize - AnscSizeOfString(pMsg),
                    "WWW-Authenticate: Basic realm=%s\r\n",
                    pAuthChal->Challenge.Basic.Realm
                );
        }
        else
        {
            _ansc_snprintf
                (
                    pMsg+AnscSizeOfString(pMsg),
                    ulSize - AnscSizeOfString(pMsg),
                    "WWW-Authenticate: Digest %s\r\n",
                    pAuthChal->Challenge.Digest.Realm
                );
        }
    }

    /* construct Server header */
    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "Server: Cisco-CcspCwmpTcpCR/1.0\r\n"
        );

    /* Date header */
#ifdef   _TIME_FUNC_SUPPORTED
    if ( TRUE )
    {
        time_t                      tmNow;
        struct tm*                  gmTimeNow;
        char*                       pAscTime;

        AnscGetNtpTime(&tmNow);
        gmTimeNow = gmtime((const time_t *)&tmNow);
        pAscTime = asctime((const struct tm*)gmTimeNow);

        if ( pAscTime )
        {
            int                     nLen= AnscSizeOfString(pAscTime); 
            int                     i;

            for ( i = nLen - 1; i >= 0; i -- )
            {
                if ( pAscTime[i] == '\r' || pAscTime[i] == '\n' )
                {
                    pAscTime[i] = 0;
                }
                else
                {
                    break;
                }
            }

            if ( pAscTime[0] )
            {
                _ansc_snprintf
                    (
                        pMsg+AnscSizeOfString(pMsg),
                        ulSize - AnscSizeOfString(pMsg),
                        "Date: %s\r\n",
                        pAscTime
                    );
            }
        }
    }
#endif

    /* Content-Length */
    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "Content-Length: %d\r\n",
            (int)(pHfoWwwAuth ? AnscSizeOfString(s_httpAuthMsgBody) : 0)
        );

    /* end of headers */
    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "\r\n"
        );

    /* message body */
    if ( pHfoWwwAuth )
    {
        _ansc_snprintf
            (
                pMsg+AnscSizeOfString(pMsg),
                ulSize - AnscSizeOfString(pMsg),
                s_httpAuthMsgBody
            );
    }

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoGenNonce
            (
                ANSC_HANDLE                 hThisObject,
                PUCHAR                      pNonce,
                ULONG                       ulNonceLen
            )

    description:

        This function is called to generate nonce.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PUCHAR                      pNonce
                Buffer to hold nonce.

                ULONG                       ulNonceLen
                The length in bytes the nonce would be, caller needs
                to make sure the buffer is large enough.

    return:     status of operation.

**********************************************************************/

extern ANSC_STATUS
AnscCryptoGetRandomOctets   
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       rand_seed,
        PUCHAR                      pOctetStream,
        ULONG                       ulSize
    );

ANSC_STATUS
CcspCwmpTcpcrhoGenNonce
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pNonce,
        ULONG                       ulNonceLen
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    ANSC_STATUS                     status       = ANSC_STATUS_SUCCESS;
    ULONG                           ulTimeNow    = AnscGetTickInMilliSeconds();
    ULONG                           ulRandomBytes= 0;
    ANSC_CRYPTO_HASH                MD5Hash;

    if ( ulNonceLen <= 2 )
    {
        return ANSC_STATUS_BAD_PARAMETER;
    }

    ulRandomBytes = ( ulNonceLen - 1 ) / 2;
    if ( ulRandomBytes >= ANSC_MD5_OUTPUT_SIZE )
    {
        ulRandomBytes = ANSC_MD5_OUTPUT_SIZE;
    }

    status =
    	AnscCryptoGetRandomOctets
            (
                NULL,
                ulTimeNow,
                MD5Hash.Value,
                ANSC_MD5_OUTPUT_SIZE
            );

    if ( status != ANSC_STATUS_SUCCESS )
    {
        _ansc_sprintf((char*)MD5Hash.Value, "%.8X", (unsigned int)ulTimeNow);

        AnscCryptoMd5Digest
            (
                (PVOID)MD5Hash.Value,
                AnscSizeOfString((char*)MD5Hash.Value),
                &MD5Hash
            );

        status = ANSC_STATUS_SUCCESS;
    }

    CcspCwmpTcpcrhoBinToHex(MD5Hash.Value, ulRandomBytes, pNonce);
    pNonce[ulRandomBytes * 2] = 0;

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoVerify
            (
                ANSC_HANDLE                 hThisObject,
                PVOID                       buffer,
                ULONG                       ulSize,
                PUCHAR                      pPassword
                ULONG                       ulServerAuthType,
                PUCHAR                      pServerAuthRealm,
                PUCHAR                      pServerNonce,
                ULONG                       ulNonceTimeout
            )

    description:

        This function is called to verify user's credentials.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PVOID                       buffer
                HTTP request message.

                ULONG                       ulSize
                HTTP request message size.

                ULONG                       ulServerAuthType
                Authentication type server specified.

                PUCHAR                      pServerAuthRealm
                Authentication realm server specified.

                PUCHAR                      pServerNonce
                Nonce server specified.

                PUCHAR                      pPassword
                User's password.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoVerify
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        ULONG                       ulSize,
        ANSC_HANDLE                 hAuthInfo,
        PUCHAR                      pPassword,
        ULONG                       ulServerAuthType,
        PUCHAR                      pServerAuthRealm,
        PUCHAR                      pServerNonce,
        ULONG                       ulNonceTimeout,
        ULONG                       ulNonceTimestamp
    )
{
    ANSC_STATUS                     status       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT)hThisObject;
    PHTTP_AUTHO_INFO                pAuthInfo    = (PHTTP_AUTHO_INFO          )hAuthInfo;
    errno_t rc = -1;
    int     ind = -1;

    

    if ( !pAuthInfo )
    {
        pAuthInfo   = (PHTTP_AUTHO_INFO)CcspCwmpTcpcrhoGetAuthInfo((ANSC_HANDLE)pMyObject, buffer, ulSize);
    }

    if ( !pAuthInfo )
    {
        status = ANSC_STATUS_DO_IT_AGAIN;
        goto EXIT;
    }

    if ( !pAuthInfo->pUserName )
    {
        status = ANSC_STATUS_BAD_AUTH_DATA;
        goto EXIT;
    }

    if ( pAuthInfo->AuthType != ulServerAuthType )
    {
        status = ANSC_STATUS_BAD_AUTH_DATA;
        goto EXIT;
    }

    if ( pAuthInfo->AuthType == HTTP_AUTH_TYPE_BASIC )
    {
        if ( !pPassword && !pAuthInfo->pPassword )
        {
            status = ANSC_STATUS_SUCCESS;
        }
        else if ( pPassword && pAuthInfo->pPassword )
        {
            rc = strcmp_s((char*)pAuthInfo->pPassword, strlen((const char *)(pAuthInfo->pPassword)), (char*)pPassword, &ind);
            ERR_CHK(rc);
            if((!ind) && (rc == EOK))
            {
                status = ANSC_STATUS_SUCCESS;
            }
            else
            {
                status = ANSC_STATUS_BAD_AUTH_DATA;
            }
        }
        else
        {
            status  = ANSC_STATUS_BAD_AUTH_DATA;
        }
    }
    else
    {
        PHTTP_AUTHO_DIGEST_INFO         pDigestInfo  = pAuthInfo->pDigest;
        char*                           pDigRep      = NULL;
        PUCHAR                          pEntityDigest= NULL;
        UCHAR                           buf[32]      = {0};
        PUCHAR                          pMethodName  = buf;
        PUCHAR                          pMsg         = (PUCHAR)buffer;
        PUCHAR                          pMsgEnd      = pMsg + ulSize - 1;
        PUCHAR                          pNext;
        ULONG                           ulLen;

        if ( !pDigestInfo->pDigestUri                           ||
             !pAuthInfo->pRealm                                 ||
             !pDigestInfo->pResponse                            ||
             (pDigestInfo->pQop && !pDigestInfo->pNonceCount)   ||
             !pDigestInfo->pNonce                               ||
             (pDigestInfo->pQop && !pDigestInfo->pCNonce) )
        {
            status = ANSC_STATUS_BAD_AUTH_DATA;
            goto EXIT;
        }
        rc = strcmp_s((char*)pAuthInfo->pRealm, strlen((const char *)(pAuthInfo->pRealm)), (char*)pServerAuthRealm,&ind); 
        ERR_CHK(rc);
        if((ind) || (rc != EOK))
        {
            status = ANSC_STATUS_BAD_AUTH_DATA;
            goto EXIT;
        }
        rc = strcmp_s((char*)pDigestInfo->pNonce, strlen((const char *)(pDigestInfo->pNonce)), (char*)pServerNonce,&ind);
        ERR_CHK(rc);
        if((ind) || (rc != EOK))
        {
            status = ANSC_STATUS_BAD_AUTH_DATA;
            goto EXIT;
        }

        pMsg    = CcspCwmpTcpcrhoSkipWS(pMsg, ulSize);
        ulSize  = pMsgEnd - pMsg + 1;

        pNext   = _ansc_memchr(pMsg, ' ', ulSize);
        if ( !pNext )
        {
            status = ANSC_STATUS_BAD_PAYLOAD;
            goto EXIT;
        }

        ulLen = pNext - pMsg;
        if ( ulLen >= 32 )
        {
            status = ANSC_STATUS_BAD_PAYLOAD;
            goto EXIT;
        }
        AnscCopyMemory(pMethodName, pMsg, ulLen);
        pMethodName[ulLen] = 0;

        pDigRep = (char*)
            CcspCwmpTcpcrhoCalcDigResponse
                (
                    (ANSC_HANDLE)pMyObject,
                    pMethodName,
                    pAuthInfo->pUserName,
                    pPassword,
                    pAuthInfo->pRealm,
                    pDigestInfo->pAlgorithm,
                    pDigestInfo->pNonce,
                    pDigestInfo->pCNonce,
                    pDigestInfo->pQop,
                    pDigestInfo->pNonceCount,
                    pEntityDigest,
                    pDigestInfo->pDigestUri
                );

        if ( !pDigRep )
        {
            status = ANSC_STATUS_RESOURCES;
        }
        else
        {



          rc = strcmp_s((char*)pDigestInfo->pResponse, strlen((const char *)(pDigestInfo->pResponse)), pDigRep,&ind);
          ERR_CHK(rc);
          if ((!ind) && (rc == EOK))
            {
                status = ANSC_STATUS_SUCCESS;
            }
            else
            {
                status = ANSC_STATUS_BAD_AUTH_DATA;
            }

            AnscFreeMemory(pDigRep);
        }

        if ( status == ANSC_STATUS_SUCCESS && ulNonceTimeout != 0 )
        {
            /* check if the nonce has expired */
            ULONG                   ulTimeNow    = AnscGetTickInSeconds();
            BOOL                    bNonceExpired;

            bNonceExpired   =
                (ulTimeNow >= ulNonceTimestamp) ?
                    ( ulTimeNow - ulNonceTimestamp > ulNonceTimeout ) :
                    ( 0xFFFFFFFF - ulNonceTimestamp + ulTimeNow > ulNonceTimeout );

            if ( bNonceExpired )
            {
                status  = ANSC_STATUS_DO_IT_AGAIN;
            }
        }
    }

EXIT:

    if ( (ANSC_HANDLE)NULL == hAuthInfo && pAuthInfo )
    {
        HttpAuthInfoRemove(pAuthInfo);
    }

    return status;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoGenResResponse
            (
                ANSC_HANDLE                 hThisObject,
                PVOID                       buffer,
                PULONG                      pulSize,
                char*                       pMediaType,
                char*                       pRes,
                ULONG                       ulResLen
            )

    description:

        This function is called to generate response to resource
        retrieval to requests other than connection requests.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PVOID                       buffer
                HTTP response buffer.

                PULONG                      pulSize
                HTTP response size.

                char*                       pMediaType
                MIME type.

                char*                       pRes
                Resource content.

                ULONG                       ulResLen
                Length of resource content.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoGenResResponse
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        PULONG                      pulSize,
        char*                       pMediaType,
        char*                       pRes,
        ULONG                       ulResLen
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    ANSC_STATUS                     status       = ANSC_STATUS_SUCCESS;
    char*                           pMsg         = (char*                     	   )buffer;
    ULONG                           ulSize       = *pulSize;
    ULONG                           ulContentLen = 0;

    *pMsg = 0;

    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "HTTP/1.1 200 OK\r\n"
        );

    /* construct Content-Type header */
    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "Content-Type: "
        );

    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            pMediaType
        );

    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "\r\n"
        );

    /* construct Connection header */
    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "Connection: close\r\n"
        );

    /* construct Server header */
    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "Server: Cisco-CcspCwmpTcpCR-MiniWebSvc/1.0\r\n"
        );

    /* Date header */
#ifdef   _TIME_FUNC_SUPPORTED
    if ( TRUE )
    {
        time_t                      tmNow;
        struct tm*                  gmTimeNow;
        char*                       pAscTime;

        AnscGetNtpTime(&tmNow);
        gmTimeNow = gmtime((const time_t *)&tmNow);
        pAscTime = asctime((const struct tm*)gmTimeNow);

        if ( pAscTime )
        {
            int                     nLen= AnscSizeOfString(pAscTime); 
            int                     i;

            for ( i = nLen - 1; i >= 0; i -- )
            {
                if ( pAscTime[i] == '\r' || pAscTime[i] == '\n' )
                {
                    pAscTime[i] = 0;
                }
                else
                {
                    break;
                }
            }

            if ( pAscTime[0] )
            {
                _ansc_snprintf
                    (
                        pMsg+AnscSizeOfString(pMsg),
                        ulSize - AnscSizeOfString(pMsg),
                        "Date: %s\r\n",
                        pAscTime
                    );
            }
        }
    }
#endif

    /* Content-Length */
    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "Content-Length: %u\r\n",
            (unsigned int)ulResLen
        );

    /* end of headers */
    _ansc_snprintf
        (
            pMsg+AnscSizeOfString(pMsg),
            ulSize - AnscSizeOfString(pMsg),
            "\r\n"
        );

    ulContentLen = AnscSizeOfString(pMsg) + 2;

    /* message body */
    if ( pRes && ulResLen <= ulSize - AnscSizeOfString(pMsg) )
    {
        AnscCopyMemory
            (
                pMsg+AnscSizeOfString(pMsg),
                pRes,
                ulResLen
            );

        ulContentLen += ulResLen;
    }
    else
    {
        status = ANSC_STATUS_INTERNAL_ERROR;
    }

    return status;
}



