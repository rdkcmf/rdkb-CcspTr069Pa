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

    module:	ccsp_cwmp_definitions_cwmp.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This file defines the configuration parameters and message
        formats used in SOAP-based CWMP communication.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/02/05    initial revision.
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_DEFINITIONS_CWMP_
#define  _CCSP_CWMP_DEFINITIONS_CWMP_


#include "slap_definitions.h"


/***********************************************************
       BASELINE RPC ELEMENT AND PARAMETER DEFINITIONS
***********************************************************/

/*
 * The Inform message contains an argument called MaxEnvelopes that indicates to the ACS the
 * maximum number of SOAP envelopes that may be contained in a single HTTP response. The value of
 * this parameter may be one or greater. Once the Inform message has been received, any HTTP
 * response from the ACS may include at most this number of SOAP envelopes (requests or responses).
 */
#define  CCSP_CWMP_CPE_MAX_ENVELOPES                1

/*
 * A CPE MUST call the Inform method to initiate a transaction sequence whenever a connection to an
 * ACS is established. The CPE must pass in an array of EventStruct, indicating one or more events
 * that caused the transaction session to be established. If one or more causes exist, the CPE MUST
 * list all such causes.
 */
#define  CCSP_CWMP_INFORM_EVENT_BootStrap           0
#define  CCSP_CWMP_INFORM_EVENT_Boot                1
#define  CCSP_CWMP_INFORM_EVENT_Periodic            2
#define  CCSP_CWMP_INFORM_EVENT_Scheduled           3
#define  CCSP_CWMP_INFORM_EVENT_ValueChange         4
#define  CCSP_CWMP_INFORM_EVENT_Kicked              5
#define  CCSP_CWMP_INFORM_EVENT_ConnectionRequest   6
#define  CCSP_CWMP_INFORM_EVENT_TransferComplete    7
#define  CCSP_CWMP_INFORM_EVENT_DiagnosticsComplete 8

#define  CCSP_CWMP_INFORM_EVENT_CODE_BootStrap           '0'
#define  CCSP_CWMP_INFORM_EVENT_CODE_Boot                '1'
#define  CCSP_CWMP_INFORM_EVENT_CODE_Periodic            '2'
#define  CCSP_CWMP_INFORM_EVENT_CODE_Scheduled           '3'
#define  CCSP_CWMP_INFORM_EVENT_CODE_ValueChange         '4'
#define  CCSP_CWMP_INFORM_EVENT_CODE_Kicked              '5'
#define  CCSP_CWMP_INFORM_EVENT_CODE_ConnectionRequest   '6'
#define  CCSP_CWMP_INFORM_EVENT_CODE_TransferComplete    '7'
#define  CCSP_CWMP_INFORM_EVENT_CODE_DiagnosticsComplete '8'

#define  CCSP_CWMP_INFORM_EVENT_Method              100
#define  CCSP_CWMP_INFORM_EVENT_VendorSpecific      101

#define  CCSP_CWMP_SAVED_EVENTS_MAX_LEN             1024


#define	 CCSP_CWMP_INFORM_EVENT_NAME_Bootstrap		"0 BOOTSTRAP"
#define	 CCSP_CWMP_INFORM_EVENT_NAME_Boot			"1 BOOT"
#define	 CCSP_CWMP_INFORM_EVENT_NAME_Peroidic		"2 PERIODIC"
#define	 CCSP_CWMP_INFORM_EVENT_NAME_Scheduled		"3 SCHEDULED"

#define	 CCSP_CWMP_INFORM_EVENT_NAME_ValueChange	"4 VALUE CHANGE"
#define	 CCSP_CWMP_INFORM_EVENT_NAME_Kicked			"5 KICKED"
#define	 CCSP_CWMP_INFORM_EVENT_NAME_ConnectionRequest						\
													"6 CONNECTION REQUEST"
#define	 CCSP_CWMP_INFORM_EVENT_NAME_TransferComplete						\
													"7 TRANSFER COMPLETE"
#define	 CCSP_CWMP_INFORM_EVENT_NAME_DiagnosticsComplete					\
													"8 DIAGNOSTICS COMPLETE"
#define	 CCSP_CWMP_INFORM_EVENT_NAME_RequestDownload                        \
													"9 REQUEST DOWNLOAD"
#define	 CCSP_CWMP_INFORM_EVENT_NAME_AutonomousTransferComplete             \
													"10 AUTONOMOUS TRANSFER COMPLETE"
#define	 CCSP_CWMP_INFORM_EVENT_NAME_DUStateChangeComplete					\
                                                    "11 DU STATE CHANGE COMPLETE"
#define	 CCSP_CWMP_INFORM_EVENT_NAME_AutonomousDUStateChangeComplete		\
                                                    "12 AUTONOMOUS DU STATE CHANGE COMPLETE"

#define  CCSP_CWMP_INFORM_EVENT_NAME_M_Download     "M Download"                                                    
#define  CCSP_CWMP_INFORM_EVENT_NAME_M_Upload       "M Upload"                                                    
#define  CCSP_CWMP_INFORM_EVENT_NAME_M_Reboot       "M Reboot"
#define  CCSP_CWMP_INFORM_EVENT_NAME_M_ScheduleInform                       \
                                                   "M ScheduleInform"
#define  CCSP_CWMP_INFORM_EVENT_NAME_M_ChangeDUState                        \
                                                   "M ChangeDUState"
