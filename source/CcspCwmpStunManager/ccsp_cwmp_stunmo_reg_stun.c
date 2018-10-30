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

    module:	ccsp_cwmp_stunmo_reg_stun.c

        For CCSP CWMP protocol 

    ---------------------------------------------------------------

    description:

        This module implements the advanced registry-access
        functions of the CCSP CWMP Stun Manager Object.

        *   CcspCwmpStunmoRegGetStunInfo
        *   CcspCwmpStunmoRegSetStunInfo

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
        CcspCwmpStunmoRegGetStunInfo
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve STUN configuration from
        system registry.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoRegGetStunInfo
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                         returnStatus           = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject              = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT    pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_STUN_INFO                pCcspCwmpStunInfo      = (PCCSP_CWMP_STUN_INFO            )&pMyObject->CcspCwmpStunInfo;
    CCSP_STRING                         pValue;

    /* retrieve STUN settings from MS FC */
    pCcspCwmpStunInfo->UDPConnectionRequestAddressNotificationLimit = 0;

    pCcspCwmpStunInfo->STUNEnable = 
        CcspManagementServer_GetSTUNEnable
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );
    
    pValue = 
        CcspManagementServer_GetSTUNServerAddress
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    AnscZeroMemory(pCcspCwmpStunInfo->STUNServerAddress, 257);
    if ( pValue )
    {
        _ansc_strncpy(pCcspCwmpStunInfo->STUNServerAddress, pValue, 256);
        CcspTr069PaFreeMemory(pValue);
    }

    pCcspCwmpStunInfo->STUNServerPort = 
        (USHORT)CcspManagementServer_GetSTUNServerPort
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    pValue =
        CcspManagementServer_GetSTUNUsername
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    AnscZeroMemory(pCcspCwmpStunInfo->STUNUsername, 257);
    if ( pValue )
    {
        _ansc_strncpy(pCcspCwmpStunInfo->STUNUsername, pValue, 256);
        CcspTr069PaFreeMemory(pValue);
    }

    pValue =
        CcspManagementServer_GetSTUNPassword
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    AnscZeroMemory(pCcspCwmpStunInfo->STUNPassword, 257);
    if ( pValue )
    {
        _ansc_strncpy(pCcspCwmpStunInfo->STUNPassword, pValue, 256);
        CcspTr069PaFreeMemory(pValue);
    }

    pCcspCwmpStunInfo->STUNMaximumKeepAlivePeriod =
        CcspManagementServer_GetSTUNMaximumKeepAlivePeriod
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    pCcspCwmpStunInfo->STUNMinimumKeepAlivePeriod = 
        CcspManagementServer_GetSTUNMinimumKeepAlivePeriod
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    pCcspCwmpStunInfo->NATDetected =
        CcspManagementServer_GetNATDetected
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoRegSetStunInfo
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to write STUN configuration into
        system registry.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoRegSetStunInfo
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                         returnStatus           = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject              = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT    pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_STUN_INFO                pCcspCwmpStunInfo      = (PCCSP_CWMP_STUN_INFO            )&pMyObject->CcspCwmpStunInfo;

#if 0
    /* save STUN settings into MS FC */
    CcspManagementServer_SetSTUNEnable
        (
            pCcspCwmpCpeController->PANameWithPrefix,
            pCcspCwmpStunInfo->STUNEnable
        );

    CcspManagementServer_SetSTUNServerAddress
        (   
            pCcspCwmpCpeController->PANameWithPrefix,
            pCcspCwmpStunInfo->STUNServerAddress
        );  

    CcspManagementServer_SetSTUNServerPort
        (   
            pCcspCwmpCpeController->PANameWithPrefix,
            pCcspCwmpStunInfo->STUNServerPort
        );  

    CcspManagementServer_SetSTUNUsername
        (   
            pCcspCwmpCpeController->PANameWithPrefix,
            pCcspCwmpStunInfo->STUNUsername
        );  

    CcspManagementServer_SetSTUNPassword
        (   
            pCcspCwmpCpeController->PANameWithPrefix,
            pCcspCwmpStunInfo->STUNPassword
        );  

    CcspManagementServer_SetSTUNMaximumKeepAlivePeriod
        (   
            pCcspCwmpCpeController->PANameWithPrefix,
            pCcspCwmpStunInfo->STUNMaximumKeepAlivePeriod
        );  

    CcspManagementServer_SetSTUNMinimumKeepAlivePeriod
        (   
            pCcspCwmpCpeController->PANameWithPrefix,
            pCcspCwmpStunInfo->STUNMinimumKeepAlivePeriod
        );  

    CcspManagementServer_SetNATDetected
        (   
            pCcspCwmpCpeController->PANameWithPrefix,
            pCcspCwmpStunInfo->NATDetected
        );  
#endif

    return  returnStatus;
}
