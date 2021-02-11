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

    module: ccsp_tr069pa_mapper_api.c

        For CCSP TR-069 PA mapper file

    ---------------------------------------------------------------

    description:

        This file implements all data structures and macros for
        handling CCSP TR-069 PA mapper file.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Hui Ma 
        Kang Quan

    ---------------------------------------------------------------

    revision:

        06/08/2011      initial revision by Hui.
        07/07/2011      Kang added parameter list parsing and namespace
                        query that resides on different sub-system as
                        well as APIs to build invisible namespace tree
                        and check namespace against invisible NS tree.

**********************************************************************/

#include "ccsp_cwmp_cpeco_global.h"
/*
#include <stdio.h>
#include <sys/stat.h>
#include "linux/user_base.h"
#include "ansc_wrapper_base.h"
#include "ansc_common_structures.h"
#include "ansc_status.h"
#include "ansc_string.h"
#include "ccsp_trace.h"
#include "ccsp_tr069pa_wrapper_api.h"
#include "ccsp_tr069pa_mapper_api.h"
#include "ansc_xml_dom_parser_interface.h"
#include "ansc_xml_dom_parser_external_api.h"
#include "ansc_xml_dom_parser_status.h"
#include "ansc_xml_dom_parser_def.h"
#include "ccsp_cwmp_definitions_cwmp.h"
*/

static PCCSP_TR069_CPEERR_MAP                       CcspTr069CpeErrMaps         = NULL;
static CCSP_INT                                     NumOfErrMaps                = 0;
static PCCSP_TR069_RPC_MAP                          CcspTr069RpcMaps            = NULL;
static CCSP_INT                                     NumOfRpcMaps                = 0;
static PCCSP_TR069_CPEINSTANCE_MAP                  CcspTr069CpeInstanceMaps    = NULL;
static CCSP_INT                                     NumOfInstanceMaps           = 0;

/* tree for all visible namespace on remote sub-systems */
static PCCSP_TR069_PARAM_INFO                       CcspTr069PiTree             = NULL;     

/* tree for all invisible namespaces (local and remote sub-systems) */
static PCCSP_TR069_PARAM_INFO                       CcspTr069InvPiTree          = NULL;     

/* array of sub-systems related to PA */
static CCSP_STRING                                  CcspTr069Subsystems[CCSP_SUBSYSTEM_MAX_COUNT] = {0};
static CCSP_INT                                     CcspTr069SubsystemsCount    = 0;


const enum dataType_e
CcspTr069PA_Cwmp2CcspType
    (
        int                         cwmpType
    )
{
    enum dataType_e                 ccspType = ccsp_string;

    switch ( cwmpType )
    {
        case    CCSP_CWMP_TR069_DATA_TYPE_Int:

                ccspType = ccsp_int;

                break;

        case    CCSP_CWMP_TR069_DATA_TYPE_UnsignedInt:

                ccspType = ccsp_unsignedInt;

                break;

        case    CCSP_CWMP_TR069_DATA_TYPE_Boolean:

                ccspType = ccsp_boolean;

                break;

        case    CCSP_CWMP_TR069_DATA_TYPE_DateTime:

                ccspType = ccsp_dateTime;

                break;

        case    CCSP_CWMP_TR069_DATA_TYPE_Base64:

                ccspType = ccsp_base64;

                break;
    }

    return  ccspType;
}


const int
CcspTr069PA_Ccsp2CwmpType
    (
        enum dataType_e             ccspType
    )
{
    int                             cwmpType = CCSP_CWMP_TR069_DATA_TYPE_Unspecified;

    switch ( ccspType )
    {
        case    ccsp_string:

                cwmpType = CCSP_CWMP_TR069_DATA_TYPE_String;

                break;

        case    ccsp_int:

                cwmpType = CCSP_CWMP_TR069_DATA_TYPE_Int;

                break;

        case    ccsp_unsignedInt:

                cwmpType = CCSP_CWMP_TR069_DATA_TYPE_UnsignedInt;

                break;

        case    ccsp_boolean:

                cwmpType = CCSP_CWMP_TR069_DATA_TYPE_Boolean;

                break;

        case    ccsp_dateTime:

                cwmpType = CCSP_CWMP_TR069_DATA_TYPE_DateTime;

                break;

        case    ccsp_base64:

                cwmpType = CCSP_CWMP_TR069_DATA_TYPE_Base64;

                break;

		default:

				cwmpType = CCSP_CWMP_TR069_DATA_TYPE_Unspecified;

				break;
    }

    return  cwmpType;
}


