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

    module: ccsp_management_server_pa_api.h

        For the interface of the management_server exposed to PA.
        
    ---------------------------------------------------------------

    description:

        This file defines all interface functions of management_server
        exposed to PA.
        The component management_server shares same process with PA,
        so the interface functions is directly called by PA.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Hui Ma 

    ---------------------------------------------------------------

    revision:

        06/14/2011    initial revision.

**********************************************************************/

#ifndef  _CCSP_MANAGEMENT_SERVER_PA_API_DEC_
#define  _CCSP_MANAGEMENT_SERVER_PA_API_DEC_

#include "ccsp_types.h"
#include "ccsp_base_api.h"

/***************
no set parameter needed ***********************/

#define CCSP_MSVC_OBJECT_ManagementServer 	0x00000001			/*  any parameters right under Device.ManagementServer. */
#define CCSP_MSVC_OBJECT_ATCP 			0x00000002
#define CCSP_MSVC_OBJECT_DSCCP 			0x00000004
#define CCSP_MSVC_OBJECT_DiagComplete 	0x00000008			    /* received '8 DIAGNOSTICS COMPLETE' event  */

#define CCSP_MGMT_CRPWD_FILE    "/nvram/.keys/MgmtCRPwdID"
#define CCSP_MGMT_PWD_FILE      "/nvram/.keys/MgmtPwdID"


typedef
CCSP_VOID
(*PFN_CCSPMS_VALUECHANGE)
    (
	  CCSP_HANDLE			hContext,		/* msvcCtx passed into CcspManagementServer_Init() */
	  CCSP_INT				changedObjID	/* Or'ed value of CCSP_MSVC_OBJECT_xxx */
    );

typedef 
void
(*PFN_CCSP_SYSREADYSIG_CB)
    (
        void*               user_data
    );

typedef
void
(*PFN_CCSP_DIAGCOMPSIG_CB)
    (
        void*               user_data
    );    

/* CcspManagementServer_Init is called by PA to register component and 
 * load configuration file.
 * Return value - none.
 */
CCSP_VOID
CcspManagementServer_Init
    (
        CCSP_STRING             ComponentName,
        CCSP_STRING             SubsystemPrefix,
        CCSP_HANDLE             hMBusHandle,
        PFN_CCSPMS_VALUECHANGE	msValueChangeCB,
        PFN_CCSP_SYSREADYSIG_CB sysReadySignalCB,
        PFN_CCSP_DIAGCOMPSIG_CB diagCompleteCB,
        CCSP_HANDLE             cbContext,
        CCSP_HANDLE             cpeContext,
        CCSP_STRING             sdmXmlFilename
    );
/* Recevie value change signal from pPAMComponentName  */
CCSP_BOOL CcspManagementServer_paramValueChanged
	(
		parameterSigStruct_t*		val,
		int				size
	);

#ifdef   _CCSP_CWMP_STUN_ENABLED
CCSP_VOID
CcspManagementServer_StunBindingChanged
    (
        CCSP_BOOL                   NATDetected,
        char*                       UdpConnReqURL
    );
#endif


/* CcspManagementServer_GetEnableCWMP is called to get
 * Device.ManagementServer.EnableCWMP.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetEnableCWMP
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetEnableCWMPStr
    (
        CCSP_STRING                 ComponentName
    );
/* CcspManagementServer_SetEnableCWMP is called to set
 * Device.ManagementServer.EnableCWMP.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetEnableCWMP
    (
        CCSP_STRING                 ComponentName,
        CCSP_BOOL                   bEnableCWMP                    
    );

/* CcspManagementServer_GetURL is called to get
 * Device.ManagementServer.URL.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetURL
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetURL is called to set
 * Device.ManagementServer.URL.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetURL
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pURL                    
    );

/* CcspManagementServer_GetUsername is called to get
 * Device.ManagementServer.Username.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetUsername
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetUsername is called to set
 * Device.ManagementServer.Username.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetUsername
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pUsername                    
    );

/* CcspManagementServer_GetPassword is called to get
 * Device.ManagementServer.Password.
 * Return value - always empty.
 */
