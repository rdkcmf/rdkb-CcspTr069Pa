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

    module:	ccsp_cwmp_stunmo_stunbsmif.c

        For CCSP CWMP protocol 

    ---------------------------------------------------------------

    description:

        This module implements the advanced interface functions of
        the CCSP CWMP Stun Manager Object.

        *   CcspCwmpStunmoStunBsmRecvMsg1
        *   CcspCwmpStunmoStunBsmRecvMsg2
        *   CcspCwmpStunmoStunBsmNotify

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/05/08    initial revision.
        11/02/11    migrate to CCSP framework

**********************************************************************/


#include "ccsp_cwmp_stunmo_global.h"
#include "ccsp_memory.h"

extern ULONG
AnscCryptoHmacSha1Digest
    (
        PVOID                       buffer,
        ULONG                       size,
        PANSC_CRYPTO_HASH           hash,
        PANSC_CRYPTO_KEY            key 
    );

typedef  struct
_CCSP_CWMP_UDP_CONN_REQ_PARAMS
{
    char*                           pTimestamp;
    char*                           pMessageID;
    char*                           pUsername;
    char*                           pCnonce;
    char*                           pSignature;
}
CCSP_CWMP_UDP_CONN_REQ_PARAMS, *PCCSP_CWMP_UDP_CONN_REQ_PARAMS;