/* CcspTr069PA_GetSubsystemCount is called to return the number
 * of sub-systems of namespaces TR-069 PA manages.
 */
const CCSP_INT
CcspTr069PA_GetSubsystemCount
    (
        CCSP_HANDLE                 MapperHandle
    )
{
    UNREFERENCED_PARAMETER(MapperHandle);

    return  CcspTr069SubsystemsCount;
}


/* CcspTr069PA_GetSubsystemCount is called to return 
 * sub-system related to PA by index.
 */
const CCSP_STRING
CcspTr069PA_GetSubsystemByIndex
    (
        CCSP_HANDLE                 MapperHandle,
        CCSP_INT                    index
    )
{
    UNREFERENCED_PARAMETER(MapperHandle);

    if ( index >= CcspTr069SubsystemsCount )
    {
        return  NULL;
    }
    else
    {
        return  CcspTr069Subsystems[index];
    }
}


/* Check for the existence of a config file path */
static CCSP_BOOL CcspTr069PA_CheckFileExists( const char *path )
{
    FILE *file;

    /* Try and open the config file */  
    if( ( file = fopen( path, "rb" ) ) == NULL )
    {
        return CCSP_FALSE;
    }

    fclose( file );

    return CCSP_TRUE;
}


static
CCSP_VOID
CcspTr069PA_FreePiTree
    (
        PCCSP_TR069_PARAM_INFO      pRoot
    )
{
    PCCSP_TR069_PARAM_INFO          pChild, pSibling;

    if ( !pRoot ) return;

    if ( pRoot->Name      ) CcspTr069PaFreeMemory(pRoot->Name);
    if ( pRoot->Subsystem ) CcspTr069PaFreeMemory(pRoot->Subsystem);

    pChild      = pRoot->Child;
    pSibling    = pRoot->Sibling;

    CcspTr069PA_FreePiTree(pSibling);
    CcspTr069PA_FreePiTree(pChild);

    CcspTr069PaFreeMemory(pRoot);
}


/* CcspTr069PA_LoadMappingFile is called to load mapping file for TR-069 PA,
 * Return value - transparent handle to caller and will be used for all
 * other APIs.
 */
CCSP_HANDLE
CcspTr069PA_LoadMappingFile
    (
        CCSP_STRING                 MappingFile
    )
{
    struct stat   statBuf;

    /* load from XML file */
    PANSC_XML_DOM_NODE_OBJECT       pRootNode   = NULL;

    CcspTr069PaTraceDebug(("TR-069 PA is loading mapper file <%s> ...\n", MappingFile));


    if( CcspTr069PA_CheckFileExists( MappingFile ) )
    {
        CCSP_INT fileHandle   = open(MappingFile,  O_RDONLY);
        fstat(fileHandle,&statBuf);
        CCSP_INT iContentSize = statBuf.st_size;

        if ( iContentSize > 500000)
        {
            CcspTr069PaTraceError(("Internal error: TR-069 PA mapper file is too big!\n"));
        }
        else
        {
            char * pFileContent = CcspTr069PaAllocateMemory(iContentSize + 1);
            
            if ( pFileContent )
            {
                AnscZeroMemory(pFileContent, iContentSize + 1);

                if ( read((int)fileHandle, pFileContent, iContentSize) > 0)
                {
                    /* Some Unicode file may have hidden content at the beginning. So search for the first '<' to begin the XML parse. */
                    PCHAR pBack = pFileContent;
                    while(pBack && *pBack != '<') pBack++;
                    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
                    AnscXmlDomParseString((ANSC_HANDLE)NULL, (PCHAR*)&pBack, iContentSize);
                }

            /* loca from the node */
                if (pRootNode != NULL)
                {
                    pRootNode->Remove(pRootNode);
                }

                CcspTr069PaFreeMemory(pFileContent);
            }
        }

        close(fileHandle);
        
        CcspTr069PaTraceError(("TR-069 PA mapper file <%s> has been loaded!\n", MappingFile));
    }
    else
    {
        CcspTr069PaTraceError(("Internal error: TR-069 PA mapper file <%s> is NOT found!\n", MappingFile));
    }

    return NULL;
}


/* CcspTr069PA_UnloadMappingFile is called to unload mapper file and
 * remove all allocated resources accordingly.
 * Return value - 0 success, failure otherwise.
 */
