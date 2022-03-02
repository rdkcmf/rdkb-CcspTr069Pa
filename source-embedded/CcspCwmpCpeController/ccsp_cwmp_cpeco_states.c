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

    module: ccsp_cwmp_cpeco_states.c

        For CCSP CWMP implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced state-access functions
        of the CCSP CWMP Cpe Controller Object.

        *   CcspCwmpCpecoGetCcspCwmpAcsBroker
        *   CcspCwmpCpecoGetCcspCwmpProcessor
        *   CcspCwmpCpecoGetCcspCwmpSoapParser
        *   CcspCwmpCpecoGetCcspCwmpTcpConnReqHandler
        *   CcspCwmpCpecoGetRootObject
        *   CcspCwmpCpecoSetRootObject
        *   CcspCwmpCpecoGetProperty
        *   CcspCwmpCpecoSetProperty
        *   CcspCwmpCpecoResetProperty
        *   CcspCwmpCpecoReset
        *   CcspCwmpCpecoGetCcspCwmpCfgIf
        *   CcspCwmpCpecoSetCcspCwmpCfgIf

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/09/05    initial revision.
        06/20/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.

**********************************************************************/


#include "ccsp_cwmp_cpeco_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoGetCcspCwmpAcsBroker
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
CcspCwmpCpecoGetCcspCwmpAcsBroker
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject    = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return  pMyObject->hCcspCwmpAcsBroker;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoGetCcspCwmpProcessor
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
CcspCwmpCpecoGetCcspCwmpProcessor
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject    = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return  pMyObject->hCcspCwmpProcessor;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoGetCcspCwmpSoapParser
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
CcspCwmpCpecoGetCcspCwmpSoapParser
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject    = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return  pMyObject->hCcspCwmpSoapParser;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoGetCcspCwmpTcpConnReqHandler
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
CcspCwmpCpecoGetCcspCwmpTcpConnReqHandler
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;

    return  pMyObject->hCcspCwmpTcpConnReqHandler;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmpCpecoGetRootObject
            (
                ANSC_HANDLE                 hThisObject
            )

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

char*
CcspCwmpCpecoGetRootObject
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;

    return pMyObject->pRootObject;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetRootObject
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRootObject
            )   

    description:

        This function is called to set object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pRootObject
                Object state.

    return:     object state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSetRootObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRootObject
    )   
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;

    if ( pMyObject->pRootObject )
    {
        AnscFreeMemory(pMyObject->pRootObject);
        pMyObject->pRootObject = NULL;
    }

    if ( pRootObject )
    {
        pMyObject->pRootObject = AnscCloneString(pRootObject);
    }

    return ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoGetProperty
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
CcspCwmpCpecoGetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject    = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_PROPERTY   pProperty    = (PCCSP_CWMP_CPE_CONTROLLER_PROPERTY)&pMyObject->Property;

    *(PCCSP_CWMP_CPE_CONTROLLER_PROPERTY)hProperty = *pProperty;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetProperty
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
CcspCwmpCpecoSetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject    = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_PROPERTY   pProperty    = (PCCSP_CWMP_CPE_CONTROLLER_PROPERTY)&pMyObject->Property;

    *pProperty = *(PCCSP_CWMP_CPE_CONTROLLER_PROPERTY)hProperty;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoResetProperty
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
CcspCwmpCpecoResetProperty
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject    = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_PROPERTY   pProperty    = (PCCSP_CWMP_CPE_CONTROLLER_PROPERTY)&pMyObject->Property;

    AnscZeroMemory(pProperty, sizeof(CCSP_CWMP_CPE_CONTROLLER_PROPERTY));

    pProperty->bStartCwmpRightAway = FALSE;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoReset
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
CcspCwmpCpecoReset
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject    = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    pMyObject->bDelayedInformCancelled  = FALSE;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoGetCcspCwmpCfgIf
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
CcspCwmpCpecoGetCcspCwmpCfgIf
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject    = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;

    return  pMyObject->hCcspCwmpCfgIf;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetCcspCwmpCfgIf
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hInterface
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hInterface
                Object state to be set.

    return:     object state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSetCcspCwmpCfgIf
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hInterface
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject    = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;

    pMyObject->hCcspCwmpCfgIf = hInterface;

    return  returnStatus;
}


