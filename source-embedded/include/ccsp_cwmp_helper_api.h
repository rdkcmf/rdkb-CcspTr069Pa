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

    module:	ccsp_cwmp_helper_api.h

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This file defines helper APIs that faciliates CWMP
        migration to CCSP new architecture.

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

#ifndef  _CCSP_CWMP_HELPER_API_DEF_
#define  _CCSP_CWMP_HELPER_API_DEF_

#include "ansc_time.h"
#include "ccsp_base_api.h"
#include "dslh_definitions_database.h"
#include "ccsp_cwmp_definitions_cwmp.h"
#include "ccsp_tr069pa_wrapper_api.h"

#define  CCSP_TR069PA_WRITE_ID                      DSLH_MPA_ACCESS_CONTROL_ACS

#define  CCSP_NS_ACCESS_ACSONLY                     0x00000000
#define  CCSP_NS_ACCESS_SUBSCRIBER                  0xFFFFFFFF

#define  CCSP_TR069PA_PARAM_NOTIF_ATTR              "PmNotif"


__inline static
void 
CCSP_CWMP_SET_SOAP_FAULT
    (
        PCCSP_CWMP_SOAP_FAULT       pCwmpSoapFault, 
        ULONG                       faultCode
    )
{
    switch (faultCode)
    {
        case    CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_methodUnsupported);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_methodUnsupported);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_methodUnsupported;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_methodUnsupported);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_requestDenied:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_requestDenied);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_requestDenied);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_requestDenied;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_requestDenied);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_internalError:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_internalError);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_internalError);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_internalError;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_internalError);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_invalidArgs:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_invalidArgs);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_invalidArgs);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_invalidArgs;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_invalidArgs);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_resources:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_resources);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_resources);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_resources;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_resources);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_invalidParamName:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_invalidParamName);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_invalidParamName);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_invalidParamName;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_invalidParamName);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_invalidParamType:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_invalidParamType);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_invalidParamType);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_invalidParamType;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_invalidParamType);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_invalidParamValue:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_invalidParamValue);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_invalidParamValue);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_invalidParamValue;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_invalidParamValue);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_notWritable:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_notWritable);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_notWritable);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_notWritable;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_notWritable);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_notifyRejected:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_notifyRejected);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_notifyRejected);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_notifyRejected;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_notifyRejected);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_downloadFailure:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_downloadFailure);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_downloadFailure);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_downloadFailure;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_downloadFailure);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_uploadFailure:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_uploadFailure);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_uploadFailure);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_uploadFailure;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_uploadFailure);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_authFailure:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_authFailure);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_authFailure);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_authFailure;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_authFailure);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_protUnsupported:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_protUnsupported);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_protUnsupported);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_protUnsupported;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_protUnsupported);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_mcastJoinFailed:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_mcastJoinFailed);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_mcastJoinFailed);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_mcastJoinFailed;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_mcastJoinFailed);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_notReachFileServer:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_notReachFileServer);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_notReachFileServer);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_notReachFileServer;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_notReachFileServer);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_noAccessToFile:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_noAccessToFile);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_noAccessToFile);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_noAccessToFile;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_noAccessToFile);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_downloadIncomplete:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_downloadIncomplete);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_downloadIncomplete);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_downloadIncomplete;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_downloadIncomplete);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_dlfileCorrputed:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_dlfileCorrputed);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_dlfileCorrputed);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_dlfileCorrputed;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_dlfileCorrputed);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_fileAuthFailure:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_fileAuthFailure);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_fileAuthFailure);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_fileAuthFailure;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_fileAuthFailure);

                break;


        case    CCSP_CWMP_CPE_CWMP_FaultCode_dlFailureWindow:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_dlFailureWindow);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_dlFailureWindow);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_dlFailureWindow;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_dlFailureWindow);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_cancelXferFailure:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_cancelXferFailure);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_cancelXferFailure);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_cancelXferFailure;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_cancelXferFailure);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_invalidUUID:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_invalidUUID);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_invalidUUID);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_invalidUUID;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_invalidUUID);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_UnknownExecEnv:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_UnknownExecEnv);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_UnknownExecEnv);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_UnknownExecEnv;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_UnknownExecEnv);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_ExecEnvDisabled:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_ExecEnvDisabled);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_ExecEnvDisabled);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_ExecEnvDisabled;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_ExecEnvDisabled);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_DUMismatchEE:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_DUMismatchEE);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_DUMismatchEE);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_DUMismatchEE;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_DUMismatchEE);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_DuplicateDU:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_DuplicateDU);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_DuplicateDU);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_DuplicateDU;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_DuplicateDU);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_NoResToInstall:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_NoResToInstall);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_NoResToInstall);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_NoResToInstall;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_NoResToInstall);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_UnknownDU:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_UnknownDU);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_UnknownDU);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_UnknownDU;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_UnknownDU);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_InvalidDUState:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_InvalidDUState);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_InvalidDUState);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_InvalidDUState;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_InvalidDUState);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_DUNoDowngrade:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_DUNoDowngrade);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_DUNoDowngrade);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_DUNoDowngrade;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_DUNoDowngrade);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_DUUpdateNoVersion:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_DUUpdateNoVersion);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_DUUpdateNoVersion);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_DUUpdateNoVersion;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_DUUpdateNoVersion);

                break;

        case    CCSP_CWMP_CPE_CWMP_FaultCode_DUUpdateVerExist:

                pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_DUUpdateVerExist);
                pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_DUUpdateVerExist);
                pCwmpSoapFault->Fault.FaultCode     = CCSP_CWMP_CPE_CWMP_FaultCode_DUUpdateVerExist;
                pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(CCSP_CWMP_CPE_CWMP_FaultText_DUUpdateVerExist);

                break;

        default:

                CcspTr069PaTraceWarning(("Faultcode %u not caught\n", (unsigned int)faultCode));
                if ( TRUE )
                {
                    char            buf[32];

                    _ansc_sprintf(buf, "Falult Code: %u", (unsigned int)faultCode);

                    pCwmpSoapFault->soap_faultcode      = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faultcode_internalError);
                    pCwmpSoapFault->soap_faultstring    = CcspTr069PaCloneString(CCSP_CWMP_CPE_SOAP_faulttext_internalError);
                    pCwmpSoapFault->Fault.FaultCode     = faultCode;
                    pCwmpSoapFault->Fault.FaultString   = CcspTr069PaCloneString(buf);
                }

                break;
    }
}


