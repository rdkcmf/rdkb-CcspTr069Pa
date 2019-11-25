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

    module: ccsp_management_server.c

        For CCSP management server component

    ---------------------------------------------------------------

    description:

        This file implements all internal functions of
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

#include "ansc_platform.h"
#include "ccsp_memory.h"
#include "ccsp_custom.h"
#include "ccsp_types.h"
#include "ccsp_trace.h"
#include "ccsp_tr069pa_wrapper_api.h"
#include "ccsp_management_server.h"
#include "ccsp_management_server_api.h"
#include "ccsp_management_server_pa_api.h"
#include "ccsp_supported_data_model.h"
#include "slap_definitions.h"
#include "ccsp_psm_helper.h"
#include "ansc_string.h"
#include "stdio.h"
#include "dslh_definitions_database.h"
#include "secure_wrapper.h"
#include "slap_vco_internal_api.h"

// TELEMETRY 2.0 //RDKB-25996
#include <telemetry_busmessage_sender.h>

#define TEMP_SIZE 23
#define MAX_SIZE_TMPPWD_VALUE 512

char *CcspManagementServer_ComponentName = NULL;
char *CcspManagementServer_SubsystemPrefix = NULL;
msParameterValSettingArray parameterSetting = {0};
name_spaceType_t *CcspManagementServer_Namespace;
/* Make sure that CcspManagementServer_RegisterWanInterface is successfully done. */
BOOL RegisterWanInterfaceDone = FALSE;  
extern PFN_CCSPMS_VALUECHANGE CcspManagementServer_ValueChangeCB;
extern CCSP_HANDLE            CcspManagementServer_cbContext;
extern int                    sdmObjectNumber;
extern INT                    g_iTraceLevel;

/* for ManagementServer. ManagementServer.AutonomousTransferCompletePolicy.
 * ManagementServer.DUStateChangeComplPolicy.
 */
msObjectInfo *objectInfo;
//char * objectManagementServerName = _ManagementServerObjectName;
void *bus_handle = NULL;
char * CCSP_DBUS_PATH_MS = "/com/cisco/spvtg/ccsp/MS";
char * CCSP_DBUS_PATH_PA = "/com/cisco/stbservice/protocolagent";
char *pPAMComponentName = NULL;
char *pPAMComponentPath = NULL;
#define FirstUpstreamIpInterfaceParameterName "com.cisco.spvtg.ccsp.pam.Helper.FirstUpstreamIpInterface"

char *pFirstUpstreamIpInterface = NULL;
char *pFirstUpstreamIpAddress = NULL;

int g_ACSChangedURL = 0;


char * pDTXml = "<?xml version=\"1.0\"  encoding=\"UTF-8\" ?>\
<DT>\
  <object ref=\""DM_ROOTNAME"ManagementServer.\" access=\"readOnly\" minEntries=\"1\" maxEntries=\"1\">\
    <parameter ref=\"EnableCWMP\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>boolean</syntax>\
    </parameter>\
    <parameter ref=\"URL\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"Username\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"Password\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"PeriodicInformEnable\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>boolean</syntax>\
    </parameter>\
    <parameter ref=\"PeriodicInformInterval\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>unsignedInt</syntax>\
    </parameter>\
    <parameter ref=\"PeriodicInformTime\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>dateTime</syntax>\
    </parameter>\
    <parameter ref=\"ParameterKey\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"ConnectionRequestURL\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"ConnectionRequestUsername\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"ConnectionRequestPassword\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"UpgradesManaged\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>boolean</syntax>\
    </parameter>\
    <parameter ref=\"KickURL\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"DownloadProgressURL\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"ManageableDeviceNumberOfEntries\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>unsignedInt</syntax>\
    </parameter>\
    <parameter ref=\"ManageableDeviceNotificationLimit\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>unsignedInt</syntax>\
    </parameter>\
    <parameter ref=\"X_CISCO_COM_LastConReqReceived\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>dateTime</syntax>\
    </parameter>\
    <parameter ref=\"X_CISCO_COM_LastCWMPInformTime\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>dateTime</syntax>\
    </parameter>\
    <parameter ref=\"X_CISCO_COM_LastCWMPInformStatus\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"X_CISCO_COM_SucceededCWMPInforms\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>unsignedInt</syntax>\
    </parameter>\
    <parameter ref=\"X_CISCO_COM_FailedCWMPInforms\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>unsignedInt</syntax>\
    </parameter>\
    <parameter ref=\"X_CISCO_COM_LastValueChangeTime\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>dateTime</syntax>\
    </parameter>\
    <parameter ref=\"X_CISCO_COM_LastValueChangeParam\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"X_CISCO_COM_DiagComplete\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>boolean</syntax>\
    </parameter>\
  </object>\
  <object ref=\""DM_ROOTNAME"ManagementServer.ManageableDevice.{i}.\" access=\"readOnly\" minEntries=\"0\" maxEntries=\"128\">\
    <parameter ref=\"ManufacturerOUI\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"SerialNumber\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"ProductClass\" access=\"readOnly\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
  </object>\
  <object ref=\""DM_ROOTNAME"Time.\" access=\"readOnly\" minEntries=\"1\" maxEntries=\"1\">\
    <parameter ref=\"NTPServer1\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
    </parameter>\
    <parameter ref=\"NTPServer2\" access=\"readWrite\" activeNotify=\"normal\">\
      <syntax>string</syntax>\
</parameter>\
</object>\
</DT>";

