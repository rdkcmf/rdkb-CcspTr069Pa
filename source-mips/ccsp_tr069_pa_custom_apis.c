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

    File: ccsp_tr069_pa_custom_apis.c

    For Data Model Implementation,
    Common Component Software Platform (CCSP)

    ---------------------------------------------------------------

    Copyright (c) 2011 - 2013, Cisco Systems, Inc.

                    CISCO CONFIDENTIAL
      Unauthorized distribution or copying is prohibited
                    All rights reserved

    No part of this computer software may be reprinted, reproduced or utilized
    in any form or by any electronic, mechanical, or other means, now known or
    hereafter invented, including photocopying and recording, or using any
    information storage and retrieval system, without permission in writing
    from Cisco Systems, Inc.

    -------------------------------------------------------------------

    description:

        This source file defines the platform specific APIs
        required by TR-069 PA.

    ---------------------------------------------------------------

    environment:

        Linux PC sim

    ---------------------------------------------------------------

    author:

        Ding Hua

    ---------------------------------------------------------------

    revision:

        01/02/2014  initial revision.

**********************************************************************/

#include "ansc_platform.h"
#include "ccsp_base_api.h"
#include "ccsp_memory.h"
#include "ccsp_custom.h"
#include "ccsp_types.h"
#include "ccsp_trace.h"

#include "ccsp_management_server.h"
#include "ccsp_management_server_api.h"
#include "ccsp_management_server_pa_api.h"
#include "ccsp_supported_data_model.h"
#include "slap_definitions.h"
#include "ccsp_psm_helper.h"
#include "ansc_string.h"

#include <uuid/uuid.h>
/* Fake, to be removed */
#include "ccsp_cwmp_proco_interface.h"
#include "ccsp_cwmp_cpeco_interface.h"
#include "ccsp_cwmp_sesso_interface.h"
#include "ccsp_tr069pa_psm_keys.h"

#include "ccsp_cwmp_ifo_cfg.h"

#include "ccsp_tr069pa_wrapper_api.h"
extern msObjectInfo *objectInfo;

CCSP_VOID
CcspCwmpsoStartRetryTimerCustom
    (
        PCCSP_CWMP_SESSION_OBJECT                 pMyObject
    )
{
}

CCSP_VOID
CcspCwmpsoInformCustom
    (
       PCCSP_CWMP_CFG_INTERFACE pCcspCwmpCfgIf
    )
{
}

CCSP_VOID
CcspCwmpsoInformCustom1
    (
       PCCSP_CWMP_EVENT pCwmpEvent,
       PCCSP_CWMP_CFG_INTERFACE pCcspCwmpCfgIf
    )
{
}

ANSC_STATUS
CcspCwmpsoInformPopulateTRInformationCustom
    (
        PCCSP_CWMP_PARAM_VALUE          pCwmpParamValueArray,
        ULONG                           *ulPresetParamCount,
        BOOL                            bDevice20OrLater
    )
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    ULONG index_start = *ulPresetParamCount;

    pCwmpParamValueArray[(*ulPresetParamCount)++].Name  = CcspTr069PaCloneString("Device.DeviceSummary"                        );
    pCwmpParamValueArray[(*ulPresetParamCount)++].Name  = CcspTr069PaCloneString("Device.DeviceInfo.HardwareVersion"           );
    pCwmpParamValueArray[(*ulPresetParamCount)++].Name  = CcspTr069PaCloneString("Device.DeviceInfo.SoftwareVersion"           );
    pCwmpParamValueArray[(*ulPresetParamCount)++].Name  = CcspTr069PaCloneString("Device.ManagementServer.ConnectionRequestURL");
    pCwmpParamValueArray[(*ulPresetParamCount)++].Name  = CcspTr069PaCloneString("Device.ManagementServer.ParameterKey"        );

    if ( !bDevice20OrLater )
    {
        pCwmpParamValueArray[(*ulPresetParamCount)++].Name  = CcspTr069PaCloneString("Device.GatewayInfo.ManufacturerOUI"      );
        pCwmpParamValueArray[(*ulPresetParamCount)++].Name  = CcspTr069PaCloneString("Device.GatewayInfo.ProductClass"         );
        pCwmpParamValueArray[(*ulPresetParamCount)++].Name  = CcspTr069PaCloneString("Device.GatewayInfo.SerialNumber"         );
        pCwmpParamValueArray[(*ulPresetParamCount)++].Name  = CcspTr069PaCloneString("Device.LAN.IPAddress"                    );
        pCwmpParamValueArray[(*ulPresetParamCount)++].Name  = CcspTr069PaCloneString("Device.LAN.MACAddress"                   );
    }

    if ( !pCwmpParamValueArray[index_start+0].Name ||
         !pCwmpParamValueArray[index_start+1].Name ||
         !pCwmpParamValueArray[index_start+2].Name ||
         !pCwmpParamValueArray[index_start+3].Name ||
         !pCwmpParamValueArray[index_start+4].Name ||
         ( !bDevice20OrLater && 
            (
            !pCwmpParamValueArray[index_start+5].Name || 
            !pCwmpParamValueArray[index_start+6].Name || 
            !pCwmpParamValueArray[index_start+7].Name || 
            !pCwmpParamValueArray[index_start+8].Name || 
            !pCwmpParamValueArray[index_start+9].Name )
        ) )
    {
        returnStatus = ANSC_STATUS_RESOURCES;
    }

    return returnStatus;
}

