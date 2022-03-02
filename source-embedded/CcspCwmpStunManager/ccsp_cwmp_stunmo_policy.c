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

    module:	ccsp_cwmp_stunmo_policy.c

        For CCSP CWMP protocol 

    ---------------------------------------------------------------

    description:

        This module implements the advanced policy-loading
        functions of the CCSP CWMP Stun Manager Object.

        *   CcspCwmpStunmoLoadPolicy
        *   CcspCwmpStunmoApplyStunSettings
        *   CcspCwmpStunmoGetAcsHostName

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


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoLoadPolicy
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to load the policy.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoLoadPolicy
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                         returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject    = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;

    returnStatus = pMyObject->RegGetStunInfo   ((ANSC_HANDLE)pMyObject);
    returnStatus = pMyObject->ApplyStunSettings((ANSC_HANDLE)pMyObject);

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoApplyStunSettings
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to apply configuration change.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoApplyStunSettings
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject           = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PCCSP_CWMP_STUN_MANAGER_PROPERTY    pProperty           = (PCCSP_CWMP_STUN_MANAGER_PROPERTY)&pMyObject->Property;
    PCCSP_CWMP_STUN_INFO                pCcspCwmpStunInfo   = (PCCSP_CWMP_STUN_INFO            )&pMyObject->CcspCwmpStunInfo;
    PSTUN_SIMPLE_CLIENT_OBJECT          pStunSimpleClient   = (PSTUN_SIMPLE_CLIENT_OBJECT )pMyObject->hStunSimpleClient;
    char*                               pAcsHostName        = (char*                      )NULL;
    ANSC_IPV4_ADDRESS                   client_ip4_addr;
    ANSC_IPV4_ADDRESS                   server_ip4_addr;

    if ( !pCcspCwmpStunInfo->STUNEnable )
    {
        return  ANSC_STATUS_SUCCESS;
    }

    client_ip4_addr.Value = pProperty->ClientAddr.Value;
    server_ip4_addr.Value = 0;

#if 0 /* we don't want socket bound on loopback interface */
    if ( client_ip4_addr.Value == 0 )
    {
        AnscGetLocalHostAddress(client_ip4_addr.Dot);
    }
#endif

    /*
     * The STUNServerAddress parameter specifies the host name or IP address of the STUN server for
     * the CPE to send Binding Requests if STUN is enabled via STUNEnable. If this parameter is an
     * empty string and STUNEnable is true, the CPE MUST use the address of the ACS extracted from
     * the host portion of the ACS URL.
     */
    if ( AnscSizeOfString(pCcspCwmpStunInfo->STUNServerAddress) > 0 )
    {
        AnscResolveHostName(pCcspCwmpStunInfo->STUNServerAddress, server_ip4_addr.Dot);
    }
    else
    {
        pAcsHostName = pMyObject->GetAcsHostName((ANSC_HANDLE)pMyObject);

        if ( !pAcsHostName )
        {
            return  ANSC_STATUS_SUCCESS;
        }

        AnscResolveHostName(pAcsHostName, server_ip4_addr.Dot);

        AnscFreeMemory(pAcsHostName);
    }

    pStunSimpleClient->Cancel                 ((ANSC_HANDLE)pStunSimpleClient);
    pStunSimpleClient->SetClientAddr          ((ANSC_HANDLE)pStunSimpleClient, client_ip4_addr.Value                    );
    pStunSimpleClient->SetClientPort          ((ANSC_HANDLE)pStunSimpleClient, pProperty->ClientPort                    );
    pStunSimpleClient->SetServerAddr          ((ANSC_HANDLE)pStunSimpleClient, server_ip4_addr.Value                    );
    pStunSimpleClient->SetServerPort          ((ANSC_HANDLE)pStunSimpleClient, pCcspCwmpStunInfo->STUNServerPort            );
    pStunSimpleClient->SetUsername            ((ANSC_HANDLE)pStunSimpleClient, pCcspCwmpStunInfo->STUNUsername              );
    pStunSimpleClient->SetPassword            ((ANSC_HANDLE)pStunSimpleClient, pCcspCwmpStunInfo->STUNPassword              );
    pStunSimpleClient->SetMinKeepAliveInterval((ANSC_HANDLE)pStunSimpleClient, pCcspCwmpStunInfo->STUNMinimumKeepAlivePeriod);
    pStunSimpleClient->SetMaxKeepAliveInterval((ANSC_HANDLE)pStunSimpleClient, pCcspCwmpStunInfo->STUNMaximumKeepAlivePeriod);
    pStunSimpleClient->Engage                 ((ANSC_HANDLE)pStunSimpleClient);

    if ( !pCcspCwmpStunInfo->STUNEnable )
    {
        /*
         * The STUN client MUST remain engaged even STUN is disabled.
         */
        return  ANSC_STATUS_SUCCESS;
    }

    pStunSimpleClient->PlayRole1((ANSC_HANDLE)pStunSimpleClient);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmpStunmoGetAcsHostName
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve the host name from the ACS
        URL.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     host name.

**********************************************************************/

char*
CcspCwmpStunmoGetAcsHostName
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject               = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT    pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT         pCcspCwmpProcessor      = (PCCSP_CWMP_PROCESSOR_OBJECT)pCcspCwmpCpeController->hCcspCwmpProcessor;
    char*                               pAcsUrl                 = (char*                      )pCcspCwmpProcessor->GetAcsUrl((ANSC_HANDLE)pCcspCwmpProcessor);
    char*                               pAcsHostName            = (char*                      )NULL;

    /*
     * URL for the CPE to connect to the ACS using the CPE WAN Management Protocol. In one of the
     * folowing forms:
     *
     *          x-dslf_cwmp://host:port/
     *          x-dslf_cwmps://host:port/
     *
     * The "s" suffix of the scheme component indicates the use of SSL. 
     */

    if ( pAcsUrl && AnscSizeOfString(pAcsUrl) != 0 )
    {
        char*                       pHostName       = NULL;
        char*                       pHostNameEnd    = NULL;
        int                         nHostNameLen    = 0;

        pHostName    = _ansc_strstr(pAcsUrl, "://");
        if ( pHostName )
        {
            pHostName   += 3;
            pHostNameEnd = _ansc_strchr(pHostName, ':');
            if ( !pHostNameEnd )
            {
                pHostNameEnd = _ansc_strchr(pHostName, '/');
            }
            if ( !pHostNameEnd )
            {
                pHostNameEnd = pHostName + AnscSizeOfString(pHostName);
            }

            nHostNameLen = pHostNameEnd - pHostName;

            pAcsHostName = (char*)AnscAllocateMemory(nHostNameLen + 1);
            if ( pAcsHostName )
            {
                AnscCopyMemory(pAcsHostName, pHostName, nHostNameLen);
                pAcsHostName[nHostNameLen] = 0;
            }
        }
    }

    return  pAcsHostName;
}

