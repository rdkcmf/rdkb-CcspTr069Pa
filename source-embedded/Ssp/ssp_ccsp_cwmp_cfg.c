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

    module:	ssp_ccsp_cwmp_cfg.c

        For CCSP TR-069 PA SSP,
        BroadWay Service Delivery System

    ---------------------------------------------------------------

    description:

        SSP implementation of the CCSP CWMP Cfg interface
        for TR-069 PA SSP.

        *   ssp_BbhmInitCcspCwmpCfgIf
        *   ssp_CcspCwmpCfgGetDevDataModelVersion

        If _ANSC_USE_OPENSSL_
        *   CcspTr069PaSsp_OpensslSetCertificateLocation 

    ---------------------------------------------------------------

    environment:

        Platform dependent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/03/2011  initial revision.

**********************************************************************/


#include "ssp_global.h"
#include "ansc_xml_dom_parser_interface.h"
#include "ansc_xml_dom_parser_external_api.h"
#include "ansc_xml_dom_parser_status.h"
#include "cJSON.h"



extern  CCSP_CWMP_CFG_INTERFACE                          ccspCwmpCfgIf;
extern  UCHAR g_MACAddress[];
extern  ANSC_HANDLE bus_handle;

#define	 SSP_CCSP_CWMP_HTTP_SESSION_IDLE_TIMEOUT	120
#define	 SSP_CCSP_CWMP_RPC_TIMEOUT					120


#ifdef   _ANSC_USE_OPENSSL_
#define CCSP_TR069PA_CFG_Name_Certificates      "certificates"
#define CCSP_TR069PA_CERTIFICATE_CFG_Name_ca    "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.TR69CertLocation"
#define CCSP_TR069PA_CERTIFICATE_CFG_Name_dev   "DEV"
#define CCSP_TR069PA_CERTIFICATE_CFG_Name_pkey  "PrivateKey"

extern char* openssl_client_ca_certificate_files;
extern char* openssl_client_dev_certificate_file;
extern char* openssl_client_private_key_file;
#endif

#define PSM_CMD_SYNDICATION_TR69CertLocation		"psmcli get dmsb.device.deviceinfo.X_RDKCENTRAL-COM_Syndication.TR69CertLocation"

#define CCSP_TR069PA_CFG_Name_Outbound_If       "OutboundInterface"
extern char* g_Tr069PaOutboundIfName;

#define BOOTSTRAP_INFO_FILE                              "/nvram/bootstrap.json"
#define CCSP_TR069PA_CFG_Name_AcsDefAddr		"Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.TR69ACSConnectURL"
extern char* g_Tr069PaAcsDefAddr;

#define DEVICE_PROPERTIES    "/etc/device.properties"

void CcspTr069PaSsp_GetPartnerID(char *partnerID);

#if 0
static ANSC_STATUS  
CcspTr069PaSsp_XML_GetMultipleItemWithSameName
    (
        PANSC_XML_DOM_NODE_OBJECT   pRootNode,
        char*                       ItemName,
        char**                      retVal
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode         = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    char                            buffer[512]        = {0};
    ULONG                           uLength            = 511;
    errno_t rc       = -1;
    int     ind      = -1;  
    
    if (pRootNode && ItemName && retVal)
    {
        if(*retVal) { CcspTr069PaFreeMemory(*retVal); *retVal = NULL; }
        
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
            AnscXmlDomNodeGetChildByName(pRootNode, ItemName);
        
        while( pChildNode != NULL)
        {
            rc = strcmp_s(ItemName,strlen(ItemName),pChildNode->Name,&ind);
            ERR_CHK(rc);
            if((ind == 0) && (rc == EOK))
            {
                uLength = 511;  // uLength: passes in max buffer length; gets out actual length
                
                if (pChildNode->GetDataString(pChildNode, NULL, buffer, &uLength) == ANSC_STATUS_SUCCESS && 
                    uLength > 0) 
                {
                    if(*retVal) 
                    {
                        char* sptr = (char*)CcspTr069PaAllocateMemory(AnscSizeOfString(*retVal)+1+uLength+1);
                        if(sptr == NULL) 
                        {
                            CcspTr069PaTraceWarning(("Failed to reallocate returnCA\n"));
                            CcspTr069PaFreeMemory(*retVal);
                            *retVal = NULL;
                            returnStatus =  ANSC_STATUS_RESOURCES;
                            goto EXIT;
                        }

                        _ansc_sprintf(sptr, "%s,%s", *retVal, buffer);
                        CcspTr069PaFreeMemory(*retVal);
                        *retVal = sptr;
                    }
                    else *retVal = CcspTr069PaCloneString(buffer);
                }
            }
            
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT) AnscXmlDomNodeGetNextChild(pRootNode, pChildNode);
        }
        CcspTr069PaTraceDebug(("%s: %s = %s\n", __FUNCTION__, (ItemName)?(ItemName):"NULL", (*retVal)?(*retVal):"NULL"));
        //        fprintf(stderr, "%s: %s = %s\n", __FUNCTION__, (ItemName)?(ItemName):"NULL", (*retVal)?(*retVal):"NULL");
    }

 EXIT:
    return returnStatus;
}
#endif