CCSP_INT
CcspTr069PA_UnloadMappingFile
    (
        CCSP_HANDLE                 MapperHandle
        )
{
    UNREFERENCED_PARAMETER(MapperHandle);
    CCSP_INT    i,j;

    CcspTr069PaFreeMemory(CcspTr069CpeErrMaps);
    CcspTr069CpeErrMaps = NULL;

    for (i=0; i< NumOfInstanceMaps; i++) {
        if (CcspTr069CpeInstanceMaps[i].CcspDmlName) {
            CcspTr069PaFreeMemory(CcspTr069CpeInstanceMaps[i].CcspDmlName);
        }
        CcspTr069PaFreeMemory(CcspTr069CpeInstanceMaps[i].InstanceMap);
    }
    CcspTr069PaFreeMemory(CcspTr069CpeInstanceMaps);
    
    for(i=0; i<NumOfRpcMaps; i++)
    {
        if ( CcspTr069RpcMaps[i].Namespace )
        {
            CcspTr069PaFreeMemory(CcspTr069RpcMaps[i].Namespace);
        }
        for (j=0; j<CcspTr069RpcMaps[i].NumOfArgs; j++)
        {
            if ( CcspTr069RpcMaps[i].ArgMaps[j].Name )
            {
                CcspTr069PaFreeMemory(CcspTr069RpcMaps[i].ArgMaps[j].Name);
            }
            if ( CcspTr069RpcMaps[i].ArgMaps[j].MappedNS )
            {
                CcspTr069PaFreeMemory(CcspTr069RpcMaps[i].ArgMaps[j].MappedNS);
            }
        }
        CcspTr069PaFreeMemory(CcspTr069RpcMaps[i].ArgMaps);
    }
    CcspTr069PaFreeMemory(CcspTr069RpcMaps);
    CcspTr069RpcMaps = NULL;
    NumOfRpcMaps     = 0;

    CcspTr069PA_FreePiTree(CcspTr069PiTree);
    CcspTr069PiTree = NULL;

    CcspTr069PA_FreePiTree(CcspTr069InvPiTree);
    CcspTr069InvPiTree = NULL;

    for ( i = 0; i < CcspTr069SubsystemsCount; i ++ )
    {
        if ( CcspTr069Subsystems[i] )
        {
            CcspTr069PaFreeMemory(CcspTr069Subsystems[i]);
            CcspTr069Subsystems[i] = NULL;
        }
    }
    AnscZeroMemory(CcspTr069Subsystems, sizeof(CCSP_STRING) * CCSP_SUBSYSTEM_MAX_COUNT);
    CcspTr069SubsystemsCount = 0;

    return 0;
}


/* CcspTr069PA_MapCcspErrCode is called to map CCSP error code
 * to CWMP CPE error code.
 * Return value - mapped error code. Minus code means no error
 * code mapping found.
 */
CCSP_INT
CcspTr069PA_MapCcspErrCode
    (
        CCSP_HANDLE                 MapperHandle,
        CCSP_INT                    CcspErrCode
    )
{
    UNREFERENCED_PARAMETER(MapperHandle);
    CCSP_INT    i;
    for(i=0; i<NumOfErrMaps; i++)
    {
        if(CcspTr069CpeErrMaps[i].CcspErrorCode == CcspErrCode)
            return CcspTr069CpeErrMaps[i].Tr069CpeErrorCode;
    }

    /* handling CCSP error codes that are defined the same as those CWMP has defined */
    if ( CcspErrCode >= CCSP_ERR_CWMP_BEGINNING && CcspErrCode <= CCSP_ERR_CWMP_ENDING )
    {
        return CcspErrCode;
    }

    return CCSP_CWMP_CPE_CWMP_FaultCode_internalError;
}

/*
 *  CWMP_2_DM_INT_INSTANCE_NUMBER_MAPPING
    Description:
        This function maps the instance number within a
        specific Data Model name or value string in CWMP
        requests to Data Model internal instance number.

        E.g.
          Device.WiFi.Radio.10000. -> Device.WiFi.Radio.1.
          Device.WiFi.Radio.10100. -> Device.WiFi.Radio.2.
          Device.WiFi.SSID.10001.  -> Device.WiFi.SSID.1.
          Device.WiFi.SSID.10101.  -> Device.WiFi.SSID.2.

    Arguments:
        CCSP_STRING                 pCwmpString
        This is the opque handle received from init SBAPI.

    Returns:
        The string with CWMP instance replaced by Data Model
        internal instance number is returned, if succeeded.
        NULL is returned if failed.
 *
 */
