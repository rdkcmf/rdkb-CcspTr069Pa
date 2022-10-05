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

    module: ccsp_management_server_pa_api.c

        For CCSP management server component

    ---------------------------------------------------------------

    description:

        This file implements all api functions of CCSP management server 
        component called by PA.
        Since PA and management server component shares one process, 
        these api functions are different to component public apis.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Hui Ma 
        Kang Quan

    ---------------------------------------------------------------

    revision:

        06/15/2011    initial revision.

**********************************************************************/
#include "ansc_platform.h"
//#include "ccsp_base_api.h"
#include "string.h"
#include "stdio.h"
#include "ccsp_tr069pa_wrapper_api.h"
#include "ccsp_management_server.h"
#include "ccsp_management_server_pa_api.h"
#include "ccsp_supported_data_model.h"
#include "ccsp_psm_helper.h"
#include "ccsp_cwmp_cpeco_interface.h"
#include "ccsp_cwmp_ifo_sta.h"
#include "Tr69_Tlv.h"
#include "syscfg/syscfg.h"
#include <sys/stat.h>
#define TR69_TLVDATA_FILE "/nvram/TLVData.bin"
#define ETHWAN_FILE     "/nvram/ETHWAN_ENABLE"
#include "secure_wrapper.h"

#define MAX_UDP_VAL  257
#define MAX_BUF_SIZE 256
// TELEMETRY 2.0 //RDKB-25996
#include <telemetry_busmessage_sender.h>

#if defined (INTEL_PUMA7)
//Intel Proposed RDKB Generic Bug Fix from XB6 SDK
#define NO_OF_RETRY 90                 /* No of times the management server will wait before giving up*/
#endif

PFN_CCSPMS_VALUECHANGE  CcspManagementServer_ValueChangeCB;
CCSP_HANDLE             CcspManagementServer_cbContext;
CCSP_HANDLE             CcspManagementServer_cpeContext;
extern char             *CcspManagementServer_ComponentName;
extern char             *CcspManagementServer_SubsystemPrefix;
extern msObjectInfo     *objectInfo;
extern void             *bus_handle;
extern char             *pPAMComponentName;
extern char             *pPAMComponentPath;
extern ULONG            g_ulAllocatedSizePeak;
extern ULONG            g_ulAllocatedSizeCurr;
extern INT              g_iTraceLevel;
extern BOOL             RegisterWanInterfaceDone;
extern char             *pFirstUpstreamIpAddress;

extern char             *g_Tr069PaAcsDefAddr;
extern char             *_SupportedDataModelConfigFile;
static CCSP_BOOL        s_MS_Init_Done  = FALSE;

extern void waitUntilSystemReady(   void*   cbContext);


/* CcspManagementServer_Init is called by PA to register component and
 * load configuration file.
 * Return value - none.
 */

extern CCSP_VOID 
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
    );

/* Customizable default password generation, platform specific
 */
extern ANSC_STATUS
CcspManagementServer_GenerateDefaultPassword
    (
        CCSP_STRING             pDftPassword,
        PULONG                  pulLength
    );

#if defined (INTEL_PUMA7)
//Intel Proposed RDKB Generic Bug Fix from XB6 SDK
//Used to obtain the output from the shell for the given cmd
void _get_shell_output(FILE *fp, char * out, int len)
{    
    char * p;
    if (fp)
    {
        fgets(out, len, fp);
        if ((p = strchr(out, '\n'))) 
        {
           *p = '\0';
        }
    }     
}
#endif

