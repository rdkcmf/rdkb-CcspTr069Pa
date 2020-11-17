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

    module:	ccsp_cwmp_stunmo_operation.c

        For CCSP CWMP protocol

    ---------------------------------------------------------------

    description:

        This module implements the advanced operation functions
        of the CCSP CWMP Stun Manager Object.

        *   CcspCwmpStunmoEngage
        *   CcspCwmpStunmoCancel
        *   CcspCwmpStunmoSetupEnv
        *   CcspCwmpStunmoCloseEnv

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
        CcspCwmpStunmoEngage
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
CcspCwmpStunmoEngage
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                         returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject               = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PSTUN_BSM_INTERFACE                 pStunBsmIf              = (PSTUN_BSM_INTERFACE        )pMyObject->hStunBsmIf;
    PSTUN_SIMPLE_CLIENT_OBJECT          pStunSimpleClient       = (PSTUN_SIMPLE_CLIENT_OBJECT )pMyObject->hStunSimpleClient;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT    pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;

    if ( pMyObject->bActive )
    {
        return  ANSC_STATUS_SUCCESS;
    }
    else if ( !pCcspCwmpCpeController )
    {
        return  ANSC_STATUS_NOT_READY;
    }

    pStunSimpleClient->SetStunBsmIf((ANSC_HANDLE)pStunSimpleClient, (ANSC_HANDLE)pStunBsmIf);

    returnStatus = pMyObject->SetupEnv((ANSC_HANDLE)pMyObject);

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        return  returnStatus;
    }

    pMyObject->bActive = TRUE;

    returnStatus = pMyObject->LoadPolicy((ANSC_HANDLE)pMyObject);

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoCancel
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
CcspCwmpStunmoCancel
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                         returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_STUN_MANAGER_OBJECT      pMyObject          = (PCCSP_CWMP_STUN_MANAGER_OBJECT  )hThisObject;
    PSTUN_SIMPLE_CLIENT_OBJECT          pStunSimpleClient  = (PSTUN_SIMPLE_CLIENT_OBJECT )pMyObject->hStunSimpleClient;
    ULONG                               ulMaxWaitTimes     = 10;
    ULONG                               ulWaitTimes        = 0;

    if ( !pMyObject->bActive )
    {
        return  ANSC_STATUS_SUCCESS;
    }
    else
    {
        pMyObject->bActive = FALSE;
    }

    while ( pMyObject->bRetryTaskRunning && ulWaitTimes <= ulMaxWaitTimes )
    {
        AnscSleep(1000);
        ulWaitTimes ++;
    }

    pStunSimpleClient->Cancel((ANSC_HANDLE)pStunSimpleClient);

    pMyObject->Reset((ANSC_HANDLE)pMyObject);

    returnStatus = pMyObject->CloseEnv((ANSC_HANDLE)pMyObject);

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoSetupEnv
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to setup the operating environment.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoSetupEnv
    (
        ANSC_HANDLE                 hThisObject
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    ANSC_STATUS                         returnStatus           = ANSC_STATUS_SUCCESS;
    
    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoCloseEnv
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to close the operating environment.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoCloseEnv
    (
        ANSC_HANDLE                 hThisObject
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    ANSC_STATUS                         returnStatus           = ANSC_STATUS_SUCCESS;

    return  returnStatus;
}
