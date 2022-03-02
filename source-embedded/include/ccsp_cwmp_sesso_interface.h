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

    module:	ccsp_cwmp_sesso_interface.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This wrapper file defines all the platform-independent
        functions and macros for the CCSP CWMP Wmp Session Object.

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
        06/21/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_SESSO_INTERFACE_
#define  _CCSP_CWMP_SESSO_INTERFACE_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_co_interface.h"
#include "ansc_co_external_api.h"
#include "slap_definitions.h"
#include "ccsp_cwmp_properties.h"
#include "ccsp_cwmp_ifo_mco.h"


/***********************************************************
         CCSP CWMP SESSION COMPONENT OBJECT DEFINITION
***********************************************************/

/*
 * Define some const values that will be used in the object mapper object definition.
 */
#define  CCSP_CWMPSO_MAX_EVENT_NUMBER                64
#define  CCSP_CWMPSO_MAX_PARAM_NUMBER                128
#define  CCSP_CWMPSO_MAX_CALL_RETRY_TIMES            3

#define  CCSP_CWMPSO_RPCCALL_TIMEOUT                 45          /* in seconds */
#define  CCSP_CWMPSO_SESSION_TIMEOUT                 60          /* in seconds */
#define  CCSP_CWMPSO_SESSION_RETRY_INTERVAL          10          /* in seconds */

#define  CCSP_CWMPSO_SESSION_STATE_idle              0
#define  CCSP_CWMPSO_SESSION_STATE_connectNow        1
#define  CCSP_CWMPSO_SESSION_STATE_connected         2
#define  CCSP_CWMPSO_SESSION_STATE_informed          3
#define  CCSP_CWMPSO_SESSION_STATE_disconnectNow     4
#define  CCSP_CWMPSO_SESSION_STATE_abort             5
#define  CCSP_CWMPSO_SESSION_STATE_timeout           6
#define  CCSP_CWMPSO_SESSION_STATE_reconnect         7

#define  CCSP_CWMPSO_EVENTCODE_ValueChange           4 
#define  CCSP_CWMPSO_EVENTCODE_ConnectionRequest     6

/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support),
 * we have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  ANSC_HANDLE
(*PFN_CWMPSESSO_GET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_SET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hContext
    );

typedef  ANSC_HANDLE
(*PFN_CWMPSESSO_GET_IF)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_SET_IF)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hInterface
    );

typedef  char*
(*PFN_CWMPSESSO_GET_NAME)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_SET_NAME)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_GET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_SET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_RESET)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_ACQ_ACCESS)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_REL_ACCESS)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_INVOKE)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  BOOL
(*PFN_CWMPSESSO_IS_CONNECTED)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_CONNECT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_CLOSE_CONN)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_REBOOT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_PERFORM)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_ADD_EVENT)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCwmpEvent,
        BOOL                        bConnectNow
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_DISCARD_EVENT)
    (
        ANSC_HANDLE                 hThisObject,
        int                         EventCode
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_SAVE_EVENT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_DEL_ALL)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_ADD_PARAM)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        char*                       pParamValue,
        ULONG                       CwmpDataType,
        BOOL                        bConnectNow
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_GET_METHODS)
    (
        ANSC_HANDLE                 hThisObject,
        SLAP_STRING_ARRAY**         ppMethodList,
        PULONG                      pulErrorCode
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_INFORM)
    (
        ANSC_HANDLE                 hThisObject,
        PULONG                      pulErrorCode
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_TRANSFER_COMP)
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        char*                       pCommandKey,        /* 'CommentKey' argument passed in Download() or Upload() */
        ANSC_HANDLE                 hFault,             /* non-zero FaultCode is specified if transfer failed     */
        ANSC_HANDLE                 hStartTime,         /* the date and time transfer was started in UTC          */
        ANSC_HANDLE                 hCompleteTime,      /* the date and time transfer compoleted in UTC           */
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode       /* if event code(s) shall be added into session */
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_AT_TRANSFER_COMP)
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        ANSC_HANDLE                 hFault,             /* non-zero FaultCode is specified if transfer failed     */
        ANSC_HANDLE                 hStartTime,         /* the date and time transfer was started in UTC          */
        ANSC_HANDLE                 hCompleteTime,      /* the date and time transfer compoleted in UTC           */
        char*                       AnnounceURL,
        char*                       TransferURL,
        char*                       FileType,
        unsigned int                FileSize,
        char*                       TargetFileName,
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode       /* if event code(s) shall be added into session */
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_DUSC_COMP)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hDsccReq,
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode       /* if event code(s) shall be added into session */
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_AT_DUSC_COMP)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hAdsccReq,
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode       /* if event code(s) shall be added into session */
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_KICKED)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCommand,
        char*                       pReferer,
        char*                       pArg,
        char*                       pNext,
        char**                      ppNextUrl,
        PULONG                      pulErrorCode
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_REQ_DOWNLOAD)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pFileType,
        ANSC_HANDLE                 hFileTypeArgArray,
        ULONG                       ulArraySize,
        PULONG                      pulErrorCode
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_RECV)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pMessage
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_NOTIFY)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_ASYNC_PROCESS)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_ACTION)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPSESSO_CANCEL_RETRY)
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bConnectAgain
    );

