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

    module:	ccsp_cwmp_proco_process.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced process functions
        of the CcspCwmp Wmp Processor Object.

        *   CcspCwmppoConfigPeriodicInform
        *   CcspCwmppoScheduleInform
        *   CcspCwmppoSignalSession
        *   CcspCwmppoAsyncConnectTask

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
        06/22/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_proco_global.h"



#ifndef  _CCSP_TR069PA_ACTIVE_DURING_REBOOT
ANSC_STATUS
CcspCwmppoStopCwmpTask
    (
        ANSC_HANDLE                 hCtx
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject              = (PCCSP_CWMP_PROCESSOR_OBJECT     )hCtx;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;

    AnscSleep(2000);

    if ( pCcspCwmpCpeController )
        pCcspCwmpCpeController->StopCWMP((ANSC_HANDLE)pCcspCwmpCpeController);

    pCcspCwmpCpeController->bCWMPStopping = FALSE;

    return  ANSC_STATUS_SUCCESS;
}
#endif

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoConfigPeriodicInform
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to configure the periodic inform.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoConfigPeriodicInform
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject          = (PCCSP_CWMP_PROCESSOR_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_PROPERTY    pProperty          = (PCCSP_CWMP_PROCESSOR_PROPERTY )&pMyObject->Property;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pPeriodicTimerObj  = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hPeriodicTimerObj;
    ULONG                           ulTimeReferenceSec = (ULONG                        )0;
    ULONG                           ulCurrentTimeSec   = (ULONG                        )0;
    ANSC_UNIVERSAL_TIME             system_time;
    ANSC_UNIVERSAL_TIME             temp_time;

    if( pProperty->bPeriodicInformEnabled)
    {
        CcspTr069PaTraceDebug(("**********************\nConfigPeriodicInform is called: periodicInfrom is enabled and interval = %u seconds.\n**********************\n", (unsigned int)pProperty->PeriodicInformInterval));
        if ( pProperty->PeriodicInformInterval == 0 )
        {
            CcspTr069PaTraceWarning(("Internal error - periodic inform interval cannot be 0, ignored!\n"));
            return  ANSC_STATUS_INTERNAL_ERROR;
        } 
    }
    else
    {
        CcspTr069PaTraceDebug(("**********************\nConfigPeriodicInform is called: periodicInfrom is disabled and interval = %u seconds.\n**********************\n", (unsigned int)pProperty->PeriodicInformInterval));
    }

    pProperty->FirstTimeInterval = pProperty->PeriodicInformInterval;
    pProperty->FollowingInterval = pProperty->PeriodicInformInterval;

    if ( !pProperty->bPeriodicInformEnabled )
    {
        pPeriodicTimerObj->Stop((ANSC_HANDLE)pPeriodicTimerObj);

        return  ANSC_STATUS_SUCCESS;
    }
    else if ( (pProperty->PeriodicInformTime.Year       == 0) &&
              (pProperty->PeriodicInformTime.Month      == 0) &&
              (pProperty->PeriodicInformTime.DayOfMonth == 0) &&
              (pProperty->PeriodicInformTime.Hour       == 0) &&
              (pProperty->PeriodicInformTime.Minute     == 0) &&
              (pProperty->PeriodicInformTime.Second     == 0) )
    {
        /*
         * A zero dateTime value (0000-00-00T00:00:00) indicates that no particular time reference
         * is specified. That is, the CPE may locally choose the time reference, required only to
         * adhere to the specified PeriodicInformInterval.
         */
        pProperty->FirstTimeInterval = pProperty->PeriodicInformInterval;
        pProperty->FollowingInterval = pProperty->PeriodicInformInterval;
    }
    else if ( (pProperty->PeriodicInformTime.Year       == 1) &&
              (pProperty->PeriodicInformTime.Month      == 1) &&
              (pProperty->PeriodicInformTime.DayOfMonth == 1) &&
              (pProperty->PeriodicInformTime.Hour       == 0) &&
              (pProperty->PeriodicInformTime.Minute     == 0) &&
              (pProperty->PeriodicInformTime.Second     == 0) )
    {
        /*
         * A zero dateTime value (0000-00-00T00:00:00) indicates that no particular time reference
         * is specified. That is, the CPE may locally choose the time reference, required only to
         * adhere to the specified PeriodicInformInterval.
         */
        pProperty->FirstTimeInterval = pProperty->PeriodicInformInterval;
        pProperty->FollowingInterval = pProperty->PeriodicInformInterval;
    }
    else
    {
        if( pProperty->PeriodicInformTime.Year < 1000)
        {
            /* relative time */
            ulCurrentTimeSec = AnscGetTickInSeconds();

            AnscCopyMemory(&temp_time, (ANSC_HANDLE)&pProperty->PeriodicInformTime, sizeof(ANSC_UNIVERSAL_TIME));
            temp_time.Year           -= 1;
            temp_time.Month          -= 1;
            temp_time.DayOfMonth     -= 1;
            ulTimeReferenceSec = AnscCalendarToSecondFromEpoch((ANSC_HANDLE)&temp_time);
        }
        else
        {
            /* absolute time */
            ulTimeReferenceSec = AnscCalendarToSecondFromEpoch((ANSC_HANDLE)&pProperty->PeriodicInformTime);

            AnscGetSystemTime(&system_time);
            ulCurrentTimeSec = AnscCalendarToSecondFromEpoch((ANSC_HANDLE)&system_time);
        }

        CcspTr069PaTraceDebug(("ulCurrentTimeSec = %u, ulTimeReferenceSec = %u\n", (unsigned int)ulCurrentTimeSec, (unsigned int)ulTimeReferenceSec));
        
        if ( ulCurrentTimeSec >= ulTimeReferenceSec )
        {
            pProperty->FirstTimeInterval = (ulCurrentTimeSec - ulTimeReferenceSec) % pProperty->PeriodicInformInterval;
            pProperty->FirstTimeInterval = pProperty->PeriodicInformInterval - pProperty->FirstTimeInterval;
        }
        else
        {
            pProperty->FirstTimeInterval = (ulTimeReferenceSec - ulCurrentTimeSec) % pProperty->PeriodicInformInterval;
        }
    }

    CcspTr069PaTraceDebug(("ConfigPeriodicInform is called: first periodic inform will be sent in %u seconds.\n", (unsigned int)pProperty->FirstTimeInterval));


    if ( pProperty->FirstTimeInterval != pProperty->FollowingInterval )
    {
        pPeriodicTimerObj->Stop        ((ANSC_HANDLE)pPeriodicTimerObj);
        pPeriodicTimerObj->SetTimerType((ANSC_HANDLE)pPeriodicTimerObj, ANSC_TIMER_TYPE_SPORADIC           );
        pPeriodicTimerObj->SetInterval ((ANSC_HANDLE)pPeriodicTimerObj, pProperty->FirstTimeInterval * 1000);
        pPeriodicTimerObj->Start       ((ANSC_HANDLE)pPeriodicTimerObj);
    }
    else
    {
        pPeriodicTimerObj->Stop        ((ANSC_HANDLE)pPeriodicTimerObj);
        pPeriodicTimerObj->SetTimerType((ANSC_HANDLE)pPeriodicTimerObj, ANSC_TIMER_TYPE_PERIODIC           );
        pPeriodicTimerObj->SetInterval ((ANSC_HANDLE)pPeriodicTimerObj, pProperty->FollowingInterval * 1000);
        pPeriodicTimerObj->Start       ((ANSC_HANDLE)pPeriodicTimerObj);
    }

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoScheduleInform
            (
                ANSC_HANDLE                 hThisObject,
                ULONG                       ulDelaySeconds,
                char*                       pCommandKey
            );

    description:

        This function is called to schedule an Inform() method call.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ULONG                       ulDelaySeconds
                Specifies the number of seconds from the time this
                method is called to the time the CPE is requested to
                initiate a one-time inform method call.

                char*                       pCommandKey
                Specifies the string to return in CommandKey element
                of the InformStruct when the CPE calls the Inform
                method.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoScheduleInform
    (
        ANSC_HANDLE                 hThisObject,
        ULONG                       ulDelaySeconds,
        char*                       pCommandKey
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject         = (PCCSP_CWMP_PROCESSOR_OBJECT   )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pScheduleTimerObj = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hScheduleTimerObj;

    if ( pMyObject->SecheduledCommandKey )
    {
        AnscFreeMemory(pMyObject->SecheduledCommandKey);

        pMyObject->SecheduledCommandKey = NULL;
    }

    pMyObject->SecheduledCommandKey = AnscCloneString(pCommandKey);

    pScheduleTimerObj->SetInterval((ANSC_HANDLE)pScheduleTimerObj, ulDelaySeconds * 1000);
    pScheduleTimerObj->Start      ((ANSC_HANDLE)pScheduleTimerObj);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoSignalSession
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hWmpSession
            );

    description:

        This function is called when the state of the specified session
        is changed and requires some action to be taken.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hWmpSession
                Specifies the session whose state has updated.

    return:     status of operation.

**********************************************************************/


#define	 CCSP_CWMP_SESSION_STATE_ID_TO_STRING(id, string)					\
	do {																	\
		string = "Unknown";													\
		switch (id)															\
		{																	\
			case	CCSP_CWMPSO_SESSION_STATE_idle:							\
					string = "Idle";										\
					break;													\
																			\
			case	CCSP_CWMPSO_SESSION_STATE_connectNow:					\
					string = "ConnectNow";  								\
					break;													\
																			\
			case	CCSP_CWMPSO_SESSION_STATE_connected:					\
					string = "Connected";									\
					break;													\
																			\
			case	CCSP_CWMPSO_SESSION_STATE_informed:						\
					string = "Informed";      								\
					break;													\
																			\
			case	CCSP_CWMPSO_SESSION_STATE_disconnectNow:				\
					string = "DisconnectNow";								\
					break;													\
																			\
			case	CCSP_CWMPSO_SESSION_STATE_abort:						\
					string = "Abort";										\
					break;													\
																			\
			case	CCSP_CWMPSO_SESSION_STATE_timeout:						\
					string = "Timeout";										\
					break;													\
																			\
			case	CCSP_CWMPSO_SESSION_STATE_reconnect:					\
					string = "Reconnect";									\
					break;													\
																			\
		}																	\
	} while (0)


ANSC_STATUS
CcspCwmppoSignalSession
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpSession
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject       = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_SESSION_OBJECT       pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT    )hWmpSession;
	char*							pState			= NULL;
			
	CCSP_CWMP_SESSION_STATE_ID_TO_STRING(pCcspCwmpSession->SessionState, pState);

	CcspTr069PaTraceDebug(("CcspCwmppoSignalSession - session state is set to <%s>.\n", pState));

	pCcspCwmpSession->StopDelayedActiveNotifTimer((ANSC_HANDLE)pCcspCwmpSession);

    AnscSetEvent(&pMyObject->AsyncConnectEvent);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmppoAsyncConnectTask
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This task is created to initiate and management the HTTP/SOAP
        connections to ACS.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmppoAsyncConnectTask
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject          = (PCCSP_CWMP_PROCESSOR_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT )pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_STAT_INTERFACE       pCcspCwmpStatIf    = (PCCSP_CWMP_STAT_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpStaIf;
    PCCSP_CWMP_CFG_INTERFACE        pCcspCwmpCfgIf     = (PCCSP_CWMP_CFG_INTERFACE        )pCcspCwmpCpeController->hCcspCwmpCfgIf;
    PCCSP_CWMP_SESSION_OBJECT       pCcspCwmpSession   = (PCCSP_CWMP_SESSION_OBJECT    )NULL;
    PCCSP_CWMP_PROCESSOR_PROPERTY   pProperty          = (PCCSP_CWMP_PROCESSOR_PROPERTY )&pMyObject->Property;
    ULONG                           ulEventTimeout     = CCSP_CWMPSO_SESSION_TIMEOUT * 1000;
    ULONG                           ulErrorCode        = 0;
	ULONG							ulRpcTimeout       = 0;

	if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCwmpRpcTimeout )
	{
		ulRpcTimeout = pCcspCwmpCfgIf->GetCwmpRpcTimeout(pCcspCwmpCfgIf->hOwnerContext);

		if ( ulRpcTimeout > CCSP_CWMPSO_SESSION_TIMEOUT )
		{
			ulEventTimeout = ulRpcTimeout * 1000;
		}
	}

    while ( pMyObject->bActive )
    {
        /*
         * If there's no ASYNC_SESSION in the queue, we create one and wait for it to be notified...
         */
        AnscAcquireLock(&pMyObject->WmpsoQueueLock);

        if ( AnscQueueQueryDepth(&pMyObject->WmpsoQueue) == 0 )
        {
            pCcspCwmpSession =
                (PCCSP_CWMP_SESSION_OBJECT)CcspCwmpCreateWmpSession
                    (
                        pMyObject->hContainerContext,
                        (ANSC_HANDLE)pMyObject,
                        (ANSC_HANDLE)NULL
                    );

            if ( !pCcspCwmpSession )
            {
                AnscReleaseLock(&pMyObject->WmpsoQueueLock);

                break;
            }
            else
            {
                pCcspCwmpSession->SetCcspCwmpCpeController((ANSC_HANDLE)pCcspCwmpSession, pMyObject->hCcspCwmpCpeController);
                pCcspCwmpSession->SetCcspCwmpProcessor ((ANSC_HANDLE)pCcspCwmpSession, (ANSC_HANDLE)pMyObject       );
            }

            AnscQueuePushEntry(&pMyObject->WmpsoQueue, &pCcspCwmpSession->Linkage);
        }

        AnscReleaseLock(&pMyObject->WmpsoQueueLock);

        /*
         * Wait for the one of the causes to be justified...
         */
        AnscWaitEvent (&pMyObject->AsyncConnectEvent, ulEventTimeout);
        AnscResetEvent(&pMyObject->AsyncConnectEvent);

        if ( !pMyObject->bActive )
        {
            break;
        }
        else
        {
            pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pMyObject->AcqWmpSession((ANSC_HANDLE)pMyObject);

            /*
             * Depending on the different "SessionState", we may need to take different actions:
             *
             *      - "idle"         : do nothing, fall back to sleep
             *      - "connectNow"   : initiate the connection
             *      - "connected"    : do nothing, fall back to sleep
             *      - "disconnectNow": gracefully terminate the transaction
             *      - "abort"        : re-attempt the connection
             */
            if( CCSP_CWMPSO_SESSION_STATE_idle != pCcspCwmpSession->SessionState)
            {
                pCcspCwmpSession->StopRetryTimer(pCcspCwmpSession);
            }

            switch ( pCcspCwmpSession->SessionState )
            {
                case    CCSP_CWMPSO_SESSION_STATE_idle :

                        pMyObject->RelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);

                        break;

                case    CCSP_CWMPSO_SESSION_STATE_connectNow :

                        CcspTr069PaTraceDebug(("SessionState == connectNow. RetryCount = %u\n", (unsigned int)pCcspCwmpSession->RetryCount));

                        /* if ( pMyObject->IsCwmpEnabled((ANSC_HANDLE)pMyObject) ) */
                        {
                            returnStatus = pCcspCwmpSession->ConnectToAcs((ANSC_HANDLE)pCcspCwmpSession);

                            if ( returnStatus == ANSC_STATUS_SUCCESS )
                            {
                                if( pCcspCwmpStatIf)
                                {
                                    pCcspCwmpStatIf->ConnectAcs(pCcspCwmpStatIf->hOwnerContext);
                                }
                            }
                            else if ( returnStatus == ANSC_STATUS_NOT_READY )
                            {
                                /*
                                 * So the "(InternetGateway)Device.ManagementServer.URL" parameter hasn't
                                 * been configured yet, we should go back to sleep and wait for this
                                 * parameter to be configured by other means (e.g. via GUI or LAN-side
                                 * CPE Management Protocol).
                                 */
                                pCcspCwmpSession->SessionState = CCSP_CWMPSO_SESSION_STATE_idle;
                            }
                            else
                            {
                                if( pCcspCwmpStatIf)
                                {
                                    pCcspCwmpStatIf->ConnectAcs(pCcspCwmpStatIf->hOwnerContext);
                                }
                            }
                        }
                        /*
                        else
                        {
                            pCcspCwmpSession->SessionState = CCSP_CWMPSO_SESSION_STATE_idle;

                            CcspTr069PaTraceDebug(("TR069 (CWMP) is disabled.\n"));

                        }
						*/

                        pMyObject->RelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);

                        break;

                case    CCSP_CWMPSO_SESSION_STATE_connected :

                        CcspTr069PaTraceDebug(("SessionState == connected.\n"));

                        returnStatus =
                            pCcspCwmpSession->Inform
                                (
                                    (ANSC_HANDLE)pCcspCwmpSession,
                                    &ulErrorCode
                                );

                        pMyObject->RelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);

                        break;

                case    CCSP_CWMPSO_SESSION_STATE_informed :

                        CcspTr069PaTraceDebug(("SessionState == informed.\n"));

                        pMyObject->RelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);

                        /* re-deliver events if any */
                        pMyObject->RedeliverEvents(pMyObject);

                        break;

                case    CCSP_CWMPSO_SESSION_STATE_disconnectNow :

                        CcspTr069PaTraceDebug(("SessionState == disconnectNow.\n"));

                        pMyObject->DelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);