CCSP_STRING
CcspTr069PA_MapInstNumCwmpToDmInt
    (
        CCSP_STRING                 pCwmpString
    )
{
    CCSP_INT                        i;
    CCSP_INT                        j;
    CCSP_BOOL                       foundMatch      = FALSE;
    CCSP_INT                        tr069StringSize = strlen(pCwmpString);
    CCSP_STRING                     pDmIntString    = NULL;

    CcspTr069PaTraceDebug(("%s...\n", __FUNCTION__));

    for(i=0; i< NumOfInstanceMaps; i++)
    { 
        /*
         *  DH  TBF  Calculating the string length for each mapping name
         *  string every time has performance penalties
         */
        CCSP_INT dmlNameLen = strlen(CcspTr069CpeInstanceMaps[i].CcspDmlName);

        if ( strncmp(pCwmpString, CcspTr069CpeInstanceMaps[i].CcspDmlName, dmlNameLen) == 0 )
        {
            CCSP_STRING     instNumStart = pCwmpString + dmlNameLen;
            CCSP_CHAR       restDmlString[CCSP_TR069_INSTMAP_MaxStringSize] = {0}; //initialize - to resolve invlid chars issue in RPC response
            CCSP_INT        instNum = 0; //initialize - to resolve invlid chars issue in RPC response

            if ( strlen(pCwmpString) < (unsigned int)(dmlNameLen+1) )
            {
                // Found match on table, but there is no instance number
                break;
            }

            if (instNumStart[0] == '.') {
                instNumStart++;
            }
            sscanf(instNumStart,"%d%s",&instNum,restDmlString);

            // Find instance match and translate
            for(j=0; j < CcspTr069CpeInstanceMaps[i].numMaps; j++)
            {
                if (CcspTr069CpeInstanceMaps[i].InstanceMap[j].Tr069CpeInstanceNumber == instNum)
                {    
                    foundMatch = TRUE;
            
                    pDmIntString = (CCSP_STRING) CcspTr069PaAllocateMemory(tr069StringSize + 10);

                    if ( pDmIntString )
                    {
                        sprintf
                            (
                                pDmIntString,
                                "%s.%d%s",
                                CcspTr069CpeInstanceMaps[i].CcspDmlName,
                                CcspTr069CpeInstanceMaps[i].InstanceMap[j].CcspInstanceNumber,
                                restDmlString
                            );
                    }
                    else
                    {
                        CcspTr069PaTraceWarning(("%s - insufficient resources!", __FUNCTION__));
                    }

                    break;
                } 
            }

            if (foundMatch == TRUE)
            {
                break;
            }
        }
    }

    if ( foundMatch == TRUE )
    {
        CcspTr069PaTraceWarning(("%s - %s -> %s \n", __FUNCTION__, pCwmpString, pDmIntString ? pDmIntString : "NULL"));
    }
    else
    {
        CcspTr069PaTraceWarning(("%s - no mapping for %s \n", __FUNCTION__, pCwmpString));
    }

    return  pDmIntString;
}


/*
 *  CWMP_2_DM_INT_INSTANCE_NUMBER_MAPPING
    Description:
        This function maps a Data Model internal instance
        number within a specific Data Model name or value
        string to the instance number expected in CWMP requests.

        E.g.
          Device.WiFi.Radio.1.  -> Device.WiFi.Radio.10000.
          Device.WiFi.Radio.2.  -> Device.WiFi.Radio.10100.
          Device.WiFi.SSID.1.   -> Device.WiFi.SSID.10001.
          Device.WiFi.SSID.2.   -> Device.WiFi.SSID.10101.

    Arguments:
        CCSP_STRING                 pCwmpString
        This is the opque handle received from init SBAPI.

    Returns:
        The string with Data Model internal instance number
        replaced by CWMP instance number is returned, if succeeded.
        NULL is returned if failed.
 *
 */