#define  CCSP_CWMP_INFORM_EVENT_NAME_M_ScheduleDownload                     \
                                                   "M ScheduleDownload"

/*
 * A fault response MUST make use of the SOAP Fault element using the following conventions:
 *
 *      - The SOAP faultcode element MUST indicate the source of the fault, either
 *        Client or Server, as appropriate for the particular fault. In this usage,
 *        Client represents the originator of the SOAP request, and Server represents
 *        the SOAP responder.
 *      - The SOAP faultstring sub-element MUST contain the string "CWMP fault".
 *      - The SOAP detail element MUST contain a Fault structure defined in the
 *        "urn:dslforum-org:cwmp-1-0" namespace.
 */
#define  CCSP_CWMP_CPE_CWMP_FaultCode_noError            0

#define  CCSP_CWMP_CPE_SOAP_faultcode_methodUnsupported  "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_methodUnsupported  "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported  9000
#define  CCSP_CWMP_CPE_CWMP_FaultText_methodUnsupported  "Method not supported"

#define  CCSP_CWMP_CPE_SOAP_faultcode_requestDenied      "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_requestDenied      "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_requestDenied      9001
#define  CCSP_CWMP_CPE_CWMP_FaultText_requestDenied      "Request denied"

#define  CCSP_CWMP_CPE_SOAP_faultcode_internalError      "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_internalError      "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_internalError      9002
#define  CCSP_CWMP_CPE_CWMP_FaultText_internalError      "Internal error"

#define  CCSP_CWMP_CPE_SOAP_faultcode_invalidArgs        "Client"
#define  CCSP_CWMP_CPE_SOAP_faulttext_invalidArgs        "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_invalidArgs        9003
#define  CCSP_CWMP_CPE_CWMP_FaultText_invalidArgs        "Invalid arguments"

#define  CCSP_CWMP_CPE_SOAP_faultcode_resources          "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_resources          "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_resources          9004
#define  CCSP_CWMP_CPE_CWMP_FaultText_resources          "Resources exceeded"

#define  CCSP_CWMP_CPE_SOAP_faultcode_invalidParamName   "Client"
#define  CCSP_CWMP_CPE_SOAP_faulttext_invalidParamName   "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_invalidParamName   9005
#define  CCSP_CWMP_CPE_CWMP_FaultText_invalidParamName   "Invalid parameter name"

#define  CCSP_CWMP_CPE_SOAP_faultcode_invalidParamType   "Client"
#define  CCSP_CWMP_CPE_SOAP_faulttext_invalidParamType   "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_invalidParamType   9006
#define  CCSP_CWMP_CPE_CWMP_FaultText_invalidParamType   "Invalid parameter type"

#define  CCSP_CWMP_CPE_SOAP_faultcode_invalidParamValue  "Client"
#define  CCSP_CWMP_CPE_SOAP_faulttext_invalidParamValue  "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_invalidParamValue  9007
#define  CCSP_CWMP_CPE_CWMP_FaultText_invalidParamValue  "Invalid parameter value"

#define  CCSP_CWMP_CPE_SOAP_faultcode_notWritable        "Client"
#define  CCSP_CWMP_CPE_SOAP_faulttext_notWritable        "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_notWritable        9008
#define  CCSP_CWMP_CPE_CWMP_FaultText_notWritable        "Attempt to set a non-writable parameter"

#define  CCSP_CWMP_CPE_SOAP_faultcode_notifyRejected     "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_notifyRejected     "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_notifyRejected     9009
#define  CCSP_CWMP_CPE_CWMP_FaultText_notifyRejected     "Notification request rejected"

#define  CCSP_CWMP_CPE_SOAP_faultcode_downloadFailure    "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_downloadFailure    "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_downloadFailure    9010
#define  CCSP_CWMP_CPE_CWMP_FaultText_downloadFailure    "Download failure"

#define  CCSP_CWMP_CPE_SOAP_faultcode_uploadFailure      "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_uploadFailure      "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_uploadFailure      9011
#define  CCSP_CWMP_CPE_CWMP_FaultText_uploadFailure      "Upload failure"

#define  CCSP_CWMP_CPE_SOAP_faultcode_authFailure        "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_authFailure        "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_authFailure        9012
#define  CCSP_CWMP_CPE_CWMP_FaultText_authFailure        "File transfer server authentication failure"

#define  CCSP_CWMP_CPE_SOAP_faultcode_protUnsupported    "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_protUnsupported    "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_protUnsupported    9013
#define  CCSP_CWMP_CPE_CWMP_FaultText_protUnsupported    "Unsupported protocol for file transfer"

#define  CCSP_CWMP_CPE_SOAP_faultcode_maxEnvExceeded     "Client"
#define  CCSP_CWMP_CPE_SOAP_faulttext_maxEnvExceeded     "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_maxEnvExceeded     9014
#define  CCSP_CWMP_CPE_CWMP_FaultText_maxEnvExceeded     "Max Envelopes exceeded"

#define  CCSP_CWMP_CPE_SOAP_faultcode_mcastJoinFailed    "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_mcastJoinFailed    "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_mcastJoinFailed    9014
#define  CCSP_CWMP_CPE_CWMP_FaultText_mcastJoinFailed    "Max Envelopes exceeded"