/* The program will crash if try to free the parameter values directly initialized here. */
msParameterInfo managementServerParameters[] = 
{
    { "EnableCWMP", NULL, ccsp_boolean, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "URL", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "Username", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "Password", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "PeriodicInformEnable", NULL, ccsp_boolean, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "PeriodicInformInterval", NULL, ccsp_unsignedInt, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "PeriodicInformTime", NULL, ccsp_dateTime, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "ParameterKey", NULL, ccsp_string, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "ConnectionRequestURL", NULL, ccsp_string, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "ConnectionRequestUsername", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "ConnectionRequestPassword", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "ACSOverride", NULL, ccsp_boolean, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "UpgradesManaged", NULL, ccsp_boolean, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "X_CISCO_COM_DiagComplete", NULL, ccsp_boolean, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
// #if 0 //Not used anymore
#ifndef _COSA_VEN501_
    { "KickURL", NULL, ccsp_string, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "DownloadProgressURL", NULL, ccsp_string, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "DefaultActiveNotificationThrottle", NULL, ccsp_unsignedInt, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "CWMPRetryMinimumWaitInterval", NULL, ccsp_unsignedInt, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "CWMPRetryIntervalMultiplier", NULL, ccsp_unsignedInt, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
#endif
    { "UDPConnectionRequestAddress", NULL, ccsp_string, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "UDPConnectionRequestAddressNotificationLimit", NULL, ccsp_unsignedInt, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "STUNEnable", NULL, ccsp_boolean, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "STUNServerAddress", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "STUNServerPort", NULL, ccsp_unsignedInt, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "STUNUsername", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "STUNPassword", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "STUNMaximumKeepAlivePeriod", NULL, ccsp_int, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "STUNMinimumKeepAlivePeriod", NULL, ccsp_unsignedInt, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "NATDetected", NULL, ccsp_boolean, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "AliasBasedAddressing", NULL, ccsp_boolean, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
// #if 0 //Not used anymore
#ifndef _COSA_VEN501_
    { "X_CISCO_COM_ConnectionRequestURLPort", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "X_CISCO_COM_ConnectionRequestURLPath", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 }
#endif
};

msParameterInfo autonomousTransferCompletePolicyParameters[] = 
{
    { "Enable", NULL, ccsp_boolean, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "TransferTypeFilter", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "ResultTypeFilter", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "FileTypeFilter", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 }
};

msParameterInfo duStateChangeComplPolicyParameters[] = 
{
    { "Enable", NULL, ccsp_boolean, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "OperationTypeFilter", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "ResultTypeFilter", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "FaultCodeFilter", NULL, ccsp_string, CCSP_RW, ~((unsigned int)0), (unsigned int)0 }
};

msParameterInfo tr069paParameters[] = 
{
    { "Name", NULL, ccsp_string, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "Version", NULL, ccsp_string, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "Author", NULL, ccsp_string, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "Health", NULL, ccsp_string, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "State", NULL, ccsp_string, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "DTXml", NULL, ccsp_string, CCSP_RO, ~((unsigned int)0), (unsigned int)0 }
};

msParameterInfo memoryParameters[] = 
{
    { "MinUsage", NULL, ccsp_unsignedInt, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "MaxUsage", NULL, ccsp_unsignedInt, CCSP_RO, ~((unsigned int)0), (unsigned int)0 },
    { "Consumed", NULL, ccsp_unsignedInt, CCSP_RO, ~((unsigned int)0), (unsigned int)0 }
};

msParameterInfo loggingParameters[] = 
{
    { "Enable", NULL, ccsp_boolean, CCSP_RW, ~((unsigned int)0), (unsigned int)0 },
    { "LogLevel", NULL, ccsp_unsignedInt, CCSP_RW, ~((unsigned int)0), (unsigned int)0 }
};

extern CCSP_VOID
CcspManagementServer_FillInObjectInfoCustom(msObjectInfo *objectInfo);

CCSP_VOID
CcspManagementServer_FillInObjectInfo()
{
    errno_t rc = -1;
    //CcspTraceWarning("ms", ( "CcspManagementServer_FillObjectInfo 0\n"));
    CcspManagementServer_FillInSDMObjectInfo();
    /* First setup default object array. */
    objectInfo[DeviceID].name = CcspManagementServer_CloneString(_DeviceObjectName);

    objectInfo[DeviceID].numberOfChildObjects = 2;
    objectInfo[DeviceID].childObjectIDs =
        CcspManagementServer_Allocate(objectInfo[DeviceID].numberOfChildObjects * sizeof(unsigned int));
    objectInfo[DeviceID].childObjectIDs[0] = ManagementServerID;
    objectInfo[DeviceID].childObjectIDs[1] = DeviceInfoID;
    objectInfo[DeviceID].numberOfParameters = 0;
    objectInfo[DeviceID].parameters = NULL;

    objectInfo[ManagementServerID].name = CcspManagementServer_CloneString(_ManagementServerObjectName);
// #if 0 //Not used anymore
#ifndef _COSA_VEN501_
    objectInfo[ManagementServerID].numberOfChildObjects = 2;
    objectInfo[ManagementServerID].childObjectIDs = 
        CcspManagementServer_Allocate(objectInfo[ManagementServerID].numberOfChildObjects * sizeof(unsigned int));
    objectInfo[ManagementServerID].childObjectIDs[0] = AutonomousTransferCompletePolicyID;
    objectInfo[ManagementServerID].childObjectIDs[1] = DUStateChangeComplPolicyID;
#else
    objectInfo[ManagementServerID].numberOfChildObjects = 0;
#endif
    objectInfo[ManagementServerID].numberOfParameters = ManagementServerNumOfParameters;
    objectInfo[ManagementServerID].parameters = managementServerParameters;

    objectInfo[DeviceInfoID].name = CcspManagementServer_CloneString(_DeviceInfoObjectName);
    objectInfo[DeviceInfoID].numberOfChildObjects = 1;
    objectInfo[DeviceInfoID].childObjectIDs = 
        CcspManagementServer_Allocate(objectInfo[DeviceInfoID].numberOfChildObjects * sizeof(unsigned int));
    objectInfo[DeviceInfoID].childObjectIDs[0] = SupportedDataModelID;
    objectInfo[DeviceInfoID].numberOfParameters = 0;
    objectInfo[DeviceInfoID].parameters = NULL;

// #if 0 //Not used anymore
#ifndef _COSA_VEN501_    
    objectInfo[AutonomousTransferCompletePolicyID].name = CcspManagementServer_CloneString(_AutonomousTransferCompletePolicyObjectName);
    objectInfo[AutonomousTransferCompletePolicyID].numberOfChildObjects = 0;
    objectInfo[AutonomousTransferCompletePolicyID].numberOfParameters = AutonomousTransferCompletePolicyNumOfParameters;
    objectInfo[AutonomousTransferCompletePolicyID].parameters = autonomousTransferCompletePolicyParameters;
    
    objectInfo[DUStateChangeComplPolicyID].name = CcspManagementServer_CloneString(_DUStateChangeComplPolicyObjectName);
    objectInfo[DUStateChangeComplPolicyID].numberOfChildObjects = 0;
    objectInfo[DUStateChangeComplPolicyID].numberOfParameters = DUStateChangeComplPolicyNumOfParameters;
    objectInfo[DUStateChangeComplPolicyID].parameters = duStateChangeComplPolicyParameters;
#endif

    objectInfo[ComID].name = CcspManagementServer_CloneString(_ComObjectName);
    objectInfo[ComID].numberOfChildObjects = 1;
    objectInfo[ComID].childObjectIDs = 
        CcspManagementServer_Allocate(objectInfo[ComID].numberOfChildObjects * sizeof(unsigned int));
    objectInfo[ComID].childObjectIDs[0] = CiscoID;
    objectInfo[ComID].numberOfParameters = 0;
    objectInfo[ComID].parameters = NULL;

    objectInfo[CiscoID].name = CcspManagementServer_CloneString(_CiscoObjectName);
    objectInfo[CiscoID].numberOfChildObjects = 1;
    objectInfo[CiscoID].childObjectIDs = 
        CcspManagementServer_Allocate(objectInfo[CiscoID].numberOfChildObjects * sizeof(unsigned int));
    objectInfo[CiscoID].childObjectIDs[0] = SpvtgID;
    objectInfo[CiscoID].numberOfParameters = 0;
    objectInfo[CiscoID].parameters = NULL;

    objectInfo[SpvtgID].name = CcspManagementServer_CloneString(_SpvtgObjectName);
    objectInfo[SpvtgID].numberOfChildObjects = 1;
    objectInfo[SpvtgID].childObjectIDs = 
        CcspManagementServer_Allocate(objectInfo[SpvtgID].numberOfChildObjects * sizeof(unsigned int));
    objectInfo[SpvtgID].childObjectIDs[0] = CcspID;
    objectInfo[SpvtgID].numberOfParameters = 0;
    objectInfo[SpvtgID].parameters = NULL;

    objectInfo[CcspID].name = CcspManagementServer_CloneString(_CcspObjectName);
    objectInfo[CcspID].numberOfChildObjects = 1;
    objectInfo[CcspID].childObjectIDs = 
        CcspManagementServer_Allocate(objectInfo[CcspID].numberOfChildObjects * sizeof(unsigned int));
    objectInfo[CcspID].childObjectIDs[0] = Tr069paID;
    objectInfo[CcspID].numberOfParameters = 0;
    objectInfo[CcspID].parameters = NULL;

    objectInfo[Tr069paID].name = CcspManagementServer_CloneString(_Tr069paObjectName);
    objectInfo[Tr069paID].numberOfChildObjects = 2;
    objectInfo[Tr069paID].childObjectIDs = 
        CcspManagementServer_Allocate(objectInfo[Tr069paID].numberOfChildObjects * sizeof(unsigned int));
    objectInfo[Tr069paID].childObjectIDs[0] = MemoryID;
    objectInfo[Tr069paID].childObjectIDs[1] = LoggingID;
    objectInfo[Tr069paID].numberOfParameters = Tr069paNumOfParameters;
    objectInfo[Tr069paID].parameters = tr069paParameters;
    objectInfo[Tr069paID].parameters[Tr069paNameID].value = CcspManagementServer_CloneString("com.cisco.spvtg.ccsp.tr069pa");
    objectInfo[Tr069paID].parameters[Tr069paVersionID].value = CcspManagementServer_CloneString("1.0");
    objectInfo[Tr069paID].parameters[Tr069paAuthorID].value = CcspManagementServer_CloneString("CCSP Team");
    objectInfo[Tr069paID].parameters[Tr069paHealthID].value = CcspManagementServer_CloneString("Green");
    objectInfo[Tr069paID].parameters[Tr069paStateID].value = CcspManagementServer_CloneString("2");
    objectInfo[Tr069paID].parameters[Tr069paDTXmlID].value = CcspManagementServer_CloneString(pDTXml);

    objectInfo[MemoryID].name = CcspManagementServer_CloneString(_MemoryObjectName);
    objectInfo[MemoryID].numberOfChildObjects = 0;
    objectInfo[MemoryID].numberOfParameters = MemoryNumOfParameters;
    objectInfo[MemoryID].parameters = memoryParameters;

    objectInfo[LoggingID].name = CcspManagementServer_CloneString(_LoggingObjectName);
    objectInfo[LoggingID].numberOfChildObjects = 0;
    objectInfo[LoggingID].numberOfParameters = LoggingNumOfParameters;
    objectInfo[LoggingID].parameters = loggingParameters;
    objectInfo[LoggingID].parameters[LoggingEnableID].value = CcspManagementServer_CloneString("true");
    char str[100] = {0};
    //_ansc_itoa(g_iTraceLevel, str, 10);
    _ansc_sprintf(str, "%d", g_iTraceLevel);
    objectInfo[LoggingID].parameters[LoggingLogLevelID].value = CcspManagementServer_CloneString(str);

    /* set up default values of Management Server settings, another way to do this is
       through PSM, however on different sub-system, the record names must be
       prefixed with sub-system prefix. If possible, we set default values in code
       to avoid troubles of maitaining default configuration files on different
       sub-systems.
     */
    if ( TRUE )
    {
        char    buf[16];
   
        objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value
            = CcspManagementServer_CloneString("0");

        // _ansc_sprintf(buf, "%d", 51005);
        
        _ansc_sprintf(buf, "%d", CWMP_PORT);      

        objectInfo[ManagementServerID].parameters[ManagementServerX_CISCO_COM_ConnectionRequestURLPortID].value 
            = CcspManagementServer_CloneString(buf);

        /* Now PA is started before PAM. We can't get it now We will try to get from PAM directly when do INFORM */
        objectInfo[ManagementServerID].parameters[ManagementServerX_CISCO_COM_ConnectionRequestURLPathID].value 
            = NULL;
    
        objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].notification = 1;
            
        _ansc_sprintf(buf, "%d", STUN_PORT);
        objectInfo[ManagementServerID].parameters[ManagementServerSTUNServerPortID].value 
            = CcspManagementServer_CloneString(buf);

        objectInfo[ManagementServerID].parameters[ManagementServerSTUNEnableID].value
            = CcspManagementServer_CloneString("0");

        objectInfo[ManagementServerID].parameters[ManagementServerSTUNMinimumKeepAlivePeriodID].value 
            = CcspManagementServer_CloneString("5");

        objectInfo[ManagementServerID].parameters[ManagementServerSTUNMaximumKeepAlivePeriodID].value 
            = CcspManagementServer_CloneString("30");

        objectInfo[ManagementServerID].parameters[ManagementServerUDPConnectionRequestAddressNotificationLimitID].value 
            = CcspManagementServer_CloneString("0");
        objectInfo[ManagementServerID].parameters[ManagementServerX_CISCO_COM_DiagCompleteID].value
            = CcspManagementServer_CloneString("0");

    }


    /* Then read values from PSM. */
    /* The program will crash if try to free the parameter values directly assigned by initialization. */
    int i, j, res;
    char pRecordName[1000] = {0};

    size_t len1 = strlen(CcspManagementServer_ComponentName);
    size_t len2, len3;
    char* pValue = NULL;
    rc = strncpy_s(pRecordName, sizeof(pRecordName), CcspManagementServer_ComponentName, len1);
    ERR_CHK(rc);
    pRecordName[len1] = '.';
// #if 0 //Not used anymore_
#ifndef _COSA_VEN501_
    for(i = ManagementServerID; i<=DUStateChangeComplPolicyID; i++){ /* Assume no persistent state for com. objects. */
#else
        for(i = ManagementServerID; i<= DeviceInfoID; i++){ /* Assume no persistent state for com. objects. */
#endif
        len2 = strlen(objectInfo[i].name);
        rc = strncat_s(pRecordName, sizeof(pRecordName), objectInfo[i].name, len2);
        ERR_CHK(rc);
        for(j=0; j<objectInfo[i].numberOfParameters; j++){
            len3 = strlen(objectInfo[i].parameters[j].name);
            rc = strncpy_s(&pRecordName[len1+len2+1], sizeof(pRecordName)-len1-len2, objectInfo[i].parameters[j].name, len3);
            ERR_CHK(rc);
            rc = strncpy_s(&pRecordName[len1+len2+len3+1], sizeof(pRecordName)-len1-len2-len3, ".Value", strlen(".Value"));
            ERR_CHK(rc);
             /* If SAFEC_DUMMY_API is enabled, then the redirected API strncpy should be null terminated */
            #ifdef SAFEC_DUMMY_API
                pRecordName[len1+len2+len3+7] = '\0';
            #endif

            res = PSM_Get_Record_Value2(
                bus_handle,
                CcspManagementServer_SubsystemPrefix,
                pRecordName,
                NULL,
                &pValue);


            if(res == CCSP_SUCCESS){
                CcspTraceDebug2
                    (
                        "ms",
                        ("PSM_Get_Record_Value2 returns %d, name=<%s>, value=<%s>\n", res, pRecordName, pValue ? pValue : "NULL")
                    );
        
                /* free default value */
                if ( objectInfo[i].parameters[j].value )
                {
                    CcspManagementServer_Free(objectInfo[i].parameters[j].value);
                }

                objectInfo[i].parameters[j].value = pValue;
				pValue = NULL;

				// Needs to be encrypt on NVMEM files during migration case
				if ( 	  ( ManagementServerID == i ) && \
					  ( ( ManagementServerPasswordID == j ) || \
						( ManagementServerConnectionRequestPasswordID == j ) || \
						( ManagementServerSTUNPasswordID == j )
					  ) 
					)
				{
					int IsEncryptFileAvailable = 0;

					// Check whether already encrypted file is available or not
					if ( 0 == CcspManagementServer_IsEncryptedFileInDB( j, &IsEncryptFileAvailable 	) )
					{
						// if encrypted file is not available then only we have to encrypt
						if( 0 == IsEncryptFileAvailable )
						{
							CcspManagementServer_StoreMGMTServerPasswordValuesintoDB( objectInfo[i].parameters[j].value,
																					  j );
						}
					}

					//Delete old existing entries from PSM DB
					PSM_Del_Record( bus_handle, CcspManagementServer_SubsystemPrefix, pRecordName );
				}
            }
			else
			{
				// Needs to be decrypt from NVMEM files
				if ( 	  ( ManagementServerID == i ) && \
					  ( ( ManagementServerPasswordID == j ) || \
						( ManagementServerConnectionRequestPasswordID == j ) || \
						( ManagementServerSTUNPasswordID == j )
					  ) 
					)
				{
					char tmpPWDValue[MAX_SIZE_TMPPWD_VALUE]  = { 0 };

					// If  return success then process otherwise leave it as it is
					if ( 0 == CcspManagementServer_GetMGMTServerPasswordValuesFromDB( j, tmpPWDValue ))
					{
						if( '\0' != tmpPWDValue[ 0 ] )
						{
							/* free default value */
							if ( objectInfo[i].parameters[j].value )
							{
								CcspManagementServer_Free(objectInfo[i].parameters[j].value);
							}
						
							objectInfo[i].parameters[j].value = CcspManagementServer_Allocate( strlen( tmpPWDValue ) + 1 );
                                                        rc = strncpy_s(objectInfo[i].parameters[j].value, strlen(tmpPWDValue) + 1, tmpPWDValue, strlen( tmpPWDValue ) ) ;
                                                        ERR_CHK(rc);
						}
					}
				}
			}

            rc = strncpy_s(&pRecordName[len1+len2+len3+1], sizeof(pRecordName)-len1-len2-len3, ".Notification", 13);
            ERR_CHK(rc);
             /* If SAFEC_DUMMY_API is enabled, then the redirected API strncpy should be null terminated */
            #ifdef SAFEC_DUMMY_API
                pRecordName[len1+len2+len3+14] = '\0';
            #endif

            res = PSM_Get_Record_Value2(
                bus_handle,
                CcspManagementServer_SubsystemPrefix,
                pRecordName,
                NULL,
                &pValue);


            if(res == CCSP_SUCCESS){
                CcspTraceDebug2
                    (
                        "ms",
                        (
                        "PSM_Get_Record_Value2 returns %d, name=<%s>, value=<%s>\n",
                        res,
                        pRecordName,
                        pValue ? pValue : "NULL"
                        )
                    );
                /*RDKB-7329, CID-33414, null check before use */
                if(pValue){
                    objectInfo[i].parameters[j].notification = _ansc_atoi(pValue);
                    CcspManagementServer_Free(pValue);
                }
                else{
                    objectInfo[i].parameters[j].notification = 0;
                }
                pValue = NULL;
            }
        }
    }

    
    CcspManagementServer_FillInObjectInfoCustom(objectInfo);

}

DBusHandlerResult
CcspManagementServer_path_message_func (DBusConnection  *conn,
                   DBusMessage     *message,
                   void            *user_data)
{
    CCSP_MESSAGE_BUS_INFO *bus_info =(CCSP_MESSAGE_BUS_INFO *) user_data;
    const char *interface = dbus_message_get_interface(message);
    const char *method   = dbus_message_get_member(message);
    DBusMessage *reply;
    reply = dbus_message_new_method_return (message);
    if (reply == NULL)
    {
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    return CcspBaseIf_base_path_message_func (conn,
            message,
            reply,
            interface,
            method,
            bus_info);

}

int getHealth()
{
    extern int g_tr069Health;
    return g_tr069Health;
}

CCSP_VOID CcspManagementServer_InitDBus()
{
    int ret ;
    int times = 0;
    int success = 0;
    errno_t rc = -1;

    if ( !bus_handle )
    {
#ifdef DBUS_INIT_SYNC_MODE
        CCSP_Message_Bus_Init_Synced(CcspManagementServer_ComponentName, CCSP_MSG_BUS_CFG, &bus_handle, Ansc_AllocateMemory_Callback, Ansc_FreeMemory_Callback);
#else
        CCSP_Message_Bus_Init(CcspManagementServer_ComponentName, CCSP_MSG_BUS_CFG, &bus_handle, (CCSP_MESSAGE_BUS_MALLOC)Ansc_AllocateMemory_Callback, Ansc_FreeMemory_Callback);
#endif
        CCSP_Msg_SleepInMilliSeconds(1000); 
    }

    while(times++ < DBUS_REGISTER_PATH_TIMES)
    {
        ret = CCSP_Message_Bus_Register_Path2(bus_handle, CCSP_DBUS_PATH_MS, CcspManagementServer_path_message_func, bus_handle);
        if (ret != CCSP_Message_Bus_OK)
            CCSP_Msg_SleepInMilliSeconds(1000);
        else 
        {
            success = 1;
            break;
        }
    }

    if (!success)
        AnscTraceWarning(("%s, Register Path failure !!!\n", __FUNCTION__));

    CCSP_Base_Func_CB  fccb ;
    CCSP_Base_Func_CB *cb = &fccb;
    rc = memset_s(cb,sizeof(CCSP_Base_Func_CB), 0,sizeof(CCSP_Base_Func_CB));
    ERR_CHK(rc);
    cb->setParameterValues  = CcspManagementServer_SetParameterValues;
    cb->setCommit  = CcspManagementServer_SetCommit;
    cb->getParameterValues  = CcspManagementServer_GetParameterValues;
    cb->setParameterAttributes  = CcspManagementServer_SetParameterAttributes;
    cb->getParameterAttributes  = CcspManagementServer_GetParameterAttributes;
    cb->AddTblRow  = CcspManagementServer_AddTblRow;
    cb->DeleteTblRow  = (CCSPBASEIF_DELETETBLROW )CcspManagementServer_DeleteTblRow;
    cb->getParameterNames  = (CCSPBASEIF_GETPARAMETERNAMES )CcspManagementServer_GetParameterNames;
    CcspBaseIf_SetCallback (bus_handle,  cb);
    
    //Custom
    CcspManagementServer_InitDBusCustom(cb);
}

ANSC_STATUS CcspManagementServer_RegisterNameSpace()
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    int         namespaceNumber = 0;
    int         i, j;
    int         index = 0;
    char CrName[256] = {0};

    if ( CcspManagementServer_SubsystemPrefix)
    {
        _ansc_sprintf(CrName, "%s%s", CcspManagementServer_SubsystemPrefix, CCSP_DBUS_INTERFACE_CR);
    }
    else
    {
        _ansc_sprintf(CrName, "%s", CCSP_DBUS_INTERFACE_CR);
    }

    for(i=0; i<SupportedDataModelID; i++)  
    {
        namespaceNumber += objectInfo[i].numberOfParameters;
    }
    /* For SupportedDataModel, the parameter number is fixed here. */
    namespaceNumber += 3;

    CcspManagementServer_Namespace = (name_spaceType_t *)CcspManagementServer_Allocate(namespaceNumber * sizeof(name_spaceType_t));
    for(i=0; i<SupportedDataModelID; i++)
    {
        for(j=0; j<objectInfo[i].numberOfParameters; j++)
        {
            CcspManagementServer_Namespace[index].name_space = CcspManagementServer_MergeString(objectInfo[i].name, objectInfo[i].parameters[j].name);
            CcspManagementServer_Namespace[index].dataType = objectInfo[i].parameters[j].type;
            index++;
        }
    }
    for(j=0; j<3; j++)
    {
        CcspManagementServer_Namespace[index+j].dataType = ccsp_string;
    }
    CcspManagementServer_Namespace[index].name_space = CcspManagementServer_MergeString(_SupportedDataModelTableName, "URL");
    CcspManagementServer_Namespace[index+1].name_space = CcspManagementServer_MergeString(_SupportedDataModelTableName, "URN");
    CcspManagementServer_Namespace[index+2].name_space = CcspManagementServer_MergeString(_SupportedDataModelTableName, "Features");

#ifdef   _DEBUG
    CcspTraceDebug2("ms", ("registering following namespaces into CR with prefix=%s ...\n", CcspManagementServer_SubsystemPrefix));
    for (i=0;i<namespaceNumber;i++)
    {
        CcspTraceDebug2("ms", ("  %s, type=%d\n", CcspManagementServer_Namespace[i].name_space, CcspManagementServer_Namespace[i].dataType));
    }
#endif

    returnStatus =
        CcspBaseIf_registerCapabilities
            (
                bus_handle,
                CrName,
                CcspManagementServer_ComponentName,
                CCSP_MANAGEMENT_SERVER_COMPONENT_VERSION,
                CCSP_DBUS_PATH_MS,
                CcspManagementServer_SubsystemPrefix,
                CcspManagementServer_Namespace,
                namespaceNumber
            );

    CcspTraceDebug2("ms", ("MS-FC: registration to CR returns %d.\n", (int)returnStatus));

    return returnStatus;
}

ANSC_STATUS
CcspManagementServer_DiscoverComponent
    (
    )
{
    char CrName[256] = {0};
    if ( CcspManagementServer_SubsystemPrefix)
    {
        _ansc_sprintf(CrName, "%s%s", CcspManagementServer_SubsystemPrefix, CCSP_DBUS_INTERFACE_CR);
    }
    else
    {
        _ansc_sprintf(CrName, "%s", CCSP_DBUS_INTERFACE_CR);
    }

#ifndef NO_PAM_COMP
    if(!pPAMComponentName || !pPAMComponentPath){
        componentStruct_t **components = NULL;
        int compNum = 0;
        int res = CcspBaseIf_discComponentSupportingNamespace (
            bus_handle,
            CrName,
            FirstUpstreamIpInterfaceParameterName,
            "",
            &components,
            &compNum);
        if(res != CCSP_SUCCESS || compNum < 1){
            CcspTraceWarning(("CcspManagementServer_DiscoverComponent find component error %d--%s\n", res, FirstUpstreamIpInterfaceParameterName));
        }
        else{
            if(pPAMComponentName) CcspManagementServer_Free(pPAMComponentName);
            if(pPAMComponentPath) CcspManagementServer_Free(pPAMComponentPath);
            pPAMComponentName = CcspManagementServer_CloneString(components[0]->componentName);
            pPAMComponentPath = CcspManagementServer_CloneString(components[0]->dbusPath);
            CcspTraceDebug2("ms", ("CcspManagementServer_DiscoverComponent find component %s--%s\n", pPAMComponentName, pPAMComponentPath));
        }
        // free component
        free_componentStruct_t(bus_handle, compNum, components);
        CcspTraceDebug2("ms", ("CcspManagementServer_DiscoverComponent ends with %s--%s\n", pPAMComponentName, pPAMComponentPath));
    }
#endif
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CcspManagementServer_UtilGetParameterValues
    (
        char*                       parameterNames[],
        int                         size,
        int*                        pval_size,
        parameterValStruct_t***     pppval
    )
/***
    This routine assumes the input parameter values owned by the same
    component. Otherwise, the caller should have divided the call into
    multiple calls, each with the parameter set owned by the same
    component.
***********************************************************************/
{
    char                            CrName[256]     = {0};
    componentStruct_t **            components      = NULL;
    int                             compNum         = 0;
    int                             res;

    char *                          pComponentName  = NULL;
    char *                          pComponentPath  = NULL;
    ANSC_STATUS                     ret             = ANSC_STATUS_FAILURE;
    if ( CcspManagementServer_SubsystemPrefix)
    {
        _ansc_sprintf(CrName, "%s%s", CcspManagementServer_SubsystemPrefix, CCSP_DBUS_INTERFACE_CR);
    }
    else
    {
        _ansc_sprintf(CrName, "%s", CCSP_DBUS_INTERFACE_CR);
    }

    if ( size > 0 )
    {
        res = 
            CcspBaseIf_discComponentSupportingNamespace
            (
                bus_handle,
                CrName,
                parameterNames[0],
                "",
                &components,
                &compNum
            );

        if ( res != CCSP_SUCCESS || compNum < 1 )
        {
            CcspTraceWarning(("CcspManagementServer_UtilGetParameterValues find component error %d--%s\n", res, parameterNames[0]));
            return  ANSC_STATUS_FAILURE;
        }
        else
        {
            pComponentName = CcspManagementServer_CloneString(components[0]->componentName);
            pComponentPath = CcspManagementServer_CloneString(components[0]->dbusPath);
            CcspTraceNotice(("CcspManagementServer_UtilGetParameterValues find component %s--%s\n", pComponentName, pComponentPath));
        }

        free_componentStruct_t(bus_handle, compNum, components);
    }

    if ( pComponentName )
    {
        res =
            CcspBaseIf_getParameterValues
                (
                    bus_handle,
                    pComponentName,
                    pComponentPath,
                    parameterNames,
                    size,
                    pval_size,
                    pppval
                );
        if( (res==CCSP_SUCCESS) && ((int)pval_size > 0) ) // This needs to be checked
            ret= ANSC_STATUS_SUCCESS;
        else {
            CcspTraceWarning(("CcspManagementServer_UtilGetParameterValues -- getParameterValues failed!\n"));
        }

        /*RDKB-7329, CID-33436, free unused resources before exit */
        CcspManagementServer_Free(pComponentName);
        
    }

    if(pComponentPath)
    {
        CcspManagementServer_Free(pComponentPath);
    }
    return  ret;
}


ANSC_STATUS
CcspManagementServer_UtilFreeParameterValStruct
    (
        int                         val_size,
        parameterValStruct_t**      ppval
    )
{
    free_parameterValStruct_t (bus_handle, val_size, ppval);

    return  ANSC_STATUS_SUCCESS;
}


ANSC_STATUS
CcspManagementServer_RegisterWanInterface()
{
    /*if(pFirstUpstreamIpAddress) 
        if(strlen(pFirstUpstreamIpAddress) > 0)
            return ANSC_STATUS_SUCCESS;*/
#ifdef NO_PAM_COMP
    return ANSC_STATUS_SUCCESS;
#endif

    if(RegisterWanInterfaceDone) return ANSC_STATUS_SUCCESS;

    if(!pPAMComponentName || !pPAMComponentPath) CcspManagementServer_DiscoverComponent();
    if(!pPAMComponentName || !pPAMComponentPath) return ANSC_STATUS_FAILURE;

    char CrName[256] = {0};
    if ( CcspManagementServer_SubsystemPrefix)
    {
        _ansc_sprintf(CrName, "%s%s", CcspManagementServer_SubsystemPrefix, CCSP_DBUS_INTERFACE_CR);
    }
    else
    {
        _ansc_sprintf(CrName, "%s", CCSP_DBUS_INTERFACE_CR);
    }

    /*this macro should be DM_IGD. But currently the root name switch is not finish. DM_IGD is not defined. */
#if 0
//#if defined(_COSA_VEN501_)
    /* We get Address path name directly for TR-098*/

    char * parameterNames[1];
    parameterNames[0] = CcspManagementServer_CloneString(FirstUpstreamIpInterfaceParameterName);

    parameterValStruct_t **parameterval = NULL;
    int val_size = 0;
    int res = CcspBaseIf_getParameterValues(
        bus_handle,
        pPAMComponentName,
        pPAMComponentPath,
        parameterNames,
        1,
        &val_size,
        &parameterval);
    if(parameterNames[0]) CcspManagementServer_Free(parameterNames[0]);
    if( (res==CCSP_SUCCESS) && (val_size > 0) && (AnscSizeOfString(parameterval[0]->parameterValue) > 0) ) {
        CcspTraceDebug2("ms", ("CcspManagementServer_RegisterWanInterface pFirstUpstreamIpv4Address is: %s\n", parameterval[0]->parameterValue));

        pFirstUpstreamIpAddress = CcspManagementServer_CloneString(parameterval[0]->parameterValue);
        free_parameterValStruct_t (bus_handle, val_size, parameterval);
    }
    else
        CcspTraceWarning2("ms", ("CcspManagementServer_RegisterWanInterface gets pFirstUpstreamIpv4Address fail..\n"));
    
#else

    // Get FirstUpstreamIpInterfaceParameterName parameter value 
    char * parameterNames[1];
    parameterNames[0] = CcspManagementServer_CloneString(FirstUpstreamIpInterfaceParameterName);
    
    parameterValStruct_t **parameterval = NULL;
    int val_size = 0;
    int res = CcspBaseIf_getParameterValues(
        bus_handle,
        pPAMComponentName,
        pPAMComponentPath,
        parameterNames,
        1,
        &val_size,
        &parameterval);
    if( parameterNames[0]) CcspManagementServer_Free(parameterNames[0]);
    if(val_size > 0) {
        pFirstUpstreamIpInterface = CcspManagementServer_CloneString(parameterval[0]->parameterValue);
        free_parameterValStruct_t (bus_handle, val_size, parameterval);
    }
    if(!pFirstUpstreamIpInterface) {
        CcspTraceWarning(("CcspManagementServer_RegisterWanInterface failed to get pFirstUpstreamIpInterface\n"));
        return ANSC_STATUS_FAILURE;
    }
    CcspTraceDebug2("ms", ("CcspManagementServer_RegisterWanInterface pFirstUpstreamIpInterface is %s\n", pFirstUpstreamIpInterface));
    
    // Get parameter name to obtain the smallest table entry 
    val_size = 0;
    char pFirstUpstreamIpInterfaceIpv4AddrTbl[200] = {0};
    errno_t rc = -1;
    rc = strcpy_s(pFirstUpstreamIpInterfaceIpv4AddrTbl, sizeof(pFirstUpstreamIpInterfaceIpv4AddrTbl), pFirstUpstreamIpInterface);
    ERR_CHK(rc);
    rc = strcat_s(pFirstUpstreamIpInterfaceIpv4AddrTbl, sizeof(pFirstUpstreamIpInterfaceIpv4AddrTbl),  "IPv4Address.");
    ERR_CHK(rc);
    parameterInfoStruct_t **parameterInfo = NULL;
    res = CcspBaseIf_getParameterNames(
        bus_handle,
        pPAMComponentName,
        pPAMComponentPath,
        pFirstUpstreamIpInterfaceIpv4AddrTbl,
        1,        
        &val_size,
        &parameterInfo);
    if(val_size > 0){
        size_t len = strlen(pFirstUpstreamIpInterfaceIpv4AddrTbl);
        int i = 0;
        int minInstance = 0;
        int minInstanceFlag = 0;
        for(; i<val_size; i++){
            int instanceNum = _ansc_atoi(parameterInfo[i]->parameterName + len);
            if(i == 0) {
                minInstance = instanceNum;
                minInstanceFlag = 0;
            }
            else if(minInstance > instanceNum) {
                minInstance = instanceNum;
                minInstanceFlag = i;
            }
        }
        pFirstUpstreamIpAddress = CcspManagementServer_MergeString(parameterInfo[minInstanceFlag]->parameterName, "IPAddress");
        free_parameterInfoStruct_t (bus_handle, val_size, parameterInfo);
    }

#endif

    if(!pFirstUpstreamIpAddress) {
        CcspTraceWarning(("CcspManagementServer_RegisterWanInterface failed to get pFirstUpstreamIpAddress\n"));
        return ANSC_STATUS_FAILURE;
    }
    char pRecordName[1000] = {0};
    rc = strcpy_s(pRecordName, sizeof(pRecordName), CcspManagementServer_ComponentName);
    ERR_CHK(rc);
    rc = strcat_s(pRecordName, sizeof(pRecordName), ".FirstUpstreamIpAddress.Value");
    ERR_CHK(rc);
    res = PSM_Set_Record_Value2(
        bus_handle,
        CcspManagementServer_SubsystemPrefix,
        pRecordName,
        ccsp_string,
        pFirstUpstreamIpAddress);
    if(res != CCSP_SUCCESS){
        CcspTraceWarning2("ms", ( "CcspManagementServer_RegisterWanInterface PSM write failure %d: %s----%s.\n", 
            res, pRecordName, pFirstUpstreamIpAddress)); 
    }

    CcspTraceDebug2("ms", ("CcspManagementServer_RegisterWanInterface pFirstUpstreamIpAddress is %s\n", pFirstUpstreamIpAddress));

    // Set pFirstUpstreamIpAddress active notification attribute.
/*
    int priority = 0, sessionID;
    res = CcspBaseIf_requestSessionID (
        bus_handle,   
        CrName,       
        priority,
        &sessionID);
    if(res != CCSP_Message_Bus_OK) {
        CcspTraceWarning(("CcspManagementServer_RegisterWanInterface CcspBaseIf_requestSessionID returns %d\n", res));
        sessionID = 0;
    }
*/
    parameterAttributeStruct_t val[1];
    val[0].parameterName = pFirstUpstreamIpAddress;
    val[0].notificationChanged = 1;
    val[0].notification = 1;
    val[0].accessControlChanged = 0;
   // val[1].parameterName = FirstUpstreamIpInterfaceParameterName;
   // val[1].notificationChanged = 1;
   // val[1].notification = 1;
   // val[1].accessControlChanged = 0;
    res = CcspBaseIf_setParameterAttributes(
        bus_handle,
        pPAMComponentName,
        pPAMComponentPath,
        0,
        val,
        1);
/*
    CcspBaseIf_informEndOfSession(
        bus_handle,
        CrName,
        sessionID
        );
*/
    if(res != CCSP_Message_Bus_OK) {
        CcspTraceWarning(("CcspManagementServer_RegisterWanInterface CcspBaseIf_setParameterAttributes returns %d, name=<%s>\n",
            res,
                          val[0].parameterName));
        return ANSC_STATUS_FAILURE;
    }

    CcspManagementServer_GenerateConnectionRequestURL(FALSE, NULL);
    RegisterWanInterfaceDone = TRUE;

    return ANSC_STATUS_SUCCESS;
}

//Custom
extern void CcspManagementServer_GenerateConnectionRequestURLCustom(
    BOOL fromValueChangeSignal,
    char *newValue,
    char *ipAddr,
    msObjectInfo *objectInfo);

ANSC_STATUS CcspManagementServer_GenerateConnectionRequestURL(
    BOOL fromValueChangeSignal,
    char *newValue)
{
    char ipAddr[200] = {0};
    char result[200] = "http://";
    errno_t rc = -1;
    if(!fromValueChangeSignal){
#ifndef NO_PAM_COMP
        if(pPAMComponentName && pPAMComponentPath && pFirstUpstreamIpAddress) 
        {
            // Get FirstUpstreamIpInterfaceParameterName parameter value 
            char * parameterNames[1];
            parameterNames[0] = CcspManagementServer_CloneString(pFirstUpstreamIpAddress);

            parameterValStruct_t **parameterval = NULL;
            int val_size = 0;
            CcspBaseIf_getParameterValues(
                bus_handle,
                pPAMComponentName,
                pPAMComponentPath,
                parameterNames,
                1,
                &val_size,
                &parameterval);
            if(parameterNames[0]) CcspManagementServer_Free(parameterNames[0]);
            if(val_size <= 0) return ANSC_STATUS_FAILURE;
            rc = strcpy_s(ipAddr, sizeof(ipAddr), parameterval[0]->parameterValue);
            ERR_CHK(rc);
            free_parameterValStruct_t (bus_handle, val_size, parameterval);
        }
#else

#if defined(_COSA_HYBRID_LINUX_))
        if (g_IPAddress[0] != NULL)
        {
            AnscCopyString(ipAddr, g_IPAddress);
        }
        else
        {
            unsigned int addr_n;
            char addr_a[32] = {0};
            
            if (!_ansc_get_ipv4_addr("eth0", &addr_n, addr_a, sizeof(addr_a)))
            {
                rc = strncpy_s(ipAddr, sizeof(ipAddr), addr_a, strlen(addr_a));
                ERR_CHK(rc);
            }
        }
#endif
        
#endif
    }
    else
    {
         rc = strcpy_s(ipAddr, sizeof(ipAddr), newValue);
         ERR_CHK(rc);
    }
    /* If no IP Address, keep the previous value. */
    if(strlen(ipAddr) <= 0  || AnscEqualString(ipAddr, "0.0.0.0", TRUE)) {
        return ANSC_STATUS_SUCCESS;
    }
    rc = strcat_s(result, sizeof(result), ipAddr);
    ERR_CHK(rc);

    if ( TRUE )
    {
        char* pPort = objectInfo[ManagementServerID].parameters[ManagementServerX_CISCO_COM_ConnectionRequestURLPortID].value;
        if(pPort && strlen(pPort) > 0) {
            rc =  strcat_s(result, sizeof(result), ":");
            ERR_CHK(rc);
            if ( strlen(pPort) > 0 )
            {
                rc = strcat_s(result, sizeof(result), objectInfo[ManagementServerID].parameters[ManagementServerX_CISCO_COM_ConnectionRequestURLPortID].value);
                ERR_CHK(rc);
            }
            else
            {
                char buf[10];
            
                sprintf(buf, "%d", CWMP_PORT);
                rc = strcat_s(result, sizeof(result), buf);
                ERR_CHK(rc);
            }
        }
    }
    rc = strcat_s(result, sizeof(result),  "/");
    ERR_CHK(rc);

    if(objectInfo[ManagementServerID].parameters[ManagementServerX_CISCO_COM_ConnectionRequestURLPathID].value)
    {
        rc = strcat_s(result, sizeof(result), objectInfo[ManagementServerID].parameters[ManagementServerX_CISCO_COM_ConnectionRequestURLPathID].value);
        ERR_CHK(rc);
     }
    if(!AnscEqualString(objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].value, result, TRUE)){
        // Send Value change signal.
        char *oldValue = objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].value;
        objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].value = CcspManagementServer_CloneString(result);
        /* PSM write */
        char pRecordName[1000] = {0};
        size_t len1, len2, len3;
        len1 = strlen(CcspManagementServer_ComponentName);
        rc = strncpy_s(pRecordName, sizeof(pRecordName), CcspManagementServer_ComponentName, len1);
        ERR_CHK(rc);
        pRecordName[len1] = '.';
        len2 = strlen(objectInfo[ManagementServerID].name);
        rc = strncat_s(pRecordName, sizeof(pRecordName), objectInfo[ManagementServerID].name, len2);
        ERR_CHK(rc);
        len3 = strlen(objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].name);
        rc = strncat_s(pRecordName, sizeof(pRecordName), objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].name, len3);
        ERR_CHK(rc);
        rc = strncat_s(pRecordName, sizeof(pRecordName), ".Value", 6);
        ERR_CHK(rc);
        int res = PSM_Set_Record_Value2(
            bus_handle,
            CcspManagementServer_SubsystemPrefix,
            pRecordName,
            ccsp_string,
            objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].value);
        if(res != CCSP_SUCCESS){
            CcspTraceWarning2("ms", ( "CcspManagementServer_GenerateConnectionRequestURL PSM write failure %d!=%d: %s----%s.\n", 
                res, CCSP_SUCCESS, pRecordName, objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].value)); 
        }

        SendValueChangeSignal(ManagementServerID, ManagementServerConnectionRequestURLID, oldValue);
        CcspManagementServer_ValueChangeCB(CcspManagementServer_cbContext, CcspManagementServer_GetPAObjectID(ManagementServerID));
        if(oldValue) CcspManagementServer_Free(oldValue);
    }
    
    //Custom
    CcspManagementServer_GenerateConnectionRequestURLCustom(fromValueChangeSignal,newValue,ipAddr,objectInfo);
    
    CcspTraceDebug2
        (
            "ms", 
            (
                "CcspManagementServer_GenerateConnectionRequestURL ConnectionRequestURL is %s\n", 
                 objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].value
            )
        );
    return ANSC_STATUS_SUCCESS;
}