void ReadTr69TlvData()
{
	int                             res;
	char                            recordName[MAX_BUF_SIZE];
	errno_t                         rc               = -1;
    	int                             ind              = -1;

#if defined (INTEL_PUMA7)
	//Intel Proposed RDKB Generic Bug Fix from XB6 SDK
	char out[MAX_BUF_SIZE] = {0};
#endif
	Tr69TlvData *object2=malloc(sizeof(Tr69TlvData));
#if !defined (INTEL_PUMA7)
	FILE * file= fopen(TR69_TLVDATA_FILE, "rb");
#else
	//Intel Proposed RDKB Generic Bug Fix from XB6 SDK
	FILE *file = NULL;
	int watchdog = NO_OF_RETRY;
        FILE *fp = NULL;
	int ret = 0;
	do
	{
		fp = v_secure_popen("r", "sysevent get TLV202-status");
		if(!fp)
		{
		    AnscTraceWarning(("%s Error in opening pipe! \n",__FUNCTION__));
		}
                else
                {
		   _get_shell_output(fp, out, MAX_BUF_SIZE);
		   ret = v_secure_pclose(fp);
		   if(ret !=0) {
                       AnscTraceWarning(("%s Error in closing command pipe! [%d] \n",__FUNCTION__,ret));
		   }
                }
		sleep(1);
		watchdog--;
	}while ((!strstr(out,"success")) && (watchdog != 0));

	if ( watchdog == 0 )
	{
		fprintf(stderr, "\n%s(): Ccsp_GwProvApp haven't been able to initialize TLV Data.\n", __FUNCTION__);
	}

	file = fopen(TR69_TLVDATA_FILE, "rb");
#endif
	if ((file != NULL) && (object2) && (access(ETHWAN_FILE, F_OK) != 0)) //RDKB-40531: As T69_TLVDATA_FILE should not be considered for ETHWAN mode
	{
		size_t nm = fread(object2, sizeof(Tr69TlvData), 1, file);
		fclose(file);
		file = NULL;

		if (nm != 1)
		{
			/*
			   File is corrupted or couldn't be read.
			   Fixme: do we need to handle this case better?
			*/
			memset (object2, 0, sizeof(Tr69TlvData));
		}

		AnscTraceInfo(("%s %s File is available and processing by Tr069\n", __FUNCTION__, TR69_TLVDATA_FILE ));
		AnscTraceInfo(("**********************************************************\n"));
		AnscTraceInfo(("%s -#- FreshBootUp: %d\n", __FUNCTION__, object2->FreshBootUp));
		AnscTraceInfo(("%s -#- , Tr69Enable: %d\n", __FUNCTION__, object2->Tr69Enable));
		AnscTraceInfo(("%s -#- , AcsOverRide: %d\n", __FUNCTION__, object2->AcsOverRide));
		AnscTraceInfo(("%s -#- , EnableCWMP: %d\n", __FUNCTION__, object2->EnableCWMP));
		AnscTraceInfo(("%s -#- , URLchanged: %d\n", __FUNCTION__, object2->URLchanged));
		AnscTraceInfo(("%s -#- , URL: %s\n", __FUNCTION__, ( object2->URL[ 0 ] != '\0' ) ? object2->URL : "NULL"));
		AnscTraceInfo(("**********************************************************\n"));

        // Always fetch the ACSOverride flag from the boot config file.
        // Revert to default value "0" if AcsOverRide flag is not present.
        if (object2->AcsOverRide==1){
            objectInfo[ManagementServerID].parameters[ManagementServerACSOverrideID].value=CcspManagementServer_CloneString("true");
        }
        else {
            objectInfo[ManagementServerID].parameters[ManagementServerACSOverrideID].value=CcspManagementServer_CloneString("false");
        }

		// Check if it's a fresh bootup / boot after factory reset / TR69 was never enabled
		// If TR69 was never enabled, then we will always take URL from boot config file.
          	AnscTraceWarning(("%s -#- FreshBootUp: %d, Tr69Enable: %d\n", __FUNCTION__, object2->FreshBootUp, object2->Tr69Enable));
		if((object2->FreshBootUp == 1) || (object2->Tr69Enable == 0))
		{
			AnscTraceWarning(("%s -#- Inside FreshBootUp=1 OR Tr69Enable=0 \n", __FUNCTION__));
			AnscTraceWarning(("%s -#- ACS URL from PSM DB- %s\n", __FUNCTION__, objectInfo[ManagementServerID].parameters[ManagementServerURLID].value));
			AnscTraceWarning(("%s -#- ACS URL from cmconfig - %s\n", __FUNCTION__, CcspManagementServer_CloneString(object2->URL)));
			object2->FreshBootUp = 0;
			objectInfo[ManagementServerID].parameters[ManagementServerURLID].value = CcspManagementServer_CloneString(object2->URL);
			//on Fresh bootup / boot after factory reset, if the URL is empty, set default URL value
                        if (object2->URL[0] == '\0')
			{
                if (g_Tr069PaAcsDefAddr!= NULL)
                {
                    AnscTraceWarning(("ACS URL = %s  \n",g_Tr069PaAcsDefAddr));
                    objectInfo[ManagementServerID].parameters[ManagementServerURLID].value = CcspManagementServer_CloneString(g_Tr069PaAcsDefAddr);
                }
                else
                {
                    AnscTraceWarning(("Unable to retrieve ACS URL  \n"));
                }
			}
			else
			{
				objectInfo[ManagementServerID].parameters[ManagementServerURLID].value = CcspManagementServer_CloneString(object2->URL);
			}
			// Here, we need to check what is the value that we got through boot config file and update TR69 PA
			if(object2->EnableCWMP == 1)
				objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value = CcspManagementServer_CloneString("true");
			else
				objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value = CcspManagementServer_CloneString("false");
		}
		// During normal boot-up check if TR69 was enabled in device anytime.
		// If TR69 was enabled at least once URL will be already updated. 
		// But we need to get the latest flag value from boot-config file.
		if ((object2->FreshBootUp == 0) && (object2->Tr69Enable == 1))
		{
                        CCSP_STRING acsURL = CcspManagementServer_CloneString(object2->URL);
			AnscTraceWarning(("%s -#-  Inside FreshBootUp=0 AND Tr69Enable=1 \n", __FUNCTION__));
			AnscTraceWarning(("%s -#-  ACS URL from PSM DB- %s\n", __FUNCTION__, objectInfo[ManagementServerID].parameters[ManagementServerURLID].value));
			AnscTraceWarning(("%s -#-  ACS URL from cmconfig - %s\n", __FUNCTION__, acsURL));
                        CcspManagementServer_Free(acsURL);

			if (access(CCSP_MGMT_CRPWD_FILE,F_OK)!=0)
				{
				AnscTraceWarning(("%s -#-  %s file is missing in normal boot scenario\n", __FUNCTION__, CCSP_MGMT_CRPWD_FILE));
                        	t2_event_d("SYS_ERROR_MissingMgmtCRPwdID", 1);
				}

            // Check AcsOverRide flag status
            rc = strcasecmp_s("1",strlen("1"),objectInfo[ManagementServerID].parameters[ManagementServerACSOverrideID].value,&ind);
            ERR_CHK(rc);
            if ( rc != EOK || ind )
            {
                rc = strcasecmp_s("true",strlen("true"),objectInfo[ManagementServerID].parameters[ManagementServerACSOverrideID].value, &ind);
                ERR_CHK(rc);
            }
            if ( rc == EOK && !ind )
            {
                /* If AcsOverRide enabled, EnableCWMP and URL is updated from boot config
                 * Any configuration missing in boot config would continue to use existing value
                 */
                if(object2->EnableCWMP){
                    if(object2->EnableCWMP == 1)
                        objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value = CcspManagementServer_CloneString("true");
                    else
                        objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value = CcspManagementServer_CloneString("false");
                }
                if(object2->URL && objectInfo[ManagementServerID].parameters[ManagementServerURLID].value){
                    rc = strcasecmp_s(object2->URL,strlen(object2->URL),objectInfo[ManagementServerID].parameters[ManagementServerURLID].value,&ind);
                    ERR_CHK(rc);
                    if ( rc != EOK || ind ){
                        AnscTraceInfo(("%s -#- ACS URL in TLV file different from ACS URL in PSM DB. \n", __FUNCTION__));
                        objectInfo[ManagementServerID].parameters[ManagementServerURLID].value = CcspManagementServer_CloneString(object2->URL);
                        _ansc_sprintf(recordName, "%s.%sURL.Value", CcspManagementServer_ComponentName, objectInfo[ManagementServerID].name);
                        res = PSM_Set_Record_Value2(bus_handle, CcspManagementServer_SubsystemPrefix, recordName, ccsp_string, object2->URL);
                        if(res != CCSP_SUCCESS){
                            AnscTraceWarning(("%s -#- Failed to write object2->URL <%s> into PSM!\n", __FUNCTION__, object2->URL));
                        }
                        /* Since we are switching to new ACS server, existing key would not work.
                         * Removing CCSP_MGMT_CRPWD_FILE and marking object2->Tr69Enable=0 to mimic fresh boot scenario
                         */
                        object2->Tr69Enable=0;
                        remove(CCSP_MGMT_CRPWD_FILE);
                        remove(CCSP_MGMT_PWD_FILE);
                    }
                }
            }
            else
            {
                /* If AcsOverRide is true, EnableCWMP and URL values would be updated from bootfile
                 * No need to check again.
                 */

                /* If TR69Enabled is already enabled, then no need to read URL.
                   Update only EnableCWMP value to bbhm. */
                if(object2->EnableCWMP == 1)
                {
                    objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value = CcspManagementServer_CloneString("true");
                }
                else if(object2->EnableCWMP == 0)
                {
                    /* There are possibilities that SNMP can enable TR69. In that case, bbhm will have updated value.
                       We will make the TLV file in sync with the bbhm values.
                       In next boot-up EnableCWMP will again update value from boot-config file*/
                    rc = strcasecmp_s("1",strlen("1"),objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value,&ind);
                    if ( rc != EOK || ind )
                    {
                        rc = strcasecmp_s("true",strlen("true"),objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value, &ind);
                    }
                    if ( rc == EOK && !ind )
                    {
                        object2->EnableCWMP = 1;
                    }
                }
            }
        }

		//Below check is needed to make sure PSM has correct ACS URL. This is required for clients that continue to use ACS url from cmconfig.
		if(objectInfo[ManagementServerID].parameters[ManagementServerURLID].value == NULL)
		{
			if(object2->URL != NULL )
			{
				//We are here because, PSM DB doesnt have a valid ACS url but cmconfig has. In this case, setting value from cmconfig to PSM DB
				AnscTraceWarning(("%s -#- PSM DB reported NULL ACS URL.... Setting URL from cmconfig and continue..\n", __FUNCTION__));
				objectInfo[ManagementServerID].parameters[ManagementServerURLID].value = CcspManagementServer_CloneString(object2->URL);
				_ansc_sprintf(recordName, "%s.%sURL.Value", CcspManagementServer_ComponentName, objectInfo[ManagementServerID].name);
				res = PSM_Set_Record_Value2(bus_handle, CcspManagementServer_SubsystemPrefix, recordName, ccsp_string, object2->URL);

				if(res != CCSP_SUCCESS){
					AnscTraceWarning(("%s -#- Failed to write object2->URL <%s> into PSM!\n", __FUNCTION__, object2->URL));
				}
			}
		}

		/* setting cursor at begining of the file & open file in write mode */
		file= fopen(TR69_TLVDATA_FILE, "wb");
		if (file != NULL) 
		{
			fseek(file, 0, SEEK_SET);
			/* Write the updated object2 to the file*/
			fwrite(object2, sizeof(Tr69TlvData), 1, file);
			free(object2);
			fclose(file);
			file = NULL;
			object2 = NULL;
		}
	}
	else
	{
        char *pValue 								 = NULL;
		int   IsNeed2ApplySyndicationPartnerCFGValue = 1;
		
		AnscTraceWarning(("%s TLV data file is missing!!!\n", __FUNCTION__));
		AnscTraceInfo(("%s %s File is not available so unable to process by Tr069\n", __FUNCTION__, TR69_TLVDATA_FILE ));
		creat("/tmp/.TLVmissedtoparsebytr069",S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

		//Check whether PSM entry is there or not
                rc = memset_s( recordName, sizeof( recordName ), 0, sizeof( recordName ) );
                ERR_CHK(rc);
		_ansc_sprintf(recordName, "%s.%sEnableCWMP.Value", CcspManagementServer_ComponentName, objectInfo[ManagementServerID].name);

        res = PSM_Get_Record_Value2( bus_handle,
						             CcspManagementServer_SubsystemPrefix,
						             recordName,
						             NULL,
						             &pValue );
		
        if( res == CCSP_SUCCESS )
		{
            AnscTraceInfo(("%s PSM_Get_Record_Value2 success %d, name=<%s> value<%s>\n", __FUNCTION__, res, recordName, ( pValue )  ?  pValue : "NULL" ));

            if( NULL != pValue )
            {
			((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(pValue);
            }
			//No Need to apply since PSM entry is available for EnableCWMP param
            IsNeed2ApplySyndicationPartnerCFGValue = 0;
        }

		//Check whether we need to apply syndication config or not
		if( IsNeed2ApplySyndicationPartnerCFGValue )
		{
			char buf[ 8 ] = { 0 };

			//Init syscfg if not already init case
			syscfg_init( );
			
			//Get the Syndication_EnableCWMP value and overwrite always during boot-up when cmconfig file not available case
			if( ( 0 == syscfg_get( NULL, "Syndication_EnableCWMP", buf, sizeof( buf )) ) && \
				( '\0' != buf[ 0 ] ) 
			  )
			{
				int Tr69EnableValue = 0;
				
				//Configure EnableCWMP param based on partner's default json config
                                rc = strcmp_s("true", strlen("true"), buf, &ind );
                                ERR_CHK(rc);
                                if((rc == EOK) && (ind == 0))
				{
					objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value = CcspManagementServer_CloneString("1");
					Tr69EnableValue = 1;
				}
				else
				{
					objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value = CcspManagementServer_CloneString("0");
					Tr69EnableValue = 0;
				}
			
				AnscTraceInfo(("%s Applying Syndication EnableCWMP:%s\n", __FUNCTION__, buf ));
				
				//Overwrite syndication Enable CWMP value
                                rc = memset_s( recordName, sizeof( recordName ), 0, sizeof( recordName ) );
                                ERR_CHK(rc);
				_ansc_sprintf(recordName, "%s.%sEnableCWMP.Value", CcspManagementServer_ComponentName, objectInfo[ManagementServerID].name);
				res = PSM_Set_Record_Value2(bus_handle, CcspManagementServer_SubsystemPrefix, recordName, ccsp_string, ( 1 == Tr69EnableValue ) ?  "1" : "0" );
				
				if( res != CCSP_SUCCESS )
				{
					AnscTraceWarning(("%s -#- Failed to write EnableCWMP <%s> into PSM!\n", __FUNCTION__, buf ));
				}
			}
		}
	}

	/*RDKB-7333, CID-32939, free unused resources before exit */
	if(object2)
	{
		free(object2);
	}
	if(file)
	{
		fclose(file);;
	}

}

CCSP_VOID
CcspManagementServer_Init
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
    size_t nameLen = strlen(ComponentName) + 1;
    errno_t rc = -1;
    int res;
    char recordName[MAX_BUF_SIZE];

    if ( s_MS_Init_Done ) return;

    CcspManagementServer_ComponentName = AnscAllocateMemory(nameLen);
    if(CcspManagementServer_ComponentName == NULL)
    {
        AnscTraceInfo(("Failed in Allocating Memory: %s %d %s", __FUNCTION__, __LINE__, __FILE__ ));
        exit(1);
    }
    rc = strncpy_s(CcspManagementServer_ComponentName, strlen(ComponentName) + 1, ComponentName, nameLen);
    ERR_CHK(rc);

    if ( !SubsystemPrefix )
    {
        CcspManagementServer_SubsystemPrefix = NULL;
    }
    else
    {
        CcspManagementServer_SubsystemPrefix = CcspManagementServer_CloneString(SubsystemPrefix);
    }

    if ( sdmXmlFilename )
    {
        _SupportedDataModelConfigFile = CcspManagementServer_CloneString(sdmXmlFilename); 
    }
    else
    {
        _SupportedDataModelConfigFile = CcspManagementServer_CloneString(_CCSP_MANAGEMENT_SERVER_DEFAULT_SDM_FILE); 
    }

    bus_handle = hMBusHandle;

    if(msValueChangeCB) CcspManagementServer_ValueChangeCB = msValueChangeCB;
    CcspManagementServer_cbContext  = cbContext;
    CcspManagementServer_cpeContext = cpeContext;
    CcspManagementServer_InitDBus();

    
    /* set callback function on Message Bus to handle systemReadySignal */
    if ( sysReadySignalCB )
    {
        CcspBaseIf_Register_Event(hMBusHandle, NULL, "systemReadySignal");

        CcspBaseIf_SetCallback2
            (
                hMBusHandle,
                "systemReadySignal",
                sysReadySignalCB,
                (void*)cbContext
            );   
    }

    /* set callback function on Message Bus to handle diagCompleteSignal */
    if ( diagCompleteSignalCB )
    {
        CcspBaseIf_Register_Event(hMBusHandle, NULL, "diagCompleteSignal");

        CcspBaseIf_SetCallback2
            (
                hMBusHandle,
                "diagCompleteSignal",
                diagCompleteSignalCB, 
                (void*)cbContext
            );   
    }

    /* TODO: Retrieve IP address/MAC address/Serial Number for later use */
	
    //Custom
    CcspManagementServer_InitCustom(ComponentName,
        SubsystemPrefix,
        hMBusHandle,
        msValueChangeCB,
        sysReadySignalCB,
        diagCompleteSignalCB,
        cbContext,
        cpeContext,
        sdmXmlFilename);

    /* This has to be after InitDBus since PSM needs bus_handle. */
    CcspManagementServer_FillInObjectInfo(); 
    CcspManagementServer_RegisterNameSpace();
    CcspManagementServer_DiscoverComponent();
    if(pPAMComponentName && pPAMComponentPath){
        CcspManagementServer_RegisterWanInterface();
    }

    s_MS_Init_Done = TRUE;
	// Function has to be called if TLV_DATA_FILE is present or not
	ReadTr69TlvData();
	if (access(ETHWAN_FILE, F_OK) != -1) //RDKB-40531: URL updated for EWAN mode
	{
		if(objectInfo[ManagementServerID].parameters[ManagementServerURLID].value == NULL)
		{
			//We are here because, PSM DB doesnt have a valid ACS url, device in EWAN mode, setting value from partners_defaults.json to PSM DB
			AnscTraceWarning(("%s -#- PSM DB reported NULL ACS URL.. Device in EWAN mode, setting URL from partners_defaults.json and continue..\n", __FUNCTION__));

			//g_Tr069PaAcsDefAddr has the ACS url from partners_defaults.json, populated via CcspTr069PaSsp_LoadCfgFile() during initialization
			if (g_Tr069PaAcsDefAddr!= NULL)
			{
				AnscTraceWarning(("ACS URL = %s  \n",g_Tr069PaAcsDefAddr));
				objectInfo[ManagementServerID].parameters[ManagementServerURLID].value = CcspManagementServer_CloneString(g_Tr069PaAcsDefAddr);
				rc = memset_s( recordName, sizeof( recordName ), 0, sizeof( recordName ) );                                                                                       
				ERR_CHK(rc);                                                                                                                                                      
				_ansc_sprintf(recordName, "%s.%sURL.Value", CcspManagementServer_ComponentName, objectInfo[ManagementServerID].name);
				res = PSM_Set_Record_Value2(bus_handle, CcspManagementServer_SubsystemPrefix, recordName, ccsp_string, g_Tr069PaAcsDefAddr);
				if(res != CCSP_SUCCESS){
					AnscTraceWarning(("%s -#- Failed to write g_Tr069PaAcsDefAddr <%s> into PSM!\n", __FUNCTION__, g_Tr069PaAcsDefAddr));
				}
			}
			else
			{
				AnscTraceWarning(("Unable to retrieve ACS URL , ACS url retrieved as NULL \n"));
			}

		}
		AnscTraceWarning(("%s -#- ACS URL from PSM DB- %s\n", __FUNCTION__, objectInfo[ManagementServerID].parameters[ManagementServerURLID].value));

	}
    char str[100] = {0};
    _ansc_ultoa(g_ulAllocatedSizeCurr, str, 10);
    objectInfo[MemoryID].parameters[MemoryMinUsageID].value = CcspManagementServer_CloneString(str);

    // To check and wait for system ready signal from CR to proceed further
    waitUntilSystemReady( CcspManagementServer_cbContext );

    //    return  (CCSP_HANDLE)bus_handle;
    return;
}

static CCSP_STRING
CcspManagementServer_GetBooleanValue
(
    CCSP_STRING                 ParameterValue,
    CCSP_STRING                 DefaultValue
)
{
    errno_t rc  = -1;
    int     ind  = -1;
    if(ParameterValue){
       rc = strcasecmp_s("0",strlen("0"),ParameterValue,&ind);
       if ( rc != EOK || ind )
       {
           rc = strcasecmp_s("false",strlen("false"),ParameterValue,&ind);
       }
       if ( rc == EOK && !ind )
       {
          return CcspManagementServer_CloneString("false");
       }
       else
       {
           rc = strcasecmp_s("1",strlen("1"),ParameterValue,&ind);
           if ( rc != EOK || ind )
           {
               rc = strcasecmp_s("true",strlen("true"),ParameterValue,&ind);
           }
           if ( rc == EOK && !ind )
           {
              return CcspManagementServer_CloneString("true");
           }
        }
     }

    if(DefaultValue){
       rc = strcasecmp_s("0",strlen("0"), DefaultValue, &ind);
       if ( rc != EOK || ind )
       {
           rc = strcasecmp_s("false",strlen("false"), DefaultValue, &ind);
       }
       if ( rc == EOK && !ind )
       {
          return CcspManagementServer_CloneString("false");
       }
       else
       {
           rc = strcasecmp_s("1",strlen("1"), DefaultValue, &ind);
           if ( rc != EOK || ind )
           {
               rc = strcasecmp_s("true",strlen("true"), DefaultValue, &ind);
           }
           if ( rc == EOK && !ind )
           {
               return CcspManagementServer_CloneString("true");
           }
       }
    }

    CcspTraceWarning(("Neither Parameter '%s' nor Default '%s' is valid.  Returning "" for BooleanStr!!!\n", 
                      (ParameterValue)?(ParameterValue):"",
                      (DefaultValue)?(DefaultValue):""));    
    return CcspManagementServer_CloneString("");
}

/* CcspManagementServer_GetEnableCWMP is called to get
 * Device.ManagementServer.EnableCWMP.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetEnableCWMP
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    errno_t rc  = -1;
    int ind = -1;

    rc = strcasecmp_s("0",strlen("0"),objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value,&ind);
    if ( rc != EOK || ind )
    {
        rc = strcasecmp_s("false",strlen("false"),objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value,&ind);
    }
    if ( rc == EOK && !ind ) 
    {
        return FALSE;
    }
    else return TRUE;
}
CCSP_STRING
CcspManagementServer_GetEnableCWMPStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_GetBooleanValue(objectInfo[ManagementServerID].parameters[ManagementServerEnableCWMPID].value, "0");
}

/* CcspManagementServer_GetURL is called to get
 * Device.ManagementServer.URL.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetURL
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    CCSP_STRING pStr = objectInfo[ManagementServerID].parameters[ManagementServerURLID].value;
    if ( pStr && AnscSizeOfString(pStr) > 0 )
    {
        AnscTraceWarning(("%s -#- ManagementServerURLID_PSM: %s\n", __FUNCTION__, pStr));
	t2_event_s("acs_split", pStr);				
        return  CcspManagementServer_CloneString(pStr);
    }
    else
    {
     //   #if 0
        if(g_Tr069PaAcsDefAddr && AnscSizeOfString(g_Tr069PaAcsDefAddr) > 0)
        {
            if(pStr)
            {
                CcspManagementServer_Free(pStr);
                objectInfo[ManagementServerID].parameters[ManagementServerURLID].value = NULL;
            }
            objectInfo[ManagementServerID].parameters[ManagementServerURLID].value = CcspManagementServer_CloneString(g_Tr069PaAcsDefAddr);
            return CcspManagementServer_CloneString(g_Tr069PaAcsDefAddr);
        }
        else
      //  #endif
        {
            return  CcspManagementServer_CloneString("");
        }
    }
}

/* CcspManagementServer_GetUsername is called to get
 * Device.ManagementServer.Username.
 * Return value - the parameter value.
 */

/*DH  Customizable default username generation, platform specific*/
ANSC_STATUS
CcspManagementServer_GenerateDefaultUsername
    (
        CCSP_STRING                 pDftUsername,
        PULONG                      pulLength
    );

CCSP_STRING
CcspManagementServer_GetUsername
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    
    CCSP_STRING pUsername = objectInfo[ManagementServerID].parameters[ManagementServerUsernameID].value;

    if ( pUsername && AnscSizeOfString(pUsername) > 0 )
    {
        return  CcspManagementServer_CloneString(pUsername);
    }
    else  
    {
        char        DftUsername[72] = {0};
        ULONG       ulLength        = sizeof(DftUsername) - 1;
        ANSC_STATUS returnStatus    = CcspManagementServer_GenerateDefaultUsername(DftUsername, &ulLength);

        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            AnscTraceWarning(("%s -- default username generation failed\n", __FUNCTION__));
            return  CcspManagementServer_CloneString("");
        }
        else
        {
            // Save Username -- TBD  save it to PSM
            if ( pUsername )
            {
                CcspManagementServer_Free(pUsername);
                objectInfo[ManagementServerID].parameters[ManagementServerUsernameID].value = NULL;
            }
            objectInfo[ManagementServerID].parameters[ManagementServerUsernameID].value = CcspManagementServer_CloneString(DftUsername);
            AnscTraceWarning(("%s -- default username generation Success %s\n", __FUNCTION__, objectInfo[ManagementServerID].parameters[ManagementServerUsernameID].value));
            return  CcspManagementServer_CloneString(DftUsername);
        }
    }
}