typedef  BOOL
(*PFN_CWMPSESSO_INF_PENDING)
    (
        ANSC_HANDLE                 hThisObject
    );


/*
 * All transaction sessions MUST begin with an Inform message from the CPE contained in the initial
 * HTTP post. This serves to initiate the set of transactions and communicate the limitations of
 * the CPE with regard to message encoding. The session ceases when both the ACS and CPE have no
 * more requests to send, no responses remain due from either the ACS or the CPE. At such time, the
 * CPE may close the connection.
 *
 * No more than one transaction session between a CPE and its associated ACS may exist at a time.
 */
#define  CCSP_CWMP_SESSION_CLASS_CONTENT                                                    \
    /* duplication of the base object class content */                                      \
    ANSCCO_CLASS_CONTENT                                                                    \
    /* start of object class content */                                                     \
    ANSC_HANDLE                     hSessionTimerObj;                                       \
    ANSC_HANDLE                     hSessionTimerIf;                                        \
    ANSC_HANDLE                     hRetryTimerObj;                                         \
    ANSC_HANDLE                     hRetryTimerIf;                                          \
	ANSC_HANDLE						hDelayedActiveNotifTimerObj;							\
	ANSC_HANDLE						hDelayedActiveNotifTimerIf;								\
	BOOL							bDelayedActiveNotifTimerScheduled;						\
    ANSC_HANDLE                     hCcspCwmpAcsConnection;                                 \
    ANSC_HANDLE                     hCcspCwmpMcoIf;                                         \
    ANSC_HANDLE                     hCcspCwmpCpeController;                                 \
    ANSC_HANDLE                     hCcspCwmpProcessor;                                     \
    ULONG                           AsyncTaskCount;                                         \
    ANSC_LOCK                       AccessLock;                                             \
    BOOL                            bActive;                                                \
                                                                                            \
    ULONG                           GlobalRequestID;                                        \
    ULONG                           SessionState;                                           \
    ULONG                           CpeMaxEnvelopes;                                        \
    ULONG                           AcsMaxEnvelopes;                                        \
    ULONG                           ActiveResponses;                                        \
    BOOL                            bHoldRequests;                                          \
    BOOL                            bNoMoreRequests;                                        \
    BOOL                            bLastEmptyRequestSent;                                  \
                                                                                            \
    ULONG                           EventCount;                                             \
    ANSC_HANDLE                     EventArray[CCSP_CWMPSO_MAX_EVENT_NUMBER];               \
    ULONG                           RetryCount;                                             \
    ULONG                           ModifiedParamCount;                                     \
    char*                           ModifiedParamArray[CCSP_CWMPSO_MAX_PARAM_NUMBER];       \
    char*                           ModifiedParamValueArray[CCSP_CWMPSO_MAX_PARAM_NUMBER];  \
    ULONG                           ModifiedParamTypeArray[CCSP_CWMPSO_MAX_PARAM_NUMBER];   \
                                                                                            \
    BOOL                            bInformWhenActive;                                      \
                                                                                            \
    QUEUE_HEADER                    AsyncReqQueue;                                          \
    ANSC_LOCK                       AsyncReqQueueLock;                                      \
    QUEUE_HEADER                    SavedReqQueue;                                          \
    ANSC_LOCK                       SavedReqQueueLock;                                      \
    QUEUE_HEADER                    AsyncRepQueue;                                          \
    ANSC_LOCK                       AsyncRepQueueLock;                                      \
    ANSC_EVENT                      AsyncProcessEvent;                                      \
																							\
    PFN_CWMPSESSO_GET_CONTEXT       GetCcspCwmpAcsConnection;                               \
    PFN_CWMPSESSO_GET_IF            GetCcspCwmpMcoIf;                                       \
    PFN_CWMPSESSO_GET_CONTEXT       GetCcspCwmpCpeController;                               \
    PFN_CWMPSESSO_SET_CONTEXT       SetCcspCwmpCpeController;                               \
    PFN_CWMPSESSO_GET_CONTEXT       GetCcspCwmpProcessor;                                   \
    PFN_CWMPSESSO_SET_CONTEXT       SetCcspCwmpProcessor;                                   \
    PFN_CWMPSESSO_RESET             Reset;                                                  \
                                                                                            \
    PFN_CWMPSESSO_ACQ_ACCESS        AcqAccess;                                              \
    PFN_CWMPSESSO_REL_ACCESS        RelAccess;                                              \
    PFN_CWMPSESSO_INVOKE            SessionTimerInvoke;                                     \
    PFN_CWMPSESSO_INVOKE            SessionRetryTimerInvoke;                                \
    PFN_CWMPSESSO_INVOKE            DelayedActiveNotifTimerInvoke;                          \
    PFN_CWMPSESSO_ACTION            StartRetryTimer;                                        \
    PFN_CWMPSESSO_ACTION            StopRetryTimer;                                         \
    PFN_CWMPSESSO_ACTION            StopDelayedActiveNotifTimer;                            \
                                                                                            \
    PFN_CWMPSESSO_IS_CONNECTED      IsAcsConnected;                                         \
    PFN_CWMPSESSO_CONNECT           ConnectToAcs;                                           \
    PFN_CWMPSESSO_CLOSE_CONN        CloseConnection;                                        \
                                                                                            \
    PFN_CWMPSESSO_ADD_EVENT         AddCwmpEvent;                                           \
    PFN_CWMPSESSO_DISCARD_EVENT     DiscardCwmpEvent;                                       \
    PFN_CWMPSESSO_DEL_ALL           DelAllEvents;                                           \
    PFN_CWMPSESSO_ADD_PARAM         AddModifiedParameter;                                   \
    PFN_CWMPSESSO_DEL_ALL           DelAllParameters;                                       \
    PFN_CWMPSESSO_SAVE_EVENT        SaveCwmpEvent;                                          \
                                                                                            \
    PFN_CWMPSESSO_GET_METHODS       GetRpcMethods;                                          \
    PFN_CWMPSESSO_INFORM            Inform;                                                 \
    PFN_CWMPSESSO_TRANSFER_COMP     TransferComplete;                                       \
    PFN_CWMPSESSO_AT_TRANSFER_COMP  AutonomousTransferComplete;                             \
    PFN_CWMPSESSO_KICKED            Kicked;                                                 \
    PFN_CWMPSESSO_REQ_DOWNLOAD      RequestDownload;                                        \
                                                                                            \
    PFN_CWMPSESSO_RECV              RecvSoapMessage;                                        \
    PFN_CWMPSESSO_NOTIFY            NotifySessionClosed;                                    \
    PFN_CWMPSESSO_ASYNC_PROCESS     AsyncProcessTask;                                       \
                                                                                            \
    PFN_CWMPSESSO_CANCEL_RETRY      CancelRetryDelay;                                       \
                                                                                            \
    PFN_CWMPSESSO_DUSC_COMP         DUStateChangeComplete;                                  \
    PFN_CWMPSESSO_AT_DUSC_COMP      AutonomousDUStateChangeComplete;                        \
                                                                                            \
    PFN_CWMPSESSO_INF_PENDING       InformPending;                                          \
    /* end of object class content */                                                       \