__inline static
void 
CcspTr069FreeStringArray
    (
        char**                      pStringArray,
        ULONG                       ulArraySize,
        BOOL                        bFreeArrayAsWell
    )
{
    ULONG                           i;

    if ( !pStringArray )
    {
        return;
    }

    for ( i = 0; i < ulArraySize; i ++ )
    {
        if ( pStringArray[i] )
        {
            CcspTr069PaFreeMemory(pStringArray[i]);
        }
    }

    if ( bFreeArrayAsWell )
    {
        CcspTr069PaFreeMemory(pStringArray);
    }
}


__inline static
int
CcspTr069SearchStringArray
    (
        char**                      pStringArray,
        ULONG                       ulArraySize,
        char*                       pSearchString,
        BOOL                        bCaseSensitive
    )
{
    ULONG                           i;

    if ( !pStringArray )
    {
        return -1;
    }

    for ( i = 0; i < ulArraySize; i ++ )
    {
        if ( pStringArray[i] && AnscEqualString(pSearchString, pStringArray[i], bCaseSensitive) )
        {
            return  i;
        }
    }

    return -1;
}

// String list/queue definitions
typedef
struct
_CCSP_TR069PA_STRING_SLIST_ENTRY
{
    SINGLE_LINK_ENTRY               Linkage;
    char*                           Value;
}
CCSP_TR069PA_STRING_SLIST_ENTRY, *PCCSP_TR069PA_STRING_SLIST_ENTRY;

