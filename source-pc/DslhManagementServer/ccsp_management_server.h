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

    module: ccsp_management_server.h

        For CCSP management server component

    ---------------------------------------------------------------

    copyright:

        Cisco Systems, Inc.
        All Rights Reserved. 2011

    ---------------------------------------------------------------

    description:

        This file defines all data structures and macros for
        CCSP management server component.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Hui Ma 

    ---------------------------------------------------------------

    revision:

        06/15/2011    initial revision.

**********************************************************************/

#ifndef  _CCSP_MANAGEMENT_SERVER_
#define  _CCSP_MANAGEMENT_SERVER_

#include "ccsp_types.h"
#include "ccsp_custom.h"
#include <pthread.h>
#include <dbus/dbus.h>
#include <ansc_wrapper_base.h>
#include "ccsp_message_bus.h"
#include "ccsp_base_api.h"
#include "stdio.h"
#include "string.h"

// may have multiple components, so this name may be dynamic
//#define CCSP_MANAGEMENT_SERVER_COMPONENT_NAME_BASE  "com.cisco.spvtg.ccsp.managementServer"
#define CCSP_MANAGEMENT_SERVER_COMPONENT_VERSION    (int)1

#define _DeviceObjectName DM_ROOTNAME
#define _ManagementServerObjectName DM_ROOTNAME"ManagementServer."
#define _TR069NotifyObjectName DM_ROOTNAME"TR069Notify."
#define _AutonomousTransferCompletePolicyObjectName DM_ROOTNAME"ManagementServer.AutonomousTransferCompletePolicy."
#define _DUStateChangeComplPolicyObjectName DM_ROOTNAME"ManagementServer.DUStateChangeComplPolicy."
#define _ComObjectName "com."
#define _CiscoObjectName "com.cisco."
#define _SpvtgObjectName "com.cisco.spvtg."
#define _CcspObjectName "com.cisco.spvtg.ccsp."
#define _Tr069paObjectName "com.cisco.spvtg.ccsp.tr069pa."
#define _MemoryObjectName "com.cisco.spvtg.ccsp.tr069pa.Memory."
#define _LoggingObjectName "com.cisco.spvtg.ccsp.tr069pa.Logging."

#define TR69_INTERNAL_ERROR             9002
#define TR69_INVALID_ARGUMENTS          9003
#define TR69_INVALID_PARAMETER_NAME     9005
#define TR69_INVALID_PARAMETER_TYPE     9006
#define TR69_INVALID_PARAMETER_VALUE    9007
#define TR69_UPDATE_NON_WRITABLE_PARAMETER    9008

typedef struct _msParameterInfo{
    char * name; /* name without a path */
    char * value; /* always char * for all kinds of parameters */
    enum dataType_e type;
    //CCSP_BOOL writable;
    enum access_e access;
    unsigned int accessControlBitmask;
    unsigned int notification;
} msParameterInfo;

typedef struct _msObjectInfo{
    char * name; /* full name end with '.' */
    int numberOfChildObjects;
    unsigned int *childObjectIDs;
    int numberOfParameters;
    msParameterInfo *parameters;
    //CCSP_BOOL writable;
    enum access_e access;
} msObjectInfo;

/* each object has a fixed array ID. */
#define DeviceID                                0
#define ManagementServerID                      1
#define DeviceInfoID                            2
#define AutonomousTransferCompletePolicyID      3
#define DUStateChangeComplPolicyID              4
#define ComID                                   5
#define CiscoID                                 6
#define SpvtgID                                 7
#define CcspID                                  8
#define Tr069paID                               9
#define MemoryID                                10
#define LoggingID                               11

#define ServicesID                              12
#define STBServiceID                            13
#define STBService1ID                           14
#define X_CISCO_COM_DiagID                      15
#define CWMPID                                  16
#define NotifyID                                17
/*  Place holder for STBService.{i}. */
#define SupportedDataModelID                    18    /* This has to be the last one since multiple instances are added based on it. */


