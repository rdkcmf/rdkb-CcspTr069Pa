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

    module:	ccsp_cwmp_sesso_operation.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced operation functions
        of the CCSP CWMP Session Object.

        *   CcspCwmpsoAcqAccess
        *   CcspCwmpsoRelAccess
        *   CcspCwmpsoSessionTimerInvoke
        *   CcspCwmpsoSessionRetryTimerInvoke
        *   CcspCwmpsoDelayedActiveNotifTimerInvoke
        *   CcspCwmpsoStartRetryTimer
        *   CcspCwmpsoStopRetryTimer
        *   CcspCwmpsoCancelRetryDelay
        *   CcspCwmpsoStopDelayedActiveNotifTimer

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
#include "ansc_wrapper_base.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoAcqAccess
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to acquire access to this object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoAcqAccess
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;

    AnscAcquireLock(&pMyObject->AccessLock);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoRelAccess
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to release access to this object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoRelAccess
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;

    AnscReleaseLock(&pMyObject->AccessLock);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoSessionTimerInvoke
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called only if the ACS didn't respond in time
        and we have to close the HTTP session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoSessionTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;

    CcspTr069PaTraceDebug(("CcspCwmpsoSessionTimerInvoke - CWMP session timed out\n"));

    pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_reconnect;

    return
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoSessionRetryTimerInvoke
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called only after certain timer to start
        a retry session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoSessionRetryTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;

    CcspTr069PaTraceDebug(("Start the session retry at time %u...\n", (unsigned int)AnscGetTickInSeconds()));

    pMyObject->StopRetryTimer(pMyObject);

    pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_connectNow;
    
    return
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoDelayedActiveNotifTimerInvoke
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to send delayed inform to ACS which
        was triggered by "4 VALUE CHANGE" on changed parameter whose
        notification was set to active.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoDelayedActiveNotifTimerInvoke
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;

    CcspTr069PaTraceDebug(("CcspCwmpsoDelayedActiveNotifTimerInvoke - resuming delayed active notification ...\n"));

	pMyObject->bDelayedActiveNotifTimerScheduled = FALSE;

    pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_connectNow;

    return
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoStartRetryTimer
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to set a timer to retry the session

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

extern CCSP_VOID
CcspCwmpsoStartRetryTimerCustom
    (
        PCCSP_CWMP_SESSION_OBJECT                 pMyObject
    );
    
