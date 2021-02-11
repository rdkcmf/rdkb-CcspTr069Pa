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

    module:	ccsp_cwmp_acsbo_operation.c

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This module implements the advanced operation functions
        of the CCSP CWMP Acs Broker Object.

        *   CcspCwmpAcsboEngage
        *   CcspCwmpAcsboCancel

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/30/05    initial revision.
        06/20/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.

**********************************************************************/


#include "ccsp_cwmp_acsbo_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboEngage
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to engage the object activity.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcsboEngage
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                      returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_BROKER_OBJECT     pMyObject          = (PCCSP_CWMP_ACS_BROKER_OBJECT    )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;

    if ( pMyObject->bActive )
    {
        return  ANSC_STATUS_SUCCESS;
    }
    else if ( !pCcspCwmpCpeController )
    {
        return  ANSC_STATUS_NOT_READY;
    }

    pMyObject->bActive = TRUE;

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboCancel
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to cancel the object activity.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcsboCancel
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                      returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_BROKER_OBJECT     pMyObject          = (PCCSP_CWMP_ACS_BROKER_OBJECT    )hThisObject;

    if ( !pMyObject->bActive )
    {
        return  ANSC_STATUS_SUCCESS;
    }
    else
    {
        pMyObject->bActive = FALSE;
    }

    return  returnStatus;
}