/* each parameter has a fixed array ID. */
enum
{
    ManagementServerEnableCWMPID = 0,
    ManagementServerURLID,
    ManagementServerUsernameID,
    ManagementServerPasswordID,
    ManagementServerPeriodicInformEnableID,
    ManagementServerPeriodicInformIntervalID,
    ManagementServerPeriodicInformTimeID,
    ManagementServerParameterKeyID,
    ManagementServerConnectionRequestURLID,
    ManagementServerConnectionRequestUsernameID,
    ManagementServerConnectionRequestPasswordID,
    ManagementServerACSOverrideID,
    ManagementServerUpgradesManagedID,
    ManagementServerX_CISCO_COM_DiagCompleteID,
    ManagementServerKickURLID,
    ManagementServerDownloadProgressURLID,
    ManagementServerDefaultActiveNotificationThrottleID,
    ManagementServerCWMPRetryMinimumWaitIntervalID,
    ManagementServerCWMPRetryIntervalMultiplierID,
    ManagementServerUDPConnectionRequestAddressID,
    ManagementServerUDPConnectionRequestAddressNotificationLimitID,
    ManagementServerSTUNEnableID,
    ManagementServerSTUNServerAddressID,
    ManagementServerSTUNServerPortID,
    ManagementServerSTUNUsernameID,
    ManagementServerSTUNPasswordID,
    ManagementServerSTUNMaximumKeepAlivePeriodID,
    ManagementServerSTUNMinimumKeepAlivePeriodID,
    ManagementServerNATDetectedID,
    ManagementServerAliasBasedAddressingID,
    ManagementServerX_CISCO_COM_ConnectionRequestURLPortID,
    ManagementServerX_CISCO_COM_ConnectionRequestURLPathID,
    ManagementServerNumOfParameters
};
#if 0
//Not used anymore
//#ifndef _COSA_VEN501_
/*for compilation, don't worry for crash, these parameters are not registered.*/
enum
{
    ManagementServerKickURLID = 1000,
    ManagementServerDownloadProgressURLID,
    ManagementServerDefaultActiveNotificationThrottleID,
    ManagementServerCWMPRetryMinimumWaitIntervalID,
    ManagementServerCWMPRetryIntervalMultiplierID,
    ManagementServerX_CISCO_COM_ConnectionRequestURLPortID,
    ManagementServerX_CISCO_COM_ConnectionRequestURLPathID,
};
#endif

#define AutonomousTransferCompletePolicyEnableID                0
#define AutonomousTransferCompletePolicyTransferTypeFilterID    1
#define AutonomousTransferCompletePolicyResultTypeFilterID      2
#define AutonomousTransferCompletePolicyFileTypeFilterID        3
#define AutonomousTransferCompletePolicyNumOfParameters         4

#define DUStateChangeComplPolicyEnableID                0
#define DUStateChangeComplPolicyOperationTypeFilterID   1             
#define DUStateChangeComplPolicyResultTypeFilterID      2          
#define DUStateChangeComplPolicyFaultCodeFilterID       3         
#define DUStateChangeComplPolicyNumOfParameters         4

#define Tr069paNameID           0
#define Tr069paVersionID        1
#define Tr069paAuthorID         2
#define Tr069paHealthID         3
#define Tr069paStateID          4
#define Tr069paDTXmlID          5
#define Tr069paNumOfParameters  6

#define MemoryMinUsageID        0
#define MemoryMaxUsageID        1
#define MemoryConsumedID        2
#define MemoryNumOfParameters   3

#define LoggingEnableID         0
#define LoggingLogLevelID       1
#define LoggingNumOfParameters  2

#define TR069Notify_TR069_Notification_ID       0
#define TR069Notify_Connected_ClientID 		    1
#define TR069NotifyNumOfParameters 				2

#define FALSE 0
#define TRUE  1

/* for msParameterValSetting.backupStatus */
#define NoBackup       0
#define BackupNewValue 1
#define BackupOldValue 2

typedef struct _msParameterValSetting
{
    int objectID;
    int parameterID;
    char *parameterValue;
    int type;
    unsigned char backupStatus;  /* cross compiler may set char as "unsigned char", so negative values cannot be assigned. */
} msParameterValSetting;

