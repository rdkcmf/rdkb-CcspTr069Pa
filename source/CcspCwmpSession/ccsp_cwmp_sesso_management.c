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

    module:	ccsp_cwmp_sesso_management.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    copyright:

        Cisco Systems, Inc., 1997 ~ 2011
        All Rights Reserved.

    ---------------------------------------------------------------

    description:

        This module implements the advanced management functions
        of the CCSP CWMP Session Object.

        *   CcspCwmpsoAddCwmpEvent
        *   CcspCwmpsoDiscardCwmpEvent
        *   CcspCwmpsoDelAllEvents
        *   CcspCwmpsoAddModifiedParameter
        *   CcspCwmpsoDelAllParameters
        *   CcspCwmpsoSaveCwmpEvent

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/14/05    initial revision.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_sesso_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoAddCwmpEvent
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCwmpEvent,
                BOOL                        bConnectNow
            );

    description:

        This function is called to add an event that will be included
        in the next Inform method call to ACS.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hCwmpEvent
                Specifies the event structure to be added.

                BOOL                        bConnectNow
                Specifies whether a connection to the ACS should be
                established right now.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoAddCwmpEvent
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCwmpEvent,
        BOOL                        bConnectNow
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT       pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT     pCcspCwmpProcessor = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_EVENT                pCcspCwmpEvent     = (PCCSP_CWMP_EVENT           )hCwmpEvent;
    PCCSP_CWMP_EVENT                pCcspCwmpEventExist= (PCCSP_CWMP_EVENT           )NULL;
    BOOL                            bExist             = FALSE;
    ULONG                           i                  = 0;

    if ( pMyObject->EventCount >= CCSP_CWMPSO_MAX_EVENT_NUMBER )
    {
        CcspCwmpFreeEvent(pCcspCwmpEvent);
	}
	else
	{
        /*
         *  Bin Zhu updated here on 12/18/2006
         *  According to WT151, for all the single event (starts with a number "0" - "9"),
         *  If there's only already in the array, the new one will be discarded.
         */
        if( pCcspCwmpEvent->EventCode[0] >= '0' && pCcspCwmpEvent->EventCode[0] <= '9')
        {
            for( i = 0; i < pMyObject->EventCount; i ++)
            {
                pCcspCwmpEventExist = (PCCSP_CWMP_EVENT)pMyObject->EventArray[i];

                if( AnscEqualString(pCcspCwmpEvent->EventCode, pCcspCwmpEventExist->EventCode, TRUE))
                {
                    bExist = TRUE;

                    CcspTr069PaTraceWarning(("The event '%s' is already there, discarded.\n", pCcspCwmpEvent->EventCode));

                    break;
                }
            }
        } 
        else 
        if ( pCcspCwmpEvent->EventCode[0] >= 'M' )
        {
            for( i = 0; i < pMyObject->EventCount; i ++)
            {
                pCcspCwmpEventExist = (PCCSP_CWMP_EVENT)pMyObject->EventArray[i];

                if ( AnscEqualString(pCcspCwmpEvent->EventCode, pCcspCwmpEventExist->EventCode, TRUE) &&
                     ( (!pCcspCwmpEvent->CommandKey && !pCcspCwmpEventExist->CommandKey) || 
                        (pCcspCwmpEvent->CommandKey && pCcspCwmpEventExist->CommandKey &&
                         AnscEqualString(pCcspCwmpEvent->CommandKey, pCcspCwmpEventExist->CommandKey, TRUE)) ) )
                {
                    bExist = TRUE;

                    CcspTr069PaTraceWarning
                        ((
                            "The event '%s' with CommandKey '%s' is already there, discarded.\n", 
                            pCcspCwmpEvent->EventCode,
                            pCcspCwmpEvent->CommandKey
                        ));

                    break;
                }
            }
        }

        if( !bExist )
        {
  		    pMyObject->EventArray[pMyObject->EventCount++] = (ANSC_HANDLE)pCcspCwmpEvent;
            CcspTr069PaTraceDebug(("<RT> Event '%s' with CommandKey '%s' added at location '%d'\n",
                                   pCcspCwmpEvent->EventCode, pCcspCwmpEvent->CommandKey, pMyObject->EventCount));
        }
        else
        {
            CcspCwmpFreeEvent(pCcspCwmpEvent);
        }
	}

    if ( bConnectNow )
    {
		PANSC_TIMER_DESCRIPTOR_OBJECT   pDelayedActiveNotifTimerObj
                                                        = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hDelayedActiveNotifTimerObj;
    	PCCSP_CWMP_PROCESSOR_PROPERTY   pProperty       = (PCCSP_CWMP_PROCESSOR_PROPERTY)&pCcspCwmpProcessor->Property;
		BOOL							bInformDelayed  = FALSE;
        PCCSP_CWMP_EVENT                pFirstEvent     = (PCCSP_CWMP_EVENT)pMyObject->EventArray[0];
	
		/* Active Notification Throttling */	
		if ( pMyObject->EventCount == 1 /* "4 VALUE CHANGE" event is the only one to trigger Inform */ && 
			 AnscEqualString(pFirstEvent->EventCode, CCSP_CWMP_INFORM_EVENT_NAME_ValueChange, TRUE) &&
			 pProperty->DefActiveNotifThrottle != 0 && 
			 pProperty->LastActiveNotifTime != 0 )
		{
			/* calculate the delay inform timer interval */
			ULONG						ulTimeNow = AnscGetTickInSeconds();
			ULONG						ulDelta   = 0;
			ULONG						ulInterval= 0;

			if ( ulTimeNow >= pProperty->LastActiveNotifTime )
			{
				ulDelta = ulTimeNow - pProperty->LastActiveNotifTime;
			}
			else
			{
				ulDelta = 0xFFFFFFFF - pProperty->LastActiveNotifTime + ulTimeNow;
			}

			if ( ulDelta < pProperty->DefActiveNotifThrottle )
			{
				bInformDelayed = TRUE;

				ulInterval = pProperty->DefActiveNotifThrottle - ulDelta;
 
    			CcspTr069PaTraceDebug(("Active notification will be delayed by %u seconds\n", ulInterval));

                if ( !pMyObject->bDelayedActiveNotifTimerScheduled )
                {
		            pDelayedActiveNotifTimerObj->SetInterval((ANSC_HANDLE)pDelayedActiveNotifTimerObj, ulInterval * 1000);
	    	        pDelayedActiveNotifTimerObj->Start((ANSC_HANDLE)pDelayedActiveNotifTimerObj);
    				pMyObject->bDelayedActiveNotifTimerScheduled = TRUE;
                }
 			}
		}


		if ( !bInformDelayed )
		{
            ULONG                   ulActiveSessions    = pCcspCwmpProcessor->GetActiveWmpSessionCount((ANSC_HANDLE)pCcspCwmpProcessor, TRUE);

            if ( ulActiveSessions == 0 )
            {
        	    pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_connectNow;
    	        returnStatus            =
        	        pCcspCwmpProcessor->SignalSession
            	        (
                	        (ANSC_HANDLE)pCcspCwmpProcessor,
                    	    (ANSC_HANDLE)pMyObject
	                    );
            }
            else
            {
                pMyObject->bInformWhenActive = TRUE;
            }
		}
    }

    return 	returnStatus; 
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoDiscardCwmpEvent
            (
                ANSC_HANDLE                 hThisObject,
                int                         EventCode
            );

    description:

        This function is called to discard certain Cwmp Events such as 
        "6 ConnectionRequest" before the session retry;

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                int                         EventCode
                Specify the event code to be removed.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoDiscardCwmpEvent
    (
        ANSC_HANDLE                 hThisObject,
        int                         EventCode
    )
{
    PCCSP_CWMP_SESSION_OBJECT       pMyObject          = (PCCSP_CWMP_SESSION_OBJECT  )hThisObject;
    PCCSP_CWMP_EVENT                pCcspCwmpEvent     = (PCCSP_CWMP_EVENT           )NULL;
    ULONG                           i                  = 0;
    ULONG                           j                  = 0;

    /*
     * According to WT151, only the event "6 CONNECTIONREQUEST" must NOT retry delivery and it's a
     * single type event.
     *
     * Let's find and discard it;   
     */
    for( i = 0; i < pMyObject->EventCount; i ++)
    { 
        pCcspCwmpEvent = (PCCSP_CWMP_EVENT)pMyObject->EventArray[i];

        if ( ( EventCode == CCSP_CWMPSO_EVENTCODE_ConnectionRequest && 
             AnscEqualString(pCcspCwmpEvent->EventCode, CCSP_CWMP_INFORM_EVENT_NAME_ConnectionRequest, TRUE) ) ||
             ( EventCode == CCSP_CWMPSO_EVENTCODE_ValueChange && 
             AnscEqualString(pCcspCwmpEvent->EventCode, CCSP_CWMP_INFORM_EVENT_NAME_ValueChange, TRUE) ) )

        {
            if ( EventCode == CCSP_CWMPSO_EVENTCODE_ConnectionRequest )
            {
                CcspTr069PaTraceWarning(("Before the session retry, the event '%s' was discarded.\n", CCSP_CWMP_INFORM_EVENT_NAME_ConnectionRequest));
            }
            else
            {
                CcspTr069PaTraceWarning(("The event with code %d was discarded.\n", EventCode));
            }

            CcspCwmpFreeEvent(pCcspCwmpEvent);

            pMyObject->EventArray[i] = NULL;

            /* move the reminder event back */
            for( j = i; j < pMyObject->EventCount - 1; j ++)
            {
                pMyObject->EventArray[j] = pMyObject->EventArray[j + 1];
            }

            pMyObject->EventArray[pMyObject->EventCount - 1]  = NULL;
            pMyObject->EventCount --;
        }
    }

    return  ANSC_STATUS_SUCCESS;;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoDelAllEvents
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to delete all events.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoDelAllEvents
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pCcspCwmpAcsConnection = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)pMyObject->hCcspCwmpAcsConnection;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;    
    PCCSP_CWMP_EVENT                pCcspCwmpEvent     = (PCCSP_CWMP_EVENT           )NULL;
    ULONG                           i                  = 0;

    for ( i = 0; i < pMyObject->EventCount; i++ )
    {
        pCcspCwmpEvent = (PCCSP_CWMP_EVENT)pMyObject->EventArray[i];

        if ( pCcspCwmpEvent )
        {
            CcspCwmpFreeEvent(pCcspCwmpEvent);

            pMyObject->EventArray[i] = (ANSC_HANDLE)NULL;
        }
    }

    pMyObject->EventCount = 0;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoAddModifiedParameter
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pParamName,
                BOOL                        bConnectNow
            );

    description:

        This function is called to add a parameter name whose value
        has been modified by an entity other than the ACS.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pParamName
                Specifies the parameter name whose value has been
                modified.

                char*                       pParamValue
                Specified the new parameter value.

                ULONG                       CwmpDataType
                CWMP data type of the parameter.

                BOOL                        bConnectNow
                Specifies whether a connection to the ACS should be
                established right now.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoAddModifiedParameter
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        char*                       pParamValue,
        ULONG                       CwmpDataType,
        BOOL                        bConnectNow
    )
{
    ANSC_STATUS                     returnStatus       		= ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT       pMyObject          		= (PCCSP_CWMP_SESSION_OBJECT)hThisObject;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     
									pCcspCwmpAcsConnection 	= (PCCSP_CWMP_ACS_CONNECTION_OBJECT)pMyObject->hCcspCwmpAcsConnection;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     
									pCcspCwmpCpeController 	= (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT     pCcspCwmpProcessor  	= (PCCSP_CWMP_PROCESSOR_OBJECT)pMyObject->hCcspCwmpProcessor;
    ULONG                           i                  		= 0;
    ULONG                           bExist             		= FALSE;

    CcspTr069PaTraceDebug(("AddModifiedParameter - '%s'\n", pParamName));

    if ( pMyObject->ModifiedParamCount >= CCSP_CWMPSO_MAX_PARAM_NUMBER )
    {
        CcspTr069PaFreeMemory(pParamName);

        return  ANSC_STATUS_RESOURCES;
    }

    for ( i = 0; i < pMyObject->ModifiedParamCount; i++ )
    {
        if ( pMyObject->ModifiedParamArray[i] )
        {
            if ( AnscEqualString
                    (
                        pParamName,
                        pMyObject->ModifiedParamArray[i],
                        TRUE
                    ) )
            {
                bExist = TRUE;

                if ( pMyObject->ModifiedParamValueArray[i] )
                {
                    CcspTr069PaFreeMemory(pMyObject->ModifiedParamValueArray[i]);
                }

                pMyObject->ModifiedParamValueArray[i] = CcspTr069PaCloneString(pParamValue);
                pMyObject->ModifiedParamTypeArray [i] = CwmpDataType;

                break;
            }
        }
    }

    if( !bExist)
    {
        pMyObject->ModifiedParamArray     [pMyObject->ModifiedParamCount]   = CcspTr069PaCloneString(pParamName);
        pMyObject->ModifiedParamValueArray[pMyObject->ModifiedParamCount]   = CcspTr069PaCloneString(pParamValue);
        pMyObject->ModifiedParamTypeArray [pMyObject->ModifiedParamCount++] = CwmpDataType;
    }

    /*
     * Bin: We are supposed to add "ValueChange" event for passive notification as well.
     */
    if ( TRUE )
    {
        PCCSP_CWMP_EVENT            pCcspCwmpEvent = (PCCSP_CWMP_EVENT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_EVENT));

        if ( !pCcspCwmpEvent )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pCcspCwmpEvent->EventCode  = CcspTr069PaCloneString(CCSP_CWMP_INFORM_EVENT_NAME_ValueChange);
            pCcspCwmpEvent->CommandKey = NULL;
        }

        returnStatus =
            pMyObject->AddCwmpEvent
                (
                    (ANSC_HANDLE)pMyObject,
                    pCcspCwmpEvent,
                    bConnectNow && pCcspCwmpCpeController->bBootInformSent
                );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoDelAllParameters
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to delete all modified parameters.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoDelAllParameters
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pCcspCwmpAcsConnection = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)pMyObject->hCcspCwmpAcsConnection;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    ULONG                           i                  = 0;

    for ( i = 0; i < pMyObject->ModifiedParamCount; i++ )
    {
        if ( pMyObject->ModifiedParamArray[i] )
        {
            CcspTr069PaFreeMemory(pMyObject->ModifiedParamArray[i]);

            pMyObject->ModifiedParamArray[i] = NULL;
        }
    }

    pMyObject->ModifiedParamCount = 0;

    return  ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoSaveCwmpEvent
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to save Cwmp Events before reboot

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/
BOOL
CcspCwmpIsUndiscardedEvent
    (
        char*                       eventName
    )
{
    /*
     *  According to WT151, the events "2 PERIODIC", "3 SCHEDULED", "7 TRANSFERCOMPLETE"
     *  and the ones start with "M" won't be discarded
     */
    if( AnscEqualString(eventName, CCSP_CWMP_INFORM_EVENT_NAME_Peroidic, TRUE))
    {
        return TRUE;
    }
    else if( AnscEqualString(eventName, CCSP_CWMP_INFORM_EVENT_NAME_Scheduled, TRUE))
    {
        return TRUE;
    }
    else if( AnscEqualString(eventName, CCSP_CWMP_INFORM_EVENT_NAME_TransferComplete, TRUE))
    {
        return TRUE;
    }
    else if( eventName[0] == 'M')
    {
        return TRUE;
    }

    return FALSE;
}