CCSP_STRING
CcspManagementServer_GetPassword
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetPassword is called to set
 * Device.ManagementServer.Password.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetPassword
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pPassword                    
    );

/* CcspManagementServer_GetPeriodicInformEnable is called to get
 * Device.ManagementServer.PeriodicInformEnable.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetPeriodicInformEnable
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetPeriodicInformEnableStr
    (
        CCSP_STRING                 ComponentName
    );
/* CcspManagementServer_SetPeriodicInformEnable is called to set
 * Device.ManagementServer.PeriodicInformEnable.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetPeriodicInformEnable
    (
        CCSP_STRING                 ComponentName,
        CCSP_BOOL                   bPeriodicInformEnable                    
    );

/* CcspManagementServer_GetPeriodicInformTime is called to get
 * Device.ManagementServer.PeriodicInformInterval.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetPeriodicInformInterval
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetPeriodicInformIntervalStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetPeriodicInformTime is called to get
 * Device.ManagementServer.PeriodicInformTime.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetPeriodicInformTime
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetPeriodicInformTimeStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetPeriodicInformTime is called to set
 * Device.ManagementServer.PeriodicInformTime.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetPeriodicInformTime
    (
        CCSP_STRING                 ComponentName,
        CCSP_UINT                   uiPeriodicInformTime                    
    );

/* CcspManagementServer_GetParameterKey is called to get
 * Device.ManagementServer.ParameterKey.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetParameterKey
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetParameterKey is called to set
 * Device.ManagementServer.ParameterKey.
 * This parameter is read-only. So it can only be written by PA.
 * Return value - 0 if success.
 * 
 */
CCSP_INT
CcspManagementServer_SetParameterKey
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pParameterKey                    
    );

/* CcspManagementServer_GetConnectionRequestURL is called to get
 * Device.ManagementServer.ConnectionRequestURL.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetConnectionRequestURL
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetFirstUpstreamIpInterface is called by PA to get value of
 *  * com.cisco.spvtg.ccsp.pam.Helper.FirstUpstreamIpInterface or psm stored value
 *   * Return value - the parameter value.
 *    */
CCSP_STRING
CcspManagementServer_GetFirstUpstreamIpAddress
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetConnectionRequestURLPort is called to get
 * Device.ManagementServer.X_CISCO_COM_ConnectionRequestURLPort.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetConnectionRequestURLPort
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetConnectionRequestURLPath is called to get
 * Device.ManagementServer.X_CISCO_COM_ConnectionRequestURLPath.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetConnectionRequestURLPath
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetConnectionRequestURL is called to set
 * Device.ManagementServer.ConnectionRequestURL.
 * This parameter is read-only. 
 * This component itself gets the parameter value from somewhere but cannt be written.
 * Return value - 0 if success.
 * 
 */
/*CCSP_INT
CcspManagementServer_SetConnectionRequestURL
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pConnectionRequestURL                    
    );*/

/* CcspManagementServer_GetConnectionRequestUsername is called to get
 * Device.ManagementServer.ConnectionRequestUsername.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetConnectionRequestUsername
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetConnectionRequestUsername is called to set
 * Device.ManagementServer.ConnectionRequestUsername.
 * Return value - 0 if success.
 * 
 */
CCSP_INT
CcspManagementServer_SetConnectionRequestUsername
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pConnectionRequestUsername                    
    );

/* CcspManagementServer_GetConnectionRequestPassword is called to get
 * Device.ManagementServer.ConnectionRequestPassword.
 * Return empty value.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetConnectionRequestPassword
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetConnectionRequestPassword is called to set
 * Device.ManagementServer.ConnectionRequestPassword.
 * Return value - 0 if success.
 * 
 */
CCSP_INT
CcspManagementServer_SetConnectionRequestPassword
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pConnectionRequestPassword                    
    );

