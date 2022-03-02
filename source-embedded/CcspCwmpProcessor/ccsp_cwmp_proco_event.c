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

    module:	ccsp_cwmp_proco_event.c

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This module implements event handling (value change)
        of the CCSP CWMP Processor Object.

        *   CcspCwmppoSysReadySignalCB
        *   CcspCwmppoDiagCompleteSignalCB
        *   CcspCwmppoParamValueChangedCB

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        07/28/2011  initial version 
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/

#include "ccsp_cwmp_proco_global.h"
#include <unistd.h>
#include "print_uptime.h"
#include <sys/stat.h>
extern ANSC_HANDLE bus_handle;

static
ANSC_STATUS
CcspCwmppoSysReadySignalProcTask
    (
        ANSC_HANDLE                 hTaskContext
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject              = (PCCSP_CWMP_PROCESSOR_OBJECT )hTaskContext;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault          = NULL;
    char*                           pConnReqUrl             = NULL;
    int                             nRetryCount             = 0;
    BOOL                            bHavePrintGottenIP      = FALSE;

    pMyObject->AsyncTaskCount ++;

    CcspTr069PaTraceError(("TR-069 PA received systemReady event!\n"));

    /* synchronize namespaces with CR */
    while ( !pMyObject->MsFcInitDone || !pCcspCwmpCpeController->bActive )
    {
        AnscSleep(1000);
        continue;
    }

    CcspTr069PaTraceError(("Synchronizing namespaces with CR ...\n"));

    returnStatus = pMyObject->SyncNamespacesWithCR((ANSC_HANDLE)pMyObject, FALSE);

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTr069PaTraceError(("CcspCwmppoSysReadySignalCB - failed to synchronize namespaces with CR.\n"));
    }
    
#ifdef   _CCSP_TR069PA_PUSH_VC_SIGNAL_ON_LOAD    
    returnStatus = pMyObject->PushAllVcToBackend((ANSC_HANDLE)pMyObject);
#endif

#ifndef CONFIG_CCSP_CWMP_SESSION_EVENT_TURN_OFF_MOINITOR_CB
    returnStatus =
        pCcspCwmpCpeController->MonitorOpState
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                TRUE,
                CCSP_NS_CHANGEDUSTATE,
                0,
                NULL,
                NULL, /* CCSP_NS_CDS_OPERATION_STATE */
                &pCwmpSoapFault
            );

    if ( returnStatus != ANSC_STATUS_SUCCESS && pCwmpSoapFault )
    {
        if ( returnStatus == ANSC_STATUS_NOT_SUPPORTED )
        {
            CcspTr069PaTraceWarning
                ((
                    "CcspCwmppoSysReadySignalCB - no component registered for Name Space: <%s>.\n",
                    CCSP_NS_CHANGEDUSTATE
                ));
        }
        else
        {
            CcspTr069PaTraceError
                ((
                    "CcspCwmppoSysReadySignalCB - failed to set ValueChange monitor on <%s>.\n",
                    CCSP_NS_CHANGEDUSTATE
                ));
        }

        if( pCwmpSoapFault )
        {
            CcspCwmpFreeSoapFault(pCwmpSoapFault);
            pCwmpSoapFault = NULL;
        }
    }
#endif
    /*returnStatus =
        pCcspCwmpCpeController->MonitorOpState
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                TRUE,
                CCSP_NS_DOWNLOAD,
                0,
                NULL,
                CCSP_NS_DOWNLOAD_STATE,
                &pCwmpSoapFault
            );

    if ( returnStatus != ANSC_STATUS_SUCCESS && pCwmpSoapFault )
    {
        CcspTr069PaTraceError
            ((
                "CcspCwmppoSysReadySignalCB - failed to set ValueChange monitor on <%s%s>.\n",
                CCSP_NS_DOWNLOAD,
                CCSP_NS_DOWNLOAD_STATE
            ));

        CcspCwmpFreeSoapFault(pCwmpSoapFault);
    }*/

#ifndef CONFIG_CCSP_CWMP_SESSION_EVENT_WAIT_FOR_CONN_REQ_URL
    /* put Inform message sending on hold until ConnectionRequestURL is ready (non-empty) */
    while ( pMyObject->bActive && !pConnReqUrl )
    {
        nRetryCount ++;

        pConnReqUrl = 
            CcspManagementServer_GetConnectionRequestURL
                (
                    pCcspCwmpCpeController->PANameWithPrefix
                );

        if ( pConnReqUrl )
        {
            bHavePrintGottenIP  = FALSE;
            CcspTr069PaFreeMemory(pConnReqUrl);
            break;
        }

        if ( !bHavePrintGottenIP && ( nRetryCount > 3 ) )
        {
            CcspTr069PaTraceError(("TR-069 PA failed to get ACS facing WAN IP address from PaM!\n"));
            bHavePrintGottenIP = TRUE;
            nRetryCount = 1;
        }
        AnscSleep(5000);
    }
#endif

    if ( pMyObject->bActive )
    {
        pCcspCwmpCpeController->StartCWMP((ANSC_HANDLE)pCcspCwmpCpeController, TRUE, FALSE);
    }

    pMyObject->AsyncTaskCount --;

    return  ANSC_STATUS_SUCCESS;
}


