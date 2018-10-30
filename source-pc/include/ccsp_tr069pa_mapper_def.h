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

    module: ccsp_tr069pa_mapper_def.h

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

#ifndef  _CCSP_TR069_MAPPER_DEF_DEC_
#define  _CCSP_TR069_MAPPER_DEF_DEC_

#include "ccsp_types.h"

/* data structure for mapping CCSP error code to TR-069 CPE error code */
typedef struct
_CCSP_TR069_CPEERR_MAP
{
    CCSP_INT                        CcspErrorCode;
    CCSP_INT                        Tr069CpeErrorCode;
}
CCSP_TR069_CPEERR_MAP, *PCCSP_TR069_CPEERR_MAP;

/*CWMP_2_DM_INT_INSTANCE_NUMBER_MAPPING*/
#define CCSP_TR069_INSTMAP_MaxStringSize 128

/* data structure for mapping CCSP instance numbers to TR-069 CPE instance numbers */
typedef struct
_CCSP_TR069_CPEINSTNUM_MAP
{
    CCSP_INT                        CcspInstanceNumber;
    CCSP_INT                        Tr069CpeInstanceNumber;
}
CCSP_TR069_CPEINSTNUM_MAP, *PCCSP_TR069_CPEINSTNUM_MAP;

typedef struct
_CCSP_TR069_CPEINSTANCE_MAP 
{
    CCSP_STRING                     CcspDmlName;   /* Data Model Name */
    CCSP_INT                        numMaps;
    PCCSP_TR069_CPEINSTNUM_MAP      InstanceMap;
}
CCSP_TR069_CPEINSTANCE_MAP, *PCCSP_TR069_CPEINSTANCE_MAP;

/*CWMP_2_DM_INT_INSTANCE_NUMBER_MAPPING ends*/

/* data structure for mapping TR-069 RPC methods to CCSP namespaces */
#define  CCSP_TR069_RPC_TYPE_Unknown                0
#define  CCSP_TR069_RPC_TYPE_Download               1
#define  CCSP_TR069_RPC_TYPE_Upload                 2
#define  CCSP_TR069_RPC_TYPE_Reboot                 3
#define  CCSP_TR069_RPC_TYPE_FactoryReset           4
#define  CCSP_TR069_RPC_TYPE_InstallPackage         5
#define  CCSP_TR069_RPC_TYPE_UpdatePackage          6
#define  CCSP_TR069_RPC_TYPE_RemovePackage          7

#define CCSP_TR069_RPC_Namespace_Length             256
#define CCSP_TR069_RPC_ArgName_Length               100
#define CCSP_TR069_Subsystem_Length                 32
/*CWMP_2_DM_INT_INSTANCE_NUMBER_MAPPING*/
#define CCSP_TR069_DmlTable_Length                  128

typedef  struct
_CCSP_RPC_ARG_MAP
{
    CCSP_STRING                     Name;                   /* TR-069 RPC argument name */
    CCSP_STRING                     MappedNS;               /* Namespace that argument maps to */
}
CCSP_RPC_ARG_MAP, *PCCSP_RPC_ARG_MAP;

typedef struct
_CCSP_TR069_RPC_MAP
{
    CCSP_INT                        RpcType;
    CCSP_STRING                     Namespace;              /* namespace for Reboot, FactoryReset that do not have any arguments */    

    CCSP_INT                        NumOfArgs;              /* number of arguments that can be mapped */
    PCCSP_RPC_ARG_MAP               ArgMaps;                /* argument maps */
}
CCSP_TR069_RPC_MAP, *PCCSP_TR069_RPC_MAP;


/* 
 * Data structure for parameter info - can be imported from TR-069 PA mapper file.
 * Parameter list only includes parameters that their owning FCs are running on
 * different sub-systems. We don't list (or ignore) those their owning FCs are
 * running on the same sub-system as TR-069 PA.
 */

#define  CCSP_CLOUD_DATA_TYPE_NAME_STRING           "string"
#define  CCSP_CLOUD_DATA_TYPE_NAME_INT              "int"
#define  CCSP_CLOUD_DATA_TYPE_NAME_UINT             "uint"
#define  CCSP_CLOUD_DATA_TYPE_NAME_BOOL             "bool"
#define  CCSP_CLOUD_DATA_TYPE_NAME_DATETIME         "dateTime"
#define  CCSP_CLOUD_DATA_TYPE_NAME_BASE64           "base64"

#define  CCSP_CLOUD_DATA_TYPE_UNSPECIFIED           0
#define  CCSP_CLOUD_DATA_TYPE_OBJECT                1
#define  CCSP_CLOUD_DATA_TYPE_STRING                2
#define  CCSP_CLOUD_DATA_TYPE_INT                   3
#define  CCSP_CLOUD_DATA_TYPE_UINT                  4
#define  CCSP_CLOUD_DATA_TYPE_BOOL                  5
#define  CCSP_CLOUD_DATA_TYPE_DATETIME              6
#define  CCSP_CLOUD_DATA_TYPE_BASE64                7


typedef  struct
_CCSP_TR069_PARAM_INFO
{
    SINGLE_LINK_ENTRY               Linkage;                /* used internally for tree traversal */
    CCSP_STRING                     Name;                   /* parameter name */
    CCSP_BOOL                       PartialName;            /* is parameter name a partial one or not */
    CCSP_UINT                       CloudType;              /* cloud data type, one of CCSP_CLOUD_DATA_TYPE_xxx */
    CCSP_BOOL                       ReadOnly;               /* if parameter is read-only */
    CCSP_STRING                     Subsystem;              /* sub-system name */
    CCSP_BOOL                       Invisible;              /* Invisible or not to ACS this PA connects to */

    struct _CCSP_TR069_PARAM_INFO*  Child;                  /* child */
    struct _CCSP_TR069_PARAM_INFO*  Sibling;                /* sibling */

    struct _CCSP_TR069_PARAM_INFO*  Parent;                 /* parent */
}
CCSP_TR069_PARAM_INFO, *PCCSP_TR069_PARAM_INFO;

#define  ACCESS_CCSP_TR069_PI_INFO(p)               \
         ACCESS_CONTAINER(p, CCSP_TR069_PARAM_INFO, Linkage)


#define  CCSP_SUBSYSTEM_MAX_COUNT                   16


#endif
