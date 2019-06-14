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

    module:	ccsp_cwmp_proco_exported_api.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This header file contains the prototype definition for all
        the exported functions provided by the CCSP CWMP Processor
        Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/02/05    initial revision.
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_PROCO_EXPORTED_API_
#define  _CCSP_CWMP_PROCO_EXPORTED_API_


/***********************************************************
       FUNCTIONS IMPLEMENTED IN CCSP_CWMP_PROCO_INTERFACE.C
***********************************************************/

ANSC_HANDLE
CcspCwmpCreateProcessor
    (
        ANSC_HANDLE                 hContainerContext,
        ANSC_HANDLE                 hOwnerContext,
        ANSC_HANDLE                 hAnscReserved
    );


/***********************************************************
          FUNCTIONS IMPLEMENTED IN CCSP_CWMP_PROCO_BASE.C
***********************************************************/

ANSC_HANDLE
CcspCwmppoCreate
    (
        ANSC_HANDLE                 hContainerContext,
        ANSC_HANDLE                 hOwnerContext,
        ANSC_HANDLE                 hAnscReserved
    );

ANSC_STATUS
CcspCwmppoRemove
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoEnrollObjects
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmppoInitialize
    (
        ANSC_HANDLE                 hThisObject
    );


#endif
