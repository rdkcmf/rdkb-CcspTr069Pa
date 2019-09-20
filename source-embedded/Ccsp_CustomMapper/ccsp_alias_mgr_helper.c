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

#include "ansc_platform.h"
#include "ansc_xml_dom_parser_interface.h"
#include "ansc_xml_dom_parser_status.h"
#include "ansc_xml_dom_parser_external_api.h"
#include "ansc_debug.h"

#include "ccsp_alias_mgr.h"
#include "ccsp_alias_mgr_helper.h"

// Maximum mapper file size, same as in TR069 PA Mapper
#define ALIAS_MANAGER_MAX_MAPPER_FILE_SIZE 500000

/**********************************************************************

    prototype:

        CcspAliasMgrLoadFromXMLFile
        (
            void*  pXMLHandle
        )

    description:

        Load alis items from XML file.

    argument: void                             pXMLHandle
              XML document handle

    return:     TRUE  if the operation succeeded
                FALSE if there was a failure

**********************************************************************/
static
BOOL
CcspAliasMgrLoadFromXMLFile
    (
        ANSC_HANDLE hAliasMgr,
        void*       pXMLHandle
    )
{
    PANSC_XML_DOM_NODE_OBJECT       pHandle        = (PANSC_XML_DOM_NODE_OBJECT)pXMLHandle;
    PANSC_XML_DOM_NODE_OBJECT       pListNode      = NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode     = NULL;

    if( pXMLHandle == NULL)
    {
        return FALSE;
    }

    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)pHandle->GetHeadChild(pHandle);

    do
    {
        if (AnscEqualString(pChildNode->Name, "AliasList", TRUE))
        {
            pListNode = (PANSC_XML_DOM_NODE_OBJECT)pChildNode->GetHeadChild(pChildNode);
            while (pListNode != NULL)
            {
                CcspAliasMgrLoadAliasInfo(hAliasMgr, pListNode);
                pListNode = (PANSC_XML_DOM_NODE_OBJECT)pChildNode->GetNextChild(pChildNode, pListNode);
            }
        }
    }
    while ((pChildNode = (PANSC_XML_DOM_NODE_OBJECT)pHandle->GetNextChild(pHandle, pChildNode)) != NULL);

    return TRUE;
}

/**********************************************************************

    prototype:

        BOOL
        CcspAliasMgrCheckFileExists
        (
           const char *path
        )

    description:

        Check if the file exists.

    argument: char*                             path
              File path

    return:   TRUE  if the file exists
              FALSE if the file doesn't exist

**********************************************************************/
static
BOOL
CcspAliasMgrCheckFileExists
    (
        const char *path
    )
{
    FILE *file;

    /* Try and open the config file */
    if( ( file = fopen( path, "rb" ) ) == NULL )
    {
        return FALSE;
    }

    fclose( file );

    return TRUE;
}

/**********************************************************************

    prototype:

        CcspAliasMgrLoadMappingFile
        (
            CCSP_STRING                 mappingFile
        )

    description:

        Load alias mapping file.

    argument: void                             pXMLHandle
              XML document handle

    return:     TRUE  if the operation succeeded
                FALSE if there was a failure

**********************************************************************/
BOOL
CcspAliasMgrLoadMappingFile
    (
        ANSC_HANDLE                 hAliasMgr,
        char*                       mappingFile
    )
{
    BOOL     bSucc         = FALSE;
    struct stat   statBuf;

    /* load from XML file */
    PANSC_XML_DOM_NODE_OBJECT       pRootNode   = NULL;

    CcspTraceDebug(("AliasManager is loading mapper file <%s> ...\n", mappingFile));


    if (CcspAliasMgrCheckFileExists( mappingFile ) )
    {
        int fileHandle  = open(mappingFile,  O_RDONLY);
        fstat(fileHandle,&statBuf);
        int contentSize = statBuf.st_size;

        if ( contentSize > ALIAS_MANAGER_MAX_MAPPER_FILE_SIZE )
        {
            CcspTraceError(("Alias Manager mapper file loading error - file is too big!\n"));
        }
        else
        {
            char * pFileContent = AnscAllocateMemory(contentSize + 1);

            if ( pFileContent )
            {
                AnscZeroMemory(pFileContent, contentSize + 1);

                if (read((int)fileHandle, pFileContent, contentSize) > 0)
                {
                    /* Some Unicode files may have hidden content at the beginning. So search for the first '<' to begin the XML parse. */
                    PCHAR pBack = pFileContent;
                    while(pBack && *pBack != '<') pBack++;
                    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
                    AnscXmlDomParseString((ANSC_HANDLE)NULL, (PCHAR*)&pBack, contentSize);
                }

                if (pRootNode != NULL)
                {
                    bSucc = CcspAliasMgrLoadFromXMLFile(hAliasMgr, (void*)pRootNode);

                    pRootNode->Remove(pRootNode);
                }

                AnscFreeMemory(pFileContent);
            }
        }

        close(fileHandle);

        CcspTraceError(("Alias Manager mapper file <%s> has been loaded!\n", mappingFile));
    }
    else
    {
        CcspTraceError(("Alias Manager mapper file <%s> is NOT found!\n", mappingFile));
    }

    return bSucc;
}
