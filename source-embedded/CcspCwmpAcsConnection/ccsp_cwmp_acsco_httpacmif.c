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

    module: ccsp_cwmp_acsco_httpacm.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced interface functions
        of the CCSP CWMP ACS Connection Object.

        *   CcspCwmpAcscoHttpGetCredential

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        11/12/05    initial revision.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_acsco_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

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

    description:

        This function is called to return the http authentication information
        before sending out.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PUCHAR                      pHostName,
                The input host name;

                USHORT                      HostPort,
                The input host port;

                PUCHAR                      pUriPath,
                The input url path;

                PUCHAR*                     ppUserName,
                The output user name informaiton

                PUCHAR*                     ppPassword
                The output password information

    return:     status of operation.

**********************************************************************/
ANSC_STATUS
CcspCwmpAcscoHttpGetCredential
    (      
        ANSC_HANDLE                 hThisObject,
        PUCHAR                      pHostName,
        USHORT                      HostPort,
        PUCHAR                      pUriPath,
        PUCHAR*                     ppUserName,
        PUCHAR*                     ppPassword
    )
{
    UNREFERENCED_PARAMETER(pHostName);
    UNREFERENCED_PARAMETER(pUriPath);
    UNREFERENCED_PARAMETER(HostPort);
    ANSC_STATUS                      returnStatus        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pMyObject           = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT       )pMyObject->hCcspCwmpSession;

    if ( pCcspCwmpSession )
    {
        PCCSP_CWMP_PROCESSOR_OBJECT  pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT)pCcspCwmpSession->GetCcspCwmpProcessor((ANSC_HANDLE)pCcspCwmpSession);
        char*                        pAcsUsername           =  pCcspCwmpProcessor->GetAcsUsername((ANSC_HANDLE)pCcspCwmpProcessor);
        char*                        pAcsPassword           =  pCcspCwmpProcessor->GetAcsPassword((ANSC_HANDLE)pCcspCwmpProcessor);

        *ppUserName = (PUCHAR)pAcsUsername;
        *ppPassword = (PUCHAR)pAcsPassword;
    }
    else
    {
        *ppUserName = (PUCHAR)AnscCloneString(pMyObject->Username);
        *ppPassword = (PUCHAR)AnscCloneString(pMyObject->Password);
    }

    return returnStatus;
}