/* CcspManagementServer_GetPassword is called to get
 * Device.ManagementServer.Password.
 * Return value - always empty.
 */
CCSP_STRING
CcspManagementServer_GetPassword
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    CCSP_STRING  pStr = objectInfo[ManagementServerID].parameters[ManagementServerPasswordID].value;


    // setting pStr to empty string "" will get the default password back
    if ( pStr && AnscSizeOfString(pStr) > 0 )
    {
        return  CcspManagementServer_CloneString(pStr);
    }
    else 
    {
        char          DftPassword[72] = {0};
        ULONG         ulLength        = sizeof(DftPassword) - 1;
        ANSC_STATUS   returnStatus    = CcspManagementServer_GenerateDefaultPassword(DftPassword, &ulLength);

        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            AnscTraceWarning(("%s-- default password generation failed, return the empty one!\n", __FUNCTION__));
            return  CcspManagementServer_CloneString("");
        }
        else
        {
            //  Save the password -- TBD  save it to PSM
            if ( pStr )
            {
                CcspManagementServer_Free(pStr);
                objectInfo[ManagementServerID].parameters[ManagementServerPasswordID].value = NULL;
            }
            objectInfo[ManagementServerID].parameters[ManagementServerPasswordID].value = CcspManagementServer_CloneString(DftPassword);

            return  CcspManagementServer_CloneString(DftPassword);
        }
    }
}