typedef  struct
_CCSP_CWMP_SESSION_OBJECT
{
    CCSP_CWMP_SESSION_CLASS_CONTENT
}
CCSP_CWMP_SESSION_OBJECT,  *PCCSP_CWMP_SESSION_OBJECT;

#define  ACCESS_CCSP_CWMP_SESSION_OBJECT(p)          \
         ACCESS_CONTAINER(p, CCSP_CWMP_SESSION_OBJECT, Linkage)


/***********************************************************
       CCSP CWMP REQUEST/RESPONSE STRUCTURE DEFINITIONS
***********************************************************/

/*
 * The CPE will initiate a transaction session to the ACS as a result of the conditions listed
 * below:
 *
 *      - The first time CPE establishes a connection to the access network on initial
 *        installation
 *      - On power-up or reset
 *      - Once every PeriodicInformInterval (for example, every 24-hours)
 *      - When so instructed by the optional ScheduleInform method
 *      - Whenever the CPE receives a valid Connection Request from an ACS
 *      - Whenever the URL of the ACS changes
 *      - Whenever a parameter is modified that is required to initiate an Inform on
 *        change.
 *      - Whenever the value of a parameter that the ACS has marked for "active
 *        notification" via the SetParameterAttributes method is modified by an external
 *        cause.
 *
 * Once the connection to the ACS is successfully established, the CPE initiates a session by
 * sending an initial Inform request to the ACS. This indicates to the ACS the current status of
 * the CPE and that the CPE is ready to accept request from the ACS.
 */