#define  CCSP_CWMP_CPE_SOAP_faultcode_notReachFileServer "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_notReachFileServer "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_notReachFileServer 9015
#define  CCSP_CWMP_CPE_CWMP_FaultText_notReachFileServer "Download failure: unable to contact file server"

#define  CCSP_CWMP_CPE_SOAP_faultcode_noAccessToFile     "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_noAccessToFile     "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_noAccessToFile     9016
#define  CCSP_CWMP_CPE_CWMP_FaultText_noAccessToFile     "Download failure: unable to access file"

#define  CCSP_CWMP_CPE_SOAP_faultcode_downloadIncomplete "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_downloadIncomplete "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_downloadIncomplete 9017
#define  CCSP_CWMP_CPE_CWMP_FaultText_downloadIncomplete "Download failure: unable to complete download"

#define  CCSP_CWMP_CPE_SOAP_faultcode_dlfileCorrputed    "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_dlfileCorrputed    "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_dlfileCorrputed    9018
#define  CCSP_CWMP_CPE_CWMP_FaultText_dlfileCorrputed    "Download failure: file corrupted"

#define  CCSP_CWMP_CPE_SOAP_faultcode_fileAuthFailure    "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_fileAuthFailure    "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_fileAuthFailure    9019
#define  CCSP_CWMP_CPE_CWMP_FaultText_fileAuthFailure    "Download failure: file authentication failure"

#define  CCSP_CWMP_CPE_SOAP_faultcode_dlFailureWindow    "Client"
#define  CCSP_CWMP_CPE_SOAP_faulttext_dlFailureWindow    "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_dlFailureWindow    9020
#define  CCSP_CWMP_CPE_CWMP_FaultText_dlFailureWindow    "File transfer failure: unable to complete download within specified time windows"

#define  CCSP_CWMP_CPE_SOAP_faultcode_cancelXferFailure  "Client"
#define  CCSP_CWMP_CPE_SOAP_faulttext_cancelXferFailure  "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_cancelXferFailure  9021
#define  CCSP_CWMP_CPE_CWMP_FaultText_cancelXferFailure  "Cancelation of file transfer not permitted in current transfer state"

#define  CCSP_CWMP_CPE_SOAP_faultcode_invalidUUID        "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_invalidUUID        "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_invalidUUID        9022
#define  CCSP_CWMP_CPE_CWMP_FaultText_invalidUUID        "Invalid UUID Format"

#define  CCSP_CWMP_CPE_SOAP_faultcode_UnknownExecEnv     "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_UnknownExecEnv     "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_UnknownExecEnv     9023
#define  CCSP_CWMP_CPE_CWMP_FaultText_UnknownExecEnv     "Unknown Execution Environment"

#define  CCSP_CWMP_CPE_SOAP_faultcode_ExecEnvDisabled    "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_ExecEnvDisabled    "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_ExecEnvDisabled    9024
#define  CCSP_CWMP_CPE_CWMP_FaultText_ExecEnvDisabled    "Disabled Execution Environment"

#define  CCSP_CWMP_CPE_SOAP_faultcode_DUMismatchEE       "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_DUMismatchEE       "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_DUMismatchEE       9025
#define  CCSP_CWMP_CPE_CWMP_FaultText_DUMismatchEE       "Deployment Unit to Execution Environment Mistmatch"

#define  CCSP_CWMP_CPE_SOAP_faultcode_DuplicateDU        "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_DuplicateDU        "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_DuplicateDU        9026
#define  CCSP_CWMP_CPE_CWMP_FaultText_DuplicateDU        "Duplicate Deployment Unit"

#define  CCSP_CWMP_CPE_SOAP_faultcode_NoResToInstall     "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_NoResToInstall     "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_NoResToInstall     9027
#define  CCSP_CWMP_CPE_CWMP_FaultText_NoResToInstall     "System Resources Exceeded"

#define  CCSP_CWMP_CPE_SOAP_faultcode_UnknownDU          "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_UnknownDU          "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_UnknownDU          9028
#define  CCSP_CWMP_CPE_CWMP_FaultText_UnknownDU          "Unknown Deployment Unit"

#define  CCSP_CWMP_CPE_SOAP_faultcode_InvalidDUState     "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_InvalidDUState     "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_InvalidDUState     9029
#define  CCSP_CWMP_CPE_CWMP_FaultText_InvalidDUState     "Invalid Deployment Unit State"

#define  CCSP_CWMP_CPE_SOAP_faultcode_DUNoDowngrade      "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_DUNoDowngrade      "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_DUNoDowngrade      9030
#define  CCSP_CWMP_CPE_CWMP_FaultText_DUNoDowngrade      "Invalid Deployment Unit Update - Downgrade not permitted"

#define  CCSP_CWMP_CPE_SOAP_faultcode_DUUpdateNoVersion  "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_DUUpdateNoVersion  "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_DUUpdateNoVersion  9031
#define  CCSP_CWMP_CPE_CWMP_FaultText_DUUpdateNoVersion  "Invalid Deployment Unit Update - Version not specified"

#define  CCSP_CWMP_CPE_SOAP_faultcode_DUUpdateVerExist   "Server"
#define  CCSP_CWMP_CPE_SOAP_faulttext_DUUpdateVerExist   "CWMP Fault"
#define  CCSP_CWMP_CPE_CWMP_FaultCode_DUUpdateVerExist   9032
#define  CCSP_CWMP_CPE_CWMP_FaultText_DUUpdateVerExist   "Invalid Deployment Unit Update - Version already exists"