#define ACCESS_CCSP_TR069PA_STRING_SLIST_ENTRY(p) ACCESS_CONTAINER(p, CCSP_TR069PA_STRING_SLIST_ENTRY, Linkage)

typedef
struct
_CCSP_TR069PA_PARAM_ATTR_SLIST_ENTRY
{
    SINGLE_LINK_ENTRY               Linkage;
    PCCSP_CWMP_SET_PARAM_ATTRIB     ParamAttr;
}
CCSP_TR069PA_PARAM_ATTR_SLIST_ENTRY, *PCCSP_TR069PA_PARAM_ATTR_SLIST_ENTRY;

#define ACCESS_CCSP_TR069PA_PARAM_ATTR_SLIST_ENTRY(p) ACCESS_CONTAINER(p, CCSP_TR069PA_PARAM_ATTR_SLIST_ENTRY, Linkage)

inline static
void
CcspTr069FreeStringQueue(PQUEUE_HEADER pQueueHeader, BOOL bFreeValue)
{
    PSINGLE_LINK_ENTRY              pLink;
    PCCSP_TR069PA_STRING_SLIST_ENTRY pSListEntry;

    while ( (pLink = AnscQueuePopEntry(pQueueHeader)))
    {
        pSListEntry = ACCESS_CCSP_TR069PA_STRING_SLIST_ENTRY(pLink);

        if ( bFreeValue )
        {
            CcspTr069PaFreeMemory(pSListEntry->Value);
        }

        CcspTr069PaFreeMemory(pSListEntry);
    }
}

inline static
void
CcspTr069FreeParamAttrQueue(PQUEUE_HEADER pQueueHeader)
{
    PSINGLE_LINK_ENTRY              pLink;
    PCCSP_TR069PA_PARAM_ATTR_SLIST_ENTRY pSListEntry;

    while ( (pLink = AnscQueuePopEntry(pQueueHeader)))
    {
        pSListEntry = ACCESS_CCSP_TR069PA_PARAM_ATTR_SLIST_ENTRY(pLink);

        CcspTr069PaFreeMemory(pSListEntry);
    }
}

inline static
void
CcspTr069FreeStringSList(PSLIST_HEADER pSListHeader, BOOL bFreeValue)
{
    PSINGLE_LINK_ENTRY              pLink;
    PCCSP_TR069PA_STRING_SLIST_ENTRY pSListEntry;

    while ( (pLink = AnscSListPopEntry(pSListHeader)) )
    {
        pSListEntry = ACCESS_CCSP_TR069PA_STRING_SLIST_ENTRY(pLink);

        if ( pSListEntry )
        {
            if ( bFreeValue )
            {
                CcspTr069PaFreeMemory(pSListEntry->Value);
            }

            CcspTr069PaFreeMemory(pSListEntry);
        }
    }
}

/*
 * The following data structures are defined to cover normalized
 * actions - GPN, GPA, SPA, GPV, SPV. They are used as intermediate
 * data structures in order for PA to dispatch calls to all matching
 * FCs and aggregate results from FCs accordingly.
 */
#define  CCSP_NORMALIZED_ACTION_TYPE_GPN            1
#define  CCSP_NORMALIZED_ACTION_TYPE_SPV            2
#define  CCSP_NORMALIZED_ACTION_TYPE_GPV            3
#define  CCSP_NORMALIZED_ACTION_TYPE_SPA            4
#define  CCSP_NORMALIZED_ACTION_TYPE_GPA            5

typedef  parameterInfoStruct_t                      CCSP_PARAM_NAME_INFO, *PCCSP_PARAM_NAME_INFO;
typedef  parameterValStruct_t                       CCSP_PARAM_VALUE_INFO, *PCCSP_PARAM_VALUE_INFO;
typedef  parameterAttributeStruct_t                 CCSP_PARAM_ATTR_INFO, *PCCSP_PARAM_ATTR_INFO;

