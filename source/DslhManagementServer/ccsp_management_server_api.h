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

    module: ccsp_management_server_api.h

        For CCSP management server component

    ---------------------------------------------------------------

    description:

        This file defines all api functions of management server 
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

#ifndef  _CCSP_MANAGEMENT_SERVER_API_DEC_
#define  _CCSP_MANAGEMENT_SERVER_API_DEC_

#include <pthread.h>
#include <dbus/dbus.h>
#include "ccsp_message_bus.h"
#include "ccsp_base_api.h"

/* The following functions implements DBus server side functions.
 */
/*typedef int  (*CCSPBASEIF_SETPARAMETERVALUES)( */
int CcspManagementServer_SetParameterValues(
    int sessionId,
    unsigned int writeID,
    parameterValStruct_t *val,
    int size,
    dbus_bool commit,
    char ** invalidParameterName,
    void* user_data
);

/*typedef int  (*CCSPBASEIF_SETCOMMIT)(*/
int  CcspManagementServer_SetCommit(
    int sessionId,
    unsigned int writeID,
    dbus_bool commit,
    void* user_data
);

/*typedef int  (*CCSPBASEIF_GETPARAMETERVALUES)(*/
/*int  CcspManagementServer_GetParameterValues(
    char * parameterNames[],
    int size,
    parameterValStruct_t ***val
);*/
int  CcspManagementServer_GetParameterValues(
    unsigned int writeID,   
    char * parameterNames[],
    int size,
    int *val_size,
    parameterValStruct_t ***param_val,
    void* user_data
    );


/*typedef int  (*CCSPBASEIF_SETPARAMETERATTRIBUTES)(*/
int  CcspManagementServer_SetParameterAttributes(
    int sessionId,
    parameterAttributeStruct_t *val,
    int size,
    void* user_data
);

/*typedef int  (*CCSPBASEIF_GETPARAMETERATTRIBUTES)(*/
/*int  CcspManagementServer_GetParameterAttributes(
    char * parameterNames[],
    int size,
    parameterAttributeStruct_t ***val
);*/
int  CcspManagementServer_GetParameterAttributes(
    char * parameterNames[],
    int size,
    int *val_size,
    parameterAttributeStruct_t ***val,  //??????????? miss return size.
    void* user_data
);


/*typedef int  (*CCSPBASEIF_ADDTBLROW)(*/
int  CcspManagementServer_AddTblRow(
    int sessionId,
    char * objectName,
    int * instanceNumber,
    void* user_data
);

/*typedef int  (*CCSPBASEIF_DELETETBLROW)(*/
int  CcspManagementServer_DeleteTblRow(
    int sessionId,
    char * objectName,
    int * instanceNumber,
    void* user_data
);

/*typedef int  (*CCSPBASEIF_GETPARAMETERNAMES)(*/
int  CcspManagementServer_GetParameterNames(
    char * parameterName,
    int nextLevel,
    int *size ,
    parameterInfoStruct_t ***val,
    void* user_data
);



#endif