CCSP_STRING
CcspTr069PA_MapInstNumDmIntToCwmp
    (
        CCSP_STRING                 pDmIntString
    )
{
    CCSP_INT                        i;
    CCSP_INT                        j;
    CCSP_BOOL                       foundMatch      = FALSE;
    CCSP_INT                        ccspStringSize  = strlen(pDmIntString);
    CCSP_STRING                     pCwmpString     = NULL;

    CcspTr069PaTraceDebug(("%s...\n", __FUNCTION__));

    for(i=0; i< NumOfInstanceMaps; i++)
    { 
        /*
         *  DH  TBF  Calculating the string length for each mapping name
         *  string every time has performance penalties
         */
        CCSP_INT dmlNameLen = strlen(CcspTr069CpeInstanceMaps[i].CcspDmlName);

        if ( strncmp(pDmIntString, CcspTr069CpeInstanceMaps[i].CcspDmlName, dmlNameLen) == 0)
        {
            CCSP_STRING             instNumStart = pDmIntString + dmlNameLen;
            CCSP_CHAR               restDmlString[CCSP_TR069_INSTMAP_MaxStringSize] = {0}; //initialize - to resolve invlid chars issue in RPC response
            CCSP_INT                instNum = 0; //initialize - to resolve invlid chars issue in RPC response

            if (strlen(pDmIntString) < (unsigned int)(dmlNameLen+1))
            {
                // Found match on table, but there is no instance number
                break;
            }

            if (instNumStart[0] == '.') {
                instNumStart++;
            }
            sscanf(instNumStart,"%d%s",&instNum,restDmlString);

            // Find instance match and translate
            for(j=0; j < CcspTr069CpeInstanceMaps[i].numMaps; j++)
            {
                if (CcspTr069CpeInstanceMaps[i].InstanceMap[j].CcspInstanceNumber == instNum)
                {    
                    foundMatch = TRUE;
            
                    pCwmpString = (CCSP_STRING) CcspTr069PaAllocateMemory(ccspStringSize + 10);

                    if ( pCwmpString )
                    {
                        sprintf
                            (
                                pCwmpString,
                                "%s.%d%s",
                                CcspTr069CpeInstanceMaps[i].CcspDmlName,
                                CcspTr069CpeInstanceMaps[i].InstanceMap[j].Tr069CpeInstanceNumber,
                                restDmlString
                            );
                    }

                    break;
                } 
            }
            if (foundMatch == TRUE) {
                break;
            }
        }
    }

    if ( foundMatch == TRUE )
    {
        CcspTr069PaTraceWarning(("%s - %s -> %s \n", __FUNCTION__, pDmIntString, pCwmpString ? pCwmpString : "NULL"));
    }
    else
    {
        CcspTr069PaTraceWarning(("%s - no mapping for %s \n", __FUNCTION__, pDmIntString));
    }

    return  pCwmpString;
}

/* CcspTr069PA_GetRpcNamespace is called to map RPC method
 * name/argument to CCSP namespace.
 * Return value - namespace, NULL indicates no mapping found.
 */
const CCSP_STRING
CcspTr069PA_GetRpcNamespace
    (
        CCSP_HANDLE                 MapperHandle,
        CCSP_INT                    RpcType,            /* RPC type, one of CCSP_TR069_RPC_TYPE_Xxx */
        CCSP_STRING                 ArgName             /* NULL for RPCs that do not take any arguments such as Reboot */
    )
{
    UNREFERENCED_PARAMETER(MapperHandle);
    CCSP_INT    i, j;
    for(i=0; i<NumOfRpcMaps; i++)
    {
        if(CcspTr069RpcMaps[i].RpcType == RpcType)
        {
            for(j=0; j<CcspTr069RpcMaps[i].NumOfArgs; j++)
            {
                if(AnscEqualString(CcspTr069RpcMaps[i].ArgMaps[j].Name, ArgName, TRUE))
                    return CcspTr069RpcMaps[i].ArgMaps[j].MappedNS;
            }
            return NULL;
        }
    }
    return NULL;
}


static
CCSP_BOOL
CcspTr069PA_IsNameInsNumber
    (
        CCSP_STRING                 Namespace
    )
{
    CCSP_INT                        len;
    CCSP_UINT                       InsNumber;
    CCSP_CHAR                       buf[32];

    len = AnscSizeOfString(Namespace);
    if ( len > 10 || *Namespace == '0' )
    {
        return CCSP_FALSE;
    }

    InsNumber = (CCSP_UINT)_ansc_atol(Namespace);

    if ( InsNumber == 0 )
    {
        return CCSP_FALSE;
    }

    _ansc_sprintf(buf, "%u", InsNumber);

    return AnscEqualString(buf, Namespace, TRUE);
}


/* CcspTr069PA_FindNamespace is called to find parameter
 * info on specified tree
 *
 * return value:
 *   the matching node - NULL or specific node
 *
 */
