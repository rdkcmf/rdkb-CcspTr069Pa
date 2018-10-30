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

    module:	ccsp_cwmp_acsbo_interface.h

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This wrapper file defines all the platform-independent
        functions and macros for the CCSP CWMP Acs Broker Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/31/05    initial revision.
        04/25/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP COSA 2.0 CDMMM.
        10/12/11    resovling name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_ACSBO_INTERFACE_
#define  _CCSP_CWMP_ACSBO_INTERFACE_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_co_interface.h"
#include "ansc_co_external_api.h"
#include "ccsp_cwmp_properties.h"
#include "ccsp_cwmp_ifo_mso.h"


/***********************************************************
         CCSP CWMP ACS BROKER COMPONENT OBJECT DEFINITION
***********************************************************/

/*
 * Define some const values that will be used in the object mapper object definition.
 */

/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support),
 * we have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  ANSC_HANDLE
(*PFN_CWMPACSBO_GET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSBO_SET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hContext
    );

typedef  ANSC_HANDLE
(*PFN_CWMPACSBO_GET_IF)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSBO_SET_IF)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hInterface
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSBO_GET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSBO_SET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSBO_RESET)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSBO_ACQ_ACCESS)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSBO_REL_ACCESS)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSBO_ENGAGE)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSBO_CANCEL)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSBO_SETUP_ENV)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSBO_CLOSE_ENV)
    (
        ANSC_HANDLE                 hThisObject
    );

/*
 * The CPE WAN Management Protocol defines the following mechanisms that may be used by a CPE to
 * discover the address of its associated ACS:
 *
 *      1. The CPE may be configured locally with the URL of the ACS.
 *      2. As part of the IP layer auto-configuration, a DHCP server on the access
 *         network may be configured to include the ACS URL as a DHCP option.
 *      3. The CPE may have a default ACS URL that it may use if no other URL is provided
 *         to it.
 *
 * The ACS URL MUST be in the form of a valid HTTP or HTTPS URL. Use of an HTTPS URL indicates that
 * the ACS supports SSL. If an HTTPS URL is given, and the CPE that does not support SSL, it MAY
 * attempt to use HTTP assuming the remainder of the URL is unchanged.
 */
#define  CCSP_CWMP_ACS_BROKER_CLASS_CONTENT                                                 \
    /* duplication of the base object class content */                                      \
    ANSCCO_CLASS_CONTENT                                                                    \
    /* start of object class content */                                                     \
    CCSP_CWMP_ACS_BROKER_PROPERTY   Property;                                               \
    ANSC_HANDLE                     hCcspCwmpMsoIf;                                         \
    ANSC_HANDLE                     hCcspCwmpCpeController;                                 \
    BOOL                            bActive;                                                \
                                                                                            \
    PFN_CWMPACSBO_GET_IF            GetCcspCwmpMsoIf;                                       \
    PFN_CWMPACSBO_GET_CONTEXT       GetCcspCwmpCpeController;                               \
    PFN_CWMPACSBO_SET_CONTEXT       SetCcspCwmpCpeController;                               \
    PFN_CWMPACSBO_GET_PROPERTY      GetProperty;                                            \
    PFN_CWMPACSBO_SET_PROPERTY      SetProperty;                                            \
    PFN_CWMPACSBO_RESET             ResetProperty;                                          \
    PFN_CWMPACSBO_RESET             Reset;                                                  \
                                                                                            \
    PFN_CWMPACSBO_ENGAGE            Engage;                                                 \
    PFN_CWMPACSBO_CANCEL            Cancel;                                                 \
                                                                                            \
    /* end of object class content */                                                       \

typedef  struct
_CCSP_CWMP_ACS_BROKER_OBJECT
{
    CCSP_CWMP_ACS_BROKER_CLASS_CONTENT
}
CCSP_CWMP_ACS_BROKER_OBJECT,  *PCCSP_CWMP_ACS_BROKER_OBJECT;

#define  ACCESS_CCSP_CWMP_ACS_BROKER_OBJECT(p)           \
         ACCESS_CONTAINER(p, CCSP_CWMP_ACS_BROKER_OBJECT, Linkage)


#endif