int SendValueChangeSignal(
    int objectID, 
    int parameterID,
    const char * oldValue)
{
    parameterSigStruct_t val;
    val.parameterName = CcspManagementServer_MergeString(objectInfo[objectID].name, objectInfo[objectID].parameters[parameterID].name);
    val.newValue = CcspManagementServer_CloneString(objectInfo[objectID].parameters[parameterID].value);
    val.oldValue = CcspManagementServer_CloneString(oldValue);
    val.type = objectInfo[objectID].parameters[parameterID].type;
    val.subsystem_prefix = CcspManagementServer_CloneString(CcspManagementServer_SubsystemPrefix);
    int res = CcspBaseIf_SendparameterValueChangeSignal (
        bus_handle,
        &val,
        1);
    CcspTraceDebug(("send value change signal %s %s \n", val.parameterName, val.newValue));
    if(val.parameterName) CcspManagementServer_Free((void*)val.parameterName);
    if(val.oldValue) CcspManagementServer_Free((void*)val.oldValue);
    if(val.newValue) CcspManagementServer_Free((void*)val.newValue);
    if(val.subsystem_prefix) CcspManagementServer_Free((void*)val.subsystem_prefix);
    return res;
}

// Recevie value change signal from pPAMComponentName
CCSP_BOOL
CcspManagementServer_paramValueChanged
	(
		parameterSigStruct_t*		val,
		int				size
	)
{
    int i = 0;
    for(; i < size; i++){
        if(AnscEqualString((char *)(val[i].parameterName), pFirstUpstreamIpAddress, TRUE)){
            CcspManagementServer_GenerateConnectionRequestURL(TRUE, (char *)(val[i].newValue));
            CcspTraceDebug(("CcspManagementServer_paramValueChanged %s %s\n", pFirstUpstreamIpAddress, val[i].newValue));
        }
        else if(AnscEqualString((char *)(val[i].parameterName), FirstUpstreamIpInterfaceParameterName, TRUE)){
            RegisterWanInterfaceDone = FALSE;
            CcspManagementServer_RegisterWanInterface();
            CcspTraceDebug(("CcspManagementServer_paramValueChanged %s %s\n", FirstUpstreamIpInterfaceParameterName, val[i].newValue));
        }
    }
    return TRUE;
}