/* CcspManagementServer_GetPeriodicInformEnable is called to get
 * Device.ManagementServer.PeriodicInformEnable.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetPeriodicInformEnable
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    errno_t rc = -1;
    int ind = -1;
    
    rc = strcasecmp_s("0",strlen("0"),objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformEnableID].value,&ind);
    if ( rc != EOK || ind )
    {
        rc = strcasecmp_s("false",strlen("false"),objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformEnableID].value,&ind);
    }
    if ( rc == EOK && !ind )
    {
       return FALSE;
    }
    else return TRUE; 
}
CCSP_STRING
CcspManagementServer_GetPeriodicInformEnableStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_GetBooleanValue(objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformEnableID].value, "0");
}

/* CcspManagementServer_GetPeriodicInformTime is called to get
 * Device.ManagementServer.PeriodicInformInterval.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetPeriodicInformInterval
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    char*   val = objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformIntervalID].value;
    return  val ? _ansc_atoi(val) : 3600;
}
CCSP_STRING
CcspManagementServer_GetPeriodicInformIntervalStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformIntervalID].value);
}

/* CcspManagementServer_GetPeriodicInformTime is called to get
 * Device.ManagementServer.PeriodicInformTime.
 * Return value - the parameter value.
 */
/*CCSP_UINT
CcspManagementServer_GetPeriodicInformTime
    (
        CCSP_STRING                 ComponentName
    )
{
    char*   val = objectInfo[ManagementServerID].parameters[ManagementServerPeriodicInformTimeID].value;
    return  val ? _ansc_atoi(val) : 0;
}*/


extern CCSP_STRING
CcspManagementServer_GetPeriodicInformTimeStrCustom
    (
        CCSP_STRING                 ComponentName
    );
    