void 
CcspCwmppoSysReadySignalCB
    (
        void*                       user_data
    )
{
	// Touch a file to indicate that tr069 can proceed with further
	if (access("/var/tmp/tr069paready", F_OK) != 0)
	{
		print_uptime("boot_to_tr069_uptime",NULL, NULL);
	}
	creat("/var/tmp/tr069paready",S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	CcspTr069PaTraceInfo(("Received system ready signal, created /var/tmp/tr069paready file\n"));

	CcspTr069PaTraceInfo(("%s, user_data - 0x%p\n",
								__FUNCTION__, 
								(user_data != NULL) ? user_data : 0 ));

/* 
* This callback process mechanism moved into CcspCwmppoProcessSysReadySignal() 
* API. Due to system ready signal handling for TR069 restart process from selfheal scripts.
*/
#if 0
	AnscSpawnTask
        (
            CcspCwmppoSysReadySignalProcTask, 
            (ANSC_HANDLE)user_data, 
            "Tr069PaSysReadyProcTask"
        );
#endif /* 0 */
 
}

/**
 * @brief waitUntilSystemReady Function to wait until the system ready signal from CR is received.
 * This is to delay tr069pa start up until other components on stack are ready.
 */
void waitUntilSystemReady(	void*	cbContext)
{
	FILE *file;
	int wait_time = 0;

	// Check received cbContext is NULL or not. If NULL then skip further 
	// steps
	if( NULL == cbContext )
	{
		CcspTr069PaTraceInfo(("cbContext NULL\n"));
		return;
	}
		  
	// Wait till Call back touches the indicator to proceed further
	while((file = fopen("/var/tmp/tr069paready", "r")) == NULL)
	{
		CcspTr069PaTraceInfo(("Waiting for system ready signal\n"));
		// After waiting for 24 * 5 = 120s (2mins) send dbus message to CR to query for system ready
		if(wait_time == 24)
		{
			wait_time = 0;
			if(checkIfSystemReady())
			{
				CcspTr069PaTraceInfo(("Checked CR - System is ready, proceed with tr069 start up\n"));
				creat("/var/tmp/tr069paready",S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				/* The checkIfSystemReady() returns true, then the file pointer will be NULL.
				 * So, breaking from here will never start the CcspCwmppoProcessSysReadySignal();
				 * We must call from here as it is going to create a thread and return;
				 */
				CcspCwmppoProcessSysReadySignal( cbContext );
				break;
				//Break out, System ready signal already delivered
			}
			else
			{
				CcspTr069PaTraceInfo(("Queried CR for system ready after waiting for 2 mins, it is still not ready\n"));
			}
		}
		sleep(5);
		wait_time++;
	};
	// In case of tr069pa restart, we should be having tr069pa already touched.
	// In normal boot up we will reach here only when system ready is received.
	if(file != NULL)
	{
		CcspTr069PaTraceInfo(("/var/tmp/tr069paready file exists, hence can proceed with tr069pa start up\n"));
		fclose(file);

		// To process further in tr069 process after receving the system ready 
		// signal from CR
		CcspCwmppoProcessSysReadySignal( cbContext );
	}	
}

/**
 * @brief checkIfSystemReady Function to query CR and check if system is ready.
 * This is just in case tr069pa registers for the systemReadySignal event late.
 * If SystemReadySignal is already sent then this will return 1 indicating system is ready.
 */
int checkIfSystemReady(void)
{
    char str[256];
    unsigned int val;
    int ret;
    snprintf(str, sizeof(str), "eRT.%s", CCSP_DBUS_INTERFACE_CR);
    // Query CR for system ready
    ret = CcspBaseIf_isSystemReady(bus_handle, str, &val);
    CcspTr069PaTraceInfo(("checkIfSystemReady(): ret %d, val %u\n", ret, val));
    return (int)val;
}

void 
CcspCwmppoProcessSysReadySignal
    (
		void*	cbContext
    )
{
	CcspTr069PaTraceInfo(("%s, cbContext - 0x%p\n",
								__FUNCTION__, 
								(cbContext != NULL) ? cbContext : 0 ));
	if( NULL != cbContext )
	{
		AnscSpawnTask
			(
				CcspCwmppoSysReadySignalProcTask, 
				(ANSC_HANDLE)cbContext, 
				"Tr069PaSysReadyProcTask"
			);
	}
}

void 
CcspCwmppoDiagCompleteSignalCB
    (
        void*                       user_data
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject              = (PCCSP_CWMP_PROCESSOR_OBJECT )user_data;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_ACS_BROKER_OBJECT    pCcspCwmpAcsBroker      = (PCCSP_CWMP_ACS_BROKER_OBJECT)pCcspCwmpCpeController->hCcspCwmpAcsBroker;
    PCCSP_CWMP_MSO_INTERFACE        pCcspCwmpMsoIf          = (PCCSP_CWMP_MSO_INTERFACE    )pCcspCwmpAcsBroker->hCcspCwmpMsoIf;
           
    pCcspCwmpMsoIf->Inform
        (
            (ANSC_HANDLE)pCcspCwmpMsoIf->hOwnerContext,
            CCSP_CWMP_INFORM_EVENT_NAME_DiagnosticsComplete,
            NULL,
            pCcspCwmpCpeController->bBootInformSent
        );
}


#define  CcspCwmppoConstructCdsPsmKey(arg)                                              \
    do {                                                                                \
        if ( !pCcspCwmpCpeController->SubsysName )                                      \
            _ansc_sprintf                                                               \
                (                                                                       \
                    buf,                                                                \
                    "%s%u.%s",                                                          \
                    CCSP_NS_CHANGEDUSTATE,                                              \
                    (unsigned int)pInsNumbers[i],                                       \
                    arg                                                                 \
                );                                                                      \
        else                                                                            \
            _ansc_sprintf                                                               \
                (                                                                       \
                    buf,                                                                \
                    "%s%s%u.%s",                                                        \
                    pCcspCwmpCpeController->SubsysName,                                 \
                    CCSP_NS_CHANGEDUSTATE,                                              \
                    (unsigned int)pInsNumbers[i],                                       \
                    arg                                                                 \
                );                                                                      \
    } while (0) 


static
void
CcspCwmppoCheckCdsResults
    (
        PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController,
        char*                       pCommandKey
    )
{
    PCCSP_CWMP_MSO_INTERFACE        pCcspCwmpMsoIf      = (PCCSP_CWMP_MSO_INTERFACE)pCcspCwmpCpeController->GetCcspCwmpMsoIf(pCcspCwmpCpeController);
    PCCSP_CWMP_PROCESSOR_OBJECT     pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT   )pCcspCwmpCpeController->hCcspCwmpProcessor;
    unsigned int                    numInstances        = 0;
    unsigned int*                   pInsNumbers         = NULL;
    int                             psmStatus;
    int                             i, j;
    char                            buf[256];
    char*                           pCK;
    ULONG                           order;
    int                             total;
    char*                           pValue;
    int                             comp;
    PCCSP_TR069_DSCC_REQ            pDsccReq        = NULL;
    char**                          pParamNames     = NULL;
    char**                          pParamValues    = NULL;
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;

    CcspTr069PaTraceInfo
        ((
            "CDS - checking if CDS operation with Command Key <%s> completes.\n", 
            pCommandKey
        ));

    if ( !pCcspCwmpCpeController->SubsysName )
    {
        _ansc_sprintf(buf, "%s", CCSP_NS_CHANGEDUSTATE);
    }
    else
    {
        _ansc_sprintf(buf, "%s%s", pCcspCwmpCpeController->SubsysName, CCSP_NS_CHANGEDUSTATE);
    }

    psmStatus = 
        PsmGetNextLevelInstances
            (
                pCcspCwmpCpeController->hMsgBusHandle,
                pCcspCwmpCpeController->SubsysName,
                buf,
                &numInstances,
                &pInsNumbers
            );

    if ( psmStatus == CCSP_SUCCESS )
    {
        total = (int)numInstances;

        for ( i = total - 1; i >= 0; i -- )
        {
            if ( pInsNumbers[i] == 0 )
            {
                for ( j = i; (unsigned int)j < numInstances - 1; j ++ )
                {
                    pInsNumbers[j] = pInsNumbers[j+1];
                }

                total --;

                continue;
            }

            if ( !pCcspCwmpCpeController->SubsysName )
            {
                _ansc_sprintf(buf, "%s%u.", CCSP_NS_CHANGEDUSTATE, pInsNumbers[i]);
            }
            else
            {
                _ansc_sprintf(buf, "%s%s%u.", pCcspCwmpCpeController->SubsysName, CCSP_NS_CHANGEDUSTATE, pInsNumbers[i]);
            }

            pCK = NULL;

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        buf,
                        NULL,
                        &pCK
                    );

            if ( ( !pCK && pCommandKey )    || 
                 ( pCK && !pCommandKey )    ||
                 ( pCK && pCommandKey && !AnscEqualString(pCK, pCommandKey, TRUE) ) )
            {
                if ( pCK )
                {
                    CcspTr069PaFreeMemory(pCK);
                }

                for ( j = i; (unsigned int)j < numInstances - 1; j ++ )
                {
                    pInsNumbers[j] = pInsNumbers[j+1];
                }

                total --;

                continue;
            }

            if ( pCK )
            {
                CcspTr069PaFreeMemory(pCK);
            }

            CcspCwmppoConstructCdsPsmKey(CCSP_NS_CDS_PSM_NODE_COMPLETE);

            psmStatus = 
                PSM_Get_Record_Value2
                    (
                        pCcspCwmpCpeController->hMsgBusHandle,
                        pCcspCwmpCpeController->SubsysName,
                        buf,
                        NULL,
                        &pValue
                    );

            if ( pValue )
            {
                comp = _ansc_atoi(pValue);
                CcspTr069PaFreeMemory(pValue);
            }
            else
            {
                comp = 0;
            }

            if ( !comp )
            {
                /* operation not complete yet */
                break;
            }
        }

        if ( i < 0 && total > 0 )
        {
            /* operation completes, compose DSCC request */
            pDsccReq = (PCCSP_TR069_DSCC_REQ)CcspTr069PaAllocateMemory(sizeof(CCSP_TR069_DSCC_REQ));

            if ( !pDsccReq )
            {
                goto EXIT;
            }

            pDsccReq->Results = 
                (PCCSP_TR069_CDS_OpResult)CcspTr069PaAllocateMemory
                    (
                        sizeof(CCSP_TR069_CDS_OpResult) * total
                    );

            if ( !pDsccReq->Results )
            {
                goto EXIT;
            }

            pDsccReq->NumResults = total;

            pParamNames  = (char**)CcspTr069PaAllocateMemory(total * sizeof(char*) * CCSP_NS_CDS_RESULT_ARG_COUNT);
            if ( !pParamNames )
            {
                goto EXIT;
            }
            AnscZeroMemory(pParamNames, total * sizeof(char*) * CCSP_NS_CDS_RESULT_ARG_COUNT);

            pParamValues = (char**)CcspTr069PaAllocateMemory(total * sizeof(char*) * CCSP_NS_CDS_RESULT_ARG_COUNT);
            if ( !pParamValues )
            {
                goto EXIT;
            }
            AnscZeroMemory(pParamValues, total * sizeof(char*) * CCSP_NS_CDS_RESULT_ARG_COUNT);

            for ( i = 0; i < total; i ++ )
            {
                CcspCwmppoConstructCdsPsmKey(CCSP_NS_CDS_PSM_NODE_ORDER);

                pValue = NULL;
                psmStatus = 
                    PSM_Get_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            buf,
                            NULL,
                            &pValue
                        );

                if ( pValue )
                {
                    order = (ULONG)_ansc_atoi(pValue);
                }
                else
                {
                    order = 0;
                }

                CcspTr069PaTraceDebug
                    ((
                        "CDS - order of <%s> in PSM is %u.\n", 
                        buf, 
                        (unsigned int)order
                    ));

                if ( pValue )
                {
                    CcspTr069PaFreeMemory(pValue);
                }

                if ( (int)order >= total )
                {
                    goto EXIT;
                }

                order *= CCSP_NS_CDS_RESULT_ARG_COUNT;

                _ansc_sprintf(buf, "%s%u.%s", CCSP_NS_CHANGEDUSTATE, pInsNumbers[i], CCSP_NS_CDS_UUID);
                pParamNames[order++] = AnscCloneString(buf);

                _ansc_sprintf(buf, "%s%u.%s", CCSP_NS_CHANGEDUSTATE, pInsNumbers[i], CCSP_NS_CDS_DEPLOYMENT_UNIT_REF);
                pParamNames[order++] = AnscCloneString(buf);

                _ansc_sprintf(buf, "%s%u.%s", CCSP_NS_CHANGEDUSTATE, pInsNumbers[i], CCSP_NS_CDS_VERSION);
                pParamNames[order++] = AnscCloneString(buf);

                _ansc_sprintf(buf, "%s%u.%s", CCSP_NS_CHANGEDUSTATE, pInsNumbers[i], CCSP_NS_CDS_CURRENT_STATE);
                pParamNames[order++] = AnscCloneString(buf);

                _ansc_sprintf(buf, "%s%u.%s", CCSP_NS_CHANGEDUSTATE, pInsNumbers[i], CCSP_NS_CDS_RESOLVED);
                pParamNames[order++] = AnscCloneString(buf);

                _ansc_sprintf(buf, "%s%u.%s", CCSP_NS_CHANGEDUSTATE, pInsNumbers[i], CCSP_NS_CDS_EXECUTION_UNIT_REF_LIST);
                pParamNames[order++] = AnscCloneString(buf);

                _ansc_sprintf(buf, "%s%u.%s", CCSP_NS_CHANGEDUSTATE, pInsNumbers[i], CCSP_NS_CDS_START_TIME);
                pParamNames[order++] = AnscCloneString(buf);

                _ansc_sprintf(buf, "%s%u.%s", CCSP_NS_CHANGEDUSTATE, pInsNumbers[i], CCSP_NS_CDS_COMPLETE_TIME);
                pParamNames[order++] = AnscCloneString(buf);

                _ansc_sprintf(buf, "%s%u.%s", CCSP_NS_CHANGEDUSTATE, pInsNumbers[i], CCSP_NS_CDS_ERROR);
                pParamNames[order++] = AnscCloneString(buf);
            }

            for ( i = 0; i < total * CCSP_NS_CDS_RESULT_ARG_COUNT; i ++ )
            {
                if ( !pParamNames[i] )
                {
                    CcspTr069PaTraceError
                        ((
                            "CDS - sth. wrong with saved state monitors, order of operations messed.\n"
                        ));

                    goto EXIT;
                }
            }

            /* retreive result values */
            returnStatus = 
                pCcspCwmpCpeController->GetParamStringValues
                    (
                        (ANSC_HANDLE)pCcspCwmpCpeController,
                        pParamNames,
                        total * CCSP_NS_CDS_RESULT_ARG_COUNT,
                        pParamValues
                    );

            if ( returnStatus != ANSC_STATUS_SUCCESS )
            {
                CcspTr069PaTraceError
                    ((
                        "CDS - failed to retrieve results with CommandKey <%s>.\n", 
                        pCommandKey 
                    ));
            }
            else
            {
                int                     base;
                int                     ccspError;
                PCCSP_CWMP_SOAP_FAULT   pCwmpFault = NULL;
    
                for ( i = 0; i < total; i ++ )
                {
                    base = i * CCSP_NS_CDS_RESULT_ARG_COUNT;

                    pDsccReq->Results[i].Uuid = pParamValues[base];
                    pParamValues[base++] = NULL;

                    pDsccReq->Results[i].DeploymentUnitRef = pParamValues[base];
                    pParamValues[base++] = NULL;

                    pDsccReq->Results[i].Version = pParamValues[base];
                    pParamValues[base++] = NULL;

                    pDsccReq->Results[i].CurrentState = pParamValues[base];
                    pParamValues[base++] = NULL;

                    if ( AnscEqualString(pParamValues[base], "0", TRUE) ||
                         AnscEqualString(pParamValues[base], "false", FALSE) )
                    {
                        pDsccReq->Results[i].Resolved = 0;
                    }
                    else
                    {
                        pDsccReq->Results[i].Resolved = 1;
                    }
                    base++;

                    pDsccReq->Results[i].ExecutionUnitRefList = pParamValues[base];
                    pParamValues[base++] = NULL;

                    pDsccReq->Results[i].StartTime = pParamValues[base];
                    pParamValues[base++] = NULL;

                    pDsccReq->Results[i].CompleteTime = pParamValues[base];
                    pParamValues[base++] = NULL;

                    ccspError = _ansc_atoi(pParamValues[base]);
                    pDsccReq->Results[i].FaultCode = CcspTr069PA_MapCcspErrCode(pCcspCwmpCpeController->hTr069PaMapper, ccspError);
                    base++;

                    if ( pDsccReq->Results[i].FaultCode != 0 )
                    {
                        CCSP_CWMP_SOAP_FAULT    cwmpFault   = {0};

                        CCSP_CWMP_SET_SOAP_FAULT(&cwmpFault, pDsccReq->Results[i].FaultCode);

                        pDsccReq->Results[i].FaultString = cwmpFault.Fault.FaultString;
                        cwmpFault.Fault.FaultString      = NULL;

                        CcspCwmpCleanSoapFault((&cwmpFault));
                    }
                }

                pDsccReq->CommandKey = AnscCloneString(pCommandKey);

                returnStatus =
                    pCcspCwmpMsoIf->DuStateChangeComplete
                        (
                            pCcspCwmpMsoIf->hOwnerContext,
                            (ANSC_HANDLE)pDsccReq,
                            TRUE        /* start a new session to deliver this */
                        );

                pCwmpFault = NULL;
                pCcspCwmpCpeController->DeleteObjects
                    (
                        (ANSC_HANDLE)pCcspCwmpCpeController,
                        CCSP_NS_CHANGEDUSTATE,
                        (int)pDsccReq->NumResults,
                        (PULONG)pInsNumbers,               //This would cause problem with 64 bit machine where long is 64bit.
                        &pCwmpFault,
                        CCSP_TR069_CDS_REQ_OBJECT_DELAY_SECONDS
                    );

                if ( pCwmpFault )
                {
                    CcspCwmpFreeSoapFault(pCwmpFault);
                    pCwmpFault = NULL;
                }

                /* remove saved monitors in PSM */
                for ( i = 0; i < total; i ++ )
                {
                    if ( !pCcspCwmpCpeController->SubsysName )
                    {
                        _ansc_sprintf(buf, "%s%u.", CCSP_NS_CHANGEDUSTATE, (unsigned int)pInsNumbers[i]);
                    }
                    else
                    {
                        _ansc_sprintf(buf, "%s%s%u.", pCcspCwmpCpeController->SubsysName, CCSP_NS_CHANGEDUSTATE, (unsigned int)pInsNumbers[i]);
                    }

                    PSM_Del_Record
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            buf
                        );
                }

                if ( returnStatus != ANSC_STATUS_SUCCESS )
                {
                    CcspTr069PaTraceError
                        ((
                            "CDS - failed to deliver DUStateChangeComplete event, CommandKey <%s>, status <%d>.\n", 
                            pCommandKey,
                            (int)returnStatus
                        ));

                    /* save un-delivered DSCC into PSM */
                    returnStatus =
                        pCcspCwmpProcessor->SaveDUStateChangeComplete
                            (
                                (ANSC_HANDLE)pCcspCwmpProcessor,
                                (ANSC_HANDLE)pDsccReq
                            );

                    if ( returnStatus == ANSC_STATUS_SUCCESS )
                    {
                        CcspTr069PaTraceError
                            ((
                                "DUStateChangeComplete event with CommandKey <%s>, will be re-delivered in future.\n", 
                                pCommandKey
                            ));
                    }
                    else
                    {
                        CcspTr069PaTraceError
                            ((
                                "Failed to persist DUStateChangeComplete event with CommandKey <%s> into PSM, event will be lost!\n", 
                                pCommandKey
                            ));
                    }
                }
            }
        }
    }

