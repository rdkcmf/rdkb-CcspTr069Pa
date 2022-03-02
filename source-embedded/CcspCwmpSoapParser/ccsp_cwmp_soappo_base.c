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

    module: ccsp_cwmp_soappo_base.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the basic container object functions
        of the DSL Home SoapParser Object.

        *   CcspCwmpSoappoCreate
        *   CcspCwmpSoappoRemove
        *   CcspCwmpSoappoEnrollObjects
        *   CcspCwmpSoappoInitialize

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        05/07/05    initial revision.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_soappo_global.h"


/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CcspCwmpSoappoCreate
            (
                ANSC_HANDLE                 hContainerContext,
                ANSC_HANDLE                 hOwnerContext,
                ANSC_HANDLE                 hAnscReserved
            );

    description:

        This function constructs the CCSP CWMP SoapParser Object and
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
CcspCwmpSoappoCreate
    (
        ANSC_HANDLE                 hContainerContext,
        ANSC_HANDLE                 hOwnerContext,
        ANSC_HANDLE                 hAnscReserved
    )
{
    UNREFERENCED_PARAMETER(hAnscReserved);
    PANSC_COMPONENT_OBJECT          pBaseObject  = NULL;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pMyObject    = NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCCSP_CWMP_SOAP_PARSER_OBJECT)AnscAllocateMemory(sizeof(CCSP_CWMP_SOAP_PARSER_OBJECT));

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
    /* AnscCopyString(pBaseObject->Name, CCSP_CWMP_SOAP_PARSER_NAME); */

    pBaseObject->hContainerContext = hContainerContext;
    pBaseObject->hOwnerContext     = hOwnerContext;
    pBaseObject->Oid               = CCSP_CWMP_SOAP_PARSER_OID;
    pBaseObject->Create            = CcspCwmpSoappoCreate;
    pBaseObject->Remove            = CcspCwmpSoappoRemove;
    pBaseObject->EnrollObjects     = CcspCwmpSoappoEnrollObjects;
    pBaseObject->Initialize        = CcspCwmpSoappoInitialize;

    pBaseObject->EnrollObjects((ANSC_HANDLE)pBaseObject);
    pBaseObject->Initialize   ((ANSC_HANDLE)pBaseObject);

    return  (ANSC_HANDLE)pMyObject;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpSoappoRemove
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
CcspCwmpSoappoRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pMyObject       = (PCCSP_CWMP_SOAP_PARSER_OBJECT)hThisObject;

    AnscCoRemove((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpSoappoEnrollObjects
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
CcspCwmpSoappoEnrollObjects
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pMyObject       = (PCCSP_CWMP_SOAP_PARSER_OBJECT)hThisObject;

    AnscCoEnrollObjects((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpSoappoInitialize
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
CcspCwmpSoappoInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pMyObject    = (PCCSP_CWMP_SOAP_PARSER_OBJECT)hThisObject;

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
    pMyObject->Oid                            = CCSP_CWMP_SOAP_PARSER_OID;
    pMyObject->Create                         = CcspCwmpSoappoCreate;
    pMyObject->Remove                         = CcspCwmpSoappoRemove;
    pMyObject->EnrollObjects                  = CcspCwmpSoappoEnrollObjects;
    pMyObject->Initialize                     = CcspCwmpSoappoInitialize;

    pMyObject->Reset                          = CcspCwmpSoappoReset;

    pMyObject->ProcessSoapEnvelopes           = CcspCwmpSoappoProcessSoapEnvelopes;

    pMyObject->BuildSoapReq_GetRpcMethods     = CcspCwmpSoappoBuildSoapReq_GetRpcMethods;
    pMyObject->BuildSoapReq_Inform            = CcspCwmpSoappoBuildSoapReq_Inform;
    pMyObject->BuildSoapReq_TransferComplete  = CcspCwmpSoappoBuildSoapReq_TransferComplete;
    pMyObject->BuildSoapReq_AutonomousTransferComplete  
                                              = CcspCwmpSoappoBuildSoapReq_AutonomousTransferComplete;
    pMyObject->BuildSoapReq_DUStateChangeComplete
                                              = CcspCwmpSoappoBuildSoapReq_DUStateChangeComplete;
    pMyObject->BuildSoapReq_AutonomousDUStateChangeComplete
                                              = CcspCwmpSoappoBuildSoapReq_AutonomousDUStateChangeComplete;

    pMyObject->BuildSoapRep_Error             = CcspCwmpSoappoBuildSoapRep_Error;
    pMyObject->BuildSoapRep_GetRpcMethods     = CcspCwmpSoappoBuildSoapRep_GetRPCMethods;
    pMyObject->BuildSoapRep_SetParameterValues= CcspCwmpSoappoBuildSoapRep_SetParameterValues;
    pMyObject->BuildSoapRep_GetParameterValues= CcspCwmpSoappoBuildSoapRep_GetParameterValues;
    pMyObject->BuildSoapRep_GetParameterNames = CcspCwmpSoappoBuildSoapRep_GetParameterNames;
    pMyObject->BuildSoapRep_SetParameterAttributes
                                              = CcspCwmpSoappoBuildSoapRep_SetParameterAttributes;
    pMyObject->BuildSoapRep_GetParameterAttributes
                                              = CcspCwmpSoappoBuildSoapRep_GetParameterAttributes;
    pMyObject->BuildSoapRep_AddObject         = CcspCwmpSoappoBuildSoapRep_AddObject;
    pMyObject->BuildSoapRep_DeleteObject      = CcspCwmpSoappoBuildSoapRep_DeleteObject;
    pMyObject->BuildSoapRep_Download          = CcspCwmpSoappoBuildSoapRep_Download;
    pMyObject->BuildSoapRep_Reboot            = CcspCwmpSoappoBuildSoapRep_Reboot;
    pMyObject->BuildSoapRep_Upload            = CcspCwmpSoappoBuildSoapRep_Upload;
    pMyObject->BuildSoapRep_FactoryReset      = CcspCwmpSoappoBuildSoapRep_FactoryReset;
    pMyObject->BuildSoapRep_ChangeDUState     = CcspCwmpSoappoBuildSoapRep_ChangeDUState;
    
    pMyObject->BuildSoapRep_ScheduleInform    = CcspCwmpSoappoBuildSoapRep_ScheduleInform;
    pMyObject->BuildSoapRep_SetVouchers       = CcspCwmpSoappoBuildSoapRep_SetVouchers;
    pMyObject->BuildSoapRep_GetQueuedTransfers= CcspCwmpSoappoBuildSoapRep_GetQueuedTransfers;
    pMyObject->BuildSoapReq_RequestDownload   = CcspCwmpSoappoBuildSoapReq_RequestDownload;
    pMyObject->BuildSoapReq_Kicked            = CcspCwmpSoappoBuildSoapReq_Kicked;
    pMyObject->BuildSoapRep_GetOptions        = CcspCwmpSoappoBuildSoapRep_GetOptions;

    return  ANSC_STATUS_SUCCESS;
}