/* CcspManagementServer_GetACSOverride is called to get
 * Device.ManagementServer.ACSOverride.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetACSOverride
    (
        CCSP_STRING                 ComponentName
    );

CCSP_STRING
CcspManagementServer_GetACSOverrideStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetUpgradesManaged is called to get
 * Device.ManagementServer.UpgradesManaged.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetUpgradesManaged
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetUpgradesManagedStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetUpgradesManaged is called to set
 * Device.ManagementServer.UpgradesManaged.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetUpgradesManaged
    (
        CCSP_STRING                 ComponentName,
        CCSP_BOOL                   bUpgradesManaged                    
    );

/* CcspManagementServer_GetKickURL is called to get
 * Device.ManagementServer.KickURL.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetKickURL
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetKickURL is called to set
 * Device.ManagementServer.KickURL.
 * This parameter is read-only. 
 * This component itself gets the parameter value from somewhere but cannt be written.
 * Return value - 0 if success.
 * 
 */
/*CCSP_INT
CcspManagementServer_SetKickURL
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pKickURL                    
    );*/

/* CcspManagementServer_GetDownloadProgressURL is called to get
 * Device.ManagementServer.DownloadProgressURL.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetDownloadProgressURL
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetDownloadProgressURL is called to set
 * Device.ManagementServer.DownloadProgressURL.
 * This parameter is read-only. 
 * This component itself gets the parameter value from somewhere but cannt be written.
 * Return value - 0 if success.
 * 
 */
/*CCSP_INT
CcspManagementServer_SetDownloadProgressURL
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pDownloadProgressURL                    
    );*/

/* CcspManagementServer_GetDefaultActiveNotificationThrottle is called to get
 * Device.ManagementServer.DefaultActiveNotificationThrottle.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetDefaultActiveNotificationThrottle
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetDefaultActiveNotificationThrottleStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetDefaultActiveNotificationThrottle is called to set
 * Device.ManagementServer.DefaultActiveNotificationThrottle.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetDefaultActiveNotificationThrottle
    (
        CCSP_STRING                 ComponentName,
        CCSP_UINT                   uiDefaultActiveNotificationThrottle                    
    );

/* CcspManagementServer_GetCWMPRetryMinimumWaitInterval is called to get
 * Device.ManagementServer.CWMPRetryMinimumWaitInterval.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetCWMPRetryMinimumWaitInterval
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetCWMPRetryMinimumWaitIntervalStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetCWMPRetryMinimumWaitInterval is called to set
 * Device.ManagementServer.CWMPRetryMinimumWaitInterval.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetCWMPRetryMinimumWaitInterval
    (
        CCSP_STRING                 ComponentName,
        CCSP_UINT                   uiCWMPRetryMinimumWaitInterval                    
    );

/* CcspManagementServer_GetCWMPRetryIntervalMultiplier is called to get
 * Device.ManagementServer.CWMPRetryIntervalMultiplier.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetCWMPRetryIntervalMultiplier
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetCWMPRetryIntervalMultiplierStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetCWMPRetryIntervalMultiplier is called to set
 * Device.ManagementServer.CWMPRetryIntervalMultiplier.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetCWMPRetryIntervalMultiplier
    (
        CCSP_STRING                 ComponentName,
        CCSP_UINT                   uiCWMPRetryIntervalMultiplier                    
    );

/* CcspManagementServer_GetUDPConnectionRequestAddress is called to get
 * Device.ManagementServer.UDPConnectionRequestAddress.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetUDPConnectionRequestAddress
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetUDPConnectionRequestAddressNotificationLimit is called to get
 * Device.ManagementServer.UDPConnectionRequestAddressNotificationLimit.
 * Return value - the parameter value.
 */
