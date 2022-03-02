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

    module:	ccsp_cwmp_sesso_control.c

        For CCSP CWMP protocol implementation


    ---------------------------------------------------------------

    description:

        This module implements the advanced control functions
        of the CcspCwmp Wmp Session Object.

        *   CcspCwmpsoIsAcsConnected
        *   CcspCwmpsoConnectToAcs
        *   CcspCwmpsoCloseConnection

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/13/05    initial revision.
        06/17/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_sesso_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        CcspCwmpsoIsAcsConnected
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to check whether ACS is currently
        connected.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     TRUE or FALSE.

**********************************************************************/

BOOL
CcspCwmpsoIsAcsConnected
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pCcspCwmpAcsConnection = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)pMyObject->hCcspCwmpAcsConnection;

    if ( !pMyObject->bActive )
    {
        return  FALSE;
    }

    return  pCcspCwmpAcsConnection->bActive;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoConnectToAcs
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to connect to the specified ACS.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoConnectToAcs
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pCcspCwmpAcsConnection = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)pMyObject->hCcspCwmpAcsConnection;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    char*                           pAcsUrl            = (char*                      )NULL;
    char*                           pAcsUsername       = (char*                      )NULL;
    char*                           pAcsPassword       = (char*                      )NULL;

    pAcsUrl      = pCcspCwmpProcessor->GetAcsUrl     ((ANSC_HANDLE)pCcspCwmpProcessor);
    pAcsUsername = pCcspCwmpProcessor->GetAcsUsername((ANSC_HANDLE)pCcspCwmpProcessor);
    pAcsPassword = pCcspCwmpProcessor->GetAcsPassword((ANSC_HANDLE)pCcspCwmpProcessor);

	if ( !pAcsUrl )
	{
		returnStatus = ANSC_STATUS_NOT_READY;

		goto  EXIT1;
	}
	else
	{
		pCcspCwmpAcsConnection->SetAcsUrl  ((ANSC_HANDLE)pCcspCwmpAcsConnection, pAcsUrl     );
		pCcspCwmpAcsConnection->SetUsername((ANSC_HANDLE)pCcspCwmpAcsConnection, pAcsUsername);
		pCcspCwmpAcsConnection->SetPassword((ANSC_HANDLE)pCcspCwmpAcsConnection, pAcsPassword);
	}

    if ( pAcsUrl ) 
    {
        AnscFreeMemory(pAcsUrl);
    }

    if ( pAcsUsername )
    {
        AnscFreeMemory(pAcsUsername);
    }

    if ( pAcsPassword )
    {
        AnscFreeMemory(pAcsPassword);
    }

    if (!pCcspCwmpAcsConnection->bActive )
    {
		returnStatus = pCcspCwmpAcsConnection->Connect((ANSC_HANDLE)pCcspCwmpAcsConnection);
	}

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        /*
         * Bin update on 07/08/2007
         *
         * We don't want to lock write access at the beginning of the session, we
         * are supposed to do it after we get a success InformResponse.
         */
        if( FALSE )
        {
            pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;
            returnStatus            =
                pCcspCwmpProcessor->SignalSession
                    (
                        (ANSC_HANDLE)pCcspCwmpProcessor,
                        (ANSC_HANDLE)pMyObject
                    );

            return  ANSC_STATUS_NOT_READY;
        }

        pMyObject->bActive      = TRUE;
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_connected;
        returnStatus            =
            pCcspCwmpProcessor->SignalSession
                (
                    (ANSC_HANDLE)pCcspCwmpProcessor,
                    (ANSC_HANDLE)pMyObject
                );
    }


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoCloseConnection
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to close the current ACS connection.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoCloseConnection
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT       pMyObject          = (PCCSP_CWMP_SESSION_OBJECT     )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pSessionTimerObj   = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hSessionTimerObj;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT pCcspCwmpAcsConnection = (PCCSP_CWMP_ACS_CONNECTION_OBJECT  )pMyObject->hCcspCwmpAcsConnection;
    PCCSP_CWMP_PROCESSOR_OBJECT     pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE        pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE          )pCcspCwmpProcessor->GetCcspCwmpMpaIf((ANSC_HANDLE)pCcspCwmpProcessor);
    int                             DelayTimes        = 10; 

    pSessionTimerObj->Stop((ANSC_HANDLE)pSessionTimerObj);

    pMyObject->bActive      = FALSE;
    pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_idle;

    /* would cause crash if AsyncProcessEvent is freed asynchronously */
    /*
    CcspTr069PaTraceDebug(("CcspCwmpsoCloseConnection -> SetEvent\n"));
    AnscSetEvent(&pMyObject->AsyncProcessEvent);
    */
    CcspTr069PaTraceDebug(("CcspCwmpsoCloseConnection -> waiting for all tasks exit ...\n"));

    while ( pMyObject->AsyncTaskCount > 0 && DelayTimes > 0 )
    {
        AnscSleep(500);
        DelayTimes --;
    }

    CcspTr069PaTraceDebug(("CcspCwmpsoCloseConnection -> Close\n"));
    pCcspCwmpAcsConnection->Close((ANSC_HANDLE)pCcspCwmpAcsConnection);

    CcspTr069PaTraceDebug(("CcspCwmpsoCloseConnection -> UnlockWriteAccess\n"));
    pCcspCwmpMpaIf->UnlockWriteAccess(pCcspCwmpMpaIf->hOwnerContext);

    return  ANSC_STATUS_SUCCESS;
}