typedef  union
_CCSP_TR069_NA_ARGS
{
    CCSP_PARAM_NAME_INFO            paramInfo;
    CCSP_PARAM_VALUE_INFO           paramValueInfo;
    CCSP_PARAM_ATTR_INFO            paramAttrInfo;
}
CCSP_TR069_NA_ARGS, *PCCSP_TR069_NA_ARGS;

typedef  struct
_CCSP_TR069PA_NSLIST
{
    SINGLE_LINK_ENTRY               Linkage;
    int                             NaType;             /* normalized action type - one of CCSP_NORMALIZED_ACTION_TYPE_xxx */
    CCSP_TR069_NA_ARGS              Args;               /* input or output arguments */
}
CCSP_TR069PA_NSLIST, *PCCSP_TR069PA_NSLIST;

#define  ACCESS_CCSP_TR069PA_NSLIST(p)                                                                  \
         ACCESS_CONTAINER(p, CCSP_TR069PA_NSLIST, Linkage)

/* API CcspTr069PaIsGpnNsInQueue checks if a given namespace returned in GPN response
 * has already been added, meaning another FC returns the same namespace. PA checks duplicate
 * in case FC (or CCSP FC supporting library) does not handle this case well or easily.
 * This feature can be turned off if there's no such problems in future.
 */
#ifndef  _NO_CCSP_TR069PA_GPN_RESULT_FILTERING
__inline static BOOL 
CcspTr069PaIsGpnNsInQueue
    (
        PQUEUE_HEADER               pNsListQueue,
        char*                       pParamName
    )
{
    PSINGLE_LINK_ENTRY              pSLinkEntry;
    PCCSP_TR069PA_NSLIST            pNsList;
    PCCSP_PARAM_NAME_INFO           pParamInfo;

    if ( !pParamName || *pParamName == 0 ) return TRUE;   
 
    pSLinkEntry = AnscQueueGetFirstEntry(pNsListQueue);          
    while ( pSLinkEntry )
    {
        pNsList = ACCESS_CCSP_TR069PA_NSLIST(pSLinkEntry);
        pSLinkEntry = AnscQueueGetNextEntry(pSLinkEntry);

        pParamInfo = &pNsList->Args.paramInfo;

        if ( AnscEqualString(pParamName, pParamInfo->parameterName, TRUE) )
        {
            return TRUE;
        }
    }

    return FALSE;
}
#endif


#define  CcspTr069PaPushGpvNsInQueue(pNsListQueue, pParamName, pParamValue, ccspType, pNsList)          \
    do {                                                                                                \
        PCCSP_PARAM_VALUE_INFO   pParamValueInfo;                                                       \
        pNsList =                                                                                       \
            (PCCSP_TR069PA_NSLIST)CcspTr069PaAllocateMemory                                             \
                (sizeof(CCSP_TR069PA_NSLIST));                                                          \
                                                                                                        \
        if ( pNsList )                                                                                  \
        {                                                                                               \
            pNsList->NaType  = CCSP_NORMALIZED_ACTION_TYPE_GPV;                                         \
            pParamValueInfo  = &pNsList->Args.paramValueInfo;                                           \
                                                                                                        \
            pParamValueInfo->parameterName  = pParamName;                                               \
            pParamValueInfo->parameterValue = pParamValue;                                              \
            pParamValueInfo->type           = ccspType;                                                 \
                                                                                                        \
            AnscQueuePushEntry(pNsListQueue, &pNsList->Linkage);                                        \
        }                                                                                               \
    } while (0)


#define  CcspTr069PaPushGpnNsInQueue(pNsListQueue, pParamName, bWritable, pNsList)                      \
    do {                                                                                                \
        PCCSP_PARAM_NAME_INFO    pParamNameInfo;                                                        \
        pNsList =                                                                                       \
            (PCCSP_TR069PA_NSLIST)CcspTr069PaAllocateMemory                                             \
                (sizeof(CCSP_TR069PA_NSLIST));                                                          \
                                                                                                        \
        if ( pNsList )                                                                                  \
        {                                                                                               \
            pNsList->NaType = CCSP_NORMALIZED_ACTION_TYPE_GPN;                                          \
            pParamNameInfo  = &pNsList->Args.paramInfo;                                                 \
                                                                                                        \
            pParamNameInfo->parameterName = pParamName;                                                 \
            pParamNameInfo->writable      = bWritable;                                                  \
                                                                                                        \
            AnscQueuePushEntry(pNsListQueue, &pNsList->Linkage);                                        \
        }                                                                                               \
    } while (0)