CCSP_STRING
CcspManagementServer_GetPeriodicInformTimeStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_GetPeriodicInformTimeStrCustom(ComponentName);
}
/* CcspManagementServer_GetParameterKey is called to get
 * Device.ManagementServer.ParameterKey.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetParameterKey
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerParameterKeyID].value);
}

/* CcspManagementServer_SetParameterKey is called by PA to set
 * Device.ManagementServer.ParameterKey.
 * This parameter is read-only. So it can only be written by PA.
 * Return value - 0 if success.
 * 
 */
CCSP_INT
CcspManagementServer_SetParameterKey
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pParameterKey                    
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    /* If it is called by PA, set it directly to PSM. */
    int								res;
    char							recordName[MAX_BUF_SIZE];

    if ( objectInfo[ManagementServerID].parameters[ManagementServerParameterKeyID].value ) {
        CcspManagementServer_Free((void*)objectInfo[ManagementServerID].parameters[ManagementServerParameterKeyID].value);
    }

    objectInfo[ManagementServerID].parameters[ManagementServerParameterKeyID].value = 
        CcspManagementServer_CloneString(pParameterKey);
    
    _ansc_sprintf(recordName, "%s.%sParameterKey.Value", CcspManagementServer_ComponentName, objectInfo[ManagementServerID].name);    
    
    CcspTraceInfo2("ms", ("Writing ParameterKey <%s> into PSM key <%s> ...\n", pParameterKey, recordName));
    
    res = PSM_Set_Record_Value2 
             (
                  bus_handle,
                  CcspManagementServer_SubsystemPrefix,
                  recordName,
                  ccsp_string,
                  pParameterKey
              );
    
    if(res != CCSP_SUCCESS){
        CcspTraceWarning2("ms", ("Failed to write ParameterKey <%s> into PSM!\n", pParameterKey));
    }
    
    return res;
}

/* CcspManagementServer_GetConnectionRequestURL is called to get
 * Device.ManagementServer.ConnectionRequestURL.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetConnectionRequestURL
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);

#ifdef _COSA_SIM_

    if( !objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].value) 
    {

        #include <sys/types.h>
        #include <ifaddrs.h>
        #include <netinet/in.h> 
        #include <arpa/inet.h>

        char ipaddr[INET_ADDRSTRLEN] = {0}, buf[128] ={0};
        struct ifaddrs *ifAddrStruct=NULL, *ifa=NULL;

        getifaddrs(&ifAddrStruct);
        for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa ->ifa_addr->sa_family==AF_INET) { // IPv4
                if (strstr(ifa->ifa_name, "eth")) { // get first ethernet interface
                    if(inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, ipaddr, INET_ADDRSTRLEN) != NULL) {
                        CcspTraceDebug(("FirstUpstreamIpInterfaceIPv4Address is %s\n", ipaddr));
                        break;
                    }
                    else ipaddr[0]='\0';
                }
            }
        }
        if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
        
        char *ptr_url = objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].value;
        char *ptr_port = objectInfo[ManagementServerID].parameters[ManagementServerX_CISCO_COM_ConnectionRequestURLPortID].value;
        char *ptr_path = objectInfo[ManagementServerID].parameters[ManagementServerX_CISCO_COM_ConnectionRequestURLPathID].value;
        
        if(ptr_port) sprintf(buf, "http://%s:%s/", ipaddr, ptr_port);
        else         sprintf(buf, "http://%s:%d/", ipaddr, CWMP_PORT);
        if(ptr_path)
        {
	    rc = strcat_s(buf, sizeof(buf), ptr_path);
            ERR_CHK(rc);
        }    
        if(ptr_url) CcspManagementServer_Free(ptr_url);

        objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].value = CcspManagementServer_CloneString(buf);
    }

    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].value);
                                
#else

    if(!pPAMComponentName || !pPAMComponentPath){
        CcspManagementServer_DiscoverComponent();
    }
    if(!RegisterWanInterfaceDone){
        CcspManagementServer_RegisterWanInterface();
    }
    CcspManagementServer_GenerateConnectionRequestURL(FALSE, NULL);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestURLID].value);

#endif
}

/* CcspManagementServer_GetFirstUpstreamIpInterface is called by PA to get value of
 * com.cisco.spvtg.ccsp.pam.Helper.FirstUpstreamIpInterface or psm stored value
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetFirstUpstreamIpAddress
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    if(!pPAMComponentName || !pPAMComponentPath){
        CcspManagementServer_DiscoverComponent();
    }
    if(!RegisterWanInterfaceDone){
        CcspManagementServer_RegisterWanInterface();
    }
    if(!pFirstUpstreamIpAddress){
        /* Have to read from psm */
        char pRecordName[1000] = {0};
        char *pValue = NULL;
        errno_t rc  = -1;
        
        rc = strcpy_s(pRecordName, sizeof(pRecordName), CcspManagementServer_ComponentName);
        ERR_CHK(rc);
        rc = strcat_s(pRecordName, sizeof(pRecordName), ".FirstUpstreamIpAddress.Value");
        ERR_CHK(rc);
        int res = PSM_Get_Record_Value2(
            bus_handle,
            CcspManagementServer_SubsystemPrefix,
            pRecordName,
            NULL,
            &pValue);
        if(res != CCSP_SUCCESS){
            CcspTraceWarning2("ms", ("CcspManagementServer_GetFirstUpstreamIpAddress PSM_Get_Record_Value2 failed %d, name=<%s>, value=<%s>\n", res, pRecordName, pValue ? pValue : "NULL"));
            if(pValue) CcspManagementServer_Free(pValue);
            return NULL;
        }
        if(pValue) {
            pFirstUpstreamIpAddress = CcspManagementServer_CloneString(pValue);
            CcspManagementServer_Free(pValue);
        }
    }
    return CcspManagementServer_CloneString(pFirstUpstreamIpAddress);
}

