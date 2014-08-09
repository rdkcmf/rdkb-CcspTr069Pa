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

CCSP_STRING
CcspManagementServer_GetPeriodicInformTimeStrCustom
    (
        CCSP_STRING                 ComponentName
    )
{

return "";
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

char*           Xb3OUI          = "088039";
char*           Xb3ProductClass = "XB3";
unsigned char   Xb3HmacKey[]    = 
    {
        0x7e, 0xbe, 0xf5, 0x57, 0x09, 0x83, 0x00, 0x7f,
        0xbf, 0x6d, 0xa3, 0xd2, 0x51, 0xd9, 0x87, 0x10,
        0x0f, 0xb1, 0xed, 0x74, 0xdb, 0x83, 0x24, 0x6d,
        0x34, 0xc2, 0xcd, 0x65, 0xe1, 0x58, 0xb0, 0x80
    };

ANSC_STATUS
CcspManagementServer_GenerateDefaultPassword
    (
        CCSP_STRING                 pDftPassword,
        PULONG                      pulLength
    )
{
    ANSC_STATUS                     returnStatus;
    int                             val_size;
    parameterValStruct_t**          ppval;

    char *                          parameterNames[3];
    char                            HashStr[128];

    ANSC_CRYPTO_KEY                 key         = {0};
    ANSC_CRYPTO_HASH                hash        = {0};
    ULONG                           hashLength  = 0;

    parameterNames[0] = CcspManagementServer_CloneString("Device.DeviceInfo.ManufactureOUI");
    parameterNames[1] = CcspManagementServer_CloneString("Device.DeviceInfo.ProductClass");
    parameterNames[2] = CcspManagementServer_CloneString("Device.DeviceInfo.SerialNumber");

    returnStatus =
        CcspManagementServer_UtilGetParameterValues
            (
                parameterNames,
                3,
                &val_size,
                &ppval
            );

    if ( (returnStatus == ANSC_STATUS_SUCCESS) && (val_size ==  3) )
    {
        /* ManufactureOUI */
        if ( AnscSizeOfString(ppval[0]->parameterValue) > 0 )
        {
            _ansc_strcpy(HashStr, ppval[0]->parameterValue);
        }
        else
        {
            AnscTraceWarning(("CcspManagementServer_GenerateDefaultPassword - null ManufactureOUI!\n"));
            goto EXIT;
        }

        /* ProductClass */
        if ( AnscSizeOfString(ppval[1]->parameterValue) > 0 )
        {
            _ansc_strcat(HashStr, "-");
            _ansc_strcat(HashStr, ppval[1]->parameterValue);
        }
        else
        {
            AnscTraceWarning(("CcspManagementServer_GenerateDefaultPassword - null ProductClass!\n"));
            goto EXIT;
        }

        /* SerialNumber */
        if ( AnscSizeOfString(ppval[2]->parameterValue) > 0 )
        {
            _ansc_strcat(HashStr, "-");
            _ansc_strcat(HashStr, ppval[2]->parameterValue);
        }
        else
        {
            AnscTraceWarning(("CcspManagementServer_GenerateDefaultPassword - null SerialNumber!\n"));
            goto EXIT;
        }

        key.KeyNumber   = 1;
        key.Length      = sizeof(Xb3HmacKey);
        AnscCopyMemory(key.Value[0], Xb3HmacKey, key.Length);

        hashLength = AnscCryptoHmacSha256Digest(HashStr, AnscSizeOfString(HashStr), &hash, &key);
    }

    if ( hashLength == 0 )
    {
        AnscTraceWarning(("CcspManagementServer_GenerateDefaultPassword - hash result is 0!\n"));
        return  ANSC_STATUS_FAILURE;
    }
    else if ( *pulLength < hashLength )
    {
        return  ANSC_STATUS_MORE_DATA;
    }
    else
    {
        *pulLength  = hashLength;
        
        AnscFromHexToString(hash.Value, pDftPassword, hashLength, TRUE);

        AnscTraceWarning(("CcspManagementServer_GenerateDefaultPassword -- the default password is %s.", pDftPassword));

        return  ANSC_STATUS_SUCCESS;
    }

EXIT:
    CcspManagementServer_UtilFreeParameterValStruct(val_size, &ppval);

    return  NULL;
}

