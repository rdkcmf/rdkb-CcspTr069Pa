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

    module:	ccsp_cwmp_tcpcrho_internal_api.h

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This header file contains the prototype definition for all
        the internal functions provided by CCSP CWMP TCP Connection
        Request Handler Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/19/08    initial revision.

**********************************************************************/


#ifndef  _CCSP_CWMP_TCPCRHO_INTERNAL_API_
#define  _CCSP_CWMP_TCPCRHO_INTERNAL_API_


/***********************************************************
          FUNCTIONS IMPLEMENTED IN CCSP_CWMP_TCPCRHO_STATES.C
***********************************************************/

ANSC_HANDLE
CcspCwmpTcpcrhoGetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpTcpcrhoSetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCcspCwmpCpeController
    );

ANSC_HANDLE
CcspCwmpTcpcrhoGetWebAcmIf
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpTcpcrhoSetWebAcmIf
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWebAcmIf
    );

ANSC_STATUS
CcspCwmpTcpcrhoGetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

ANSC_STATUS
CcspCwmpTcpcrhoSetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

ANSC_STATUS
CcspCwmpTcpcrhoResetProperty
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpTcpcrhoReset
    (
        ANSC_HANDLE                 hThisObject
    );

/***********************************************************
       FUNCTIONS IMPLEMENTED IN CCSP_CWMP_TCPCRHO_OPERATION.C
***********************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoEngage
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpTcpcrhoCancel
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpTcpcrhoCreateTcpServers
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpTcpcrhoRemoveTcpServers
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpTcpcrhoWorkerInit
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpTcpcrhoWorkerUnload
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
          FUNCTIONS IMPLEMENTED IN CCSP_CWMP_TCPCRHO_DSTOWO.C
***********************************************************/

BOOL
CcspCwmpTcpcrhoDstowoAccept
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PANSC_HANDLE                phClientContext
    );

ANSC_STATUS
CcspCwmpTcpcrhoDstowoSetOut
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket
    );

ANSC_STATUS
CcspCwmpTcpcrhoDstowoRemove
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket
    );

ULONG
CcspCwmpTcpcrhoDstowoQuery
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PVOID                       buffer,
        ULONG                       ulSize,
        PANSC_HANDLE                phQueryContext
    );

ANSC_STATUS
CcspCwmpTcpcrhoDstowoProcessSync
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PVOID                       buffer,
        ULONG                       ulSize,
        ANSC_HANDLE                 hQueryContext
    );

ANSC_STATUS
CcspCwmpTcpcrhoDstowoProcessAsync
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PVOID                       buffer,
        ULONG                       ulSize,
        ANSC_HANDLE                 hQueryContext
    );

ANSC_STATUS
CcspCwmpTcpcrhoDstowoSendComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        ANSC_HANDLE                 hReserved,
        ANSC_STATUS                 status
    );

ANSC_STATUS
CcspCwmpTcpcrhoDstowoNotify
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        ULONG                       ulEvent,
        ANSC_HANDLE                 hReserved
    );


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_TCPCRHO_PROCESS.C
***********************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoProcessRequest
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PVOID                       buffer,
        ULONG                       ulSize
    );

ANSC_STATUS
CcspCwmpTcpcrhoVerifyCredential
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        ULONG                       ulSize,
        ANSC_HANDLE                 hSessAuthInfo
    );


/***********************************************************
   INTERNAL FUNCTIONS IMPLEMENTED IN CCSP_CWMP_TCPCRHO_AUTH.C
***********************************************************/

PUCHAR
CcspCwmpTcpcrhoSkipWS
    (
        PUCHAR                  pMsg,
        ULONG                   ulSize
    );

PUCHAR
CcspCwmpTcpcrhoSkipTrailingWS
    (
        PUCHAR                  pMsg,
        ULONG                   ulSize
    );

void
CcspCwmpTcpcrhoBinToHex
    (
        PUCHAR                  Bin,
        ULONG                   ulBinLen,
        PUCHAR                  Hex
    );

PUCHAR
CcspCwmpTcpcrhoFindHeader
    (
        PUCHAR                  pMsg,
        ULONG                   ulSize,
        PUCHAR                  pHeaderName,
        PULONG                  pulHdrValueOffset,
        PULONG                  pulHdrValeLen
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
    );

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
    );

ANSC_STATUS
CcspCwmpTcpcrhoCalcDigestHA2
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pMethodName,
        PUCHAR                      pQop,
        PUCHAR                      pEntityDigest,
        PUCHAR                      pDigestUri,
        PUCHAR                      pHA2
    );

ANSC_STATUS
CcspCwmpTcpcrhoGetRequestHostUri
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        ULONG                       ulSize,
        PUCHAR                      *ppHostName,
        PUSHORT                     pHostPort,
        PUCHAR                      *ppUriPath
    );

ANSC_HANDLE
CcspCwmpTcpcrhoGetAuthInfo
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        ULONG                       ulSize
    );

ANSC_STATUS
CcspCwmpTcpcrhoGetDigestAuthInfo
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hAuthInfo,
        PUCHAR                      pDigData
    );

PUCHAR
CcspCwmpTcpcrhoGenBasicChallenge
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pRealm
    );

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
    );

ANSC_STATUS
CcspCwmpTcpcrhoGenBasicResponse
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        PULONG                      pulSize,
        PUCHAR                      pRealm
    );

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
    );

ANSC_STATUS
CcspCwmpTcpcrhoGenResponse
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        PULONG                      pulSize,
        ANSC_HANDLE                 hAuthHeader
    );

ANSC_STATUS
CcspCwmpTcpcrhoGenNonce
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pNonce,
        ULONG                       ulNonceLen
    );

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
    );

ANSC_STATUS
CcspCwmpTcpcrhoGenResResponse
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        PULONG                      pulSize,
        char*                       pMimeType,
        char*                       pRes,
        ULONG                       ulResLen
    );

/***********************************************************
   INTERNAL FUNCTIONS IMPLEMENTED IN CCSP_CWMP_TCPCRHO_MSG.C
***********************************************************/

BOOL
CcspCwmpTcpcrhoParseCredentials
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCredentials,
        PUCHAR                      pBuf,
        ULONG                       ulSize
    );

BOOL
CcspCwmpTcpcrhoParseBasicCredentials
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCredentials,
        PUCHAR                      pBuf,
        ULONG                       ulSize
    );

BOOL
CcspCwmpTcpcrhoParseDigestCredentials
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCredentials,
        PUCHAR                      pBuf,
        ULONG                       ulSize
    );

BOOL
CcspCwmpTcpcrhoIsRequestComplete
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pBuf,
        ULONG                       ulSize
    );

BOOL
CcspCwmpTcpcrhoIsValidConnRequest
    (
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pBuf,
        ULONG                       ulSize,
        PUCHAR*                     pUrlPath
    );

BOOL
CcspCwmpTcpcrhoMatchSession
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PUCHAR                      pBuf,
        ULONG                       ulSize
    );


#endif