static 
const PCCSP_TR069_PARAM_INFO
CcspTr069PA_FindNamespace
    (
        PCCSP_TR069_PARAM_INFO      pRoot,
        CCSP_STRING                 Namespace           /* namespace to be queried */
    )
{
    PCCSP_TR069_PARAM_INFO          pNode               = (PCCSP_TR069_PARAM_INFO)pRoot;
    PCCSP_TR069_PARAM_INFO          pChildNode          = (PCCSP_TR069_PARAM_INFO)NULL;
    PANSC_TOKEN_CHAIN               pNsTokenChain       = (PANSC_TOKEN_CHAIN     )NULL;
    PANSC_STRING_TOKEN              pStringToken        = (PANSC_STRING_TOKEN    )NULL;
    CCSP_BOOL                       bInsNumber          = CCSP_FALSE;
    CCSP_BOOL                       bInsMatched         = CCSP_FALSE;
    PCCSP_TR069_PARAM_INFO          pInsNode            = (PCCSP_TR069_PARAM_INFO)NULL;

    pNsTokenChain =
        AnscTcAllocate
            (
                Namespace,
                "."
            );

    /*RDKB-7323, CID-33225, CID-33324, null check before use */
    if(!pNsTokenChain)
    {
        CcspTr069PaTraceWarning(("TR-069 PA mapper file Token creation failed ret null: %s\n", Namespace));
        return  pChildNode;
    }

    /* find the last matching node on the tree */
    while ( pNode && (pStringToken = AnscTcUnlinkToken(pNsTokenChain)) )
    {
        pChildNode = pNode->Child;

        bInsNumber  = CcspTr069PA_IsNameInsNumber(pStringToken->Name);
        bInsMatched = CCSP_FALSE;

        /* find if any child node's name matches the token */
        while ( pChildNode )
        {
            if ( AnscEqualString(pStringToken->Name, pChildNode->Name, TRUE) )
            {
                pNode        = pChildNode;
                break;
            }
            else
            {
                if ( bInsNumber && AnscEqualString(pChildNode->Name, "{i}", TRUE) )
                {
                    /* we found wild match - continue searching (no need in most cases), just in case 
                     * exact instance number and wild format may co-exist, sounds crazy?
                     * Do so to be safe just in case this may really happen! E.g. if we have pre-defined
                     * table entry which is not allowed to be removed and we want table object just
                     * as writable.
                     */
                    bInsMatched = CCSP_TRUE;
                    pInsNode    = pChildNode;
                }

                pChildNode   = pChildNode->Sibling;
            }
        }

        if ( !pChildNode )
        {
            if ( !bInsMatched )
            {
                /* no match - we found the last "matching" node */
                break;
            }
            else
            {
                pChildNode = pInsNode;
                pInsNode   = NULL;
            }
        }

        AnscFreeMemory(pStringToken);

        /* go one level deeper */
        pNode        = pChildNode;
    }

    if ( pStringToken )
    {
        AnscFreeMemory(pStringToken);
    }

    if ( pNsTokenChain )
    {
        AnscTcFree((ANSC_HANDLE)pNsTokenChain);
    }

    return  pChildNode;
}


typedef struct
_CCSP_TR069PA_ENUM_NS
{
    CCSP_STRING*                    pSubsystemArray;
    CCSP_INT*                       SubsystemArraySize;
    CCSP_INT                        MaxSubsystemArraySize;
}
CCSP_TR069PA_ENUM_NS, *PCCSP_TR069PA_ENUM_NS;

static
CCSP_BOOL
CcspTr069PA_GetNsSubsystemCB
    (
        CCSP_HANDLE                 pContext,
        PCCSP_TR069_PARAM_INFO      pParamInfo
    )
{
    PCCSP_TR069PA_ENUM_NS           pEnumNs = (PCCSP_TR069PA_ENUM_NS)pContext;
    CCSP_INT                        i;
    CCSP_INT*                       pCount  = pEnumNs->SubsystemArraySize;
    
    for ( i = 0; i < *pCount; i ++ )
    {
        if ( (!pParamInfo->Subsystem && !pEnumNs->pSubsystemArray[i]) || 
             AnscEqualString(pParamInfo->Subsystem, pEnumNs->pSubsystemArray[i], TRUE) )
        {
            return CCSP_TRUE;
        }
    }

    pEnumNs->pSubsystemArray[*pCount] = pParamInfo->Subsystem ? CcspTr069PaCloneString(pParamInfo->Subsystem) : NULL;
    (*pCount) ++;

    if ( *pCount >= pEnumNs->MaxSubsystemArraySize )
    {
        return CCSP_FALSE;
    }

    return CCSP_TRUE;
}


/* CcspTr069PA_GetNamespaceSubsystems is called to check if the given
 * namespace is on different sub-system than PA is running on.
 *
 * return value: void
 *
 * output argument:
 *  ppSubsysArray: array of subsystems on which there's at least one
 *  FC owns namespace that matches the one being requested.
 *  pSubsysArraySize: number of subsystems own namesapces that
 *  match the one being requested.
 */
