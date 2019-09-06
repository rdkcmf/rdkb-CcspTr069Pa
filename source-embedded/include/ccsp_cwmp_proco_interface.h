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

    module:	ccsp_cwmp_proco_interface.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This wrapper file defines all the platform-independent
        functions and macros for the CCSP CWMP Processor Object.

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
        04/25/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP COSA 2.0 CDMMM.
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_PROCO_INTERFACE_
#define  _CCSP_CWMP_PROCO_INTERFACE_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_co_interface.h"
#include "ansc_co_external_api.h"
#include "ccsp_cwmp_properties.h"

#include "ccsp_cwmp_ifo_mco.h"
#include "ccsp_management_server_pa_api.h"

/***********************************************************
        CCSP CWMP WMP PROCESSOR COMPONENT OBJECT DEFINITION
***********************************************************/

/*
 * Define some const values that will be used in the object mapper object definition.
 */
#define  CCSP_CWMPPO_DEF_INFORM_INTERVAL                24 * 3600 * 1000    /* in milli-seconds */
#define  CCSP_CWMPPO_DEF_INFORM_DELAY                   30 * 1000           /* in milli-seconds */
#define  CCSP_CWMPPO_DEF_PENDING_INFORM_DELAY           5 * 1000            /* in milli-seconds */

#define  CCSP_CWMPPO_REMOTE_CR_RESYNC_INTERVAL          30                  /* in seconds */

/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support),
 * we have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  ANSC_HANDLE
(*PFN_CWMPPROCO_GET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hContext
    );

typedef  ANSC_HANDLE
(*PFN_CWMPPROCO_GET_IF)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SET_IF)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hInterface
    );

typedef  char*
(*PFN_CWMPPROCO_GET_STRING)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SET_STRING)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pString
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SET_STRING2)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pString,
        BOOL                        bAcquiredFromDhcp
    );

typedef  BOOL
(*PFN_CWMPPROCO_GET_BOOL)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SET_BOOL)
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bEnabled
    );

typedef  ULONG
(*PFN_CWMPPROCO_GET_ULONG)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SET_ULONG)
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulValue
    );

typedef  ANSC_HANDLE
(*PFN_CWMPPROCO_GET_HANDLE)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SET_HANDLE)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hTime
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_GET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_RESET)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_ENGAGE)
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRestart
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_CANCEL)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SETUP_ENV)
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRestart
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_CLOSE_ENV)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_INVOKE)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_HANDLE
(*PFN_CWMPPROCO_ACQ_WMPSO)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_HANDLE
(*PFN_CWMPPROCO_ACQ_WMPSO2)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_HANDLE
(*PFN_CWMPPROCO_ACQ_WMPSO3)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_REL_WMPSO)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_DEL_WMPSO)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_DEL_ALL)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_CONFIG_PER)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SCHEDULE)
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulDelaySeconds,
        char*                       pCommandKey
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SIGNAL)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_ASYNC_CONNECT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_GET_INFO)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SAVE_TC)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCommandKey,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        BOOL                        bIsDownload,
        PCCSP_CWMP_FAULT            pFault
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SAVE_VC)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParameterName
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_DSC_VC)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParameterName
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_LOAD_VC)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_REMOVE_TC)
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bIsDownload,
        char*                       pCommandKey
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_LOAD_TC)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SAVE_ATC)
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        char*                       AnnounceURL,
        char*                       TransferURL,
        char*                       FileType,
        unsigned int                FileSize,
        char*                       TargetFileName,
        ANSC_HANDLE                 hFault,      
        ANSC_HANDLE                 hStartTime,  
        ANSC_HANDLE                 hCompleteTime
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_LOAD_ATC)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SAVE_DSCC)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hDsccReq
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_LOAD_DSCC)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SAVE_ADSCC)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hAdsccReq
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_LOAD_ADSCC)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_REDELIVER_EVT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_GET_UNDEL_EVT)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_DSC_UNDEL_EVT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_INIT_PAC)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_UPD_PAC)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hParamAttrArray,
        ULONG                       ulParamCount
    );

typedef  ULONG
(*PFN_CWMPPROCO_CHK_PAC)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName
    );

typedef  ANSC_STATUS
(*PFN_CWMPPROCO_SYNC_CR)
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRemoteOnly
    );

typedef  BOOL
(*PFN_CWMPPROCO_FS_PEND)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ULONG
(*PFN_CWMPPROCO_GET_ASC)
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bQueueLockAcquired
    );

