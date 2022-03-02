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

    module:	ccsp_cwmp_acsbo_base.c

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This module implements the basic container object functions
        of the CCSP CWMP Acs Broker Object.

        *   CcspCwmpAcsboCreate
        *   CcspCwmpAcsboRemove
        *   CcspCwmpAcsboEnrollObjects
        *   CcspCwmpAcsboInitialize

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/29/05    initial revision.
        06/20/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.

**********************************************************************/


#include "ccsp_cwmp_acsbo_global.h"


/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CcspCwmpAcsboCreate
            (
                ANSC_HANDLE                 hContainerContext,
                ANSC_HANDLE                 hOwnerContext,
                ANSC_HANDLE                 hAnscReserved
            );

    description:

        This function constructs the CCSP CWMP Acs Broker Object and
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
CcspCwmpAcsboCreate
    (
        ANSC_HANDLE                 hContainerContext,
        ANSC_HANDLE                 hOwnerContext,
        ANSC_HANDLE                 hAnscReserved
    )
{
    UNREFERENCED_PARAMETER(hAnscReserved);
    PANSC_COMPONENT_OBJECT          pBaseObject  = NULL;
    PCCSP_CWMP_ACS_BROKER_OBJECT    pMyObject    = NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCCSP_CWMP_ACS_BROKER_OBJECT)AnscAllocateMemory(sizeof(CCSP_CWMP_ACS_BROKER_OBJECT));

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
    /* AnscCopyString(pBaseObject->Name, CCSP_CWMP_ACS_BROKER_NAME); */

    pBaseObject->hContainerContext = hContainerContext;
    pBaseObject->hOwnerContext     = hOwnerContext;
    pBaseObject->Oid               = CCSP_CWMP_ACS_BROKER_OID;
    pBaseObject->Create            = CcspCwmpAcsboCreate;
    pBaseObject->Remove            = CcspCwmpAcsboRemove;
    pBaseObject->EnrollObjects     = CcspCwmpAcsboEnrollObjects;
    pBaseObject->Initialize        = CcspCwmpAcsboInitialize;

    pBaseObject->EnrollObjects((ANSC_HANDLE)pBaseObject);
    pBaseObject->Initialize   ((ANSC_HANDLE)pBaseObject);

    return  (ANSC_HANDLE)pMyObject;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboRemove
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
CcspCwmpAcsboRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_BROKER_OBJECT    pMyObject       = (PCCSP_CWMP_ACS_BROKER_OBJECT)hThisObject;
    PCCSP_CWMP_MSO_INTERFACE        pCcspCwmpMsoIf  = (PCCSP_CWMP_MSO_INTERFACE    )pMyObject->hCcspCwmpMsoIf;

    pMyObject->Cancel((ANSC_HANDLE)pMyObject);
    pMyObject->Reset ((ANSC_HANDLE)pMyObject);

    if ( pCcspCwmpMsoIf )
    {
        AnscFreeMemory(pCcspCwmpMsoIf);

        pMyObject->hCcspCwmpMsoIf = (ANSC_HANDLE)NULL;
    }

    AnscCoRemove((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboEnrollObjects
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
CcspCwmpAcsboEnrollObjects
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_BROKER_OBJECT    pMyObject       = (PCCSP_CWMP_ACS_BROKER_OBJECT)hThisObject;
    PCCSP_CWMP_MSO_INTERFACE        pCcspCwmpMsoIf  = (PCCSP_CWMP_MSO_INTERFACE    )pMyObject->hCcspCwmpMsoIf;
    errno_t rc = -1;

    if ( !pCcspCwmpMsoIf )
    {
        pCcspCwmpMsoIf = (PCCSP_CWMP_MSO_INTERFACE)AnscAllocateMemory(sizeof(CCSP_CWMP_MSO_INTERFACE));

        if ( !pCcspCwmpMsoIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hCcspCwmpMsoIf = (ANSC_HANDLE)pCcspCwmpMsoIf;
        }

       rc = strcpy_s(pCcspCwmpMsoIf->Name, sizeof(pCcspCwmpMsoIf->Name), CCSP_CWMP_MSO_INTERFACE_NAME);
       if(rc != EOK)
       {
          ERR_CHK(rc);
          return  ANSC_STATUS_FAILURE;
       }

        pCcspCwmpMsoIf->InterfaceId      = CCSP_CWMP_MSO_INTERFACE_ID;
        pCcspCwmpMsoIf->hOwnerContext    = (ANSC_HANDLE)pMyObject;
        pCcspCwmpMsoIf->Size             = sizeof(CCSP_CWMP_MSO_INTERFACE);

        pCcspCwmpMsoIf->GetRpcMethods    = CcspCwmpAcsboMsoGetRpcMethods;
        pCcspCwmpMsoIf->Inform           = CcspCwmpAcsboMsoInform;
        pCcspCwmpMsoIf->TransferComplete = CcspCwmpAcsboMsoTransferComplete;
        pCcspCwmpMsoIf->AutonomousTransferComplete 
                                         = CcspCwmpAcsboMsoAutonomousTransferComplete;
        pCcspCwmpMsoIf->DuStateChangeComplete
                                         = CcspCwmpAcsboMsoDuStateChangeComplete;
        pCcspCwmpMsoIf->AutonomousDuStateChangeComplete
                                         = CcspCwmpAcsboMsoAutonomousDuStateChangeComplete;
        pCcspCwmpMsoIf->Kicked           = CcspCwmpAcsboMsoKicked;
        pCcspCwmpMsoIf->RequestDownload  = CcspCwmpAcsboMsoRequestDownload;
        pCcspCwmpMsoIf->ValueChanged     = CcspCwmpAcsboMsoValueChanged;
    }

    AnscCoEnrollObjects((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboInitialize
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
CcspCwmpAcsboInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_BROKER_OBJECT    pMyObject    = (PCCSP_CWMP_ACS_BROKER_OBJECT)hThisObject;

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
    pMyObject->Oid                      = CCSP_CWMP_ACS_BROKER_OID;
    pMyObject->Create                   = CcspCwmpAcsboCreate;
    pMyObject->Remove                   = CcspCwmpAcsboRemove;
    pMyObject->EnrollObjects            = CcspCwmpAcsboEnrollObjects;
    pMyObject->Initialize               = CcspCwmpAcsboInitialize;

    pMyObject->hCcspCwmpCpeController   = (ANSC_HANDLE)NULL;
    pMyObject->bActive                  = FALSE;

    pMyObject->GetCcspCwmpMsoIf         = CcspCwmpAcsboGetCcspCwmpMsoIf;
    pMyObject->GetCcspCwmpCpeController = CcspCwmpAcsboGetCcspCwmpCpeController;
    pMyObject->SetCcspCwmpCpeController = CcspCwmpAcsboSetCcspCwmpCpeController;
    pMyObject->GetProperty              = CcspCwmpAcsboGetProperty;
    pMyObject->SetProperty              = CcspCwmpAcsboSetProperty;
    pMyObject->ResetProperty            = CcspCwmpAcsboResetProperty;
    pMyObject->Reset                    = CcspCwmpAcsboReset;

    pMyObject->Engage                   = CcspCwmpAcsboEngage;
    pMyObject->Cancel                   = CcspCwmpAcsboCancel;

    /*
     * We shall initialize the configuration properties to the default values, which may be changed
     * later via the set_property() member function. Note that this call may not guarantee a valid
     * and legtimate configuration.
     */
    pMyObject->ResetProperty((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}