// CCSP_UINT
CCSP_STRING
CcspManagementServer_GetUDPConnectionRequestAddressNotificationLimit
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetUDPConnectionRequestAddress is called to set
 * Device.ManagementServer.UDPConnectionRequestAddress.
 * This parameter is read-only. 
 * This component itself gets the parameter value from somewhere but cannt be written.
 * Return value - 0 if success.
 * 
 */
/*CCSP_INT
CcspManagementServer_SetUDPConnectionRequestAddress
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pUDPConnectionRequestAddress                    
    );*/

/* CcspManagementServer_GetSTUNEnable is called to get
 * Device.ManagementServer.STUNEnable.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetSTUNEnable
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetSTUNEnableStr
    (
        CCSP_STRING                 ComponentName
    );

CCSP_STRING
CcspManagementServer_GetCustomDataModelEnabledStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetSTUNEnable is called to set
 * Device.ManagementServer.STUNEnable.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetSTUNEnable
    (
        CCSP_STRING                 ComponentName,
        CCSP_BOOL                   bSTUNEnable                    
    );

/* CcspManagementServer_GetSTUNServerAddress is called to get
 * Device.ManagementServer.STUNServerAddress.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSTUNServerAddress
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetSTUNServerAddress is called to set
 * Device.ManagementServer.STUNServerAddress.
 * Return value - 0 if success.
 * 
 */
CCSP_INT
CcspManagementServer_SetSTUNServerAddress
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pSTUNServerAddress                    
    );

/* CcspManagementServer_GetSTUNServerPort is called to get
 * Device.ManagementServer.STUNServerPort.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetSTUNServerPort
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetSTUNServerPortStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetSTUNServerPort is called to set
 * Device.ManagementServer.STUNServerPort.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetSTUNServerPort
    (
        CCSP_STRING                 ComponentName,
        CCSP_UINT                   uiSTUNServerPort                    
    );

/* CcspManagementServer_GetSTUNUsername is called to get
 * Device.ManagementServer.STUNUsername.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSTUNUsername
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetSTUNUsername is called to set
 * Device.ManagementServer.STUNUsername.
 * Return value - 0 if success.
 * 
 */
CCSP_INT
CcspManagementServer_SetSTUNUsername
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pSTUNUsername                    
    );

/* CcspManagementServer_GetSTUNPassword is called to get
 * Device.ManagementServer.STUNPassword.
 * Return empty value.
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetSTUNPassword
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetSTUNPassword is called to set
 * Device.ManagementServer.STUNPassword.
 * Return value - 0 if success.
 * 
 */
CCSP_INT
CcspManagementServer_SetSTUNPassword
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pSTUNPassword                    
    );

/* CcspManagementServer_GetSTUNMaximumKeepAlivePeriod is called to get
 * Device.ManagementServer.STUNMaximumKeepAlivePeriod.
 * Return value - the parameter value.
 */
CCSP_INT
CcspManagementServer_GetSTUNMaximumKeepAlivePeriod
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetSTUNMaximumKeepAlivePeriodStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetSTUNMaximumKeepAlivePeriod is called to set
 * Device.ManagementServer.STUNMaximumKeepAlivePeriod.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetSTUNMaximumKeepAlivePeriod
    (
        CCSP_STRING                 ComponentName,
        CCSP_INT                    iSTUNMaximumKeepAlivePeriod                    
    );

/* CcspManagementServer_GetSTUNMinimumKeepAlivePeriod is called to get
 * Device.ManagementServer.STUNMinimumKeepAlivePeriod.
 * Return value - the parameter value.
 */
CCSP_UINT
CcspManagementServer_GetSTUNMinimumKeepAlivePeriod
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetSTUNMinimumKeepAlivePeriodStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetSTUNMinimumKeepAlivePeriod is called to set
 * Device.ManagementServer.STUNMinimumKeepAlivePeriod.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetSTUNMinimumKeepAlivePeriod
    (
        CCSP_STRING                 ComponentName,
        CCSP_UINT                   uiSTUNMinimumKeepAlivePeriod                    
    );