CCSP_VOID
CcspTr069PA_GetNamespaceSubsystems
    (
        CCSP_HANDLE                 MapperHandle,
        CCSP_STRING                 Namespace,          /* namespace to be queried */
        CCSP_STRING*                pSubsystems,        /* string array allocated by caller */
        CCSP_INT*                   pNumSubsystems,     /* [in/out] max number of strings  */
        CCSP_BOOL                   bCheckInvPiTree     /* whether or not check invisible parameter tree */
    )
{
    PCCSP_TR069_PARAM_INFO          pParamInfo;
    CCSP_INT                        NumOfSubsystems     = 0;
    CCSP_INT                        MaxNumOfSubsystems  = *pNumSubsystems;
    CCSP_TR069PA_ENUM_NS            EnumNs;
    CCSP_BOOL                       bFoundInPiTree      = CCSP_FALSE;

    AnscZeroMemory(pSubsystems, sizeof(CCSP_STRING) * MaxNumOfSubsystems);
    *pNumSubsystems   = 0;

    pParamInfo = CcspTr069PA_FindNamespace(CcspTr069PiTree, Namespace);

    if ( pParamInfo )
    {
        bFoundInPiTree = CCSP_TRUE;

        EnumNs.MaxSubsystemArraySize = MaxNumOfSubsystems;
        EnumNs.SubsystemArraySize    = &NumOfSubsystems;
        EnumNs.pSubsystemArray       = pSubsystems;
    
        CcspTr069PA_TraversePiTree
            (
                MapperHandle,
                pParamInfo,
                TRUE,
                NULL,
                CcspTr069PA_GetNsSubsystemCB,
                &EnumNs
            );

        *pNumSubsystems = NumOfSubsystems;
    }

    if ( !bCheckInvPiTree )
    {
        return;
    }

    pParamInfo = CcspTr069PA_FindNamespace(CcspTr069InvPiTree, Namespace);

    if ( pParamInfo )
    {
        if ( !bFoundInPiTree )
        {
            EnumNs.MaxSubsystemArraySize = MaxNumOfSubsystems;
            EnumNs.SubsystemArraySize    = &NumOfSubsystems;
            EnumNs.pSubsystemArray       = pSubsystems;
        }
    
        CcspTr069PA_TraversePiTree
            (
                MapperHandle,
                pParamInfo,
                TRUE,
                NULL,
                CcspTr069PA_GetNsSubsystemCB,
                &EnumNs
            );

        *pNumSubsystems = NumOfSubsystems;
    }
}


/* CcspTr069PA_IsNamespaceSupported is called to check if the given
 * namespace is supported on given sub-system.
 */
const CCSP_BOOL
CcspTr069PA_IsNamespaceSupported
    (
        CCSP_HANDLE                 MapperHandle,
        CCSP_STRING                 Namespace,          /* namespace to be queried */
        CCSP_STRING                 Subsystem,          /* sub-system name */
        CCSP_BOOL                   bCheckInvPiTree     /* whether or not check invisible parameter tree */
    )
{
    UNREFERENCED_PARAMETER(MapperHandle);
    PCCSP_TR069_PARAM_INFO          pParamInfo;
    BOOL                            bPartialNs = CcspCwmpIsPartialName(Namespace);

    pParamInfo = CcspTr069PA_FindNamespace(CcspTr069PiTree, Namespace);

    if ( !pParamInfo )
    {
        if ( bCheckInvPiTree )
        {
            pParamInfo = CcspTr069PA_FindNamespace(CcspTr069InvPiTree, Namespace);
        }

        if ( !pParamInfo )
        {
            return  CCSP_FALSE;
        }
    }

    if ( bPartialNs )
    {
        return (pParamInfo != NULL);
    }
    else
    {
        BOOL                        bSubsysMatched = FALSE;

        if ( ( !Subsystem && !pParamInfo->Subsystem ) ||
             (Subsystem && pParamInfo->Subsystem && AnscEqualString(Subsystem, pParamInfo->Subsystem, TRUE) ))
        {
            bSubsysMatched = TRUE;
        }

        return 
            pParamInfo          && 
            !pParamInfo->Child  && 
            bSubsysMatched;
    }
}


/* CcspTr069PA_IsNamespaceInvisible is called to check if the given
 * namespace is invisible to cloud server.
 */
const CCSP_BOOL
CcspTr069PA_IsNamespaceInvisible
    (
        CCSP_HANDLE                 MapperHandle,
        CCSP_STRING                 Namespace           /* namespace to be queried */
    )
{
    UNREFERENCED_PARAMETER(MapperHandle);
    PCCSP_TR069_PARAM_INFO          pParamInfo;

    pParamInfo = CcspTr069PA_FindNamespace(CcspTr069InvPiTree, Namespace);

    return ( pParamInfo && !pParamInfo->Child );
}


/*
 * CcspTr069PA_TraversePiTree traverses given parameter info
 * tree by given filter by - leaf node only, Subsystem.
 * Once such node is browsed, invoke callback function.
 */