EXIT:

    if ( pParamNames )
    {
        CcspTr069FreeStringArray(pParamNames, total * CCSP_NS_CDS_RESULT_ARG_COUNT, TRUE);
    }

    if ( pParamValues )
    {
        CcspTr069FreeStringArray(pParamValues, total * CCSP_NS_CDS_RESULT_ARG_COUNT, TRUE);
    }

    if ( pDsccReq )
    {
        CcspTr069DsccReqRemove(pDsccReq);
    }

    CcspTr069PaFreeMemory(pInsNumbers);
}


static
void
CcspCwmppoCheckAdsccAgainstPolicy
    (
        PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController,
        PCCSP_TR069_ADSCC_REQ       pAdsccReq
    )
{
    char*                           pOperationTypeFilter    = NULL;
    char*                           pResultTypeFilter       = NULL;
    char*                           pFaultCodeFilter        = NULL;
    BOOL                            bMatched                = TRUE;
    int                             i;
    PCCSP_TR069_ADSCC_OpResult      pResult;
    char                            strFaultCode[16];

    pOperationTypeFilter = 
        CcspManagementServer_GetDUStateChangeComplPolicy_OperationTypeFilter
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    pResultTypeFilter = 
        CcspManagementServer_GetDUStateChangeComplPolicy_ResultTypeFilter
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    pFaultCodeFilter = 
        CcspManagementServer_GetDUStateChangeComplPolicy_FaultCodeFilter
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );


    for ( i = 0; i < pAdsccReq->NumResults; i ++ )
    {
        pResult = pAdsccReq->Results + i;

        if ( pOperationTypeFilter )
        {
            PANSC_TOKEN_CHAIN           pOpTokens;
            int                         nTokenCount;

            pOpTokens = AnscTcAllocate(pOperationTypeFilter, ",");

            if ( pOpTokens )
            {
                PANSC_STRING_TOKEN      pToken = NULL;

                nTokenCount = AnscTcGetTokenCount(pOpTokens);

                bMatched = FALSE;

                for ( i = 0; i < nTokenCount; i ++ )
                {
                    pToken = AnscTcUnlinkToken(pOpTokens);

                    if ( AnscEqualString(pToken->Name, CCSP_CWMP_DSC_OP_NAME_Install, FALSE) &&
                         AnscEqualString(pResult->OperationPerformed, CCSP_CWMP_DSC_OP_NAME_Install, TRUE) )
                    {
                        bMatched = TRUE;
                    }
                    else if ( AnscEqualString(pToken->Name, CCSP_CWMP_DSC_OP_NAME_Uninstall, FALSE) &&
                         AnscEqualString(pResult->OperationPerformed, CCSP_CWMP_DSC_OP_NAME_Uninstall, TRUE) )
                    {
                        bMatched = TRUE;
                    }
                    else if ( AnscEqualString(pToken->Name, CCSP_CWMP_DSC_OP_NAME_Update, FALSE) &&
                         AnscEqualString(pResult->OperationPerformed, CCSP_CWMP_DSC_OP_NAME_Update, TRUE) )
                    {
                        bMatched = TRUE;
                    }

                    AnscFreeMemory(pToken);

                    if ( bMatched )
                    {
                        break;
                    }
                }

                AnscTcFree((ANSC_HANDLE)pOpTokens);
            }
        }

        if ( !bMatched )
        {
            pResult->bFilteredOut = TRUE;
            continue;
        }

        if ( pResultTypeFilter &&
             ( ( AnscEqualString(pResultTypeFilter, "Success", FALSE) && pResult->FaultCode != 0 ) ||
               ( AnscEqualString(pResultTypeFilter, "Failure", FALSE) && pResult->FaultCode == 0 ) ) )
        {
            pResult->bFilteredOut = TRUE;
            continue;
        }

        if ( pFaultCodeFilter && pResult->FaultCode != 0 &&
            !AnscEqualString(pResultTypeFilter, "Success", FALSE) )
        {
            _ansc_sprintf(strFaultCode, "%u", pResult->FaultCode);

            if ( !_ansc_strstr(pFaultCodeFilter, strFaultCode) )
            {
                pResult->bFilteredOut = TRUE;
                continue;
            }
        }
    }

    if ( pOperationTypeFilter )
    {
        CcspTr069PaFreeMemory(pOperationTypeFilter);
    }

    if ( pResultTypeFilter )
    {
        CcspTr069PaFreeMemory(pResultTypeFilter);
    }

    if ( pFaultCodeFilter )
    {
        CcspTr069PaFreeMemory(pFaultCodeFilter);
    }
}


