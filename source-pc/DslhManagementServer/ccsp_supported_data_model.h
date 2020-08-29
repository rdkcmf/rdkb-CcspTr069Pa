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

    module: ccsp_supported_data_model.h

        For CCSP Device.DeviceInfo.SupportedDataModel.

    ---------------------------------------------------------------

    description:

        This file defines all internal functions for supporting
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

#ifndef  _CCSP_SUPPORTED_DATA_MODEL_
#define  _CCSP_SUPPORTED_DATA_MODEL_

#include "ccsp_types.h"
#include <pthread.h>
#include <dbus/dbus.h>
#include "ccsp_message_bus.h"
#include "ccsp_base_api.h"
#include "stdio.h"
#include "string.h"
#include <sys/stat.h>
#include "linux/user_base.h"
#include "ansc_wrapper_base.h"
#include "ansc_common_structures.h"
#include "ansc_status.h"
#include "ansc_string.h"
#include "ccsp_trace.h"
#include "ansc_xml_dom_parser_interface.h"
#include "ansc_xml_dom_parser_external_api.h"
#include "ansc_xml_dom_parser_status.h"
#include "ansc_xml_dom_parser_def.h"
#include "ccsp_custom.h"

#define _DeviceInfoObjectName         DM_ROOTNAME"DeviceInfo."
#define _SupportedDataModelObjectName         DM_ROOTNAME"DeviceInfo.SupportedDataModel."
#define _SupportedDataModelTableName    DM_ROOTNAME"DeviceInfo.SupportedDataModel.{i}."

/*
   The "correct" path for the sdm.xml file is unclear. Previously the file
   was installed to /usr/ccsp/tr069pa/sdm.xml but accessed via ./sdm.xml
   Since ./sdm.xml is a relative path, it required a symlink from /usr/bin/sdm.xml
   back to the actual file (/usr/bin is presumably the current directory
   when CcspTr069PaSsp is run?). Changing the definition below should hopefully
   allow the symlink to be removed.
*/
#define _CCSP_MANAGEMENT_SERVER_DEFAULT_SDM_FILE    "/usr/ccsp/tr069pa/sdm.xml"

#define CCSP_SUPPORTED_DATA_MODEL_PARAMETER_NAME_LENGTH           9
#define CCSP_SUPPORTED_DATA_MODEL_PARAMETER_VALUE_LENGTH          1000

#define SupportedDataModelURLID             0
#define SupportedDataModelURNID             1             
#define SupportedDataModelFeaturesID        2          

/* Called by init function to fill in parameter values from config file.
 */
CCSP_VOID CcspManagementServer_FillInSDMObjectInfo();


/* CcspManagementServer_GetSupportedDataModel_URL is called to get
 * Device.DeviceInfo.SupportedDataModel.URL.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSupportedDataModel_URL
    (
        CCSP_STRING                 ComponentName,
        int                         ObjectID
    );

/* CcspManagementServer_GetSupportedDataModel_URN is called to get
 * Device.DeviceInfo.SupportedDataModel.URN.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSupportedDataModel_URN
    (
        CCSP_STRING                 ComponentName,
        int                         ObjectID
    );

/* CcspManagementServer_GetSupportedDataModel_Features is called to get
 * Device.DeviceInfo.SupportedDataModel.Features.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSupportedDataModel_Features
    (
        CCSP_STRING                 ComponentName,
        int                         ObjectID
    );

#endif
