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

    module: ccsp_cwmp_cpeco_interface.h

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This wrapper file defines all the platform-independent
        functions and macros for the CCSP CWMP Cpe Controller Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Bin     Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/09/2005    initial revision.
        02/04/2010    Bin added Statistics interface for TR69
        06/21/11      decouple TR-069 PA from Data Model Manager
                      and make it work with CCSP architecture.
        10/12/11      resolve namespace conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_CPECO_INTERFACE_
#define  _CCSP_CWMP_CPECO_INTERFACE_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_co_interface.h"
#include "ansc_co_external_api.h"
#include "ccsp_cwmp_properties.h"

#include "ccsp_cwmp_ifo_mco.h"
#include "ccsp_cwmp_ifo_cfg.h"

#include "ccsp_types.h"
#include "ccsp_tr069pa_mapper_def.h"
#include "ccsp_base_api.h"

/***********************************************************
       CCSP CWMP CPE CONTROLLER COMPONENT OBJECT DEFINITION
***********************************************************/

#define  CCSP_TR069PA_SESSION_PRIORITY_READONLY     9
#define  CCSP_TR069PA_SESSION_PRIORITY_WRTIABLE     10

/*
 * Define some const values that will be used in the object mapper object definition.
 */
#define  CCSP_CR_DEF_NAME                           CCSP_DBUS_INTERFACE_CR
#define  CCSP_TR069PA_DEF_NAME                      "com.cisco.spvtg.ccsp.tr069pa"

/*
 * Define CWMP statistics related structure
 */
#define  CCSP_CWMP_CONNECTION_STATUS_SUCCESS                     1
#define  CCSP_CWMP_CONNECTION_STATUS_FAILURE                     0

typedef  struct
_CCSP_CWMP_STATISTICS
{
    ULONG                           uTcpSuccess;
    ULONG                           uTcpFailures;
    ULONG                           uTlsFailures;
    ANSC_UNIVERSAL_TIME             LastConnectionTime;
    ULONG                           uLastConnectionStatus;
    ANSC_UNIVERSAL_TIME             LastInformResponseTime;
    ANSC_UNIVERSAL_TIME             LastReceivedSPVTime;
}
CCSP_CWMP_STATISTICS,  *PCCSP_CWMP_STATISTICS;


/* define CPE controller event notification type */
#define  CCSP_CWMP_CPECO_EVENT_Unkonwn                   0
#define  CCSP_CWMP_CPECO_EVENT_InterfaceUp               1
#define  CCSP_CWMP_CPECO_EVENT_InterfaceDown             2
#define  CCSP_CWMP_CPECO_EVENT_InterfaceIpAddrChanged    3


#define  CCSP_CWMP_CPECO_CWMP_START_INTERVAL				5 * 1000           /* in milli-seconds */


#define  CcspCwmpCpecoRemoveParamNameValues(numParams, names, values)                       \
    do {                                                                                \
        int                         i;                                                  \
        for (i = 0; i < numParams; i ++)                                                \
        {                                                                               \
            if ( names[i] ) AnscFreeMemory(names[i]);                                   \
            if ( values[i] ) CcspVariableClean(AnscFreeMemory, values[i]);              \
        }                                                                               \
        AnscFreeMemory(names);                                                          \
        AnscFreeMemory(values);                                                         \
    } while (0)

#define  CcspCwmpCpecoAllocateStringArray(numParams, sarray)                                \
    sarray = (char**)AnscAllocateMemory(numParams * sizeof(char*))

#define  CcspCwmpCpecoRemoveParamNameStringValues(numParams, names, values)                 \
    do {                                                                                \
        int                         i;                                                  \
        for (i = 0; i < numParams; i ++)                                                \
        {                                                                               \
            if ( names[i] ) AnscFreeMemory(names[i]);                                   \
            if ( values[i] ) AnscFreeMemory(values[i]);                                 \
        }                                                                               \
        AnscFreeMemory(names);                                                          \
        AnscFreeMemory(values);                                                         \
    } while (0)


