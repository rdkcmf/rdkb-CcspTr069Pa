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

    module:	ccsp_cwmp_acsbo_states.c

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This module implements the advanced state-access functions
        of the CCSP CWMP Acs Broker Object.

        *   CcspCwmpAcsboGetCcspCwmpMsoIf
        *   CcspCwmpAcsboGetCcspCwmpCpeController
        *   CcspCwmpAcsboSetCcspCwmpCpeController
        *   CcspCwmpAcsboGetProperty
        *   CcspCwmpAcsboSetProperty
        *   CcspCwmpAcsboResetProperty
        *   CcspCwmpAcsboReset

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang

    ---------------------------------------------------------------

    revision:

        09/30/05    initial revision.

**********************************************************************/


#include "ccsp_cwmp_acsbo_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpAcsboGetCcspCwmpMsoIf
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
CcspCwmpAcsboGetCcspCwmpMsoIf
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_BROKER_OBJECT         pMyObject    = (PCCSP_CWMP_ACS_BROKER_OBJECT  )hThisObject;

    return  pMyObject->hCcspCwmpMsoIf;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpAcsboGetCcspCwmpCpeController
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
CcspCwmpAcsboGetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_BROKER_OBJECT         pMyObject    = (PCCSP_CWMP_ACS_BROKER_OBJECT  )hThisObject;

    return  pMyObject->hCcspCwmpCpeController;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboSetCcspCwmpCpeController
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
CcspCwmpAcsboSetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCpeController
    )
{
    PCCSP_CWMP_ACS_BROKER_OBJECT         pMyObject    = (PCCSP_CWMP_ACS_BROKER_OBJECT  )hThisObject;

    pMyObject->hCcspCwmpCpeController = hCpeController;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboGetProperty
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
CcspCwmpAcsboGetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    PCCSP_CWMP_ACS_BROKER_OBJECT         pMyObject    = (PCCSP_CWMP_ACS_BROKER_OBJECT  )hThisObject;
    PCCSP_CWMP_ACS_BROKER_PROPERTY       pProperty    = (PCCSP_CWMP_ACS_BROKER_PROPERTY)&pMyObject->Property;

    *(PCCSP_CWMP_ACS_BROKER_PROPERTY)hProperty = *pProperty;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboSetProperty
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
CcspCwmpAcsboSetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    PCCSP_CWMP_ACS_BROKER_OBJECT         pMyObject    = (PCCSP_CWMP_ACS_BROKER_OBJECT  )hThisObject;
    PCCSP_CWMP_ACS_BROKER_PROPERTY       pProperty    = (PCCSP_CWMP_ACS_BROKER_PROPERTY)&pMyObject->Property;

    *pProperty = *(PCCSP_CWMP_ACS_BROKER_PROPERTY)hProperty;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboResetProperty
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
CcspCwmpAcsboResetProperty
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_BROKER_OBJECT         pMyObject    = (PCCSP_CWMP_ACS_BROKER_OBJECT  )hThisObject;
    PCCSP_CWMP_ACS_BROKER_PROPERTY       pProperty    = (PCCSP_CWMP_ACS_BROKER_PROPERTY)&pMyObject->Property;

    AnscZeroMemory(pProperty, sizeof(CCSP_CWMP_ACS_BROKER_PROPERTY));

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboReset
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
CcspCwmpAcsboReset
    (
        ANSC_HANDLE                 hThisObject
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    return  ANSC_STATUS_SUCCESS;
}
