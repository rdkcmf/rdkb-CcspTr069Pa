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

    module:	ccsp_cwmp_stunmo_base.c

        For CCSP CWMP protocol 

    ---------------------------------------------------------------

    description:

        This module implements the basic container object functions
        of the CCSP CWMP Stun Manager Object.

        *   CcspCwmpStunmoCreate
        *   CcspCwmpStunmoRemove
        *   CcspCwmpStunmoEnrollObjects
        *   CcspCwmpStunmoInitialize

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

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CcspCwmpStunmoCreate
            (
                ANSC_HANDLE                 hContainerContext,
                ANSC_HANDLE                 hOwnerContext,
                ANSC_HANDLE                 hAnscReserved
            );

    description:

        This function constructs the CCSP CWMP Stun Manager Object and
        initializes the member variables and functions.

    argument:   ANSC_HANDLE                 hContainerContext
                This handle is used by the container object to interact
                with the outside world. It could be the real container
                or an target object.

                ANSC_HANDLE                 hOwnerContext
                This handle is passed in by the owner of this object.

                ANSC_HANDLE                 hAnscReserved
                This handle is passed in by the owner of this object.

    return:     newly created container object.

**********************************************************************/

ANSC_HANDLE
CcspCwmpStunmoCreate
    (
        ANSC_HANDLE                 hContainerContext,
        ANSC_HANDLE                 hOwnerContext,
        ANSC_HANDLE                 hAnscReserved
    )
{
    UNREFERENCED_PARAMETER(hAnscReserved);
    PANSC_COMPONENT_OBJECT          pBaseObject  = NULL;
    PCCSP_CWMP_STUN_MANAGER_OBJECT  pMyObject    = NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCCSP_CWMP_STUN_MANAGER_OBJECT)AnscAllocateMemory(sizeof(CCSP_CWMP_STUN_MANAGER_OBJECT));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }
    else
    {
        pBaseObject = (PANSC_COMPONENT_OBJECT)pMyObject;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    /* AnscCopyString(pBaseObject->Name, CCSP_CWMP_STUN_MANAGER_NAME); */

    pBaseObject->hContainerContext = hContainerContext;
    pBaseObject->hOwnerContext     = hOwnerContext;
    pBaseObject->Oid               = CCSP_CWMP_STUN_MANAGER_OID;
    pBaseObject->Create            = CcspCwmpStunmoCreate;
    pBaseObject->Remove            = CcspCwmpStunmoRemove;
    pBaseObject->EnrollObjects     = CcspCwmpStunmoEnrollObjects;
    pBaseObject->Initialize        = CcspCwmpStunmoInitialize;

    pBaseObject->EnrollObjects((ANSC_HANDLE)pBaseObject);
    pBaseObject->Initialize   ((ANSC_HANDLE)pBaseObject);

    return  (ANSC_HANDLE)pMyObject;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function destroys the object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT  pMyObject         = (PCCSP_CWMP_STUN_MANAGER_OBJECT )hThisObject;
    PSTUN_BSM_INTERFACE             pStunBsmIf        = (PSTUN_BSM_INTERFACE       )pMyObject->hStunBsmIf;
    PSTUN_SIMPLE_CLIENT_OBJECT      pStunSimpleClient = (PSTUN_SIMPLE_CLIENT_OBJECT)pMyObject->hStunSimpleClient;

    pMyObject->Cancel((ANSC_HANDLE)pMyObject);
    pMyObject->Reset ((ANSC_HANDLE)pMyObject);

    if ( pStunBsmIf )
    {
        AnscFreeMemory(pStunBsmIf);

        pMyObject->hStunBsmIf = (ANSC_HANDLE)NULL;
    }

    if ( pStunSimpleClient )
    {
        pStunSimpleClient->Remove((ANSC_HANDLE)pStunSimpleClient);

        pMyObject->hStunSimpleClient = (ANSC_HANDLE)NULL;
    }

    AnscCoRemove((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoEnrollObjects
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function enrolls all the objects required by this object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoEnrollObjects
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT  pMyObject         = (PCCSP_CWMP_STUN_MANAGER_OBJECT )hThisObject;
    PSTUN_BSM_INTERFACE             pStunBsmIf        = (PSTUN_BSM_INTERFACE       )pMyObject->hStunBsmIf;
    PSTUN_SIMPLE_CLIENT_OBJECT      pStunSimpleClient = (PSTUN_SIMPLE_CLIENT_OBJECT)pMyObject->hStunSimpleClient;
    errno_t rc = -1;

    if ( !pStunBsmIf )
    {
        pStunBsmIf = (PSTUN_BSM_INTERFACE)AnscAllocateMemory(sizeof(STUN_BSM_INTERFACE));

        if ( !pStunBsmIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hStunBsmIf = (ANSC_HANDLE)pStunBsmIf;
        }
        rc = strcpy_s(pStunBsmIf->Name,sizeof(pStunBsmIf->Name), STUN_BSM_INTERFACE_NAME);
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return ANSC_STATUS_FAILURE;
        }
        pStunBsmIf->InterfaceId   = STUN_BSM_INTERFACE_ID;
        pStunBsmIf->hOwnerContext = (ANSC_HANDLE)pMyObject;
        pStunBsmIf->Size          = sizeof(STUN_BSM_INTERFACE);

        pStunBsmIf->RecvMsg1      = CcspCwmpStunmoStunBsmRecvMsg1;
        pStunBsmIf->RecvMsg2      = CcspCwmpStunmoStunBsmRecvMsg2;
        pStunBsmIf->Notify        = CcspCwmpStunmoStunBsmNotify;
    }

    if ( !pStunSimpleClient )
    {
        pStunSimpleClient =
            (PSTUN_SIMPLE_CLIENT_OBJECT)StunCreateSimpleClient
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pStunSimpleClient )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hStunSimpleClient = (ANSC_HANDLE)pStunSimpleClient;

            pStunSimpleClient->SetStunBsmIf((ANSC_HANDLE)pStunSimpleClient, (ANSC_HANDLE)pStunBsmIf);
        }
    }

    AnscCoEnrollObjects((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpStunmoInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function first calls the initialization member function
        of the base class object to set the common member fields
        inherited from the base class. It then initializes the member
        fields that are specific to this object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpStunmoInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_STUN_MANAGER_OBJECT  pMyObject    = (PCCSP_CWMP_STUN_MANAGER_OBJECT)hThisObject;

    /*
     * Until you have to simulate C++ object-oriented programming style with standard C, you don't
     * appreciate all the nice little things come with C++ language and all the dirty works that
     * have been done by the C++ compilers. Member initialization is one of these things. While in
     * C++ you don't have to initialize all the member fields inherited from the base class since
     * the compiler will do it for you, such is not the case with C.
     */
    AnscCoInitialize((ANSC_HANDLE)pMyObject);

    /*
     * Although we have initialized some of the member fields in the "create" member function, we
     * repeat the work here for completeness. While this simulation approach is pretty stupid from
     * a C++/Java programmer perspective, it's the best we can get for universal embedded network
     * programming. Before we develop our own operating system (don't expect that to happen any
     * time soon), this is the way things gonna be.
     */
    pMyObject->Oid                    = CCSP_CWMP_STUN_MANAGER_OID;
    pMyObject->Create                 = CcspCwmpStunmoCreate;
    pMyObject->Remove                 = CcspCwmpStunmoRemove;
    pMyObject->EnrollObjects          = CcspCwmpStunmoEnrollObjects;
    pMyObject->Initialize             = CcspCwmpStunmoInitialize;

    pMyObject->hCcspCwmpCpeController = (ANSC_HANDLE)NULL;
    pMyObject->bActive                = FALSE;

    pMyObject->LastUdpConnReqID       = 0;
    pMyObject->LastUdpConnReqTS       = 0;
    pMyObject->bRetryTaskRunning      = FALSE;

    pMyObject->GetCcspCwmpCpeController   = CcspCwmpStunmoGetCcspCwmpCpeController;
    pMyObject->SetCcspCwmpCpeController   = CcspCwmpStunmoSetCcspCwmpCpeController;
    pMyObject->GetStunSimpleClient    = CcspCwmpStunmoGetStunSimpleClient;
    pMyObject->GetClientAddr          = CcspCwmpStunmoGetClientAddr;
    pMyObject->SetClientAddr          = CcspCwmpStunmoSetClientAddr;
    pMyObject->GetProperty            = CcspCwmpStunmoGetProperty;
    pMyObject->SetProperty            = CcspCwmpStunmoSetProperty;
    pMyObject->ResetProperty          = CcspCwmpStunmoResetProperty;
    pMyObject->Reset                  = CcspCwmpStunmoReset;

    pMyObject->Engage                 = CcspCwmpStunmoEngage;
    pMyObject->Cancel                 = CcspCwmpStunmoCancel;
    pMyObject->SetupEnv               = CcspCwmpStunmoSetupEnv;
    pMyObject->CloseEnv               = CcspCwmpStunmoCloseEnv;

    pMyObject->LoadPolicy             = CcspCwmpStunmoLoadPolicy;
    pMyObject->ApplyStunSettings      = CcspCwmpStunmoApplyStunSettings;
    pMyObject->GetAcsHostName         = CcspCwmpStunmoGetAcsHostName;

    pMyObject->RegGetStunInfo         = CcspCwmpStunmoRegGetStunInfo;
    pMyObject->RegSetStunInfo         = CcspCwmpStunmoRegSetStunInfo;

    pMyObject->StunBsmRecvMsg1        = CcspCwmpStunmoStunBsmRecvMsg1;
    pMyObject->StunBsmRecvMsg2        = CcspCwmpStunmoStunBsmRecvMsg2;
    pMyObject->StunBsmNotify          = CcspCwmpStunmoStunBsmNotify;

    /*
     * We shall initialize the configuration properties to the default values, which may be changed
     * later via the set_property() member function. Note that this call may not guarantee a valid
     * and legtimate configuration.
     */
    pMyObject->ResetProperty((ANSC_HANDLE)pMyObject);

    CcspCwmpInitStunInfo((&pMyObject->CcspCwmpStunInfo));

    return  ANSC_STATUS_SUCCESS;
}