/*
 * A fault response MUST make use of the SOAP Fault element using the following conventions:
 *
 *      - The SOAP faultcode element MUST indicate the source of the fault, either
 *        Client or Server, as appropriate for the particular fault. In this usage,
 *        Client represents the originator of the SOAP request, and Server represents
 *        the SOAP responder.
 *      - The SOAP faultstring sub-element MUST contain the string "CWMP fault".
 *      - The SOAP detail element MUST contain a Fault structure defined in the
 *        "urn:dslforum-org:cwmp-1-0" namespace.
 */
#define  CCSP_CWMP_ACS_SOAP_faultcode_methodUnsupported  "Server"
#define  CCSP_CWMP_ACS_SOAP_faulttext_methodUnsupported  "CWMP Fault"
#define  CCSP_CWMP_ACS_CWMP_FaultCode_methodUnsupported  8000
#define  CCSP_CWMP_ACS_CWMP_FaultText_methodUnsupported  "Method not supported"

#define  CCSP_CWMP_ACS_SOAP_faultcode_requestDenied      "Server"
#define  CCSP_CWMP_ACS_SOAP_faulttext_requestDenied      "CWMP Fault"
#define  CCSP_CWMP_ACS_CWMP_FaultCode_requestDenied      8001
#define  CCSP_CWMP_ACS_CWMP_FaultText_requestDenied      "Request denied"

#define  CCSP_CWMP_ACS_SOAP_faultcode_internalError      "Server"
#define  CCSP_CWMP_ACS_SOAP_faulttext_internalError      "CWMP Fault"
#define  CCSP_CWMP_ACS_CWMP_FaultCode_internalError      8002
#define  CCSP_CWMP_ACS_CWMP_FaultText_internalError      "Internal error"

#define  CCSP_CWMP_ACS_SOAP_faultcode_invalidArgs        "Client"
#define  CCSP_CWMP_ACS_SOAP_faulttext_invalidArgs        "CWMP Fault"
#define  CCSP_CWMP_ACS_CWMP_FaultCode_invalidArgs        8003
#define  CCSP_CWMP_ACS_CWMP_FaultText_invalidArgs        "Invalid arguments"

#define  CCSP_CWMP_ACS_SOAP_faultcode_resources          "Server"
#define  CCSP_CWMP_ACS_SOAP_faulttext_resources          "CWMP Fault"
#define  CCSP_CWMP_ACS_CWMP_FaultCode_resources          8004
#define  CCSP_CWMP_ACS_CWMP_FaultText_resources          "Resources exceeded"

#define  CCSP_CWMP_ACS_SOAP_faultcode_retryRequest       "Server"
#define  CCSP_CWMP_ACS_SOAP_faulttext_retryRequest       "CWMP Fault"
#define  CCSP_CWMP_ACS_CWMP_FaultCode_retryRequest       8005
#define  CCSP_CWMP_ACS_CWMP_FaultText_retryRequest       "Retry request"

#define  CCSP_CWMP_ACS_SOAP_faultcode_maxEnvExceeded     "Client"
#define  CCSP_CWMP_ACS_SOAP_faulttext_maxEnvExceeded     "CWMP Fault"
#define  CCSP_CWMP_ACS_CWMP_FaultCode_maxEnvExceeded     8006
#define  CCSP_CWMP_ACS_CWMP_FaultText_maxEnvExceeded     "Max Envelopes exceeded"


/*
 * The Notification field in the SetParameterAttributeStruct indicates whether the CPE should
 * include changed values of the specified parameter(s) in the Inform message, and whether the CPE
 * must initiate a session to the ACS when the specified parameter(s) in value.
 */
#define  CCSP_CWMP_NOTIFICATION_off                 0
#define  CCSP_CWMP_NOTIFICATION_passive             1
#define  CCSP_CWMP_NOTIFICATION_active              2

/*
 * If a Parameter name argument is given as a partial path name, the request is to be inter-
 * preted as a request to return all the Parameters in the branch of the naming hierarchy that
 * shares the same prefix as the argument.
 */
#define  CcspCwmpIsPartialName(name)                ( name[AnscSizeOfString(name) - 1] == '.' )


/***********************************************************
       BASELINE RPC ARGUMENT AND STRUCTURE DEFINITIONS
***********************************************************/

#define  CCSP_CWMP_TR069_DATA_TYPE_Unspecified           0
#define  CCSP_CWMP_TR069_DATA_TYPE_String                1
#define  CCSP_CWMP_TR069_DATA_TYPE_Int                   2
#define  CCSP_CWMP_TR069_DATA_TYPE_UnsignedInt           3
#define  CCSP_CWMP_TR069_DATA_TYPE_Boolean               4
#define  CCSP_CWMP_TR069_DATA_TYPE_DateTime              5
#define  CCSP_CWMP_TR069_DATA_TYPE_Base64                6

/*
 * On successful receipt of a SetParameterValues RPC, the CPE MUST apply the changes to each of the
 * specified Parameters immediately and atomically. The order of Parameters listed in the Parameter-
 * List has no significance - the application of value changes to the CPE MUST be independent from
 * the order in which they are listed.
 */