CCSP_VOID
CcspTr069PA_TraversePiTree
    (
        CCSP_HANDLE                 MapperHandle,
        PCCSP_TR069_PARAM_INFO      pRoot,
        CCSP_BOOL                   bLeafNodeOnly,
        CCSP_STRING                 Subsystem,
        PFN_CCSP_TR069PA_PITREE_CB  TraversalCB,
        CCSP_HANDLE                 pCBContext
    )
{
    UNREFERENCED_PARAMETER(MapperHandle);
    CCSP_BOOL                       bMatched = CCSP_TRUE;
    QUEUE_HEADER                    NodeStack;
    PSINGLE_LINK_ENTRY              pLinkEntry;
    PCCSP_TR069_PARAM_INFO          pNode;
    CCSP_BOOL                       bContinue;

    if ( !pRoot || !TraversalCB )
    {
        return;
    }

    AnscInitializeQueue(&NodeStack);
    AnscQueuePushEntryAtFront(&NodeStack, &pRoot->Linkage);

    while ( (pLinkEntry = AnscQueuePopEntry(&NodeStack)) )
    {
        pNode = ACCESS_CCSP_TR069_PI_INFO(pLinkEntry);

        bMatched = CCSP_TRUE;

        if ( pNode->Child && bLeafNodeOnly )
        {
            bMatched = CCSP_FALSE;
        }
        
        if ( bMatched && Subsystem && !AnscEqualString(Subsystem, pNode->Subsystem, TRUE) )
        {
            bMatched = CCSP_FALSE;
        }

        if ( bMatched )
        {
            bContinue = (*TraversalCB)(pCBContext, pNode);

            if ( !bContinue )
            {
                /* no more traversal is needed */
                break;
            }
        }

        /* push child onto stack */
        if ( pNode->Child )
        {
            AnscQueuePushEntryAtFront(&NodeStack, &pNode->Child->Linkage);
        }

        /* push siblings onto stack */
        if ( pNode->Sibling )
        {
            AnscQueuePushEntryAtFront(&NodeStack, &pNode->Sibling->Linkage);
        }
    }
}


/*
 * CcspTr069PA_TraverseRemotePiTree traverses parameter info
 * tree that only contains namespaces on remote sub-systems.
 */
CCSP_VOID
CcspTr069PA_TraverseRemotePiTree
    (
        CCSP_HANDLE                 MapperHandle,
        CCSP_BOOL                   bLeafNodeOnly,
        CCSP_STRING                 Subsystem,
        PFN_CCSP_TR069PA_PITREE_CB  TraversalCB,
        CCSP_HANDLE                 pCBContext
    )
{
    CcspTr069PA_TraversePiTree
        (
            MapperHandle,
            CcspTr069PiTree,
            bLeafNodeOnly,
            Subsystem,
            TraversalCB,
            pCBContext
        );
}


int
CcspTr069PA_GetPiFullName
    (
        PCCSP_TR069_PARAM_INFO      pParamInfo,
        char*                       pFnBuf,
        int                         nFnBufLen
    )
{
    PCCSP_TR069_PARAM_INFO          pPiNode = pParamInfo;
    char                            im[CCSP_TR069_RPC_Namespace_Length+1];
    
    *pFnBuf = 0;
   
    while ( pPiNode )
    {
        if ( !pPiNode->Name || pPiNode->Name[0] == 0 )
        {
            break;
        }

        if ( pFnBuf[0] == 0 )
        {
            AnscCopyString(pFnBuf, pPiNode->Name);
        }
        else
        {
        if ( AnscSizeOfString(pPiNode->Name) + AnscSizeOfString(pFnBuf) + 1 >= (unsigned int)nFnBufLen )
        {
            return  -1;     /* buffer is not big enough? This is VIOLATION of TR-069 specification which defines 256 as maximum length of parameter name */
        }

            _ansc_sprintf(im, "%s.%s", pPiNode->Name, pFnBuf);
            AnscCopyString(pFnBuf, im);
        }

        pPiNode = pPiNode->Parent;
    }

    if ( pParamInfo->PartialName )
    {
        AnscCopyString(pFnBuf + AnscSizeOfString(pFnBuf), ".");
    }

    return  0;
}


#ifdef   _DEBUG

PCCSP_TR069_PARAM_INFO
CcspTr069PA_GetPiTreeRoot
    (
        CCSP_BOOL                   bInvPiTree
    )
{
    return   bInvPiTree ? CcspTr069InvPiTree : CcspTr069PiTree;
}

#endif