static
void
CcspCwmppoCheckAutonomousCdsResults
    (
        PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController,
        char*                       pAdscReqObject
    )
{
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf      = (PCCSP_CWMP_MSO_INTERFACE)pCcspCwmpCpeController->GetCcspCwmpMsoIf(pCcspCwmpCpeController);
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor   = (PCCSP_CWMP_PROCESSOR_OBJECT   )pCcspCwmpCpeController->hCcspCwmpProcessor;
    int                             i;
    char                            buf[600];
    PCCSP_TR069_ADSCC_REQ           pAdsccReq       = NULL;
    char**                          pParamNames     = NULL;
    char**                          pParamValues    = NULL;
    int                             order           = 0;
    CCSP_BOOL                       bEnabled;
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;

    bEnabled = 
        CcspManagementServer_GetDUStateChangeComplPolicy_Enable
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    if ( !bEnabled )
    {
        return;
    }

    /* operation completes, compose ADSCC request */
    pAdsccReq = (PCCSP_TR069_ADSCC_REQ)CcspTr069PaAllocateMemory(sizeof(CCSP_TR069_ADSCC_REQ));

    if ( !pAdsccReq )
    {
        goto EXIT;
    }

    pAdsccReq->Results = 
        (PCCSP_TR069_ADSCC_OpResult)CcspTr069PaAllocateMemory
            (
                sizeof(CCSP_TR069_ADSCC_OpResult)
            );

    if ( !pAdsccReq->Results )
    {
        goto EXIT;
    }

    pAdsccReq->NumResults = 1;

    pParamNames  = (char**)CcspTr069PaAllocateMemory(sizeof(char*) * CCSP_NS_ACDS_RESULT_ARG_COUNT);
    if ( !pParamNames )
    {
        goto EXIT;
    }
    AnscZeroMemory(pParamNames, sizeof(char*) * CCSP_NS_ACDS_RESULT_ARG_COUNT);

    pParamValues = (char**)CcspTr069PaAllocateMemory(sizeof(char*) * CCSP_NS_ACDS_RESULT_ARG_COUNT);
    if ( !pParamValues )
    {
        goto EXIT;
    }
    AnscZeroMemory(pParamValues, sizeof(char*) * CCSP_NS_ACDS_RESULT_ARG_COUNT);

    snprintf(buf, sizeof(buf),"%s.%s", pAdscReqObject, CCSP_NS_CDS_UUID);
    pParamNames[order++] = AnscCloneString(buf);

    snprintf(buf, sizeof(buf),"%s.%s", pAdscReqObject, CCSP_NS_CDS_DEPLOYMENT_UNIT_REF);
    pParamNames[order++] = AnscCloneString(buf);

    snprintf(buf, sizeof(buf),"%s.%s", pAdscReqObject, CCSP_NS_CDS_VERSION);
    pParamNames[order++] = AnscCloneString(buf);

    snprintf(buf, sizeof(buf),"%s.%s", pAdscReqObject, CCSP_NS_CDS_CURRENT_STATE);
    pParamNames[order++] = AnscCloneString(buf);

    snprintf(buf, sizeof(buf),"%s.%s", pAdscReqObject, CCSP_NS_CDS_RESOLVED);
    pParamNames[order++] = AnscCloneString(buf);

    snprintf(buf, sizeof(buf),"%s.%s", pAdscReqObject, CCSP_NS_CDS_EXECUTION_UNIT_REF_LIST);
    pParamNames[order++] = AnscCloneString(buf);

    snprintf(buf, sizeof(buf),"%s.%s", pAdscReqObject, CCSP_NS_CDS_START_TIME);
    pParamNames[order++] = AnscCloneString(buf);

    snprintf(buf, sizeof(buf),"%s.%s", pAdscReqObject, CCSP_NS_CDS_COMPLETE_TIME);
    pParamNames[order++] = AnscCloneString(buf);

    snprintf(buf, sizeof(buf),"%s.%s", pAdscReqObject, CCSP_NS_CDS_ERROR);
    pParamNames[order++] = AnscCloneString(buf);

    snprintf(buf, sizeof(buf),"%s.%s", pAdscReqObject, CCSP_NS_CDS_OPERATION);
    pParamNames[order++] = AnscCloneString(buf);

    /* retreive result values */
    returnStatus = 
        pCcspCwmpCpeController->GetParamStringValues
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                pParamNames,
                CCSP_NS_ACDS_RESULT_ARG_COUNT,
                pParamValues
            );

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTr069PaTraceError
            ((
                "CDS - failed to retrieve ADSCC results!\n"
            ));
    }
    else
    {
        int                         base;
        int                         ccspError;

        i = 0;

        base = i * CCSP_NS_ACDS_RESULT_ARG_COUNT;

        pAdsccReq->Results[i].Uuid = pParamValues[base];
        pParamValues[base++] = NULL;

        pAdsccReq->Results[i].DeploymentUnitRef = pParamValues[base];
        pParamValues[base++] = NULL;

        pAdsccReq->Results[i].Version = pParamValues[base];
        pParamValues[base++] = NULL;

        pAdsccReq->Results[i].CurrentState = pParamValues[base];
        pParamValues[base++] = NULL;

        if ( AnscEqualString(pParamValues[base], "0", TRUE) ||
             AnscEqualString(pParamValues[base], "false", FALSE) )
        {
            pAdsccReq->Results[i].Resolved = 0;
        }
        else
        {
            pAdsccReq->Results[i].Resolved = 1;
        }
        base++;

        pAdsccReq->Results[i].ExecutionUnitRefList = pParamValues[base];
        pParamValues[base++] = NULL;

        pAdsccReq->Results[i].StartTime = pParamValues[base];
        pParamValues[base++] = NULL;

        pAdsccReq->Results[i].CompleteTime = pParamValues[base];
        pParamValues[base++] = NULL;

        ccspError = _ansc_atoi(pParamValues[base]);
        pAdsccReq->Results[i].FaultCode = CcspTr069PA_MapCcspErrCode(pCcspCwmpCpeController->hTr069PaMapper, ccspError);
        base++;

        if ( pAdsccReq->Results[i].FaultCode != 0 )
        {
            CCSP_CWMP_SOAP_FAULT    cwmpFault   = {0};

            CCSP_CWMP_SET_SOAP_FAULT(&cwmpFault, pAdsccReq->Results[i].FaultCode);

            pAdsccReq->Results[i].FaultString = cwmpFault.Fault.FaultString;
            cwmpFault.Fault.FaultString      = NULL;

            CcspCwmpCleanSoapFault((&cwmpFault));
        }

        if ( AnscEqualString(pParamValues[base], CCSP_NS_CDS_OPERATION_Install, TRUE) )
        {
            pAdsccReq->Results[i].OperationPerformed = AnscCloneString("Install");
        }
        else if ( AnscEqualString(pParamValues[base], CCSP_NS_CDS_OPERATION_Update, TRUE) )
        {
            pAdsccReq->Results[i].OperationPerformed = AnscCloneString("Update");
        }
        else
        {
            pAdsccReq->Results[i].OperationPerformed = AnscCloneString("Uninstall");
        }
        base++;

        pAdsccReq->Results[i].bFilteredOut = FALSE;
        /* check policy to make sure this event needs to be filtered out or not */
        CcspCwmppoCheckAdsccAgainstPolicy
            (
                pCcspCwmpCpeController,
                pAdsccReq
            );

        if ( !pAdsccReq->Results[0].bFilteredOut )
        {
            returnStatus =
                pCcspCwmpMsoIf->AutonomousDuStateChangeComplete
                    (
                        pCcspCwmpMsoIf->hOwnerContext,
                        (ANSC_HANDLE)pAdsccReq,
                        TRUE        /* start a new session to deliver this */
                    );

            /* we made an assumption: PA is not required to remove the request entry, 
             * it's the FC that creates the entry needs to remove it after operation
             * completes. 
             *
             * Here we got a problem. When does initiating FC remove these objects?
             * Once PA gets the signal, it needs to retrieve the result, we need
             * to define a way to hold off request object removal with a delay
             * (say about a few minutes if it's safe).
             */
            if ( returnStatus != ANSC_STATUS_SUCCESS )
            {
                CcspTr069PaTraceError
                    ((
                        "CDS - failed to make AutonomousDuStateChangeComplete, status <%d>.\n", 
                        (int)returnStatus
                    ));

                /* save un-delivered ADSCC into PSM */
                returnStatus =
                    pCcspCwmpProcessor->SaveAutonDUStateChangeComplete
                        (
                            (ANSC_HANDLE)pCcspCwmpProcessor,
                            (ANSC_HANDLE)pAdsccReq
                        );

                if ( returnStatus == ANSC_STATUS_SUCCESS )
                {
                    CcspTr069PaTraceError
                        ((
                            "AutonomousDUStateChangeComplete event saved into PSM and will be re-delivered in future.\n"
                        ));
                }
                else
                {
                    CcspTr069PaTraceError
                        ((
                            "Failed to persist AutonomousDUStateChangeComplete event into PSM, event will be lost!\nUUID - %s, operation performed - %s\n", 
                            pAdsccReq->Results->Uuid,
                            pAdsccReq->Results->OperationPerformed
                        ));
                }
            }
        }
    }