/* CcspManagementServer_GetConnectionRequestURLPort is called to get
 * Device.ManagementServer.X_CISCO_COM_ConnectionRequestURLPort.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetConnectionRequestURLPort
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerX_CISCO_COM_ConnectionRequestURLPortID].value);
}
/* CcspManagementServer_GetConnectionRequestURLPath is called to get
 * Device.ManagementServer.X_CISCO_COM_ConnectionRequestURLPath.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetConnectionRequestURLPath
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerX_CISCO_COM_ConnectionRequestURLPathID].value);
}

/* CcspManagementServer_GetConnectionRequestUsername is called to get
 * Device.ManagementServer.ConnectionRequestUsername.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetConnectionRequestUsername
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    // return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestUsernameID].value);
    CCSP_STRING pStr = objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestUsernameID].value;

    //    AnscTraceWarning(("%s -- ComponentName = %s...\n", __FUNCTION__, ComponentName));

    // setting pStr to empty string "" will get the default username back
    if ( pStr && AnscSizeOfString(pStr) > 0 )
    {
        return  CcspManagementServer_CloneString(pStr);
    }
    else  
    {
        char        DftUsername[72] = {0};
        ULONG       ulLength        = sizeof(DftUsername) - 1;
        ANSC_STATUS returnStatus    = CcspManagementServer_GenerateDefaultUsername(DftUsername, &ulLength);

        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            AnscTraceWarning(("%s -- default username generation failed, return the empty one!\n", __FUNCTION__));
            return  CcspManagementServer_CloneString("");
        }
        else
        {
            // Save Username -- TBD  save it to PSM
            if ( pStr )
            {
                CcspManagementServer_Free(pStr);
                objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestUsernameID].value = NULL;
            }
            objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestUsernameID].value = CcspManagementServer_CloneString(DftUsername);

            return  CcspManagementServer_CloneString(DftUsername);
        }
    }
}

/* CcspManagementServer_GetConnectionRequestPassword is called to get
 * Device.ManagementServer.ConnectionRequestPassword.
 * Return empty value.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetConnectionRequestPassword
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerConnectionRequestPasswordID].value);
}

/* CcspManagementServer_GetACSOverride is called to get
 * Device.ManagementServer.ACSOverride.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetACSOverride
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    errno_t rc  = -1;
    int ind = -1;
   
    if(objectInfo[ManagementServerID].parameters[ManagementServerACSOverrideID].value != NULL)
    {
        rc = strcasecmp_s("0",strlen("0"),objectInfo[ManagementServerID].parameters[ManagementServerACSOverrideID].value,&ind);
        ERR_CHK(rc);
        if ( rc != EOK || ind )
        {
            rc = strcasecmp_s("false",strlen("false"),objectInfo[ManagementServerID].parameters[ManagementServerACSOverrideID].value,&ind);
            ERR_CHK(rc);
        }
        if ( rc == EOK && !ind )
        {
            return FALSE;
        }
    }
    else
    {
       CcspTr069PaTraceWarning(("WARNING : ManagementServerID - %d, ACSOverride == NULL %s:%d\n", ManagementServerID, __FUNCTION__, __LINE__));
    }
    return TRUE;
}

CCSP_STRING
CcspManagementServer_GetACSOverrideStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_GetBooleanValue(objectInfo[ManagementServerID].parameters[ManagementServerACSOverrideID].value, "0");
}

/* CcspManagementServer_GetUpgradesManaged is called to get
 * Device.ManagementServer.UpgradesManaged.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetUpgradesManaged
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    /* Set as read only and only return TRUE. */
    //    return TRUE;
    errno_t rc  = -1;
    int ind = -1;
   
    rc = strcasecmp_s("0",strlen("0"),objectInfo[ManagementServerID].parameters[ManagementServerUpgradesManagedID].value,&ind);
    if ( rc != EOK || ind )
    {
        rc = strcasecmp_s("false",strlen("false"),objectInfo[ManagementServerID].parameters[ManagementServerUpgradesManagedID].value,&ind);
    }
    if ( rc == EOK && !ind )
    {
       return FALSE;
    }
    else return TRUE;
}
CCSP_STRING
CcspManagementServer_GetUpgradesManagedStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    /* Set as read only and only return TRUE. */
    //    return CcspManagementServer_CloneString("true"); 

    return CcspManagementServer_GetBooleanValue(objectInfo[ManagementServerID].parameters[ManagementServerUpgradesManagedID].value, "0");
}
/* CcspManagementServer_GetKickURL is called to get
 * Device.ManagementServer.KickURL.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetKickURL
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerKickURLID].value);
}

/* CcspManagementServer_GetDownloadProgressURL is called to get
 * Device.ManagementServer.DownloadProgressURL.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetDownloadProgressURL
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerDownloadProgressURLID].value);
}

/* CcspManagementServer_GetDefaultActiveNotificationThrottle is called to get
 * Device.ManagementServer.DefaultActiveNotificationThrottle.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetDefaultActiveNotificationThrottle
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    char*   val = objectInfo[ManagementServerID].parameters[ManagementServerDefaultActiveNotificationThrottleID].value;
    return  val ? _ansc_atoi(val) : 0;
}
CCSP_STRING
CcspManagementServer_GetDefaultActiveNotificationThrottleStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
     if (objectInfo[ManagementServerID].parameters[ManagementServerDefaultActiveNotificationThrottleID].value == NULL)
	  return CcspManagementServer_CloneString("1");
     
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerDefaultActiveNotificationThrottleID].value);
}

/* CcspManagementServer_GetCWMPRetryMinimumWaitInterval is called to get
 * Device.ManagementServer.CWMPRetryMinimumWaitInterval.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetCWMPRetryMinimumWaitInterval
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    char*   val = objectInfo[ManagementServerID].parameters[ManagementServerCWMPRetryMinimumWaitIntervalID].value;
    return  val ? _ansc_atoi(val) : 0;
}
CCSP_STRING
CcspManagementServer_GetCWMPRetryMinimumWaitIntervalStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerCWMPRetryMinimumWaitIntervalID].value);
}
/* CcspManagementServer_GetCWMPRetryIntervalMultiplier is called to get
 * Device.ManagementServer.CWMPRetryIntervalMultiplier.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetCWMPRetryIntervalMultiplier
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    char*   val = objectInfo[ManagementServerID].parameters[ManagementServerCWMPRetryIntervalMultiplierID].value;
    return  val ? _ansc_atoi(val) : 0;
}
CCSP_STRING
CcspManagementServer_GetCWMPRetryIntervalMultiplierStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerCWMPRetryIntervalMultiplierID].value);
}
/* CcspManagementServer_GetUDPConnectionRequestAddress is called to get
 * Device.ManagementServer.UDPConnectionRequestAddress.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetUDPConnectionRequestAddress
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerUDPConnectionRequestAddressID].value);
}

/* CcspManagementServer_GetUDPConnectionRequestAddressNotificationLimit is called to get
 * Device.ManagementServer.UDPConnectionRequestAddressNotificationLimit.
 * Return value - the parameter value.
 */
//CCSP_UINT
CCSP_STRING
CcspManagementServer_GetUDPConnectionRequestAddressNotificationLimit
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerUDPConnectionRequestAddressNotificationLimitID].value);
}

/* CcspManagementServer_GetSTUNEnable is called to get
 * Device.ManagementServer.STUNEnable.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetSTUNEnable
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    errno_t rc  = -1;
    int ind = -1;
    rc = strcasecmp_s("0",strlen("0"),objectInfo[ManagementServerID].parameters[ManagementServerSTUNEnableID].value,&ind);
    if ( rc != EOK || ind )
    {
        rc = strcasecmp_s("false",strlen("false"),objectInfo[ManagementServerID].parameters[ManagementServerSTUNEnableID].value,&ind);
    }
    if ( rc == EOK && !ind )
    {
       return FALSE;
    }
    else return TRUE;
}
CCSP_STRING
CcspManagementServer_GetSTUNEnableStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_GetBooleanValue(objectInfo[ManagementServerID].parameters[ManagementServerSTUNEnableID].value, "0");
}
/* CcspManagementServer_GetSTUNServerAddress is called to get
 * Device.ManagementServer.STUNServerAddress.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSTUNServerAddress
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerSTUNServerAddressID].value);
}

/* CcspManagementServer_GetSTUNServerPort is called to get
 * Device.ManagementServer.STUNServerPort.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetSTUNServerPort
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    char*   val = objectInfo[ManagementServerID].parameters[ManagementServerSTUNServerPortID].value;
    return  val ? _ansc_atoi(val) : 0;
}
CCSP_STRING
CcspManagementServer_GetSTUNServerPortStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerSTUNServerPortID].value);
}
/* CcspManagementServer_GetSTUNUsername is called to get
 * Device.ManagementServer.STUNUsername.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSTUNUsername
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerSTUNUsernameID].value);
}

/* CcspManagementServer_GetSTUNPassword is called to get
 * Device.ManagementServer.STUNPassword.
 * Return empty value.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSTUNPassword
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerSTUNPasswordID].value);
}

/* CcspManagementServer_GetSTUNMaximumKeepAlivePeriod is called to get
 * Device.ManagementServer.STUNMaximumKeepAlivePeriod.
 * Return value - the parameter value.
 */
CCSP_INT
CcspManagementServer_GetSTUNMaximumKeepAlivePeriod
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    char*   val = objectInfo[ManagementServerID].parameters[ManagementServerSTUNMaximumKeepAlivePeriodID].value;
    return  val ? _ansc_atoi(val) : 0;
}
CCSP_STRING
CcspManagementServer_GetSTUNMaximumKeepAlivePeriodStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerSTUNMaximumKeepAlivePeriodID].value);
}
/* CcspManagementServer_GetSTUNMinimumKeepAlivePeriod is called to get
 * Device.ManagementServer.STUNMinimumKeepAlivePeriod.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetSTUNMinimumKeepAlivePeriod
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    char*   val = objectInfo[ManagementServerID].parameters[ManagementServerSTUNMinimumKeepAlivePeriodID].value;
    return  val ? _ansc_atoi(val) : 0;
}
CCSP_STRING
CcspManagementServer_GetSTUNMinimumKeepAlivePeriodStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerSTUNMinimumKeepAlivePeriodID].value);
}
/* CcspManagementServer_GetNATDetected is called to get
 * Device.ManagementServer.NATDetected.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetNATDetected
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    errno_t rc  = -1;
    int ind = -1;
     
    if(objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].value == NULL)
    {
        return TRUE;
    }
    
    rc = strcasecmp_s("0", strlen("0"), objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].value, &ind);
    if ( rc != EOK || ind )
    {
        rc = strcasecmp_s("false", strlen("false"), objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].value, &ind);
    }
    if ( rc == EOK && !ind )
    {
        return FALSE;
    }
    else return TRUE;
}
CCSP_STRING
CcspManagementServer_GetNATDetectedStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_GetBooleanValue(objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].value, "0");
}

/* CcspManagementServer_GetAliasBasedAddressing is called to get
 * Device.ManagementServer.AliasBasedAddressing
 * Return value - the parameter value
 */
