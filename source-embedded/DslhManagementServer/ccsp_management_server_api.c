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

    module: ccsp_management_server_api.c

        For CCSP management server component

    ---------------------------------------------------------------

    description:

        This file implements all api functions of management server 
        component.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Hui Ma 

    ---------------------------------------------------------------

    revision:

        06/15/2011    initial revision.

**********************************************************************/
#include "ccsp_management_server.h"
#include "ccsp_tr069pa_wrapper_api.h"
#include "ccsp_management_server_api.h"
#include "ccsp_trace.h"

extern char *CcspManagementServer_ComponentName;
extern msObjectInfo *objectInfo;
extern msParameterValSettingArray parameterSetting;

int CcspManagementServer_SetParameterValues(
    int sessionId,
    unsigned int writeID,
    parameterValStruct_t *val,
    int size,
    dbus_bool commit,
    char ** invalidParameterName,
    void            *user_data
    )
{
    //CcspTraceWarning("ms", ( "CcspManagementServer_SetParameterValues 0: %d.\n", commit)); 
    /*  Validate first. At the same time the values are stored in to parameterSetting. */
    int validateResult = CcspManagementServer_ValidateParameterValues(sessionId, writeID, val, size, invalidParameterName);
    if(validateResult != 0) {
        /* Free the allocated data. */
        CcspManagementServer_FreeParameterSetting();
        return validateResult;
    }
    //CcspTraceWarning("ms", ( "CcspManagementServer_SetParameterValues 1: %d.\n", validateResult)); 

    if(commit)
    {
        //CcspTraceWarning("ms", ( "CcspManagementServer_SetParameterValues 2.\n")); 
        return CcspManagementServer_SetCommit(sessionId, writeID, commit, user_data);
    }
    return CCSP_SUCCESS;
}

int  CcspManagementServer_SetCommit(
    int sessionId,
    unsigned int writeID,
    dbus_bool commit,
    void* user_data
    )
{
    UNREFERENCED_PARAMETER(sessionId);
    UNREFERENCED_PARAMETER(user_data);
    if(commit)
    {
        CcspManagementServer_CommitParameterValues(writeID);
    }
    /* Free the allocated data. */
    CcspManagementServer_FreeParameterSetting();

    return CCSP_SUCCESS;
}

int  CcspManagementServer_GetParameterValues(
    unsigned int writeID, 
    char * parameterNames[],
    int size,
    int *val_size,
    parameterValStruct_t ***param_val,
    void* user_data
    )
{
    UNREFERENCED_PARAMETER(writeID);
    UNREFERENCED_PARAMETER(user_data);
    /* first count return size. */
    int i = 0;
    *val_size = 0;
    for(; i<size; i++)
    {
        char *name = NULL;
        int objectID = CcspManagementServer_GetObjectID(parameterNames[i], &name);
        if(objectID < 0) continue; // skip empty entry
        if(strlen(name) != 0) (*val_size)++;
        else *val_size += (int) CcspManagementServer_GetParameterCount(objectID);
    }
    //CcspTraceWarning("ms", ( "CcspManagementServer_GetParameterValues 1: %d to %d.\n", size, *val_size)); 

    if (*val_size == 0) return CCSP_FAILURE;

    parameterValStruct_t **val = NULL;
    val = (parameterValStruct_t **)CcspManagementServer_Allocate((*val_size)*sizeof(parameterValStruct_t *));
    if(val) {
        for(i=0; i < *val_size; i++) {
            val[i] = NULL;
            val[i] = (parameterValStruct_t *)CcspManagementServer_Allocate(sizeof(parameterValStruct_t));
            if(val[i] == NULL) {
                int j;
                for(j=0; j<i; j++) CcspManagementServer_Free(val[j]);
                CcspManagementServer_Free(val);
                return CCSP_ERR_MEMORY_ALLOC_FAIL;
            }
        }
    }
    else return CCSP_ERR_MEMORY_ALLOC_FAIL;

    int valID = 0;
    for(i=0; i<size; i++)
    {
        char *name = NULL;
        int objectID = CcspManagementServer_GetObjectID(parameterNames[i], &name);
        if(objectID < 0) continue; // skip empty entry

        /* It is a parameter. */
        if(strlen(name) != 0)
        {
            int parameterID = CcspManagementServer_GetParameterID(objectID, name);
            if(parameterID < 0)
            {
                int j;
                for(j=0; j<(*val_size); j++) CcspManagementServer_Free(val[j]);
                CcspManagementServer_Free(val);

                return CCSP_FAILURE;
            }
            else {
            CcspManagementServer_GetSingleParameterValue(objectID, parameterID, val[valID]);
            valID++;
            }
            //CcspTraceWarning("ms", ( "CcspManagementServer_GetParameterValues 2: %s.\n", name)); 
        }
        /* It is an object. */
        else
        {
            valID += CcspManagementServer_GetSingleObjectValues(
                objectID,
                val,
                valID);
            //CcspTraceWarning("ms", ( "CcspManagementServer_GetParameterValues 3: %s. \n", parameterNames[i])); 
        }
    }
    *param_val = val;
    return CCSP_SUCCESS;
}

