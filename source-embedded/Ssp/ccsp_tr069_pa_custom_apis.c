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
#include <openssl/hmac.h>
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
#include "stdio.h"
#include <uuid/uuid.h>
/* Fake, to be removed */
#include "ccsp_cwmp_proco_interface.h"
#include "ccsp_cwmp_cpeco_interface.h"
#include "ccsp_cwmp_sesso_interface.h"
#include "ccsp_tr069pa_psm_keys.h"

#include "ccsp_cwmp_ifo_cfg.h"

#include "ccsp_tr069pa_wrapper_api.h"
extern msObjectInfo *objectInfo;
extern char *CcspManagementServer_SubsystemPrefix;
extern void *bus_handle;
static int  DeviceInfoRetrieved = 0;
static char DeviceManufacturerOUI[16] = {0}; //"0000CA"
static char DeviceProductClass[16]    = {0}; // "XB3";
static char DeviceSerialNumber[32]    = {0};
static int  DefaultUsernameGenerated = 0;
static char DefaultUsername[72]      = {0};
static int  DefaultPasswordGenerated = 0;
static char* DeviceDefaultPassword      = NULL;
//static char* SharedKey = NULL;
static char SharedKey[256] = {'\0'};
#define SHAREDKEYPATH "/usr/ccsp/tr069pa/sharedkey"
#define ENCRYPTED_SHAREDKEY_PATH "/dummy.txt"
#define TEMP_SHARED_KEY_PATH "/tmp/tr069sharedkey"



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
    pCwmpParamValueArray[(*ulPresetParamCount)++].Name  = CcspTr069PaCloneString("Device.DeviceInfo.ProvisioningCode"           );
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
		 !pCwmpParamValueArray[index_start+5].Name ||
         ( !bDevice20OrLater && 
            (
            !pCwmpParamValueArray[index_start+6].Name || 
            !pCwmpParamValueArray[index_start+7].Name || 
            !pCwmpParamValueArray[index_start+8].Name || 
            !pCwmpParamValueArray[index_start+9].Name || 
            !pCwmpParamValueArray[index_start+10].Name )
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
      time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		char p[30];
		sprintf(p,"%d-%d-%d%s", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,"T00:00:00Z");
		objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformTimeID].value = CcspManagementServer_CloneString(p);
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
typedef struct _msParameter{
    char * name; /* name without a path */
} msParameter;
msParameter ParamName[] = {
{"dmsb.ManagementServer.PeriodicInformEnable"},
{"dmsb.ManagementServer.PeriodicInformInterval"},
//{"dmsb.ManagementServer.PeriodicInformTime"},
{"dmsb.ManagementServer.DefaultActiveNotificationThrottle"},
{"dmsb.ManagementServer.CWMPRetryMinimumWaitInterval"},
{"dmsb.ManagementServer.CWMPRetryIntervalMultiplier"}
};
int i = 0;
int ArrLen = sizeof(ParamName)/sizeof(char*);
for(i = 0;i < ArrLen;i++)
{
		 char *pValue = NULL;
		 int res;
		 res = PSM_Get_Record_Value2(
                                bus_handle,
                                CcspManagementServer_SubsystemPrefix,
                                ParamName[i].name,
                                NULL,
                                &pValue);
         if ( res == CCSP_SUCCESS)
                    {
						if(0 == strcmp(ParamName[i].name,"dmsb.ManagementServer.PeriodicInformEnable"))
						{
						   objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformEnableID].value
							= CcspManagementServer_CloneString(pValue);
						}
						else if(0 == strcmp(ParamName[i].name,"dmsb.ManagementServer.PeriodicInformInterval"))
						{
						   objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformIntervalID].value
							= CcspManagementServer_CloneString(pValue);			
						}
						/*else if(0 == strcmp(ParamName[i].name,"dmsb.ManagementServer.PeriodicInformTime"))
						{
						   objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformTimeID].value
							= CcspManagementServer_CloneString(pValue);					
						}*/
						else if(0 == strcmp(ParamName[i].name,"dmsb.ManagementServer.DefaultActiveNotificationThrottle"))
						{
						   objectInfo[ManagementServerID].parameters[ManagementServerDefaultActiveNotificationThrottleID].value
							= CcspManagementServer_CloneString(pValue);						
						}
						else if(0 == strcmp(ParamName[i].name,"dmsb.ManagementServer.CWMPRetryMinimumWaitInterval"))
						{
						   objectInfo[ManagementServerID].parameters[ManagementServerCWMPRetryMinimumWaitIntervalID].value
							= CcspManagementServer_CloneString(pValue);						
						}
						else if(0 == strcmp(ParamName[i].name,"dmsb.ManagementServer.CWMPRetryIntervalMultiplier"))
						{
						   objectInfo[ManagementServerID].parameters[ManagementServerCWMPRetryIntervalMultiplierID].value
							= CcspManagementServer_CloneString(pValue);						
						}
                    }
                    else 
                    {
                        CcspTraceWarning(("%s - Cannot get PSM value for '%s'\n", __FUNCTION__, "dmsb.ManagementServer.ACSChangedURL"));
                        res = TR69_INTERNAL_ERROR;
						
                    }
        if (pValue)
        {
            CcspTr069PaFreeMemory(pValue);
            pValue = NULL;
        }
}
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

