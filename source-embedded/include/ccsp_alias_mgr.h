/*********************************************************************
 * Copyright 2018-2019 ARRIS Enterprises, LLC.
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
 **********************************************************************/

#ifndef  _CCSP_COMMON_ALIAS_MANAGER_DEC_
#define  _CCSP_COMMON_ALIAS_MANAGER_DEC_

#include "ansc_platform.h"
#include "ansc_xml_dom_parser_interface.h"

/***********************************************************
   CCSP Alias Manager External APIs
***********************************************************/

ANSC_HANDLE
CcspAliasMgrInitialize();

VOID
CcspAliasMgrFree
    (
        ANSC_HANDLE               hAliasMgr
    );

BOOL
CcspAliasMgrLoadAliasInfo
    (
        ANSC_HANDLE               hAliasMgr,
        PANSC_XML_DOM_NODE_OBJECT pNode
    );

PSLIST_HEADER
CcspAliasMgrGetInternalNames
    (
        ANSC_HANDLE               hAliasMgr,
        char*                     ParamName
    );

const char*
CcspAliasMgrGetNextName
    (
        PSLIST_HEADER             pNamesList
    );

VOID
CcspAliasMgrFreeNamesList
    (
        PSLIST_HEADER             pNamesList
    );

const char*
CcspAliasMgrGetFirstInternalName
    (
        ANSC_HANDLE               hAliasMgr,
        char*                     ParamName
    );

const char*
CcspAliasMgrGetExternalName
    (
        ANSC_HANDLE               hAliasMgr,
        char*                     ParamName
    );

#endif
