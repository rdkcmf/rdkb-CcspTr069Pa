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

    module:	ccsp_cwmp_stunmo_internal_api.h

        For CCSP CWMP protocol

    ---------------------------------------------------------------

    description:

        This header file contains the prototype definition for all
        the internal functions provided by the CCSP CWMP Stun Manager
        Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/05/08    initial revision.
        11/02/11    migrate to CCSP framework

**********************************************************************/


#ifndef  _CCSP_CWMP_STUNMO_INTERNAL_API_
#define  _CCSP_CWMP_STUNMO_INTERNAL_API_


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_STUNMO_STATES.C
***********************************************************/

ANSC_HANDLE
CcspCwmpStunmoGetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpStunmoSetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCpeController
    );

ANSC_HANDLE
CcspCwmpStunmoGetStunSimpleClient
    (
        ANSC_HANDLE                 hThisObject
    );

ULONG
CcspCwmpStunmoGetClientAddr
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpStunmoSetClientAddr
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulAddr
    );

ANSC_STATUS
CcspCwmpStunmoGetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

ANSC_STATUS
CcspCwmpStunmoSetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    );

ANSC_STATUS
CcspCwmpStunmoResetProperty
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpStunmoReset
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
       FUNCTIONS IMPLEMENTED IN CCSP_CWMP_STUNMO_OPERATION.C
***********************************************************/

ANSC_STATUS
CcspCwmpStunmoEngage
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpStunmoCancel
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpStunmoSetupEnv
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpStunmoCloseEnv
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
        FUNCTIONS IMPLEMENTED IN CCSP_CWMP_STUNMO_POLICY.C
***********************************************************/

ANSC_STATUS
CcspCwmpStunmoLoadPolicy
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpStunmoApplyStunSettings
    (
        ANSC_HANDLE                 hThisObject
    );

char*
CcspCwmpStunmoGetAcsHostName
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
       FUNCTIONS IMPLEMENTED IN CCSP_CWMP_STUNMO_REG_STUN.C
***********************************************************/

ANSC_STATUS
CcspCwmpStunmoRegGetStunInfo
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CcspCwmpStunmoRegSetStunInfo
    (
        ANSC_HANDLE                 hThisObject
    );


/***********************************************************
      FUNCTIONS IMPLEMENTED IN CCSP_CWMP_STUNMO_STUNBSMIF.C
***********************************************************/

ANSC_STATUS
CcspCwmpStunmoStunBsmRecvMsg1
    (
        ANSC_HANDLE                 hThisObject,
        void*                       buffer,
        ULONG                       ulSize
    );

ANSC_STATUS
CcspCwmpStunmoStunBsmRecvMsg2
    (
        ANSC_HANDLE                 hThisObject,
        void*                       buffer,
        ULONG                       ulSize
    );

ANSC_STATUS
CcspCwmpStunmoStunBsmNotify
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulEvent,
        ANSC_HANDLE                 hReserved
    );


#endif