/* CcspManagementServer_GetNATDetected is called to get
 * Device.ManagementServer.NATDetected.
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetNATDetected
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetNATDetectedStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetNATDetected is called to set
 * Device.ManagementServer.NATDetected.
 * This parameter is read-only. 
 * It can only be written by PA.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetNATDetected
    (
        CCSP_STRING                 ComponentName,
        CCSP_BOOL                   bNATDetected                    
    );

/* 
 * Device.ManagementServer.ManageableDevice.{i}.
 * is not supported. So Device.ManagementServer.ManageableDeviceNumberOfEntries 
 * is not supported here.
 * 
 */

/* CcspManagementServer_GetAutonomousTransferCompletePolicy_Enable is called to get
 * Device.ManagementServer.AutonomousTransferCompletePolicy.Enable
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetAutonomousTransferCompletePolicy_Enable
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetAutonomousTransferCompletePolicy_EnableStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetAutonomousTransferCompletePolicy_Enable is called to set
 * Device.ManagementServer.AutonomousTransferCompletePolicy.Enable.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetAutonomousTransferCompletePolicy_Enable
    (
        CCSP_STRING                 ComponentName,
        CCSP_BOOL                   bAutonomousTransferCompletePolicy_Enable                    
    );

/* CcspManagementServer_GetAutonomousTransferCompletePolicy_TransferTypeFilter is called to get
 * Device.ManagementServer.AutonomousTransferCompletePolicy.TransferTypeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetAutonomousTransferCompletePolicy_TransferTypeFilter
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetAutonomousTransferCompletePolicy_TransferTypeFilter is called to set
 * Device.ManagementServer.AutonomousTransferCompletePolicy.TransferTypeFilter.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetAutonomousTransferCompletePolicy_TransferTypeFilter
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pAutonomousTransferCompletePolicy_TransferTypeFilter                    
    );

/* CcspManagementServer_GetAutonomousTransferCompletePolicy_ResultTypeFilter is called to get
 * Device.ManagementServer.AutonomousTransferCompletePolicy.ResultTypeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetAutonomousTransferCompletePolicy_ResultTypeFilter
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetAutonomousTransferCompletePolicy_ResultTypeFilter is called to set
 * Device.ManagementServer.AutonomousTransferCompletePolicy.ResultTypeFilter.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetAutonomousTransferCompletePolicy_ResultTypeFilter
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pAutonomousTransferCompletePolicy_ResultTypeFilter                    
    );

/* CcspManagementServer_GetAutonomousTransferCompletePolicy_FileTypeFilter is called to get
 * Device.ManagementServer.AutonomousTransferCompletePolicy.FileTypeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetAutonomousTransferCompletePolicy_FileTypeFilter
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetAutonomousTransferCompletePolicy_FileTypeFilter is called to set
 * Device.ManagementServer.AutonomousTransferCompletePolicy.FileTypeFilter.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetAutonomousTransferCompletePolicy_FileTypeFilter
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pAutonomousTransferCompletePolicy_FileTypeFilter                    
    );

/* CcspManagementServer_GetDUStateChangeComplPolicy_Enable is called to get
 * Device.ManagementServer.DUStateChangeComplPolicy.Enable
 * Return value - the parameter value.
 */
CCSP_BOOL
CcspManagementServer_GetDUStateChangeComplPolicy_Enable
    (
        CCSP_STRING                 ComponentName
    );
CCSP_STRING
CcspManagementServer_GetDUStateChangeComplPolicy_EnableStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetDUStateChangeComplPolicy_Enable is called to set
 * Device.ManagementServer.DUStateChangeComplPolicy.Enable.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetDUStateChangeComplPolicy_Enable
    (
        CCSP_STRING                 ComponentName,
        CCSP_BOOL                   bDUStateChangeComplPolicy_Enable                    
    );

