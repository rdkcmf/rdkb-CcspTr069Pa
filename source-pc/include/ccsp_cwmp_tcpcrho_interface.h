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

    module:	ccsp_cwmp_tcpcrho_interface.h

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This wrapper file defines all the platform-independent
        functions and macros for CCSP CWMP TCP Connection Request
        Handler object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/19/08    initial revision.
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_TCPCRHO_INTERFACE_
#define  _CCSP_CWMP_TCPCRHO_INTERFACE_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_co_interface.h"
#include "ansc_co_external_api.h"

#include "ansc_dsto_interface.h"
#include "ansc_dsto_external_api.h"
#include "ansc_dstowo_interface.h"

#include "http_properties.h"
#include "http_autho_info.h"
#include "web_properties.h"

/***********************************************************
PLATFORM INDEPENDENT CCSP CWMP TCP CONNECTION REQUEST HANDLER
***********************************************************/

/***********************************************************
CCSP CWMP TCP CONNECTION REQUEST HANDLER PROPERTY DEFINITION
***********************************************************/

#define  CCSP_CWMP_TCPCR_HANDLER_NAME                    "cwmpTcpConnReqHandler"
#define  CCSP_CWMP_TCPCR_HANDLER_OID                     0

#define  CCSP_CWMP_TCPCR_HANDLER_MODE_useXsocket         0x00000001

#define  CCSP_CWMP_TCPCR_HANDLER_DefSessionTimeout       1200    /* in seconds */

#define  CCSP_CWMP_TCPCR_MAX_MSG_SIZE                    4096    /* in bytes */

#define  CCSP_CWMP_TCPCR_SESSION_MODE_FINISH             0
#define  CCSP_CWMP_TCPCR_SESSION_MODE_COLLECT            1
#define  CCSP_CWMP_TCPCR_SESSION_MODE_PROCESS            2

#define  CCSP_CWMP_TCPCR_CWMP_TCP_PORT                   7547

#define  CCSP_CWMP_TCPCR_COOKIE_MAGIC_NUMBER             "MGCN"

typedef  struct
_CCSP_CWMP_TCPCR_HANDLER_PROPERTY
{
#ifdef _ANSC_IPV6_COMPATIBLE_
    char                            HostAddr[INET6_ADDRSTRLEN];
#else
    ANSC_IPV4_ADDRESS               HostAddress;
#endif
    USHORT                          HostPort;
    ULONG                           ServerMode;

    ULONG                           SessionTimeout;

    WEB_AUTH_SERVER_PROPERTY        AuthProperty;
}
CCSP_CWMP_TCPCR_HANDLER_PROPERTY,  *PCCSP_CWMP_TCPCR_HANDLER_PROPERTY;


typedef  struct
_CCSP_CWMP_TCPCR_HANDLER_SESSINFO
{
    WEB_AUTH_SESS_INFO_CONTENT

#ifdef _ANSC_IPV6_COMPATIBLE_
    char                            RemoteAddress[INET6_ADDRSTRLEN];
#else
    ULONG                           RemoteAddr;
#endif
}
CCSP_CWMP_TCPCR_HANDLER_SESSINFO, *PCCSP_CWMP_TCPCR_HANDLER_SESSINFO;


/*
 * Define some const values that will be used in the object mapper object definition.
 */

/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support),
 * we have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  ANSC_HANDLE
(*PFN_CWMPTCPCRHO_GET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPTCPCRHO_SET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hContext
    );

typedef  ANSC_HANDLE
(*PFN_CWMPTCPCRHO_GET_IF)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPTCPCRHO_SET_IF)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hInterface
    );

typedef  ANSC_STATUS
(*PFN_CWMPTCPCRHO_GET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPTCPCRHO_SET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPTCPCRHO_RESET)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPTCPCRHO_ENGAGE)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPTCPCRHO_CANCEL)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPTCPCRHO_PROCESS_REQ)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PVOID                       buffer,
        ULONG                       ulSize
    );

typedef  ANSC_STATUS
(*PFN_CWMPTCPCRHO_AUTH_REQ)
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        ULONG                       ulSize
    );

