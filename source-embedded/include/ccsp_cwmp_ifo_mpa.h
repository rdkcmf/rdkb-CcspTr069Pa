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

    module:	ccsp_cwmp_ifo_mpa.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This wrapper file defines the base class data structure and
        interface for the CCSP CWMP Management Parameter Access Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/06/05    initial revision.
        06/15/11    supporting CCSP architecture
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_IFO_MPA_
#define  _CCSP_CWMP_IFO_MPA_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_ifo_interface.h"

#if 0
#define  CCSP_CWMP_MPA_ENTITY_ACS                        "ACS"
#define  CCSP_CWMP_MPA_ENTITY_Subscriber                 "Subscriber"
#define  CCSP_CWMP_MPA_ENTITY_System                     "System"        /* software itself */
#endif

/***********************************************************
    CCSP CWMP MANAGEMENT PARAMETER ACCESS INTERFACE DEFINITION
***********************************************************/

/*
 * Define some const values that will be used in the os wrapper object definition.
 */
#define  CCSP_CWMP_MPA_INTERFACE_NAME                    "cwmpManagementParameterAccessIf"
#define  CCSP_CWMP_MPA_INTERFACE_ID                      0

/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support), we
 * have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  BOOL
(*PFN_CWMPMPAIF_LOCK_ACCESS)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPMPAIF_UNLOCK_ACCESS)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPMPAIF_SET_VALUES)
    (
        ANSC_HANDLE                 hThisObject,
        void*                       pParamValueArray,
        ULONG                       ulArraySize,
        int*                        piStatus,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

typedef  ANSC_STATUS
(*PFN_CWMPMPAIF_SET_VALUES2)
    (
        ANSC_HANDLE                 hThisObject,
        void*                       pParamValueArray,
        ULONG                       ulArraySize,
        ULONG                       ulWriteID,
        int*                        piStatus,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

typedef  ANSC_STATUS
(*PFN_CWMPMPAIF_GET_VALUES)
    (
        ANSC_HANDLE                 hThisObject,
        SLAP_STRING_ARRAY*          pParamNameArray,
		ULONG						uMaxEntry,
        void**                      ppParamValueArray,
        PULONG                      pulArraySize,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

typedef  ANSC_STATUS
(*PFN_CWMPMPAIF_GET_NAMES)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamPath,
        BOOL                        bNextLevel,
        void**                      ppParamInfoArray,
        PULONG                      pulArraySize,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

typedef  ANSC_STATUS
(*PFN_CWMPMPAIF_SET_ATTRIBS)
    (
        ANSC_HANDLE                 hThisObject,
        void*                       pSetParamAttribArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

typedef  ANSC_STATUS
(*PFN_CWMPMPAIF_GET_ATTRIBS)
    (
        ANSC_HANDLE                 hThisObject,
        SLAP_STRING_ARRAY*          pParamNameArray,
		ULONG						uMaxEntry,
        void**                      ppParamAttribArray,
        PULONG                      pulArraySize,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

typedef  ANSC_STATUS
(*PFN_CWMPMPAIF_ADD_OBJ)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pObjName,
        PULONG                      pulObjInsNumber,
        int*                        piStatus,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );

typedef  ANSC_STATUS
(*PFN_CWMPMPAIF_DEL_OBJ)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pObjName,
        int*                        piStatus,
        ANSC_HANDLE*                phSoapFault,
        BOOL                        bExcludeInvNs
    );


/*
 * In each direction, the order of SOAP envelopes is defined independently from the number of enve-
 * lopes per HTTP post/response pair. Specifically, envelopes are ordered from first to last within
 * a single HTTP post/response and then between successive post/response pairs.
 *
 * On reception of SOAP requests from the ACS, the CPE MUST respond to each request in the order
 * they were received, where order is defined as described above. The definition of order places no
 * constraint on whether multiple responses are sent in a single HTTP post (if the ACS can accept
 * more than one envelope), or distributed over multiple HTTP posts.
 */
#define  CCSP_CWMP_MPA_INTERFACE_CLASS_CONTENT                                              \
    /* duplication of the base object class content */                                      \
    ANSCIFO_CLASS_CONTENT                                                                   \
    /* start of object class content */                                                     \
    PFN_CWMPMPAIF_LOCK_ACCESS       LockWriteAccess;                                        \
    PFN_CWMPMPAIF_UNLOCK_ACCESS     UnlockWriteAccess;                                      \
                                                                                            \
    /* RPC handling APIs */                                                                 \
    PFN_CWMPMPAIF_SET_VALUES        SetParameterValues;                                     \
    PFN_CWMPMPAIF_SET_VALUES2       SetParameterValuesWithWriteID;                          \
    PFN_CWMPMPAIF_GET_VALUES        GetParameterValues;                                     \
    PFN_CWMPMPAIF_GET_NAMES         GetParameterNames;                                      \
    PFN_CWMPMPAIF_SET_ATTRIBS       SetParameterAttributes;                                 \
    PFN_CWMPMPAIF_GET_ATTRIBS       GetParameterAttributes;                                 \
    PFN_CWMPMPAIF_ADD_OBJ           AddObject;                                              \
    PFN_CWMPMPAIF_DEL_OBJ           DeleteObject;                                           \
    /* end of object class content */                                                       \

typedef  struct
_CCSP_CWMP_MPA_INTERFACE
{
    CCSP_CWMP_MPA_INTERFACE_CLASS_CONTENT
}
CCSP_CWMP_MPA_INTERFACE,  *PCCSP_CWMP_MPA_INTERFACE;

#define  ACCESS_CCSP_CWMP_MPA_INTERFACE(p)               \
         ACCESS_CONTAINER(p, CCSP_CWMP_MPA_INTERFACE, Linkage)


#endif