int  CcspManagementServer_SetParameterAttributes(
    int sessionId,
    parameterAttributeStruct_t *val,
    int size,
    void* user_data
)
{
    UNREFERENCED_PARAMETER(user_data);
    int i = 0;
    for(; i<size; i++)
    {
        char *name;
        int objectID = CcspManagementServer_GetObjectID(val[i].parameterName, &name);
        if(objectID < 0)
        {
            return CCSP_FAILURE;
        }
        /* It is a parameter. */
        if(strlen(name) != 0)
        {
            int parameterID = CcspManagementServer_GetParameterID(objectID, name);
            if(parameterID < 0)
            {
                return CCSP_FAILURE;
            }
            CcspManagementServer_SetSingleParameterAttributes(
                sessionId,
                objectID,
                parameterID,
                &val[i]);
        }
        /* It is an object. */
        else
        {
            CcspManagementServer_SetSingleObjectAttributes(
                sessionId,
                objectID,
                &val[i]);
        }
    }

    return CCSP_SUCCESS;
}

int  CcspManagementServer_GetParameterAttributes(
    char * parameterNames[],
    int size,
    int *val_size,
    parameterAttributeStruct_t ***val,  
    void* user_data
)
{
    UNREFERENCED_PARAMETER(user_data);
    /* first count return size. */
    int i = 0;
    *val_size = 0;
    for(; i<size; i++)
    {
        char *name;
        int objectID = CcspManagementServer_GetObjectID(parameterNames[i], &name);
        if(objectID < 0) continue;
        if(strlen(name) != 0) (*val_size)++;
        else *val_size += CcspManagementServer_GetParameterCount(objectID);
    }
    //CcspTraceWarning("ms", ( "CcspManagementServer_GetParameterAttributes 1: %d to %d.\n", size, *val_size)); 

    parameterAttributeStruct_t **attr = NULL;
    attr = CcspManagementServer_Allocate(*val_size*sizeof(parameterAttributeStruct_t *));
    if(!attr)
    {
        return CCSP_FAILURE;
    }
    for(i=0; i < *val_size; i++) attr[i] = CcspManagementServer_Allocate(sizeof(parameterAttributeStruct_t));

    int attrID = 0;
    for(i=0; i<size; i++)
    {
        char *name;
        int objectID = CcspManagementServer_GetObjectID(parameterNames[i], &name);
        if(objectID < 0) continue;

        /* It is a parameter. */
        if(strlen(name) != 0)
        {
            int parameterID = CcspManagementServer_GetParameterID(objectID, name);
            if(parameterID < 0)
            {
                /*RDKB-7331, CID-33143, free unused resources before exit */
                for(i=0; i < *val_size; i++)
                {
                    if(attr[i])
                    {
                        CcspManagementServer_Free(attr[i]);
                        attr[i] = NULL;
                    }
                }
                CcspManagementServer_Free(attr);
                return CCSP_FAILURE;
            }
            CcspManagementServer_GetSingleParameterAttributes(objectID, parameterID, attr[attrID]);
            attrID++;
            //CcspTraceWarning("ms", ( "CcspManagementServer_GetParameterAttributes 2: %d, %p.\n", attrID, attr[attrID-1])); 
        }
        /* It is an object. */
        else
        {
            attrID += CcspManagementServer_GetSingleObjectAttributes(
                objectID,
                attr,
                attrID);
            //CcspTraceWarning("ms", ( "CcspManagementServer_GetParameterAttributes 3: %d, %p.\n", attrID, attr[attrID-1])); 
        }
    }
    *val = attr;
    return CCSP_SUCCESS;
}

int  CcspManagementServer_AddTblRow(
    int sessionId,
    char * objectName,
    int * instanceNumber,
    void* user_data
)
{
    UNREFERENCED_PARAMETER(sessionId);
    UNREFERENCED_PARAMETER(objectName);
    UNREFERENCED_PARAMETER(instanceNumber);
    UNREFERENCED_PARAMETER(user_data);
    return CCSP_ERR_NOT_SUPPORT;
}

int  CcspManagementServer_DeleteTblRow(
    int sessionId,
    char * objectName,
    int* instanceNumber,
    void* user_data
    )
{
    UNREFERENCED_PARAMETER(sessionId);
    UNREFERENCED_PARAMETER(objectName);
    UNREFERENCED_PARAMETER(instanceNumber);
    UNREFERENCED_PARAMETER(user_data);
    return CCSP_ERR_NOT_SUPPORT;
}