// Currently set to a permanent value of FALSE.  
CCSP_BOOL
CcspManagementServer_GetAliasBasedAddressing
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    errno_t rc  = -1;
    int ind = -1;
    
    rc = strcasecmp_s("0",strlen("0"),objectInfo[ManagementServerID].parameters[ManagementServerAliasBasedAddressingID].value,&ind);
    if ( rc != EOK || ind )
    {
        rc = strcasecmp_s("false",strlen("false"),objectInfo[ManagementServerID].parameters[ManagementServerAliasBasedAddressingID].value,&ind);
    }
    if ( rc == EOK && !ind )
    {
       return FALSE;
    }
    else return TRUE;
}
CCSP_STRING
CcspManagementServer_GetAliasBasedAddressingStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_GetBooleanValue(objectInfo[ManagementServerID].parameters[ManagementServerAliasBasedAddressingID].value, "0");
}

/* 
 * Device.ManagementServer.ManageableDevice.{i}.
 * is not supported. So Device.ManagementServer.ManageableDeviceNumberOfEntries 
 * is not supported here.
 * 
 */

/* CcspManagementServer_GetAutonomousTransferCompletePolicy_Enable is called to get
 * Device.ManagementServer.AutonomousTransferCompletePolicy.Enable
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetAutonomousTransferCompletePolicy_Enable
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    errno_t rc  = -1;
    int ind = -1;
    
    rc =strcasecmp_s("0",strlen("0"),objectInfo[AutonomousTransferCompletePolicyID].parameters[AutonomousTransferCompletePolicyEnableID].value,&ind);
    if ( rc != EOK || ind )
    {
        rc =strcasecmp_s("false",strlen("false"),objectInfo[AutonomousTransferCompletePolicyID].parameters[AutonomousTransferCompletePolicyEnableID].value,&ind);
    }
    if ( rc == EOK && !ind )
    {
       return FALSE;
    }
    else return TRUE;
}
CCSP_STRING
CcspManagementServer_GetAutonomousTransferCompletePolicy_EnableStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_GetBooleanValue(objectInfo[AutonomousTransferCompletePolicyID].parameters[AutonomousTransferCompletePolicyEnableID].value, "0");
}

/* CcspManagementServer_GetAutonomousTransferCompletePolicy_TransferTypeFilter is called to get
 * Device.ManagementServer.AutonomousTransferCompletePolicy.TransferTypeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetAutonomousTransferCompletePolicy_TransferTypeFilter
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[AutonomousTransferCompletePolicyID].parameters[AutonomousTransferCompletePolicyTransferTypeFilterID].value);
}

/* CcspManagementServer_GetAutonomousTransferCompletePolicy_ResultTypeFilter is called to get
 * Device.ManagementServer.AutonomousTransferCompletePolicy.ResultTypeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetAutonomousTransferCompletePolicy_ResultTypeFilter
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[AutonomousTransferCompletePolicyID].parameters[AutonomousTransferCompletePolicyResultTypeFilterID].value);
}

/* CcspManagementServer_GetAutonomousTransferCompletePolicy_FileTypeFilter is called to get
 * Device.ManagementServer.AutonomousTransferCompletePolicy.FileTypeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetAutonomousTransferCompletePolicy_FileTypeFilter
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[AutonomousTransferCompletePolicyID].parameters[AutonomousTransferCompletePolicyFileTypeFilterID].value);
}

/* CcspManagementServer_GetDUStateChangeComplPolicy_Enable is called to get
 * Device.ManagementServer.DUStateChangeComplPolicy.Enable
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetDUStateChangeComplPolicy_Enable
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    errno_t rc  = -1;
    int ind = -1;

    rc = strcasecmp_s("0",strlen("0"),objectInfo[DUStateChangeComplPolicyID].parameters[DUStateChangeComplPolicyEnableID].value,&ind);
    if ( rc != EOK || ind )
    {
        rc = strcasecmp_s("false",strlen("false"),objectInfo[DUStateChangeComplPolicyID].parameters[DUStateChangeComplPolicyEnableID].value,&ind);
    }
    if ( rc == EOK && !ind )
    {
       return FALSE;
    }
    else return TRUE;
}

CCSP_STRING
CcspManagementServer_GetDUStateChangeComplPolicy_EnableStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_GetBooleanValue(objectInfo[DUStateChangeComplPolicyID].parameters[DUStateChangeComplPolicyEnableID].value, "0");
}

/* CcspManagementServer_GetDUStateChangeComplPolicy_OperationTypeFilter is called to get
 * Device.ManagementServer.DUStateChangeComplPolicy.OperationTypeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetDUStateChangeComplPolicy_OperationTypeFilter
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[DUStateChangeComplPolicyID].parameters[DUStateChangeComplPolicyOperationTypeFilterID].value);
}

/* CcspManagementServer_GetDUStateChangeComplPolicy_ResultTypeFilter is called to get
 * Device.ManagementServer.DUStateChangeComplPolicy.ResultTypeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetDUStateChangeComplPolicy_ResultTypeFilter
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[DUStateChangeComplPolicyID].parameters[DUStateChangeComplPolicyResultTypeFilterID].value);
}

/* CcspManagementServer_GetDUStateChangeComplPolicy_FaultCodeFilter is called to get
 * Device.ManagementServer.DUStateChangeComplPolicy.FaultCodeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetDUStateChangeComplPolicy_FaultCodeFilter
    (
        CCSP_STRING                 ComponentName
        )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[DUStateChangeComplPolicyID].parameters[DUStateChangeComplPolicyFaultCodeFilterID].value);
}

/* CcspManagementServer_GetTr069pa_Name is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Name
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_Name
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[Tr069paID].parameters[Tr069paNameID].value);
}

/* CcspManagementServer_GetTr069pa_Version is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Version
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_Version
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[Tr069paID].parameters[Tr069paVersionID].value);
}

/* CcspManagementServer_GetTr069pa_Author is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Author
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_Author
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[Tr069paID].parameters[Tr069paAuthorID].value);
}

/* CcspManagementServer_GetTr069pa_Health is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Health
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_Health
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[Tr069paID].parameters[Tr069paHealthID].value);
}

/* CcspManagementServer_GetTr069pa_State is called to get
 * com.cisco.spvtg.ccsp.tr069pa.State
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_State
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[Tr069paID].parameters[Tr069paStateID].value);
}

/* CcspManagementServer_GetTr069pa_DTXml is called to get
 * com.cisco.spvtg.ccsp.tr069pa.DTXml
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_DTXml
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_CloneString(objectInfo[Tr069paID].parameters[Tr069paDTXmlID].value);
}

/* CcspManagementServer_GetMemory_MinUsageStr is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Memory.MinUsage
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetMemory_MinUsageStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    /* MinUsage is the memory consumed right after init. It does not change. */
    return CcspManagementServer_CloneString(objectInfo[MemoryID].parameters[MemoryMinUsageID].value);
}

/* CcspManagementServer_GetMemory_MaxUsage is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Memory.MaxUsage
 * Return value - the parameter value.
 */
ULONG
CcspManagementServer_GetMemory_MaxUsage
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return g_ulAllocatedSizePeak;
}

/* CcspManagementServer_GetMemory_MaxUsageStr is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Memory.MaxUsage
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetMemory_MaxUsageStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    char str[100] = {0};
    _ansc_ultoa(g_ulAllocatedSizePeak, str, 10);

    //return CcspManagementServer_CloneString(objectInfo[MemoryID].parameters[MemoryMaxUsageID].value);
    return CcspManagementServer_CloneString(str);
}

/* CcspManagementServer_GetMemory_Consumed is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Memory.Consumed
 * Return value - the parameter value.
 */
ULONG
CcspManagementServer_GetMemory_Consumed
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return g_ulAllocatedSizeCurr;
}