typedef  struct
_CCSP_CWMP_PARAM_VALUE
{
    char*                           Name;
    int                             Tr069DataType;
    SLAP_VARIABLE*                  Value;
}
CCSP_CWMP_PARAM_VALUE,  *PCCSP_CWMP_PARAM_VALUE;

#define  CcspCwmpCleanParamValue(param_value)                                               \
         {                                                                                  \
            if ( param_value->Name )                                                        \
            {                                                                               \
                AnscFreeMemory(param_value->Name);                                          \
                                                                                            \
                param_value->Name = NULL;                                                   \
            }                                                                               \
                                                                                            \
            if ( param_value->Value )                                                       \
            {                                                                               \
                SlapFreeVariable(param_value->Value);                                       \
                                                                                            \
                param_value->Value = NULL;                                                  \
            }                                                                               \
         }

#define  CcspCwmpFreeParamValue(param_value)                                                \
         {                                                                                  \
            CcspCwmpCleanParamValue(param_value);                                           \
            AnscFreeMemory         (param_value);                                           \
         }

typedef  struct
_CCSP_CWMP_PARAM_INFO
{
    char*                           Name;
    BOOL                            bWritable;
}
CCSP_CWMP_PARAM_INFO,  *PCCSP_CWMP_PARAM_INFO;

#define  CcspCwmpCleanParamInfo(param_info)                                                 \
         {                                                                                  \
            if ( param_info->Name )                                                         \
            {                                                                               \
                AnscFreeMemory(param_info->Name);                                           \
                                                                                            \
                param_info->Name = NULL;                                                    \
            }                                                                               \
         }

#define  CcspCwmpFreeParamInfo(param_info)                                                  \
         {                                                                                  \
            CcspCwmpCleanParamInfo(param_info);                                             \
            AnscFreeMemory        (param_info);                                             \
         }

typedef  struct
_CCSP_CWMP_SET_PARAM_ATTRIB
{
    char*                           Name;
    BOOL                            bNotificationChange;
    int                             Notification;
    BOOL                            bAccessListChange;
    char*                           AccessList;     /* comma separated list */
}
CCSP_CWMP_SET_PARAM_ATTRIB,  *PCCSP_CWMP_SET_PARAM_ATTRIB;

#define  CcspCwmpCleanSetParamAttrib(set_param_attrib)                                      \
         {                                                                                  \
            if ( set_param_attrib->Name )                                                   \
            {                                                                               \
                AnscFreeMemory(set_param_attrib->Name);                                     \
                                                                                            \
                set_param_attrib->Name = NULL;                                              \
            }                                                                               \
                                                                                            \
            if ( set_param_attrib->AccessList )                                             \
            {                                                                               \
                AnscFreeMemory(set_param_attrib->AccessList);                               \
                                                                                            \
                set_param_attrib->AccessList = NULL;                                        \
            }                                                                               \
         }

#define  CcspCwmpFreeSetParamAttrib(set_param_attrib)                                       \
         {                                                                                  \
            CcspCwmpCleanSetParamAttrib(set_param_attrib);                                  \
            AnscFreeMemory             (set_param_attrib);                                  \
         }

typedef  struct
_CCSP_CWMP_PARAM_ATTRIB
{
    char*                           Name;
    int                             Notification;
    char*                           AccessList;     /* comma separated list */
}
CCSP_CWMP_PARAM_ATTRIB,  *PCCSP_CWMP_PARAM_ATTRIB;

#define  CcspCwmpCleanParamAttrib(param_attrib)                                             \
         {                                                                                  \
            if ( param_attrib->Name )                                                       \
            {                                                                               \
                AnscFreeMemory(param_attrib->Name);                                         \
                                                                                            \
                param_attrib->Name = NULL;                                                  \
            }                                                                               \
                                                                                            \
            if ( param_attrib->AccessList )                                                 \
            {                                                                               \
                AnscFreeMemory(param_attrib->AccessList);                                   \
                                                                                            \
                param_attrib->AccessList = NULL;                                            \
            }                                                                               \
         }

#define  CcspCwmpFreeParamAttrib(param_attrib)                                              \
         {                                                                                  \
            CcspCwmpCleanParamAttrib(param_attrib);                                         \
            AnscFreeMemory          (param_attrib);                                         \
         }

/*
 * A CPE MUST call the Inform method to initiate a transaction sequence whenever a connection to an
 * ACS is established. When the Inform call results from a change to one or more parameter values
 * (due to a cause other than being set by the ACS itself) that the ACS has marked for notification
 * (either active or passive) via SetParameterAttributes, all of the changed parameters must also
 * be included in the ParameterList.
 */
typedef  struct
_CCSP_CWMP_DEVICE_ID
{
    char*                           Manufacturer;
    char*                           OUI;
    char*                           ProductClass;
    char*                           SerialNumber;
    char*                           ProvisioningCode;
}
CCSP_CWMP_DEVICE_ID,  *PCCSP_CWMP_DEVICE_ID;