static void
CcspTr069PaSsp_XML_GetOneItemByName
    (
        PANSC_XML_DOM_NODE_OBJECT  pRootNode,
        char*                      ItemName,
        char**                     retVal
     )
{
    PANSC_XML_DOM_NODE_OBJECT pChildNode  = NULL;
    ULONG                     uLength     = 511;
    char                      buffer[512] = {0};

    if (pRootNode && retVal && ItemName)
    {
        if(*retVal) { AnscFreeMemory(*retVal); *retVal=NULL; }
        uLength=511;
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT) AnscXmlDomNodeGetChildByName(pRootNode, ItemName);
        if(pChildNode != NULL  && 
           pChildNode->GetDataString(pChildNode, NULL, buffer, &uLength) == ANSC_STATUS_SUCCESS &&
           uLength > 0)
        {
            *retVal = CcspTr069PaCloneString(buffer);
        }
    }    
    CcspTr069PaTraceDebug(("%s: %s = %s\n", __FUNCTION__, (ItemName)?(ItemName):"NULL", (*retVal)?(*retVal):"NULL"));                                                     
    //    fprintf(stderr, "%s: %s = %s\n", __FUNCTION__, (ItemName)?(ItemName):"NULL", (*retVal)?(*retVal):"NULL");
}

ANSC_STATUS CcspTr069PaSsp_JSON_GetItemByName    (
        char*                      partnerID,
        char*                      itemName,
        char**                     retVal
     )
{
        char *data = NULL;
        cJSON *json = NULL;
        cJSON *partnerObj = NULL;
        FILE *fileRead = NULL;
        unsigned int len;
	char* buffer = NULL;

	if(*retVal) { AnscFreeMemory(*retVal); *retVal=NULL; }
	
	fileRead = fopen( BOOTSTRAP_INFO_FILE, "r" );
	if( fileRead == NULL )
	{
	 	CcspTr069PaTraceWarning(("%s-%d : Error in opening JSON file\n" , __FUNCTION__, __LINE__ ));
		return ANSC_STATUS_FAILURE;
	}


	fseek( fileRead, 0, SEEK_END );
	len = ftell( fileRead );
	fseek( fileRead, 0, SEEK_SET );
	data = ( char* )malloc( sizeof(char) * (len + 1) );
	if ( data )
	{
		data[len] = '\0';  // add terminating NUL character
		if ( fread(data, 1, len, fileRead) != len )
		{
			fclose(fileRead);
			return ANSC_STATUS_FAILURE;
		}
	}
	else
	{
	 	CcspTr069PaTraceWarning(("%s-%d : Memory allocation failed \n", __FUNCTION__, __LINE__));
	 	fclose( fileRead );
	 	return ANSC_STATUS_FAILURE;
	}

        fclose( fileRead );

	if ( data == NULL )
	{
		CcspTr069PaTraceWarning(("%s-%d : fileRead failed \n", __FUNCTION__, __LINE__));
		return ANSC_STATUS_FAILURE;
	}
	else if ( strlen(data) != 0)
	{
		json = cJSON_Parse( data );
		if( !json )
		{
			 CcspTr069PaTraceWarning((  "%s : json file parser error : [%d]\n", __FUNCTION__,__LINE__));
			 free(data);
			 return ANSC_STATUS_FAILURE;
		}
		else
		{
                	partnerObj = cJSON_GetObjectItem( json, partnerID );
		        if( partnerObj != NULL)
		        {
                                cJSON *partnerObjVal = cJSON_GetObjectItem(cJSON_GetObjectItem( partnerObj, itemName), "ActiveValue");
				if ( partnerObjVal != NULL )
                                {
					buffer = partnerObjVal->valuestring;
					*retVal = CcspTr069PaCloneString(buffer);
				}
				else {
                                        CcspTr069PaTraceWarning(("%s - Item Object is NULL\n", __FUNCTION__ ));
				}
			}
			else
			{
				CcspTr069PaTraceWarning(("%s - PARTNER ID OBJECT is NULL\n", __FUNCTION__ ));
			}

			cJSON_Delete(json);
		}

		free(data);
          	data = NULL;
	}
	else
	{
		CcspTr069PaTraceWarning(("BOOTSTRAP_INFO_FILE %s is empty\n", BOOTSTRAP_INFO_FILE));
		return ANSC_STATUS_FAILURE;
	}

    	CcspTr069PaTraceWarning(("%s: %s = %s\n", __FUNCTION__, (itemName)?(itemName):"NULL", (*retVal)?(*retVal):"NULL"));

	return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS  
CcspTr069PaSsp_LoadCfgFile
    (
        char*                       pCfgFileName
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    char*                           pXMLContent        = NULL;
    ULONG                           uBufferSize        = 0;
    char partnerID[128];
    /* load configuration file */
    {
        ANSC_HANDLE  pFileHandle = NULL;

        pFileHandle = AnscOpenFile 
                          (
                               pCfgFileName,
                               ANSC_FILE_O_BINARY | ANSC_FILE_O_RDONLY,
                               ANSC_FILE_S_IREAD
                           );
        if( pFileHandle == NULL)
        {
            CcspTr069PaTraceWarning(("Failed to open file %s\n", pCfgFileName));
            return ANSC_STATUS_FAILURE;
        }

        uBufferSize = AnscGetFileSize(pFileHandle) + 8;
        pXMLContent = (char*)AnscAllocateMemory(uBufferSize);
        if (pXMLContent == NULL)
        {
            CcspTr069PaTraceWarning(("Failed to allocate buffer\n"));
            AnscCloseFile(pFileHandle);
            return ANSC_STATUS_RESOURCES;
        }

        if ( AnscReadFile(pFileHandle, pXMLContent, &uBufferSize) != ANSC_STATUS_SUCCESS )
        {
            CcspTr069PaTraceWarning(("Failed to read file %s\n", pCfgFileName));
            AnscFreeMemory(pXMLContent);
            AnscCloseFile(pFileHandle);
            return ANSC_STATUS_FAILURE;
        }

        AnscCloseFile(pFileHandle);
    }

    /*
     *  Parse the XML content
     */
    {
        PANSC_XML_DOM_NODE_OBJECT       pRootNode          = (PANSC_XML_DOM_NODE_OBJECT)NULL;
        PANSC_XML_DOM_NODE_OBJECT       pChildNode         = (PANSC_XML_DOM_NODE_OBJECT)NULL;
        char*                           pXMLIterator       = pXMLContent;
        
        pRootNode   = (PANSC_XML_DOM_NODE_OBJECT)
            AnscXmlDomParseString((ANSC_HANDLE)NULL, (PCHAR*)&pXMLIterator, uBufferSize);
        if ( pRootNode == NULL )
        {
            CcspTr069PaTraceWarning(("Failed to parse the file to retrieve the root node!\n"));
            returnStatus = ANSC_STATUS_FAILURE;
            goto EXIT;
        }

	CcspTr069PaSsp_GetPartnerID(partnerID);

#ifdef   _ANSC_USE_OPENSSL_
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
            AnscXmlDomNodeGetChildByName(pRootNode, CCSP_TR069PA_CFG_Name_Certificates);
        if ( pChildNode != NULL) {
            CcspTr069PaSsp_XML_GetOneItemByName(pChildNode, CCSP_TR069PA_CERTIFICATE_CFG_Name_dev, &openssl_client_dev_certificate_file);
            CcspTr069PaSsp_XML_GetOneItemByName(pChildNode, CCSP_TR069PA_CERTIFICATE_CFG_Name_pkey, &openssl_client_private_key_file);
        }

        if ( ANSC_STATUS_SUCCESS != CcspTr069PaSsp_GetTr069CertificateLocationForSyndication( &openssl_client_ca_certificate_files ) )
        {
            char cmd[512] = {0};

            // Fallback case to load default cerification file
            CcspTr069PaSsp_JSON_GetItemByName(partnerID, CCSP_TR069PA_CERTIFICATE_CFG_Name_ca, &openssl_client_ca_certificate_files);

            // Load  Certfication file only if it is not NULL
            if (openssl_client_ca_certificate_files)
            {
                snprintf(cmd, sizeof(cmd), "psmcli set dmsb.device.deviceinfo.X_RDKCENTRAL-COM_Syndication.TR69CertLocation  %s", openssl_client_ca_certificate_files);
                CcspTr069PaTraceWarning(("%s\n",cmd));
                system(cmd);
            }
        }

#endif
        
        CcspTr069PaSsp_XML_GetOneItemByName(pRootNode, CCSP_TR069PA_CFG_Name_Outbound_If, &g_Tr069PaOutboundIfName);

	if(ANSC_STATUS_SUCCESS != CcspTr069PaSsp_JSON_GetItemByName(partnerID, CCSP_TR069PA_CFG_Name_AcsDefAddr, &g_Tr069PaAcsDefAddr)) {
		returnStatus = ANSC_STATUS_FAILURE;
	}
	else {
        	returnStatus = ANSC_STATUS_SUCCESS;
	}
    }
    
EXIT:
    AnscFreeMemory(pXMLContent);
    return  returnStatus;
}

//Custom
extern int CcspTr069PaSsp_InitCcspCwmpCfgIf_Custom(ANSC_HANDLE hCcspCwmpCpeController);

int
CcspTr069PaSsp_InitCcspCwmpCfgIf(ANSC_HANDLE hCcspCwmpCpeController)
{
    AnscZeroMemory(&ccspCwmpCfgIf, sizeof(CCSP_CWMP_CFG_INTERFACE));
    ccspCwmpCfgIf.hOwnerContext  = hCcspCwmpCpeController;
    ccspCwmpCfgIf.Size           = sizeof(CCSP_CWMP_CFG_INTERFACE);

    ccspCwmpCfgIf.NoRPCMethods              	= CcspTr069PaSsp_CcspCwmpCfgNoRPCMethods;
    ccspCwmpCfgIf.GetDevDataModelVer        	= CcspTr069PaSsp_CcspCwmpCfgGetDevDataModelVersion;

	ccspCwmpCfgIf.GetHttpSessionIdleTimeout 	= CcspTr069PaSsp_GetHttpSessionIdleTimeout;
	ccspCwmpCfgIf.GetCwmpRpcTimeout				= CcspTr069PaSsp_GetCwmpRpcTimeout;

	ccspCwmpCfgIf.GetCustomForcedInformParams	= CcspTr069PaSsp_GetCustomForcedInformParams;

	//Custom call
	CcspTr069PaSsp_InitCcspCwmpCfgIf_Custom(hCcspCwmpCpeController);

    return 0;
}

ULONG
CcspTr069PaSsp_CcspCwmpCfgNoRPCMethods
    (
        ANSC_HANDLE                 hThisObject
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    ULONG                           ulNoMethods = 0;

    ulNoMethods = 
        CCSP_CWMP_CFG_RPC_METHOD_NO_GetQueuedTransfers   |
        CCSP_CWMP_CFG_RPC_METHOD_NO_SetVouchers          |
        CCSP_CWMP_CFG_RPC_METHOD_NO_GetOptions           |
        CCSP_CWMP_CFG_RPC_METHOD_NO_ScheduleInform       |
        CCSP_CWMP_CFG_RPC_METHOD_NO_Upload;

    return ulNoMethods;
}


extern void 
CcspTr069PaSsp_CcspCwmpCfgGetDevDataModelVersionCustom
    (
        ANSC_HANDLE                 hThisObject,
        PULONG                      pulDevVerionMajor,
        PULONG                      pulDevVersionMinor
    );
    
ANSC_STATUS
CcspTr069PaSsp_CcspCwmpCfgGetDevDataModelVersion
    (
        ANSC_HANDLE                 hThisObject,
        PULONG                      pulDevVerionMajor,
        PULONG                      pulDevVersionMinor
    )
{
    *pulDevVerionMajor  = 2;
    *pulDevVersionMinor = 0;

    CcspTr069PaSsp_CcspCwmpCfgGetDevDataModelVersionCustom(hThisObject,pulDevVerionMajor,pulDevVersionMinor);
    
    return  ANSC_STATUS_SUCCESS;
}


ULONG
CcspTr069PaSsp_GetHttpSessionIdleTimeout
    (
        ANSC_HANDLE                 hThisObject
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    return	SSP_CCSP_CWMP_HTTP_SESSION_IDLE_TIMEOUT;
}


ULONG
CcspTr069PaSsp_GetCwmpRpcTimeout
    (
        ANSC_HANDLE                 hThisObject
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    return SSP_CCSP_CWMP_RPC_TIMEOUT;
}

//custom
extern char*
CcspTr069PaSsp_GetCustomForcedInformParamsCustom
    (
        ANSC_HANDLE                 hThisObject
    );
    
char*
CcspTr069PaSsp_GetCustomForcedInformParams
    (
        ANSC_HANDLE                 hThisObject
    )
{

    /* API GetCustomForcedInformParams is called while configured to load customized       
     * Forced Inform Parameters other than those defined in Data Model Definition such    
     * as TR-098 or TR-181 etc. Return value would be comma-separated full parameter      
     * names, this API allocates the memory returned and caller will release it */          

	char*							pFIPs = NULL;

	if(!pFIPs)
	{
	  CcspTr069PaSsp_GetCustomForcedInformParamsCustom(hThisObject);
	}
	
	return	pFIPs;
}


ANSC_STATUS
CcspTr069PaSsp_GetTr069CertificateLocationForSyndication
    (
		char**						ppretTr069CertLocation
	)
{
   FILE 	  *FilePtr		       = NULL;
   char 	   fileContent[ 256 ]  = { 0 };

	FilePtr = popen( PSM_CMD_SYNDICATION_TR69CertLocation, "r" );

	if ( FilePtr ) 
	{
		if( fgets( fileContent, 256, FilePtr ) != NULL )
		{
			pclose( FilePtr );
			FilePtr = NULL;

			// Tr069 Location should have valid length
			if(fileContent[0])
			{
				char *pos;

				// Remove line \n charecter from string
				if ( ( pos = strchr( fileContent, '\n' ) ) != NULL )
				 *pos = '\0';

				// Free if it is already allocated
				if( NULL != *ppretTr069CertLocation )
				{
					free( *ppretTr069CertLocation );
					*ppretTr069CertLocation = NULL;
				}

				*ppretTr069CertLocation = ( char * ) malloc ( strlen( fileContent )  + 1 );

				if( NULL != *ppretTr069CertLocation )
				{
					AnscCopyString( *ppretTr069CertLocation, fileContent );
					CcspTr069PaTraceWarning(("%s %d - Syndication TR69CertLocation %s\n", __FUNCTION__, __LINE__, *ppretTr069CertLocation ));
				}
				else
				{
					CcspTr069PaTraceWarning(("%s %d - Syndication TR69CertLocation Memory Allocation Fail\n", __FUNCTION__, __LINE__ ));
					return	ANSC_STATUS_FAILURE;
				}
			}
			else
			{
				CcspTr069PaTraceWarning(("%s %d - Syndication TR69CertLocation is Empty\n", __FUNCTION__, __LINE__ ));
				return	ANSC_STATUS_FAILURE;
			}
		}
	} 
	else
	{
		CcspTr069PaTraceWarning(("%s %d - Failed to Get Syndication TR69CertLocation Param\n", __FUNCTION__, __LINE__ ));
		return	ANSC_STATUS_FAILURE;
	}

	if( fileContent[ 0 ] == '\0') {
                CcspTr069PaTraceWarning(("%s %d - Syndication TR69CertLocation is Empty\n", __FUNCTION__, __LINE__ ));
                return  ANSC_STATUS_FAILURE;
	}

	CcspTr069PaTraceWarning(("%s %d - Syndication TR69CertLocation : %s\n", 
													 __FUNCTION__, 
													 __LINE__,
													 ( fileContent[ 0 ] != '\0') ? fileContent : "NULL"  ));
   return  ANSC_STATUS_SUCCESS;
}

void
CcspTr069PaSsp_GetPartnerID(char *partnerID)
{
	getPartnerId ( partnerID ) ;
}