static ANSC_STATUS CcspTr069PaSsp_GetDeviceInfo
    (
    )
{
    ANSC_STATUS                     returnStatus           = ANSC_STATUS_SUCCESS;
    int                             val_size               = 0;
    parameterValStruct_t**          ppval                  = NULL;
    char *                          parameterNames[3]      = {NULL, NULL, NULL};

    parameterNames[0] = CcspManagementServer_CloneString("Device.DeviceInfo.ManufacturerOUI");
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
        if ( AnscSizeOfString(ppval[0]->parameterValue) > 0 )
        {
            _ansc_strcpy(DeviceManufacturerOUI, ppval[0]->parameterValue);
        }
        else
        {
            AnscTraceWarning(("%s - null ManufacturerOUI!\n", __FUNCTION__));
            returnStatus = ANSC_STATUS_FAILURE;
            goto EXIT;
        }

        if ( AnscSizeOfString(ppval[1]->parameterValue) > 0 )
        {
            _ansc_strcpy(DeviceProductClass, ppval[1]->parameterValue);
        }
        else
        {
            AnscTraceWarning(("%s - null ProductClass!\n", __FUNCTION__));
            returnStatus = ANSC_STATUS_FAILURE;
            goto EXIT;
        }

        if ( AnscSizeOfString(ppval[2]->parameterValue) > 0 )
        {
            _ansc_strcpy(DeviceSerialNumber, ppval[2]->parameterValue);
        }
        else
        {
            AnscTraceWarning(("%s- null SerialNumber!\n", __FUNCTION__));
            returnStatus = ANSC_STATUS_FAILURE;
            goto EXIT;
        }
    }
    
    DeviceInfoRetrieved = 1;

EXIT:
    if(ppval && val_size) { CcspManagementServer_UtilFreeParameterValStruct(val_size, ppval); }
    CcspManagementServer_Free(parameterNames[0]);
    CcspManagementServer_Free(parameterNames[1]);
    CcspManagementServer_Free(parameterNames[2]);
    return returnStatus;
}

static ANSC_STATUS
CcspTr069PaSsp_DeviceDefaultUsernameGenerate
    (
    )
{

    if( !DeviceInfoRetrieved ) 
    {
        if ( CcspTr069PaSsp_GetDeviceInfo() != ANSC_STATUS_SUCCESS) 
        {
            return ANSC_STATUS_FAILURE;
        }
    }

    _ansc_sprintf(DefaultUsername, "%s-%s-%s",  
                  DeviceManufacturerOUI, DeviceProductClass, DeviceSerialNumber); 
        
    AnscTraceWarning(("%s -- default username is '%s'\n", __FUNCTION__, DefaultUsername));

    //DefaultUsernameGenerated = 1;
    
    return ANSC_STATUS_SUCCESS;
}

//#if 0

