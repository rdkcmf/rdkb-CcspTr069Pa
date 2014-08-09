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

    module: ccsp_cwmp_acsco_states.c

        For CCSP CWMP protocol implementation
        BroadWay Service Delivery System

    ---------------------------------------------------------------

    copyright:

        Cisco Systems, Inc., 1997 ~ 2011
        All Rights Reserved

    ---------------------------------------------------------------

    description:

        This module implements the advanced state-access functions
        of the CCSP CWMP ACS Connection Object.

            *   CcspCwmpAcscoGetHttpSimpleClient
            *   CcspCwmpAcscoGetCcspCwmpSession
            *   CcspCwmpAcscoSetCcspCwmpSession
            *   CcspCwmpAcscoGetAcsUrl
            *   CcspCwmpAcscoSetAcsUrl
            *   CcspCwmpAcscoGetUsername
            *   CcspCwmpAcscoSetUsername
            *   CcspCwmpAcscoGetPassword
            *   CcspCwmpAcscoSetPassword
            *   CcspCwmpAcscoReset

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
        10/13/11    resolve name conflict with DM library.

**********************************************************************/


#include "ccsp_cwmp_acsco_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpAcscoGetHttpSimpleClient
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to get the Http simple client;

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The HttpSimpleClient handle;

**********************************************************************/
ANSC_HANDLE
CcspCwmpAcscoGetHttpSimpleClient
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;

    return pMyObject->hHttpSimpleClient;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpAcscoGetCcspCwmpSession
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to get the Wmp Session;

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The WmpSession handle;

**********************************************************************/
ANSC_HANDLE
CcspCwmpAcscoGetCcspCwmpSession
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;

    return pMyObject->hCcspCwmpSession;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoSetCcspCwmpSession
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hHandle
            );

    description:

        This function is called to set the Wmp Session;

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hHandle
                The new CcspCwmpSession handle;

    return:     The status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpAcscoSetCcspCwmpSession
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hHandle
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    
	pMyObject->hCcspCwmpSession = hHandle;

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpAcscoGetAcsUrl
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to get the Acs Url string;

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The Acs Url string;

**********************************************************************/
char*
CcspCwmpAcscoGetAcsUrl
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;

    return pMyObject->AcsUrl;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoSetAcsUrl
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pName
            );

    description:

        This function is called to set the Acs Url string;

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pName
                The new string;

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpAcscoSetAcsUrl
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;

    if(pMyObject->AcsUrl != NULL)
    {
        CcspTr069PaFreeMemory(pMyObject->AcsUrl);

        pMyObject->AcsUrl = NULL;
    }

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

    if( _ansc_strstr(pName, ACS_DSLF_CWMPS) == pName)
    {
        pMyObject->AcsUrl = (PCHAR)CcspTr069PaAllocateMemory(AnscSizeOfString(pName) + 8);

        if( pMyObject->AcsUrl == NULL)
        {
            return ANSC_STATUS_RESOURCES;
        }

        AnscCopyString(pMyObject->AcsUrl, "https");

        AnscCopyMemory
            (
                (PCHAR)(pMyObject->AcsUrl + AnscSizeOfString("https")),
                (PCHAR)(pName + AnscSizeOfString(ACS_DSLF_CWMPS)),
                AnscSizeOfString(pName) - AnscSizeOfString(ACS_DSLF_CWMPS)
            );
    }
    else if( _ansc_strstr(pName, ACS_DSLF_CWMP) == pName)
    {
        pMyObject->AcsUrl = (PCHAR)CcspTr069PaAllocateMemory(AnscSizeOfString(pName) + 8);

        if( pMyObject->AcsUrl == NULL)
        {
            return ANSC_STATUS_RESOURCES;
        }

        AnscCopyString(pMyObject->AcsUrl, "http");

        AnscCopyMemory
            (
                (PCHAR)(pMyObject->AcsUrl + AnscSizeOfString("http")),
                (PCHAR)(pName + AnscSizeOfString(ACS_DSLF_CWMP)),
                AnscSizeOfString(pName) - AnscSizeOfString(ACS_DSLF_CWMP)
            );
    }
    else
    {
        pMyObject->AcsUrl = CcspTr069PaCloneString(pName);
    }

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpAcscoGetUsername
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to get the Username;

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The Username;

**********************************************************************/
char*
CcspCwmpAcscoGetUsername
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;

    return pMyObject->Username;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoSetUsername
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pName
            );

    description:

        This function is called to set the Username;

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pName
                The new string;

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpAcscoSetUsername
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PHTTP_SIMPLE_CLIENT_OBJECT      pHttpClient     = (PHTTP_SIMPLE_CLIENT_OBJECT )pMyObject->hHttpSimpleClient;

    if ( pName && pMyObject->Username )
    {
        if ( AnscEqualString(pName, pMyObject->Username, TRUE ) )
        {
            return ANSC_STATUS_SUCCESS;
        }
    }
    else if ( !pName && !pMyObject->Username )
    {
        return ANSC_STATUS_SUCCESS;
    }

    if(pMyObject->Username != NULL)
    {
        CcspTr069PaFreeMemory(pMyObject->Username);
        pMyObject->Username = NULL;
    }

    if( pName != NULL)
    {
        pMyObject->Username = CcspTr069PaCloneString(pName);
    }

    if ( pHttpClient )
    {
        PHTTP_CAS_INTERFACE         pHttpCasIf      = (PHTTP_CAS_INTERFACE)NULL;

        pHttpCasIf  = (PHTTP_CAS_INTERFACE)pHttpClient->GetCasIf((ANSC_HANDLE)pHttpClient);
        pHttpCasIf->ClearAuthInfo(pHttpCasIf->hOwnerContext);
    }

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpAcscoGetPassword
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to get the Password;

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The Password;

**********************************************************************/
char*
CcspCwmpAcscoGetPassword
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;

    return pMyObject->Password;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoSetPassword
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pPassword
            );

    description:

        This function is called to set the Password;

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pPassword
                The new string;

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpAcscoSetPassword
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pPassword
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PHTTP_SIMPLE_CLIENT_OBJECT      pHttpClient     = (PHTTP_SIMPLE_CLIENT_OBJECT )pMyObject->hHttpSimpleClient;

    if ( pPassword && pMyObject->Password )
    {
        if ( AnscEqualString(pPassword, pMyObject->Password, TRUE ) )
        {
            return ANSC_STATUS_SUCCESS;
        }
    }
    else if ( !pPassword && !pMyObject->Password )
    {
        return ANSC_STATUS_SUCCESS;
    }

    if(pMyObject->Password != NULL)
    {
        CcspTr069PaFreeMemory(pMyObject->Password);
        pMyObject->Password = NULL;
    }

    if( pPassword != NULL)
    {
        pMyObject->Password = CcspTr069PaCloneString(pPassword);
    }

    if ( pHttpClient )
    {
        PHTTP_CAS_INTERFACE         pHttpCasIf      = (PHTTP_CAS_INTERFACE)NULL;

        pHttpCasIf  = (PHTTP_CAS_INTERFACE)pHttpClient->GetCasIf((ANSC_HANDLE)pHttpClient);
        pHttpCasIf->ClearAuthInfo(pHttpCasIf->hOwnerContext);
    }

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoReset
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
CcspCwmpAcscoReset
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;

    return  ANSC_STATUS_SUCCESS;
}
