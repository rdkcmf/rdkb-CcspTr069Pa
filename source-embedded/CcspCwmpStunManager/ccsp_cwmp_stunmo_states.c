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

    module:	ccsp_cwmp_stunmo_states.c

        For CCSP CWMP protocol 

    ---------------------------------------------------------------

    description:

        This module implements the advanced state-access functions
        of the CCSP CWMP Stun Manager Object.

        *   CcspCwmpStunmoGetCcspCwmpCpeController
        *   CcspCwmpStunmoSetCcspCwmpCpeController
        *   CcspCwmpStunmoGetStunSimpleClient
        *   CcspCwmpStunmoGetClientAddr
        *   CcspCwmpStunmoSetClientAddr
        *   CcspCwmpStunmoGetProperty
        *   CcspCwmpStunmoSetProperty
        *   CcspCwmpStunmoResetProperty
        *   CcspCwmpStunmoReset

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

        ANSC_HANDLE
        CcspCwmpStunmoGetCcspCwmpCpeController
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
CcspCwmpStunmoGetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject    = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;

    return  pMyObject->hCcspCwmpCpeController;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoSetCcspCwmpCpeController
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCpeController
            );

    description:

        This function is called to configure object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hCpeController
                Specifies the object state to be configured.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoSetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCpeController
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject    = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;

    pMyObject->hCcspCwmpCpeController = hCpeController;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpStunmoGetStunSimpleClient
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
CcspCwmpStunmoGetStunSimpleClient
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject    = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;

    return  pMyObject->hStunSimpleClient;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmpStunmoGetClientAddr
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

ULONG
CcspCwmpStunmoGetClientAddr
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject    = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PCCSP_CWMP_STUN_MANAGER_PROPERTY    pProperty    = (PCCSP_CWMP_STUN_MANAGER_PROPERTY)&pMyObject->Property;

    return  pProperty->ClientAddr.Value;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoSetClientAddr
            (
                ANSC_HANDLE                 hThisObject,
                ULONG                       ulAddr
            );

    description:

        This function is called to configure object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ULONG                       ulAddr
                Specifies the object state to be configured.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoSetClientAddr
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulAddr
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject    = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PCCSP_CWMP_STUN_MANAGER_PROPERTY    pProperty    = (PCCSP_CWMP_STUN_MANAGER_PROPERTY)&pMyObject->Property;

    pProperty->ClientAddr.Value = ulAddr;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoGetProperty
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
CcspCwmpStunmoGetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject    = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PCCSP_CWMP_STUN_MANAGER_PROPERTY    pProperty    = (PCCSP_CWMP_STUN_MANAGER_PROPERTY)&pMyObject->Property;

    *(PCCSP_CWMP_STUN_MANAGER_PROPERTY)hProperty = *pProperty;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoSetProperty
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
CcspCwmpStunmoSetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject    = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PCCSP_CWMP_STUN_MANAGER_PROPERTY    pProperty    = (PCCSP_CWMP_STUN_MANAGER_PROPERTY)&pMyObject->Property;

    *pProperty = *(PCCSP_CWMP_STUN_MANAGER_PROPERTY)hProperty;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoResetProperty
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
CcspCwmpStunmoResetProperty
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject           = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PCCSP_CWMP_STUN_MANAGER_PROPERTY    pProperty           = (PCCSP_CWMP_STUN_MANAGER_PROPERTY)&pMyObject->Property;
    PCCSP_CWMP_STUN_INFO                pCcspCwmpStunInfo   = (PCCSP_CWMP_STUN_INFO            )&pMyObject->CcspCwmpStunInfo;

    AnscZeroMemory(pProperty, sizeof(CCSP_CWMP_STUN_MANAGER_PROPERTY));

    pProperty->ClientAddr.Value = 0;
    pProperty->ClientPort       = CWMP_PORT;

    AnscZeroMemory(pCcspCwmpStunInfo, sizeof(CCSP_CWMP_STUN_INFO));
    pCcspCwmpStunInfo->STUNServerPort = STUN_PORT;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoReset
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
CcspCwmpStunmoReset
    (
        ANSC_HANDLE                 hThisObject
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    return  ANSC_STATUS_SUCCESS;
}
