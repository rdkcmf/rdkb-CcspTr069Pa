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

    module: ccsp_tr069pa_mapper_api.h

        For CCSP TR-069 PA mapper file

    ---------------------------------------------------------------

    description:

        This file defines all data structures and macros for
        handling CCSP TR-069 PA mapper file.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan 

    ---------------------------------------------------------------

    revision:

        06/07/2011    initial revision.

**********************************************************************/

#ifndef  _CCSP_TR069_MAPPER_API_DEC_
#define  _CCSP_TR069_MAPPER_API_DEC_

#include "ccsp_tr069pa_mapper_def.h"
#include "ccsp_base_api.h"


/* CcspTr069PA_LoadMappingFile is called to load mapping file for TR-069 PA,
 * Return value - transparent handle to caller and will be used for all
 * other APIs.
 */
CCSP_HANDLE
CcspTr069PA_LoadMappingFile
    (
        CCSP_STRING                 MappingFile
    );

/* CcspTr069PA_UnloadMappingFile is called to unload mapper file and
 * remove all allocated resources accordingly.
 * Return value - 0 success, failure otherwise.
 */
CCSP_INT
CcspTr069PA_UnloadMappingFile
    (
        CCSP_HANDLE                 MapperHandle
    );


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
    );


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
    );


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
    );


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
    );


const enum dataType_e
CcspTr069PA_Cwmp2CcspType
    (
        int                         cwmpType
    );

const int
CcspTr069PA_Ccsp2CwmpType
    (
        enum dataType_e             ccspType
    );


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
    );

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
    );


/* CcspTr069PA_IsNamespaceInvisible is called to check if the given
 * namespace is invisible to cloud server.
 */
const CCSP_BOOL
CcspTr069PA_IsNamespaceInvisible
    (
        CCSP_HANDLE                 MapperHandle,
        CCSP_STRING                 Namespace           /* namespace to be queried */
    );

/* CcspTr069PA_IsNamespaceVisible is called to check if the given
 * namespace is visible to cloud server.
 */
const CCSP_BOOL
CcspTr069PA_IsNamespaceVisible
    (
        CCSP_HANDLE                 MapperHandle,
        CCSP_STRING                 Namespace           /* namespace to be queried */
    );

/* CcspTr069PA_GetSubsystemCount is called to return the number
 * of sub-systems of namespaces TR-069 PA is manages.
 */
const CCSP_INT
CcspTr069PA_GetSubsystemCount
    (
        CCSP_HANDLE                 MapperHandle
    );

/* CcspTr069PA_GetSubsystemCount is called to return 
 * sub-system related to PA by index.
 */
const CCSP_STRING
CcspTr069PA_GetSubsystemByIndex
    (
        CCSP_HANDLE                 MapperHandle,
        CCSP_INT                    index
    );


/*
 * PFN_CCSP_TR069PA_PITREE_CB is the prototype
 * of callback function that is invoked during
 * tree traversal.
 *
 * return value:
 *   TRUE: continue traversal
 *   otherwise, break out of tree treversal
 */
typedef 
CCSP_BOOL
(*PFN_CCSP_TR069PA_PITREE_CB)
    (
        CCSP_HANDLE                 pContext,
        PCCSP_TR069_PARAM_INFO      pParamInfo
    );

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
    );

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
    );

int
CcspTr069PA_GetPiFullName
    (
        PCCSP_TR069_PARAM_INFO      pParamInfo,
        char*                       pFnBuf,
        int                         nFnBufLen
    );

#ifdef   _DEBUG

PCCSP_TR069_PARAM_INFO
CcspTr069PA_GetPiTreeRoot
    (
        CCSP_BOOL                   bInvPiTree
    );

#endif

PSLIST_HEADER
CcspTr069PA_GetParamInternalNames
    (
        CCSP_HANDLE                MapperHandle,
        CCSP_STRING                ParamName
    );

const char *
CcspTr069PA_GetNextInternalName
    (
        PSLIST_HEADER              pListHeader
    );

const char *
CcspTr069PA_GetParamFirstInternalName
    (
        CCSP_HANDLE                MapperHandle,
        CCSP_STRING                ParamName
    );

VOID
CcspTr069PA_FreeInternalNamesList
    (
        PSLIST_HEADER             pListHeader
    );

CCSP_STRING
CcspTr069PA_GetParamExternalName
    (
        CCSP_HANDLE                MapperHandle,
        CCSP_STRING                ParamName
    );
ANSC_STATUS
CcspTr069PaMapFirstInternalAlias
    (
        CCSP_HANDLE                hTr069PaMapper,
        CCSP_STRING*               pParamName,
        BOOL*                      pbIncludeInvQuery,
        BOOL                       bFreeMemory
    );

VOID
CcspTr069PaMapToExternalAlias
    (
        CCSP_HANDLE                hTr069PaMapper,
        CCSP_STRING*               pParamName
    );

CCSP_BOOL
CcspTr069PA_CheckFileExists
    (
        const char*                path
    );

/* CcspTr069PA_LoadCustomMappingFile is called to load additional mapping file for TR-069 PA.
 * Resource clean up happens in CcspTr069PA_UnloadMappingFile().
 * Accepts CCSP_HANDLE returned from CcspTr069PA_LoadMappingFile() as an input.
 */
void
CcspTr069PA_LoadCustomMappingFile
    (
        CCSP_HANDLE                 CcspHandle,
        CCSP_STRING                 MappingFile
    );
#endif