EXIT:

    if ( pParamNames )
    {
        CcspTr069FreeStringArray(pParamNames, CCSP_NS_ACDS_RESULT_ARG_COUNT, TRUE);
    }

    if ( pParamValues )
    {
        CcspTr069FreeStringArray(pParamValues, CCSP_NS_ACDS_RESULT_ARG_COUNT, TRUE);
    }

    if ( pAdsccReq )
    {
        CcspTr069AdsccReqRemove(pAdsccReq);
    }
}


static
BOOL
CcspCwmppoCheckAtcAgainstPolicy
    (
        PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController,
        BOOL                        bIsDownload,
        char*                       pFileType,
        ULONG                       FaultCode
    )
{
    CCSP_BOOL                       bEnabled;
    CCSP_STRING                     pTransferTypeFilter = NULL;
    CCSP_STRING                     pResultTypeFilter   = NULL;
    CCSP_STRING                     pFileTypeFilter	= NULL;
    BOOL                            bFilteredOut    	= FALSE;

    bEnabled = 
        CcspManagementServer_GetAutonomousTransferCompletePolicy_Enable
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    if ( !bEnabled )
    {
        bFilteredOut = TRUE;
        goto EXIT;
    }

    pTransferTypeFilter = 
        CcspManagementServer_GetAutonomousTransferCompletePolicy_TransferTypeFilter
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    pResultTypeFilter = 
        CcspManagementServer_GetAutonomousTransferCompletePolicy_ResultTypeFilter
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    pFileTypeFilter = 
        CcspManagementServer_GetAutonomousTransferCompletePolicy_FileTypeFilter
            (
                pCcspCwmpCpeController->PANameWithPrefix
            );

    if ( pTransferTypeFilter &&
         ( ( AnscEqualString(pTransferTypeFilter, "Upload", FALSE) && bIsDownload ) ||
           ( AnscEqualString(pTransferTypeFilter, "Download", FALSE) && !bIsDownload ) ) )
    {
        bFilteredOut = TRUE;
        goto EXIT;
    }

    if ( pResultTypeFilter &&
         ( ( AnscEqualString(pResultTypeFilter, "Success", FALSE) && FaultCode != 0 ) ||
           ( AnscEqualString(pResultTypeFilter, "Failure", FALSE) && FaultCode == 0 ) ) )
    {
        bFilteredOut = TRUE;
        goto EXIT;
    }

    if ( pFileTypeFilter && !_ansc_strstr(pFileTypeFilter, pFileType) )
    {
        bFilteredOut = TRUE;
        goto EXIT;
    }

EXIT:

    if ( pTransferTypeFilter )
    {
        CcspTr069PaFreeMemory(pTransferTypeFilter);
    }

    if ( pResultTypeFilter )
    {
        CcspTr069PaFreeMemory(pResultTypeFilter);
    }

    if ( pFileTypeFilter )
    {
        CcspTr069PaFreeMemory(pFileTypeFilter);
    }

    return  bFilteredOut;
}


static
void
CcspCwmppoRetrieveFirmwareDownloadResults
    (
        PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController,
        PULONG                      pulFaultCode,
        PANSC_UNIVERSAL_TIME        pStartTime,
        PANSC_UNIVERSAL_TIME        pEndTime,
        char**                      ppCommandKey
    )
{
    char                            buf[256];
    ULONG                           order               = 0;
    char**                          pParamNames         = NULL;
    char**                          pParamValues        = NULL;
    ANSC_STATUS                     returnStatus        = ANSC_STATUS_SUCCESS;

    *pulFaultCode   = 0;
    *ppCommandKey   = NULL;
    AnscZeroMemory(pStartTime, sizeof(ANSC_UNIVERSAL_TIME));
    AnscZeroMemory(pEndTime,   sizeof(ANSC_UNIVERSAL_TIME));

    CcspTr069PaTraceInfo
        ((
            "Download - retrieving firmware download operation ...\n"
        ));

    pParamNames  = (char**)CcspTr069PaAllocateMemory(sizeof(char*) * CCSP_NS_DOWNLOAD_RESULT_ARG_COUNT);
    if ( !pParamNames )
    {
        goto EXIT;
    }
    AnscZeroMemory(pParamNames, sizeof(char*) * CCSP_NS_DOWNLOAD_RESULT_ARG_COUNT);

    pParamValues = (char**)CcspTr069PaAllocateMemory(sizeof(char*) * CCSP_NS_DOWNLOAD_RESULT_ARG_COUNT);
    if ( !pParamValues )
    {
        goto EXIT;
    }
    AnscZeroMemory(pParamValues, sizeof(char*) * CCSP_NS_DOWNLOAD_RESULT_ARG_COUNT);

    _ansc_sprintf(buf, "%s%s", CCSP_NS_DOWNLOAD, CCSP_NS_DOWNLOAD_COMMAND_KEY);
    pParamNames[order++] = AnscCloneString(buf);

    _ansc_sprintf(buf, "%s%s", CCSP_NS_DOWNLOAD, CCSP_NS_DOWNLOAD_ERROR);
    pParamNames[order++] = AnscCloneString(buf);

    _ansc_sprintf(buf, "%s%s", CCSP_NS_DOWNLOAD, CCSP_NS_DOWNLOAD_START_TIME);
    pParamNames[order++] = AnscCloneString(buf);

    _ansc_sprintf(buf, "%s%s", CCSP_NS_DOWNLOAD, CCSP_NS_DOWNLOAD_COMPLETE_TIME);
    pParamNames[order++] = AnscCloneString(buf);

    /* retreive result values */
    returnStatus = 
        pCcspCwmpCpeController->GetParamStringValues
            (
                (ANSC_HANDLE)pCcspCwmpCpeController,
                pParamNames,
                CCSP_NS_DOWNLOAD_RESULT_ARG_COUNT,
                pParamValues
            );

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTr069PaTraceError
            ((
                "Download - failed to retrieve firmware download result!\n"
            ));
    }
    else
    {
        int                         ccspError;
        PANSC_UNIVERSAL_TIME        pTime;

        *ppCommandKey = pParamValues[0];
        pParamValues[0] = NULL;
                    
        CcspTr069PaTraceDebug(("TR-069 PA is notified a download has completed, command key <%s>.\n", *ppCommandKey));
        
        ccspError = _ansc_atoi(pParamValues[1]);

        pTime = CcspStringToCalendarTime(pParamValues[2]);
        if ( pTime )
        {
            *pStartTime = *pTime;
            CcspTr069PaFreeMemory(pTime);
        }

        pTime = CcspStringToCalendarTime(pParamValues[3]);
        if ( pTime )
        {
            *pEndTime = *pTime;
            CcspTr069PaFreeMemory(pTime);
        }

        *pulFaultCode = (ULONG)CcspTr069PA_MapCcspErrCode(pCcspCwmpCpeController->hTr069PaMapper, ccspError);
    }

EXIT:

    if ( pParamNames )
    {
        CcspTr069FreeStringArray(pParamNames, CCSP_NS_DOWNLOAD_RESULT_ARG_COUNT, TRUE);
    }

    if ( pParamValues )
    {
        CcspTr069FreeStringArray(pParamValues, CCSP_NS_DOWNLOAD_RESULT_ARG_COUNT, TRUE);
    }
}


static void
CcspCwmppoFreeParamValueChangeSignalStruct
    (
        parameterSigStruct_t*       pSigStruct,
        int                         size
    )
{
    parameterSigStruct_t*           pVC;
    int                             i;

    if ( !pSigStruct ) return;

    for ( i = 0; i < size; i ++ )
    {
        pVC = pSigStruct + i;

        if ( pVC->parameterName    ) CcspTr069PaFreeMemory((PVOID)pVC->parameterName   );
        if ( pVC->oldValue         ) CcspTr069PaFreeMemory((PVOID)pVC->oldValue        );
        if ( pVC->newValue         ) CcspTr069PaFreeMemory((PVOID)pVC->newValue        );
        if ( pVC->subsystem_prefix ) CcspTr069PaFreeMemory((PVOID)pVC->subsystem_prefix);
    }

    CcspTr069PaFreeMemory(pSigStruct);
}


