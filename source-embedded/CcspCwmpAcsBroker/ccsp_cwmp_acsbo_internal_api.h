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

    module:	ccsp_cwmp_acsbo_internal_api.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This header file contains the prototype definition for all
        the internal functions provided by the CCSP CWMP Acs Broker
        Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang

    ---------------------------------------------------------------

    revision:

        09/29/05    initial revision.

**********************************************************************/


#ifndef  _CCSP_CWMP_ACSBO_INTERNAL_API_
#define  _CCSP_CWMP_ACSBO_INTERNAL_API_


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_ACSBO_STATES.C
***********************************************************/

ANSC_HANDLE
CcspCwmpAcsboGetCcspCwmpMsoIf
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_HANDLE
CcspCwmpAcsboGetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpAcsboSetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCpeController
    );

ANSC_STATUS
CcspCwmpAcsboGetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

ANSC_STATUS
CcspCwmpAcsboSetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

ANSC_STATUS
CcspCwmpAcsboResetProperty
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpAcsboReset
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
       FUNCTIONS IMPLEMENTED IN CCSP_CWMP_ACSBO_OPERATION.C
***********************************************************/

ANSC_STATUS
CcspCwmpAcsboEngage
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpAcsboCancel
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_ACSBO_MSOIF.C
***********************************************************/

ANSC_STATUS
CcspCwmpAcsboMsoGetRpcMethods
    (
        ANSC_HANDLE                 hThisObject,
        SLAP_STRING_ARRAY**         ppMethodList
    );

ANSC_STATUS
CcspCwmpAcsboMsoInform
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pEventCode,
        char*                       pCommandKey,
        BOOL                        bConnectNow
    );

ANSC_STATUS
CcspCwmpAcsboMsoTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        char*                       pCommandKey,
        ANSC_HANDLE                 hFault,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        BOOL                        bNewSession
    );

ANSC_STATUS
CcspCwmpAcsboMsoAutonomousTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        char*                       AnnounceURL,
        char*                       TransferURL,
        char*                       FileType,
        unsigned int                FileSize,
        char*                       TargetFileName,
        ANSC_HANDLE                 hFault,      
        ANSC_HANDLE                 hStartTime,  
        ANSC_HANDLE                 hCompleteTime,
        BOOL                        bNewSession
    );

ANSC_STATUS
CcspCwmpAcsboMsoDuStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hDsccReq,
        BOOL                        bNewSession
    );

ANSC_STATUS
CcspCwmpAcsboMsoAutonomousDuStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hAdsccReq,
        BOOL                        bNewSession
    );

ANSC_STATUS
CcspCwmpAcsboMsoKicked
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCommand,
        char*                       pReferer,
        char*                       pArg,
        char*                       pNext,
        char**                      ppNextUrl
    );

ANSC_STATUS
CcspCwmpAcsboMsoRequestDownload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pFileType,
        ANSC_HANDLE                 hFileTypeArgArray,
        ULONG                       ulArraySize
    );

ANSC_STATUS
CcspCwmpAcsboMsoValueChanged
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        char*                       pParamValue,
        ULONG                       CwmpDataType,
        BOOL                        bConnectNow
    );


#endif