#define  CCSP_TR069PA_DFT_PARAM_VAL_SETTINGS_NUMBER     32

typedef struct _msParameterValSettingArray
{
    int                             sessionID;
    unsigned int                    writeID;
    int                             size;
    unsigned int                    currIndex;
    msParameterValSetting *         msParameterValSettings;
} msParameterValSettingArray;

/* Called by init function to fill in parameter values from PSM.
 */
CCSP_VOID CcspManagementServer_FillInObjectInfo();
CCSP_VOID CcspManagementServer_InitDBus();
ANSC_STATUS CcspManagementServer_RegisterNameSpace();
ANSC_STATUS CcspManagementServer_DiscoverComponent();
ANSC_STATUS CcspManagementServer_RegisterWanInterface();
ANSC_STATUS CcspManagementServer_GenerateConnectionRequestURL(
    BOOL fromValueChangeSignal,
    char *newValue);
int SendValueChangeSignal(
    int objectID, 
    int parameterID,
    const char * oldValue);

//#define CcspManagementServer_Allocate(size) AnscAllocateMemoryCountSize(CcspManagementServer_ComponentName, size)
//#define CcspManagementServer_Free(p) AnscFreeMemoryCountSize(CcspManagementServer_ComponentName, p)

void CcspManagementServer_GetSingleParameterValue(
        int objectID,
        int parameterID,
        parameterValStruct_t *val
    );

/* CcspManagementServer_ValidateParameterValues is called to validate
 * prameters that are sent for updating.
 */
int CcspManagementServer_ValidateParameterValues(
    int sessionId,
    unsigned int writeID,
    parameterValStruct_t *val,
    int size,
    char ** invalidParameterName
    );

/* Get the number of all parameters of this object and its children.
*/
int CcspManagementServer_GetParameterCount(
    int objectID
    );

/* Set single parameter value.
*/
void CcspManagementServer_SetSingleParameterValue(
    int sessionId,
    int objectID,
    int parameterID,
    parameterAttributeStruct_t *val
    );

/* Set single object attributes. 
 * All the parameters with this prefix are set here.
*/
int CcspManagementServer_SetSingleObjectAttributes(
    int sessionId,
    int objectID,
    parameterAttributeStruct_t *val
    );

/* For each parameter, add one record item to attr.
 * Return how many records have been added. 
 */
int CcspManagementServer_GetSingleObjectAttributes(
    int objectID,
    parameterAttributeStruct_t **attr,
    int beginAttrID
    );

/* Get attribute of one parameter. 
 */
void CcspManagementServer_GetSingleParameterAttributes(
    int objectID,
    int parameterID,
    parameterAttributeStruct_t *attr
    );

/* Commit the parameter setting stored in parameterSetting.
 */
int CcspManagementServer_CommitParameterValues(unsigned int writeID);

/* Roll back the parameters already being committed.
 */
int CcspManagementServer_RollBackParameterValues();

/* The parameter setting values are stored in ParameterSetting before commit.
 * After commit or cancel, ParameterSetting is freed by this function.
 */
int CcspManagementServer_FreeParameterSetting();

/* There is dismatch between objectID here and objectID in PA. */
int CcspManagementServer_GetPAObjectID
(
    int objectID
);

char * CcspManagementServer_RemoveWhiteString
(
    const char * src
);

void * CcspManagementServer_Allocate
(
    size_t size
);

void CcspManagementServer_Free
(
    void *p
);

/* Malloc memory that will be freed by other modules.
  */
char * CcspManagementServer_CloneString
    (
    const char * src
    );

/* Malloc memory that will be freed by itself.
 */
char * CcspManagementServer_CloneStringInComponent
    (
    const char * src
    );

/* Malloc memory that will be freed by other modules.
  */
char * CcspManagementServer_MergeString
    (
    const char * src1,
    const char * src2
    );

/* Malloc memory that will be freed by itself.
 */
char * CcspManagementServer_MergeStringInComponent
    (
    const char * src1,
    const char * src2
    );
    
ANSC_STATUS
CcspManagementServer_RestoreDefaultValues
    (
        void
    );

#endif
