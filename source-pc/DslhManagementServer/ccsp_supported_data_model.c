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

    module: ccsp_supported_data_model.c

        For CCSP Device.DeviceInfo.SupportedDataModel.

    ---------------------------------------------------------------

    description:

        This file implements all internal functions for supporting
        Device.DeviceInfo.SupportedDataModel.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Hui Ma 

    ---------------------------------------------------------------

    revision:

        07/19/2011    initial revision.

**********************************************************************/
#include "ccsp_types.h"
#include "ccsp_trace.h"
#include "ccsp_supported_data_model.h"
#include "ccsp_management_server.h"
#include "ccsp_management_server_pa_api.h"
#include "ansc_platform.h"
#include "ansc_string.h"


char* _SupportedDataModelConfigFile = NULL;
extern char *CcspManagementServer_ComponentName;
extern msObjectInfo *objectInfo;
int                 sdmObjectNumber = 0;

/* Check for the existence of a config file path */
static CCSP_BOOL CheckFileExists( const char *path )
{
    FILE *file;

    /* Try and open the config file */  
    if( ( file = fopen( path, "rb" ) ) == NULL )
    {
        CcspTraceWarning(("Cannot open configuration file '%s'\n", path));
        return FALSE;
    }

    fclose( file );

    return TRUE;
}

CCSP_VOID
CcspManagementServer_FillInSDMObjectInfo()
{
    struct stat   statBuf       = {0};

    /* load from XML file */
    PANSC_XML_DOM_NODE_OBJECT       pRootNode   = NULL;

    if( ( _SupportedDataModelConfigFile ) && ( CheckFileExists( _SupportedDataModelConfigFile ) ) )
    {
        CCSP_INT fileHandle   = open(_SupportedDataModelConfigFile,  O_RDONLY);
        CCSP_INT iContentSize = 0;

        /*RDKB-7334, CID-33035, validate file open*/
        if(fileHandle != -1)
        {
            fstat(fileHandle,&statBuf);
            iContentSize = statBuf.st_size;

            if( iContentSize < 500000)
            {
                char * pOrigFileContent = CcspManagementServer_Allocate(iContentSize + 1);
                /*RDKB-7334, CID-33035, null check and use*/
                if(pOrigFileContent)
                {
                    char * pFileContent = pOrigFileContent;  /*Duplicate the pointer, to prevant leak*/
                    memset(pFileContent, 0, iContentSize + 1);

                    if( read((int)fileHandle, pFileContent, iContentSize) > 0)
                    {
                        pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
                            AnscXmlDomParseString((ANSC_HANDLE)NULL, (PCHAR*)&pFileContent, iContentSize); /*"pFileContent" may get udpated*/
                    }

                    /* loca from the node */
                    if( pRootNode != NULL)
                    {
                        pRootNode->Remove(pRootNode);
                    }
                    /*RDKB-7334, CID-33035, free memory after use*/
                    CcspManagementServer_Free(pOrigFileContent);
                    pOrigFileContent = pFileContent = NULL;
                }

            }

            close(fileHandle);
        }
    }
    else
    {
        sdmObjectNumber = 0;
        objectInfo = (msObjectInfo *)CcspManagementServer_Allocate((SupportedDataModelID + sdmObjectNumber + 1) * sizeof(msObjectInfo));

        objectInfo[SupportedDataModelID].name = CcspManagementServer_CloneString(_SupportedDataModelObjectName);
        objectInfo[SupportedDataModelID].numberOfChildObjects = sdmObjectNumber;
        objectInfo[SupportedDataModelID].childObjectIDs = NULL;
        objectInfo[SupportedDataModelID].numberOfParameters = 0;
        objectInfo[SupportedDataModelID].parameters = NULL;
        objectInfo[SupportedDataModelID].access = CCSP_RO;
    }
}



/* CcspManagementServer_GetSupportedDataModel_URL is called to get
 * Device.DeviceInfo.SupportedDataModel.URL.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSupportedDataModel_URL
    (
        CCSP_STRING                 ComponentName,
        int                         ObjectID
    )
{
    UNREFERENCED_PARAMETER(ComponentName);

    return CcspManagementServer_CloneString(objectInfo[ObjectID].parameters[SupportedDataModelURLID].value);
}

/* CcspManagementServer_GetSupportedDataModel_URN is called to get
 * Device.DeviceInfo.SupportedDataModel.URN.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSupportedDataModel_URN
    (
        CCSP_STRING                 ComponentName,
        int                         ObjectID
    )
{
    UNREFERENCED_PARAMETER(ComponentName);

    return CcspManagementServer_CloneString(objectInfo[ObjectID].parameters[SupportedDataModelURNID].value);
}

/* CcspManagementServer_GetSupportedDataModel_Features is called to get
 * Device.DeviceInfo.SupportedDataModel.Features.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSupportedDataModel_Features
    (
        CCSP_STRING                 ComponentName,
        int                         ObjectID
    )
{
    UNREFERENCED_PARAMETER(ComponentName);

    return CcspManagementServer_CloneString(objectInfo[ObjectID].parameters[SupportedDataModelFeaturesID].value);
}