ANSC_STATUS
CcspCwmpsoSaveCwmpEvent
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_EVENT                pCcspCwmpEvent     = (PCCSP_CWMP_EVENT           )NULL;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_CFG_INTERFACE             pCcspCwmpCfgIf         = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->GetCcspCwmpCfgIf((ANSC_HANDLE)pCcspCwmpCpeController);
    char                            pSavedEvents[CCSP_CWMP_SAVED_EVENTS_MAX_LEN] = { 0 };
    PCHAR                           pCurBuf            = pSavedEvents;
    ULONG                           i                  = 0;
    ULONG                           uLength            = 0;
    char*                           pRootObjName       = pCcspCwmpCpeController->GetRootObject((ANSC_HANDLE)pCcspCwmpCpeController);
    BOOL                            bRootDevice        = AnscEqualString(pRootObjName, DM_ROOTNAME, FALSE);

    /*
     * All the undiscarded events will be saved in a string seperated by a ',' and put as 
     * the value of parameter "(InternetGateway)Device.ManagementServer.SavedEvents".
     */
    for( i = 0; i < pMyObject->EventCount; i ++)
    { 
        pCcspCwmpEvent = (PCCSP_CWMP_EVENT)pMyObject->EventArray[i];

        if( CcspCwmpIsUndiscardedEvent(pCcspCwmpEvent->EventCode))
        {
            if( pCurBuf != pSavedEvents)
            {
                pCurBuf[0] = ',';
                pCurBuf ++;
            }

            uLength = AnscSizeOfString(pCcspCwmpEvent->EventCode);

            if( pCurBuf + uLength >= pSavedEvents + CCSP_CWMP_SAVED_EVENTS_MAX_LEN)
            {
                CcspTr069PaTraceWarning(("Too many events to save, ignored.\n"));

                break;
            }

            AnscCopyMemory(pCurBuf, pCcspCwmpEvent->EventCode, uLength);
            pCurBuf   += uLength;
            pCurBuf[0]=  '+';
            pCurBuf   ++;

            if( pCcspCwmpEvent->CommandKey )
            {
                uLength = AnscSizeOfString(pCcspCwmpEvent->CommandKey);

                if( uLength > 0)
                {
                    if( pCurBuf + uLength >= pSavedEvents + CCSP_CWMP_SAVED_EVENTS_MAX_LEN)
                    {
                        CcspTr069PaTraceWarning(("Too many events to save, ignored.\n"));

                        break;
                    }

                    AnscCopyMemory(pCurBuf, pCcspCwmpEvent->EventCode, uLength);
                    pCurBuf   += uLength;
                }
            }
        }
    }

    if( AnscSizeOfString(pSavedEvents) > 0)
    {
        char                        psmKeyPrefixed[CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN + 16];

        CcspCwmpPrefixPsmKey(psmKeyPrefixed, pCcspCwmpCpeController->SubsysName, CCSP_TR069PA_PSM_KEY_SavedEvents);

        CcspTr069PaTraceDebug(("Save the event to the registry before reboot:\n%s\n", pSavedEvents));

        returnStatus = 
            pCcspCwmpCpeController->SaveCfgToPsm
                (
                    (ANSC_HANDLE)pCcspCwmpCpeController,
                    psmKeyPrefixed,
                    pSavedEvents
                );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        CcspCwmpsoInformPending
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to check if the session has to 
        send inform immediately, probably due to an event has
        been generated.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

BOOL
CcspCwmpsoInformPending
    (
       ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;

    return  pMyObject->bInformWhenActive;
}