static parameterSigStruct_t*
CcspCwmppoCloneParamValueChangeSignalStruct
    (
        parameterSigStruct_t*       pSigStruct,
        int                         size
    )
{
    parameterSigStruct_t*           pNewSigStruct = NULL;
    parameterSigStruct_t*           pNewVC = NULL;
    parameterSigStruct_t*           pVC    = NULL;
    int                             i;

    if ( size <= 0 ) return NULL;

    pNewSigStruct = CcspTr069PaAllocateMemory(sizeof(parameterSigStruct_t)*size);

    if ( pNewSigStruct )
    {
        AnscCopyMemory(pNewSigStruct, pSigStruct, sizeof(parameterSigStruct_t)*size);

        for ( i = 0; i < size; i ++ )
        {
            pNewVC = pNewSigStruct + i;
            pVC    = pSigStruct + i;

            pNewVC->parameterName    = pVC->parameterName ? CcspTr069PaCloneString((char*)pVC->parameterName) : NULL;
            pNewVC->oldValue         = pVC->oldValue ? CcspTr069PaCloneString((char*)pVC->oldValue) : NULL;
            pNewVC->newValue         = pVC->newValue ? CcspTr069PaCloneString((char*)pVC->newValue) : NULL;
            pNewVC->subsystem_prefix = pVC->subsystem_prefix ? CcspTr069PaCloneString((char*)pVC->subsystem_prefix) : NULL;
        }
    }

    return  pNewSigStruct;
}

typedef  struct
_CCSP_CWMPPO_PVC_SIGNAL_LIST
{
    SINGLE_LINK_ENTRY               Linkage;
    parameterSigStruct_t            *pSignalStruct;
    int                             size;
}
CCSP_CWMPPO_PVC_SIGNAL_LIST, *PCCSP_CWMPPO_PVC_SIGNAL_LIST;

#define  ACCESS_CCSP_CWMPPO_PVC_SIGNAL(p)                                                                  \
         ACCESS_CONTAINER(p, CCSP_CWMPPO_PVC_SIGNAL_LIST, Linkage)


static  BOOL                        s_PVC_SigList_Initialized   = FALSE;
static  QUEUE_HEADER                s_PVC_SigList;
ANSC_LOCK                           s_PVC_SigList_Lock;
static  ULONG						s_lastUdpConnReqAddrNotifyTime = 0;
static  ULONG						s_nextUdpConnReqAddrNotifyTime = 0;

ANSC_STATUS
CcspCwmppoDelayUdpConnReqAddrTask
    (
        ANSC_HANDLE                 hContext
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject             	= (PCCSP_CWMP_PROCESSOR_OBJECT )hContext;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MSO_INTERFACE         pCcspCwmpMsoIf         = (PCCSP_CWMP_MSO_INTERFACE        )pCcspCwmpCpeController->GetCcspCwmpMsoIf(pCcspCwmpCpeController);

    pMyObject->AsyncTaskCount++;

    while ( pMyObject->bActive )
    {
        ULONG                       uTimeNow = AnscGetTickInSeconds();
        ULONG                       uNotificationLimit = 0;
        CCSP_STRING                 pNotificationLimit = NULL;

        pNotificationLimit = 
            CcspManagementServer_GetUDPConnectionRequestAddressNotificationLimit
                (
                    pCcspCwmpCpeController->PANameWithPrefix
                );

        /*RDKB-7324, CID-33500, free resource after use*/
        if(pNotificationLimit)
        {
            uNotificationLimit = (ULONG)_ansc_atoi(pNotificationLimit);
            CcspManagementServer_Free(pNotificationLimit);
        }
        else
        {
            uNotificationLimit = 0;
        }

        if ( uNotificationLimit == 0 || (s_nextUdpConnReqAddrNotifyTime != 0 && uTimeNow >= s_nextUdpConnReqAddrNotifyTime ) )
        {
            char*                   pRootObjName = pCcspCwmpCpeController->GetRootObject((ANSC_HANDLE)pCcspCwmpCpeController);
            char                    paramName[256];
            CCSP_STRING             paramValue;

            s_nextUdpConnReqAddrNotifyTime = 0;	/* reset next notification time */
            s_lastUdpConnReqAddrNotifyTime = uTimeNow;

            paramValue = CcspManagementServer_GetUDPConnectionRequestAddress(pCcspCwmpCpeController->PANameWithPrefix);

            _ansc_sprintf(paramName, "%s.ManagementServer.UDPConnectionRequestAddress", pRootObjName);

            /* notify value change */
            pCcspCwmpMsoIf->ValueChanged
                (
                    pCcspCwmpMsoIf->hOwnerContext,
                    (char*)paramName,
                    (char*)paramValue,
                    CCSP_CWMP_TR069_DATA_TYPE_String,
                    TRUE
                );

            s_lastUdpConnReqAddrNotifyTime = AnscGetTickInSeconds();

            break;
        }

        AnscSleep(1000);
    }

    pMyObject->AsyncTaskCount--;

    

    return    ANSC_STATUS_SUCCESS;
}


static char                         s_ns_download_state[256] = {0};