static
ANSC_STATUS
CcspCwmpStunmoRetryTask
    (
        ANSC_HANDLE                 hTaskContext
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_STUN_MANAGER_OBJECT       pMyObject    = (PCCSP_CWMP_STUN_MANAGER_OBJECT)hTaskContext;
    ULONG                           ulWaitTime   = 30;
    ULONG                           ulTimeNow    = AnscGetTickInSeconds();
    ULONG                           ulRetryTime  = ulTimeNow + ulWaitTime;

    AnscTrace("CcspCwmpStunmoRetryTask - retry STUN after %u seconds.\n", (unsigned int)ulWaitTime);

    pMyObject->bRetryTaskRunning = TRUE;

    while ( pMyObject->bActive && ulTimeNow < ulRetryTime )
    {
        AnscSleep(3000);
        ulTimeNow = AnscGetTickInSeconds();
    }

    if ( pMyObject->bActive )
    {
        AnscTrace("CcspCwmpStunmoRetryTask - retrying STUN ...\n");
        returnStatus = pMyObject->ApplyStunSettings((ANSC_HANDLE)pMyObject);
    }

    pMyObject->bRetryTaskRunning = FALSE;

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        static  ANSC_STATUS
        CcspCwmpStunmoStunBsmParseUdpConnReq
            (
                char*                       pReqLine,
                PCCSP_CWMP_UDP_CONN_REQ_PARAMS   pCrParams
            )

    description:

        This function is called to process an incoming message
        re-directed by STUN client.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pReqLine
                Contains request line, after parsing, the original
                content will most likely be changed.

                PCCSP_CWMP_UDP_CONN_REQ_PARAMS   pCrParams
                Connection request parameters.

    return:     status of operation.

**********************************************************************/

static  ANSC_STATUS
CcspCwmpStunmoStunBsmParseUdpConnReq
    (
        char*                       pReqLine,
        PCCSP_CWMP_UDP_CONN_REQ_PARAMS   pCrParams
    )
{
    char*                           pCur;
    char*                           pEnd;
    ULONG                           ulLen;
    char*                           pValue;
    char*                           pNext;

    pCur = pReqLine;

    if ( !AnscEqualString2(pCur, "GET", 3, FALSE) )
    {
        return  ANSC_STATUS_BAD_PAYLOAD;
    }

    pCur += 3;

    if ( *pCur != ' ' && *pCur != '\t' )
    {
        return  ANSC_STATUS_BAD_PAYLOAD;
    }

    pCur ++;
    while ( *pCur != 0 && (*pCur == ' ' || *pCur == '\t') )
    {
        pCur ++;
    }

    if ( *pCur == 0 )
    {
        return  ANSC_STATUS_BAD_PAYLOAD;
    }

    if ( !AnscEqualString2(pCur, "http://", 7, FALSE) )
    {
        return  ANSC_STATUS_BAD_PAYLOAD;
    }

    pCur += 7;

    /* skip host:port and look for query string, shall we verify host:port equals to UdpConnReqAddress? */
    /* shall we consider URL-Encoding? - probably NOT, we can go back do it as it is definitely needed */
    pCur = _ansc_strchr(pCur, '?');
    if ( !pCur )
    {
        return  ANSC_STATUS_BAD_PAYLOAD;
    }

    AnscZeroMemory(pCrParams, sizeof(CCSP_CWMP_UDP_CONN_REQ_PARAMS));

    pCur ++;
    pEnd = _ansc_strchr(pCur, ' ');
    if ( !pEnd )
    {
        pEnd = _ansc_strchr(pCur, '\t');
    }

    if ( !pEnd )
    {
        return  ANSC_STATUS_BAD_PAYLOAD;
    }

    *pEnd = 0;

    while ( pCur )
    {
        pNext  = _ansc_strchr(pCur, '&');
        pValue = _ansc_strchr(pCur, '=');

        if ( pValue && ((pNext && pValue < pNext) || pValue) )
        {
            ulLen = pValue - pCur;

            if ( ulLen == 3 || AnscEqualString2(pCur, "sig", 3, FALSE) )
            {
                pCrParams->pSignature = pValue+1;
            }
            else if ( ulLen == 2 )
            {
                if ( AnscEqualString2(pCur, "ts", 2, FALSE) )
                {
                    pCrParams->pTimestamp = pValue+1;
                }
                else if ( AnscEqualString2(pCur, "id", 2, FALSE) )
                {
                    pCrParams->pMessageID = pValue+1;
                }
                else if ( AnscEqualString2(pCur, "un", 2, FALSE) )
                {
                    pCrParams->pUsername  = pValue+1;
                }
                else if ( AnscEqualString2(pCur, "cn", 2, FALSE) )
                {
                    pCrParams->pCnonce    = pValue+1;
                }
            }
        }

        pCur = pNext ? pNext+1 : NULL;
        if ( pNext )
        {
            *pNext = 0;
        }
    }

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoStunBsmRecvMsg1
            (
                ANSC_HANDLE                 hThisObject,
                void*                       buffer,
                ULONG                       ulSize
            );

    description:

        This function is called to process an incoming message
        re-directed by STUN client.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                void*                       buffer
                Specifies the incoming message to be processed.

                ULONG                       ulSize
                Specifies the size of the incoming message.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoStunBsmRecvMsg1
    (
        ANSC_HANDLE                 hThisObject,
        void*                       buffer,
        ULONG                       ulSize
    )
{
    ANSC_STATUS                     status              = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_STUN_MANAGER_OBJECT  pMyObject           = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    char*                           pMsg                = (char*                      )buffer;
    char*                           pMsgEnd             = pMsg + ulSize - 1;
    char*                           pConnReqUsername    = NULL;
    char*                           pConnReqPassword    = NULL;
    char*                           pReqLine            = pMsg;
    char*                           pReqLineEnd;
    CCSP_CWMP_UDP_CONN_REQ_PARAMS        CrParams;
    ANSC_CRYPTO_KEY                 key;
    ANSC_CRYPTO_HASH                hash;
    char*                           pText;
    ULONG                           ulTextLen;
    ULONG                           ulTimestamp;
    ULONG                           ulMessageID;
    errno_t                         rc = -1;
    int                             ind = -1;

    CcspTr069PaTraceDebug(("CcspCwmpStunmoStunBsmRecvMsg1 - processing an imcoming UDP connection request ... \n"));

    /*
     * Because STUN responses and UDP Connection Requests will be received on the same UDP port,
     * the CPE MUST appropriately distinguish STUN messages from UDP Connection Requests using the
     * content of the messages themselves.
     */

    /* 40 is a conservative minimum size required by TR-106 for UDP connection request,
     * by no means it is accurate
     */
    if ( !pMsg || ulSize < 40 )
    {
        CcspTr069PaTraceWarning(("CcspCwmpStunmoStunBsmRecvMsg1 - UDP connection request payload size is too small, dropped! \n"));
        return  ANSC_STATUS_BAD_PAYLOAD;
    }

    /* To be simplifed, we now only check request line and do authentication as
     * TR-069 Amendment 1/2 Annex G defines.
     */

    pReqLineEnd = _ansc_memchr(pReqLine, '\n', pMsgEnd - pReqLine + 1);

    if ( !pReqLineEnd )
    {
        CcspTr069PaTraceWarning(("CcspCwmpStunmoStunBsmRecvMsg1 - UDP connection request payload is malformed (LF missing), dropped! \n"));
        return  ANSC_STATUS_BAD_PAYLOAD;
    }

    pReqLineEnd --;
    if ( *pReqLineEnd == '\r' )
    {
        pReqLineEnd --;
    }
    *(pReqLineEnd+1) = 0;

    status =
        CcspCwmpStunmoStunBsmParseUdpConnReq
            (
                pReqLine,
                &CrParams
            );

    if ( status != ANSC_STATUS_SUCCESS ||
         ! CrParams.pTimestamp         ||
         ! CrParams.pMessageID         ||
         /* ! CrParams.pUsername          || */ /* no authentication on connection request possibly? */
         ! CrParams.pCnonce            ||
         ! CrParams.pSignature         ||
         AnscSizeOfString(CrParams.pSignature) != 2 * ANSC_SHA1_OUTPUT_SIZE )
    {
        CcspTr069PaTraceWarning(("CcspCwmpStunmoStunBsmRecvMsg1 - UDP connection request payload is malformed (bad query string), dropped! \n"));
        return  ANSC_STATUS_BAD_PAYLOAD;
    }

    /* verify signature and convert it into binary value */
    if ( TRUE )
    {
        int                         i;
        UCHAR                       uc1, uc2;

        for ( i = 0; i < ANSC_SHA1_OUTPUT_SIZE; i ++ )
        {
            uc1 = CrParams.pSignature[2*i];
            uc2 = CrParams.pSignature[2*i+1];

            if ( uc1 >= '0' && uc1 <= '9' )
            {
                uc1 -= '0';
            }
            else if ( uc1 >= 'a' && uc1 <= 'f' )
            {
                uc1 = uc1 - 'a' + 10;
            }
            else if ( uc1 >= 'A' && uc1 <= 'F' )
            {
                uc1 = uc1 - 'A' + 10;
            }
            else
            {
                return  ANSC_STATUS_BAD_PAYLOAD;
            }

            if ( uc2 >= '0' && uc2 <= '9' )
            {
                uc2 -= '0';
            }
            else if ( uc2 >= 'a' && uc2 <= 'f' )
            {
                uc2 = uc2 - 'a' + 10;
            }
            else if ( uc2 >= 'A' && uc2 <= 'F' )
            {
                uc2 = uc2 - 'A' + 10;
            }
            else
            {
                return  ANSC_STATUS_BAD_PAYLOAD;
            }

            CrParams.pSignature[i] = (uc1 << 4) + uc2;
        }
    }

    ulTimestamp = (ULONG)_ansc_atol(CrParams.pTimestamp);
    ulMessageID = (ULONG)_ansc_atol(CrParams.pMessageID);

    /* verify timestamp and message ID */
    if ( ulMessageID == pMyObject->LastUdpConnReqID && pMyObject->LastUdpConnReqTS != 0 )
    {
        CcspTr069PaTraceWarning(("CcspCwmpStunmoStunBsmRecvMsg1 - duplicate message id, dropped! \n"));
        /* duplicate message */
        return  ANSC_STATUS_SUCCESS;
    }

    if ( ulTimestamp != 0 && ulTimestamp < pMyObject->LastUdpConnReqTS )
    {
        CcspTr069PaTraceWarning(("CcspCwmpStunmoStunBsmRecvMsg1 - timestamp is way too far from current time (possible message replay attack?), dropped! \n"));
        /* message delayed too much or replay ? */
        return  ANSC_STATUS_FAILURE;
    }

    /* verify username */
 ULONG CrParams_size  = AnscSizeOfString(CrParams.pUsername);

    pConnReqUsername = 
        CcspManagementServer_GetConnectionRequestUsername
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );
    rc = strcmp_s(CrParams.pUsername,CrParams_size,pConnReqUsername,&ind);
    ERR_CHK(rc);
    if ( (!pConnReqUsername && CrParams.pUsername)                              ||
       ((pConnReqUsername && *pConnReqUsername != 0) && !CrParams.pUsername ) ||
       (pConnReqUsername && CrParams.pUsername && ((ind) && (rc == EOK))))
    
    {
        if ( pConnReqUsername )
        {
            AnscFreeMemory(pConnReqUsername);
        }

        CcspTr069PaTraceWarning(("CcspCwmpStunmoStunBsmRecvMsg1 - username does not match, dropped! \n"));
        return  ANSC_STATUS_BAD_PAYLOAD;
    }

    if ( pConnReqUsername )
    {
        AnscFreeMemory(pConnReqUsername);
    }

    /* prepare 'Key' */
    pConnReqPassword = 
        CcspManagementServer_GetConnectionRequestPassword
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    key.KeyNumber    = 1;
    key.RoundNumber  = 0;
    key.Length       = pConnReqPassword ? AnscSizeOfString(pConnReqPassword) : 0;

    if ( key.Length != 0 )
    {
       rc = strcpy_s((char *)key.Value[0],sizeof(key.Value[0]),pConnReqPassword);
       if(rc!=EOK)
       {
          ERR_CHK(rc);
          return  ANSC_STATUS_FAILURE;
       }
    }

    if ( pConnReqPassword )
    {
        AnscFreeMemory(pConnReqPassword);
    }

    /* prepare 'Text' */
    ulTextLen =
        AnscSizeOfString(CrParams.pTimestamp) +
        AnscSizeOfString(CrParams.pMessageID) +
        (CrParams.pUsername ? AnscSizeOfString(CrParams.pUsername) : 0) +
        AnscSizeOfString(CrParams.pCnonce);

    pText = (char*)AnscAllocateMemory(ulTextLen + 2);
    if ( !pText )
    {
        CcspTr069PaTraceWarning(("CcspCwmpStunmoStunBsmRecvMsg1 - out of resources, dropped! \n"));
        return  ANSC_STATUS_RESOURCES;
    }

    rc = strcpy_s(pText,ulTextLen + 2,CrParams.pTimestamp);
    if(rc!=EOK)
    {
      ERR_CHK(rc);
      AnscFreeMemory(pText);
      return  ANSC_STATUS_FAILURE;
    }
    rc = strcat_s(pText,ulTextLen + 2,CrParams.pMessageID);
        if(rc!=EOK)
    {
      ERR_CHK(rc);
      AnscFreeMemory(pText);
      return  ANSC_STATUS_FAILURE;
    }

    if ( CrParams.pUsername )
    {
        rc = strcat_s(pText,ulTextLen + 2,CrParams.pUsername);
       if(rc!=EOK)
      {
         ERR_CHK(rc);
         AnscFreeMemory(pText);
         return  ANSC_STATUS_FAILURE;
      }

    }
    rc = strcat_s(pText,ulTextLen + 2, CrParams.pCnonce   );
    if(rc!=EOK)
    {
         ERR_CHK(rc);
          AnscFreeMemory(pText);
         return  ANSC_STATUS_FAILURE;
    }
    hash.Length = ANSC_SHA1_OUTPUT_SIZE;
    AnscCryptoHmacSha1Digest
        (
            (PVOID)pText,
            (ULONG)AnscSizeOfString(pText),
            &hash,
            &key
        );

    AnscFreeMemory(pText);

    if ( hash.Length != ANSC_SHA1_OUTPUT_SIZE &&
         !AnscEqualMemory(hash.Value, CrParams.pSignature, ANSC_SHA1_OUTPUT_SIZE) )
    {
        CcspTr069PaTraceWarning(("CcspCwmpStunmoStunBsmRecvMsg1 - signatures do not match, dropped! \n"));
        return  ANSC_STATUS_FAILURE;
    }
    else
    {
        PCCSP_CWMP_ACS_BROKER_OBJECT     pCcspCwmpAcsBroker  = (PCCSP_CWMP_ACS_BROKER_OBJECT      )pCcspCwmpCpeController->hCcspCwmpAcsBroker;
        PCCSP_CWMP_MSO_INTERFACE         pCcspCwmpMsoIf      = (PCCSP_CWMP_MSO_INTERFACE          )pCcspCwmpAcsBroker->hCcspCwmpMsoIf;

        /* save Timestamp and ID */
        pMyObject->LastUdpConnReqTS = ulTimestamp;
        pMyObject->LastUdpConnReqID = ulMessageID;
        
        CcspTr069PaTraceInfo(("CcspCwmpStunmoStunBsmRecvMsg1 - UDP connection request is accepted, ACS is going to be informed! \n"));

        /* make connection request */
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

            pCcspCwmpCpeController->InformNow((ANSC_HANDLE)pCcspCwmpCpeController);
        }
        else
        {
            status =
                pCcspCwmpMsoIf->Inform
                    (
                        pCcspCwmpMsoIf->hOwnerContext,
                        CCSP_CWMP_INFORM_EVENT_NAME_ConnectionRequest,
                        NULL,
                        pCcspCwmpCpeController->bBootInformSent
                    );
        }

        return  ANSC_STATUS_SUCCESS;
    }
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoStunBsmRecvMsg2
            (
                ANSC_HANDLE                 hThisObject,
                void*                       buffer,
                ULONG                       ulSize
            );

    description:

        This function is called to process an incoming message
        re-directed by STUN client.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                void*                       buffer
                Specifies the incoming message to be processed.

                ULONG                       ulSize
                Specifies the size of the incoming message.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoStunBsmRecvMsg2
    (
        ANSC_HANDLE                 hThisObject,
        void*                       buffer,
        ULONG                       ulSize
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    UNREFERENCED_PARAMETER(buffer);
    UNREFERENCED_PARAMETER(ulSize);
    /*
     * This function is reserved for future use...
     */

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoStunBsmNotify
            (
                ANSC_HANDLE                 hThisObject,
                ULONG                       ulEvent,
                ANSC_HANDLE                 hReserved
            );

    description:

        This function is called to process a notification from STUN
        client.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ULONG                       ulEvent
                Specifies the notification event to be processed.

                ANSC_HANDLE                 hReserved
                Specifies the associated event context.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoStunBsmNotify
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulEvent,
        ANSC_HANDLE                 hReserved
    )
{
    UNREFERENCED_PARAMETER(hReserved);
    PCCSP_CWMP_STUN_MANAGER_OBJECT       pMyObject    = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PCCSP_CWMP_STUN_MANAGER_PROPERTY     pProperty    = (PCCSP_CWMP_STUN_MANAGER_PROPERTY)&pMyObject->Property;
    BOOL                            bRetryStun   = FALSE;
    PSTUN_SIMPLE_CLIENT_OBJECT      pStunSimpleClient   = (PSTUN_SIMPLE_CLIENT_OBJECT )pMyObject->hStunSimpleClient;
    PCCSP_CWMP_STUN_INFO                pCcspCwmpStunInfo      = (PCCSP_CWMP_STUN_INFO            )&pMyObject->CcspCwmpStunInfo;

    switch ( ulEvent )
    {
        case    STUN_BSM_EVENT_serverUnreachable :

                bRetryStun = TRUE;

                break;

        case    STUN_BSM_EVENT_errorReturned :

                bRetryStun = TRUE;

                break;

        case    STUN_BSM_EVENT_unknownMsgReceived :

                break;

        case    STUN_BSM_EVENT_bindingRemoved :
        case    STUN_BSM_EVENT_bindingChanged :

                if ( TRUE )
                {
                    char            udp_cru[257] = {0};
                    pCcspCwmpStunInfo->NATDetected = pStunSimpleClient->BindingInfo.bNatDetected;

                    if ( pCcspCwmpStunInfo->NATDetected )
                    {
                        _ansc_sprintf
                            (
                                udp_cru,
                                "%d.%d.%d.%d:%d",
                                pStunSimpleClient->BindingInfo.Ip4Addr.Dot[0],
                                pStunSimpleClient->BindingInfo.Ip4Addr.Dot[1],
                                pStunSimpleClient->BindingInfo.Ip4Addr.Dot[2],
                                pStunSimpleClient->BindingInfo.Ip4Addr.Dot[3],
                                pStunSimpleClient->BindingInfo.UdpPort
                            );
                    }
                    else if ( pProperty->ClientAddr.Value != 0 )
                    {
                        _ansc_sprintf
                            (
                                udp_cru,
                                "%d.%d.%d.%d:%d",
                                pProperty->ClientAddr.Dot[0],
                                pProperty->ClientAddr.Dot[1],
                                pProperty->ClientAddr.Dot[2],
                                pProperty->ClientAddr.Dot[3],
                                pProperty->ClientPort
                            );
                    }
                    else
                    {
                        ANSC_IPV4_ADDRESS       client_ip4_addr;

                        AnscGetLocalHostAddress(client_ip4_addr.Dot);

                        _ansc_sprintf
                            (
                                udp_cru,
                                "%d.%d.%d.%d:%d",
                                client_ip4_addr.Dot[0],
                                client_ip4_addr.Dot[1],
                                client_ip4_addr.Dot[2],
                                client_ip4_addr.Dot[3],
                                pProperty->ClientPort
                            );
                    }

                    CcspManagementServer_StunBindingChanged
                        (
                            pCcspCwmpStunInfo->NATDetected,
                            udp_cru
                        );
                }       

                break;

        default :

                break;
    }

    if ( bRetryStun && !pMyObject->bRetryTaskRunning )
    {
        AnscSpawnTask3
            (
                (void*)CcspCwmpStunmoRetryTask,
                (ANSC_HANDLE)pMyObject,
                "CcspCwmpStunmoRetryTask",
                USER_DEFAULT_TASK_PRIORITY,
                USER_DEFAULT_TASK_STACK_SIZE
             );
    }

    return  ANSC_STATUS_SUCCESS;
}