#define  CcspCwmpCleanDeviceId(device_id)                                                   \
         {                                                                                  \
            if ( device_id->Manufacturer )                                                  \
            {                                                                               \
                AnscFreeMemory(device_id->Manufacturer);                                    \
                                                                                            \
                device_id->Manufacturer = NULL;                                             \
            }                                                                               \
            if ( device_id->ProvisioningCode )                                                  \
            {                                                                               \
                AnscFreeMemory(device_id->ProvisioningCode);                                \
                                                                                            \
                device_id->ProvisioningCode = NULL;                                             \
            }                                                                               \
            if ( device_id->OUI )                                                           \
            {                                                                               \
                AnscFreeMemory(device_id->OUI);                                             \
                                                                                            \
                device_id->OUI = NULL;                                                      \
            }                                                                               \
                                                                                            \
            if ( device_id->ProductClass )                                                  \
            {                                                                               \
                AnscFreeMemory(device_id->ProductClass);                                    \
                                                                                            \
                device_id->ProductClass = NULL;                                             \
            }                                                                               \
                                                                                            \
            if ( device_id->SerialNumber )                                                  \
            {                                                                               \
                AnscFreeMemory(device_id->SerialNumber);                                    \
                                                                                            \
                device_id->SerialNumber = NULL;                                             \
            }                                                                               \
         }

#define  CcspCwmpFreeDeviceId(device_id)                                                    \
         {                                                                                  \
            CcspCwmpCleanDeviceId(device_id);                                               \
            AnscFreeMemory       (device_id);                                               \
         }

typedef  struct
_CCSP_CWMP_EVENT
{
    char*                           EventCode;
    char*                           CommandKey;
}
CCSP_CWMP_EVENT,  *PCCSP_CWMP_EVENT;

#define  CcspCwmpCleanEvent(event)                                                          \
         {                                                                                  \
            if ( event->EventCode )                                                         \
            {                                                                               \
                AnscFreeMemory(event->EventCode);                                           \
                                                                                            \
                event->EventCode = NULL;                                                    \
            }                                                                               \
                                                                                            \
            if ( event->CommandKey )                                                        \
            {                                                                               \
                AnscFreeMemory(event->CommandKey);                                          \
                                                                                            \
                event->CommandKey = NULL;                                                   \
            }                                                                               \
         }

#define  CcspCwmpFreeEvent(event)                                                           \
         {                                                                                  \
            CcspCwmpCleanEvent(event);                                                      \
            AnscFreeMemory    (event);                                                      \
         }

typedef  struct
_CCSP_CWMP_FAULT
{
    ULONG                           FaultCode;
    char*                           FaultString;
}
CCSP_CWMP_FAULT,  *PCCSP_CWMP_FAULT;

#define  CcspCwmpCleanFault(fault)                                                          \
         {                                                                                  \
            if ( fault->FaultString )                                                       \
            {                                                                               \
                AnscFreeMemory(fault->FaultString);                                         \
                                                                                            \
                fault->FaultString = NULL;                                                  \
            }                                                                               \
         }

#define  CcspCwmpFreeFault(fault)                                                           \
         {                                                                                  \
            CcspCwmpCleanFault(fault);                                                      \
            AnscFreeMemory    (fault);                                                      \
         }

typedef  struct
_CCSP_CWMP_SET_PARAM_FAULT
{
    char*                           ParameterName;
    ULONG                           FaultCode;
    char*                           FaultString;
}
CCSP_CWMP_SET_PARAM_FAULT,  *PCCSP_CWMP_SET_PARAM_FAULT;

#define  CcspCwmpCleanSetParamFault(param_fault)                                            \
         {                                                                                  \
            if ( param_fault->ParameterName )                                               \
            {                                                                               \
                AnscFreeMemory(param_fault->ParameterName);                                 \
                                                                                            \
                param_fault->ParameterName = NULL;                                          \
            }                                                                               \
                                                                                            \
            if ( param_fault->FaultString )                                                 \
            {                                                                               \
                AnscFreeMemory(param_fault->FaultString);                                   \
                                                                                            \
                param_fault->FaultString = NULL;                                            \
            }                                                                               \
         }

#define  CcspCwmpFreeSetParamFault(param_fault)                                             \
         {                                                                                  \
            CcspCwmpCleanSetParamFault(param_fault);                                        \
            AnscFreeMemory            (param_fault);                                        \
         }

/*
 * The GetQueuedTransfers method can be called by the server to determine the status of previously
 * requested downloads or uploads. The CPE device shall return an array of QueuedTransferStruct,
 * each describing the state of one transfer that the CPE has been instructed to perform, but has
 * not yet been fully completed.
 */
typedef  struct
_CCSP_CWMP_QUEUED_TRANSFER
{
    char*                           CommandKey;
    ULONG                           State;
}
CCSP_CWMP_QUEUED_TRANSFER,  *PCCSP_CWMP_QUEUED_TRANSFER;

#define  CcspCwmpCleanQueuedTransfer(queued_transfer)                                       \
         {                                                                                  \
            if ( queued_transfer->CommandKey )                                              \
            {                                                                               \
                AnscFreeMemory(queued_transfer->CommandKey);                                \
                                                                                            \
                queued_transfer->CommandKey = NULL;                                         \
            }                                                                               \
         }

#define  CcspCwmpFreeQueuedTransfer(queued_transfer)                                        \
         {                                                                                  \
            CcspCwmpCleanQueuedTransfer(queued_transfer);                                   \
            AnscFreeMemory             (queued_transfer);                                   \
         }

typedef  struct
_CCSP_CWMP_OPTION
{
    char*                           OptionName;
    ULONG                           VoucherSN;
    ULONG                           State;
    int                             Mode;
    ANSC_UNIVERSAL_TIME             StartDate;
    ANSC_UNIVERSAL_TIME             ExpirationDate;
    BOOL                            IsTransferable;
}
CCSP_CWMP_OPTION,  *PCCSP_CWMP_OPTION;

