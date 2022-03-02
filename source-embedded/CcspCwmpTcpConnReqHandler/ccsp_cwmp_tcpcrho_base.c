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

    module:	ccsp_cwmp_tcpcrho_base.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the basic container object functions
        of CCSP CWMP TCP Connection Request Handler object.

        *   CcspCwmpTcpcrhoCreate
        *   CcspCwmpTcpcrhoRemove
        *   CcspCwmpTcpcrhoEnrollObjects
        *   CcspCwmpTcpcrhoInitialize

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/19/08    initial revision.

**********************************************************************/


#include "ccsp_cwmp_tcpcrho_global.h"


/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CcspCwmpTcpcrhoCreate
            (
                ANSC_HANDLE                 hContainerContext,
                ANSC_HANDLE                 hOwnerContext,
                ANSC_HANDLE                 hAnscReserved
            );

    description:

        This function constructs CCSP CWMP TCP Conn Request Handler Object and
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
CcspCwmpTcpcrhoCreate
    (
        ANSC_HANDLE                 hContainerContext,
        ANSC_HANDLE                 hOwnerContext,
        ANSC_HANDLE                 hAnscReserved
    )
{
    UNREFERENCED_PARAMETER(hAnscReserved);
    PANSC_COMPONENT_OBJECT          pBaseObject  = NULL;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT)AnscAllocateMemory(sizeof(CCSP_CWMP_TCPCR_HANDLER_OBJECT));

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
    /* AnscCopyString(pBaseObject->Name, CCSP_CWMP_TCPCR_HANDLER_NAME); */

    pBaseObject->hContainerContext = hContainerContext;
    pBaseObject->hOwnerContext     = hOwnerContext;
    pBaseObject->Oid               = CCSP_CWMP_TCPCR_HANDLER_OID;
    pBaseObject->Create            = CcspCwmpTcpcrhoCreate;
    pBaseObject->Remove            = CcspCwmpTcpcrhoRemove;
    pBaseObject->EnrollObjects     = CcspCwmpTcpcrhoEnrollObjects;
    pBaseObject->Initialize        = CcspCwmpTcpcrhoInitialize;

    pBaseObject->EnrollObjects((ANSC_HANDLE)pBaseObject);
    pBaseObject->Initialize   ((ANSC_HANDLE)pBaseObject);

    return  (ANSC_HANDLE)pMyObject;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoRemove
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
CcspCwmpTcpcrhoRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PANSC_DSTO_WORKER_OBJECT        pDstoWorker   = (PANSC_DSTO_WORKER_OBJECT      )pMyObject->hDstoWorker;

    pMyObject->Cancel((ANSC_HANDLE)pMyObject);
    pMyObject->Reset ((ANSC_HANDLE)pMyObject);

    if ( pDstoWorker )
    {
        AnscFreeMemory(pDstoWorker);

        pMyObject->hDstoWorker = (ANSC_HANDLE)NULL;
    }

    AnscCoRemove((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoEnrollObjects
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
CcspCwmpTcpcrhoEnrollObjects
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PANSC_DSTO_WORKER_OBJECT        pDstoWorker   = (PANSC_DSTO_WORKER_OBJECT      )pMyObject->hDstoWorker;

    if ( !pDstoWorker )
    {
        pDstoWorker = (PANSC_DSTO_WORKER_OBJECT)AnscAllocateMemory(sizeof(ANSC_DSTO_WORKER_OBJECT));

        if ( !pDstoWorker )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hDstoWorker = (ANSC_HANDLE)pDstoWorker;
        }

        pDstoWorker->hWorkerContext = (ANSC_HANDLE)pMyObject;
        pDstoWorker->Init           = CcspCwmpTcpcrhoWorkerInit;
        pDstoWorker->Unload         = CcspCwmpTcpcrhoWorkerUnload;

        pDstoWorker->Accept         = CcspCwmpTcpcrhoDstowoAccept;
        pDstoWorker->SetOut         = CcspCwmpTcpcrhoDstowoSetOut;
        pDstoWorker->Remove         = CcspCwmpTcpcrhoDstowoRemove;
        pDstoWorker->Query          = CcspCwmpTcpcrhoDstowoQuery;
        pDstoWorker->ProcessSync    = CcspCwmpTcpcrhoDstowoProcessSync;
        pDstoWorker->ProcessAsync   = CcspCwmpTcpcrhoDstowoProcessAsync;
        pDstoWorker->SendComplete   = CcspCwmpTcpcrhoDstowoSendComplete;
        pDstoWorker->Notify         = CcspCwmpTcpcrhoDstowoNotify;
    }

    AnscCoEnrollObjects((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoInitialize
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
CcspCwmpTcpcrhoInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject    = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT)hThisObject;

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
    pMyObject->Oid                   = CCSP_CWMP_TCPCR_HANDLER_OID;
    pMyObject->Create                = CcspCwmpTcpcrhoCreate;
    pMyObject->Remove                = CcspCwmpTcpcrhoRemove;
    pMyObject->EnrollObjects         = CcspCwmpTcpcrhoEnrollObjects;
    pMyObject->Initialize            = CcspCwmpTcpcrhoInitialize;

    pMyObject->bActive               = FALSE;

    pMyObject->GetCcspCwmpCpeController  = CcspCwmpTcpcrhoGetCcspCwmpCpeController;
    pMyObject->SetCcspCwmpCpeController  = CcspCwmpTcpcrhoSetCcspCwmpCpeController;
    pMyObject->GetWebAcmIf           = CcspCwmpTcpcrhoGetWebAcmIf;
    pMyObject->SetWebAcmIf           = CcspCwmpTcpcrhoSetWebAcmIf;

    pMyObject->GetProperty           = CcspCwmpTcpcrhoGetProperty;
    pMyObject->SetProperty           = CcspCwmpTcpcrhoSetProperty;
    pMyObject->ResetProperty         = CcspCwmpTcpcrhoResetProperty;
    pMyObject->Reset                 = CcspCwmpTcpcrhoReset;

    pMyObject->Engage                = CcspCwmpTcpcrhoEngage;
    pMyObject->Cancel                = CcspCwmpTcpcrhoCancel;
    pMyObject->CreateTcpServers      = CcspCwmpTcpcrhoCreateTcpServers;
    pMyObject->RemoveTcpServers      = CcspCwmpTcpcrhoRemoveTcpServers;

    pMyObject->DstowoAccept          = CcspCwmpTcpcrhoDstowoAccept;
    pMyObject->DstowoSetOut          = CcspCwmpTcpcrhoDstowoSetOut;
    pMyObject->DstowoRemove          = CcspCwmpTcpcrhoDstowoRemove;
    pMyObject->DstowoQuery           = CcspCwmpTcpcrhoDstowoQuery;
    pMyObject->DstowoProcessSync     = CcspCwmpTcpcrhoDstowoProcessSync;
    pMyObject->DstowoProcessAsync    = CcspCwmpTcpcrhoDstowoProcessAsync;
    pMyObject->DstowoSendComplete    = CcspCwmpTcpcrhoDstowoSendComplete;
    pMyObject->DstowoNotify          = CcspCwmpTcpcrhoDstowoNotify;

    pMyObject->ProcessRequest        = CcspCwmpTcpcrhoProcessRequest;
    pMyObject->VerifyCredential      = CcspCwmpTcpcrhoVerifyCredential;

    /*
     * We shall initialize the configuration properties to the default values, which may be changed
     * later via the set_property() member function. Note that this call may not guarantee a valid
     * and legtimate configuration.
     */
    pMyObject->ResetProperty((ANSC_HANDLE)pMyObject);
    pMyObject->Reset        ((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}
