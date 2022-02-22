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

    module:	ccsp_cwmp_proco_states.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced state-access functions
        of the CCSP CWMP Processor Object.

        *   CcspCwmppoGetCcspCwmpCpeController
        *   CcspCwmppoSetCcspCwmpCpeController
        *   CcspCwmppoGetCcspCwmpMpaIf
        *   CcspCwmppoGetInitialContact
        *   CcspCwmppoSetInitialContact
        *   CcspCwmppoIsCwmpEnabled
        *   CcspCwmppoGetAcsUrl
        *   CcspCwmppoGetAcsUsername
        *   CcspCwmppoGetAcsPassword
        *   CcspCwmppoGetPeriodicInformEnabled
        *   CcspCwmppoGetPeriodicInformInterval
        *   CcspCwmppoGetPeriodicInformTime
        *   CcspCwmppoGetProperty
        *   CcspCwmppoSetProperty
        *   CcspCwmppoResetProperty
        *   CcspCwmppoReset

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/12/05    initial revision.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_proco_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmppoGetCcspCwmpCpeController
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

ANSC_HANDLE
CcspCwmppoGetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;

    return  pMyObject->hCcspCwmpCpeController;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSetCcspCwmpCpeController
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCpeController
            );

    description:

        This function is called to configure object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hCpeController
                Specifies the object state to be configured.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoSetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCpeController
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;

    pMyObject->hCcspCwmpCpeController = hCpeController;

    return  ANSC_STATUS_SUCCESS;
}



/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmppoGetCcspCwmpMpaIf
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

ANSC_HANDLE
CcspCwmppoGetCcspCwmpMpaIf
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;

    return  pMyObject->hCcspCwmpMpaIf;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        CcspCwmppoGetInitialContact
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

static BOOL						    s_bICLoaded = FALSE;

BOOL
CcspCwmppoGetInitialContact
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_PROPERTY    pProperty           = (PCCSP_CWMP_PROCESSOR_PROPERTY)&pMyObject->Property;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;

	if ( !s_bICLoaded )
	{
		char*						pValue               = NULL;
	    char                        psmKeyPrefixed[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN + 16];

    	CcspCwmpPrefixPsmKey(psmKeyPrefixed, pCcspCwmpCpeController->SubsysName, CCSP_TR069PA_PSM_KEY_InitialContact);
	    pValue =
    	    pCcspCwmpCpeController->LoadCfgFromPsm
	            (
                	(ANSC_HANDLE)pCcspCwmpCpeController,
            	    psmKeyPrefixed
        	    );

    	pProperty->bInitialContact = pValue ? _ansc_atoi(pValue) : TRUE;

        CcspTr069PaTraceWarning(("InitialContact read from PSM is: <%s>\n", pProperty->bInitialContact ? "TRUE":"FALSE")); 

        if (access(CCSP_MGMT_CRPWD_FILE,F_OK)!=0 && pValue!=NULL)
	{
		pProperty->bInitialContact = 1;
        	CcspTr069PaTraceWarning(("bInitialContact switched to <%s> as MgmtCRPwdID was missing\n", pProperty->bInitialContact ? "TRUE":"FALSE")); 
	}

    	if ( pValue )
	    {
    	    AnscFreeMemory(pValue);
	    }	
	}

    return  pProperty->bInitialContact;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSetInitialContact
            (
                ANSC_HANDLE                 hThisObject,
                BOOL                        bEnabled
            );

    description:

        This function is called to configure object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                BOOL                        bEnabled
                Specifies the object state to be configured.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoSetInitialContact
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bEnabled
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject          = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_PROPERTY   pProperty          = (PCCSP_CWMP_PROCESSOR_PROPERTY)&pMyObject->Property;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    char                            psmKeyPrefixed[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN + 16];

    CcspCwmpPrefixPsmKey(psmKeyPrefixed, pCcspCwmpCpeController->SubsysName, CCSP_TR069PA_PSM_KEY_InitialContact);

    pProperty->bInitialContact = bEnabled;

    returnStatus =
        pCcspCwmpCpeController->SaveCfgToPsm
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                psmKeyPrefixed,
                bEnabled ? "1" : "0"
            );

    return  returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        CcspCwmppoGetInitialContactFactory
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

BOOL
CcspCwmppoGetInitialContactFactory
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject              = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    BOOL                             bInitialContactFactory = FALSE;

	if ( !s_bICLoaded )
	{
		char*						pValue               = NULL;
	    char                        psmKeyPrefixed[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN + 16];

    	CcspCwmpPrefixPsmKey(psmKeyPrefixed, pCcspCwmpCpeController->SubsysName, CCSP_TR069PA_PSM_KEY_InitialContactFactory);
	    pValue =
    	    pCcspCwmpCpeController->LoadCfgFromPsm
	            (
                	(ANSC_HANDLE)pCcspCwmpCpeController,
            	    psmKeyPrefixed
        	    );

    	bInitialContactFactory = pValue ? _ansc_atoi(pValue) : TRUE;

        CcspTr069PaTraceWarning(("InitialContact read from PSM is: <%s>\n", bInitialContactFactory ? "TRUE":"FALSE")); 

    	if ( pValue )
	    {
    	    AnscFreeMemory(pValue);
	    }	
	}

    return  bInitialContactFactory;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSetInitialContactFactory
            (
                ANSC_HANDLE                 hThisObject,
                BOOL                        bEnabled
            );

    description:

        This function is called to configure object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                BOOL                        bEnabled
                Specifies the object state to be configured.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoSetInitialContactFactory
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bEnabled
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject          = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    char                            psmKeyPrefixed[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN + 16];

    CcspCwmpPrefixPsmKey(psmKeyPrefixed, pCcspCwmpCpeController->SubsysName, CCSP_TR069PA_PSM_KEY_InitialContactFactory);

    returnStatus =
        pCcspCwmpCpeController->SaveCfgToPsm
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                psmKeyPrefixed,
                bEnabled ? "1" : "0"
            );

    return  returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        CcspCwmppoIsCwmpEnabled
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