/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support),
 * we have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  ANSC_HANDLE
(*PFN_CWMPCPECO_GET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_SET_CONTEXT)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hContext
    );

typedef  ANSC_HANDLE
(*PFN_CWMPCPECO_GET_IF)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_SET_IF)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hInterface
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_GET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_SET_PROPERTY)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_RESET)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_ACTION)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_START_CWMP)
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bInformAcs,
        BOOL                        bRestart
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_RESTART_CWMP)
    (
        ANSC_HANDLE                         hThisObject,
        BOOL                                bInformAcs
    );

typedef  void
(*PFN_CWMPCPECO_EVENT_NOTIFY)
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulEvent,
        ANSC_HANDLE                 hEventContext
    );

typedef  char*
(*PFN_CWMPCPECO_GET_ROBJ)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_SET_ROBJ)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRootObject
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_GPV)
    (
        ANSC_HANDLE                 hThisObject,
        char**                      ppParamNames,
        int                         NumOfParams,
        PCCSP_VARIABLE              pParamValues
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_GPV2)
    (
        ANSC_HANDLE                 hThisObject,
        char**                      ppParamNames,
        int                         NumOfParams,
        char**                      pParamValues
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_GPV3)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        char**                      pParamValue
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_SPV)
    (
        ANSC_HANDLE                 hThisObject,
        char**                      ppParamNames,
        int                         NumOfParams,
        PCCSP_VARIABLE              pParamValues
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_AOS)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pObjectName,
        int                         NumInstances,
        PULONG                      pInsNumbers,
        PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_DOS)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pObjectName,
        int                         NumInstances,
        PULONG                      pInsNumbers,
        PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault,
        ULONG                       DelaySeconds
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_MONITOR_OPS)
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bMonitor,
        char*                       pObjectName,
        int                         NumInstances,
        PULONG                      pInsNumbers,
        char*                       pStateParamName,
        PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_REG)
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRegister
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_SETNAME)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_SETNAME2)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pKey,
        char*                       pName
    );

typedef  char*
(*PFN_CWMPCPECO_GETNAME)
    (
        ANSC_HANDLE                 hThisObject
    );

typedef  char*
(*PFN_CWMPCPECO_GETNAME2)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pKey
    );

typedef  int
(*PFN_CWMPCPECO_GET_NOTIF)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_SET_NOTIF)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        int                         Notification
    );

typedef  int
(*PFN_CWMPCPECO_GET_PTYPE)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_SAVE_CFG_PSM)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCfgKey,
        char*                       pCfgValue
    );

typedef  char*
(*PFN_CWMPCPECO_LOAD_CFG_PSM)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCfgKey
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_SET_PARAM_KEY)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamKey
    );

typedef  ULONG
(*PFN_CWMPCPECO_ACQ_CR_SESSID)
    (
        ANSC_HANDLE                 hThisObject,
        int                         Priority
    );

typedef  ANSC_STATUS
(*PFN_CWMPCPECO_REL_CR_SESSID)
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       CrSessionID
    );

/*
 * The CPE WAN Management Protocol allows an ACS to provision a CPE or collection of CPE based on a
 * variety of criteria. The provisioning mechanism includes specific provisioning parameters and a
 * general mecahnism for adding vendor-specific provisioning capabilities as needed.
 *
 * The provisioning mechanism allows CPE provisioning at the time of initial connection to the
 * broadband access network, and the ability to re-provision at any subsequent time. This includes
 * support for asynchronous ACS-initiated re-provisioning of a CPE.
 */
