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

    module:	ccsp_cwmp_sesso_management.c

        For CCSP CWMP protocol implementation

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
    errno_t   rc  =   -1;
    int ind = -1;


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
                    rc = strcmp_s(pCcspCwmpEvent->EventCode, strlen(pCcspCwmpEvent->EventCode), pCcspCwmpEventExist->EventCode,&ind);
                    ERR_CHK(rc);
                 if((!ind) && (rc == EOK))
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
            BOOL check = FALSE;
            for( i = 0; i < pMyObject->EventCount; i ++)
            {
                pCcspCwmpEventExist = (PCCSP_CWMP_EVENT)pMyObject->EventArray[i];
                check = FALSE;
                rc = strcmp_s(pCcspCwmpEvent->EventCode, strlen(pCcspCwmpEvent->EventCode), pCcspCwmpEventExist->EventCode, &ind);
                ERR_CHK(rc);                
                if((rc == EOK) && (!ind))
                {
                    if((!pCcspCwmpEvent->CommandKey && !pCcspCwmpEventExist->CommandKey))
                    {
                        check = TRUE;
                    }
                }
                if(check == FALSE)
                {
                    if(pCcspCwmpEvent->CommandKey && pCcspCwmpEventExist->CommandKey)
                    {
                        rc = strcmp_s(pCcspCwmpEvent->CommandKey, strlen(pCcspCwmpEvent->CommandKey), pCcspCwmpEventExist->CommandKey, &ind);
                        ERR_CHK(rc);
                        if(rc == EOK && (!ind))
                        {
                            check = TRUE;
                        }
                    }
                }
                if(check == TRUE)
                {
                    bExist = TRUE;

                    CcspTr069PaTraceWarning(("The event '%s' with CommandKey '%s' is already there, discarded.\n", pCcspCwmpEvent->EventCode, pCcspCwmpEvent->CommandKey));
                    break;
                }
            }
        }
        if( !bExist )
        {
  		    pMyObject->EventArray[pMyObject->EventCount++] = (ANSC_HANDLE)pCcspCwmpEvent;
            CcspTr069PaTraceDebug(("<RT> Event '%s' with CommandKey '%s' added at location '%lu'\n",
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
                   pProperty->DefActiveNotifThrottle != 0 && 
                   pProperty->LastActiveNotifTime != 0 )
                {
                  rc = strcmp_s(CCSP_CWMP_INFORM_EVENT_NAME_ValueChange,strlen(CCSP_CWMP_INFORM_EVENT_NAME_ValueChange),pFirstEvent->EventCode,&ind);
                  ERR_CHK(rc);
                  if((rc == EOK) && (!ind))
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
 
			        CcspTr069PaTraceDebug(("Active notification will be delayed by %lu seconds\n", ulInterval));

                                if ( !pMyObject->bDelayedActiveNotifTimerScheduled )
                                {
		                    pDelayedActiveNotifTimerObj->SetInterval((ANSC_HANDLE)pDelayedActiveNotifTimerObj, ulInterval * 1000);
	    	                    pDelayedActiveNotifTimerObj->Start((ANSC_HANDLE)pDelayedActiveNotifTimerObj);
    			            pMyObject->bDelayedActiveNotifTimerScheduled = TRUE;
                                }
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
    errno_t rc = -1,rc1 = -1;
    int ind =-1,ind1 = -1;

    /*
     * According to WT151, only the event "6 CONNECTIONREQUEST" must NOT retry delivery and it's a
     * single type event.
     *
     * Let's find and discard it;   
     */
    for( i = 0; i < pMyObject->EventCount; i ++)
    { 
        pCcspCwmpEvent = (PCCSP_CWMP_EVENT)pMyObject->EventArray[i];

  rc = strcmp_s(CCSP_CWMP_INFORM_EVENT_NAME_ConnectionRequest,strlen(CCSP_CWMP_INFORM_EVENT_NAME_ConnectionRequest),pCcspCwmpEvent->EventCode,&ind);
  ERR_CHK(rc);
  rc1 = strcmp_s(CCSP_CWMP_INFORM_EVENT_NAME_ValueChange,strlen(CCSP_CWMP_INFORM_EVENT_NAME_ValueChange),pCcspCwmpEvent->EventCode,&ind1);
  ERR_CHK(rc1);
        if ( ( EventCode == CCSP_CWMPSO_EVENTCODE_ConnectionRequest &&
             ((!ind) && (rc == EOK)) ) ||
             ( EventCode == CCSP_CWMPSO_EVENTCODE_ValueChange &&
             ((!ind1) && (rc1 == EOK)) ) )

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
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
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
    ANSC_STATUS                         returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT           pMyObject               = (PCCSP_CWMP_SESSION_OBJECT)hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT    pCcspCwmpCpeController  = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT         pCcspCwmpProcessor      = (PCCSP_CWMP_PROCESSOR_OBJECT) pMyObject->hCcspCwmpProcessor;
    ULONG   i       = 0;
    ULONG   bExist  = FALSE;
    errno_t rc      = -1;
    int     ind     = -1;

    CcspTr069PaTraceDebug(("AddModifiedParameter - '%s'\n", pParamName));

    if ( pMyObject->ModifiedParamCount >= CCSP_CWMPSO_MAX_PARAM_NUMBER )
    {
        AnscFreeMemory(pParamName);

        return  ANSC_STATUS_RESOURCES;
    }

    if (pCcspCwmpProcessor != NULL && CCSP_CWMP_NOTIFICATION_off == pCcspCwmpProcessor->CheckParamAttrCache((ANSC_HANDLE)pCcspCwmpProcessor, pParamName))
    {
        CcspTr069PaTraceDebug(("Modified parameter '%s' not added as Notification='%d'\n",pParamName,CCSP_CWMP_NOTIFICATION_off));
        return ANSC_STATUS_FAILURE;
    }

    for ( i = 0; i < pMyObject->ModifiedParamCount; i++ )
    {
        if ( pMyObject->ModifiedParamArray[i] )
        {
            rc = strcmp_s(pMyObject->ModifiedParamArray[i],sizeof(pMyObject->ModifiedParamArray[i]),pParamName,&ind);
            ERR_CHK(rc);
            if((!ind) && (rc == EOK))
            {
                bExist = TRUE;

                if ( pMyObject->ModifiedParamValueArray[i] )
                {
                    AnscFreeMemory(pMyObject->ModifiedParamValueArray[i]);
                }

                pMyObject->ModifiedParamValueArray[i] = AnscCloneString(pParamValue);
                pMyObject->ModifiedParamTypeArray [i] = CwmpDataType;

                break;
            }
        }
    }

    if( !bExist)
    {
        pMyObject->ModifiedParamArray     [pMyObject->ModifiedParamCount]   = AnscCloneString(pParamName);
        pMyObject->ModifiedParamValueArray[pMyObject->ModifiedParamCount]   = AnscCloneString(pParamValue);
        pMyObject->ModifiedParamTypeArray [pMyObject->ModifiedParamCount++] = CwmpDataType;
    }

    /*
     * Bin: We are supposed to add "ValueChange" event for passive notification as well.
     */
    if ( TRUE )
    {
        PCCSP_CWMP_EVENT            pCcspCwmpEvent = (PCCSP_CWMP_EVENT)AnscAllocateMemory(sizeof(CCSP_CWMP_EVENT));

        if ( !pCcspCwmpEvent )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pCcspCwmpEvent->EventCode  = AnscCloneString(CCSP_CWMP_INFORM_EVENT_NAME_ValueChange);
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
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    ULONG                           i                  = 0;

    for ( i = 0; i < pMyObject->ModifiedParamCount; i++ )
    {
        if ( pMyObject->ModifiedParamArray[i] )
        {
            AnscFreeMemory(pMyObject->ModifiedParamArray[i]);

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
    errno_t rc   = -1;
    int     ind  = -1;
    /*
     *  According to WT151, the events "2 PERIODIC", "3 SCHEDULED", "7 TRANSFERCOMPLETE"
     *  and the ones start with "M" won't be discarded
     */
    rc = strcmp_s(CCSP_CWMP_INFORM_EVENT_NAME_Peroidic,strlen(CCSP_CWMP_INFORM_EVENT_NAME_Peroidic), eventName,&ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        return TRUE;
    }
    else
    {
       rc =  strcmp_s(CCSP_CWMP_INFORM_EVENT_NAME_Scheduled,strlen(CCSP_CWMP_INFORM_EVENT_NAME_Scheduled), eventName,&ind);
       ERR_CHK(rc);
       if((!ind) && (rc == EOK))
       {
	   return TRUE;
       }
       else
       {
           rc = strcmp_s(CCSP_CWMP_INFORM_EVENT_NAME_TransferComplete,strlen(CCSP_CWMP_INFORM_EVENT_NAME_TransferComplete), eventName,&ind);
           ERR_CHK(rc);
	   if((!ind) && (rc == EOK))
	   {
	       return TRUE;
	   }
    	   else if(eventName[0] == 'M')
	   {
	       return TRUE;
           }
        }
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
    char                            pSavedEvents[CCSP_CWMP_SAVED_EVENTS_MAX_LEN] = { 0 };
    PCHAR                           pCurBuf            = pSavedEvents;
    ULONG                           i                  = 0;
    ULONG                           uLength            = 0;

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


