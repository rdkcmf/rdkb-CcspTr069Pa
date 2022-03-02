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

    module:	ssp_web_acm.c

        For CCSP TR-069 PA SSP
        BroadWay Service Delivery System

    ---------------------------------------------------------------

    description:

        SSP implementation of the web ACM interface
        for CCSP TR-069 PA.

        *   ssp_BbhmInitWebAcmIf
        *   ssp_BbhmWebAcmIfGetCredential
    
    ---------------------------------------------------------------

    environment:

        Platform dependent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/03/2011  initial revision.

**********************************************************************/


#include "ssp_global.h"
#define MAX_CONN_SIZE 256

extern  WEB_ACM_INTERFACE                           webAcmIf;
extern  PCCSP_CWMP_CPE_CONTROLLER_OBJECT                 g_pCcspCwmpCpeController;


int  
ssp_BbhmInitWebAcmIf()
{

    webAcmIf.hOwnerContext  = (ANSC_HANDLE)NULL;
    webAcmIf.Size           = sizeof(WEB_ACM_INTERFACE);

    webAcmIf.GetCredential = ssp_BbhmWebAcmIfGetCredential;

    return 0;
}


ANSC_STATUS
ssp_BbhmWebAcmIfGetCredential
    (      
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pHostName,
        USHORT                      HostPort,
        PUCHAR                      pUriPath,
        PUCHAR                      pUserName,
        PUCHAR                      pRealm,
        PUCHAR*                     ppPassword
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    UNREFERENCED_PARAMETER(pHostName);
    UNREFERENCED_PARAMETER(HostPort);
    UNREFERENCED_PARAMETER(pUriPath);
    UNREFERENCED_PARAMETER(pRealm);
    ANSC_STATUS                     returnStatus     = ANSC_STATUS_SUCCESS;
    char*                           pConnReqUsername = NULL;  
    char*                           pConnReqPassword = NULL;
    errno_t rc       = -1;
    int     ind      = -1;

    pConnReqUsername = 
        CcspManagementServer_GetConnectionRequestUsername
            (
                g_pCcspCwmpCpeController->PANameWithPrefix
            );

    pConnReqPassword = 
        CcspManagementServer_GetConnectionRequestPassword
            (
                g_pCcspCwmpCpeController->PANameWithPrefix
            );

    /*
       pConnReqUsername or pUserName (Fixme: which one?) may be NULL when
       Device.ManagementServer.ConnectionRequestUsername is set to an empty
       string, so explicit NULL checks are required.
    */
    if (pConnReqUsername && pUserName)
    {
        rc = strcmp_s(pConnReqUsername,MAX_CONN_SIZE,(char*)pUserName,&ind);
        if((!ind) && (rc == EOK))
        {
            *ppPassword = (PUCHAR)AnscCloneString(pConnReqPassword);
        }
        else
        {
            returnStatus = ANSC_STATUS_FAILURE;
        }
    }
    else
    {
        returnStatus = ANSC_STATUS_FAILURE;
    }

    if ( pConnReqUsername )
    {
        AnscFreeMemory(pConnReqUsername);
    }

    if ( pConnReqPassword )
    {
        AnscFreeMemory(pConnReqPassword);
    }

    return  returnStatus;
}