int CcspManagementServer_GetParameterID(
    int objectID,
    char *paraName   /* parameterName is the pure name without object name prefix. */
    )
{
    if(objectID >= SupportedDataModelID + sdmObjectNumber + 1) return -1;
    int i = 0;
    for(; i<objectInfo[objectID].numberOfParameters; i++){
        if(AnscEqualString(objectInfo[objectID].parameters[i].name, paraName, TRUE)) 
            return i;
    }
    return -1;
}

//Custom
extern int CcspManagementServer_GetObjectIDCustom(char *p1);

int CcspManagementServer_GetObjectID(
    char *parameterName,   /* name has to be a full object name (ends with .) or full parameter name */
    char **name
    )
{
    char *p = NULL;

    if ( !parameterName )
    {
        return -1;
    }
    p=strstr(parameterName, _DeviceObjectName);
    if(!p || p != parameterName) {
        if((p = strstr(parameterName, _MemoryObjectName)) == parameterName){
            *name = p + strlen(_MemoryObjectName);
            return MemoryID;
        }
        if((p = strstr(parameterName, _LoggingObjectName)) == parameterName){
            *name = p + strlen(_LoggingObjectName);
            return LoggingID;
        }
        if((p = strstr(parameterName, _Tr069paObjectName)) == parameterName){
            *name = p + strlen(_Tr069paObjectName);
            return Tr069paID;
        }
        if((p = strstr(parameterName, _CcspObjectName)) == parameterName){
            *name = p + strlen(_CcspObjectName);
            return CcspID;
        }
        if((p = strstr(parameterName, _SpvtgObjectName)) == parameterName){
            *name = p + strlen(_SpvtgObjectName);
            return SpvtgID;
        }
        if((p = strstr(parameterName, _CiscoObjectName)) == parameterName){
            *name = p + strlen(_CiscoObjectName);
            return CiscoID;
        }
        if((p = strstr(parameterName, _ComObjectName)) == parameterName){
            *name = p + strlen(_ComObjectName);
            return ComID;
        }
        return -1;
    }

    p += strlen(_DeviceObjectName);
    if(*p == '\0') 
    {
        *name = p;
        return DeviceID;
    }

    char *p1 = NULL;

    //Custom
    int nRetVal = CcspManagementServer_GetObjectIDCustom(p1);
    if(nRetVal == -1)
    {
      return -1;
    }
   
    p1=strstr(p, "ManagementServer.");
    if(!p1 || p1 != p) {
        p1 = strstr(p, "DeviceInfo.");
        if(!p1 || p1 != p) {
            return -1;
        }
        p1 += strlen("DeviceInfo.");
        if(*p1 == '\0') 
        {
            *name = p1;
            return DeviceInfoID;
        }
        p = strstr(p1, "SupportedDataModel.");
        if(!p || p != p1) return -1;
        *name = p + strlen("SupportedDataModel.");
        if(strlen(*name) == 0) return SupportedDataModelID;
        /* Get intance number of the table object under SupportedDataModel. */
        char instanceNumber[50];
        int  i;
        for(i=0; (size_t)i<strlen(*name); i++)
        {
            if((*name)[i] == '\0' || (*name)[i] == '.' || i >= 49){
                instanceNumber[i] = '\0';
                break;
            }
            instanceNumber[i] = (*name)[i];
        }
        *name += i + 1;
        i = atoi(instanceNumber);
        if(i <= 0 || i > sdmObjectNumber) return -1;
        return i + SupportedDataModelID;
    }
    else{
        p1 += strlen("ManagementServer.");
        char *s1 = strstr(p1, "AutonomousTransferCompletePolicy.");
        if(!s1 || s1 != p1)
        {
            char *s2 = strstr(p1, "DUStateChangeComplPolicy.");
            if(!s2 || s2 != p1) /* ManagementServer. */
            {
                *name = p1;
                return ManagementServerID;
            }
            else /* DUStateChangeComplPolicy. */
            {
                *name = p1 + 25;
                return DUStateChangeComplPolicyID;
            }
        }
        else /* "AutonomousTransferCompletePolicy." */
        {
            *name = p1 + 33;
            return AutonomousTransferCompletePolicyID;
        }
    }
}

int CcspManagementServer_GetParameterCount(
    int objectID
    )
{
    int n = objectInfo[objectID].numberOfParameters;
    int i = 0;
    for(; i<objectInfo[objectID].numberOfChildObjects; i++ )
    {
        n += CcspManagementServer_GetParameterCount(objectInfo[objectID].childObjectIDs[i]);
    }
    return n;
}

//Custom
extern void CcspManagementServer_GetSingleParameterValueCustom(
        int objectID,
        int parameterID,
        parameterValStruct_t *val
    );