ANSC_STATUS
CcspCwmpsoStartRetryTimer
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject           = (PCCSP_CWMP_SESSION_OBJECT       )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCwmpCpeCotroller   = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_CFG_INTERFACE         pCcspCwmpCfgIf      = (PCCSP_CWMP_CFG_INTERFACE        )pCwmpCpeCotroller->GetCcspCwmpCfgIf((ANSC_HANDLE)pCwmpCpeCotroller);
    PCCSP_CWMP_STAT_INTERFACE        pCcspCwmpStatIf     = (PCCSP_CWMP_STAT_INTERFACE       )pCwmpCpeCotroller->hCcspCwmpStaIf;
    PANSC_TIMER_DESCRIPTOR_OBJECT    pRetryTimerObj      = (PANSC_TIMER_DESCRIPTOR_OBJECT   )pMyObject->hRetryTimerObj;
    ULONG                            uRandom             = 0;
    ULONG                            uTimer              = 0;

    /*
     *  Discard unnecessary events before retry;
     */
    pMyObject->DiscardCwmpEvent(pMyObject, CCSP_CWMPSO_EVENTCODE_ConnectionRequest);

    if( pMyObject->EventCount == 0)
    {
        CcspTr069PaTraceWarning(("There's no event in the array, don't have to retry the session.\n"));
    }
    else if (FALSE) /* use 900 seconds is not a good idea */
    {
        pMyObject->RetryCount++;
        uTimer = 900;

        CcspTr069PaTraceWarning
			((
				"Try to reconnect the ACS after %u seconds...RetryCount = %u\n", 
				(unsigned int)uTimer, 
				(unsigned int)pMyObject->RetryCount
			));

        pRetryTimerObj->SetInterval((ANSC_HANDLE)pRetryTimerObj,  uTimer * 1000);
        pRetryTimerObj->Start((ANSC_HANDLE)pRetryTimerObj);
    }
    else
    {

        pMyObject->RetryCount++;
        
        if( pCcspCwmpStatIf)
        {
            pCcspCwmpStatIf->IncTcpFailure(pCcspCwmpStatIf->hOwnerContext);
        }

        if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetRetryInformDelay )
        {
            //uTimer = 10;
//#if 0
            uTimer =
                pCcspCwmpCfgIf->GetRetryInformDelay
                    (
                        pCcspCwmpCfgIf->hOwnerContext,
                        pMyObject->RetryCount
                    );

            if ( uTimer == 0xFFFFFFFF )
            {
	      //Custom
	      CcspCwmpsoStartRetryTimerCustom(pMyObject);
              return ANSC_STATUS_SUCCESS;
            }
//#endif
        }
        else
        {
            ULONG                        ulSeed = AnscGetTickInMilliSeconds();

            srand(ulSeed);
            uRandom      = _ansc_rand();

            /*
             *  WT151
             *  Session Retry Wait Intervals Table
             *  -------------------------------------------------------------------------------------
             *  Post reboot session retry count | Wait interval range (min-max seconds)
             *  -------------------------------------------------------------------------------------
             *  #1                              | 5-10
             *  #2                              | 10-20
             *  #3                              | 20-40
             *  #4                              | 40-80
             *  #5                              | 80-160
             *  #6                              | 160-320
             *  #7                              | 320-640
             *  #8                              | 640-1280
             *  #9                              | 1280-2560
             *  #10 and subsequent              | 2560-5120
             *  -------------------------------------------------------------------------------------
             */
            switch ( pMyObject->RetryCount)
            {

                case  0:
                case  1:    uTimer =  5;
                            break;

                case  2:    uTimer =  10  + uRandom % 5;
                            break;

                case  3:    uTimer =  20  + uRandom % 15;
                            break;

                case  4:    uTimer =  40  + uRandom % 35;
                            break;

                case  5:    uTimer =  80  + uRandom % 70;
                            break;

                case  6:    uTimer =  160 + uRandom % 150;
                            break;

                case  7:    uTimer =  320 + uRandom % 300;
                            break;

                case  8:    uTimer =  640 + uRandom % 620;
                            break;

                case  9:    uTimer =  1280+ uRandom % 1260;
                            break;

                default:    uTimer =  2560+ uRandom % 2540;
                            break;
            }
        }

        CcspTr069PaTraceWarning
			((
				"Try to reconnect the ACS after %u seconds...RetryCount = %u, current time = %u\n", 
				(unsigned int)uTimer, 
				(unsigned int)pMyObject->RetryCount, 
				(unsigned int)AnscGetTickInSeconds()
			));

        pRetryTimerObj->SetInterval((ANSC_HANDLE)pRetryTimerObj,  uTimer * 1000);
        pRetryTimerObj->Start((ANSC_HANDLE)pRetryTimerObj);

    }

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoStopRetryTimer
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to stop the session retry timer;
        a retry session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoStopRetryTimer
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pRetryTimerObj     = (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hRetryTimerObj;

    if( pRetryTimerObj != NULL)
    {
        pRetryTimerObj->Stop((ANSC_HANDLE)pRetryTimerObj);
    }

    return ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoCancelRetryDelay
            (
                ANSC_HANDLE                 hThisObject,
                BOOL                        bConnectAgain
            );

    description:

        This function is called to cancel retry delay and start
        inform immediately upon receiving request explicitly
        from other means.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                BOOL                        bConnectAgain
                To connection to ACS again right away.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoCancelRetryDelay
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bConnectAgain
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;

    pMyObject->StopRetryTimer((ANSC_HANDLE)pMyObject);

    /* reset RetryCount */
    pMyObject->RetryCount = 0;

    if ( pMyObject->EventCount == 0 )   /* no event */
    {
        return  ANSC_STATUS_SUCCESS;
    }

    /* In case ACS makes connection again right after it
     * sends back '204 No Content' response to current session,
     * we need to make sure if the current session has been
     * torn down or not, if not, we don't need to do anything  */
    if ( bConnectAgain && pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_idle )
    {
        /* start Inform immediately */
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_connectNow;

        return
            pCcspCwmpProcessor->SignalSession
                (
                    (ANSC_HANDLE)pCcspCwmpProcessor,
                    (ANSC_HANDLE)pMyObject
                );
    }

    return ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoStopDelayedActiveNotifTimer
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to stop the session retry timer;
        a retry session.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoStopDelayedActiveNotifTimer
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT       pMyObject       = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PANSC_TIMER_DESCRIPTOR_OBJECT   pDelayedActiveNotifTimerObj     
													= (PANSC_TIMER_DESCRIPTOR_OBJECT)pMyObject->hDelayedActiveNotifTimerObj;

	pMyObject->bDelayedActiveNotifTimerScheduled = FALSE;

    if ( pDelayedActiveNotifTimerObj )
    {
        pDelayedActiveNotifTimerObj->Stop((ANSC_HANDLE)pDelayedActiveNotifTimerObj);
    }

    return ANSC_STATUS_SUCCESS;
}



