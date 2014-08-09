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

    copyright:

        Cisco Systems, Inc., 2011
        All Rights Reserved.

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

extern  CCSP_CWMP_CFG_INTERFACE                          ccspCwmpCfgIf;
extern  UCHAR g_MACAddress[];
extern  ANSC_HANDLE bus_handle;

#define	 SSP_CCSP_CWMP_HTTP_SESSION_IDLE_TIMEOUT	120
#define	 SSP_CCSP_CWMP_RPC_TIMEOUT					120


#ifdef   _ANSC_USE_OPENSSL_
#define CCSP_TR069PA_CFG_Name_Certificates      "certificates"
#define CCSP_TR069PA_CERTIFICATE_CFG_Name_ca    "CA"
#define CCSP_TR069PA_CERTIFICATE_CFG_Name_dev   "DEV"
#define CCSP_TR069PA_CERTIFICATE_CFG_Name_pkey  "PrivateKey"

extern char* openssl_client_ca_certificate_files;
extern char* openssl_client_dev_certificate_file;
extern char* openssl_client_private_key_file;
#endif

#define CCSP_TR069PA_CFG_Name_Outbound_If       "outbound_interface"
extern char* ccsp_tr069pa_outbound_if;

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
    
    if (pRootNode && ItemName && retVal)
    {
        if(*retVal) { CcspTr069PaFreeMemory(*retVal); *retVal = NULL; }
        
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
            AnscXmlDomNodeGetChildByName(pRootNode, ItemName);
        
        while( pChildNode != NULL)
        {
            if (AnscEqualString(pChildNode->Name, ItemName, TRUE))
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

static void
CcspTr069PaSsp_XML_GetOneItemByName
    (
        PANSC_XML_DOM_NODE_OBJECT  pRootNode,
        char*                      ItemName,
        char**                     retVal
     )
{
    PANSC_XML_DOM_NODE_OBJECT pChildNode  = NULL;
    int                       uLength     = 511;
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

ANSC_STATUS  
CcspTr069PaSsp_LoadCfgFile
    (
        char*                       pCfgFileName
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    char*                           pXMLContent        = NULL;
    ULONG                           uBufferSize        = 0;

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

#ifdef   _ANSC_USE_OPENSSL_
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
            AnscXmlDomNodeGetChildByName(pRootNode, CCSP_TR069PA_CFG_Name_Certificates);
        if ( pChildNode != NULL) {
            CcspTr069PaSsp_XML_GetMultipleItemWithSameName(pChildNode, CCSP_TR069PA_CERTIFICATE_CFG_Name_ca, &openssl_client_ca_certificate_files);
            CcspTr069PaSsp_XML_GetOneItemByName(pChildNode, CCSP_TR069PA_CERTIFICATE_CFG_Name_dev, &openssl_client_dev_certificate_file);
            CcspTr069PaSsp_XML_GetOneItemByName(pChildNode, CCSP_TR069PA_CERTIFICATE_CFG_Name_pkey, &openssl_client_private_key_file);
                    }
#endif

        CcspTr069PaSsp_XML_GetOneItemByName(pRootNode, CCSP_TR069PA_CFG_Name_Outbound_If, &ccsp_tr069pa_outbound_if);

        returnStatus = ANSC_STATUS_SUCCESS;
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
    ULONG                           ulNoMethods = 0;

    ulNoMethods = 
        CCSP_CWMP_CFG_RPC_METHOD_NO_GetQueuedTransfers   |
        CCSP_CWMP_CFG_RPC_METHOD_NO_SetVouchers          |
        CCSP_CWMP_CFG_RPC_METHOD_NO_GetOptions           |
        CCSP_CWMP_CFG_RPC_METHOD_NO_ScheduleInform       |
        CCSP_CWMP_CFG_RPC_METHOD_NO_Upload;

//Not used	
#if 0
//#ifdef _COSA_E4200_
    ulNoMethods |= CCSP_CWMP_CFG_RPC_METHOD_NO_ChangeDUState;
#endif

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
	return	SSP_CCSP_CWMP_HTTP_SESSION_IDLE_TIMEOUT;
}


ULONG
CcspTr069PaSsp_GetCwmpRpcTimeout
    (
        ANSC_HANDLE                 hThisObject
    )
{
	return	SSP_CCSP_CWMP_RPC_TIMEOUT;
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
#if 0
//Not Used	
//#ifdef _COSA_E4200_
	if ( !pFIPs ) pFIPs = (char*)CcspTr069PaAllocateMemory(512);

	if ( pFIPs )
	{
		_ansc_sprintf
			(
				pFIPs, 
				"%s", 
				"InternetGatewayDevice.WANDevice.1.WANEthernetInterfaceConfig.MACAddress"
			);
	}
#endif

	if(!pFIPs)
	{
	  CcspTr069PaSsp_GetCustomForcedInformParamsCustom(hThisObject);
	}
	
	return	pFIPs;
}
