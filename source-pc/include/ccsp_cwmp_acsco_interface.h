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

    module:	ccsp_cwmp_acsco_interface.h

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This wrapper file defines all the platform-independent
        functions and macros for the CCSP CWMP Acs Connection Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/13/05    initial revision.
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_ACSCO_INTERFACE_
#define  _CCSP_CWMP_ACSCO_INTERFACE_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_co_interface.h"
#include "ansc_co_external_api.h"
#include "ccsp_cwmp_properties.h"

#include "http_ifo_bsp.h"
#include "http_ifo_acm.h"

/***********************************************************
       CCSP CWMP ACS CONNECTION COMPONENT OBJECT DEFINITION
***********************************************************/

/*
 * Define some const values that will be used in the object mapper object definition.
 */

#define  CCSP_CWMP_ACSCO_MAX_COOKIE                      8

/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support),
 * we have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  ANSC_HANDLE
(*PFN_CWMPACSCO_GET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_SET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hContext
    );

typedef  ANSC_HANDLE
(*PFN_CWMPACSCO_GET_IF)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_SET_IF)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hInterface
    );

typedef  char*
(*PFN_CWMPACSCO_GET_NAME)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_SET_NAME)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_GET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_SET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_RESET)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_CONNECT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_ACTION)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_REQUEST)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pSoapMessage,
        char*                       pMethodName,
        ULONG                       ulReqEnvCount,
        ULONG                       ulRepEnvCount
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_CLOSE)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_ADD_COOKIE)
    (
        ANSC_HANDLE                 hThisObject,
        PCHAR                       pCookie
    );

typedef  ANSC_STATUS
(*PFN_CWMPACSCO_REMOVE_COOKIES)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef	 int
(*PFN_CWMPACSCO_FIND_COOKIE)
	(
		ANSC_HANDLE                 hThisObject,
		PCHAR                       pCookieName
	);

typedef	 ANSC_STATUS
(*PFN_CWMPACSCO_DEL_COOKIE)
	(
		ANSC_HANDLE                 hThisObject,
		ULONG                       ulIndex
	);

/*
 * For a sequence of transactions forming a single session, a CPE SHOULD maintain a TCP connection
 * that persists throughout the duration of the session. To accommodate situations in which main-
 * taining a continuous TCP connection is not possible (e.g., operating through an HTTP 1.0 proxy),
 * the ACS SHOULD make use of a session cookie to maintain session state.
 */
#define  CCSP_CWMP_ACS_CONNECTION_CLASS_CONTENT                                             \
    /* duplication of the base object class content */                                      \
    ANSCCO_CLASS_CONTENT                                                                    \
    /* start of object class content */                                                     \
    ANSC_HANDLE                     hHttpHelpContainer;                                     \
    ANSC_HANDLE                     hHttpSimpleClient;                                      \
    ANSC_HANDLE                     hHttpMsgParser;                                         \
    ANSC_HANDLE                     hHttpBspIf;                                             \
    ANSC_HANDLE                     hHttpAcmIf;                                             \
    ANSC_HANDLE                     hCcspCwmpSession;                                       \
    char*                           AcsUrl;                                                 \
    char*                           Username;                                               \
    char*                           Password;                                               \
    char*                           Cookies[CCSP_CWMP_ACSCO_MAX_COOKIE];                    \
    ULONG                           NumCookies;                                             \
    char*                           AuthHeaderValue;                                        \
    BOOL                            bActive;                                                \
                                                                                            \
    PFN_CWMPACSCO_GET_CONTEXT       GetHttpSimpleClient;                                    \
    PFN_CWMPACSCO_GET_CONTEXT       GetCcspCwmpSession;                                     \
    PFN_CWMPACSCO_SET_CONTEXT       SetCcspCwmpSession;                                     \
    PFN_CWMPACSCO_GET_NAME          GetAcsUrl;                                              \
    PFN_CWMPACSCO_SET_NAME          SetAcsUrl;                                              \
    PFN_CWMPACSCO_GET_NAME          GetUsername;                                            \
    PFN_CWMPACSCO_SET_NAME          SetUsername;                                            \
    PFN_CWMPACSCO_GET_NAME          GetPassword;                                            \
    PFN_CWMPACSCO_SET_NAME          SetPassword;                                            \
    PFN_CWMPACSCO_RESET             Reset;                                                  \
                                                                                            \
    PFN_CWMPACSCO_CONNECT           Connect;                                                \
    PFN_CWMPACSCO_REQUEST           Request;                                                \
    PFN_CWMPACSCO_ACTION            RequestOnly;                                            \
    PFN_CWMPACSCO_CLOSE             Close;                                                  \
                                                                                            \
    PFN_HTTPBSPIF_POLISH            HttpBspPolish;                                          \
    PFN_HTTPBSPIF_BROWSE            HttpBspBrowse;                                          \
    PFN_HTTPBSPIF_NOTIFY            HttpBspNotify;                                          \
                                                                                            \
    PFN_HTTPACMIF_GET_CREDENTIAL    HttpGetCredential;                                      \
                                                                                            \
    PFN_CWMPACSCO_ADD_COOKIE        AddCookie;                                              \
    PFN_CWMPACSCO_REMOVE_COOKIES    RemoveCookies;                                          \
    PFN_CWMPACSCO_FIND_COOKIE       FindCookie;                                             \
    PFN_CWMPACSCO_DEL_COOKIE        DelCookie;                                              \
    /* end of object class content */                                                       \

typedef  struct
_CCSP_CWMP_ACS_CONNECTION_OBJECT
{
    CCSP_CWMP_ACS_CONNECTION_CLASS_CONTENT
}
CCSP_CWMP_ACS_CONNECTION_OBJECT,  *PCCSP_CWMP_ACS_CONNECTION_OBJECT;

#define  ACCESS_CCSP_CWMP_ACS_CONNECTION_OBJECT(p)       \
         ACCESS_CONTAINER(p, CCSP_CWMP_ACS_CONNECTION_OBJECT, Linkage)


#endif
