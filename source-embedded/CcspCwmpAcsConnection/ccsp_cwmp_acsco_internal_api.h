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

    module: ccsp_cwmp_acsconn_internal_api.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This header file contains the prototype definition for all
        the internal functions provided by the CCSP CWMP ACS Connection
        Object.

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


#ifndef  _CCSP_CWMP_ACSCONN_INTERNAL_API_
#define  _CCSP_CWMP_ACSCONN_INTERNAL_API_

/***********************************************************
         CONST VALUES FOR ACSCONNECTION
***********************************************************/
/*
 * URL for the CPE to connect to the ACS using the CPE
 * WAN Management Protocol. In one of the folowing forms:
 *
 *  x-dslf_cwmp://host:port/
 *  x-dslf_cwmps://host:port/
 *
 * The "s" suffix of the scheme component indicates the use
 * of SSL. 
 */
#define  ACS_DSLF_CWMP                          "x-dslf_cwmp"
#define  ACS_DSLF_CWMPS                         "x-dslf_cwmps"

/***********************************************************
         FUNCTIONS IMPLEMENTED IN CCSP_CWMP_ACSCO_STATES.C
***********************************************************/

typedef  struct
_ANSC_ACS_INTERN_HTTP_CONTENT
{
    ULONG                           ulContentSize;
    PVOID                           pContent;
    PCHAR                           SoapMessage;
    ANSC_STATUS                     CompleteStatus;
    ANSC_EVENT                      CompleteEvent;
    PCHAR                           MethodName;
    BOOL                            bIsRedirect;
    BOOL                            bUnauthorized;
}
ANSC_ACS_INTERN_HTTP_CONTENT,  *PANSC_ACS_INTERN_HTTP_CONTENT;


ANSC_HANDLE
CcspCwmpAcscoGetHttpSimpleClient
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpAcscoGetCcspCwmpSession
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpAcscoSetCcspCwmpSession
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hContext
    );

char*
CcspCwmpAcscoGetAcsUrl
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpAcscoSetAcsUrl
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

char*
CcspCwmpAcscoGetUsername
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpAcscoSetUsername
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

char*
CcspCwmpAcscoGetPassword
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpAcscoSetPassword
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

ANSC_STATUS
CcspCwmpAcscoReset
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_ACSCO_PROCESS.C
***********************************************************/

ANSC_STATUS
CcspCwmpAcscoConnect
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpAcscoRequest
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pSoapMessage,
        char*                       pMethodName,
        ULONG                       ulReqEnvCount,
        ULONG                       ulRepEnvCount
    );

ANSC_STATUS
CcspCwmpAcscoRequestOnly
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpAcscoClose
    (
        ANSC_HANDLE                 hThisObject
    );

/***********************************************************
         FUNCTIONS IMPLEMENTED IN CCSP_CWMP_ACSCO_HTTPBSPIF.C
***********************************************************/
ANSC_STATUS
CcspCwmpAcscoHttpBspPolish
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hBmoReq,
        ANSC_HANDLE                 hReqContext
    );

ANSC_STATUS
CcspCwmpAcscoHttpBspBrowse
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hBmoReq,
        ANSC_HANDLE                 hBmoRep,
        ANSC_HANDLE                 hReqContext
    );

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

ANSC_STATUS
CcspCwmpAcscoHttpAddCookie
    (
        ANSC_HANDLE                 hThisObject,
        PCHAR                       pCookie
    );

ANSC_STATUS
CcspCwmpAcscoHttpRemoveCookies
    (
        ANSC_HANDLE                 hThisObject
    );

int
CcspCwmpAcscoHttpFindCookie
	(
		ANSC_HANDLE                 hThisObject,
		PCHAR                       pCookieName
	);

ANSC_STATUS
CcspCwmpAcscoHttpDelCookie
	(
		ANSC_HANDLE                 hThisObject,
		ULONG                       ulIndex
	);

ANSC_STATUS
CcspCwmpAcscoHttpSessionClosed
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
         FUNCTIONS IMPLEMENTED IN CCSP_CWMP_ACSCO_HTTPACMIF.C
***********************************************************/
ANSC_STATUS
CcspCwmpAcscoHttpGetCredential
    (      
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pHostName,
        USHORT                      HostPort,
        PUCHAR                      pUriPath,
        PUCHAR*                     ppUserName,
        PUCHAR*                     ppPassword
    );

#endif