#define  CcspTr069PaPushGpaNsInQueue(pNsListQueue, pParamName, notif, accessControl, pNsList)           \
    do {                                                                                                \
        PCCSP_PARAM_ATTR_INFO    pParamAttrInfo;                                                        \
        pNsList =                                                                                       \
            (PCCSP_TR069PA_NSLIST)CcspTr069PaAllocateMemory                                             \
                (sizeof(CCSP_TR069PA_NSLIST));                                                          \
                                                                                                        \
        if ( pNsList )                                                                                  \
        {                                                                                               \
            pNsList->NaType  = CCSP_NORMALIZED_ACTION_TYPE_GPA;                                         \
            pParamAttrInfo   = &pNsList->Args.paramAttrInfo;                                            \
                                                                                                        \
            pParamAttrInfo->parameterName        = pParamName;                                          \
            pParamAttrInfo->notification         = notif;                                               \
            pParamAttrInfo->accessControlBitmask = accessControl;                                       \
                                                                                                        \
            AnscQueuePushEntry(pNsListQueue, &pNsList->Linkage);                                        \
        }                                                                                               \
    } while (0)


#define  CcspTr069PaFreeNsList(pNsList)                                                                 \
    do {                                                                                                \
            switch ( (pNsList)->NaType )                                                                \
            {                                                                                           \
                case    CCSP_NORMALIZED_ACTION_TYPE_GPN:                                                \
                                                                                                        \
                        {                                                                               \
                            CCSP_PARAM_NAME_INFO* pInfo;                                                \
                            pInfo = &(pNsList)->Args.paramInfo;                                         \
                            if ( pInfo->parameterName )                                                 \
                                CcspTr069PaFreeMemory(pInfo->parameterName);                            \
                        }                                                                               \
                                                                                                        \
                        break;                                                                          \
            }                                                                                           \
            CcspTr069PaFreeMemory(pNsList);                                                             \
    } while (0)


typedef  struct
_CCSP_TR069PA_FC_NSLIST
{
    SINGLE_LINK_ENTRY               Linkage;
    char*                           Subsystem;          /* Subsystem name */
    char*                           FCName;             /* FC name */
    char*                           DBusPath;           /* FC DBus path */
    QUEUE_HEADER                    NsList;             /* namespace list, varied from action to action */
}
CCSP_TR069PA_FC_NSLIST, *PCCSP_TR069PA_FC_NSLIST;

#define  ACCESS_CCSP_TR069PA_FC_NSLIST(p)                                                               \
         ACCESS_CONTAINER(p, CCSP_TR069PA_FC_NSLIST, Linkage)

#define  CcspTr069PaFreeFcNsList(pFcNsList)                                                             \
    do {                                                                                                \
        PSINGLE_LINK_ENTRY          pSLinkEntry;                                                        \
        PCCSP_TR069PA_NSLIST        pNsList;                                                            \
                                                                                                        \
        if ( (pFcNsList)->Subsystem )                                                                   \
        {                                                                                               \
            CcspTr069PaFreeMemory((pFcNsList)->Subsystem);                                              \
        }                                                                                               \
                                                                                                        \
        if ( (pFcNsList)->FCName )                                                                      \
        {                                                                                               \
            CcspTr069PaFreeMemory((pFcNsList)->FCName);                                                 \
        }                                                                                               \
                                                                                                        \
        if ( (pFcNsList)->DBusPath )                                                                    \
        {                                                                                               \
            CcspTr069PaFreeMemory((pFcNsList)->DBusPath);                                               \
        }                                                                                               \
                                                                                                        \
        pSLinkEntry = AnscQueuePopEntry((&((pFcNsList)->NsList)));                                      \
        while ( pSLinkEntry )                                                                           \
        {                                                                                               \
            pNsList = ACCESS_CCSP_TR069PA_NSLIST(pSLinkEntry);                                          \
            pSLinkEntry = AnscQueuePopEntry((&((pFcNsList)->NsList)));                                  \
                                                                                                        \
            CcspTr069PaFreeNsList(pNsList);                                                             \
        }                                                                                               \
        CcspTr069PaFreeMemory(pFcNsList);                                                               \
    } while (0)