char * CcspTr069PaSsp_retrieveSharedKey( void )
{
	FILE	*fp 			= NULL;
	char	 key [ 256 ],
		 	 retKey [ 256 ] = { 0 },
		 	 cmd[ 128 ] 	= { 0 };	
	int 	len 			= 0;
	BOOL isEncryptedFileIsThere = FALSE;

	/* 
	  * Check whether encyrpted shared key file is available or not
	  * If there then decrypt and return plain key
	  * If not then return normal plain key
	  */
	if ( ( fp = fopen ( ENCRYPTED_SHAREDKEY_PATH, "r" ) ) != NULL ) 
	{
		fclose( fp ); 
		fp = NULL;
		isEncryptedFileIsThere = TRUE;
	}

	// Do encryption -decryption for comcast image
	if(TRUE == isEncryptedFileIsThere) 
	    
	{
		AnscTraceWarning(("%s -- Encrypted Shared Key Available\n", __FUNCTION__));

		//Decrypt shared key
		snprintf( cmd, sizeof(cmd), "GetConfigFile %s", TEMP_SHARED_KEY_PATH );
		system( cmd );
		
		if ( ( fp = fopen ( TEMP_SHARED_KEY_PATH, "r" ) ) != NULL ) 
		{
			 memset( cmd, 0, sizeof( cmd ) );
			 sprintf( cmd, "rm -rf %s", TEMP_SHARED_KEY_PATH );
		
			if ( fgets ( key, sizeof(key), fp ) != NULL ) 
			{					
				sscanf( key, "%s", retKey );
				len = strlen( retKey );
				strncpy( SharedKey, retKey, len );
				memset( retKey, 0, sizeof( retKey ) );
				memset( key, 0, sizeof( key ) );
			}
			else
			{
				printf("fgets() failed CcspTr069PaSsp_retrieveSharedKey\n");
				fclose(fp);
				system( cmd );
				memset( cmd, 0, sizeof( cmd ) );
				return NULL;
			}
			
			fclose(fp);
			system( cmd );	
			memset( cmd, 0, sizeof( cmd ) );
		}
		else
		{
			 printf("fopen() failed in CcspTr069PaSsp_retrieveSharedKey\n");
			 memset( cmd, 0, sizeof( cmd ) );
			 return NULL;
		}
	}
	else
	{
		AnscTraceWarning(("%s -- Plain Shared Key Available\n", __FUNCTION__));

			if ( (fp = fopen ( SHAREDKEYPATH, "r" )) != NULL ) 
		{
			if ( fgets ( key, sizeof(key), fp ) != NULL ) 
				{					
					strip_line(key);
							len = strlen(key);
				strncpy(SharedKey,key,len);
				memset( key, 0, sizeof( key ) );
				}
				else
				{
						printf("fgets() failed CcspTr069PaSsp_retrieveSharedKey\n");
						fclose(fp);
						return NULL;
				}
			fclose(fp);
		}
		else
		{
				 printf("fopen() failed in CcspTr069PaSsp_retrieveSharedKey\n");
				 return NULL;
		}
	}
      
	return SharedKey;
}
//#endif