#define  CcspCwmpCleanOption(option)                                                        \
         {                                                                                  \
            if ( option->OptionName )                                                       \
            {                                                                               \
                AnscFreeMemory(option->OptionName);                                         \
                                                                                            \
                option->OptionName = NULL;                                                  \
            }                                                                               \
         }

#define  CcspCwmpFreeOption(option)                                                         \
         {                                                                                  \
            CcspCwmpCleanOption(option);                                                    \
            AnscFreeMemory     (option);                                                    \
         }

/*
 * The RequestDownload method allows the CPE to request a file download from the Server. On recep-
 * tion of this request, the Server MAY call the Download method to initiate the download.
 */
typedef  struct
_CCSP_CWMP_FILE_TYPE
{
    char*                           Name;
    char*                           Value;
}
CCSP_CWMP_FILE_TYPE,  *PCCSP_CWMP_FILE_TYPE;

#define  CcspCwmpCleanFileType(file_type)                                                   \
         {                                                                                  \
            if ( file_type->Name )                                                          \
            {                                                                               \
                AnscFreeMemory(file_type->Name);                                            \
                                                                                            \
                file_type->Name = NULL;                                                     \
            }                                                                               \
                                                                                            \
            if ( file_type->Value )                                                         \
            {                                                                               \
                AnscFreeMemory(file_type->Value);                                           \
                                                                                            \
                file_type->Value = NULL;                                                    \
            }                                                                               \
         }

#define  CcspCwmpFreeFileType(file_type)                                                    \
         {                                                                                  \
            CcspCwmpCleanFileType(file_type);                                               \
            AnscFreeMemory       (file_type);                                               \
         }


/***********************************************************
    SOAP-BASED MESSAGE ELEMENT AND PARAMETER DEFINITIONS
***********************************************************/

/*
 * In the CPE WAN Management Protocol, a remote procedure call mechanism is used for bi-directional
 * communication between a CPE device and an Auto-configuration Server (ACS). It is assumed that
 * the lower layers that transport RPC messages provide most aspects of security, including mutual
 * authentication between the CPE and ACS, confidentiality, and data integrity.
 */
#define  CCSP_CWMP_METHOD_Unknown                   0
#define  CCSP_CWMP_METHOD_GetRPCMethods             1
#define  CCSP_CWMP_METHOD_SetParameterValues        2
#define  CCSP_CWMP_METHOD_GetParameterValues        3
#define  CCSP_CWMP_METHOD_GetParameterNames         4
#define  CCSP_CWMP_METHOD_SetParameterAttributes    5
#define  CCSP_CWMP_METHOD_GetParameterAttributes    6
#define  CCSP_CWMP_METHOD_AddObject                 7
#define  CCSP_CWMP_METHOD_DeleteObject              8
#define  CCSP_CWMP_METHOD_Reboot                    9
#define  CCSP_CWMP_METHOD_Download                  10
#define  CCSP_CWMP_METHOD_Upload                    11
#define  CCSP_CWMP_METHOD_FactoryReset              12
#define  CCSP_CWMP_METHOD_GetQueuedTransfers        13
#define  CCSP_CWMP_METHOD_ScheduleInform            14
#define  CCSP_CWMP_METHOD_SetVouchers               15
#define  CCSP_CWMP_METHOD_GetOptions                16
#define  CCSP_CWMP_METHOD_ChangeDUState             17

#define  CCSP_CWMP_METHOD_Inform                            101
#define  CCSP_CWMP_METHOD_TransferComplete                  102
#define  CCSP_CWMP_METHOD_RequestDownload                   103
#define  CCSP_CWMP_METHOD_Kicked                            104
#define  CCSP_CWMP_METHOD_AutonomousTransferComplete        105
#define  CCSP_CWMP_METHOD_DUStateChangeComplete             106
#define  CCSP_CWMP_METHOD_AutonomousDUStateChangeComplete   107

/*
 * In each direction, the order of SOAP envelopes is defined independently from the number of
 * envelopes carried per HTTP post/response pair. Specifically, envelopes are ordered from first to
 * last within a single HTTP post/response and then between successive post/response pairs. That is,
 * the succession of envelopes within each HTTP post/response and then between successive posts or
 * responses can be thought of as a single ordered sequence of envelopes.
 */
#define  CCSP_CWMP_SOAP_HEADER_ID                   0x00000001
#define  CCSP_CWMP_SOAP_HEADER_HoldRequests         0x00000002
#define  CCSP_CWMP_SOAP_HEADER_NoMoreRequests       0x00000004

typedef  struct
_CCSP_CWMP_SOAP_HEADER
{
    ULONG                           ElementMask;
    char*                           ID;
    BOOL                            bHoldRequests;
    BOOL                            bNoMoreRequests;
}
CCSP_CWMP_SOAP_HEADER,  *PCCSP_CWMP_SOAP_HEADER;

#define  CcspCwmpCleanSoapHeader(soap_header)                                               \
         {                                                                                  \
            if ( soap_header->ID )                                                          \
            {                                                                               \
                AnscFreeMemory(soap_header->ID);                                            \
                                                                                            \
                soap_header->ID = NULL;                                                     \
            }                                                                               \
         }