#define  CcspTr069PaFreeAllFcNsList(pFcNsListQueue)                                                     \
    do {                                                                                                \
        PSINGLE_LINK_ENTRY          pSLinkEntry;                                                        \
        PCCSP_TR069PA_FC_NSLIST     pFcNsList;                                                          \
                                                                                                        \
        pSLinkEntry = AnscQueuePopEntry(pFcNsListQueue);                                                \
        while ( pSLinkEntry )                                                                           \
        {                                                                                               \
            pFcNsList = ACCESS_CCSP_TR069PA_FC_NSLIST(pSLinkEntry);                                     \
            pSLinkEntry = AnscQueuePopEntry(pFcNsListQueue);                                            \
                                                                                                        \
            CcspTr069PaFreeFcNsList(pFcNsList);                                                         \
        }                                                                                               \
    } while (0)


__inline static PCCSP_TR069PA_FC_NSLIST
CcspTr069PaFindFcNsList
    (
        PQUEUE_HEADER               pFcNsListQueue, 
        char*                       pSubSystem, 
        char*                       pFcName
    )                  
{
    PSINGLE_LINK_ENTRY          pSLinkEntry;                            
    PCCSP_TR069PA_FC_NSLIST     pFcNsList, pFcNsListFound;                             
                                                                          
    pFcNsListFound = NULL;                                           
                                                                        
    pSLinkEntry = AnscQueueGetFirstEntry(pFcNsListQueue);          
    while ( pSLinkEntry )                                         
    {                                                            
        pFcNsList = ACCESS_CCSP_TR069PA_FC_NSLIST(pSLinkEntry); 
        pSLinkEntry = AnscQueueGetNextEntry(pSLinkEntry);      
                                                               
        if ( AnscEqualString(pFcNsList->FCName, pFcName, TRUE) )            
        {   
            if ( (pSubSystem && !pFcNsList->Subsystem) ||
                 (!pSubSystem && pFcNsList->Subsystem) ||
                 (pSubSystem && !AnscEqualString(pFcNsList->Subsystem, pSubSystem, TRUE)) )
            {                                                             
                continue;                                                
            }                                                           
            pFcNsListFound = pFcNsList;                                
            break;                                                    
        }                                                            
    }                                                               
    
    return pFcNsListFound;
}


#define  CcspTr069PaAddFcIntoFcNsList(pFcNsListQueue, pSubSystem, pFcName, pDbusPath, pFcNsList)        \
    do {                                                                                                \
        /*CcspTr069PaFindFcNsList(pFcNsListQueue, pSubSystem, pFcName, pFcNsList);*/                    \
        pFcNsList = CcspTr069PaFindFcNsList(pFcNsListQueue, pSubSystem, pFcName);                       \
                                                                                                        \
        if ( !pFcNsList )                                                                               \
        {                                                                                               \
            pFcNsList =                                                                                 \
                (PCCSP_TR069PA_FC_NSLIST)CcspTr069PaAllocateMemory                                      \
                    (                                                                                   \
                        sizeof(CCSP_TR069PA_FC_NSLIST)                                                  \
                    );                                                                                  \
                                                                                                        \
            if ( pFcNsList )                                                                            \
            {                                                                                           \
                pFcNsList->Subsystem= pSubSystem ? CcspTr069PaCloneString(pSubSystem) : NULL;           \
                pFcNsList->FCName   = pFcName? CcspTr069PaCloneString(pFcName) : NULL;                  \
                pFcNsList->DBusPath = pDbusPath ? CcspTr069PaCloneString(pDbusPath) : NULL;             \
                AnscQueueInitializeHeader(&pFcNsList->NsList);                                          \
                                                                                                        \
                AnscQueuePushEntry(pFcNsListQueue, &pFcNsList->Linkage);                                \
            }                                                                                           \
        }                                                                                               \
    } while (0)