void 
CcspCwmppoProcessPvcSignal
    (
        ANSC_HANDLE                 hThisObject,
        parameterSigStruct_t*       val,
        int                         size
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject               = (PCCSP_CWMP_PROCESSOR_OBJECT )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_MSO_INTERFACE             pCcspCwmpMsoIf              = (PCCSP_CWMP_MSO_INTERFACE        )pCcspCwmpCpeController->GetCcspCwmpMsoIf(pCcspCwmpCpeController);
    int                             i;
    char*                           pParamName;
    CCSP_STRING                     Subsystems[CCSP_SUBSYSTEM_MAX_COUNT]    = {0};
    int                             NumSubsystems                           = 0;
    parameterSigStruct_t*           pVC;
    ULONG                           Notification;
    ANSC_STATUS                     status;

    if ( !s_ns_download_state[0] )
    {
        _ansc_sprintf(s_ns_download_state, "%s%s", CCSP_NS_DOWNLOAD, CCSP_NS_DOWNLOAD_STATE);
    }

    CcspTr069PaTraceInfo(("Processing queued parameterValueChangeSignal over message bus, size=%d.\n", size));

    if ( pCcspCwmpCpeController->bCpeRebooting )
    {
        CcspTr069PaTraceWarning(("parameterValueChangeSignal dropped by PA for CPE is rebooting!\n"));
        return;
    }

    if ( size <= 0 )
    {
        return;
    }


    /* let MS FC handle it first */
    CcspManagementServer_paramValueChanged(val, size);

    /*
     * Determine if value changes are intended for this PA
     */
    if ( val->writeID == CCSP_TR069PA_WRITE_ID )        /* the value change is caused by a TR-069 PA */
    {
        /* 
         * check sub-system prefix, if the parameter is on a sub-system
         * that this PA is managing, this value change is not for us.
         */
        pParamName = (char *)(val->parameterName);

        NumSubsystems = CCSP_SUBSYSTEM_MAX_COUNT;

        CcspTr069PA_GetNamespaceSubsystems
            (
                pCcspCwmpCpeController->hTr069PaMapper,
                (char*)pParamName,
                Subsystems,
                &NumSubsystems,
                CCSP_TRUE
            );
    
        if ( NumSubsystems <= 0 )
        {
            Subsystems[0] = CcspTr069PaCloneString(pCcspCwmpCpeController->SubsysName);    /* assume 'local' sub-system will be used */
            NumSubsystems = 1;
        }

        for ( i = 0; i < NumSubsystems; i ++ )
        {
			if ( !Subsystems[i] && (!val->subsystem_prefix || *(val->subsystem_prefix) == 0) ) break;
            if ( AnscEqualString((char*)Subsystems[i], (char*)val->subsystem_prefix, TRUE) ) break;
        }

        if ( i >= NumSubsystems )
        {
            /* The sub-system is not managed by this PA */
            return;
        }
    }

    /* go through each changed parameter */
    for ( i = 0; i < size; i ++ )
    {
        pVC = val + i;


        /* Security Requiremnt: Log messages must not disclose any confidential data
           like cryptographic keys and password. So don't save Passphrase on log message.
         */
        if ( NULL == strstr(pVC->parameterName, "Passphrase" ) ) {
 	    CcspTr069PaTraceInfo
            ((
                "  processing VC on namespace=<%s>, new val=<%s>, old val=<%s>.\n", 
                pVC->parameterName, 
                pVC->newValue ? pVC->newValue : "",
                pVC->oldValue ? pVC->oldValue : ""
            ));
	}
        else {
           CcspTr069PaTraceInfo((" processing VC on namespace=<%s> but not printing new and old values as it will disclose the confidential information.\n", pVC->parameterName ));
        }

        if ( _ansc_strstr(pVC->parameterName, CCSP_NS_CHANGEDUSTATE) == pVC->parameterName )
        {
            /* ChangeDUState monitor state has changed */
            char                    buf[512];
            char*                   pSep        = (char*)pVC->parameterName + AnscSizeOfString(CCSP_NS_CHANGEDUSTATE);
            char*                   pCommandKey = NULL;
            int                     len         = 0;

            if ( !pVC->newValue )
            {
                continue;
            }

            pSep = _ansc_strchr(pSep, '.');
            if ( !pSep )
            {
                continue;
            }

            if ( pCcspCwmpCpeController->SubsysName )
            {
                len = strlen(pCcspCwmpCpeController->SubsysName);
                memcpy(buf, pCcspCwmpCpeController->SubsysName, len + 1);
            }
            AnscCopyMemory((char*)buf + len, (char*)pVC->parameterName, pSep - pVC->parameterName + 1);
            buf[len + pSep - pVC->parameterName + 1] = 0;

            if ( AnscEqualString((char*)pVC->newValue, CCSP_NS_CDS_OPERATION_STATE_Complete, FALSE) ||
                 AnscEqualString((char*)pVC->newValue, CCSP_NS_CDS_OPERATION_STATE_Error, FALSE) )
            {
                /* a ChangeDUState operation has finished successfully or failed */
                int                 psmStatus;

                psmStatus = 
                    PSM_Get_Record_Value2
                        (
                            pCcspCwmpCpeController->hMsgBusHandle,
                            pCcspCwmpCpeController->SubsysName,
                            buf,
                            NULL,
                            &pCommandKey
                        );

                snprintf(buf + strlen(buf), sizeof(buf),"%s", CCSP_NS_CDS_PSM_NODE_COMPLETE);

                if ( psmStatus != CCSP_SUCCESS )
                {
                    /* 
                     * autonomous ChangeDUState operation since we cannot
                     * find it in PSM
                     */
                    CcspCwmppoCheckAutonomousCdsResults(pCcspCwmpCpeController, buf);
                }
                else
                {
                    CcspTr069PaTraceDebug
                        ((
                            "CDS - saves state monitor <%s> into PSM, value <%s>.\n", 
                            buf, 
                            "1"
                        ));

                    psmStatus = 
                        PSM_Set_Record_Value2
                            (
                                pCcspCwmpCpeController->hMsgBusHandle,
                                pCcspCwmpCpeController->SubsysName,
                                buf,
                                ccsp_boolean,
                                "1"
                            );

                    /* check if all operations included in CDS with given command key
                     * have finished.
                     */
                    CcspCwmppoCheckCdsResults(pCcspCwmpCpeController, pCommandKey);
                }
            }
        }
        else if ( _ansc_strstr(pVC->parameterName, s_ns_download_state) == pVC->parameterName &&
                  val && val->newValue &&
                  AnscEqualString((char *)(val->newValue), "Idle", TRUE) /* PA only cares the state changed back to 'Idle' */  )
        {
            /* Download monitor state has changed */
            char*                   pCommandKey     = NULL;
            ANSC_UNIVERSAL_TIME     timeStart       = { 0 };
            ANSC_UNIVERSAL_TIME     timeEnd         = { 0 };
            CCSP_CWMP_FAULT         cwmpFault       = { 0 };
            PCCSP_CWMP_FAULT        pCwmpFault      = &cwmpFault;
            BOOL                    bAtc            = FALSE;
            char*                   pAnnounceURL    = NULL;
            char*                   pTransferURL    = NULL;
            char*                   pFileType       = NULL;
            unsigned int            FileSize        = 0;
            char*                   pTargetFileName = NULL;

            /* 
             * Retrieve firmware download result
             */
            CcspCwmppoRetrieveFirmwareDownloadResults
                (
                    pCcspCwmpCpeController,
                    &pCwmpFault->FaultCode,
                    &timeStart,
                    &timeEnd,
                    &pCommandKey
                );

            /* signal TransferComplete */
            if ( !bAtc )
            {
                /* To help FU (firmware upgrade FC) clean up finished download
                 * session, PA is requested to set FU.State to Idle to tell
                 * FU that it can safely remove finished firmware download
                 * session.
                 */
                PCCSP_CWMP_MPA_INTERFACE 
                                        pCcspCwmpMpaIf = (PCCSP_CWMP_MPA_INTERFACE)pMyObject->hCcspCwmpMpaIf;
                CCSP_CWMP_PARAM_VALUE   cwmpParamValue = {0};
                SLAP_VARIABLE           slapVar;
                int                     iStatus        = 0;
                PCCSP_CWMP_SOAP_FAULT   pCwmpSoapFault = (PCCSP_CWMP_SOAP_FAULT)NULL;

                cwmpParamValue.Name = (char*)s_ns_download_state; 
                cwmpParamValue.Tr069DataType = CCSP_CWMP_TR069_DATA_TYPE_String;
                cwmpParamValue.Value = &slapVar;

                slapVar.Syntax = SLAP_VAR_SYNTAX_string;
                slapVar.Variant.varString = "Idle";

                pCcspCwmpMpaIf->SetParameterValues
                    (
                        pCcspCwmpMpaIf->hOwnerContext,
                        (ANSC_HANDLE)&cwmpParamValue,
                        1,
                        &iStatus,
                        (ANSC_HANDLE*)&pCwmpSoapFault,
                        FALSE
                    );

                status =
                    pCcspCwmpMsoIf->TransferComplete
                        (
                            pCcspCwmpMsoIf->hOwnerContext,
                            TRUE,
                            pCommandKey,
                            (ANSC_HANDLE)pCwmpFault,
                            (ANSC_HANDLE)&timeStart,
                            (ANSC_HANDLE)&timeEnd,
                            TRUE                /* start a new session to deliver this */
                        );

                if ( status != ANSC_STATUS_SUCCESS )
                {
                    /* save un-delivered TC into PSM */
                    pMyObject->SaveTransferComplete
                        (
                            (ANSC_HANDLE)pMyObject,
                            pCommandKey,
                            (ANSC_HANDLE)&timeStart,
                            (ANSC_HANDLE)&timeEnd,
                            TRUE,
                            pCwmpFault
                        );
                }

                if ( pCommandKey ) CcspTr069PaFreeMemory(pCommandKey);
            }
            else
            {
                CCSP_BOOL           bFilteredOut = FALSE;

                bFilteredOut = 
                    CcspCwmppoCheckAtcAgainstPolicy
                        (
                            pCcspCwmpCpeController,
                            TRUE,
                            pFileType,
                            pCwmpFault ? pCwmpFault->FaultCode : 0
                        );

                if ( !bFilteredOut )
                {
                    status = 
                        pCcspCwmpMsoIf->AutonomousTransferComplete
                            (
                                pCcspCwmpMsoIf->hOwnerContext,
                                TRUE,
                                pAnnounceURL,
                                pTransferURL,
                                pFileType,
                                FileSize,
                                pTargetFileName,
                                (ANSC_HANDLE)pCwmpFault,
                                (ANSC_HANDLE)&timeStart,
                                (ANSC_HANDLE)&timeEnd,
                                TRUE                /* start a new session to deliver this */
                            );

                    if ( status != ANSC_STATUS_SUCCESS )
                    {
                        /* save un-delivered ATC into PSM */
                        status = 
                            pMyObject->SaveAutonomousTransferComplete
                                (
                                    (ANSC_HANDLE)pMyObject,
                                    TRUE,
                                    pAnnounceURL,
                                    pTransferURL,
                                    pFileType,
                                    FileSize,
                                    pTargetFileName,
                                    (ANSC_HANDLE)pCwmpFault,
                                    (ANSC_HANDLE)&timeStart,
                                    (ANSC_HANDLE)&timeEnd
                                );

                        if ( status == ANSC_STATUS_SUCCESS )
                        {
                            CcspTr069PaTraceInfo
                                ((
                                    "AutonomousTransferComplete event saved into PSM, will be re-delivered in future.\nTransferURL - %s, FileType - %s\n", 
                                    pTransferURL,
                                    pFileType
                                ));
                        }
                        else
                        {
                            CcspTr069PaTraceError
                                ((
                                    "Failed to persist AutonomousTransferComplete event into PSM, event will be lost!\nTransferURL - %s, FileType - %s\n", 
                                    pTransferURL,
                                    pFileType
                                ));
                        }
                    }

                    if ( pAnnounceURL )
                    {
                        CcspTr069PaFreeMemory(pAnnounceURL);
                    }

                    if ( pTransferURL )
                    {
                        CcspTr069PaFreeMemory(pTransferURL);
                    }

                    if ( pFileType )
                    {
                        CcspTr069PaFreeMemory(pFileType);
                    }

                    if ( pTargetFileName )
                    {
                        CcspTr069PaFreeMemory(pTargetFileName);
                    }
                }
            }
        }
        else if ( _ansc_strstr(pVC->parameterName, CCSP_NS_UPLOAD) == pVC->parameterName )
        {
            /* Upload monitor state has changed */
            char*                   pCommandKey     = NULL;
            ANSC_UNIVERSAL_TIME     timeStart       = { 0 };
            ANSC_UNIVERSAL_TIME     timeEnd         = { 0 };
            CCSP_CWMP_FAULT         cwmpFault       = { 0 };
            PCCSP_CWMP_FAULT        pCwmpFault      = &cwmpFault;
            BOOL                    bAtc            = FALSE;
            char*                   pAnnounceURL    = NULL;
            char*                   pTransferURL    = NULL;
            char*                   pFileType       = NULL;
            unsigned int            FileSize        = 0;
            char*                   pTargetFileName = NULL;

            /* TODO:
             *   Read upload result
             */

            /* signal TransferComplete */
            if ( !bAtc )
            {
                pCcspCwmpMsoIf->TransferComplete
                    (
                        pCcspCwmpMsoIf->hOwnerContext,
                        FALSE,
                        pCommandKey,
                        (ANSC_HANDLE)pCwmpFault,
                        (ANSC_HANDLE)&timeStart,
                        (ANSC_HANDLE)&timeEnd,
                        TRUE        /* start a new session to deliver this */
                    );
            }
            else
            {
                CCSP_BOOL           bFilteredOut = FALSE;

                bFilteredOut = 
                    CcspCwmppoCheckAtcAgainstPolicy
                        (
                            pCcspCwmpCpeController,
                            TRUE,
                            pFileType,
                            pCwmpFault ? pCwmpFault->FaultCode : 0
                        );

                if ( !bFilteredOut )
                {
                    pCcspCwmpMsoIf->AutonomousTransferComplete
                        (
                            pCcspCwmpMsoIf->hOwnerContext,
                            FALSE,
                            pAnnounceURL,
                            pTransferURL,
                            pFileType,
                            FileSize,
                            pTargetFileName,
                            (ANSC_HANDLE)pCwmpFault,
                            (ANSC_HANDLE)&timeStart,
                            (ANSC_HANDLE)&timeEnd,
                            TRUE                /* to start a new session to deliver this */
                        );

                    if ( pAnnounceURL )
                    {
                        CcspTr069PaFreeMemory(pAnnounceURL);
                    }

                    if ( pTransferURL )
                    {
                        CcspTr069PaFreeMemory(pTransferURL);
                    }

                    if ( pFileType )
                    {
                        CcspTr069PaFreeMemory(pFileType);
                    }

                    if ( pTargetFileName )
                    {
                        CcspTr069PaFreeMemory(pTargetFileName);
                    }
                }
            }
        }
        else
        {
            if ( val->writeID == CCSP_TR069PA_WRITE_ID )        /* The value change is caused by a TR-069 PA */
            {
                CcspTr069PaTraceWarning(("Value change triggered from ACS, so dropping Value change event \n"));
                return;
            }

            /* value change on regular parameter */

            /* check if the parameter is set to Active or Passive */
            Notification = pMyObject->CheckParamAttrCache((ANSC_HANDLE)pMyObject, (char*)pVC->parameterName);

            /* Store the full parameter data model path in PSM.We not store its value; only the parameter,and
             * we store only one record per parameter. This optimises the number of transactions to the ACS,
             * only notifying it of the last value change, and corresponding latest value.
             * The PSM entry will be deleted after the "VALUE CHANGE" INFORM has been successfully delivered to the ACS.
             */
            status = pMyObject->SaveValueChanged((ANSC_HANDLE)pMyObject, (char*)pVC->parameterName);
            if ( status == ANSC_STATUS_SUCCESS )
            {
                CcspTr069PaTraceInfo
                    ((
                        "Value Changed event saved into PSM, Parameter Name - %s\n",
                        (char*)pVC->parameterName
                    ));
            }
            else
            {
                CcspTr069PaTraceError
                    ((
                        "Failed to persist Value Change event into PSM, Parameter Name - %s\n",
                        (char*)pVC->parameterName
                    ));
            }

            if ( Notification == CCSP_CWMP_NOTIFICATION_active &&
                 NULL != _ansc_strstr(pVC->parameterName, ".ManagementServer.UDPConnectionRequestAddress") )
            {
                CCSP_UINT           uNotificationLimit = 0;
                CCSP_STRING         pNotificationLimit = NULL;
                ULONG               uTimeNow = AnscGetTickInSeconds();

                pNotificationLimit = 
                    CcspManagementServer_GetUDPConnectionRequestAddressNotificationLimit
                        (
                            pCcspCwmpCpeController->PANameWithPrefix
                        );
                /* RDKB-7324, CID-33226, free resource after use
                ** The charector string required, by "CcspManagementServer_GetUDPConnectionRequestAddressNotificationLimit"
                ** converting to integer before use.
                */
                if(pNotificationLimit)
                {
                    uNotificationLimit = (ULONG)_ansc_atoi(pNotificationLimit);
                    CcspManagementServer_Free(pNotificationLimit);
                }
                else
                {
                    uNotificationLimit = 0;
                }

                if ( uNotificationLimit != 0 && s_lastUdpConnReqAddrNotifyTime != 0 &&
                     ( ( uTimeNow >= s_lastUdpConnReqAddrNotifyTime && uTimeNow - s_lastUdpConnReqAddrNotifyTime < uNotificationLimit ) ||
                       ( uTimeNow < s_lastUdpConnReqAddrNotifyTime && 0xFFFFFFFF - s_lastUdpConnReqAddrNotifyTime + uTimeNow < uNotificationLimit  ) ) )
                {
                    if ( s_nextUdpConnReqAddrNotifyTime == 0 ) /* no task previously scheduled */ 
                    {
                        s_nextUdpConnReqAddrNotifyTime = s_lastUdpConnReqAddrNotifyTime + uNotificationLimit + 1;

                        AnscSpawnTask
                            (
                                (void*)CcspCwmppoDelayUdpConnReqAddrTask,
                                (ANSC_HANDLE)pMyObject,
                                "CcspCwmppoDelayUdpConnReqAddrTask"
                            );
                    }

                    return;
                }

                s_lastUdpConnReqAddrNotifyTime = uTimeNow;
            }

            /* notify value change */
            pCcspCwmpMsoIf->ValueChanged
                (
                    pCcspCwmpMsoIf->hOwnerContext,
                    (char*)pVC->parameterName,
                    (char*)pVC->newValue,
                    CcspTr069PA_Ccsp2CwmpType(pVC->type),
                    Notification == CCSP_CWMP_NOTIFICATION_active
                );
        }
    }
}