/*
 * On reception of SOAP requests from the ACS, the CPE MUST respond to each request in the order
 * they were received, where order is defined as described in section 3.4.1. This definition of
 * order places no constraint on whether multiple responses are sent in a single HTTP post (if the
 * ACS can accept more than one envelope), or distributed over multiple HTTP posts.
 *
 * To prevent deadlocks, the CPE MUST NOT hold off responding to an ACS request to wait for a res-
 * ponse from the ACS to an earlier CPE request.
 */
typedef  struct
_CCSP_CWMPSO_ASYNC_REQUEST
{
    SINGLE_LINK_ENTRY               Linkage;
    ANSC_STATUS                     CallStatus;
    ANSC_EVENT                      AsyncEvent;
    ULONG                           Method;
    char*                           MethodName;
    char*                           RequestID;
    char*                           SoapEnvelope;
    ANSC_HANDLE                     hSoapResponse;
}
CCSP_CWMPSO_ASYNC_REQUEST,  *PCCSP_CWMPSO_ASYNC_REQUEST;

#define  ACCESS_CCSP_CWMPSO_ASYNC_REQUEST(p)         \
         ACCESS_CONTAINER(p, CCSP_CWMPSO_ASYNC_REQUEST, Linkage)

#define  CcspCwmpsoAllocAsyncRequest(async_req)                                             \
         {                                                                                  \
            async_req =                                                                     \
                (PCCSP_CWMPSO_ASYNC_REQUEST)AnscAllocateMemory                              \
                        (                                                                   \
                            sizeof(CCSP_CWMPSO_ASYNC_REQUEST)                               \
                        );                                                                  \
                                                                                            \
            if ( async_req )                                                                \
            {                                                                               \
                async_req->CallStatus    = ANSC_STATUS_TIMEOUT;                             \
                async_req->Method        = CCSP_CWMP_METHOD_Unknown;                        \
                async_req->MethodName    = NULL;                                            \
                async_req->RequestID     = NULL;                                            \
                async_req->SoapEnvelope  = NULL;                                            \
                async_req->hSoapResponse = (ANSC_HANDLE)NULL;                               \
                                                                                            \
                AnscInitializeEvent(&async_req->AsyncEvent);                                \
                AnscResetEvent     (&async_req->AsyncEvent);                                \
            }                                                                               \
         }