__inline static
PANSC_UNIVERSAL_TIME
CcspStringToCalendarTime
    (
        char*                       calendar_time
    )
{
    PANSC_UNIVERSAL_TIME            pUniversalTime = (PANSC_UNIVERSAL_TIME      )NULL;
    ULONG                           ulFieldIndex   = 0;
    char                            temp_char [5];

    pUniversalTime = (PANSC_UNIVERSAL_TIME)CcspTr069PaAllocateMemory(sizeof(ANSC_UNIVERSAL_TIME));

    if ( !pUniversalTime )
    {
        return  (SLAP_HANDLE)NULL;
    }

    /*
     * The ISO 8601 notation is today the commonly recommended format of representing date and time
     * as human-readable strings in new plain-text communication protocols and file formats.
     * Several standards and profiles have been derived from ISO 8601, including RFC 3339 and a W3C
     * note on date and time formats.
     *
     *      - e.g. 2001-01-01
     *      - e.g. 2005-09-28T18:20:41
     */

    /*
     * Convert 'Year'...
     */
    ulFieldIndex = 0;

    if ( TRUE )
    {
        AnscZeroMemory(temp_char, 5);

        temp_char[0] = calendar_time[ulFieldIndex++];
        temp_char[1] = calendar_time[ulFieldIndex++];
        temp_char[2] = calendar_time[ulFieldIndex++];
        temp_char[3] = calendar_time[ulFieldIndex++];

        pUniversalTime->Year = (USHORT)AnscGetStringUlong(temp_char);
    }

    /*
     * Convert 'Month'...
     */
    ulFieldIndex++;

    if ( TRUE )
    {
        AnscZeroMemory(temp_char, 5);

        temp_char[0] = calendar_time[ulFieldIndex++];
        temp_char[1] = calendar_time[ulFieldIndex++];

        pUniversalTime->Month = (USHORT)AnscGetStringUlong(temp_char);
    }

    /*
     * Convert 'Day'...
     */
    ulFieldIndex++;

    if ( TRUE )
    {
        AnscZeroMemory(temp_char, 5);

        temp_char[0] = calendar_time[ulFieldIndex++];
        temp_char[1] = calendar_time[ulFieldIndex++];

        pUniversalTime->DayOfMonth = (USHORT)AnscGetStringUlong(temp_char);
    }

    /*
     * Convert 'Hour'...
     */
    ulFieldIndex++;

    if ( TRUE )
    {
        AnscZeroMemory(temp_char, 5);

        temp_char[0] = calendar_time[ulFieldIndex++];
        temp_char[1] = calendar_time[ulFieldIndex++];

        pUniversalTime->Hour = (USHORT)AnscGetStringUlong(temp_char);
    }

    /*
     * Convert 'Minute'...
     */
    ulFieldIndex++;

    if ( TRUE )
    {
        AnscZeroMemory(temp_char, 5);

        temp_char[0] = calendar_time[ulFieldIndex++];
        temp_char[1] = calendar_time[ulFieldIndex++];

        pUniversalTime->Minute = (USHORT)AnscGetStringUlong(temp_char);
    }

    /*
     * Convert 'Second'...
     */
    ulFieldIndex++;

    if ( TRUE )
    {
        AnscZeroMemory(temp_char, 5);

        temp_char[0] = calendar_time[ulFieldIndex++];
        temp_char[1] = calendar_time[ulFieldIndex++];

        pUniversalTime->Second = (USHORT)AnscGetStringUlong(temp_char);
    }

    return  pUniversalTime;
}


