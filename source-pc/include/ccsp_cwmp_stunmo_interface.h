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

    module:	ccsp_cwmp_stunmo_interface.h

        For CCSP CWMP protocol

    ---------------------------------------------------------------

    description:

        This wrapper file defines the base class data structure and
        interface for the CCSP CWMP Stun Manager Objects.

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


#ifndef  _CCSP_CWMP_STUNMO_INTERFACE_
#define  _CCSP_CWMP_STUNMO_INTERFACE_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_co_interface.h"
#include "ansc_co_external_api.h"
#include "ccsp_cwmp_properties.h"

#include "stun_ifo_bsm.h"


/***********************************************************
    CCSP CWMP STUN MANAGER COMPONENT OBJECT DEFINITION
***********************************************************/

/*
 * Define some const values that will be used in the os wrapper object definition.
 */

/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support), we
 * have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  ANSC_HANDLE
(*PFN_CCSPSTUNMO_GET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_SET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hContext
    );

typedef  ANSC_HANDLE
(*PFN_CCSPSTUNMO_GET_IF)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_SET_IF)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hInterface
    );

typedef  char*
(*PFN_CCSPSTUNMO_GET_NAME)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_SET_NAME)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       name
    );

typedef  ULONG
(*PFN_CCSPSTUNMO_GET_ADDR)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_SET_ADDR)
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulAddr
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_GET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_SET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ULONG
(*PFN_CCSPSTUNMO_GET_MODE)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_SET_MODE)
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulMode
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_RESET)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_ENGAGE)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_CANCEL)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_SETUP_ENV)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_CLOSE_ENV)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_LOAD)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_APPLY)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  char*
(*PFN_CCSPSTUNMO_GET_HOST)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_REG_GET)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CCSPSTUNMO_REG_SET)
    (
        ANSC_HANDLE                 hThisObject
    );

/*
 * STUN is a lightweight protocol that allows applications to discover the presence and types of
 * NATs and firewalls between them and the public Internet. It also provides the ability for
 * applications to determine the public Internet Protocol addresses allocated to them by NAT. STUN
 * works with many existing NATs, and does not require any special behavior from them. As a result,
 * it allows a wide variety of applications to work through existing NAT infrastructure.
 */
#define  CCSP_CWMP_STUN_MANAGER_CLASS_CONTENT                                               \
    /* duplication of the base object class content */                                      \
    ANSCCO_CLASS_CONTENT                                                                    \
    /* start of object class content */                                                     \
    CCSP_CWMP_STUN_MANAGER_PROPERTY Property;                                               \
    CCSP_CWMP_STUN_INFO             CcspCwmpStunInfo;                                       \
    ANSC_HANDLE                     hCcspCwmpCpeController;                                 \
    ANSC_HANDLE                     hStunBsmIf;                                             \
    ANSC_HANDLE                     hStunSimpleClient;                                      \
    BOOL                            bActive;                                                \
    ULONG                           LastUdpConnReqTS;                                       \
    ULONG                           LastUdpConnReqID;                                       \
    BOOL                            bRetryTaskRunning;                                      \
                                                                                            \
    PFN_CCSPSTUNMO_GET_CONTEXT      GetCcspCwmpCpeController;                               \
    PFN_CCSPSTUNMO_SET_CONTEXT      SetCcspCwmpCpeController;                               \
    PFN_CCSPSTUNMO_GET_CONTEXT      GetStunSimpleClient;                                    \
    PFN_CCSPSTUNMO_GET_ADDR         GetClientAddr;                                          \
    PFN_CCSPSTUNMO_SET_ADDR         SetClientAddr;                                          \
    PFN_CCSPSTUNMO_GET_PROPERTY     GetProperty;                                            \
    PFN_CCSPSTUNMO_SET_PROPERTY     SetProperty;                                            \
    PFN_CCSPSTUNMO_RESET            ResetProperty;                                          \
    PFN_CCSPSTUNMO_RESET            Reset;                                                  \
                                                                                            \
    PFN_CCSPSTUNMO_ENGAGE           Engage;                                                 \
    PFN_CCSPSTUNMO_CANCEL           Cancel;                                                 \
    PFN_CCSPSTUNMO_SETUP_ENV        SetupEnv;                                               \
    PFN_CCSPSTUNMO_CLOSE_ENV        CloseEnv;                                               \
                                                                                            \
    PFN_CCSPSTUNMO_LOAD             LoadPolicy;                                             \
    PFN_CCSPSTUNMO_APPLY            ApplyStunSettings;                                      \
    PFN_CCSPSTUNMO_GET_HOST         GetAcsHostName;                                         \
                                                                                            \
    PFN_CCSPSTUNMO_REG_GET          RegGetStunInfo;                                         \
    PFN_CCSPSTUNMO_REG_SET          RegSetStunInfo;                                         \
                                                                                            \
    PFN_STUNBSMIF_RECV              StunBsmRecvMsg1;                                        \
    PFN_STUNBSMIF_RECV              StunBsmRecvMsg2;                                        \
    PFN_STUNBSMIF_NOTIFY            StunBsmNotify;                                          \
    /* end of object class content */                                                       \

typedef  struct
_CCSP_CWMP_STUN_MANAGER_OBJECT
{
    CCSP_CWMP_STUN_MANAGER_CLASS_CONTENT
}
CCSP_CWMP_STUN_MANAGER_OBJECT,  *PCCSP_CWMP_STUN_MANAGER_OBJECT;

#define  ACCESS_CCSP_CWMP_STUN_MANAGER_OBJECT(p)         \
         ACCESS_CONTAINER(p, CCSP_CWMP_STUN_MANAGER_OBJECT, Linkage)


#endif