static int CcspManagementServer_GetNameInfoRecordCount(
    int objectID,
    int nextLevel
    )
{
    if(nextLevel) return objectInfo[objectID].numberOfChildObjects + objectInfo[objectID].numberOfParameters;
    else
    {
        int n = 1 + objectInfo[objectID].numberOfParameters;
        int i = 0;
        for(; i<objectInfo[objectID].numberOfChildObjects; i++ )
        {
            n += CcspManagementServer_GetNameInfoRecordCount(objectInfo[objectID].childObjectIDs[i], nextLevel);
        }
        return n;
    }
}
/* return how many records have been added. */
static int CcspManagementServer_GetObjectNames(
    int objectID,
    int nextLevel,
    parameterInfoStruct_t **info,
    int beginInfoID
    )
{
    int infoID = beginInfoID;
    if(nextLevel) 
    {
        int i = 0;
        for(; i < objectInfo[objectID].numberOfParameters; i++ )
        {
            info[infoID]->parameterName = CcspManagementServer_MergeString(objectInfo[objectID].name, objectInfo[objectID].parameters[i].name);
            info[infoID]->writable = (objectInfo[objectID].parameters[i].access == CCSP_RO)? CCSP_FALSE : CCSP_TRUE;
            infoID++;
        }
        for(i = 0; i < objectInfo[objectID].numberOfChildObjects; i++ )
        {
            info[infoID]->parameterName =  CcspManagementServer_CloneString(objectInfo[objectInfo[objectID].childObjectIDs[i]].name);
            info[infoID]->writable = (objectInfo[objectInfo[objectID].childObjectIDs[i]].access == CCSP_RO)? CCSP_FALSE : CCSP_TRUE;
            infoID++;
        }
    }
    else
    {
        info[infoID]->parameterName = CcspManagementServer_CloneString(objectInfo[objectID].name);
        infoID++;
        int i = 0;
        for(; i < objectInfo[objectID].numberOfParameters; i++ )
        {
            info[infoID]->parameterName = CcspManagementServer_MergeString(objectInfo[objectID].name, objectInfo[objectID].parameters[i].name);
            info[infoID]->writable = (objectInfo[objectID].parameters[i].access == CCSP_RO)? CCSP_FALSE : CCSP_TRUE;
            infoID++;
        }
        for(i = 0; i < objectInfo[objectID].numberOfChildObjects; i++ )
        {
            infoID += CcspManagementServer_GetObjectNames(
                objectInfo[objectID].childObjectIDs[i],
                nextLevel,
                info,
                infoID);
        }
    }
    return infoID - beginInfoID;
}

int CcspManagementServer_GetParameterNames(
    char * parameterName,
    int nextLevel,
    int *size,
    parameterInfoStruct_t ***val,
    void* user_data
)
{
    UNREFERENCED_PARAMETER(user_data);
    char *name;
    errno_t rc = -1;
    //CcspTraceWarning("sample_component", ( "CcspManagementServer_GetParameterNames 1: %s\n", parameterName));
    int objectID = CcspManagementServer_GetObjectID(parameterName, &name);
    if(objectID < 0)
    {
        *size = 0;
        return CCSP_FAILURE;
    }
    //CcspTraceWarning("sample_component", ( "CcspManagementServer_GetParameterNames 2: %s\n", name));

    /* It is a parameter. */
    if(strlen(name) != 0)
    {
        //CcspTraceWarning("sample_component", ( "CcspManagementServer_GetParameterNames 3: %s\n", name));
        int parameterID = CcspManagementServer_GetParameterID(objectID, name);
        if(parameterID < 0)
        {
            *size = 0;
            return CCSP_FAILURE;
        }

        parameterInfoStruct_t **info;
        *size = 1;
        info = CcspManagementServer_Allocate(*size*sizeof(parameterInfoStruct_t *));
        if(info == NULL)
        {
            return CCSP_FAILURE;
	}
	info[0] = (parameterInfoStruct_t *) CcspManagementServer_Allocate(sizeof(parameterInfoStruct_t));
	info[0]->parameterName = CcspManagementServer_Allocate(strlen(parameterName)+1);
	rc = strcpy_s(info[0]->parameterName, strlen(parameterName)+1, parameterName);
	ERR_CHK(rc);
	info[0]->writable = (objectInfo[objectID].parameters[parameterID].access == CCSP_RO)? CCSP_FALSE : CCSP_TRUE;
	*val = info;
    }
    /* It is a partial path for an object */
    else
    {
        //CcspTraceWarning("sample_component", ( "CcspManagementServer_GetParameterNames 4: %s %d %s\n", name, objectID, objectInfo[objectID].name));
        parameterInfoStruct_t **info;
        *size = CcspManagementServer_GetNameInfoRecordCount(objectID, nextLevel);
        info = CcspManagementServer_Allocate(*size*sizeof(parameterInfoStruct_t *));
	if(info == NULL)
        {
            return CCSP_FAILURE;
	}
	int i = 0;
	for(;i < * size; i++) info[i] = (parameterInfoStruct_t *) CcspManagementServer_Allocate(sizeof(parameterInfoStruct_t));
	CcspManagementServer_GetObjectNames(
		    objectID,
		    nextLevel,
		    info,
		    0);
		*val = info;
    }
    return CCSP_SUCCESS;
}