void CcspManagementServer_GetSingleParameterValue(
        int objectID,
        int parameterID,
        parameterValStruct_t *val
    )
{
    val->parameterName = CcspManagementServer_MergeString(objectInfo[objectID].name, objectInfo[objectID].parameters[parameterID].name);
    val->type = objectInfo[objectID].parameters[parameterID].type;
    if(objectID == ManagementServerID){
        switch (parameterID)
        {
        case ManagementServerEnableCWMPID:
            val->parameterValue = CcspManagementServer_GetEnableCWMPStr(NULL);
            break;
        case ManagementServerURLID:
            val->parameterValue = CcspManagementServer_GetURL(NULL);
            break;
        case ManagementServerUsernameID:
            val->parameterValue = CcspManagementServer_GetUsername(NULL);
            break;
        case ManagementServerPasswordID:
            //val->parameterValue = NULL;
	    val->parameterValue = CcspManagementServer_GetPassword(NULL);
            break;
        case ManagementServerPeriodicInformEnableID:
            val->parameterValue = CcspManagementServer_GetPeriodicInformEnableStr(NULL);
            break;
        case ManagementServerPeriodicInformIntervalID:
            val->parameterValue = CcspManagementServer_GetPeriodicInformIntervalStr(NULL);
            break;
        case ManagementServerPeriodicInformTimeID:
            val->parameterValue = CcspManagementServer_GetPeriodicInformTimeStr(NULL);
            break;
        case ManagementServerParameterKeyID:
            val->parameterValue = CcspManagementServer_GetParameterKey(NULL);
            break;
        case ManagementServerConnectionRequestURLID:
            val->parameterValue = CcspManagementServer_GetConnectionRequestURL(NULL);
            break;
        case ManagementServerConnectionRequestUsernameID:
            val->parameterValue = CcspManagementServer_GetConnectionRequestUsername(NULL);
            break;
        case ManagementServerConnectionRequestPasswordID:
            val->parameterValue = NULL;
            break;
        case ManagementServerACSOverrideID:
            val->parameterValue = CcspManagementServer_GetACSOverrideStr(NULL);
            break;
        case ManagementServerUpgradesManagedID:
            val->parameterValue = CcspManagementServer_GetUpgradesManagedStr(NULL);
            break;
        case ManagementServerKickURLID:
            val->parameterValue = CcspManagementServer_GetKickURL(NULL);
            break;
        case ManagementServerDownloadProgressURLID:
            val->parameterValue = CcspManagementServer_GetDownloadProgressURL(NULL);
            break;
        case ManagementServerDefaultActiveNotificationThrottleID:
            val->parameterValue = CcspManagementServer_GetDefaultActiveNotificationThrottleStr(NULL);
            break;
        case ManagementServerCWMPRetryMinimumWaitIntervalID:
            val->parameterValue = CcspManagementServer_GetCWMPRetryMinimumWaitIntervalStr(NULL);
            break;
        case ManagementServerCWMPRetryIntervalMultiplierID:
            val->parameterValue = CcspManagementServer_GetCWMPRetryIntervalMultiplierStr(NULL);
            break;
        case ManagementServerUDPConnectionRequestAddressID:
            val->parameterValue = CcspManagementServer_GetUDPConnectionRequestAddress(NULL);
            break;
        case ManagementServerUDPConnectionRequestAddressNotificationLimitID:
            val->parameterValue = CcspManagementServer_GetUDPConnectionRequestAddressNotificationLimit(NULL);
            break;
        case ManagementServerSTUNEnableID:
            val->parameterValue = CcspManagementServer_GetSTUNEnableStr(NULL);
            break;
        case ManagementServerSTUNServerAddressID:
            val->parameterValue = CcspManagementServer_GetSTUNServerAddress(NULL);
            break;
        case ManagementServerSTUNServerPortID:
            val->parameterValue = CcspManagementServer_GetSTUNServerPortStr(NULL);
            break;
        case ManagementServerSTUNUsernameID:
            val->parameterValue = CcspManagementServer_GetSTUNUsername(NULL);
            break;
        case ManagementServerSTUNPasswordID:
            val->parameterValue = NULL;
            break;
        case ManagementServerSTUNMaximumKeepAlivePeriodID:
            val->parameterValue = CcspManagementServer_GetSTUNMaximumKeepAlivePeriodStr(NULL);
            break;
        case ManagementServerSTUNMinimumKeepAlivePeriodID:
            val->parameterValue = CcspManagementServer_GetSTUNMinimumKeepAlivePeriodStr(NULL);
            break;
        case ManagementServerNATDetectedID:
            val->parameterValue = CcspManagementServer_GetNATDetectedStr(NULL);
            break;
        case ManagementServerAliasBasedAddressingID:
            val->parameterValue = (char *)CcspManagementServer_GetAliasBasedAddressingStr(NULL);
            break;
        case ManagementServerX_CISCO_COM_ConnectionRequestURLPortID:
            val->parameterValue = CcspManagementServer_GetConnectionRequestURLPort(NULL);
            break;
        case ManagementServerX_CISCO_COM_ConnectionRequestURLPathID:
            val->parameterValue = CcspManagementServer_GetConnectionRequestURLPath(NULL);
            break;
        case ManagementServerX_CISCO_COM_DiagCompleteID:
            val->parameterValue = CcspManagementServer_CloneString("false");
            break;
        default: break;
        }
    }
    else if(objectID == AutonomousTransferCompletePolicyID){
        switch (parameterID)
        {
        case AutonomousTransferCompletePolicyEnableID:
            val->parameterValue = CcspManagementServer_GetAutonomousTransferCompletePolicy_EnableStr(NULL);
            break;
        case AutonomousTransferCompletePolicyTransferTypeFilterID:
            val->parameterValue = CcspManagementServer_GetAutonomousTransferCompletePolicy_TransferTypeFilter(NULL);
            break;
        case AutonomousTransferCompletePolicyResultTypeFilterID:
            val->parameterValue = CcspManagementServer_GetAutonomousTransferCompletePolicy_ResultTypeFilter(NULL);
            break;
        case AutonomousTransferCompletePolicyFileTypeFilterID:
            val->parameterValue = CcspManagementServer_GetAutonomousTransferCompletePolicy_FileTypeFilter(NULL);
            break;
        default: break;
        }
    }
    else if(objectID == DUStateChangeComplPolicyID){
        switch (parameterID)
        {
        case DUStateChangeComplPolicyEnableID:
            val->parameterValue = CcspManagementServer_GetDUStateChangeComplPolicy_EnableStr(NULL);
            break;
        case DUStateChangeComplPolicyOperationTypeFilterID:
            val->parameterValue = CcspManagementServer_GetDUStateChangeComplPolicy_OperationTypeFilter(NULL);
            break;
        case DUStateChangeComplPolicyResultTypeFilterID:
            val->parameterValue = CcspManagementServer_GetDUStateChangeComplPolicy_ResultTypeFilter(NULL);
            break;
        case DUStateChangeComplPolicyFaultCodeFilterID:
            val->parameterValue = CcspManagementServer_GetDUStateChangeComplPolicy_FaultCodeFilter(NULL);
            break;
        default: break;
        }
    }
    else if(objectID == Tr069paID){
        switch (parameterID)
        {
        case Tr069paNameID:
            val->parameterValue = CcspManagementServer_GetTr069pa_Name(NULL);
            break;
        case Tr069paVersionID:
            val->parameterValue = CcspManagementServer_GetTr069pa_Version(NULL);
            break;
        case Tr069paAuthorID:
            val->parameterValue = CcspManagementServer_GetTr069pa_Author(NULL);
            break;
        case Tr069paHealthID:
            val->parameterValue = CcspManagementServer_GetTr069pa_Health(NULL);
            break;
        case Tr069paStateID:
            val->parameterValue = CcspManagementServer_GetTr069pa_State(NULL);
            break;
        case Tr069paDTXmlID:
            val->parameterValue = CcspManagementServer_GetTr069pa_DTXml(NULL);
            break;
        default: break;
        }
    }
    else if(objectID == MemoryID){
        switch (parameterID)
        {
        case MemoryMinUsageID:
            val->parameterValue = CcspManagementServer_GetMemory_MinUsageStr(NULL);
            break;
        case MemoryMaxUsageID:
            val->parameterValue = CcspManagementServer_GetMemory_MaxUsageStr(NULL);
            break;
        case MemoryConsumedID:
            val->parameterValue = CcspManagementServer_GetMemory_ConsumedStr(NULL);
            break;
        default: break;
        }
    }
    else if(objectID == LoggingID){
        switch (parameterID)
        {
        case LoggingEnableID:
            val->parameterValue = CcspManagementServer_GetLogging_EnableStr(NULL);
            break;
        case LoggingLogLevelID:
            val->parameterValue = CcspManagementServer_GetLogging_LogLevelStr(NULL);
            break;
        default: break;
        }
    }
    
    else if(objectID == CWMPID)
    {
      //Custom
      CcspManagementServer_GetSingleParameterValueCustom(objectID,parameterID,val);
    }
    else
    {
        switch (parameterID)
        {
        case SupportedDataModelURLID:
            val->parameterValue = CcspManagementServer_GetSupportedDataModel_URL(NULL, objectID);
            break;
        case SupportedDataModelURNID:
            val->parameterValue = CcspManagementServer_GetSupportedDataModel_URN(NULL, objectID);
            break;
        case SupportedDataModelFeaturesID:
            val->parameterValue = CcspManagementServer_GetSupportedDataModel_Features(NULL, objectID);
            break;
        default: break;
        }
    }
    
    //val->parameterValue = CcspManagementServer_CloneString(objectInfo[objectID].parameters[parameterID].value);
}

/* return how many records have been added. */
int CcspManagementServer_GetSingleObjectValues(
    int objectID,
    parameterValStruct_t **val,
    int beginValID
    )
{
    int valID = beginValID;
    int i = 0;
    for(; i < objectInfo[objectID].numberOfParameters; i++ )
    {
        CcspManagementServer_GetSingleParameterValue(objectID, i, val[valID]);
        valID++;
    }
    for(i = 0; i < objectInfo[objectID].numberOfChildObjects; i++ )
    {
        valID += CcspManagementServer_GetSingleObjectValues(
                objectInfo[objectID].childObjectIDs[i],
                val,
                valID);
    }
    return valID - beginValID;
}

int CcspManagementServer_SetSingleParameterAttributes(
    int sessionId,
    int objectID,
    int parameterID,
    parameterAttributeStruct_t *val
    )
{
    UNREFERENCED_PARAMETER(sessionId);
    /* write to PSM at the same time. */
    if(val->notificationChanged){
        objectInfo[objectID].parameters[parameterID].notification = val->notification;
        /* PSM write */
        char pRecordName[1000] = {0};
        size_t len1, len2, len3;
        errno_t rc = -1;
        len1 = strlen(CcspManagementServer_ComponentName);
        rc = strncpy_s(pRecordName, sizeof(pRecordName), CcspManagementServer_ComponentName, len1);
        ERR_CHK(rc);
        pRecordName[len1] = '.';
        len2 = strlen(objectInfo[objectID].name);
        rc = strncat_s(pRecordName, sizeof(pRecordName), objectInfo[objectID].name, len2);
        ERR_CHK(rc);
        len3 = strlen(objectInfo[objectID].parameters[parameterID].name);
        rc = strncat_s(pRecordName, sizeof(pRecordName), objectInfo[objectID].parameters[parameterID].name, len3);
        ERR_CHK(rc);
        rc = strncat_s(pRecordName, sizeof(pRecordName), ".Notification", 13);
        ERR_CHK(rc);
        char pValue[50] = {0};
        //        _ansc_itoa(objectInfo[objectID].parameters[parameterID].notification, pValue, 10);
        _ansc_sprintf(pValue, "%d", objectInfo[objectID].parameters[parameterID].notification);
        int res = PSM_Set_Record_Value2(
            bus_handle,
            CcspManagementServer_SubsystemPrefix,
            pRecordName,
            ccsp_string,
            pValue);
        if(res != CCSP_SUCCESS){
            CcspTraceWarning2("ms", ( "CcspManagementServer_SetSingleParameterAttributes PSM write failure %d!=%d: %s----%s.\n", res, CCSP_SUCCESS, pRecordName, pValue)); 
        }
    }
    if(val->accessControlChanged)
    {
        objectInfo[objectID].parameters[parameterID].access = val->access;
        objectInfo[objectID].parameters[parameterID].accessControlBitmask = val->accessControlBitmask;
    }

    return ANSC_STATUS_SUCCESS;
}

int CcspManagementServer_SetSingleObjectAttributes(
    int sessionId,
    int objectID,
    parameterAttributeStruct_t *val
    ) 
{
    int i = 0;
    for(; i<objectInfo[objectID].numberOfParameters; i++)
    {
        CcspManagementServer_SetSingleParameterAttributes(
            sessionId,
            objectID,
            i,
            val);
    }
    for(i=0; i<objectInfo[objectID].numberOfChildObjects; i++)
    {
        CcspManagementServer_SetSingleObjectAttributes(
            sessionId,
            objectInfo[objectID].childObjectIDs[i],
            val);
    }
    return 0;
}

void CcspManagementServer_GetSingleParameterAttributes(
    int objectID,
    int parameterID,
    parameterAttributeStruct_t *attr
    )
{
    attr->parameterName = CcspManagementServer_MergeString(objectInfo[objectID].name, objectInfo[objectID].parameters[parameterID].name);
    attr->notification = objectInfo[objectID].parameters[parameterID].notification;
    attr->access = objectInfo[objectID].parameters[parameterID].access;
    attr->accessControlBitmask = objectInfo[objectID].parameters[parameterID].accessControlBitmask;
}

/* For each parameter, add one record item to attr.
 * Return how many records have been added. 
 */
int CcspManagementServer_GetSingleObjectAttributes(
    int objectID,
    parameterAttributeStruct_t **attr,
    int beginAttrID
    )
{
    int attrID = beginAttrID;
    int i = 0;
    for(; i < objectInfo[objectID].numberOfParameters; i++ )
    {
        CcspManagementServer_GetSingleParameterAttributes(objectID, i, attr[attrID]);
        attrID++;
    }
    for(i = 0; i < objectInfo[objectID].numberOfChildObjects; i++ )
    {
        attrID += CcspManagementServer_GetSingleObjectAttributes(
                objectInfo[objectID].childObjectIDs[i],
                attr,
                attrID);
    }
    return attrID - beginAttrID;
}

static int CcspManagementServer_ValidateINT(
    const char * intStr,
    CCSP_BOOL hasMinVal,
    int minVal,
    CCSP_BOOL hasMaxVal,
    int maxVal
    )
{
    /* Do not check if it is larger than the integer range. */
    int len = strlen(intStr);
    if(len <= 0) return -1;
    
    while(*intStr == ' ') {
        intStr++;
        len--;
    }
    if(len <= 0) return -1;

    int res = atoi(intStr);
    if(res == 0 && !(*intStr == '0' || (*intStr == '-' && *(intStr+1) == '0'))) return -1;

    if(hasMaxVal) 
        if(res > maxVal) return -1;
    if(hasMinVal)
        if(res < minVal) return -1;
    return 0;
}