#ifndef  _CCSP_TR069PA_ACTIVE_DURING_REBOOT
                        if ( pCcspCwmpCpeController->bCpeRebooting )
                        {
                            AnscSpawnTask3
                                (
                                    (void*)CcspCwmppoStopCwmpTask,
                                    (ANSC_HANDLE)pMyObject,
                                    "CcspCwmppoStopCwmpTask",
                                    USER_DEFAULT_TASK_PRIORITY,
                                    USER_DEFAULT_TASK_STACK_SIZE
                                );
                        }

#endif
						
						pProperty->LastActiveNotifTime = AnscGetTickInSeconds();

                        /* if an event was generated during previous session,
                         * we need to start a new session right away, or with
                         * some delay.
                         */
                        if ( !pCcspCwmpCpeController->bCpeRebooting )
                        {
                            if ( pCcspCwmpCpeController->bCWMPStopping )
                            {
                                AnscSpawnTask3
                                    (
                                        (void*)CcspCwmppoStopCwmpTask,
                                        (ANSC_HANDLE)pMyObject,
                                        "CcspCwmppoStopCwmpTask",
                                        USER_DEFAULT_TASK_PRIORITY,
                                        USER_DEFAULT_TASK_STACK_SIZE
                                    );
                            }
                            else 
                            {
                                BOOL    bInformPending;
                                bInformPending = pMyObject->HasPendingInform((ANSC_HANDLE)pMyObject);

                                if ( bInformPending )
                                {
                                    PANSC_TIMER_DESCRIPTOR_OBJECT   pPendingInformTimerObj  = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hPendingInformTimerObj;

                                    pPendingInformTimerObj->Start((ANSC_HANDLE)pPendingInformTimerObj);
                                }
                            }
                        }

                        break;

                case    CCSP_CWMPSO_SESSION_STATE_abort   :
                case    CCSP_CWMPSO_SESSION_STATE_timeout :

                        CcspTr069PaTraceDebug(("====================\nTR069 SessionState == abort or timeout.\n====================\n"));

                        /* the old version is to Delete this session */
                        /* pMyObject->DelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession); */

                        /* I think we should take it as "Reconnect" since retry mechanism applies as well */
                        pCcspCwmpSession->CloseConnection((ANSC_HANDLE)pCcspCwmpSession);
                        pCcspCwmpSession->StartRetryTimer((ANSC_HANDLE)pCcspCwmpSession);


                        pMyObject->RelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);
                        pMyObject->SignalSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);

                        break;

                case    CCSP_CWMPSO_SESSION_STATE_reconnect :


                        CcspTr069PaTraceDebug(("====================\nTR069 SessionState == reconnect.\n====================\n"));

                        CcspTr069PaTraceDebug(("Reconnect - CloseConnection.\n"));
                        pCcspCwmpSession->CloseConnection((ANSC_HANDLE)pCcspCwmpSession);
                        CcspTr069PaTraceDebug(("Reconnect - StartRetryTimer.\n"));
                        pCcspCwmpSession->StartRetryTimer((ANSC_HANDLE)pCcspCwmpSession);

                        CcspTr069PaTraceDebug(("Reconnect - RelWmpSession.\n"));
                        pMyObject->RelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);
                        CcspTr069PaTraceDebug(("Reconnect - SignalSession.\n"));
                        
                        pMyObject->SignalSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);
                        CcspTr069PaTraceDebug(("Reconnect - Reset the Timeout.\n"));

                        break;

                default :

                        CcspTr069PaTraceDebug(("Unknown Session state : %d\n", (int)pCcspCwmpSession->SessionState));

                        pMyObject->RelWmpSession((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pCcspCwmpSession);

                        break;
            }
        }
    }

    /*
     * Release all the async connects...
     */
    pMyObject->DelAllSessions((ANSC_HANDLE)pMyObject);

    pMyObject->AsyncTaskCount--;

    return  ANSC_STATUS_SUCCESS;
}