typedef  ANSC_STATUS
(*PFN_CWMPTCPCRHO_VERIFY_CRED)
    (
        ANSC_HANDLE                 hThisObject,
        PVOID                       buffer,
        ULONG                       ulSize,
        ANSC_HANDLE                 hSessAuthInfo
    );

/*
 * TR-069 defines a very simple way for CPE to connection to ACS in order for ACS to
 * manage CPE by using CWMP protocol. This object handles TCP connection request. By
 * nature, only one ACS is going to be used specified at any time to manage CPE. So
 * this object is actually an oversimplified web server that we can safely assume
 * there's only one session at most at any time. This object only maintains one
 * session that expires after the specified amount of time.
 */
#define  CCSP_CWMP_TCPCR_HANDLER_CLASS_CONTENT                                              \
    /* duplication of the base object class content */                                      \
    ANSCCO_CLASS_CONTENT                                                                    \
    /* start of object class content */                                                     \
    CCSP_CWMP_TCPCR_HANDLER_PROPERTY     Property;                                          \
    ANSC_HANDLE                     hTcpServer;                                             \
    ANSC_HANDLE                     hDstoWorker;                                            \
    BOOL                            bActive;                                                \
    ANSC_HANDLE                     hCcspCwmpCpeController;                                 \
    ULONG                           MagicID;                                                \
    ULONG                           LastAccessTime;                                         \
    CCSP_CWMP_TCPCR_HANDLER_SESSINFO     AuthSessionInfo;                                   \
    ANSC_HANDLE                     hWebAcmIf;                                              \
                                                                                            \
    PFN_CWMPTCPCRHO_GET_CONTEXT     GetCcspCwmpCpeController;                               \
    PFN_CWMPTCPCRHO_SET_CONTEXT     SetCcspCwmpCpeController;                               \
    PFN_CWMPTCPCRHO_GET_IF          GetWebAcmIf;                                            \
    PFN_CWMPTCPCRHO_SET_IF          SetWebAcmIf;                                            \
    PFN_CWMPTCPCRHO_GET_PROPERTY    GetProperty;                                            \
    PFN_CWMPTCPCRHO_SET_PROPERTY    SetProperty;                                            \
    PFN_CWMPTCPCRHO_RESET           ResetProperty;                                          \
    PFN_CWMPTCPCRHO_RESET           Reset;                                                  \
                                                                                            \
    PFN_CWMPTCPCRHO_ENGAGE          Engage;                                                 \
    PFN_CWMPTCPCRHO_CANCEL          Cancel;                                                 \
    PFN_CWMPTCPCRHO_ENGAGE          CreateTcpServers;                                       \
    PFN_CWMPTCPCRHO_CANCEL          RemoveTcpServers;                                       \
                                                                                            \
    PFN_DSTOWO_ACCEPT               DstowoAccept;                                           \
    PFN_DSTOWO_SETOUT               DstowoSetOut;                                           \
    PFN_DSTOWO_REMOVE               DstowoRemove;                                           \
    PFN_DSTOWO_QUERY                DstowoQuery;                                            \
    PFN_DSTOWO_PROCESS              DstowoProcessSync;                                      \
    PFN_DSTOWO_PROCESS              DstowoProcessAsync;                                     \
    PFN_DSTOWO_COMPLETE             DstowoSendComplete;                                     \
    PFN_DSTOWO_NOTIFY               DstowoNotify;                                           \
                                                                                            \
    PFN_CWMPTCPCRHO_PROCESS_REQ     ProcessRequest;                                         \
    PFN_CWMPTCPCRHO_VERIFY_CRED     VerifyCredential;                                       \
    /* end of object class content */                                                       \

typedef  struct
_CCSP_CWMP_TCPCR_HANDLER_OBJECT
{
    CCSP_CWMP_TCPCR_HANDLER_CLASS_CONTENT
}
CCSP_CWMP_TCPCR_HANDLER_OBJECT,  *PCCSP_CWMP_TCPCR_HANDLER_OBJECT;

#define  ACCESS_CCSP_CWMP_TCPCR_HANDLER_OBJECT(p)          \
         ACCESS_CONTAINER(p, CCSP_CWMP_TCPCR_HANDLER_OBJECT, Linkage)


#endif