/* CcspManagementServer_GetMemory_ConsumedStr is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Memory.Consumed
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetMemory_ConsumedStr
    (
        CCSP_STRING                 ComponentName
    )
{
    char str[100] = {0};
    _ansc_ultoa(CcspManagementServer_GetMemory_Consumed(ComponentName), str, 10);

    //return CcspManagementServer_CloneString(objectInfo[MemoryID].parameters[MemoryConsumedID].value);
    return CcspManagementServer_CloneString(str);
}

/* CcspManagementServer_GetLogging_EnableStr is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Logging.Enable
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetLogging_EnableStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    return CcspManagementServer_GetBooleanValue(objectInfo[LoggingID].parameters[LoggingEnableID].value, "0");
}

/* CcspManagementServer_SetLogging_EnableStr is called to set
 * com.cisco.spvtg.ccsp.tr069pa.Logging.Enable
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetLogging_EnableStr
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 Value
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    errno_t rc  = -1;
    int ind = -1;

    rc = strcasecmp_s(Value, strlen(Value), objectInfo[LoggingID].parameters[LoggingEnableID].value, &ind);
    ERR_CHK(rc);
    if ((!ind) && (rc == EOK))
    {
        return CCSP_SUCCESS;
    }
    if(objectInfo[LoggingID].parameters[LoggingEnableID].value) 
        CcspManagementServer_Free(objectInfo[LoggingID].parameters[LoggingEnableID].value);
    objectInfo[LoggingID].parameters[LoggingEnableID].value = NULL;

    rc = strcasecmp_s("TRUE",strlen("TRUE"),Value,&ind);
    if ( rc != EOK || ind )
    {
        rc = strcasecmp_s("1",strlen("1"),Value,&ind);
    }
    if ( rc == EOK && !ind )
    {
        objectInfo[LoggingID].parameters[LoggingEnableID].value = CcspManagementServer_CloneString("true");
        if(objectInfo[LoggingID].parameters[LoggingLogLevelID].value)
            AnscSetTraceLevel(_ansc_atoi(objectInfo[LoggingID].parameters[LoggingLogLevelID].value));
    }
    else
    {
        objectInfo[LoggingID].parameters[LoggingEnableID].value = CcspManagementServer_CloneString("false");
        AnscSetTraceLevel(CCSP_TRACE_INVALID_LEVEL);
    }
    return CCSP_SUCCESS;
}

/* CcspManagementServer_GetLogging_LogLevelStr is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Logging.LogLevel
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetLogging_LogLevelStr
    (
        CCSP_STRING                 ComponentName
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    if(objectInfo[LoggingID].parameters[LoggingLogLevelID].value) {
        return CcspManagementServer_CloneString(objectInfo[LoggingID].parameters[LoggingLogLevelID].value);
    }
    else
    {
        char str[100] = {0};
        //        _ansc_itoa(g_iTraceLevel, str, 10);
        sprintf(str, "%d", g_iTraceLevel);
        return CcspManagementServer_CloneString(str);
    }
}

/* CcspManagementServer_SetLogging_LogLevelStr is called to set
 * com.cisco.spvtg.ccsp.tr069pa.Logging.LogLevel
 * Return value - 0 if success.
 */
CCSP_STRING
CcspManagementServer_SetLogging_LogLevelStr
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 Value
    )
{
    UNREFERENCED_PARAMETER(ComponentName);
    int level = _ansc_atoi(Value);
    errno_t rc  = -1;
    int ind = -1;
    
    rc = strcasecmp_s("TRUE",strlen("TRUE"),objectInfo[LoggingID].parameters[LoggingEnableID].value,&ind);
    ERR_CHK(rc);
    if((level != g_iTraceLevel) && ((!ind) && (rc == EOK)))
    {
        AnscSetTraceLevel(level);
    }
    if(objectInfo[LoggingID].parameters[LoggingLogLevelID].value) {
    rc = strcasecmp_s(Value, strlen(Value), objectInfo[LoggingID].parameters[LoggingLogLevelID].value, &ind);
    ERR_CHK(rc);
    if((ind) && (rc == EOK)){
           CcspManagementServer_Free(objectInfo[LoggingID].parameters[LoggingLogLevelID].value);
           objectInfo[LoggingID].parameters[LoggingLogLevelID].value = CcspManagementServer_CloneString(Value);
        }
    }
    else {
        objectInfo[LoggingID].parameters[LoggingLogLevelID].value = CcspManagementServer_CloneString(Value);
    }
    return CcspManagementServer_CloneString("0");  // return CCSP_SUCCESS;
}

#ifdef   _CCSP_CWMP_STUN_ENABLED
CCSP_VOID
CcspManagementServer_StunBindingChanged
    (
        CCSP_BOOL                   NATDetected,
        char*                       UdpConnReqURL
    )
{
    CCSP_BOOL                       bPrevNatDetected = FALSE;
    char*                           pOldNatDetected = objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].value;
    char*                           pOldUrl = objectInfo[ManagementServerID].parameters[ManagementServerUDPConnectionRequestAddressID].value;
    parameterSigStruct_t            valChanged[2];
    int                             valChangedSize = 0;
    errno_t                         rc             = -1;
    int                             ind               = -1;
    rc = memset_s(valChanged, sizeof(parameterSigStruct_t)*2, 0, sizeof(parameterSigStruct_t)*2);
    ERR_CHK(rc);

    /* NATDetected */
    if ( objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].value ) 
    {
        rc = strcmp_s("0",strlen("0"),objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].value,&ind);
        ERR_CHK(rc);
        if (rc == EOK)
        {
           bPrevNatDetected = !(!ind);
        }
    }

    objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].value = 
        CcspManagementServer_CloneString(NATDetected ? "1" : "0");
        
    if ( objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].notification &&
         ((bPrevNatDetected && !NATDetected) || (!bPrevNatDetected && NATDetected) ) )
    {
        valChanged[valChangedSize].parameterName = CcspManagementServer_MergeString(objectInfo[ManagementServerID].name, objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].name);
        valChanged[valChangedSize].oldValue = pOldNatDetected;
        pOldNatDetected = NULL;
        valChanged[valChangedSize].newValue = CcspManagementServer_CloneString(objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].value);
        valChanged[valChangedSize].type = objectInfo[ManagementServerID].parameters[ManagementServerNATDetectedID].type;
        valChangedSize++;
    }
    
    if ( pOldNatDetected )
    {
        CcspManagementServer_Free(pOldNatDetected);
    }

    /* UDPConnectionRequestAddress */
    rc = strcmp_s(UdpConnReqURL,MAX_UDP_VAL,pOldUrl,&ind);
    ERR_CHK(rc);
    if ((!pOldUrl && UdpConnReqURL) || ( UdpConnReqURL &&  ((ind) && (rc == EOK))))
    
    {
        if ( objectInfo[ManagementServerID].parameters[ManagementServerUDPConnectionRequestAddressID].notification )
        {
            valChanged[valChangedSize].parameterName = CcspManagementServer_MergeString(objectInfo[ManagementServerID].name, objectInfo[ManagementServerID].parameters[ManagementServerUDPConnectionRequestAddressID].name);
            valChanged[valChangedSize].oldValue = pOldUrl;
            pOldUrl = NULL;
            valChanged[valChangedSize].newValue = CcspManagementServer_CloneString(UdpConnReqURL);
            valChanged[valChangedSize].type = objectInfo[ManagementServerID].parameters[ManagementServerUDPConnectionRequestAddressID].type;
            valChangedSize++;
        }
    }

    if ( pOldUrl )
    {
        objectInfo[ManagementServerID].parameters[ManagementServerUDPConnectionRequestAddressID].value = NULL;
        CcspManagementServer_Free(pOldUrl);
    }

    if ( UdpConnReqURL )
    {
        objectInfo[ManagementServerID].parameters[ManagementServerUDPConnectionRequestAddressID].value =
            CcspManagementServer_CloneString(UdpConnReqURL);
    }

    if ( valChangedSize > 0 )
    {
        int                         res;
        int                         i;

        res = CcspBaseIf_SendparameterValueChangeSignal (
            bus_handle,
            valChanged,
            valChangedSize);
        if(res != CCSP_SUCCESS){
            CcspTraceWarning2("ms", ( "CcspManagementServer_StunBindingChanged - failed to send value change signal, error = %d.\n", res));
        }

        for(i=0; i<valChangedSize; i++)
        {
            if(valChanged[i].parameterName) CcspManagementServer_Free((void*)valChanged[i].parameterName);
            if(valChanged[i].oldValue) CcspManagementServer_Free((void*)valChanged[i].oldValue);
            if(valChanged[i].newValue) CcspManagementServer_Free((void*)valChanged[i].newValue);
        }

    }
}
#endif