static int CcspManagementServer_ValidateBoolean(
    const char * boolStr
    )
{
    /* Only supports "0" or "1" or case free "true" and "false". */
    int len = strlen(boolStr);
    if(len <= 0) return -1;
    
    while(*boolStr == ' ') {
        boolStr++;
        len--;
    }
    if(len <= 0) return -1;

    if(*boolStr == '1') return 1;
    if(*boolStr == '0') return 0;
    if(AnscEqualString((char *)boolStr, "true", FALSE)) return 1;
    if(AnscEqualString((char *)boolStr, "false", FALSE)) return 0;
    return -1;
}

static int CcspManagementServer_ValidateStrLen(
    const char * str,
    int lenLimit  /* -1 means no limit. */
    )
{
    if((lenLimit == -1)||(AnscSizeOfString(str) < (unsigned int)lenLimit))
        return 0;
    else
        return -1;
}

static int CcspManagementServer_ValidateURL(
    const char * URL
    )
{
    if(CcspManagementServer_ValidateStrLen(URL, 256) != 0) return -1;
    /* get the hostname */
    const char * hostname = strstr(URL, "//");
    if(hostname == NULL) hostname  = URL;
    else hostname += 2;
    const char * hostnameEnd = strstr(hostname, "/");
    if(hostnameEnd == NULL) hostnameEnd = &URL[strlen(URL) - 1];
    /* validate hostname */
    while(hostname < hostnameEnd){
        /* Each label must be between 1 and 63 characters long. */
        const char * p = strstr(hostname + 1, ".");
        if(p == NULL) p = hostnameEnd;
        if(p - hostname > 63 || p - hostname < 1)
            return -1;
        hostname = p;
    }
    return 0;
}

static int CcspManagementServer_ValidateDateTime(
    const char * dt
    )
{
    PANSC_UNIVERSAL_TIME pUniversalTime = (PANSC_UNIVERSAL_TIME) SlapVcoStringToCalendarTime(NULL, (char *)dt);
    if(!pUniversalTime) return -1;
    if(pUniversalTime->Year > 2100 || pUniversalTime->Year < 1970)
    {
        CcspManagementServer_Free(pUniversalTime); /*RDKB-7329, CID-33053, free unused resources */
        return -1;
    }
    CcspManagementServer_Free(pUniversalTime); /*RDKB-7329, CID-33053, free unused resources */
    return 0;
}

//Custom
extern int CcspManagementServer_ValidateParameterValuesCustom(
    int sessionId,
    unsigned int writeID,
    parameterValStruct_t *val,
    int size,
    char ** invalidParameterName
    );

//Custom
extern int CcspManagementServer_GetPAObjectIDCustom(int objectID);

