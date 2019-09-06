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

    module:	ccsp_cwmp_proco_layout.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This wrapper file defines the configuration layout that is
        implemented by the CCSP CWMP Processor Object in PSM.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        07/28/11    initial version.
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMPPO_LAYOUT_
#define  _CCSP_CWMPPO_LAYOUT_


/***********************************************************
     CCSP CWMP PROCESSOR CONFIGURATION LAYOUT DEFINITION
***********************************************************/

/*
 * All the configuration parameters are stored in Persistent Storage Manager (PSM).
 */

/* for TransferComplete events */
#define  CCSP_CWMPPO_FOLDER_NAME_TransferComplete    "Undelivered_TC"

#define  CCSP_CWMPPO_PARAM_NAME_CommandKey           "CommandKey"
#define  CCSP_CWMPPO_PARAM_NAME_FaultCode            "FaultCode"
#define  CCSP_CWMPPO_PARAM_NAME_FaultString          "FaultString"
#define  CCSP_CWMPPO_PARAM_NAME_StartTime            "StartTime"
#define  CCSP_CWMPPO_PARAM_NAME_CompleteTime         "CompleteTime"
#define  CCSP_CWMPPO_PARAM_NAME_IsDownload           "IsDownload"

#define  CCSP_CWMPPO_PENDING_TC_MAX_COUNT            8192

/* for AutonomousTransferComplete events */
#define  CCSP_CWMPPO_FOLDER_NAME_AutonXferComplete   "Undelivered_ATC"

#define  CCSP_CWMPPO_PARAM_NAME_AnnounceURL          "AnnounceURL"
#define  CCSP_CWMPPO_PARAM_NAME_TransferURL          "TransferURL"
#define  CCSP_CWMPPO_PARAM_NAME_FileType             "FileType"
#define  CCSP_CWMPPO_PARAM_NAME_FileSize             "FileSize"
#define  CCSP_CWMPPO_PARAM_NAME_TargetFileName       "TargetFileName"

#define  CCSP_CWMPPO_PENDING_ATC_MAX_COUNT           8192

/* for DUStateChangeComplete events */
#define  CCSP_CWMPPO_FOLDER_NAME_DSCC                "Undelivered_DSCC"
/* for AutonomousDUStateChangeComplete events */
#define  CCSP_CWMPPO_FOLDER_NAME_ADSCC               "Undelivered_ADSCC"

#define  CCSP_CWMPPO_PARAM_NAME_Uuid                 "UUID"
#define  CCSP_CWMPPO_PARAM_NAME_DeploymentUnitRef    "DeploymentUnitRef"
#define  CCSP_CWMPPO_PARAM_NAME_Version              "Version"
#define  CCSP_CWMPPO_PARAM_NAME_CurrentState         "CurrentState"
#define  CCSP_CWMPPO_PARAM_NAME_Resolved             "Resolved"
#define  CCSP_CWMPPO_PARAM_NAME_ExecutionUnitRefList "ExecutionUnitRefList"
#define  CCSP_CWMPPO_PARAM_NAME_OperationPerformed   "OperationPerformed"

#define  CCSP_CWMPPO_WPMMO_PENDING_DSCC_MAX_COUNT          4096

/* for ValueChange events */
#define  CCSP_CWMPPO_FOLDER_NAME_ValueChanged        "Undelivered_VC"
#define  CCSP_CWMPPO_PARAM_NAME_ValueChanged         "ValueChanged"

#endif