ANSC_STATUS
CcspCwmppoProcessVcSignalTask
    (
        ANSC_HANDLE                 hContext
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT     pMyObject  = (PCCSP_CWMP_PROCESSOR_OBJECT )hContext;
    PCCSP_CWMPPO_PVC_SIGNAL_LIST    pPvcSig;
    PSINGLE_LINK_ENTRY              pSLinkEntry;
    int                             nCount     = 0;

    pMyObject->AsyncTaskCount++;

    while ( pMyObject->bActive )
    {
		while (TRUE)
        {
			AnscAcquireLock(&s_PVC_SigList_Lock);
        	pSLinkEntry = AnscQueuePopEntry(&s_PVC_SigList);
			AnscReleaseLock(&s_PVC_SigList_Lock);

			if ( !pSLinkEntry ) break;

            pPvcSig = ACCESS_CCSP_CWMPPO_PVC_SIGNAL(pSLinkEntry);

            CcspCwmppoProcessPvcSignal
                (
                    (ANSC_HANDLE)pMyObject,
                    pPvcSig->pSignalStruct,
                    pPvcSig->size
                );

            CcspCwmppoFreeParamValueChangeSignalStruct(pPvcSig->pSignalStruct, pPvcSig->size);
            CcspTr069PaFreeMemory(pPvcSig);

            nCount ++;
            if ( nCount >= 10 )
            {
                nCount = 0;
                break;
            }
        }

        AnscSleep(1000);
    }

	AnscAcquireLock(&s_PVC_SigList_Lock);

    while ( (pSLinkEntry = AnscQueuePopEntry(&s_PVC_SigList)) )
    {
        pPvcSig = ACCESS_CCSP_CWMPPO_PVC_SIGNAL(pSLinkEntry);
    
        CcspCwmppoFreeParamValueChangeSignalStruct(pPvcSig->pSignalStruct, pPvcSig->size);
        CcspTr069PaFreeMemory(pPvcSig);    
    }
    
	AnscReleaseLock(&s_PVC_SigList_Lock);
    AnscFreeLock(&s_PVC_SigList_Lock);

    pMyObject->AsyncTaskCount--;

	return	ANSC_STATUS_SUCCESS;
}


void
CcspCwmppoParamValueChangedCB
    (
        parameterSigStruct_t*       val,
        int                         size,
        void*                       user_data
    )
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pMyObject  = (PCCSP_CWMP_PROCESSOR_OBJECT )user_data;	  
    PCCSP_CWMPPO_PVC_SIGNAL_LIST     pPvcSig;
    BOOL                             bFirstTime =  !s_PVC_SigList_Initialized;

    CcspTr069PaTraceInfo(("PA received parameterValueChangeSignal over message bus, size=%d.\n", size));

    if ( size == 0 ) return;

    pPvcSig = (PCCSP_CWMPPO_PVC_SIGNAL_LIST)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMPPO_PVC_SIGNAL_LIST));
    if ( !pPvcSig )  return;
    pPvcSig->pSignalStruct = CcspCwmppoCloneParamValueChangeSignalStruct(val, size);
    if ( !pPvcSig->pSignalStruct ) goto EXIT;
    pPvcSig->size          = size;

    if ( bFirstTime )
    {
        s_PVC_SigList_Initialized   = TRUE;

        AnscInitializeLock(&s_PVC_SigList_Lock);
        AnscQueueInitializeHeader(&s_PVC_SigList);
    }

    AnscAcquireLock(&s_PVC_SigList_Lock);
    AnscQueuePushEntry(&s_PVC_SigList, &pPvcSig->Linkage);
    AnscReleaseLock(&s_PVC_SigList_Lock);
    
    CcspTr069PaTraceInfo(("parameterValueChangeSignal pushed into processing queue.\n"));

    if ( bFirstTime )
    {
        AnscSpawnTask
            (
                (void*)CcspCwmppoProcessVcSignalTask,
                (ANSC_HANDLE)pMyObject,
                "CcspCwmppoProcessVcSignalTask"
             );
    }

    return;

EXIT:
    CcspTr069PaFreeMemory(pPvcSig);
}