/* CcspManagementServer_GetDUStateChangeComplPolicy_OperationTypeFilter is called to get
 * Device.ManagementServer.DUStateChangeComplPolicy.OperationTypeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetDUStateChangeComplPolicy_OperationTypeFilter
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetAutonomousTransferCompletePolicy_OperationTypeFilter is called to set
 * Device.ManagementServer.DUStateChangeComplPolicy.OperationTypeFilter.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetDUStateChangeComplPolicy_OperationTypeFilter
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pDUStateChangeComplPolicy_OperationTypeFilter                    
    );

/* CcspManagementServer_GetDUStateChangeComplPolicy_ResultTypeFilter is called to get
 * Device.ManagementServer.DUStateChangeComplPolicy.ResultTypeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetDUStateChangeComplPolicy_ResultTypeFilter
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetAutonomousTransferCompletePolicy_ResultTypeFilter is called to set
 * Device.ManagementServer.DUStateChangeComplPolicy.ResultTypeFilter.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetDUStateChangeComplPolicy_ResultTypeFilter
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pDUStateChangeComplPolicy_ResultTypeFilter                    
    );

/* CcspManagementServer_GetDUStateChangeComplPolicy_FaultCodeFilter is called to get
 * Device.ManagementServer.DUStateChangeComplPolicy.FaultCodeFilter
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetDUStateChangeComplPolicy_FaultCodeFilter
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetAutonomousTransferCompletePolicy_FaultCodeFilter is called to set
 * Device.ManagementServer.DUStateChangeComplPolicy.FaultCodeFilter.
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetDUStateChangeComplPolicy_FaultCodeFilter
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 pDUStateChangeComplPolicy_FaultCodeFilter                    
    );

/* CcspManagementServer_GetTr069pa_Name is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Name
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_Name
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetTr069pa_Version is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Version
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_Version
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetTr069pa_Author is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Author
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_Author
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetTr069pa_Health is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Health
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_Health
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetTr069pa_State is called to get
 * com.cisco.spvtg.ccsp.tr069pa.State
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_State
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetTr069pa_DTXml is called to get
 * com.cisco.spvtg.ccsp.tr069pa.DTXml
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetTr069pa_DTXml
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetMemory_MinUsageStr is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Memory.MinUsage
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetMemory_MinUsageStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetMemory_MaxUsage is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Memory.MaxUsage
 * Return value - the parameter value.
 */
ULONG
CcspManagementServer_GetMemory_MaxUsage
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetMemory_MaxUsageStr is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Memory.MaxUsage
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetMemory_MaxUsageStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetMemory_Consumed is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Memory.Consumed
 * Return value - the parameter value.
 */
ULONG
CcspManagementServer_GetMemory_Consumed
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetMemory_ConsumedStr is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Memory.Consumed
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetMemory_ConsumedStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_GetLogging_EnableStr is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Logging.Enable
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetLogging_EnableStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetLogging_EnableStr is called to set
 * com.cisco.spvtg.ccsp.tr069pa.Logging.Enable
 * Return value - 0 if success.
 */
CCSP_INT
CcspManagementServer_SetLogging_EnableStr
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 Value
    );

/* CcspManagementServer_GetLogging_LogLevelStr is called to get
 * com.cisco.spvtg.ccsp.tr069pa.Logging.LogLevel
 * Return value - the parameter value.
 */
CCSP_STRING
CcspManagementServer_GetLogging_LogLevelStr
    (
        CCSP_STRING                 ComponentName
    );

/* CcspManagementServer_SetLogging_LogLevelStr is called to set
 * com.cisco.spvtg.ccsp.tr069pa.Logging.LogLevel
 * Return value - 0 if success.
 */
CCSP_STRING
CcspManagementServer_SetLogging_LogLevelStr
    (
        CCSP_STRING                 ComponentName,
        CCSP_STRING                 Value
    );

CCSP_STRING
CcspManagementServer_GetTR069_NotificationStr
    (
        CCSP_STRING                 ComponentName
    );

CCSP_STRING
CcspManagementServer_GetConnected_ClientStr
    (
        CCSP_STRING                 ComponentName
    );

#endif