CCSP_STRING
CcspManagementServer_GetPeriodicInformTimeStrCustom
    (
        CCSP_STRING                 ComponentName
    )
{

    if(!objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformTimeID].value){
        objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformTimeID].value = CcspManagementServer_CloneString("0001-01-01T00:00:00Z");
    }

    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformTimeID].value);

}

CCSP_VOID
CcspManagementServer_InitCustom
    (
        CCSP_STRING             ComponentName,
        CCSP_STRING             SubsystemPrefix,
        CCSP_HANDLE             hMBusHandle,
        PFN_CCSPMS_VALUECHANGE	msValueChangeCB,
        PFN_CCSP_SYSREADYSIG_CB sysReadySignalCB,
        PFN_CCSP_DIAGCOMPSIG_CB diagCompleteSignalCB,
        CCSP_HANDLE             cbContext,
        CCSP_HANDLE             cpeContext,
        CCSP_STRING             sdmXmlFilename
    )
{
};

void CcspManagementServer_GenerateConnectionRequestURLCustom(
    BOOL fromValueChangeSignal,
    char *newValue,
    char *ipAddr,
    msObjectInfo *objectInfo)
{
  
}
/* Temporarily hardcode to support restartPA for now, will merge partial factory reset from USGv2 branch later */
ANSC_STATUS
CcspManagementServer_RestoreDefaultValuesCustom
    (
        void
    )
{
  return ANSC_STATUS_SUCCESS;
}

int CcspManagementServer_CommitParameterValuesCustom(int parameterID)
{ 
  return 0;
}

int CcspManagementServer_GetPAObjectIDCustom(int objectID)
{  
  return -1;
}

int CcspManagementServer_ValidateParameterValuesCustom(
    int sessionId,
    unsigned int writeID,
    parameterValStruct_t *val,
    int size,
    char ** invalidParameterName
    )
{
 

  return 0;
}

void CcspManagementServer_GetSingleParameterValueCustom(
        int objectID,
        int parameterID,
        parameterValStruct_t *val
    )
{
}

int CcspManagementServer_GetObjectIDCustom(char *p1)
{
  return 0;
}

CCSP_VOID CcspManagementServer_InitDBusCustom(CCSP_Base_Func_CB *cb)
{

  
}

CCSP_VOID
CcspManagementServer_FillInObjectInfoCustom(msObjectInfo *objectInfo)
{
    objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformEnableID].value
        = CcspManagementServer_CloneString("0");

    objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformIntervalID].value
        = CcspManagementServer_CloneString("86400");
  
}

char*
CcspTr069PaSsp_GetCustomForcedInformParamsCustom
    (
        ANSC_HANDLE                 hThisObject,
	char *			     g_ForcedInformParams
    )
{
  
  return NULL;
}

void
CcspTr069PaSsp_CcspCwmpCfgGetDevDataModelVersionCustom
    (
        ANSC_HANDLE                 hThisObject,
        PULONG                      pulDevVerionMajor,
        PULONG                      pulDevVersionMinor
    )
{

}

int
CcspTr069PaSsp_InitCcspCwmpCfgIf_Custom(ANSC_HANDLE hCcspCwmpCpeController)
{
    return 0;
}

ANSC_STATUS
CcspManagementServer_GenerateDefaultUsername
    (
        CCSP_STRING                 pDftUsername,
        PULONG                      pulLength
    )
{
    return  ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CcspManagementServer_GenerateDefaultPassword
    (
        CCSP_STRING                 pDftPassword,
        PULONG                      pulLength
    )
{
    return  ANSC_STATUS_FAILURE;
}