#define  CcspCwmpsoSignalAsyncRequest(async_req, status)                                    \
         {                                                                                  \
            async_req->CallStatus = status;                                                 \
                                                                                            \
            AnscSetEvent(&async_req->AsyncEvent);                                           \
         }

#define  CcspCwmpsoFreeAsyncRequest(async_req)                                              \
         {                                                                                  \
            if ( async_req->MethodName )                                                    \
            {                                                                               \
                AnscFreeMemory(async_req->MethodName);                                      \
                                                                                            \
                async_req->MethodName = NULL;                                               \
            }                                                                               \
                                                                                            \
            if ( async_req->RequestID )                                                     \
            {                                                                               \
                AnscFreeMemory(async_req->RequestID);                                       \
                                                                                            \
                async_req->RequestID = NULL;                                                \
            }                                                                               \
                                                                                            \
            if ( async_req->SoapEnvelope )                                                  \
            {                                                                               \
                AnscFreeMemory(async_req->SoapEnvelope);                                    \
                                                                                            \
                async_req->SoapEnvelope = NULL;                                             \
            }                                                                               \
                                                                                            \
            if ( async_req->hSoapResponse )                                                 \
            {                                                                               \
                CcspCwmpFreeSoapResponse                                                    \
                    (                                                                       \
                        ((PCCSP_CWMP_SOAP_RESPONSE)async_req->hSoapResponse)                \
                    );                                                                      \
                                                                                            \
                async_req->hSoapResponse = (ANSC_HANDLE)NULL;                               \
            }                                                                               \
                                                                                            \
            AnscFreeEvent     (&async_req->AsyncEvent);                                     \
            AnscFreeMemory(async_req);                                                      \
         }

typedef  struct
_CCSP_CWMPSO_ASYNC_RESPONSE
{
    SINGLE_LINK_ENTRY               Linkage;
    ULONG                           Method;
    char*                           SoapEnvelope;
}
CCSP_CWMPSO_ASYNC_RESPONSE,  *PCCSP_CWMPSO_ASYNC_RESPONSE;

#define  ACCESS_CCSP_CWMPSO_ASYNC_RESPONSE(p)        \
         ACCESS_CONTAINER(p, CCSP_CWMPSO_ASYNC_RESPONSE, Linkage)

#define  CcspCwmpsoAllocAsyncResponse(async_rep)                                            \
         {                                                                                  \
            async_rep =                                                                     \
                (PCCSP_CWMPSO_ASYNC_RESPONSE)AnscAllocateMemory                             \
                        (                                                                   \
                            sizeof(CCSP_CWMPSO_ASYNC_RESPONSE)                              \
                        );                                                                  \
                                                                                            \
            if ( async_rep )                                                                \
            {                                                                               \
                async_rep->Method       = CCSP_CWMP_METHOD_Unknown;                         \
                async_rep->SoapEnvelope = NULL;                                             \
            }                                                                               \
         }

#define  CcspCwmpsoFreeAsyncResponse(async_rep)                                             \
         {                                                                                  \
            if ( async_rep->SoapEnvelope )                                                  \
            {                                                                               \
                AnscFreeMemory(async_rep->SoapEnvelope);                                    \
                                                                                            \
                async_rep->SoapEnvelope = NULL;                                             \
            }                                                                               \
                                                                                            \
            AnscFreeMemory(async_rep);                                                      \
         }

#endif