#define  CCSP_CWMP_CPE_CONTROLLER_CLASS_CONTENT                                             \
    /* duplication of the base object class content */                                      \
    ANSCCO_CLASS_CONTENT                                                                    \
    /* start of object class content */                                                     \
    CCSP_CWMP_CPE_CONTROLLER_PROPERTY    Property;                                          \
    ANSC_HANDLE                     hCcspCwmpAcsBroker;                                     \
    ANSC_HANDLE                     hCcspCwmpProcessor;                                     \
    ANSC_HANDLE                     hCcspCwmpDatabase;                                      \
    ANSC_HANDLE                     hCcspCwmpSoapParser;                                    \
    ANSC_HANDLE                     hCcspCwmpTcpConnReqHandler;                             \
    ANSC_HANDLE                     hCcspCwmpStunManager;                                   \
    ANSC_HANDLE                     hMsgBusHandle;                                          \
    char*                           PAName;                                                 \
    char*                           SubsysName;                                             \
    char*                           CRName;                                                 \
    char*                           CRNameWithPrefix;                                       \
    char*                           PAMapperFile;                                           \
    char*                           PACustomMapperFile;                                     \
    char*                           PANameWithPrefix;                                       \
    char*                           SdmXmlFile;                                             \
    char*                           OutboundIfName;                                         \
                                                                                            \
    BOOL                            bBootstrapInformScheduled;                              \
    BOOL                            bBootInformScheduled;                                   \
    BOOL                            bDelayedInformCancelled;                                \
    CCSP_CWMP_STATISTICS            cwmpStats;                                              \
    BOOL                            bCWMPStarted;                                           \
    BOOL                            bCpeRebooting;                                          \
    BOOL                            bCWMPStopping;                                          \
    BOOL                            bBootInformSent;                                        \
    BOOL                            bRestartCWMP;                                           \
                                                                                            \
    ANSC_HANDLE                     hStartCwmpTimerObj;                                     \
    ANSC_HANDLE                     hStartCwmpTimerIf;                                      \
                                                                                            \
    ANSC_HANDLE                     hCcspCwmpCfgIf;                                         \
    ANSC_HANDLE                     hCcspCwmpStaIf;                                         \
    BOOL                            bActive;                                                \
                                                                                            \
    BOOL                            bBootEventAdded;                                        \
    char*                           pRootObject;                                            \
    CCSP_HANDLE                     hTr069PaMapper;                                         \
    char*                           Subsystem[CCSP_SUBSYSTEM_MAX_COUNT];                    \
    char*                           MBusPath [CCSP_SUBSYSTEM_MAX_COUNT];                    \
    int                             NumSubsystems;                                          \
                                                                                            \
    PFN_CWMPCPECO_GET_CONTEXT       GetCcspCwmpAcsBroker;                                   \
    PFN_CWMPCPECO_GET_CONTEXT       GetCcspCwmpProcessor;                                   \
    PFN_CWMPCPECO_GET_CONTEXT       GetCcspCwmpSoapParser;                                  \
    PFN_CWMPCPECO_GET_CONTEXT       GetCcspCwmpTcpConnReqHandler;                           \
    PFN_CWMPCPECO_GET_ROBJ          GetRootObject;                                          \
    PFN_CWMPCPECO_SET_ROBJ          SetRootObject;                                          \
                                                                                            \
    PFN_CWMPCPECO_GET_PROPERTY      GetProperty;                                            \
    PFN_CWMPCPECO_SET_PROPERTY      SetProperty;                                            \
    PFN_CWMPCPECO_RESET             ResetProperty;                                          \
    PFN_CWMPCPECO_RESET             Reset;                                                  \
                                                                                            \
    PFN_CWMPCPECO_GET_IF            GetCcspCwmpMcoIf;                                       \
    PFN_CWMPCPECO_GET_IF            GetCcspCwmpMsoIf;                                       \
                                                                                            \
    PFN_CWMPCPECO_ACTION            Engage;                                                 \
    PFN_CWMPCPECO_ACTION            Cancel;                                                 \
    PFN_CWMPCPECO_ACTION            SetupEnv;                                               \
    PFN_CWMPCPECO_ACTION            CloseEnv;                                               \
                                                                                            \
    PFN_CWMPCPECO_START_CWMP        StartCWMP;                                              \
    PFN_CWMPCPECO_ACTION            StopCWMP;                                               \
    PFN_CWMPCPECO_RESTART_CWMP      RestartCWMP;                                            \
    PFN_CWMPCPECO_GET_IF            GetCcspCwmpCfgIf;                                       \
    PFN_CWMPCPECO_SET_IF            SetCcspCwmpCfgIf;                                       \
                                                                                            \
    PFN_CWMPCPECO_ACTION            InformNow;                                              \
    PFN_CWMPCPECO_EVENT_NOTIFY      NotifyEvent;                                            \
    PFN_CWMPCPECO_ACQ_CR_SESSID     AcqCrSessionID;                                         \
    PFN_CWMPCPECO_REL_CR_SESSID     RelCrSessionID;                                         \
                                                                                            \
    PFN_CWMPCPECO_GPV               GetParamValues;                                         \
    PFN_CWMPCPECO_GPV2              GetParamStringValues;                                   \
    PFN_CWMPCPECO_GPV3              GetParamStringValue;                                    \
    /*PFN_CWMPCPECO_SPV               SetParamValues;*/                                     \
    PFN_CWMPCPECO_AOS               AddObjects;                                             \
    PFN_CWMPCPECO_DOS               DeleteObjects;                                          \
    PFN_CWMPCPECO_MONITOR_OPS       MonitorOpState;                                         \
    PFN_CWMPCPECO_REG               RegisterPA;                                             \
    PFN_CWMPCPECO_SETNAME           SetPAName;                                              \
    PFN_CWMPCPECO_GETNAME           GetPAName;                                              \
    PFN_CWMPCPECO_SETNAME           SetSubsysName;                                          \
    PFN_CWMPCPECO_GETNAME           GetSubsysName;                                          \
    PFN_CWMPCPECO_SETNAME           SetCRName;                                              \
    PFN_CWMPCPECO_GETNAME           GetCRName;                                              \
    PFN_CWMPCPECO_SETNAME2          SetCRBusPath;                                           \
    PFN_CWMPCPECO_GETNAME2          GetCRBusPath;                                           \
    PFN_CWMPCPECO_SETNAME           SetPAMapperFile;                                        \
    PFN_CWMPCPECO_SETNAME           SetPACustomMapperFile;                                  \
    PFN_CWMPCPECO_SETNAME           SetSDMXmlFilename;                                      \
    PFN_CWMPCPECO_GETNAME           GetOutboundIfName;                                      \
    PFN_CWMPCPECO_SETNAME           SetOutboundIfName;                                      \
    PFN_CWMPCPECO_GETNAME           GetSDMXmlFilename;                                      \
    PFN_CWMPCPECO_GET_CONTEXT       GetMsgBusHandle;                                        \
    PFN_CWMPCPECO_GET_NOTIF         GetParamNotification;                                   \
    PFN_CWMPCPECO_SET_NOTIF         SetParamNotification;                                   \
    PFN_CWMPCPECO_GET_PTYPE         GetParamDataType;                                       \
    PFN_CWMPCPECO_SAVE_CFG_PSM      SaveCfgToPsm;                                           \
    PFN_CWMPCPECO_LOAD_CFG_PSM      LoadCfgFromPsm;                                         \
    PFN_CWMPCPECO_SET_PARAM_KEY     SetParameterKey;                                        \
    /* end of object class content */                                                       \

typedef  struct
_CCSP_CWMP_CPE_CONTROLLER_OBJECT
{
    CCSP_CWMP_CPE_CONTROLLER_CLASS_CONTENT
}
CCSP_CWMP_CPE_CONTROLLER_OBJECT,  *PCCSP_CWMP_CPE_CONTROLLER_OBJECT;

#define  ACCESS_CCSP_CWMP_CPE_CONTROLLER_OBJECT(p)       \
         ACCESS_CONTAINER(p, CCSP_CWMP_CPE_CONTROLLER_OBJECT, Linkage)


#endif