static ANSC_STATUS
CcspTr069PaSsp_DeviceDefaultPasswordGenerate
    (
    )
{

    if( !DeviceInfoRetrieved ) 
    {
        if ( CcspTr069PaSsp_GetDeviceInfo() != ANSC_STATUS_SUCCESS) 
        {
            return ANSC_STATUS_FAILURE;
        }
    }

    {
        char                            HashStr[128]    = {0};
        ANSC_CRYPTO_KEY                 key             = {0};
        ANSC_CRYPTO_HASH                hash            = {0};
        ULONG                           hashLength      = 0;
	char *tmp = NULL, *convertTo = NULL;
        //char shKey [ 256 ];
	char cmp[64] = {'\0'};
	char sharedText[128] = {'\0'}, sharedmd[64] = {'\0'};
	int  iIndex = 0, sharedKey_len = 0, sharedText_len = 0,  sharedmd_len = 0;
	#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
	HMAC_CTX ctx;
	#else
	HMAC_CTX *pctx = HMAC_CTX_new();
	#endif
        
	_ansc_sprintf(sharedText, "%s-%s", 
                      DeviceManufacturerOUI, DeviceProductClass);

	//strcpy(SharedKey, SHAREDKEY);
        if(NULL== CcspTr069PaSsp_retrieveSharedKey())
	{
		return ANSC_STATUS_FAILURE;
        }
	sharedKey_len = strlen(SharedKey);

	sharedText_len = strlen(sharedText);

	#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
	HMAC_CTX_init( &ctx);
	HMAC_Init(   &ctx, SharedKey,  sharedKey_len, EVP_sha256());
	HMAC_Update( &ctx, (unsigned char *)sharedText, sharedText_len);
	HMAC_Final(  &ctx, (unsigned char *)sharedmd, (unsigned int *)&sharedmd_len );
	#else
	HMAC_CTX_reset(pctx);
	HMAC_Init(   pctx, SharedKey,  sharedKey_len, EVP_sha256());
	HMAC_Update( pctx, (unsigned char *)sharedText, sharedText_len);
	HMAC_Final(  pctx, (unsigned char *)sharedmd, (unsigned int *)&sharedmd_len );
	#endif

	convertTo = cmp;
	for (iIndex = 0; iIndex < sharedmd_len; iIndex++) 
	{
            sprintf(convertTo,"%02x", sharedmd[iIndex] & 0xff);
	    convertTo += 2;
	}
	/* RDKB-7335, CID-33495, leaked and unused memory as variable is overrided by cmp 
	** DeviceDefaultPassword = (char *) malloc(strlen (cmp) + 3);
	*/
	DeviceDefaultPassword = cmp;

       #if (OPENSSL_VERSION_NUMBER < 0x10100000L)
	HMAC_CTX_cleanup( &ctx );
	#else
	HMAC_CTX_free( pctx );
	#endif
        AnscTraceWarning(("%s -- default password is '%s', hashLength = %d\n", 
                          __FUNCTION__, DeviceDefaultPassword, hashLength));

        //        fprintf(stderr, "<RT> %s -- default password is '%s', hashLength = %d\n", 
        //                          __FUNCTION__, DeviceDefaultPassword, hashLength);

#if defined(_COSA_BCM_MIPS_)

        /*
         *  Write the password to a file
         */
        {
            FILE*              pFile       = NULL;
            mode_t             origMod     = umask(0);

            pFile = fopen("/tmp/acsPasswd.txt", "w");

            if ( pFile )
            {
                fprintf
                (
                        pFile,
                        "Default password length = %d, first 4 bytes %02X%02X%02X%02X, value = %s\n",
                        hashLength,
                        hash.Value[0], hash.Value[1], hash.Value[2], hash.Value[3],
                        DeviceDefaultPassword
                );

                fclose(pFile);
            }
            umask(origMod);
        }

#endif        

        //DefaultPasswordGenerated = 1;
    
        return ANSC_STATUS_SUCCESS;
    }
}
  
ANSC_STATUS
CcspManagementServer_GenerateDefaultUsername
    (
        CCSP_STRING                 pDftUsername,
        PULONG                      pulLength
    )
{

    if( !DefaultUsernameGenerated ) 
    {
        if ( CcspTr069PaSsp_DeviceDefaultUsernameGenerate() != ANSC_STATUS_SUCCESS) 
        {
            return ANSC_STATUS_FAILURE;
        }
    }

    {
        int len = _ansc_strlen(DefaultUsername);
        if ( *pulLength < len )
        {
            return  ANSC_STATUS_MORE_DATA;
        }
        else
        {
            *pulLength = len;
            _ansc_strcpy(pDftUsername, DefaultUsername);
            return ANSC_STATUS_SUCCESS;
        }
    }
}

ANSC_STATUS
CcspManagementServer_GenerateDefaultPassword
    (
        CCSP_STRING                 pDftPassword,
        PULONG                      pulLength
    )
{
    if ( !DefaultPasswordGenerated ) 
    {
        if ( CcspTr069PaSsp_DeviceDefaultPasswordGenerate() != ANSC_STATUS_SUCCESS)
        { 
            return ANSC_STATUS_FAILURE;
        }
    }
    //else
    {
        int len = _ansc_strlen(DeviceDefaultPassword);

        if ( *pulLength < len )
        {
            return  ANSC_STATUS_MORE_DATA;
        }
        else
        {
            *pulLength  = len;
            _ansc_strcpy(pDftPassword, DeviceDefaultPassword);
            return  ANSC_STATUS_SUCCESS;
        }
    }
}
