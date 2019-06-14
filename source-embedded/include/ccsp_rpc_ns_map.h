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

    module: ccsp_rpc_ns_map.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This file defines RPC to CCSP internal namespace
        mapping.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        07/12/2011  initial revision.

**********************************************************************/

#ifndef  _CCSP_RPC_NS_MAPPER_DEF_
#define  _CCSP_RPC_NS_MAPPER_DEF_

/*
 * RPC name/argument to CCSP namespace mappings
 *
 * Since it's not easy to come up with a unified mapping schema,
 * hard-code the mapping is the easist solution for once functional
 * specifications are finalized, they are not expected to change
 * often. Another reason for this, there is currently no rule
 * how to map a RPC into CCSP namespaces, a straight-forward 
 * mapping schema is hard to define. In another word, it's not
 * easy to do this through TR-069 PA mapper file.
 */

/**********************************************************************
    RPC 'FactoryReset'
**********************************************************************/
//#define  CCSP_NS_FACTORYRESET                       "com.cisco.spvtg.ccsp.command.FactoryReset"
#define  CCSP_NS_FACTORYRESET                       "Device.X_CISCO_COM_DeviceControl.FactoryReset"


/**********************************************************************
    RPC 'Reboot'
**********************************************************************/
//#define  CCSP_NS_REBOOT                             "com.cisco.spvtg.ccsp.rm.Reboot.Enable"
#define  CCSP_NS_REBOOT                   "Device.X_CISCO_COM_DeviceControl.RebootDevice"

/**********************************************************************
    RPC 'RebootReason'
**********************************************************************/
#define  CCSP_NS_REBOOT_REASON             "Device.DeviceInfo.X_RDKCENTRAL-COM_LastRebootReason"

/**********************************************************************
    RPC 'Download'
**********************************************************************/
#define  CCSP_NS_DOWNLOAD                           "com.cisco.spvtg.ccsp.fu.Configuration."

#define  CCSP_NS_DOWNLOAD_COMMAND_KEY               "CommandKey"
#define  CCSP_NS_DOWNLOAD_FILE_TYPE                 "FileType"
#define  CCSP_NS_DOWNLOAD_URL                       "URL"
#define  CCSP_NS_DOWNLOAD_USERNAME                  "UserName"
#define  CCSP_NS_DOWNLOAD_PASSWORD                  "Password"
#define  CCSP_NS_DOWNLOAD_FILE_SIZE                 "FileSize"
#define  CCSP_NS_DOWNLOAD_TARGET_FILE_NAME          "TargetFileName"
#define  CCSP_NS_DOWNLOAD_DELAY_SECONDS             "DelaySeconds"
#define  CCSP_NS_DOWNLOAD_SUCCESS_URL               "SuccessURL"
#define  CCSP_NS_DOWNLOAD_FAILURE_URL               "FailureURL"

#define  CCSP_NS_DOWNLOAD_START_TIME                "StartTime"
#define  CCSP_NS_DOWNLOAD_COMPLETE_TIME             "CompleteTime"
#define  CCSP_NS_DOWNLOAD_ERROR                     "ErrorCode"

#define  CCSP_NS_DOWNLOAD_STATE                     "State"

#define  CCSP_NS_DOWNLOAD_ARG_MAX_COUNT             10
#define  CCSP_NS_DOWNLOAD_RESULT_ARG_COUNT          4

/**********************************************************************
    RPC 'Upload'
**********************************************************************/
#define  CCSP_NS_UPLOAD                             "CCSP.COMMAND.FILE.UPLOADER."

#define  CCSP_NS_UPLOAD_COMMAND_KEY                 "ID"
#define  CCSP_NS_UPLOAD_FILE_TYPE                   "FileType"
#define  CCSP_NS_UPLOAD_URL                         "URL"
#define  CCSP_NS_UPLOAD_USERNAME                    "Username"
#define  CCSP_NS_UPLOAD_PASSWORD                    "Password"

#define  CCSP_NS_UPLOAD_START_TIME                  "StartTime"
#define  CCSP_NS_UPLOAD_COMPLETE_TIME               "CompleteTime"
#define  CCSP_NS_UPLOAD_STATUS                      "Status"


/**********************************************************************
    RPC 'ChangeDUState'
**********************************************************************/
#define  CCSP_NS_CHANGEDUSTATE                      "CCSP.COMMAND.PACKAGE.MANAGEMENT."

#define  CCSP_NS_CDS_ID                             "ID"
#define  CCSP_NS_CDS_OPERATION                      "Operation"
#define  CCSP_NS_CDS_OPERATION_STATE                "OperationState"
#define  CCSP_NS_CDS_URL                            "URL"
#define  CCSP_NS_CDS_UUID                           "UUID"
#define  CCSP_NS_CDS_Username                       "Username"
#define  CCSP_NS_CDS_Password                       "Password"
#define  CCSP_NS_CDS_EXECUTION_ENV_REF              "ExecutionEnvRef"
#define  CCSP_NS_CDS_VERSION                        "Version"
#define  CCSP_NS_CDS_DEPLOYMENT_UNIT_REF            "DeploymentUnitRef"
#define  CCSP_NS_CDS_CURRENT_STATE                  "CurrentState"
#define  CCSP_NS_CDS_RESOLVED                       "Resolved"
#define  CCSP_NS_CDS_EXECUTION_UNIT_REF_LIST        "ExecutionUnitRefList"
#define  CCSP_NS_CDS_START_TIME                     "StartTime"
#define  CCSP_NS_CDS_COMPLETE_TIME                  "CompleteTime"
#define  CCSP_NS_CDS_ERROR                          "Error"

#define  CCSP_NS_CDS_OPERATION_ARG_MAX_COUNT        10
#define  CCSP_NS_CDS_RESULT_ARG_COUNT               9
#define  CCSP_NS_ACDS_RESULT_ARG_COUNT              10

#define  CCSP_NS_CDS_OPERATION_Install              "Install"
#define  CCSP_NS_CDS_OPERATION_Update               "Update"
#define  CCSP_NS_CDS_OPERATION_Remove               "Remove"

#define  CCSP_NS_CDS_OPERATION_STATE_None           "None"
#define  CCSP_NS_CDS_OPERATION_STATE_Requested      "Requested"
#define  CCSP_NS_CDS_OPERATION_STATE_Complete       "Complete"
#define  CCSP_NS_CDS_OPERATION_STATE_Error          "Error"

/* PSM related namespace */
#define  CCSP_NS_CDS_PSM_NODE_COMPLETE              "Complete"
#define  CCSP_NS_CDS_PSM_NODE_ORDER                 "Order"

#endif


