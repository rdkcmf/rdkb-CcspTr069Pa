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

    module:	ccsp_cwmp_tcpcrho_states.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced state-access functions
        of CCSP CWMP TCP Connection Request Handler Object.

        *   CcspCwmpTcpcrhoGetCcspCwmpCpeController
        *   CcspCwmpTcpcrhoSetCcspCwmpCpeController
        *   CcspCwmpTcpcrhoGetWebAcmIf
        *   CcspCwmpTcpcrhoSetWebAcmIf
        *   CcspCwmpTcpcrhoGetProperty
        *   CcspCwmpTcpcrhoSetProperty
        *   CcspCwmpTcpcrhoResetProperty
        *   CcspCwmpTcpcrhoReset

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


#include "ccsp_cwmp_tcpcrho_global.h"



/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpTcpcrhoGetCcspCwmpCpeController
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

ANSC_HANDLE
CcspCwmpTcpcrhoGetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT  )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty    = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY)&pMyObject->Property;

    return  pMyObject->hCcspCwmpCpeController;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoSetCcspCwmpCpeController
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCcspCwmpCpeController
            );

    description:

        This function is called to configure object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hCcspCwmpCpeController
                Specifies the object state to be configured.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoSetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCcspCwmpCpeController
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT  )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty    = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY)&pMyObject->Property;

    pMyObject->hCcspCwmpCpeController = hCcspCwmpCpeController;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpTcpcrhoGetWebAcmIf
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

ANSC_HANDLE
CcspCwmpTcpcrhoGetWebAcmIf
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT  )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty    = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY)&pMyObject->Property;

    return  pMyObject->hWebAcmIf;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoSetWebAcmIf
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hWebAcmIf
            );

    description:

        This function is called to configure object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hCcspCwmpCpeController
                Specifies the object state to be configured.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoSetWebAcmIf
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWebAcmIf
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT  )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty    = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY)&pMyObject->Property;

    pMyObject->hWebAcmIf = hWebAcmIf;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoGetProperty
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hProperty
            );

    description:

        This function is called to retrieve object property.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hProperty
                Specifies the property data structure to be filled.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoGetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT  )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty    = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY)&pMyObject->Property;

    *(PCCSP_CWMP_TCPCR_HANDLER_PROPERTY)hProperty = *pProperty;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoSetProperty
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hProperty
            );

    description:

        This function is called to configure object property.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hProperty
                Specifies the property data structure to be copied.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoSetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT  )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty    = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY)&pMyObject->Property;

    *pProperty = *(PCCSP_CWMP_TCPCR_HANDLER_PROPERTY)hProperty;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoResetProperty
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to reset object property.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoResetProperty
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT  )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty    = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY)&pMyObject->Property;
    PWEB_AUTH_SERVER_PROPERTY       pAuthProperty= &pProperty->AuthProperty;

#ifdef _ANSC_IPV6_COMPATIBLE_
    AnscZeroMemory(pProperty->HostAddr, sizeof(pProperty->HostAddr));
#else
    pProperty->HostAddress.Value = 0;
#endif
    pProperty->HostPort          = CCSP_CWMP_TCPCR_CWMP_TCP_PORT;
    pProperty->ServerMode        = CCSP_CWMP_TCPCR_HANDLER_MODE_useXsocket;
    pProperty->SessionTimeout    = CCSP_CWMP_TCPCR_HANDLER_DefSessionTimeout;

    pAuthProperty->AuthType      = HTTP_AUTH_TYPE_DIGEST;
    pAuthProperty->Algorithm     = WEB_AUTH_ALGORITHM_MD5;
    pAuthProperty->bNoQop        = FALSE;
    pAuthProperty->NonceTimeout  = WEB_AUTH_NONCE_TIMEOUT_INTERVAL;
    errno_t rc =    -1;
    
rc = strcpy_s((char*)pAuthProperty->Realm,sizeof(pAuthProperty->Realm),"Cisco_CCSP_CWMP_TCPCR");
if(rc!= EOK)
{
   ERR_CHK(rc);
   return ANSC_STATUS_FAILURE;
}
rc = strcpy_s((char*)pAuthProperty->Domain,sizeof(pAuthProperty->Domain), "/");
if(rc!= EOK)
{
  ERR_CHK(rc);
  return ANSC_STATUS_FAILURE;
}

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoReset
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to reset object states.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoReset
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT  )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty    = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY)&pMyObject->Property;
    PCCSP_CWMP_TCPCR_HANDLER_SESSINFO    pWebAuthInfo = &pMyObject->AuthSessionInfo;
    PWEB_AUTH_SERVER_PROPERTY       pAuthProperty= &pWebAuthInfo->ServerAuthInfo;
    ULONG                           ulTimes      = 0;

    pMyObject->LastAccessTime    = 0;
    pMyObject->MagicID           = 0;

    while ( pMyObject->MagicID == 0 && ulTimes <= 3 )
    {
        ULONG                       ulRandValue = AnscGetTickInMilliSeconds() * 1103515245 + 12345;

        /* Randomly pick up the starting global session id to be hard predictable */
        pMyObject->MagicID = (ulRandValue % 0x7FFFFFFF);

        ulTimes ++;
    }

    if ( pMyObject->MagicID == 0 )
    {
        pMyObject->MagicID = (ULONG)hThisObject;
    }

    AnscZeroMemory(pWebAuthInfo, sizeof(WEB_AUTH_SESSION_INFO));

    return  ANSC_STATUS_SUCCESS;
}