/*
 * SOAP messages are carried between a CPE and an ACS using HTTP 1.1, where the CPE acts as the
 * HTTP client and the ACS acts as the HTTP server. The encoding of SOAP over HTTP extends the
 * basic HTTP profile for SOAP, as follows:
 *
 *      - A SOAP request from an ACS to a CPE is sent over a HTTP response, while the
 *        CPE's SOAP response to an ACS request is sent over a subsequent HTTP post.
 *      - Each HTTP post or response may contain more than one SOAP envelope (within the
 *        negotiated limits). Each envelope may contain a SOAP request or response,
 *        independent from any other envelope.
 *      - When there is more than one envelope in a single HTTP Request, the SOAPAction
 *        header in the HTTP Request MUST have no value (with no quotes), indicating that
 *        this header provides no information as to the intent of the message.
 */
#define  CCSP_CWMP_PROCESSOR_CLASS_CONTENT                                                  \
    /* duplication of the base object class content */                                      \
    ANSCCO_CLASS_CONTENT                                                                    \
    /* start of object class content */                                                     \
    CCSP_CWMP_PROCESSOR_PROPERTY    Property;                                               \
    ANSC_HANDLE                     hPeriodicTimerObj;                                      \
    ANSC_HANDLE                     hPeriodicTimerIf;                                       \
    ANSC_HANDLE                     hScheduleTimerObj;                                      \
    ANSC_HANDLE                     hScheduleTimerIf;                                       \
    ANSC_HANDLE                     hPendingInformTimerObj;                                 \
    ANSC_HANDLE                     hPendingInformTimerIf;                                  \
    char*                           SecheduledCommandKey;                                   \
    ANSC_HANDLE                     hCcspCwmpCpeController;                                 \
    ULONG                           AsyncTaskCount;                                         \
    ANSC_HANDLE                     hParamAttrCache;                                        \
    ANSC_HANDLE                     hCcspNamespaceMgr;                                      \
    ANSC_HANDLE                     CcspNamespaceMgrLock;                                   \
    ANSC_HANDLE                     hCcspCwmpMpaIf;                                         \
    BOOL                            bRemoteCRsSyncDone;                                     \
    ULONG                           LastRemoteCrSyncTime;                                   \
    BOOL                            bActive;                                                \
    BOOL                            MsFcInitDone;                                           \
    BOOL                            bAcsInfoRetrieved;                                      \
                                                                                            \
    QUEUE_HEADER                    WmpsoQueue;                                             \
    ANSC_LOCK                       WmpsoQueueLock;                                         \
    ANSC_EVENT                      AsyncConnectEvent;                                      \
                                                                                            \
    PFN_CWMPPROCO_GET_CONTEXT       GetCcspCwmpCpeController;                               \
    PFN_CWMPPROCO_SET_CONTEXT       SetCcspCwmpCpeController;                               \
    PFN_CWMPPROCO_GET_BOOL          GetInitialContact;                                      \
    PFN_CWMPPROCO_SET_BOOL          SetInitialContact;                                      \
    PFN_CWMPPROCO_GET_BOOL          GetInitialContactFactory;                               \
    PFN_CWMPPROCO_SET_BOOL          SetInitialContactFactory;                               \
    PFN_CWMPPROCO_GET_BOOL          IsCwmpEnabled;                                          \
    PFN_CWMPPROCO_GET_STRING        GetAcsUrl;                                              \
    PFN_CWMPPROCO_GET_STRING        GetAcsUsername;                                         \
    PFN_CWMPPROCO_GET_STRING        GetAcsPassword;                                         \
    PFN_CWMPPROCO_GET_BOOL          GetPeriodicInformEnabled;                               \
    PFN_CWMPPROCO_GET_ULONG         GetPeriodicInformInterval;                              \
    PFN_CWMPPROCO_GET_HANDLE        GetPeriodicInformTime;                                  \
                                                                                            \
    PFN_CWMPPROCO_GET_IF            GetCcspCwmpMpaIf;                                       \
    PFN_CWMPPROCO_GET_PROPERTY      GetProperty;                                            \
    PFN_CWMPPROCO_SET_PROPERTY      SetProperty;                                            \
    PFN_CWMPPROCO_RESET             ResetProperty;                                          \
    PFN_CWMPPROCO_RESET             Reset;                                                  \
                                                                                            \
    PFN_CWMPPROCO_ENGAGE            Engage;                                                 \
    PFN_CWMPPROCO_CANCEL            Cancel;                                                 \
    PFN_CWMPPROCO_SETUP_ENV         SetupEnv;                                               \
    PFN_CWMPPROCO_CLOSE_ENV         CloseEnv;                                               \
    PFN_CWMPPROCO_INVOKE            PeriodicTimerInvoke;                                    \
    PFN_CWMPPROCO_INVOKE            ScheduleTimerInvoke;                                    \
    PFN_CWMPPROCO_INVOKE            PendingInformTimerInvoke;                               \
    PFN_CWMPPROCO_SAVE_TC           SaveTransferComplete;                                   \
    PFN_CWMPPROCO_LOAD_TC           LoadTransferComplete;                                   \
    PFN_CWMPPROCO_SAVE_ATC          SaveAutonomousTransferComplete;                         \
    PFN_CWMPPROCO_LOAD_ATC          LoadAutonomousTransferComplete;                         \
    PFN_CWMPPROCO_SAVE_DSCC         SaveDUStateChangeComplete;                              \
    PFN_CWMPPROCO_LOAD_DSCC         LoadDUStateChangeComplete;                              \
    PFN_CWMPPROCO_SAVE_ADSCC        SaveAutonDUStateChangeComplete;                         \
    PFN_CWMPPROCO_LOAD_ADSCC        LoadAutonDUStateChangeComplete;                         \
    PFN_CWMPPROCO_REDELIVER_EVT     RedeliverEvents;                                        \
    PFN_CWMPPROCO_LOAD_VC           LoadValueChanged;                                       \
    PFN_CWMPPROCO_SAVE_VC           SaveValueChanged;                                       \
    PFN_CWMPPROCO_DSC_VC            DiscardValueChanged;                                    \
    PFN_CWMPPROCO_GET_UNDEL_EVT     GetUndeliveredEvents;                                   \
    PFN_CWMPPROCO_GET_UNDEL_EVT     GetUndeliveredTcEvents;                                 \
    PFN_CWMPPROCO_GET_UNDEL_EVT     GetUndeliveredAtcEvents;                                \
    PFN_CWMPPROCO_GET_UNDEL_EVT     GetUndeliveredDscEvents;                                \
    PFN_CWMPPROCO_GET_UNDEL_EVT     GetUndeliveredAdscEvents;                               \
    PFN_CWMPPROCO_DSC_UNDEL_EVT     DiscardUndeliveredEvents;                               \
                                                                                            \
    PFN_CWMPPROCO_ACQ_WMPSO         AcqWmpSession;                                          \
    PFN_CWMPPROCO_ACQ_WMPSO2        AcqWmpSession2;                                         \
    PFN_CWMPPROCO_ACQ_WMPSO3        AcqWmpSession3;                                         \
    PFN_CWMPPROCO_REL_WMPSO         RelWmpSession;                                          \
    PFN_CWMPPROCO_DEL_WMPSO         DelWmpSession;                                          \
    PFN_CWMPPROCO_DEL_ALL           DelAllSessions;                                         \
    PFN_CWMPPROCO_GET_ASC           GetActiveWmpSessionCount;                               \
    PFN_CWMPPROCO_FS_PEND           HasPendingInform;                                       \
                                                                                            \
    PFN_CWMPPROCO_CONFIG_PER        ConfigPeriodicInform;                                   \
    PFN_CWMPPROCO_SCHEDULE          ScheduleInform;                                         \
    PFN_CWMPPROCO_SIGNAL            SignalSession;                                          \
    PFN_CWMPPROCO_ASYNC_CONNECT     AsyncConnectTask;                                       \
                                                                                            \
    PFN_CWMPPROCO_GET_INFO          GetAcsInfo;                                             \
    PFN_CCSPMS_VALUECHANGE          MsValueChanged;                                         \
                                                                                            \
    PFN_CWMPPROCO_SYNC_CR           SyncNamespacesWithCR;                                   \
    PFN_CWMPPROCO_INIT_PAC          InitParamAttrCache;                                     \
    PFN_CWMPPROCO_INIT_PAC          PushAllVcToBackend;                                     \
    PFN_CWMPPROCO_UPD_PAC           UpdateParamAttrCache;                                   \
    PFN_CWMPPROCO_CHK_PAC           CheckParamAttrCache;                                    \
    /* end of object class content */                                                       \

typedef  struct
_CCSP_CWMP_PROCESSOR_OBJECT
{
    CCSP_CWMP_PROCESSOR_CLASS_CONTENT
}
CCSP_CWMP_PROCESSOR_OBJECT,  *PCCSP_CWMP_PROCESSOR_OBJECT;

#define  ACCESS_CCSP_CWMP_PROCESSOR_OBJECT(p)        \
         ACCESS_CONTAINER(p, CCSP_CWMP_PROCESSOR_OBJECT, Linkage)


#endif