__inline static
char*
CcspCalendarTimeToString
    (
        PANSC_UNIVERSAL_TIME        pUniversalTime
    )
{
    char*                           var_string     = (char*)CcspTr069PaAllocateMemory(32);

    if ( !var_string )
    {
        return  NULL;
    }

    _ansc_sprintf
        (
            var_string,
            "%04d-%02d-%02dT%02d:%02d:%02dZ",
            pUniversalTime->Year,
            pUniversalTime->Month,
            pUniversalTime->DayOfMonth,
            pUniversalTime->Hour,
            pUniversalTime->Minute,
            pUniversalTime->Second
        );

    return  var_string;
}


/*
 * CcspFindUnusedInsNumber finds first unused instance number
 * from 1.
 *
 * Return value - instance number, 0 means failure.
 */
__inline static
unsigned int
CcspFindUnusedInsNumber
    (
        unsigned int                NumInstances,
        unsigned int*               pInsNumbers,
        unsigned int                MaxInsNumber
    )
{
    unsigned int                    InsNumber   = 1;
    unsigned int                    i;

    if ( NumInstances == 0 )
    {
        return  1;
    }
    else if ( NumInstances >= MaxInsNumber )
    {
        return  0;
    }

    while ( InsNumber != 0 )
    {
        for ( i = 0; i < NumInstances; i ++ )
        {
            if ( InsNumber == pInsNumbers[i] )
            {
                break;
            }
        }

        if ( i >= NumInstances )
        {
            return  InsNumber;
        }

        InsNumber ++;
        if ( InsNumber >= MaxInsNumber )
        {
            return 0;
        }
    }

    return  0;
}


__inline static
char*
CcspMemorySearch
    (
        PUCHAR                      pBuf,
        ULONG                       ulSize,
        PUCHAR                      pPattern,
        ULONG                       ulPatternLen,
        BOOL                        bCaseSensitive
    )
{
    PUCHAR                          pBufEnd     = pBuf + ulSize - 1;
    PUCHAR                          pNext, pNext1;
    UCHAR                           fc          = *pPattern;
    UCHAR                           fc1         = 0;

    if ( fc >= 'a' && fc <= 'z')
    {
        fc1 = fc - 'a' + 'A';
    }
    else if ( fc >= 'A' && fc <= 'Z' )
    {
        fc1 = fc - 'A' + 'a';
    }

    pNext1 = NULL;

    while ( pBuf && pBuf <= pBufEnd )
    {
        pNext = _ansc_memchr(pBuf, fc, pBufEnd - pBuf + 1);
        if ( fc1 != 0 )
        {
            pNext1 = _ansc_memchr(pBuf, fc, pBufEnd - pBuf + 1);
        }

        if ( !pNext )
        {
            pNext = pNext1;
        }
        else if ( pNext1 && pNext1 < pNext )
        {
            pNext = pNext1;
        }

        if ( !pNext )
        {
            return  NULL;
        }

        if ( pBufEnd - pNext + 1ul >= ulPatternLen &&
             AnscEqualString2((char*)pNext, (char*)pPattern, ulPatternLen, bCaseSensitive) )
        {
            return  (char*)pNext;
        }

        pBuf = pNext + 1;
    }

    return  NULL;
}


__inline static
void
CcspCwmpPrefixPsmKey
    (
        char*                       pPsmKeyWithPrefix,
        char*                       pSubsysPrefix,
        char*                       psmKey
        
    )
{
    /* we don't check buffer size, caller should make sure the buffer is big enough */

    if ( pSubsysPrefix && AnscSizeOfString(pSubsysPrefix) > 0 )
    {
        _ansc_sprintf(pPsmKeyWithPrefix, "%s%s", pSubsysPrefix, psmKey);
    }
    else
    {
        AnscCopyString(pPsmKeyWithPrefix, psmKey);
    }
}


#endif