BOOL
CcspCwmppoIsCwmpEnabled
    (
        ANSC_HANDLE                 hThisObject
    )
{
    BOOL                            bCwmpEnabled        = FALSE;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;

    bCwmpEnabled = CcspManagementServer_GetEnableCWMP(pCcspCwmpCpeController->PANameWithPrefix);

    return  bCwmpEnabled;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmppoGetAcsUrl
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

char*
CcspCwmppoGetAcsUrl
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    char*                           pAcsUrl      = NULL;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;

    pAcsUrl = CcspManagementServer_GetURL(pCcspCwmpCpeController->PANameWithPrefix);

    return  pAcsUrl;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmppoGetAcsUsername
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

char*
CcspCwmppoGetAcsUsername
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    char*                           pAcsUsername = NULL;

    pAcsUsername = CcspManagementServer_GetUsername(pCcspCwmpCpeController->PANameWithPrefix);

    return  pAcsUsername;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmppoGetAcsPassword
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

char*
CcspCwmppoGetAcsPassword
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    char*                           pAcsPassword = NULL;

    pAcsPassword = CcspManagementServer_GetPassword(pCcspCwmpCpeController->PANameWithPrefix);

    return  pAcsPassword;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        CcspCwmppoGetPeriodicInformEnabled
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

BOOL
CcspCwmppoGetPeriodicInformEnabled
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    BOOL                            bEnabled     = FALSE;

    bEnabled = CcspManagementServer_GetPeriodicInformEnable(pCcspCwmpCpeController->PANameWithPrefix);

    return  bEnabled;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmppoGetPeriodicInformInterval
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

ULONG
CcspCwmppoGetPeriodicInformInterval
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    ULONG                           ulInterval   = 3600;

    ulInterval = CcspManagementServer_GetPeriodicInformInterval(pCcspCwmpCpeController->PANameWithPrefix);

    return  ulInterval;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmppoGetPeriodicInformTime
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     object state.

**********************************************************************/

ANSC_HANDLE
CcspCwmppoGetPeriodicInformTime
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject           = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    PANSC_UNIVERSAL_TIME            pInformTime          = NULL;
    PANSC_UNIVERSAL_TIME            pCalendarTime       = NULL;
    char*                           pPeriodicInformTime = NULL;

	pInformTime = (PANSC_UNIVERSAL_TIME)AnscAllocateMemory(sizeof(ANSC_UNIVERSAL_TIME));

	if ( !pInformTime ) return NULL;
    
	pPeriodicInformTime = 
        CcspManagementServer_GetPeriodicInformTimeStr
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    if ( pPeriodicInformTime && AnscSizeOfString(pPeriodicInformTime) != 0 )
    {
        pCalendarTime = CcspStringToCalendarTime(pPeriodicInformTime);

        if ( pCalendarTime )
        {
            *pInformTime = *pCalendarTime;
            AnscFreeMemory(pCalendarTime);
        }
    }

    if ( pPeriodicInformTime )
    {
        AnscFreeMemory(pPeriodicInformTime);
    }

    return  (ANSC_HANDLE)pInformTime;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoGetProperty
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hProperty
            );

    description:

        This function is called to retrieve object property.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hProperty
                Specifies the property data structure to be filled.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoGetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_PROPERTY    pProperty    = (PCCSP_CWMP_PROCESSOR_PROPERTY)&pMyObject->Property;

    *(PCCSP_CWMP_PROCESSOR_PROPERTY)hProperty = *pProperty;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSetProperty
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hProperty
            );

    description:

        This function is called to configure object property.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hProperty
                Specifies the property data structure to be copied.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoSetProperty
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hProperty
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_PROPERTY    pProperty    = (PCCSP_CWMP_PROCESSOR_PROPERTY)&pMyObject->Property;

    *pProperty = *(PCCSP_CWMP_PROCESSOR_PROPERTY)hProperty;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoResetProperty
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to reset object property.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoResetProperty
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_PROPERTY    pProperty    = (PCCSP_CWMP_PROCESSOR_PROPERTY)&pMyObject->Property;

    AnscZeroMemory(pProperty, sizeof(CCSP_CWMP_PROCESSOR_PROPERTY));

    pProperty->bInitialContact        = FALSE;
    pProperty->FirstTimeInterval      = 0;
    pProperty->FollowingInterval      = 0;

	pProperty->DefActiveNotifThrottle = 0;
	pProperty->LastActiveNotifTime    = 0;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoReset
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to reset object states.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoReset
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject    = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;

    if ( pMyObject->SecheduledCommandKey )
    {
        AnscFreeMemory(pMyObject->SecheduledCommandKey);

        pMyObject->SecheduledCommandKey = NULL;
    }

    return  ANSC_STATUS_SUCCESS;
}
