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

    module: ccsp_cwmp_cpeco_interface.c

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This module implements the some platform-dependent and
        general utility functions for CCSP CWMP Cpe Controller Object.

        *   CcspCwmphCreateCpeController

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
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_cpeco_global.h"


/**********************************************************************

    caller:     component objects

    prototype:

        ANSC_HANDLE
        CcspCwmpCreateCpeController
            (
                char*                       pTr069PAName,
                char*                       pCRName,
                ANSC_HANDLE                 hContainerContext,
                ANSC_HANDLE                 hOwnerContext,
                ANSC_HANDLE                 hAnscReserved
            )

    description:

        This function is called to create a new CCSP CWMP Cpe Controller
        object.

    argument:   char*                       pTr069PAName
                TR-069 PA name.

                char*                       pCRName
                CR name.
    
                ANSC_HANDLE                 hContainerContext
                This context handle is transparent to the object mapper
                wrapper, it's only meanful to the caller.

                ANSC_HANDLE                 hOwnerContext
                This context handle is transparent to the object mapper
                wrapper, it's only meanful to the caller.

                ANSC_HANDLE                 hAnscReserved
                This context handle is transparent to the object mapper
                wrapper, it's only meanful to the caller.

    return:     handle of the object mapper object.

**********************************************************************/

ANSC_HANDLE
CcspCwmpCreateCpeController
    (
        char*                       pTr069PAName,
        char*                       pCRName,
        ANSC_HANDLE                 hContainerContext,
        ANSC_HANDLE                 hOwnerContext,
        ANSC_HANDLE                 hAnscReserved
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = NULL;
    
    pCcspCwmpCpeController = 
        (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)CcspCwmpCpecoCreate
            (
                hContainerContext, 
                hOwnerContext, 
                hAnscReserved
            );

    if ( pCcspCwmpCpeController )
    {
        pCcspCwmpCpeController->SetPAName((ANSC_HANDLE)pCcspCwmpCpeController, pTr069PAName);
        pCcspCwmpCpeController->SetCRName((ANSC_HANDLE)pCcspCwmpCpeController, pCRName     );
    }

    return  (ANSC_HANDLE)pCcspCwmpCpeController;
}