#define  CcspCwmpFreeSoapHeader(soap_header)                                                \
         {                                                                                  \
            CcspCwmpCleanSoapHeader(soap_header);                                           \
            AnscFreeMemory         (soap_header);                                           \
         }

typedef  struct
_CCSP_CWMP_SOAP_REQUEST
{
    CCSP_CWMP_SOAP_HEADER           Header;
    ULONG                           Method;
    ANSC_HANDLE                     hReqArguments;
}
CCSP_CWMP_SOAP_REQUEST,  *PCCSP_CWMP_SOAP_REQUEST;

#define  CcspCwmpCleanSoapReq(soap_req)                                                     \
         {                                                                                  \
            CcspCwmpCleanSoapHeader((&soap_req->Header));                                   \
         }

#define  CcspCwmpFreeSoapReq(soap_req)                                                      \
         {                                                                                  \
            CcspCwmpCleanSoapReq(soap_req);                                                 \
            AnscFreeMemory      (soap_req);                                                 \
         }

typedef  struct
_CCSP_CWMP_SOAP_FAULT
{
    char*                           soap_faultcode;
    char*                           soap_faultstring;
    CCSP_CWMP_FAULT                 Fault;
    ULONG                           SetParamValuesFaultCount;
    CCSP_CWMP_SET_PARAM_FAULT       SetParamValuesFaultArray[ANSC_ZERO_ARRAY_SIZE];
}
CCSP_CWMP_SOAP_FAULT,  *PCCSP_CWMP_SOAP_FAULT;

#define  CcspCwmpCleanSoapFault(soap_fault)                                                 \
         {                                                                                  \
            ULONG                   ii = 0;                                                 \
                                                                                            \
            if ( soap_fault->soap_faultcode )                                               \
            {                                                                               \
                AnscFreeMemory(soap_fault->soap_faultcode);                                 \
                                                                                            \
                soap_fault->soap_faultcode = NULL;                                          \
            }                                                                               \
                                                                                            \
            if ( soap_fault->soap_faultstring )                                             \
            {                                                                               \
                AnscFreeMemory(soap_fault->soap_faultstring);                               \
                                                                                            \
                soap_fault->soap_faultstring = NULL;                                        \
            }                                                                               \
                                                                                            \
            CcspCwmpCleanFault((&soap_fault->Fault));                                       \
                                                                                            \
            for ( ii = 0; ii < soap_fault->SetParamValuesFaultCount; ii++ )                 \
            {                                                                               \
                CcspCwmpCleanSetParamFault((&soap_fault->SetParamValuesFaultArray[ii]));    \
            }                                                                               \
         }

#define  CcspCwmpFreeSoapFault(soap_fault)                                                  \
         {                                                                                  \
            CcspCwmpCleanSoapFault(soap_fault);                                             \
            AnscFreeMemory        (soap_fault);                                             \
         }

typedef  struct
_CCSP_CWMP_SOAP_RESPONSE
{
    CCSP_CWMP_SOAP_HEADER           Header;
    ULONG                           Method;
    ANSC_HANDLE                     hRepArguments;
    CCSP_CWMP_SOAP_FAULT*           Fault;
}
CCSP_CWMP_SOAP_RESPONSE,  *PCCSP_CWMP_SOAP_RESPONSE;

#define  CcspCwmpCleanSoapResponse(soap_rep)                                                \
         {                                                                                  \
            CcspCwmpCleanSoapHeader((&soap_rep->Header));                                   \
                                                                                            \
            if ( soap_rep->Fault )                                                          \
            {                                                                               \
                CcspCwmpFreeSoapFault(soap_rep->Fault);                                     \
                                                                                            \
                soap_rep->Fault = NULL;                                                     \
            }                                                                               \
                                                                                            \
            if ( soap_rep->hRepArguments )                                                  \
            {                                                                               \
                if ( soap_rep->Method == CCSP_CWMP_METHOD_GetRPCMethods )                   \
                {                                                                           \
                    SlapFreeVarArray(((SLAP_STRING_ARRAY*)soap_rep->hRepArguments));        \
                                                                                            \
                    soap_rep->hRepArguments = (ANSC_HANDLE)NULL;                            \
                }                                                                           \
                else if ( soap_rep->Method == CCSP_CWMP_METHOD_Kicked )                     \
                {                                                                           \
                    AnscFreeMemory((char*)soap_rep->hRepArguments);                         \
                                                                                            \
                    soap_rep->hRepArguments = (ANSC_HANDLE)NULL;                            \
                }                                                                           \
            }                                                                               \
         }

#define  CcspCwmpFreeSoapResponse(soap_rep)                                                 \
         {                                                                                  \
            CcspCwmpCleanSoapResponse(soap_rep);                                            \
            AnscFreeMemory           (soap_rep);                                            \
         }

/*
 * The RPC methods defined in TR-069 make use of a limited subset of the default SOAP data types.
 * They also make use of structures and arrays (in some cases containing mixed types). Array ele-
 * ments are indicated with square brackets after the data type. If specified, the maximum length
 * of the array would be indicated within the brackets.
 */

#define  CCSP_CWMP_DSC_OP_NAME_Install              "Install"
#define  CCSP_CWMP_DSC_OP_NAME_Update               "Update"
#define  CCSP_CWMP_DSC_OP_NAME_Uninstall            "Uninstall"

#endif