/* validation of the parameters before commit */
int CcspManagementServer_ValidateParameterValues(
    int sessionId,
    unsigned int writeID,
    parameterValStruct_t *val,
    int size,
    char ** invalidParameterName
    )
{
    int returnStatus = 0;
    char *invalidParaName = NULL;
    parameterSetting.sessionID = sessionId;
    parameterSetting.writeID = writeID;
    unsigned int                    prevIndex   = 0;

    if ( parameterSetting.msParameterValSettings == NULL )
    {
        if ( size > CCSP_TR069PA_DFT_PARAM_VAL_SETTINGS_NUMBER )
        {
            parameterSetting.size                   = size;
        }
        else
        {
            parameterSetting.size                   = CCSP_TR069PA_DFT_PARAM_VAL_SETTINGS_NUMBER;
        }
        parameterSetting.msParameterValSettings = CcspManagementServer_Allocate(sizeof(msParameterValSetting) * parameterSetting.size);
        parameterSetting.currIndex              = 0;

        if ( parameterSetting.msParameterValSettings == NULL )
        {
            returnStatus = TR69_INTERNAL_ERROR;
            *invalidParameterName = CcspManagementServer_CloneString("Insufficient Resources");
            return  returnStatus;
        }
    }

    prevIndex = parameterSetting.currIndex;

    int i = 0;
    for(; i<size; i++)
    {
        char *name;
        int objectID = CcspManagementServer_GetObjectID(val[i].parameterName, &name);
        if(objectID < 0)
        {
            if(!invalidParaName) invalidParaName = CcspManagementServer_CloneString(val[i].parameterName);
            if(returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_NAME; 
            continue;
        }
        /* Device.DeviceInfo.SupportedDataModel. is read only */
        if(objectID >= SupportedDataModelID)
        {
            if(!invalidParaName) invalidParaName = CcspManagementServer_CloneString(val[i].parameterName);
            if(returnStatus == 0) returnStatus = TR69_UPDATE_NON_WRITABLE_PARAMETER; 
            continue;
        }
        /* It is a parameter. */
        if(strlen(name) != 0)
        {
            int parameterID = CcspManagementServer_GetParameterID(objectID, name);
            if(parameterID < 0)
            {
                if(!invalidParaName) invalidParaName = CcspManagementServer_CloneString(val[i].parameterName);
                if(returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_NAME; 
                continue;
            }

            /* If types mismatch, return invalid argument. */
            if(objectInfo[objectID].parameters[parameterID].type != val[i].type)
            {
                if(!invalidParaName) invalidParaName = CcspManagementServer_CloneString(val[i].parameterName);
                if(returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_TYPE; 
                continue;
            }
            if(objectInfo[objectID].parameters[parameterID].access == CCSP_RO)
            {
                if(!invalidParaName) invalidParaName = CcspManagementServer_CloneString(val[i].parameterName);
                if(returnStatus == 0) returnStatus = TR69_UPDATE_NON_WRITABLE_PARAMETER; 
                continue;
            }

            /*
             *  Check if we still have enough buffer and double the size if needed
             */
            if ( parameterSetting.currIndex >= (unsigned int)parameterSetting.size )
            {
                msParameterValSetting*      pNewSettings    = NULL;
                AnscTraceWarning
                    ((
                        "CcspManagementServer_ValidateParameterValues -- enlarge array size from %d to %d!\n",
                        parameterSetting.size,
                        parameterSetting.size * 2
                    ));

                pNewSettings = CcspManagementServer_Allocate(sizeof(msParameterValSetting) * parameterSetting.size * 2);

                if ( !pNewSettings )
                {
                    returnStatus = TR69_INTERNAL_ERROR;
                    *invalidParameterName = CcspManagementServer_CloneString("Insufficient Resources");
                    return  returnStatus;
                }
                else
                {
                    /* Free the old array and replace it with the new one */
                    AnscCopyMemory(pNewSettings, parameterSetting.msParameterValSettings, sizeof(msParameterValSetting) * parameterSetting.size);
                    CcspManagementServer_Free(parameterSetting.msParameterValSettings);
                    parameterSetting.msParameterValSettings = pNewSettings;
                    parameterSetting.size *= 2;
                }
            }

            int res = 0;
            if(objectID == ManagementServerID){
                switch (parameterID)
                {
                case ManagementServerEnableCWMPID:
                //case ManagementServerPeriodicInformEnableID:
                case ManagementServerACSOverrideID:
                case ManagementServerUpgradesManagedID:
                case ManagementServerSTUNEnableID:
                case ManagementServerNATDetectedID:
                    res = CcspManagementServer_ValidateBoolean(val[i].parameterValue);
                    if(res == -1 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else if(res == 0) parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString("0");
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString("1");
                    break;
				case ManagementServerPeriodicInformEnableID:
				    res = CcspManagementServer_ValidateBoolean(val[i].parameterValue);
                    if(res == -1 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else if(res == 0)
					{
						parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString("0");
					}
                    else
					{
						parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString("1");
					}
					res = PSM_Set_Record_Value2
                    (
                        bus_handle,
                        CcspManagementServer_SubsystemPrefix,
                        "dmsb.ManagementServer.PeriodicInformEnable",
                        ccsp_string,
                        parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue
                    );
					if(res != CCSP_SUCCESS)
					{
						/* It seems that only chance to invoke roll back is PSM save error. */
						CcspManagementServer_RollBackParameterValues();
					}
					break;
                case ManagementServerURLID:
                {
                    int res = CCSP_FAILURE;
                    int ACSChangedURL = 0;
                    int ACSOverride = CcspManagementServer_GetACSOverride(NULL);
                    char *pValue = NULL;

                    // printf("<RT> %s - WriteID = %d, URL = '%s'\n", __FUNCTION__, writeID, val[i].parameterValue ? val[i].parameterValue : "NULL");

                    res = PSM_Get_Record_Value2(
                                bus_handle,
                                CcspManagementServer_SubsystemPrefix,
                                "dmsb.ManagementServer.ACSChangedURL",
                                NULL,
                                &pValue);
                    if ( res == CCSP_SUCCESS)
                    {
                        // printf("<RT> ACSChangedURL from PSM_Get = '%s'\n", pValue ? pValue : "NULL");
                        if (AnscEqualString(pValue, "0", FALSE) == TRUE)  ACSChangedURL = 0;
                        else ACSChangedURL = 1;
                    }
                    else 
                    {
                        CcspTraceWarning(("%s - Cannot get PSM value for '%s'\n", __FUNCTION__, "dmsb.ManagementServer.ACSChangedURL"));
                        returnStatus = TR69_INTERNAL_ERROR;
                    }
                    if(pValue) CcspManagementServer_Free(pValue);

                    if ( returnStatus == 0)  /* check writeID */
                    {
                        switch( writeID )
                        {
                            case (DSLH_MPA_ACCESS_CONTROL_SNMP): /* from CM Config file */
                            {
                                //printf("ACSOverride = %d, ACSChangedURL = %d\n", ACSOverride, ACSChangedURL);  
                                if ( !ACSOverride && ACSChangedURL ) returnStatus = TR69_UPDATE_NON_WRITABLE_PARAMETER;      
                            }
                            break;

                            case (DSLH_MPA_ACCESS_CONTROL_ACS): /* from ACS */
                            {
                                char *url = NULL;
                                if ( !ACSChangedURL ) 
                                {
                                    url = CcspManagementServer_GetURL(NULL);
                                    if (AnscEqualString(val[i].parameterValue, url, TRUE) == FALSE) g_ACSChangedURL = 1; 
                                    if (url) CcspManagementServer_Free(url);
                                }
                            }
                            break;

                            default:
                            break;
                        }
                    }

                    if ( returnStatus == 0) /* check parameter value */
                    {
                        if(CcspManagementServer_ValidateURL(val[i].parameterValue) != 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                        else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
                    }
                }
                    break;          
                case ManagementServerConnectionRequestURLID:
                case ManagementServerKickURLID:
                case ManagementServerDownloadProgressURLID:
                case ManagementServerUDPConnectionRequestAddressID:
                case ManagementServerSTUNServerAddressID:
                    if(CcspManagementServer_ValidateURL(val[i].parameterValue) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
                    break;
                case ManagementServerUsernameID:
                case ManagementServerPasswordID:
                case ManagementServerConnectionRequestUsernameID:
                case ManagementServerConnectionRequestPasswordID:
                case ManagementServerSTUNUsernameID:
                case ManagementServerSTUNPasswordID:
                case ManagementServerX_CISCO_COM_ConnectionRequestURLPortID:
                case ManagementServerX_CISCO_COM_ConnectionRequestURLPathID:
                    if(CcspManagementServer_ValidateStrLen(val[i].parameterValue, 256) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
                    break;
                case ManagementServerPeriodicInformIntervalID:
                    if(CcspManagementServer_ValidateINT(val[i].parameterValue, TRUE, 1, FALSE, 0) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
					res = PSM_Set_Record_Value2
                    (
                        bus_handle,
                        CcspManagementServer_SubsystemPrefix,
                        "dmsb.ManagementServer.PeriodicInformInterval",
                        ccsp_string,
                        parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue
                    );
					if(res != CCSP_SUCCESS)
					{
						/* It seems that only chance to invoke roll back is PSM save error. */
						CcspManagementServer_RollBackParameterValues();
					}
                    break;
                case ManagementServerCWMPRetryMinimumWaitIntervalID:
                    if(CcspManagementServer_ValidateINT(val[i].parameterValue, TRUE, 1, TRUE, 65535) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
	
					res = PSM_Set_Record_Value2
                    (
                        bus_handle,
                        CcspManagementServer_SubsystemPrefix,
                        "dmsb.ManagementServer.CWMPRetryMinimumWaitInterval",
                        ccsp_string,
                        parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue
                    );
					if(res != CCSP_SUCCESS)
					{
						/* It seems that only chance to invoke roll back is PSM save error. */
						CcspManagementServer_RollBackParameterValues();
					}
                    break;
                case ManagementServerCWMPRetryIntervalMultiplierID:
                    if(CcspManagementServer_ValidateINT(val[i].parameterValue, TRUE, 1000, TRUE, 65535) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
					
					res = PSM_Set_Record_Value2
                    (
                        bus_handle,
                        CcspManagementServer_SubsystemPrefix,
                        "dmsb.ManagementServer.CWMPRetryIntervalMultiplier",
                        ccsp_string,
                        parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue
                    );
					if(res != CCSP_SUCCESS)
					{

						/* It seems that only chance to invoke roll back is PSM save error. */
						CcspManagementServer_RollBackParameterValues();
					}
                    break;
                case ManagementServerSTUNMinimumKeepAlivePeriodID:
                case ManagementServerDefaultActiveNotificationThrottleID:
                    if(CcspManagementServer_ValidateINT(val[i].parameterValue, TRUE, 0, FALSE, 0) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
															printf("<<< calling  PSM_Set_Record_Value2 >>>>>>\n");
					res = PSM_Set_Record_Value2
                    (
                        bus_handle,
                        CcspManagementServer_SubsystemPrefix,
                        "dmsb.ManagementServer.DefaultActiveNotificationThrottle",
                        ccsp_string,
                        parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue
                    );
					if(res != CCSP_SUCCESS)
					{
						/* It seems that only chance to invoke roll back is PSM save error. */
						CcspManagementServer_RollBackParameterValues();
					}
                    break;
                case ManagementServerUDPConnectionRequestAddressNotificationLimitID:
                    parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
                    break;
                case ManagementServerSTUNServerPortID:
                    if(CcspManagementServer_ValidateINT(val[i].parameterValue, TRUE, 0, TRUE, 65535) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
                    break;
                case ManagementServerPeriodicInformTimeID:
                    if(CcspManagementServer_ValidateDateTime(val[i].parameterValue) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);

					res = PSM_Set_Record_Value2
                    (
                        bus_handle,
                        CcspManagementServer_SubsystemPrefix,
                        "dmsb.ManagementServer.PeriodicInformTime",
                        ccsp_string,
                        parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue
                    );
					if(res != CCSP_SUCCESS)
					{
						/* It seems that only chance to invoke roll back is PSM save error. */
						CcspManagementServer_RollBackParameterValues();
					}
                    break;
                case ManagementServerParameterKeyID:
                    if(CcspManagementServer_ValidateStrLen(val[i].parameterValue, 32) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
                    break;
                case ManagementServerSTUNMaximumKeepAlivePeriodID:
                    if(CcspManagementServer_ValidateINT(val[i].parameterValue, TRUE, -1, FALSE, 0) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
                    break;
                default: break;
                }
            }
            else if(objectID == AutonomousTransferCompletePolicyID){
                switch (parameterID)
                {
                case AutonomousTransferCompletePolicyEnableID:
                    res = CcspManagementServer_ValidateBoolean(val[i].parameterValue);
                    if(res == -1 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else if(res == 0) parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString("0");
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString("1");
                    break;
                case AutonomousTransferCompletePolicyTransferTypeFilterID:
                case AutonomousTransferCompletePolicyResultTypeFilterID:
                    parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
                    break;
                case AutonomousTransferCompletePolicyFileTypeFilterID:
                    if(CcspManagementServer_ValidateStrLen(val[i].parameterValue, 1024) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
                    break;
                default: break;
                }
            }
            else if(objectID == DUStateChangeComplPolicyID){
                switch (parameterID)
                {
                case DUStateChangeComplPolicyEnableID:
                    res = CcspManagementServer_ValidateBoolean(val[i].parameterValue);
                    if(res == -1 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else if(res == 0) parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString("0");
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString("1");
                    break;
                case DUStateChangeComplPolicyOperationTypeFilterID:
                case DUStateChangeComplPolicyResultTypeFilterID:
                case DUStateChangeComplPolicyFaultCodeFilterID:
                    parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
                    break;
                default: break;
                }
            }
            if(objectID == LoggingID){
                switch (parameterID)
                {
                case LoggingEnableID:
                    res = CcspManagementServer_ValidateBoolean(val[i].parameterValue);
                    if(res == -1 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else if(res == 0) parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString("false");
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString("true");
                    break;
                case LoggingLogLevelID:
                    if(CcspManagementServer_ValidateINT(val[i].parameterValue, FALSE, 0, FALSE, 0) != 0 && returnStatus == 0) returnStatus = TR69_INVALID_PARAMETER_VALUE;
                    else parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterValue = CcspManagementServer_CloneString(val[i].parameterValue);
                    break;
                default: break;
                }
            }

            if( objectID == CWMPID)
            {
	      CcspManagementServer_ValidateParameterValuesCustom(sessionId,writeID,val,size,invalidParameterName);
            }
            
            if( returnStatus != 0 )
            {
                if (!invalidParaName )
                {
                    invalidParaName = CcspManagementServer_CloneString(val[i].parameterName);
                }
            }
            parameterSetting.msParameterValSettings[parameterSetting.currIndex].objectID = objectID;
            parameterSetting.msParameterValSettings[parameterSetting.currIndex].parameterID = parameterID;
            parameterSetting.msParameterValSettings[parameterSetting.currIndex].backupStatus = BackupNewValue;
            parameterSetting.currIndex++;
        }
    }
    int j;
    /* Do not allow duplicate parameter names. */
    for(i=0; i<size; i++)
    {
        for(j = i+1; j<size; j++)
        {
            if(parameterSetting.msParameterValSettings[i].objectID == parameterSetting.msParameterValSettings[j].objectID
                && parameterSetting.msParameterValSettings[i].parameterID == parameterSetting.msParameterValSettings[j].parameterID
                && returnStatus == 0){
                if(!invalidParaName) invalidParaName = CcspManagementServer_CloneString(val[i].parameterName);
                returnStatus = TR69_INVALID_ARGUMENTS;
            }
        }
    }

    if ( parameterSetting.currIndex == prevIndex )
    {
        if ( returnStatus == 0 )
        {
            returnStatus = TR69_INVALID_ARGUMENTS;
        }
    }

    if ( invalidParaName )
    {
        *invalidParameterName = invalidParaName;
    }

    return returnStatus;
}

/* There is dismatch between objectID here and objectID in PA. */
int CcspManagementServer_GetPAObjectID
(
    int objectID
)
{
    switch (objectID)
    {
    case ManagementServerID: return CCSP_MSVC_OBJECT_ManagementServer;
    case CWMPID:
    {
      int nRetVal = CcspManagementServer_GetPAObjectIDCustom(objectID);
      if(nRetVal != -1)
      {
	return nRetVal;
      }
    }
    break;
    
    case AutonomousTransferCompletePolicyID: return CCSP_MSVC_OBJECT_ATCP;
    case DUStateChangeComplPolicyID: return CCSP_MSVC_OBJECT_DSCCP;
    case SupportedDataModelID: return -1; /* This is useless here since SupportedDataModel cannot be changed. */
    default: return -1;
    }
    return -1;
}

/* CcspManagementServer_IsEncryptedFileInDB() */
int CcspManagementServer_IsEncryptedFileInDB( int parameterID, int *pIsEncryptFileAvailable )
{
	int returnStatus  = 0;

	//Validate passed arguments
	if( NULL == pIsEncryptFileAvailable )
	{
		return TR69_INVALID_ARGUMENTS;
	}

	*pIsEncryptFileAvailable = 0;
	
	if( ManagementServerPasswordID == parameterID )
	{
		FILE	*fp 			  = NULL;
		
		//	Check whether input file is existing or not
		if ( ( fp = fopen ( "/nvram/.keys/MgmtPwdID", "r" ) ) != NULL ) 
		{
			fclose( fp );
			fp = NULL;
			*pIsEncryptFileAvailable = 1;
			AnscTraceWarning((" TR069 %s %d : MgmtPwdID file available\n", __FUNCTION__, __LINE__));
		}
		else
		{
			AnscTraceWarning((" TR069 %s %d : MgmtPwdID file not available\n", __FUNCTION__, __LINE__));
		}
	}
	else if ( ManagementServerConnectionRequestPasswordID == parameterID )
	{
		FILE	*fp 			  = NULL;
		
		//	Check whether input file is existing or not
		if ( ( fp = fopen ( "/nvram/.keys/MgmtCRPwdID", "r" ) ) != NULL ) 
		{
			fclose( fp );
			fp = NULL;
			*pIsEncryptFileAvailable = 1;
			AnscTraceWarning((" TR069 %s %d : MgmtCRPwdID file available\n", __FUNCTION__, __LINE__));
		}
		else
		{
			AnscTraceWarning((" TR069 %s %d : MgmtCRPwdID file not available\n", __FUNCTION__, __LINE__));
		}
	}
	else if ( ManagementServerSTUNPasswordID == parameterID )
	{
		FILE	*fp 			  = NULL;
		
		//	Check whether input file is existing or not
		if ( ( fp = fopen ( "/nvram/.keys/MgmtCRSTUNPwdID", "r" ) ) != NULL ) 
		{
			fclose( fp );
			fp = NULL;
			*pIsEncryptFileAvailable = 1;
			AnscTraceWarning((" TR069 %s %d : MgmtCRSTUNPwdID file available\n", __FUNCTION__, __LINE__));
		}
		else
		{
			AnscTraceWarning((" TR069 %s %d : MgmtCRSTUNPwdID file not available\n", __FUNCTION__, __LINE__));
		}
	}
	else
	{
		return TR69_INVALID_ARGUMENTS;
	}

	return returnStatus;
}


/* CcspManagementServer_RetrievePassword() */
int CcspManagementServer_RetrievePassword( int parameterID, char *pInputFile, char *pOutputString )
{

	FILE 	*fp 		  	  = NULL;
	int 	 returnstatus 	  = 0,
			 isInputFileExist = 0;

    //Since the TEMP_MGMT_SERV_PWD_PATH is same for all parameterID.
    //We use this method to identify each one
    char TEMP_MGMT_SERV_PWD_PATH[30]={ 0 };
    errno_t rc = -1;

    if( ManagementServerPasswordID == parameterID )
    {
        rc = strncpy_s(TEMP_MGMT_SERV_PWD_PATH, sizeof(TEMP_MGMT_SERV_PWD_PATH), "/tmp/tmpMgmtPWDFile1", strlen("/tmp/tmpMgmtPWDFile1"));
        ERR_CHK(rc);
    }
    else if ( ManagementServerConnectionRequestPasswordID == parameterID )
    {
        rc = strncpy_s(TEMP_MGMT_SERV_PWD_PATH, sizeof(TEMP_MGMT_SERV_PWD_PATH), "/tmp/tmpMgmtPWDFile2", strlen("/tmp/tmpMgmtPWDFile2"));
        ERR_CHK(rc);
    }
    else if ( ManagementServerSTUNPasswordID == parameterID )
    {
        rc = strncpy_s(TEMP_MGMT_SERV_PWD_PATH, sizeof(TEMP_MGMT_SERV_PWD_PATH),  "/tmp/tmpMgmtPWDFile3", strlen("/tmp/tmpMgmtPWDFile3"));
        ERR_CHK(rc);
    }

    //  Check whether input file is existing or not
    if ( ( fp = fopen ( pInputFile, "r" ) ) != NULL ) 
    {
        fclose( fp );
        fp = NULL;
        isInputFileExist = 1;
    }
    else
    {
        returnstatus = TR69_INTERNAL_ERROR;
    }

    //  if  input file is there then decrypt and return string
    if( isInputFileExist )
    {
        v_secure_system( "GetConfigFile %s", TEMP_MGMT_SERV_PWD_PATH );

        if ( ( fp = fopen ( TEMP_MGMT_SERV_PWD_PATH, "r" ) ) != NULL ) 
        {
            char password [ 512 ]	 = { 0 },
                 retPassword [ 512 ] = { 0 };
            int length  = 0;

            if ( fgets ( password, sizeof( password ), fp ) != NULL ) 
            {
                sscanf( password, "%s" ,retPassword );
                length = strlen( retPassword );
                rc = strncpy_s( pOutputString, MAX_SIZE_TMPPWD_VALUE, retPassword, length );
                ERR_CHK(rc);

                rc = memset_s(retPassword, sizeof(retPassword), 0, sizeof(retPassword));
                ERR_CHK(rc);
                rc = memset_s(password, sizeof(password), 0, sizeof(password));
                ERR_CHK(rc);
            }
            else
            {
                AnscTraceWarning(( "%s -- fgets() failed\n", __FUNCTION__ ));
                fclose( fp );
                unlink (TEMP_MGMT_SERV_PWD_PATH);
                return TR69_INTERNAL_ERROR;
            }
        
            fclose( fp );
            unlink (TEMP_MGMT_SERV_PWD_PATH);

        }
        else
        {
            AnscTraceWarning(( "%s -- fopen() failed\n", __FUNCTION__ ));
            return TR69_INTERNAL_ERROR;
        }
    }

    return returnstatus;
}

/* CcspManagementServer_GetMGMTServerPasswordValuesFromDB() */
int CcspManagementServer_GetMGMTServerPasswordValuesFromDB( int parameterID, char *pOutputString )
{
	int returnStatus  = 0;

	//Validate passed arguments
	if( NULL == pOutputString )
	{
		return TR69_INVALID_ARGUMENTS;
	}
	
	if( ManagementServerPasswordID == parameterID )
	{
		returnStatus = CcspManagementServer_RetrievePassword( parameterID,"/nvram/.keys/MgmtPwdID", pOutputString );
	}
	else if ( ManagementServerConnectionRequestPasswordID == parameterID )
	{
		returnStatus = CcspManagementServer_RetrievePassword( parameterID, "/nvram/.keys/MgmtCRPwdID", pOutputString );
	}
	else if ( ManagementServerSTUNPasswordID == parameterID )
	{
		returnStatus = CcspManagementServer_RetrievePassword( parameterID,"/nvram/.keys/MgmtCRSTUNPwdID", pOutputString );
	}

	return returnStatus;
}

/* CcspManagementServer_StoreMGMTServerPasswordValuesintoDB() */
int CcspManagementServer_StoreMGMTServerPasswordValuesintoDB( char *pString, int parameterID )
{
	int returnStatus  = 0;

	//Validate passed arguments
	if( NULL == pString )
	{
		return TR69_INVALID_ARGUMENTS;
	}
	
	if( ManagementServerPasswordID == parameterID )
	{
		v_secure_system("echo %s > /tmp/tempMSPwdFile; mkdir -p /nvram/.keys; SaveConfigFile /tmp/tempMSPwdFile ; rm -rf /tmp/tempMSPwdFile", 
				 pString );
		AnscTraceWarning((" TR069 %s %d : ManagementServerPasswordID Changed\n", __FUNCTION__, __LINE__));
	}
	else if ( ManagementServerConnectionRequestPasswordID == parameterID )
	{
		v_secure_system( "echo %s > /tmp/tempMSCRPwdFile; mkdir -p /nvram/.keys; SaveConfigFile /tmp/tempMSCRPwdFile; rm -rf /tmp/tempMSCRPwdFile", 
				 pString );
		AnscTraceWarning((" TR069 %s %d : ManagementServerConnectionRequestPasswordID Changed\n", __FUNCTION__, __LINE__));
		if (access(CCSP_MGMT_CRPWD_FILE,F_OK)!=0)
			{
			AnscTraceWarning((" TR069 %s %d : %s file is not generated\n", __FUNCTION__, __LINE__,CCSP_MGMT_CRPWD_FILE));
			t2_event_d("SYS_ERROR_NotGenMgmtCRPwdID", 1);
			}
	}
	else if ( ManagementServerSTUNPasswordID == parameterID )
	{
		v_secure_system("echo %s > /tmp/tempMSSTUNPwdFile; mkdir -p /nvram/.keys; SaveConfigFile /tmp/tempMSSTUNPwdFile; rm -rf /tmp/tempMSSTUNPwdFile", 
				 pString );
		AnscTraceWarning((" TR069 %s %d : ManagementServerSTUNPasswordID Changed\n", __FUNCTION__, __LINE__));
	}

	return returnStatus;
}


//Custom
extern int CcspManagementServer_CommitParameterValuesCustom(int parameterID);
/* Commit the parameter setting stored in parameterSetting.
 */
int CcspManagementServer_CommitParameterValues(unsigned int writeID)
{
    int i = 0, objectID, parameterID, paObjectID;
    int objectCommitedStatus[SupportedDataModelID] = {0}; /* We only support four objects (id: 1, 3, 4, 15) here. */
    SLAP_VARIABLE slapVar;
    int res;
    char pRecordName[1000] = {0};
    size_t len1, len2, len3;
    int diagComplete = 0;
    errno_t rc = -1;

    len1 = strlen(CcspManagementServer_ComponentName);
    rc = strncpy_s(pRecordName, sizeof(pRecordName), CcspManagementServer_ComponentName, len1);
    ERR_CHK(rc);
    pRecordName[len1] = '.';
    slapVar.Syntax = SLAP_VAR_SYNTAX_string;
    
    int valueChangeSize = 0;
    parameterSigStruct_t *val = CcspManagementServer_Allocate(parameterSetting.currIndex * sizeof(parameterSigStruct_t));

    for(; (unsigned int)i<parameterSetting.currIndex; i++)
    {
        objectID = parameterSetting.msParameterValSettings[i].objectID;
        parameterID = parameterSetting.msParameterValSettings[i].parameterID;

	//Custom
	int nRetVal = CcspManagementServer_CommitParameterValuesCustom(parameterID);
	if(nRetVal == 1)
	{
	  if ( objectID == CWMPID)
	  {
	      objectID    = ManagementServerID;
	      parameterID = ManagementServerEnableCWMPID;
	  }  
	}

        if ( objectID == ManagementServerID && parameterID == ManagementServerX_CISCO_COM_DiagCompleteID )
        {
            diagComplete = 1;
        }

        if(objectInfo[objectID].parameters[parameterID].access == CCSP_RO)
        {
            continue;
        }
        if(AnscEqualString(objectInfo[objectID].parameters[parameterID].value, parameterSetting.msParameterValSettings[i].parameterValue, TRUE))
        {
            continue;
        }
        if(objectInfo[objectID].parameters[parameterID].notification){
            val[valueChangeSize].parameterName = CcspManagementServer_MergeString(objectInfo[objectID].name, objectInfo[objectID].parameters[parameterID].name);
            val[valueChangeSize].oldValue = CcspManagementServer_CloneString(objectInfo[objectID].parameters[parameterID].value);
            val[valueChangeSize].newValue = CcspManagementServer_CloneString(parameterSetting.msParameterValSettings[i].parameterValue);
            val[valueChangeSize].type = objectInfo[objectID].parameters[parameterID].type;
			val[valueChangeSize].writeID = writeID;
            valueChangeSize++;
       }
        /* No free here. Just exchange the values between parameter and backup. */
        char *backup = objectInfo[objectID].parameters[parameterID].value;
        objectInfo[objectID].parameters[parameterID].value = parameterSetting.msParameterValSettings[i].parameterValue;
        parameterSetting.msParameterValSettings[i].parameterValue = backup;
        parameterSetting.msParameterValSettings[i].backupStatus = BackupOldValue;

		// Needs to be encrypt on NVMEM files
        if ( ( ManagementServerID == objectID ) && \
			  ( ( ManagementServerPasswordID == parameterID ) || \
			    ( ManagementServerConnectionRequestPasswordID == parameterID ) || \
			    ( ManagementServerSTUNPasswordID == parameterID )
			  ) 
			)
		{
			CcspManagementServer_StoreMGMTServerPasswordValuesintoDB( objectInfo[objectID].parameters[parameterID].value,
																  	  parameterID );
		}
		else
		{
			/* PSM write */
			len2 = strlen(objectInfo[objectID].name);
                        rc = strncat_s(pRecordName, sizeof(pRecordName), objectInfo[objectID].name, len2);
			ERR_CHK(rc);
                        len3 = strlen(objectInfo[objectID].parameters[parameterID].name);
                        rc = strncat_s(pRecordName, sizeof(pRecordName), objectInfo[objectID].parameters[parameterID].name, len3);
                        ERR_CHK(rc);
                        rc = strncat_s(pRecordName, sizeof(pRecordName), ".Value", 6);
			ERR_CHK(rc);
			slapVar.Variant.varString = objectInfo[objectID].parameters[parameterID].value;
			
			res = PSM_Set_Record_Value2(
				bus_handle,
				CcspManagementServer_SubsystemPrefix,
				pRecordName,
				ccsp_string,
				slapVar.Variant.varString);
			if(res != CCSP_SUCCESS){
				CcspTraceWarning2("ms", ( "CcspManagementServer_CommitParameterValues PSM write failure %d!=%d: %s----%s.\n", res, CCSP_SUCCESS, pRecordName, slapVar.Variant.varString)); 
				/* It seems that only chance to invoke roll back is PSM save error. */
				CcspManagementServer_RollBackParameterValues();
				goto EXIT1;
			}
		}

        if ( objectID == ManagementServerID && parameterID == ManagementServerURLID && g_ACSChangedURL == 1)
        {
            res = PSM_Set_Record_Value2
                    (
                        bus_handle,
                        CcspManagementServer_SubsystemPrefix,
                        "dmsb.ManagementServer.ACSChangedURL",
                        ccsp_string,
                        "1"
                    );
            if(res != CCSP_SUCCESS)
            {
                CcspTraceWarning2("ms", ( "CcspManagementServer_CommitParameterValues PSM write failure %d!=%d: %s----%s.\n", 
                    res, CCSP_SUCCESS, "dmsb.ManagementServer.ACSChnagedURL", "1")); 
                /* It seems that only chance to invoke roll back is PSM save error. */
                CcspManagementServer_RollBackParameterValues();
                goto EXIT1;
            }
            else
	    {
		g_ACSChangedURL = 0;
        //Commenting out whitelisting as it is not required.
		/*char url[150];
		snprintf(url,sizeof(url),"sh /etc/whitelist.sh %s",slapVar.Variant.varString);
		system(url);*/
	    }
        }

        if(objectID < SupportedDataModelID) objectCommitedStatus[objectID] = 1;

        if(objectID == LoggingID){
            if(parameterID == LoggingEnableID) CcspManagementServer_SetLogging_EnableStr(NULL, objectInfo[objectID].parameters[parameterID].value);
            else if(parameterID == LoggingLogLevelID) CcspManagementServer_SetLogging_LogLevelStr(NULL, objectInfo[objectID].parameters[parameterID].value);
        }
    }

    /* Inform PA about the data change. */
    for(i=0; i<SupportedDataModelID; i++)
    {
        if(objectCommitedStatus[i]){
            /* There is dismatch between objectID here and objectID in PA. */
            paObjectID = CcspManagementServer_GetPAObjectID(i);
            if ( diagComplete ) paObjectID |= CCSP_MSVC_OBJECT_DiagComplete;
            if(paObjectID < 0) continue;
            if(CcspManagementServer_ValueChangeCB != NULL) 
            {
                CcspManagementServer_ValueChangeCB(CcspManagementServer_cbContext, paObjectID);
            }
        }
    }

    if(valueChangeSize > 0){
        res = CcspBaseIf_SendparameterValueChangeSignal (
            bus_handle,
            val,
            valueChangeSize);
        if(res != CCSP_SUCCESS){
            CcspTraceWarning2("ms", ( "CcspManagementServer_CommitParameterValues send value change signal failure %d.\n", res)); 
        }
    }

EXIT1:
    for(i=0; i<valueChangeSize; i++)
    {
        if(val[i].parameterName) CcspManagementServer_Free((void*)val[i].parameterName);
        if(val[i].oldValue) CcspManagementServer_Free((void*)val[i].oldValue);
        if(val[i].newValue) CcspManagementServer_Free((void*)val[i].newValue);
    }
    if(val) CcspManagementServer_Free(val);

    return 0;
}

/* Roll back the parameters already being committed.
 */
int CcspManagementServer_RollBackParameterValues()
{
    int i = 0, objectID, parameterID;
    SLAP_VARIABLE slapVar;
    int res;
    char pRecordName[1000] = {0};
    size_t len1, len2, len3;
    errno_t rc = -1;
    len1 = strlen(CcspManagementServer_ComponentName);
    rc = strncpy_s(pRecordName, sizeof(pRecordName), CcspManagementServer_ComponentName, len1);
    ERR_CHK(rc);
    pRecordName[len1] = '.';
    slapVar.Syntax = SLAP_VAR_SYNTAX_string;

    for(; (unsigned int)i<parameterSetting.currIndex; i++)
    {
        objectID = parameterSetting.msParameterValSettings[i].objectID;
        parameterID = parameterSetting.msParameterValSettings[i].parameterID;
        if(objectInfo[objectID].parameters[parameterID].access == CCSP_RO)
        {
            continue;
        }
        if(parameterSetting.msParameterValSettings[i].backupStatus != BackupOldValue)
        {
            continue;
        }
        if(AnscEqualString(objectInfo[objectID].parameters[parameterID].value, parameterSetting.msParameterValSettings[i].parameterValue, TRUE))
        {
            continue;
        }
        if(objectInfo[objectID].parameters[parameterID].value) CcspManagementServer_Free(objectInfo[objectID].parameters[parameterID].value);
        objectInfo[objectID].parameters[parameterID].value = CcspManagementServer_CloneString(parameterSetting.msParameterValSettings[i].parameterValue);
        parameterSetting.msParameterValSettings[i].backupStatus = NoBackup;

		// Needs to be encrypt on NVMEM files
        if ( ( ManagementServerID == objectID ) && \
			  ( ( ManagementServerPasswordID == parameterID ) || \
			    ( ManagementServerConnectionRequestPasswordID == parameterID ) || \
			    ( ManagementServerSTUNPasswordID == parameterID )
			  ) 
			)
		{
			CcspManagementServer_StoreMGMTServerPasswordValuesintoDB( objectInfo[objectID].parameters[parameterID].value,
																  	  parameterID );
		}
		else
		{
			/* PSM write */
			len2 = strlen(objectInfo[objectID].name);
                        rc = strncat_s(pRecordName, sizeof(pRecordName), objectInfo[objectID].name, len2);
			ERR_CHK(rc);
                        len3 = strlen(objectInfo[objectID].parameters[parameterID].name);
                        rc = strncat_s(pRecordName, sizeof(pRecordName), objectInfo[objectID].parameters[parameterID].name, len3);
                        ERR_CHK(rc);
                        rc = strncat_s(pRecordName, sizeof(pRecordName), ".Value", 6);
			ERR_CHK(rc);
			slapVar.Variant.varString = objectInfo[objectID].parameters[parameterID].value;
			res = PSM_Set_Record_Value2(
				bus_handle,
				CcspManagementServer_SubsystemPrefix,
				pRecordName,
				ccsp_string,
				slapVar.Variant.varString);
			if(res != CCSP_SUCCESS){
				/* It seems that only chance to invoke roll back is PSM save error.
				 * Nothing can be done if another PSM save error happens again during roll back.
				 */
				CcspTraceError2("ms", ( "CcspManagementServer_RollBackParameterValues PSM write failure %d!=%d: %s----%s.\n", res, CCSP_SUCCESS, pRecordName, slapVar.Variant.varString)); 
			}
		}
   }

    return 0;
}

int CcspManagementServer_FreeParameterSetting()
{
    if(!parameterSetting.msParameterValSettings)
    {
        parameterSetting.size = 0;
        return 0;
    }
    int i = 0;
    for(; (unsigned int)i<parameterSetting.currIndex; i++)
    {
        if(parameterSetting.msParameterValSettings[i].parameterValue) {
            CcspManagementServer_Free(parameterSetting.msParameterValSettings[i].parameterValue);
            parameterSetting.msParameterValSettings[i].parameterValue = NULL;
        }
    }
    if(parameterSetting.msParameterValSettings) {
        CcspManagementServer_Free(parameterSetting.msParameterValSettings);
        parameterSetting.msParameterValSettings = NULL;
    }
    parameterSetting.currIndex = 0;
    parameterSetting.size = 0;
    return 0;
}

char * CcspManagementServer_RemoveWhiteString
    (
    const char * src
    )
{
    char *p = (char*)src;
    while(*p != 0){
        if(*p == ' ') p++;
        else break;
    }
    return p;
}

void * CcspManagementServer_Allocate
(
    size_t size
)
{
    return (void *) AnscAllocateMemory(size);
}

void CcspManagementServer_Free
(
    void *p
)
{
    return AnscFreeMemory(p);
}

char * CcspManagementServer_CloneString
    (
    const char * src
    )
{
    errno_t rc = -1;
    if(src == NULL) return NULL;
    size_t len = strlen(src) + 1;
    char * dest = CcspManagementServer_Allocate(len);
    if ( dest )
    {
        rc = strcpy_s(dest, len, src);
        ERR_CHK(rc);
    }
    return dest;
}

char * CcspManagementServer_MergeString
    (
    const char * src1,
    const char * src2
    )
{
    size_t len1 = 0, len2 = 0;
    errno_t rc = -1;
    if(src1 != NULL) len1 = strlen(src1);
    if(src2 != NULL) len2 = strlen(src2);
    size_t len = len1 + len2 + 1;
    if(len <= 1) return NULL;
    char * dest = CcspManagementServer_Allocate(len);
    if ( dest )
    {
        *dest = 0;
        if ( src1 )
        {
            rc = strcpy_s(dest, len, src1);
            ERR_CHK(rc);
        }
        if ( src2 )
        {
            rc = strcat_s(dest, len, src2);
            ERR_CHK(rc);
        }
    }
    return dest;
}

//Custom
extern ANSC_STATUS
CcspManagementServer_RestoreDefaultValuesCustom
    (
        void
    );
    
/* Temporarily hardcode to support restartPA for now, will merge partial factory reset from USGv2 branch later */
ANSC_STATUS
CcspManagementServer_RestoreDefaultValues
    (
        void
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    
    CcspManagementServer_RestoreDefaultValuesCustom();

    return returnStatus;
}

