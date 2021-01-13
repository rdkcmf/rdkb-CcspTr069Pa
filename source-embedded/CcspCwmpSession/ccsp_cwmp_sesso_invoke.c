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

    module:	ccsp_cwmp_sesso_invoke.c

        For CCSP CWMP protocol implementation


    ---------------------------------------------------------------

    description:

        This module implements the advanced method invocation
        functions of the CcspCwmp Wmp Session Object.

        *   CcspCwmpsoGetRpcMethods
        *   CcspCwmpsoInform
        *   CcspCwmpsoTransferComplete
        *   CcspCwmpsoAutonomousTransferComplete
        *   CcspCwmpsoKicked
        *   CcspCwmpsoRequestDownload
        *   CcspCwmpsoDUStateChangeComplete
        *   CcspCwmpsoAutonomousDUStateChangeComplete

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/13/07    initial revision.
        01/02/11    Kang added support of following RPCs.
                        AutonomousTransferComplete
                        DUStateChangeComplete
                        AutonomousDUStateChangeComplete
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_sesso_global.h"
#include "slap_definitions.h"
#include "Tr69_Tlv.h"
#define TR69_TLVDATA_FILE "/nvram/TLVData.bin"
#include "ccsp_tr069pa_mapper_api.h"
#define  CcspCwmppoMpaMapParamInstNumDmIntToCwmp(pParam)                        \
            {                                                                       \
                CCSP_STRING     pReturnStr  = NULL;                                 \
                                                                                    \
                CcspTr069PaTraceDebug(("%s - Param DmInt to CWMP\n", __FUNCTION__));\
                                                                                    \
                pReturnStr =                                                        \
                    CcspTr069PA_MapInstNumDmIntToCwmp(pParam);                      \
                                                                                    \
                if ( pReturnStr )                                                   \
                {                                                                   \
                    /* we are responsible for releasing the original string */      \
                    CcspTr069PaFreeMemory(pParam);                                  \
                    pParam = pReturnStr;                                            \
                }                                                                   \
            }
static int tlvFileFlag = 0;
/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoGetRpcMethods
            (
                ANSC_HANDLE                 hThisObject,
                SLAP_STRING_ARRAY**         ppMethodList,
                PULONG                      pulErrorCode
            );

    description:

        This function is called to discover the set of methods
        supported by the server.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                SLAP_STRING_ARRAY**         ppMethodList
                Returns the array of strings containing the names of
                each of the RPC methods.

                PULONG                      pulErrorCode
                Specifies the error code to be returned.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoGetRpcMethods
    (
        ANSC_HANDLE                 hThisObject,
        SLAP_STRING_ARRAY**         ppMethodList,
        PULONG                      pulErrorCode
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq     = (PCCSP_CWMPSO_ASYNC_REQUEST  )NULL;
    PCCSP_CWMP_SOAP_RESPONSE        pCwmpSoapResponse  = (PCCSP_CWMP_SOAP_RESPONSE   )NULL;
    ULONG                           ulRetryTimes       = 0;
    char                            request_id[16];
   	PCCSP_CWMP_CFG_INTERFACE        pCcspCwmpCfgIf  = (PCCSP_CWMP_CFG_INTERFACE)pCcspCwmpCpeController->hCcspCwmpCfgIf;
    ULONG                           ulRpcCallTimeout= CCSP_CWMPSO_RPCCALL_TIMEOUT;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCwmpRpcTimeout )
    {
        ulRpcCallTimeout = pCcspCwmpCfgIf->GetCwmpRpcTimeout(pCcspCwmpCfgIf->hOwnerContext);
        if ( ulRpcCallTimeout < CCSP_CWMPSO_RPCCALL_TIMEOUT )
        {
            ulRpcCallTimeout = CCSP_CWMPSO_RPCCALL_TIMEOUT;
        }
    }

    *pulErrorCode = 0;

    if ( pMyObject->SessionState != CCSP_CWMPSO_SESSION_STATE_informed )
    {
        return  ANSC_STATUS_FAILURE;
    }

    CcspCwmpsoAllocAsyncRequest(pWmpsoAsyncReq);

    if ( !pWmpsoAsyncReq )
    {
        return  ANSC_STATUS_RESOURCES;
    }
    else
    {
        AnscZeroMemory(request_id, 16);
        _ansc_itoa    (pMyObject->GlobalRequestID++, request_id, 10);

        pWmpsoAsyncReq->CallStatus   = ANSC_STATUS_TIMEOUT;
        pWmpsoAsyncReq->Method       = CCSP_CWMP_METHOD_GetRPCMethods;
        pWmpsoAsyncReq->MethodName   = CcspTr069PaCloneString("GetRPCMethods");
        pWmpsoAsyncReq->RequestID    = CcspTr069PaCloneString(request_id);
        pWmpsoAsyncReq->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapReq_GetRpcMethods
                (
                    (ANSC_HANDLE)pCcspCwmpSoapParser,
                    pWmpsoAsyncReq->RequestID
                );
    }

    if ( !pWmpsoAsyncReq->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }

    do
    {
        AnscAcquireLock   (&pMyObject->AsyncReqQueueLock);
        AnscQueuePushEntry(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
        AnscReleaseLock   (&pMyObject->AsyncReqQueueLock);


        AnscSetEvent(&pMyObject->AsyncProcessEvent);

        /*
         * Now we have to wait...
         */
        AnscWaitEvent (&pWmpsoAsyncReq->AsyncEvent, ulRpcCallTimeout * 1000);
        AnscResetEvent(&pWmpsoAsyncReq->AsyncEvent);

        pCwmpSoapResponse = (PCCSP_CWMP_SOAP_RESPONSE)pWmpsoAsyncReq->hSoapResponse;

        if ( pWmpsoAsyncReq->CallStatus != ANSC_STATUS_SUCCESS )
        {
            returnStatus = pWmpsoAsyncReq->CallStatus;

            if ( pWmpsoAsyncReq->CallStatus == ANSC_STATUS_TIMEOUT )
            {
                AnscQueuePopEntryByLink(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
                AnscQueuePopEntryByLink(&pMyObject->SavedReqQueue, &pWmpsoAsyncReq->Linkage);
            }

            goto  EXIT1;
        }

        if ( pCwmpSoapResponse )
        {
                *ppMethodList = (SLAP_STRING_ARRAY*)pCwmpSoapResponse->hRepArguments;
                *pulErrorCode = 0;

                pCwmpSoapResponse->hRepArguments = (ANSC_HANDLE)NULL;
        }

        break;
    }
    while ( ulRetryTimes < CCSP_CWMPSO_MAX_CALL_RETRY_TIMES );

    returnStatus = ANSC_STATUS_SUCCESS;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    if(pWmpsoAsyncReq )
    {
        CcspCwmpsoFreeAsyncRequest(pWmpsoAsyncReq);
    }

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_disconnectNow;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoInform
            (
                ANSC_HANDLE                 hThisObject,
                PULONG                      pulErrorCode
            );

    description:

        This function is called to initiate a transaction sequence
        whenever a connection to an ACS is established.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                PULONG                      pulErrorCode
                Specifies the error code to be returned.

    return:     status of operation.

**********************************************************************/
//Custom
extern CCSP_VOID
CcspCwmpsoInformCustom
    (
      PCCSP_CWMP_CFG_INTERFACE pCcspCwmpCfgIf
    );
    
extern CCSP_VOID
CcspCwmpsoInformCustom1
    (
       PCCSP_CWMP_EVENT pCwmpEvent,
       PCCSP_CWMP_CFG_INTERFACE pCcspCwmpCfgIf
    );
    
extern ANSC_STATUS
CcspCwmpsoInformPopulateTRInformationCustom
    (
        PCCSP_CWMP_PARAM_VALUE          pCwmpParamValueArray,
        ULONG                           *ulPresetParamCount,
        BOOL                            bDevice20OrLater
    );

ANSC_STATUS
CcspCwmpsoInform
    (
        ANSC_HANDLE                 hThisObject,
        PULONG                      pulErrorCode
    )
{
    ANSC_STATUS                     returnStatus         = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject            = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor    = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController   = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_STAT_INTERFACE            pCcspCwmpStatIf          = (PCCSP_CWMP_STAT_INTERFACE       )pCcspCwmpCpeController->hCcspCwmpStaIf;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser      = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq       = (PCCSP_CWMPSO_ASYNC_REQUEST  )NULL;
    PCCSP_CWMP_SOAP_RESPONSE        pCwmpSoapResponse    = (PCCSP_CWMP_SOAP_RESPONSE   )NULL;
    ULONG                           ulRetryTimes         = (ULONG                      )0;
    PCCSP_CWMP_DEVICE_ID            pCwmpDeviceId        = (PCCSP_CWMP_DEVICE_ID       )NULL;
    PCCSP_CWMP_PARAM_VALUE          pCwmpParamValueArray = (PCCSP_CWMP_PARAM_VALUE     )NULL;
#define  CCSP_CWMPSO_MAX_FORCED_PARAM_COUNT          32
    ULONG                           ulParamCount         = (ULONG                      )pMyObject->ModifiedParamCount + CCSP_CWMPSO_MAX_FORCED_PARAM_COUNT;
    ULONG                           ulParamIndex         = (ULONG                      )0;
    PANSC_UNIVERSAL_TIME            pCurrentSystemTime   = (PANSC_UNIVERSAL_TIME       )NULL;
    int                             iTimeZoneOffset      = (int                        )0;
    ULONG                           ulPresetParamCount   = 0;
    ULONG                           i                    = 0;
    ULONG                           j                    = 0;
    char                            request_id[16];
    char*                           pRootObjName         = pCcspCwmpCpeController->GetRootObject((ANSC_HANDLE)pCcspCwmpCpeController);
    char                            paramName[512];
    int                             notifAttr;
    PCCSP_CWMP_CFG_INTERFACE        pCcspCwmpCfgIf  = (PCCSP_CWMP_CFG_INTERFACE)pCcspCwmpCpeController->hCcspCwmpCfgIf;
    ULONG                           ulRpcCallTimeout= CCSP_CWMPSO_RPCCALL_TIMEOUT;
	BOOL							bIGD				 = TRUE;
    PSLAP_VARIABLE          		pSlapValue			 = NULL;
    BOOL                            bInitialContact      = pCcspCwmpProcessor->GetInitialContact(pCcspCwmpProcessor); 

    BOOL                            bValChange           = FALSE;
    BOOL                            bBootStrap           = FALSE;
    static int bFirstInform = 1;
    static char Manufacturer[100];
    static char ManufacturerOUI[100];
    static char ProductClass[100];
    static char ProvisioningCode[100];
    static char SerialNumber[100];
    static char HardwareVersion[100];
    static char SoftwareVersion[256];
    errno_t     rc =  -1;
    int         ind = -1;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCwmpRpcTimeout )
    {
        ulRpcCallTimeout = pCcspCwmpCfgIf->GetCwmpRpcTimeout(pCcspCwmpCfgIf->hOwnerContext);
        if ( ulRpcCallTimeout < CCSP_CWMPSO_RPCCALL_TIMEOUT )
        {
            ulRpcCallTimeout = CCSP_CWMPSO_RPCCALL_TIMEOUT;
        }
    }

    *pulErrorCode = 0;

    if ( pMyObject->SessionState != CCSP_CWMPSO_SESSION_STATE_connected )
    {
        return  ANSC_STATUS_FAILURE;
    }

    pCwmpDeviceId        = (PCCSP_CWMP_DEVICE_ID  )CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_DEVICE_ID  ));
    pCwmpParamValueArray = (PCCSP_CWMP_PARAM_VALUE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_PARAM_VALUE) * ulParamCount);
    pCurrentSystemTime    = (PANSC_UNIVERSAL_TIME  )CcspTr069PaAllocateMemory(sizeof(ANSC_UNIVERSAL_TIME  ));

    if ( !pCwmpDeviceId || !pCwmpParamValueArray || !pCurrentSystemTime )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }

    /*
     * The Inform message must contain a DeviceID structure, which uniquely identifies the CPE.
     * This structure includes following information:
     *
     *      - Manufacturer: Manufacturer of the device
     *      - OUI         : Organizationally unique identifier of the device manufacturer.
     *                      Represented as a six hexadecimal-digit value using all upper-
     *                      case letters and including any leading zeros.
     *      - ProductClass: Identifier of the class of product for which the serial
     *                      number applies.
     *      - SerialNumber: Identifier of the particular device that is unique for the
     *                      indicated class of product and manufacturer.
     */
if(bFirstInform)
{
    _ansc_sprintf(paramName, "%s%s", pRootObjName, "DeviceInfo.Manufacturer");
    pCcspCwmpCpeController->GetParamStringValue
        (
            (ANSC_HANDLE)pCcspCwmpCpeController,
            paramName,
            &pCwmpDeviceId->Manufacturer
        );
        if(pCwmpDeviceId->Manufacturer != NULL)
        {
          memset(Manufacturer,'\0',sizeof(Manufacturer));
	  strncpy(Manufacturer, pCwmpDeviceId->Manufacturer,sizeof(Manufacturer)-1);
          Manufacturer[sizeof(Manufacturer) - 1 ] = '\0';
        }
         
    _ansc_sprintf(paramName, "%s%s", pRootObjName, "DeviceInfo.ManufacturerOUI");
    pCcspCwmpCpeController->GetParamStringValue
        (
            (ANSC_HANDLE)pCcspCwmpCpeController,
            paramName,
            &pCwmpDeviceId->OUI
        );
        if(pCwmpDeviceId->OUI != NULL)
        {
          memset(ManufacturerOUI,'\0',sizeof(ManufacturerOUI));
	  strncpy(ManufacturerOUI, pCwmpDeviceId->OUI,sizeof(ManufacturerOUI)-1);
          ManufacturerOUI[sizeof(ManufacturerOUI) - 1 ] = '\0';
        }

    _ansc_sprintf(paramName, "%s%s", pRootObjName, "DeviceInfo.ProductClass");
    pCcspCwmpCpeController->GetParamStringValue
        (
            (ANSC_HANDLE)pCcspCwmpCpeController,
            paramName,
            &pCwmpDeviceId->ProductClass
        );
        if(pCwmpDeviceId->ProductClass != NULL)
        {
          memset(ProductClass, '\0' , sizeof(ProductClass));
	  strncpy(ProductClass, pCwmpDeviceId->ProductClass,sizeof(ProductClass) - 1 );
          ProductClass[sizeof(ProductClass) - 1 ] = '\0';
        }
    _ansc_sprintf(paramName, "%s%s", pRootObjName, "DeviceInfo.SerialNumber");
    pCcspCwmpCpeController->GetParamStringValue
        (
            (ANSC_HANDLE)pCcspCwmpCpeController,
            paramName,
            &pCwmpDeviceId->SerialNumber
        );

        if(pCwmpDeviceId->SerialNumber != NULL) 
        {
          memset(SerialNumber, '\0' , sizeof(SerialNumber));
          strncpy(SerialNumber, pCwmpDeviceId->SerialNumber,sizeof(SerialNumber) - 1);
          SerialNumber[sizeof(SerialNumber) - 1 ] = '\0';
        }

    _ansc_sprintf(paramName, "%s%s", pRootObjName, "DeviceInfo.ProvisioningCode");
    pCcspCwmpCpeController->GetParamStringValue
        (
            (ANSC_HANDLE)pCcspCwmpCpeController,
            paramName,
            &pCwmpDeviceId->ProvisioningCode
        );
        if(pCwmpDeviceId->ProvisioningCode != NULL)
        {
           memset(ProvisioningCode, '\0' , sizeof(ProvisioningCode));
           strncpy(ProvisioningCode, pCwmpDeviceId->ProvisioningCode,sizeof(ProvisioningCode) - 1);
           ProvisioningCode[sizeof(ProvisioningCode) - 1 ] = '\0';
        }

	if ( !pCwmpDeviceId->Manufacturer ||
             !pCwmpDeviceId->OUI          ||
             !pCwmpDeviceId->ProductClass ||
             !pCwmpDeviceId->ProvisioningCode ||
             !pCwmpDeviceId->SerialNumber )
	{
		CcspTr069PaTraceWarning(("WARNING: failed to get Manufacturer/OUI/ProductClass/SerialNumber! 'informed' may be rejected by ACS!\n"));
	}
}
else
{

	pCwmpDeviceId->Manufacturer = (char *)CcspManagementServer_CloneString(Manufacturer);
	pCwmpDeviceId->OUI 			= (char *)CcspManagementServer_CloneString(ManufacturerOUI);
	pCwmpDeviceId->ProductClass = (char *)CcspManagementServer_CloneString(ProductClass);
	pCwmpDeviceId->SerialNumber = (char *)CcspManagementServer_CloneString(SerialNumber);
        pCwmpDeviceId->ProvisioningCode = (char *)CcspManagementServer_CloneString(ProvisioningCode);
        CcspTr069PaTraceWarning((" Manufacturer: %s , OUI : %s, ProductClass: %s, SerialNumber: %s, ProvisionCode: %s\n",pCwmpDeviceId->Manufacturer,pCwmpDeviceId->OUI,
                               pCwmpDeviceId->ProductClass,pCwmpDeviceId->SerialNumber,pCwmpDeviceId->ProvisioningCode));
}

    /*
     * The Inform message must contain the current date and time known to the CPE in UTC (defined by original TR-069 spec).
     * Local time with time zone offset and daylight saving must be used.
     */
    AnscGetLocalTime(pCurrentSystemTime);
    iTimeZoneOffset = AnscGetTimeZoneOffset();

	CcspTr069PaTraceDebug
		((
			"LocalTime is %.2d:%.2d:%.2d, zone offset=%d\n",
			pCurrentSystemTime->Hour,
			pCurrentSystemTime->Minute,
			pCurrentSystemTime->Second,
            iTimeZoneOffset
		));


    /*
     * The Inform message must contain an array of informational parameters, specific to a given
     * type of CPE. There're certain parameter MUST be included in the case of the Internet Gateway
     * Device:
     *
     *      - InternetGatewayDevice.DeviceInfo.DeviceSummary
     *      - InternetGatewayDevice.DeviceInfo.SpecVersion
     *      - InternetGatewayDevice.DeviceInfo.HardwareVersion
     *      - InternetGatewayDevice.DeviceInfo.SoftwareVersion
     *      - InternetGatewayDevice.DeviceInfo.ProvisioningCode
     *      - InternetGatewayDevice.ManagementServer.ConnectionRequestURL
     *      - InternetGatewayDevice.ManagementServer.ParameterKey
     *      - InternetGatewayDevice.ManagementServer.AliasBasedAddressing
     *      - InternetGatewayDevice.WANDevice.{i}.WANConectionDevice.{j}.WAN{***}-
     *        Connection.{k}.ExternalIPAddress
     *
     *  For TR106 defined manageable device, the following parameters must be included in each
     * inform.
     *
     *      - Device.DeviceSummary
     *      - Device.DeviceInfo.HardwareVersion
     *      - Device.DeviceInfo.SoftwareVersion
     *      - Device.DeviceInfo.ProvisioningCode
     *      - Device.ManagementServer.ParameterKey
     *      - Device.ManagementServer.ConnectionRequestURL
     *      - Device.ManagementServer.AliasBasedAddressing
     *      - Device.LAN.IPAddress
     *
     */
    ulPresetParamCount = 0;
    rc = strcasecmp_s("Device.",strlen("Device."),pRootObjName,&ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        ULONG                       ulDevDMVerMajor  = 1, ulDevDMVerMinor = 0;
        BOOL                        bDevice20OrLater = FALSE;

		bIGD = FALSE;

        pCcspCwmpCfgIf->GetDevDataModelVer(pCcspCwmpCfgIf->hOwnerContext, &ulDevDMVerMajor, &ulDevDMVerMinor);
        bDevice20OrLater = (ulDevDMVerMajor >= 2);

        returnStatus = CcspCwmpsoInformPopulateTRInformationCustom(pCwmpParamValueArray, &ulPresetParamCount, bDevice20OrLater);
        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            goto EXIT1;
        }
    }
    else
    {
        /*
         * Although the parameter 'InternetGatewayDevice.DeviceInfo.SpecVersion' is deprecated according to WT151,
         * the parameter is forced in Inform messages.
         */
        pCwmpParamValueArray[ulPresetParamCount++].Name  = CcspTr069PaCloneString("InternetGatewayDevice.DeviceSummary"                        );
        pCwmpParamValueArray[ulPresetParamCount++].Name  = CcspTr069PaCloneString("InternetGatewayDevice.DeviceInfo.SpecVersion"               );
        pCwmpParamValueArray[ulPresetParamCount++].Name  = CcspTr069PaCloneString("InternetGatewayDevice.DeviceInfo.HardwareVersion"           );
        pCwmpParamValueArray[ulPresetParamCount++].Name  = CcspTr069PaCloneString("InternetGatewayDevice.DeviceInfo.SoftwareVersion"           );
        pCwmpParamValueArray[ulPresetParamCount++].Name  = CcspTr069PaCloneString("InternetGatewayDevice.DeviceInfo.ProvisioningCode"          );
        pCwmpParamValueArray[ulPresetParamCount++].Name  = CcspTr069PaCloneString("InternetGatewayDevice.ManagementServer.ConnectionRequestURL");
        pCwmpParamValueArray[ulPresetParamCount++].Name  = CcspTr069PaCloneString("InternetGatewayDevice.ManagementServer.ParameterKey"        );

        if ( !pCwmpParamValueArray[0].Name ||
             !pCwmpParamValueArray[1].Name ||
             !pCwmpParamValueArray[2].Name ||
             !pCwmpParamValueArray[3].Name ||
             !pCwmpParamValueArray[4].Name ||
             !pCwmpParamValueArray[5].Name ||
             !pCwmpParamValueArray[6].Name )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            goto  EXIT1;
        }
    }

	/* load customized Forced Inform Parameters if configured through CWMP CFG interface */
	if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCustomForcedInformParams )
	{
		char*						pCFIPs;

		pCFIPs = pCcspCwmpCfgIf->GetCustomForcedInformParams(pCcspCwmpCfgIf->hOwnerContext);

		if ( pCFIPs )
		{
			char*					pParamName, *pNext;

			pParamName = pCFIPs;

			while ( pParamName )
			{
				pNext = _ansc_strchr(pParamName, ',');
				if ( pNext ) *pNext = 0;

        		pCwmpParamValueArray[ulPresetParamCount++].Name  = CcspTr069PaCloneString(pParamName);
				pParamName = pNext ? pNext + 1 : NULL;
			}

			CcspTr069PaFreeMemory(pCFIPs);	
		}
	}

    ulParamIndex = ulPresetParamCount;

    // for "O BOOTSTRAP", all parameters are treated as in their original values, thus all "4 VALUE CHANGE" should be suppressed.
    for( i = 0; i < pMyObject->EventCount; i ++)
    { 
        rc = strcmp_s(CCSP_CWMP_INFORM_EVENT_NAME_Bootstrap,strlen(CCSP_CWMP_INFORM_EVENT_NAME_Bootstrap),((PCCSP_CWMP_EVENT)pMyObject->EventArray[i])->EventCode,&ind);
        ERR_CHK(rc);
        if((!ind) && (rc == EOK))
        {
            bBootStrap = TRUE;
            break;
        }
    }

    if ( !bBootStrap )
    {
        /*
         * When the Inform call results from a change to one or more parameter values (due to cause
         * other than being set by the ACS itself) that the ACS has marked for notification (either
         * active or passive) via SetParameterAttributes, all of the changed parameters must also be
         * included in the ParameterList. However, we must make sure the same parameter is not included
         * twice by comparing the parameter name to all pre-defined parameters.
        */

        for ( i = 0; i < pMyObject->ModifiedParamCount; i++ )
        {
            if ( pMyObject->ModifiedParamArray[i] )
            {
                CcspTr069PaTraceDebug(("CcspCwmpsoInform -- adding VC parameter <%s>.\n", pMyObject->ModifiedParamArray[i]));

                if ( CcspTr069PA_IsNamespaceInvisible
                     (
                         pCcspCwmpCpeController->hTr069PaMapper,
                         pMyObject->ModifiedParamArray[i]
                     ) )
                {
                    CcspTr069PaTraceDebug(("CcspCwmpsoInform -- should not contain any invisible parameters.\n"));
                    continue;
                }
            
                for ( j = 0; j < ulPresetParamCount; j++ )
                {
                    rc = strcmp_s(pMyObject->ModifiedParamArray[i],strlen(pMyObject->ModifiedParamArray[i]),pCwmpParamValueArray[j].Name,&ind);
                    ERR_CHK(rc);
                    if((!ind) && (rc == EOK))
                    {
                        if ( CCSP_CWMP_NOTIFICATION_off != 
                            pCcspCwmpProcessor->CheckParamAttrCache
                                ( (ANSC_HANDLE)pCcspCwmpProcessor, pMyObject->ModifiedParamArray[i]) )
                        {    
                            CcspTr069PaTraceDebug(("<RT> CcspCwmpsoInform -- VC parameter <%s> notification is %d. \n", pMyObject->ModifiedParamArray[i], 
                               pCcspCwmpProcessor->CheckParamAttrCache( (ANSC_HANDLE)pCcspCwmpProcessor, pMyObject->ModifiedParamArray[i])));
                            bValChange = TRUE;
                        }

                        break;
                    }
                }

                if ( j >= ulPresetParamCount )
                {
                    int                 dataType;

                    /* give the namespace last check to make sure the notification has been turned off */
                    if ( CCSP_CWMP_NOTIFICATION_off == 
                        pCcspCwmpProcessor->CheckParamAttrCache
                            ( (ANSC_HANDLE)pCcspCwmpProcessor, pMyObject->ModifiedParamArray[i]) )
                    {
                        continue;
                    }

                    pCwmpParamValueArray[ulParamIndex].Name = CcspTr069PaCloneString(pMyObject->ModifiedParamArray[i]);
                
                    dataType = 
                        pCcspCwmpCpeController->GetParamDataType
                            (
                                (ANSC_HANDLE)pCcspCwmpCpeController,
                                pMyObject->ModifiedParamArray[i]
                            );

                    pCwmpParamValueArray[ulParamIndex].Tr069DataType = dataType;

                    SlapAllocVariable(pCwmpParamValueArray[ulParamIndex].Value);
                    pCwmpParamValueArray[ulParamIndex].Value->Syntax = SLAP_VAR_SYNTAX_TYPE_string;
                    pCwmpParamValueArray[ulParamIndex].Value->Variant.varString = CcspTr069PaCloneString(pMyObject->ModifiedParamValueArray[i]);

                    if ( pCwmpParamValueArray[ulParamIndex].Name )
                    {
                        ulParamIndex++;
                        bValChange = TRUE;
                    }
                    CcspTr069PaTraceDebug(("<RT> CcspCwmpsoInform -- VC parameter <%s> ADDED.\n", pMyObject->ModifiedParamArray[i]));
                }
            }
        }
    }

    if ( bBootStrap || !bValChange ) // "0 BOOTSTRAP" or NO passive/active VALUE CHANGE to be reported
    {
        pMyObject->DiscardCwmpEvent((ANSC_HANDLE)pMyObject, CCSP_CWMPSO_EVENTCODE_ValueChange);

        if ( pMyObject->EventCount == 0 )
        {
            returnStatus = ANSC_STATUS_DISCARD;

            goto  EXIT1;
        }
    }

    CcspTr069PaTraceDebug(("CcspCwmpsoInform -- Start to get the parameter values.\n"));

    for ( i = 0; i < ulParamIndex; i++ )
    {
        notifAttr = 
            pCcspCwmpCpeController->GetParamNotification
                (
                    (ANSC_HANDLE)pCcspCwmpCpeController,
                    paramName
                );

        if ( (i         >= ulPresetParamCount            ) &&
             (notifAttr != CCSP_CWMP_NOTIFICATION_passive) &&
             (notifAttr != CCSP_CWMP_NOTIFICATION_active ) )
        {
            continue;
        }
        else
        {
            char*                   pValue = NULL;

            CcspTr069PaTraceDebug(("CcspCwmpsoInform -- Start to get parameter value #%d: %s.\n", i, pCwmpParamValueArray[i].Name));

            if ( i < ulPresetParamCount )
            {
                pValue = NULL;

         	if((!strcmp("Device.ManagementServer.ConnectionRequestURL",pCwmpParamValueArray[i].Name)) ||
		  (!strcmp("Device.ManagementServer.ParameterKey",pCwmpParamValueArray[i].Name))|| (bFirstInform == 1))
		  {
                     pCcspCwmpCpeController->GetParamStringValue
                     (
                        (ANSC_HANDLE)pCcspCwmpCpeController,
                        pCwmpParamValueArray[i].Name,
                        &pValue
                     );
			if(bFirstInform == 1)
			{
				if(!strcmp("Device.DeviceInfo.HardwareVersion",pCwmpParamValueArray[i].Name))
				{
                                        if(pValue != NULL)
					strcpy(HardwareVersion,pValue);
				}
				else if(!strcmp("Device.DeviceInfo.SoftwareVersion",pCwmpParamValueArray[i].Name))
				{
                                        if(pValue != NULL)
					strcpy(SoftwareVersion,pValue);
				}
                                else if(!strcmp("Device.DeviceInfo.ProvisioningCode",pCwmpParamValueArray[i].Name))
                                {
                                        if(pValue != NULL)
                                        strcpy(ProvisioningCode,pValue);
                                }
					
			}
	     }
             else
             {
		if(!strcmp("Device.DeviceInfo.HardwareVersion",pCwmpParamValueArray[i].Name))
		{
			pValue = (char *)CcspManagementServer_CloneString(HardwareVersion);
		}
		else if(!strcmp("Device.DeviceInfo.SoftwareVersion",pCwmpParamValueArray[i].Name))
		{
			pValue = (char *)CcspManagementServer_CloneString(SoftwareVersion);
		}
                else if(!strcmp("Device.DeviceInfo.ProvisioningCode",pCwmpParamValueArray[i].Name))
                {
                        pCcspCwmpCpeController->GetParamStringValue((ANSC_HANDLE)pCcspCwmpCpeController,pCwmpParamValueArray[i].Name, &pValue);
                }
				

            }

            if ( pValue && pValue[0] != '\0' )
            {
                 SlapAllocVariable(pSlapValue);
                  
                 if ( pSlapValue )
                 {
            	        pSlapValue->Syntax = SLAP_VAR_SYNTAX_string;
                        pSlapValue->Variant.varString = pValue;
    	                pCwmpParamValueArray[i].Value = pSlapValue;
                        pCwmpParamValueArray[i].Tr069DataType = 
                            pCcspCwmpCpeController->GetParamDataType
                            (
                                (ANSC_HANDLE)pCcspCwmpCpeController, 
                                pCwmpParamValueArray[i].Name
                             );
                        pValue = NULL;
                    }
                }
                else { // parameter value is empty
                    CcspTr069PaFreeMemory(pValue); /*RDKB-7326, CID-33499, free resource before exit*/
                    pValue = NULL;
                   rc = strcmp_s("Device.ManagementServer.ConnectionRequestURL",strlen("Device.ManagementServer.ConnectionRequestURL"),pCwmpParamValueArray[i].Name,&ind);
                   if((rc == EOK) && (!ind))
                   {
                        returnStatus = ANSC_STATUS_FAILURE;
                        goto EXIT1;
                   }
                }
                /*RDKB-7326, CID-33499, free resource before exit*/
                if(pValue)
                {
                    CcspTr069PaFreeMemory(pValue);
                    pValue = NULL;
                }
            }
        }
    }
bFirstInform = 0;
    /* special handling of AliasBasedAddressing - backend does not support it, but
     * some test suites such as CD-Router complains its missing and we got failures
     */
    SlapAllocVariable(pSlapValue);
    if ( pSlapValue )
    {
        char*			    pValue = NULL;
        BOOL			    bValue = FALSE;
        char*                       pPName = NULL;
            
        pPName  =  bIGD ? "InternetGatewayDevice.ManagementServer.AliasBasedAddressing" :
            "Device.ManagementServer.AliasBasedAddressing";

        /* check if this parameter has been included */
        for ( j = 0; j < ulParamIndex; j++ )
        {
            rc = strcmp_s(pPName, strlen(pPName), pCwmpParamValueArray[j].Name, &ind);
            ERR_CHK(rc);
            if((!ind) && (rc == EOK))
            {
                break;
            }
        }

        if ( j >= ulParamIndex )
        {
            pValue = NULL;
            pCwmpParamValueArray[ulParamIndex].Name  = CcspTr069PaCloneString(pPName);

	        pCcspCwmpCpeController->GetParamStringValue
    	       (
        	    (ANSC_HANDLE)pCcspCwmpCpeController,
            	pCwmpParamValueArray[ulParamIndex].Name,
	            &pValue
    	       );

		    if ( pValue )
		    {
                           errno_t  rc1 = -1,rc2 = -1;
                           int  notZero = -1, notfalse = -1, notTRUE = -1;
                           rc = strcmp_s("0",strlen("0"),pValue,&notZero);
                           rc1 = strcasecmp_s("false",strlen("false"),pValue,&notfalse);
                           rc2 = strcasecmp_s("TRUE",strlen("TRUE"),pValue,&notTRUE);
 
                            if ((rc == EOK)  && (rc1 == EOK) && (rc2 == EOK))
                            {
                                 bValue = (!((!notZero) || (!notfalse)) || (!notTRUE));
                            } 
                            CcspTr069PaFreeMemory(pValue);
                            pValue = NULL;
		    }

            pSlapValue->Syntax = SLAP_VAR_SYNTAX_string;
            pSlapValue->Variant.varString = bValue ? AnscCloneString("1") : AnscCloneString("0");
            pCwmpParamValueArray[ulParamIndex].Value = pSlapValue;
		    pCwmpParamValueArray[ulParamIndex].Tr069DataType = CCSP_CWMP_TR069_DATA_TYPE_Boolean;

            ulParamIndex++;
            pSlapValue = NULL; /*RDKB-7326, CID-33388, Once Assigned making pSlapValue NULL*/
        }
    }

    /*RDKB-7326, CID-33388, free if not used*/
    if(pSlapValue)
    {
        CcspTr069PaFreeMemory(pSlapValue);
        pSlapValue = NULL;
    }

    /* include the WAN connection IP address defined by TR-098 */
    if ( bIGD )
    {
        char*                       pDefWanConnection     = NULL;
        char*                       pDefWanConnIfIpv4Addr = NULL;
        char*			    pConnReqUrl           = NULL;
            
        pDefWanConnection = CcspManagementServer_GetFirstUpstreamIpAddress(pCcspCwmpCpeController->PANameWithPrefix);
        if ( pDefWanConnection )
        {
            /* check if this parameter has been included */
            for ( j = 0; j < ulParamIndex; j++ )
            {
                rc = strcmp_s(pCwmpParamValueArray[j].Name,strlen(pCwmpParamValueArray[j].Name), pDefWanConnection, &ind);
                if((!ind) && (rc == EOK))
                {
                    break;
                }
            }
            
            if ( j >= ulParamIndex )
            {
			pConnReqUrl = CcspManagementServer_GetConnectionRequestURL(pCcspCwmpCpeController->PANameWithPrefix);
			if ( pConnReqUrl )
			{
				char*				pDomain = NULL;
				char*				pDomainEnd = NULL;

				/* parse Connection Request URL to get the default WAN connection interface IP address */
				pDomain = _ansc_strstr(pConnReqUrl, "://");
			
				if ( pDomain ) 
				{
					char*			pPort = NULL;

					pDomain += 3;

					pPort = _ansc_strstr(pDomain, ":");
					pDomainEnd = _ansc_strstr(pDomain, "/");
					if ( !pDomainEnd && !pPort ) pDomainEnd = pDomain + AnscSizeOfString(pDomain);
					else if ( !pDomainEnd ) pDomainEnd = pPort;
					else if ( pPort < pDomainEnd ) pDomainEnd = pPort;
					
					pDefWanConnIfIpv4Addr = (char*)CcspTr069PaAllocateMemory(pDomainEnd - pDomain + 1);
					if ( pDefWanConnIfIpv4Addr )
					{
						AnscCopyMemory(pDefWanConnIfIpv4Addr, pDomain, pDomainEnd - pDomain);
						pDefWanConnIfIpv4Addr[pDomainEnd - pDomain] = 0;
					}
				}

                pCwmpParamValueArray[ulParamIndex].Name  = AnscCloneString(pDefWanConnection);

			    SlapAllocVariable(pSlapValue);

                if ( pSlapValue )
                {
                    pSlapValue->Syntax = SLAP_VAR_SYNTAX_string;
                    pSlapValue->Variant.varString = pDefWanConnIfIpv4Addr;
                    pCwmpParamValueArray[ulParamIndex].Value = pSlapValue;
                    pCwmpParamValueArray[ulParamIndex].Tr069DataType = CCSP_CWMP_TR069_DATA_TYPE_String;
                    pSlapValue = NULL; /*RDKB-7326, CID-33388, Once Assigned making pSlapValue NULL*/
                    pDefWanConnIfIpv4Addr = NULL;/*RDKB-7326, CID-33309, Once Assigned making pDefWanConnIfIpv4Addr NULL*/
                }

                if(pDefWanConnIfIpv4Addr)
                {
                    CcspTr069PaFreeMemory(pDefWanConnIfIpv4Addr);/*RDKB-7326, CID-33309, free if not used*/
                    pDefWanConnIfIpv4Addr = NULL;
                }
				 ulParamIndex++;

                 CcspTr069PaFreeMemory(pConnReqUrl); 
                 pConnReqUrl = NULL;
			}
            }
            
            CcspTr069PaFreeMemory(pDefWanConnection);			
            pDefWanConnection = NULL;
        }
	}

    /* get undelivered event codes only on the first try */
    if ( pMyObject->RetryCount == 0 && !bInitialContact )
    {
        pCcspCwmpProcessor->GetUndeliveredEvents((ANSC_HANDLE)pCcspCwmpProcessor, (ANSC_HANDLE)pMyObject);
    }
    else if ( bInitialContact )
    {
        pCcspCwmpProcessor->DiscardUndeliveredEvents((ANSC_HANDLE)pCcspCwmpProcessor);
    }

    /*
     * Now we have all the data ready, it's time to create the ASYNC_REQUEST structure and build
     * the SOAP message...
     */
    CcspCwmpsoAllocAsyncRequest(pWmpsoAsyncReq);

    if ( !pWmpsoAsyncReq )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }
    else
    {
        CcspTr069PaTraceDebug(("CcspCwmpsoInform -- Start to build the soap message.\n"));
        ULONG x = 0;
        AnscZeroMemory(request_id, 16);
        _ansc_itoa    (pMyObject->GlobalRequestID++, request_id, 10);

        pWmpsoAsyncReq->CallStatus   = ANSC_STATUS_TIMEOUT;
        pWmpsoAsyncReq->Method       = CCSP_CWMP_METHOD_Inform;
        pWmpsoAsyncReq->MethodName   = CcspTr069PaCloneString("Inform");
        pWmpsoAsyncReq->RequestID    = CcspTr069PaCloneString(request_id);
		 for(x = 0;x<ulParamIndex;x++)
		 {
			char *temp = NULL;
			temp = (char *)malloc(sizeof(char)*(strlen(pCwmpParamValueArray[x].Name) + 1)); // Need to Account for '\0'
			if(temp != NULL)
			{
				strcpy(temp,pCwmpParamValueArray[x].Name);
				CcspCwmppoMpaMapParamInstNumDmIntToCwmp(temp);
				if ( pCwmpParamValueArray[x].Name )
				{
				    CcspTr069PaFreeMemory(pCwmpParamValueArray[x].Name);
				    pCwmpParamValueArray[x].Name = NULL;
				}
				pCwmpParamValueArray[x].Name = CcspTr069PaCloneString(temp);
				free(temp);
			}
		 }

		
        pWmpsoAsyncReq->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapReq_Inform
                (
                    (ANSC_HANDLE)pCcspCwmpSoapParser,
                    (char*      )pWmpsoAsyncReq->RequestID,
                    (ANSC_HANDLE)pCwmpDeviceId,
                    (ANSC_HANDLE)pMyObject->EventArray,
                    (ULONG      )pMyObject->EventCount,
                    (ULONG      )pMyObject->CpeMaxEnvelopes,
                    (ANSC_HANDLE)pCurrentSystemTime,
                    (int        )iTimeZoneOffset,
                    (ULONG      )pMyObject->RetryCount,
                    (ANSC_HANDLE)pCwmpParamValueArray,
                    (ULONG      )ulParamIndex
                );
    }

    if ( !pWmpsoAsyncReq->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT2;
    }

    CcspTr069PaTraceDebug(("CPE Inform Message:\n%s\n", pWmpsoAsyncReq->SoapEnvelope));
    // printf("<RT> CPE Inform Message:\n%s\n", pWmpsoAsyncReq->SoapEnvelope);

    do
    {
        AnscAcquireLock          (&pMyObject->AsyncReqQueueLock);
        AnscQueuePushEntryAtFront(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
        AnscReleaseLock          (&pMyObject->AsyncReqQueueLock);

        CcspTr069PaTraceDebug(("CcspCwmpsoInform -- Start the task 'CcspCwmpsoAsyncProcessTask'.\n"));

        if ( ulRetryTimes == 0 )
        {
            pMyObject->AsyncTaskCount++;
            returnStatus =
                AnscSpawnTask3
                    (
                        (void*)pMyObject->AsyncProcessTask,
                        (ANSC_HANDLE)pMyObject,
                        "CcspCwmpsoAsyncProcessTask",
                        USER_DEFAULT_TASK_PRIORITY,
                        3*USER_DEFAULT_TASK_STACK_SIZE

                     );
        }

        AnscSetEvent(&pMyObject->AsyncProcessEvent);

        /*
         * Now we have to wait...
         */
        CcspTr069PaTraceDebug(("CcspCwmpsoInform -- Wait for the AsyncEvent.\n"));

        AnscWaitEvent (&pWmpsoAsyncReq->AsyncEvent, ulRpcCallTimeout * 1000);
        AnscResetEvent(&pWmpsoAsyncReq->AsyncEvent);

        pCwmpSoapResponse = (PCCSP_CWMP_SOAP_RESPONSE)pWmpsoAsyncReq->hSoapResponse;

        if ( pWmpsoAsyncReq->CallStatus != ANSC_STATUS_SUCCESS )
        {
            /* this includes response with fault code other than 8005 */

            if ( pWmpsoAsyncReq->CallStatus == ANSC_STATUS_TIMEOUT )
            {
                AnscQueuePopEntryByLink(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
                AnscQueuePopEntryByLink(&pMyObject->SavedReqQueue, &pWmpsoAsyncReq->Linkage);

                CcspTr069PaTraceDebug(("====================\nTR069 Session Timeout\n====================\n"));
            }
            else
            {
                CcspTr069PaTraceDebug(("====================\nTR069 Fault\n====================\n"));
            }

            /*
             * Retry the connection...
             */
            pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_reconnect;
            returnStatus            =
                pCcspCwmpProcessor->SignalSession
                    (
                        (ANSC_HANDLE)pCcspCwmpProcessor,
                        (ANSC_HANDLE)pMyObject
                    );

            break;
        }

        if ( pCwmpSoapResponse )
        {
			
                 pMyObject->AcsMaxEnvelopes = (ULONG)pCwmpSoapResponse->hRepArguments;

                /*
                 * ACS indicates the maximum number of SOAP envelopes in a single HTTP post that the ACS
                 * will accept from the CPE. A value of zero indicates that there is no specific limit on
                 * the number of envelopes. In which case, we set it to the same limit as CPE.
                 */
                if ( pMyObject->AcsMaxEnvelopes == 0 )
                {
                    pMyObject->AcsMaxEnvelopes = pMyObject->CpeMaxEnvelopes;
                }

                pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_informed;

                /* 
                 * TODO: we need to acquire session lock from CR if necessary
                 */ 
                returnStatus = ANSC_STATUS_SUCCESS;

                if ( returnStatus != ANSC_STATUS_SUCCESS )
                {
                    pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_abort;

                    CcspTr069PaTraceWarning(("\nWARNING:Failed to LockWriteAccess after 'informed'\n\n"));
                }
                else
                {
                    /* Check for any value changed parameters sent as part of current INFORM,discard those
                       value change parameter entries from  the PSM
                    */
                    ULONG x;
                    for( x = 0; x<ulParamIndex; x++ )
                    {
                        char* pDMParamName = NULL;

                        pDMParamName = CcspTr069PA_MapInstNumCwmpToDmInt(pCwmpParamValueArray[x].Name );
                        if ( pDMParamName != NULL )
                        {
                            pCcspCwmpProcessor->DiscardValueChanged((ANSC_HANDLE)pCcspCwmpProcessor, pDMParamName);
                            CcspTr069PaFreeMemory(pDMParamName);
                            pDMParamName = NULL;
                        }
                        else
                        {
                            pCcspCwmpProcessor->DiscardValueChanged((ANSC_HANDLE)pCcspCwmpProcessor, pCwmpParamValueArray[x].Name);
                        }
                    }
                }

                pCcspCwmpCpeController->bBootInformSent = TRUE;

                pCcspCwmpProcessor->SignalSession
                    (
                        (ANSC_HANDLE)pCcspCwmpProcessor,
                        (ANSC_HANDLE)pMyObject
                    );

                /*
                 *  Record the time of InformResponse 
                 */
                if( pCcspCwmpCpeController != NULL)
                {
                    AnscGetLocalTime(&pCcspCwmpCpeController->cwmpStats.LastInformResponseTime);
                }

                /*
                 * If this is the first time we talk to this particular ACS server (identified by
                 * the "InternetGateway.ManagementServer.URL" parameter), we MUST have included the
                 * "0 BOOTSTRAP" event in the Inform message, which should have been taken care of
                 * by the CcspCwmpAcsBroker object. After the initial contact, we must reset a flag so
                 * following sessions don't send "0 BOOTSTRAP" event. This is done by adding a new
                 * 'invisible' (i.e. invisible to ACS) parameter under the same ManagementServer
                 * object: "InternetGateway.ManagementServer.InitialContact" (boolean).
                 */

                if( pCcspCwmpProcessor->GetInitialContact(pCcspCwmpProcessor))
                {
			        pCcspCwmpProcessor->SetInitialContact((ANSC_HANDLE)pCcspCwmpProcessor, FALSE);
                    CcspTr069PaTraceDebug(("Set 'InitialContact' to FALSE.\n"));

                    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NotifyEvent )
                    {
                        if ( pCcspCwmpProcessor->GetInitialContactFactory(pCcspCwmpProcessor) )
                        {
			     //Custom
			     CcspCwmpsoInformCustom(pCcspCwmpCfgIf);
                            pCcspCwmpProcessor->SetInitialContactFactory((ANSC_HANDLE)pCcspCwmpProcessor, FALSE);
                        }
                        else
                        {
                            pCcspCwmpCfgIf->NotifyEvent(pCcspCwmpCfgIf->hOwnerContext, CCSP_CWMP_CFG_EVENT_CODE_BootstrapInformed);
                        }
                        
                        pCcspCwmpCfgIf->NotifyEvent(pCcspCwmpCfgIf->hOwnerContext, CCSP_CWMP_CFG_EVENT_CODE_BootInformed);
                    }
                }
                else
                {
			if (tlvFileFlag == 0)
			{
				Tr69TlvData tlvdata;
				FILE *file;

				tlvFileFlag = 1;

				file = fopen (TR69_TLVDATA_FILE, "rb");
				if (file != NULL)
				{
					size_t nm = fread (&tlvdata, sizeof(Tr69TlvData), 1, file);
					fclose (file);

					if ((nm == 1) && (tlvdata.Tr69Enable == 0))
					{
						tlvdata.Tr69Enable = 1;
						file = fopen (TR69_TLVDATA_FILE, "wb");
						if (file != NULL)
						{
							fwrite (&tlvdata, sizeof(Tr69TlvData), 1, file);
							fclose (file);
						}
					}
				}
			}

                    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->NotifyEvent )
                    {
                        BOOL                bHasBootEvent = FALSE;

                        PCCSP_CWMP_EVENT    pCwmpEvent;
                        ULONG               i;

                        /* go through event array to see if this inform includes "1 BOOT" event */
                        for ( i = 0; i < pMyObject->EventCount; i ++ )
                        {
                            pCwmpEvent = (PCCSP_CWMP_EVENT)pMyObject->EventArray[i];

                            if ( pCwmpEvent->EventCode && pCwmpEvent->EventCode[0] == CCSP_CWMP_INFORM_EVENT_CODE_Boot )
                            {
                                bHasBootEvent = TRUE;
                                break;
                            }
                            
			     //Custom
			     CcspCwmpsoInformCustom1(pCwmpEvent,pCcspCwmpCfgIf);
                        }

                        if ( bHasBootEvent )
                        {
                            pCcspCwmpCfgIf->NotifyEvent(pCcspCwmpCfgIf->hOwnerContext, CCSP_CWMP_CFG_EVENT_CODE_BootInformed);
                        }
                    }
                }

                if( pCcspCwmpStatIf)
                {
                    pCcspCwmpStatIf->IncTcpSuccess(pCcspCwmpStatIf->hOwnerContext);
                }

                /* we cannot reset retry count and remove all events until the whole session completes successfully,
                 * otherwise, in some cases, CPE won't retry session since all events has been removed.
                 */
#if 0

            /* reset retry count to zero to prevent side effect  */
            pMyObject->RetryCount   = 0;

            /* remove all events from event list */
            pMyObject->DelAllEvents((ANSC_HANDLE)pMyObject);
#endif
        }

        break;
    }
    while ( ulRetryTimes < CCSP_CWMPSO_MAX_CALL_RETRY_TIMES );

    returnStatus = ANSC_STATUS_SUCCESS;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT2:

    if(pWmpsoAsyncReq )
    {
        CcspCwmpsoFreeAsyncRequest(pWmpsoAsyncReq);
    }

EXIT1:

    if ( pCwmpDeviceId )
    {
        CcspCwmpFreeDeviceId(pCwmpDeviceId);
    }

    if ( pCwmpParamValueArray )
    {
        for ( i = 0; i < ulParamIndex; i++ )
        {
            CcspCwmpCleanParamValue((&pCwmpParamValueArray[i]));
        }

        CcspTr069PaFreeMemory(pCwmpParamValueArray);
    }

    if ( pCurrentSystemTime )
    {
        CcspTr069PaFreeMemory(pCurrentSystemTime);
    }

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
	if ( bBootStrap || !bValChange ) // "0 BOOTSTRAP" or NO passive/active VALUE CHANGE to be reported
	{
	    CcspTr069PaTraceWarning(("Discard: 0 BOOTSTRAP or No VALUE CHANGE event\n"));
	}
	else
	{
	    CcspTr069PaTraceError(("!!!Failed to construct Inform message!!!\n"));
	}
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_timeout;    /* retry session to redeliver this RPC */
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoTransferComplete
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pCommandKey,
                ANSC_HANDLE                 hFault,
                ANSC_HANDLE                 hStartTime,
                ANSC_HANDLE                 hCompleteTime,
                PULONG                      pulErrorCode,
                BOOL                        bAddEventCode
            );

    description:

        This function is called to inform the server of the completion
        of a file transfer initiated by an earlier Download or Upload
        method call.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

				BOOL						bIsDownload,
				It's Download or upload;

                char*                       pCommandKey
                Specifies the CommandKey argument passed to CPE in the
                Download or Upload method call that initiated the
                transfer.

                ANSC_HANDLE                 hFault
                If the transfer was successful, the FaultCode is set to
                zero. Otherwise a non-zero FaultCode is specified along
                with a FaultString indicating the failure reason.

                ANSC_HANDLE                 hStartTime
                Specifies the date and time transfer was started in UTC.

                ANSC_HANDLE                 hCompleteTime
                Specifies the date and time transfer completed in UTC.

                PULONG                      pulErrorCode
                Specifies the error code to be returned.

                BOOL                        bAddEventCode
                Specifies if event code shall be added. FALSE is used
                when PA tries to redeliver this RPC.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        char*                       pCommandKey,
        ANSC_HANDLE                 hFault,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq     = (PCCSP_CWMPSO_ASYNC_REQUEST  )NULL;
    PCCSP_CWMP_SOAP_RESPONSE        pCwmpSoapResponse  = (PCCSP_CWMP_SOAP_RESPONSE   )NULL;
    ULONG                           ulRetryTimes       = 0;
    char                            request_id[16];
    BOOL                            bConnectNow        = TRUE;
    PCCSP_CWMP_CFG_INTERFACE        pCcspCwmpCfgIf  = (PCCSP_CWMP_CFG_INTERFACE)pCcspCwmpCpeController->hCcspCwmpCfgIf;
    ULONG                           ulRpcCallTimeout= CCSP_CWMPSO_RPCCALL_TIMEOUT;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCwmpRpcTimeout )
    {
        ulRpcCallTimeout = pCcspCwmpCfgIf->GetCwmpRpcTimeout(pCcspCwmpCfgIf->hOwnerContext);
        if ( ulRpcCallTimeout < CCSP_CWMPSO_RPCCALL_TIMEOUT )
        {
            ulRpcCallTimeout = CCSP_CWMPSO_RPCCALL_TIMEOUT;
        }
    }

    *pulErrorCode = 0;

    /*
     * This method informs the server of completion (either successful or unsuccessful) of a file
     * transfer initiated by an earlier Download or Upload method call. This MUST be called only
     * when the associaetd Download or Upload response indicated that the transfer had not yet
     * completed at that time (indicated by a non-zero value of the Status argument in the response).
     * In such case, it MAY be called either later in the same session in which the transfer was
     * initiated or in any subsequent sessions.
     */
    if ( bAddEventCode )
    {
        PCCSP_CWMP_EVENT            pCcspCwmpEvent = (PCCSP_CWMP_EVENT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_EVENT));

        if ( !pCcspCwmpEvent )
        {
            return  ANSC_STATUS_RESOURCES;
        }

		if( bIsDownload)
		{
	        pCcspCwmpEvent->EventCode  = CcspTr069PaCloneString(CCSP_CWMP_INFORM_EVENT_NAME_M_Download);
		}
		else
		{
	        pCcspCwmpEvent->EventCode  = CcspTr069PaCloneString(CCSP_CWMP_INFORM_EVENT_NAME_M_Upload);
		}

        pCcspCwmpEvent->CommandKey = CcspTr069PaCloneString(pCommandKey);

        returnStatus =
            pMyObject->AddCwmpEvent
                (
                    (ANSC_HANDLE)pMyObject,
                    pCcspCwmpEvent,
                    FALSE
                );

        pCcspCwmpEvent = (PCCSP_CWMP_EVENT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_EVENT));

        if ( !pCcspCwmpEvent )
        {
            return  ANSC_STATUS_RESOURCES;
        }

        pCcspCwmpEvent->EventCode  = CcspTr069PaCloneString(CCSP_CWMP_INFORM_EVENT_NAME_TransferComplete);
        pCcspCwmpEvent->CommandKey = NULL;

        if ( ( pCcspCwmpCpeController->bBootInformScheduled || pCcspCwmpCpeController->bBootstrapInformScheduled ) &&
             !pCcspCwmpCpeController->bDelayedInformCancelled )
        {
            bConnectNow = FALSE;
        }
        else
        {
            /* bConnectNow = (pMyObject->RetryCount == 0); */
            bConnectNow = TRUE;
        }

        returnStatus =
            pMyObject->AddCwmpEvent
                (
                    (ANSC_HANDLE)pMyObject,
                    pCcspCwmpEvent,
                    bConnectNow && pCcspCwmpCpeController->bBootInformSent
                );
    }

    CcspCwmpsoAllocAsyncRequest(pWmpsoAsyncReq);

    if ( !pWmpsoAsyncReq )
    {
        return  ANSC_STATUS_RESOURCES;
    }
    else
    {
        AnscZeroMemory(request_id, 16);
        _ansc_itoa    (pMyObject->GlobalRequestID++, request_id, 10);

        pWmpsoAsyncReq->CallStatus   = ANSC_STATUS_TIMEOUT;
        pWmpsoAsyncReq->Method       = CCSP_CWMP_METHOD_TransferComplete;
        pWmpsoAsyncReq->MethodName   = CcspTr069PaCloneString("TransferComplete");
        pWmpsoAsyncReq->RequestID    = CcspTr069PaCloneString(request_id);
        pWmpsoAsyncReq->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapReq_TransferComplete
                (
                    (ANSC_HANDLE)pCcspCwmpSoapParser,
                    pWmpsoAsyncReq->RequestID,
                    pCommandKey,
                    hFault,
                    hStartTime,
                    hCompleteTime
                );
    }

    if ( !pWmpsoAsyncReq->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }

    do
    {
        AnscAcquireLock   (&pMyObject->AsyncReqQueueLock);
        AnscQueuePushEntry(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
        AnscReleaseLock   (&pMyObject->AsyncReqQueueLock);

        AnscSetEvent(&pMyObject->AsyncProcessEvent);

        /*
         * Now we have to wait...
         */
        AnscWaitEvent (&pWmpsoAsyncReq->AsyncEvent, ulRpcCallTimeout * 1000);
        AnscResetEvent(&pWmpsoAsyncReq->AsyncEvent);

        pCwmpSoapResponse = (PCCSP_CWMP_SOAP_RESPONSE)pWmpsoAsyncReq->hSoapResponse;

        if ( pWmpsoAsyncReq->CallStatus != ANSC_STATUS_SUCCESS )
        {
            returnStatus = pWmpsoAsyncReq->CallStatus;

            if ( pWmpsoAsyncReq->CallStatus == ANSC_STATUS_TIMEOUT )
            {
                AnscQueuePopEntryByLink(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
                AnscQueuePopEntryByLink(&pMyObject->SavedReqQueue, &pWmpsoAsyncReq->Linkage);
            }

            goto  EXIT1;
        }

        if ( pCwmpSoapResponse )
        {
            /*
             * There's nothing to do...
             */
            *pulErrorCode = 0;
        }

        break;
    }
    while ( ulRetryTimes < CCSP_CWMPSO_MAX_CALL_RETRY_TIMES );

    returnStatus = ANSC_STATUS_SUCCESS;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    if(pWmpsoAsyncReq )
    {
        CcspCwmpsoFreeAsyncRequest(pWmpsoAsyncReq);
    }

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        if ( (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_connected) ||
             (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_informed ) )
        {
            pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_timeout;    /* retry session to redeliver this RPC */
            pCcspCwmpProcessor->SignalSession
                (
                    (ANSC_HANDLE)pCcspCwmpProcessor,
                    (ANSC_HANDLE)pMyObject
                );
        }
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoAutonomousTransferComplete
            (
                ANSC_HANDLE                 hThisObject,
		        BOOL						bIsDownload,
                ANSC_HANDLE                 hFault,             
                ANSC_HANDLE                 hStartTime,         
                ANSC_HANDLE                 hCompleteTime,      
                char*                       AnnounceURL,
                char*                       TransferURL,
                char*                       FileType,
                unsigned int                FileSize,
                char*                       TargetFileName,
                PULONG                      pulErrorCode,
                BOOL                        bAddEventCode
            )

    description:

        This function is called to inform the server of the completion
        of a file transfer initiated by other means than ACS.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

				BOOL						bIsDownload,
				It's Download or upload;

                ANSC_HANDLE                 hFault
                If the transfer was successful, the FaultCode is set to
                zero. Otherwise a non-zero FaultCode is specified along
                with a FaultString indicating the failure reason.

                ANSC_HANDLE                 hStartTime
                Specifies the date and time transfer was started in UTC.

                ANSC_HANDLE                 hCompleteTime
                Specifies the date and time transfer completed in UTC.

                char*                       AnnounceURL
                Announce URL.

                char*                       TransferURL
                Transfer URL.

                char*                       FileType
                File type.

                unsigned int                FileSize
                File size.

                char*                       TargetFileName
                Target file name.

                PULONG                      pulErrorCode
                Specifies the error code to be returned.

                BOOL                        bAddEventCode
                Specifies if event code shall be added. FALSE is used
                when PA tries to redeliver this RPC.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoAutonomousTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        ANSC_HANDLE                 hFault,             
        ANSC_HANDLE                 hStartTime,         
        ANSC_HANDLE                 hCompleteTime,      
        char*                       AnnounceURL,
        char*                       TransferURL,
        char*                       FileType,
        unsigned int                FileSize,
        char*                       TargetFileName,
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq     = (PCCSP_CWMPSO_ASYNC_REQUEST  )NULL;
    PCCSP_CWMP_SOAP_RESPONSE        pCwmpSoapResponse  = (PCCSP_CWMP_SOAP_RESPONSE   )NULL;
    ULONG                           ulRetryTimes       = 0;
    char                            request_id[16];
    BOOL                            bConnectNow        = TRUE;
    PCCSP_CWMP_CFG_INTERFACE        pCcspCwmpCfgIf  = (PCCSP_CWMP_CFG_INTERFACE)pCcspCwmpCpeController->hCcspCwmpCfgIf;
    ULONG                           ulRpcCallTimeout= CCSP_CWMPSO_RPCCALL_TIMEOUT;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCwmpRpcTimeout )
    {
        ulRpcCallTimeout = pCcspCwmpCfgIf->GetCwmpRpcTimeout(pCcspCwmpCfgIf->hOwnerContext);
        if ( ulRpcCallTimeout < CCSP_CWMPSO_RPCCALL_TIMEOUT )
        {
            ulRpcCallTimeout = CCSP_CWMPSO_RPCCALL_TIMEOUT;
        }
    }

    *pulErrorCode = 0;

    /*
     * This method informs the server of completion (either successful or unsuccessful) of a file
     * transfer initiated by a means other than ACS.
     */
    if ( bAddEventCode )
    {
        PCCSP_CWMP_EVENT            pCcspCwmpEvent = (PCCSP_CWMP_EVENT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_EVENT));

        if ( !pCcspCwmpEvent )
        {
            return  ANSC_STATUS_RESOURCES;
        }

        pCcspCwmpEvent->EventCode  = CcspTr069PaCloneString(CCSP_CWMP_INFORM_EVENT_NAME_AutonomousTransferComplete);
        pCcspCwmpEvent->CommandKey = NULL;

        if ( ( pCcspCwmpCpeController->bBootInformScheduled || pCcspCwmpCpeController->bBootstrapInformScheduled ) &&
             !pCcspCwmpCpeController->bDelayedInformCancelled )
        {
            bConnectNow = FALSE;
        }
        else
        {
            /* bConnectNow = (pMyObject->RetryCount == 0); */
            bConnectNow = TRUE;
        }

        returnStatus =
            pMyObject->AddCwmpEvent
                (
                    (ANSC_HANDLE)pMyObject,
                    pCcspCwmpEvent,
                    bConnectNow && pCcspCwmpCpeController->bBootInformSent
                );
    }

    CcspCwmpsoAllocAsyncRequest(pWmpsoAsyncReq);

    if ( !pWmpsoAsyncReq )
    {
        return  ANSC_STATUS_RESOURCES;
    }
    else
    {
        AnscZeroMemory(request_id, 16);
        _ansc_itoa    (pMyObject->GlobalRequestID++, request_id, 10);

        pWmpsoAsyncReq->CallStatus   = ANSC_STATUS_TIMEOUT;
        pWmpsoAsyncReq->Method       = CCSP_CWMP_METHOD_AutonomousTransferComplete;
        pWmpsoAsyncReq->MethodName   = CcspTr069PaCloneString("AutonomousTransferComplete");
        pWmpsoAsyncReq->RequestID    = CcspTr069PaCloneString(request_id);
        pWmpsoAsyncReq->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapReq_AutonomousTransferComplete
                (
                    (ANSC_HANDLE)pCcspCwmpSoapParser,
                    pWmpsoAsyncReq->RequestID,
                    hFault,
                    hStartTime,
                    hCompleteTime,
                    bIsDownload,
                    AnnounceURL,
                    TransferURL,
                    FileType,
                    FileSize,
                    TargetFileName
                );
    }

    if ( !pWmpsoAsyncReq->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }

    do
    {
        AnscAcquireLock   (&pMyObject->AsyncReqQueueLock);
        AnscQueuePushEntry(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
        AnscReleaseLock   (&pMyObject->AsyncReqQueueLock);

        AnscSetEvent(&pMyObject->AsyncProcessEvent);

        /*
         * Now we have to wait...
         */
        AnscWaitEvent (&pWmpsoAsyncReq->AsyncEvent, ulRpcCallTimeout * 1000);
        AnscResetEvent(&pWmpsoAsyncReq->AsyncEvent);

        pCwmpSoapResponse = (PCCSP_CWMP_SOAP_RESPONSE)pWmpsoAsyncReq->hSoapResponse;

        if ( pWmpsoAsyncReq->CallStatus != ANSC_STATUS_SUCCESS )
        {
            returnStatus = pWmpsoAsyncReq->CallStatus;

            if ( pWmpsoAsyncReq->CallStatus == ANSC_STATUS_TIMEOUT )
            {
                AnscQueuePopEntryByLink(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
                AnscQueuePopEntryByLink(&pMyObject->SavedReqQueue, &pWmpsoAsyncReq->Linkage);
            }

            goto  EXIT1;
        }

        if ( pCwmpSoapResponse )
        {
            /*
             * There's nothing to do...
             */
            *pulErrorCode = 0;
        }

        break;
    }
    while ( ulRetryTimes < CCSP_CWMPSO_MAX_CALL_RETRY_TIMES );

    returnStatus = ANSC_STATUS_SUCCESS;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    if(pWmpsoAsyncReq )
    {
        CcspCwmpsoFreeAsyncRequest(pWmpsoAsyncReq);
    }

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        if ( (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_connected) ||
             (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_informed ) )
        {
            pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_timeout;    /* retry session to redeliver this RPC */
            pCcspCwmpProcessor->SignalSession
                (
                    (ANSC_HANDLE)pCcspCwmpProcessor,
                    (ANSC_HANDLE)pMyObject
                );
        }
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoKicked
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pCommand,
                char*                       pReferer,
                char*                       pArg,
                char*                       pNext,
                char**                      ppNextUrl,
                PULONG                      pulErrorCode
            );

    description:

        This function is called whenever the CPE is "kicked".

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pCommand
                Specifies a generic argument that may be used by the
                server for identification or other purposes.

                char*                       pReferer
                Specifies the content of the "Referer" HTTP header sent
                to the CPE when it was kicked.

                char*                       pArg
                Specifies a generic argument that may be used by the
                server for identification or other purposes.

                char*                       pNext
                Specifies the URL the server should return in the
                method response under normal conditions.

                char**                      ppNextUrl
                Returns the next URL the user's browser should be
                redirected to.

                PULONG                      pulErrorCode
                Specifies the error code to be returned.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoKicked
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCommand,
        char*                       pReferer,
        char*                       pArg,
        char*                       pNext,
        char**                      ppNextUrl,
        PULONG                      pulErrorCode
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq     = (PCCSP_CWMPSO_ASYNC_REQUEST  )NULL;
    PCCSP_CWMP_SOAP_RESPONSE        pCwmpSoapResponse  = (PCCSP_CWMP_SOAP_RESPONSE   )NULL;
    ULONG                           ulRetryTimes       = 0;
    char                            request_id[16];
    PCCSP_CWMP_CFG_INTERFACE        pCcspCwmpCfgIf  = (PCCSP_CWMP_CFG_INTERFACE)pCcspCwmpCpeController->hCcspCwmpCfgIf;
    ULONG                           ulRpcCallTimeout= CCSP_CWMPSO_RPCCALL_TIMEOUT;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCwmpRpcTimeout )
    {
        ulRpcCallTimeout = pCcspCwmpCfgIf->GetCwmpRpcTimeout(pCcspCwmpCfgIf->hOwnerContext);
        if ( ulRpcCallTimeout < CCSP_CWMPSO_RPCCALL_TIMEOUT )
        {
            ulRpcCallTimeout = CCSP_CWMPSO_RPCCALL_TIMEOUT;
        }
    }

    *ppNextUrl    = NULL;
    *pulErrorCode = 0;

    /*
     * The CPE's invocation of the Kicked method is initiated by an external stimulus to the CPE.
     * This external stimulus is assumed to be web-based, and the thus the associated method
     * provides a means to communicate information that would be useful in a web-based transaction.
     * The information contained in the Kicked method call includes both the information needed to
     * uniquely identify the CPE, but also parameters that may be used to associate the method call
     * with a particular web browser session.
     */
    if ( (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_idle      ) ||
         (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_connectNow) )
    {
        PCCSP_CWMP_EVENT            pCcspCwmpEvent = (PCCSP_CWMP_EVENT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_EVENT));

        if ( !pCcspCwmpEvent )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pCcspCwmpEvent->EventCode  = CcspTr069PaCloneString("5 KICKED");
            pCcspCwmpEvent->CommandKey = NULL;
        }

        returnStatus =
            pMyObject->AddCwmpEvent
                (
                    (ANSC_HANDLE)pMyObject,
                    pCcspCwmpEvent,
                    pCcspCwmpCpeController->bBootInformSent
                );
    }

    CcspCwmpsoAllocAsyncRequest(pWmpsoAsyncReq);

    if ( !pWmpsoAsyncReq )
    {
        return  ANSC_STATUS_RESOURCES;
    }
    else
    {
        AnscZeroMemory(request_id, 16);
        _ansc_itoa    (pMyObject->GlobalRequestID++, request_id, 10);

        pWmpsoAsyncReq->CallStatus   = ANSC_STATUS_TIMEOUT;
        pWmpsoAsyncReq->Method       = CCSP_CWMP_METHOD_Kicked;
        pWmpsoAsyncReq->MethodName   = CcspTr069PaCloneString("Kicked");
        pWmpsoAsyncReq->RequestID    = CcspTr069PaCloneString(request_id);
        pWmpsoAsyncReq->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapReq_Kicked
                (
                    (ANSC_HANDLE)pCcspCwmpSoapParser,
                    pWmpsoAsyncReq->RequestID,
                    pCommand,
                    pReferer,
                    pArg,
                    pNext
                );
    }

    if ( !pWmpsoAsyncReq->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }

    do
    {
        AnscAcquireLock   (&pMyObject->AsyncReqQueueLock);
        AnscQueuePushEntry(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
        AnscReleaseLock   (&pMyObject->AsyncReqQueueLock);

        AnscSetEvent(&pMyObject->AsyncProcessEvent);

        /*
         * Now we have to wait...
         */
        AnscWaitEvent (&pWmpsoAsyncReq->AsyncEvent, ulRpcCallTimeout * 1000);
        AnscResetEvent(&pWmpsoAsyncReq->AsyncEvent);

        pCwmpSoapResponse = (PCCSP_CWMP_SOAP_RESPONSE)pWmpsoAsyncReq->hSoapResponse;

        if ( pWmpsoAsyncReq->CallStatus != ANSC_STATUS_SUCCESS )
        {
            returnStatus = pWmpsoAsyncReq->CallStatus;

            if ( pWmpsoAsyncReq->CallStatus == ANSC_STATUS_TIMEOUT )
            {
                AnscQueuePopEntryByLink(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
                AnscQueuePopEntryByLink(&pMyObject->SavedReqQueue, &pWmpsoAsyncReq->Linkage);
            }

            goto  EXIT1;
        }

        if ( pCwmpSoapResponse )
        {
            *ppNextUrl    = (char*)pCwmpSoapResponse->hRepArguments;
            *pulErrorCode = 0;

            pCwmpSoapResponse->hRepArguments = (ANSC_HANDLE)NULL;
        }

        break;
    }
    while ( ulRetryTimes < CCSP_CWMPSO_MAX_CALL_RETRY_TIMES );

    returnStatus = ANSC_STATUS_SUCCESS;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    if(pWmpsoAsyncReq )
    {
        CcspCwmpsoFreeAsyncRequest(pWmpsoAsyncReq);
    }

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        if ( (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_connected) ||
             (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_informed ) )
        {
            pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_disconnectNow;
            pCcspCwmpProcessor->SignalSession
                (
                    (ANSC_HANDLE)pCcspCwmpProcessor,
                    (ANSC_HANDLE)pMyObject
                );
        }
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoRequestDownload
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pFileType,
                ANSC_HANDLE                 hFileTypeArgArray,
                ULONG                       ulArraySize,
                PULONG                      pulErrorCode
            );

    description:

        This function is called to request a file download from the
        server. On reception of this request, the server MAY call the
        Download method to initiate the download.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pFileType
                Specifies the file type being requested.

                ANSC_HANDLE                 hFileTypeArgArray
                Specifies the array of zero or more additional
                arguments, where each argument is a structure of
                name-value pairs.

                ULONG                       ulArraySize
                Specifies the number of name-value pairs in the
                'hFileTypeArgArray' argument.

                PULONG                      pulErrorCode
                Specifies the error code to be returned.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoRequestDownload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pFileType,
        ANSC_HANDLE                 hFileTypeArgArray,
        ULONG                       ulArraySize,
        PULONG                      pulErrorCode
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq     = (PCCSP_CWMPSO_ASYNC_REQUEST  )NULL;
    PCCSP_CWMP_SOAP_RESPONSE        pCwmpSoapResponse  = (PCCSP_CWMP_SOAP_RESPONSE   )NULL;
    ULONG                           ulRetryTimes       = 0;
    char                            request_id[16];
    PCCSP_CWMP_CFG_INTERFACE        pCcspCwmpCfgIf  = (PCCSP_CWMP_CFG_INTERFACE)pCcspCwmpCpeController->hCcspCwmpCfgIf;
    ULONG                           ulRpcCallTimeout= CCSP_CWMPSO_RPCCALL_TIMEOUT;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCwmpRpcTimeout )
    {
        ulRpcCallTimeout = pCcspCwmpCfgIf->GetCwmpRpcTimeout(pCcspCwmpCfgIf->hOwnerContext);
        if ( ulRpcCallTimeout < CCSP_CWMPSO_RPCCALL_TIMEOUT )
        {
            ulRpcCallTimeout = CCSP_CWMPSO_RPCCALL_TIMEOUT;
        }
    }

    *pulErrorCode = 0;

    if ( pMyObject->SessionState != CCSP_CWMPSO_SESSION_STATE_informed )
    {
        return  ANSC_STATUS_FAILURE;
    }

    CcspCwmpsoAllocAsyncRequest(pWmpsoAsyncReq);

    if ( !pWmpsoAsyncReq )
    {
        return  ANSC_STATUS_RESOURCES;
    }
    else
    {
        AnscZeroMemory(request_id, 16);
        _ansc_itoa    (pMyObject->GlobalRequestID++, request_id, 10);

        pWmpsoAsyncReq->CallStatus   = ANSC_STATUS_TIMEOUT;
        pWmpsoAsyncReq->Method       = CCSP_CWMP_METHOD_RequestDownload;
        pWmpsoAsyncReq->MethodName   = CcspTr069PaCloneString("RequestDownload");
        pWmpsoAsyncReq->RequestID    = CcspTr069PaCloneString(request_id);
        pWmpsoAsyncReq->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapReq_RequestDownload
                (
                    (ANSC_HANDLE)pCcspCwmpSoapParser,
                    pWmpsoAsyncReq->RequestID,
                    pFileType,
                    hFileTypeArgArray,
                    ulArraySize
                );
    }

    if ( !pWmpsoAsyncReq->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }

    do
    {
        AnscAcquireLock   (&pMyObject->AsyncReqQueueLock);
        AnscQueuePushEntry(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
        AnscReleaseLock   (&pMyObject->AsyncReqQueueLock);

        AnscSetEvent(&pMyObject->AsyncProcessEvent);

        /*
         * Now we have to wait...
         */
        AnscWaitEvent (&pWmpsoAsyncReq->AsyncEvent, ulRpcCallTimeout * 1000);
        AnscResetEvent(&pWmpsoAsyncReq->AsyncEvent);

        pCwmpSoapResponse = (PCCSP_CWMP_SOAP_RESPONSE)pWmpsoAsyncReq->hSoapResponse;

        if ( pWmpsoAsyncReq->CallStatus != ANSC_STATUS_SUCCESS )
        {
            returnStatus = pWmpsoAsyncReq->CallStatus;

            if ( pWmpsoAsyncReq->CallStatus == ANSC_STATUS_TIMEOUT )
            {
                AnscQueuePopEntryByLink(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
                AnscQueuePopEntryByLink(&pMyObject->SavedReqQueue, &pWmpsoAsyncReq->Linkage);
            }

            goto  EXIT1;
        }

        if ( pCwmpSoapResponse )
        {
            *pulErrorCode = 0;
        }

        break;
    }
    while ( ulRetryTimes < CCSP_CWMPSO_MAX_CALL_RETRY_TIMES );

    returnStatus = ANSC_STATUS_SUCCESS;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    if(pWmpsoAsyncReq )
    {
        CcspCwmpsoFreeAsyncRequest(pWmpsoAsyncReq);
    }

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_disconnectNow;
        pCcspCwmpProcessor->SignalSession
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                (ANSC_HANDLE)pMyObject
            );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoDUStateChangeComplete
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hDsccReq,
                char*                       pCommandKey,
                PULONG                      pulErrorCode,
                BOOL                        bAddEventCode
            );

    description:

        This function is called to inform the server of the completion
        of a file transfer initiated by an earlier Download or Upload
        method call.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hDsccReq,
                DSCC request.

                char*                       pCommandKey
                Command key.

                PULONG                      pulErrorCode
                Specifies the error code to be returned.

                BOOL                        bAddEventCode
                Specifies if event code shall be added. FALSE is used
                when PA tries to redeliver this RPC.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hDsccReq,
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_TR069_DSCC_REQ            pDsccReq           = (PCCSP_TR069_DSCC_REQ       )hDsccReq;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq     = (PCCSP_CWMPSO_ASYNC_REQUEST  )NULL;
    PCCSP_CWMP_SOAP_RESPONSE        pCwmpSoapResponse  = (PCCSP_CWMP_SOAP_RESPONSE   )NULL;
    ULONG                           ulRetryTimes       = 0;
    char                            request_id[16];
    BOOL                            bConnectNow        = TRUE;
    char*                           pCommandKey        = pDsccReq->CommandKey;
    PCCSP_CWMP_CFG_INTERFACE        pCcspCwmpCfgIf  = (PCCSP_CWMP_CFG_INTERFACE)pCcspCwmpCpeController->hCcspCwmpCfgIf;
    ULONG                           ulRpcCallTimeout= CCSP_CWMPSO_RPCCALL_TIMEOUT;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCwmpRpcTimeout )
    {
        ulRpcCallTimeout = pCcspCwmpCfgIf->GetCwmpRpcTimeout(pCcspCwmpCfgIf->hOwnerContext);
        if ( ulRpcCallTimeout < CCSP_CWMPSO_RPCCALL_TIMEOUT )
        {
            ulRpcCallTimeout = CCSP_CWMPSO_RPCCALL_TIMEOUT;
        }
    }

    *pulErrorCode = 0;

    /*
     * This method informs the server of completion (either successful or unsuccessful) of a file
     * transfer initiated by a means other than ACS.
     */
    if ( bAddEventCode )
    {
        PCCSP_CWMP_EVENT            pCcspCwmpEvent = (PCCSP_CWMP_EVENT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_EVENT));

        if ( !pCcspCwmpEvent )
        {
            return  ANSC_STATUS_RESOURCES;
        }

	    pCcspCwmpEvent->EventCode  = CcspTr069PaCloneString(CCSP_CWMP_INFORM_EVENT_NAME_M_ChangeDUState);

        pCcspCwmpEvent->CommandKey = CcspTr069PaCloneString(pCommandKey);

        returnStatus =
            pMyObject->AddCwmpEvent
                (
                    (ANSC_HANDLE)pMyObject,
                    pCcspCwmpEvent,
                    FALSE
                );

        pCcspCwmpEvent = (PCCSP_CWMP_EVENT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_EVENT));

        if ( !pCcspCwmpEvent )
        {
            return  ANSC_STATUS_RESOURCES;
        }

        pCcspCwmpEvent->EventCode  = CcspTr069PaCloneString(CCSP_CWMP_INFORM_EVENT_NAME_DUStateChangeComplete);
        pCcspCwmpEvent->CommandKey = NULL; 

        if ( ( pCcspCwmpCpeController->bBootInformScheduled || pCcspCwmpCpeController->bBootstrapInformScheduled ) &&
             !pCcspCwmpCpeController->bDelayedInformCancelled )
        {
            bConnectNow = FALSE;
        }
        else
        {
            /* bConnectNow = (pMyObject->RetryCount == 0); */
            bConnectNow = TRUE;
        }

        returnStatus =
            pMyObject->AddCwmpEvent
                (
                    (ANSC_HANDLE)pMyObject,
                    pCcspCwmpEvent,
                    bConnectNow && pCcspCwmpCpeController->bBootInformSent
                );
    }

    CcspCwmpsoAllocAsyncRequest(pWmpsoAsyncReq);

    if ( !pWmpsoAsyncReq )
    {
        return  ANSC_STATUS_RESOURCES;
    }
    else
    {
        AnscZeroMemory(request_id, 16);
        _ansc_itoa    (pMyObject->GlobalRequestID++, request_id, 10);

        pWmpsoAsyncReq->CallStatus   = ANSC_STATUS_TIMEOUT;
        pWmpsoAsyncReq->Method       = CCSP_CWMP_METHOD_AutonomousTransferComplete;
        pWmpsoAsyncReq->MethodName   = CcspTr069PaCloneString("AutonomousTransferComplete");
        pWmpsoAsyncReq->RequestID    = CcspTr069PaCloneString(request_id);
        pWmpsoAsyncReq->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapReq_DUStateChangeComplete
                (
                    (ANSC_HANDLE)pCcspCwmpSoapParser,
                    pWmpsoAsyncReq->RequestID,
                    hDsccReq,
                    pCommandKey
                );
    }

    if ( !pWmpsoAsyncReq->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }

    do
    {
        AnscAcquireLock   (&pMyObject->AsyncReqQueueLock);
        AnscQueuePushEntry(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
        AnscReleaseLock   (&pMyObject->AsyncReqQueueLock);

        AnscSetEvent(&pMyObject->AsyncProcessEvent);

        /*
         * Now we have to wait...
         */
        AnscWaitEvent (&pWmpsoAsyncReq->AsyncEvent, ulRpcCallTimeout * 1000);
        AnscResetEvent(&pWmpsoAsyncReq->AsyncEvent);

        pCwmpSoapResponse = (PCCSP_CWMP_SOAP_RESPONSE)pWmpsoAsyncReq->hSoapResponse;

        if ( pWmpsoAsyncReq->CallStatus != ANSC_STATUS_SUCCESS )
        {
            returnStatus = pWmpsoAsyncReq->CallStatus;

            if ( pWmpsoAsyncReq->CallStatus == ANSC_STATUS_TIMEOUT )
            {
                AnscQueuePopEntryByLink(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
                AnscQueuePopEntryByLink(&pMyObject->SavedReqQueue, &pWmpsoAsyncReq->Linkage);
            }

            goto  EXIT1;
        }

        if ( pCwmpSoapResponse )
        {
            /*
             * There's nothing to do...
             */
            *pulErrorCode = 0;
        }

        break;
    }
    while ( ulRetryTimes < CCSP_CWMPSO_MAX_CALL_RETRY_TIMES );

    returnStatus = ANSC_STATUS_SUCCESS;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    if(pWmpsoAsyncReq )
    {
        CcspCwmpsoFreeAsyncRequest(pWmpsoAsyncReq);
    }

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        if ( (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_connected) ||
             (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_informed ) )
        {
            pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_timeout;    /* retry session to redeliver this RPC */
            pCcspCwmpProcessor->SignalSession
                (
                    (ANSC_HANDLE)pCcspCwmpProcessor,
                    (ANSC_HANDLE)pMyObject
                );
        }
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoAutonomousDUStateChangeComplete
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hAdsccReq,
                PULONG                      pulErrorCode,
                BOOL                        bAddEventCode
            );

    description:

        This function is called to inform the server of the completion
        of a file transfer initiated by an earlier Download or Upload
        method call.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hAdsccReq
                ADSCC request.

                PULONG                      pulErrorCode
                Specifies the error code to be returned.

                BOOL                        bAddEventCode
                Specifies if event code shall be added. FALSE is used
                when PA tries to redeliver this RPC.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoAutonomousDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hAdsccReq,
        PULONG                      pulErrorCode,
        BOOL                        bAddEventCode
    )
{
    ANSC_STATUS                     returnStatus       = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SESSION_OBJECT        pMyObject          = (PCCSP_CWMP_SESSION_OBJECT   )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SOAP_PARSER_OBJECT        pCcspCwmpSoapParser    = (PCCSP_CWMP_SOAP_PARSER_OBJECT   )pCcspCwmpCpeController->hCcspCwmpSoapParser;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq     = (PCCSP_CWMPSO_ASYNC_REQUEST  )NULL;
    PCCSP_CWMP_SOAP_RESPONSE        pCwmpSoapResponse  = (PCCSP_CWMP_SOAP_RESPONSE   )NULL;
    ULONG                           ulRetryTimes       = 0;
    char                            request_id[16];
    BOOL                            bConnectNow        = TRUE;
    PCCSP_CWMP_CFG_INTERFACE        pCcspCwmpCfgIf  = (PCCSP_CWMP_CFG_INTERFACE)pCcspCwmpCpeController->hCcspCwmpCfgIf;
    ULONG                           ulRpcCallTimeout= CCSP_CWMPSO_RPCCALL_TIMEOUT;

    if ( pCcspCwmpCfgIf && pCcspCwmpCfgIf->GetCwmpRpcTimeout )
    {
        ulRpcCallTimeout = pCcspCwmpCfgIf->GetCwmpRpcTimeout(pCcspCwmpCfgIf->hOwnerContext);
        if ( ulRpcCallTimeout < CCSP_CWMPSO_RPCCALL_TIMEOUT )
        {
            ulRpcCallTimeout = CCSP_CWMPSO_RPCCALL_TIMEOUT;
        }
    }

    *pulErrorCode = 0;

    /*
     * This method informs the server of completion (either successful or unsuccessful) of a file
     * transfer initiated by a means other than ACS.
     */
    if ( bAddEventCode )
    {
        PCCSP_CWMP_EVENT            pCcspCwmpEvent = (PCCSP_CWMP_EVENT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_EVENT));

        if ( !pCcspCwmpEvent )
        {
            return  ANSC_STATUS_RESOURCES;
        }

        pCcspCwmpEvent->EventCode  = CcspTr069PaCloneString(CCSP_CWMP_INFORM_EVENT_NAME_AutonomousDUStateChangeComplete);
        pCcspCwmpEvent->CommandKey = NULL;

        if ( ( pCcspCwmpCpeController->bBootInformScheduled || pCcspCwmpCpeController->bBootstrapInformScheduled ) &&
             !pCcspCwmpCpeController->bDelayedInformCancelled )
        {
            bConnectNow = FALSE;
        }
        else
        {
            /* bConnectNow = (pMyObject->RetryCount == 0); */
            bConnectNow = TRUE;
        }

        returnStatus =
            pMyObject->AddCwmpEvent
                (
                    (ANSC_HANDLE)pMyObject,
                    pCcspCwmpEvent,
                    bConnectNow && pCcspCwmpCpeController->bBootInformSent
                );
    }

    CcspCwmpsoAllocAsyncRequest(pWmpsoAsyncReq);

    if ( !pWmpsoAsyncReq )
    {
        return  ANSC_STATUS_RESOURCES;
    }
    else
    {
        AnscZeroMemory(request_id, 16);
        _ansc_itoa    (pMyObject->GlobalRequestID++, request_id, 10);

        pWmpsoAsyncReq->CallStatus   = ANSC_STATUS_TIMEOUT;
        pWmpsoAsyncReq->Method       = CCSP_CWMP_METHOD_AutonomousTransferComplete;
        pWmpsoAsyncReq->MethodName   = CcspTr069PaCloneString("AutonomousTransferComplete");
        pWmpsoAsyncReq->RequestID    = CcspTr069PaCloneString(request_id);
        pWmpsoAsyncReq->SoapEnvelope =
            pCcspCwmpSoapParser->BuildSoapReq_AutonomousDUStateChangeComplete
                (
                    (ANSC_HANDLE)pCcspCwmpSoapParser,
                    pWmpsoAsyncReq->RequestID,
                    hAdsccReq
                );
    }

    if ( !pWmpsoAsyncReq->SoapEnvelope )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }

    do
    {
        AnscAcquireLock   (&pMyObject->AsyncReqQueueLock);
        AnscQueuePushEntry(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
        AnscReleaseLock   (&pMyObject->AsyncReqQueueLock);

        AnscSetEvent(&pMyObject->AsyncProcessEvent);

        /*
         * Now we have to wait...
         */
        AnscWaitEvent (&pWmpsoAsyncReq->AsyncEvent, ulRpcCallTimeout * 1000);
        AnscResetEvent(&pWmpsoAsyncReq->AsyncEvent);

        pCwmpSoapResponse = (PCCSP_CWMP_SOAP_RESPONSE)pWmpsoAsyncReq->hSoapResponse;

        if ( pWmpsoAsyncReq->CallStatus != ANSC_STATUS_SUCCESS )
        {
            returnStatus = pWmpsoAsyncReq->CallStatus;

            if ( pWmpsoAsyncReq->CallStatus == ANSC_STATUS_TIMEOUT )
            {
                AnscQueuePopEntryByLink(&pMyObject->AsyncReqQueue, &pWmpsoAsyncReq->Linkage);
                AnscQueuePopEntryByLink(&pMyObject->SavedReqQueue, &pWmpsoAsyncReq->Linkage);
            }

            goto  EXIT1;
        }

        if ( pCwmpSoapResponse )
        {
            /*
             * There's nothing to do...
             */
            *pulErrorCode = 0;
        }

        break;
    }
    while ( ulRetryTimes < CCSP_CWMPSO_MAX_CALL_RETRY_TIMES );

    returnStatus = ANSC_STATUS_SUCCESS;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    if(pWmpsoAsyncReq )
    {
        CcspCwmpsoFreeAsyncRequest(pWmpsoAsyncReq);
    }

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        if ( (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_connected) ||
             (pMyObject->SessionState == CCSP_CWMPSO_SESSION_STATE_informed ) )
        {
            pMyObject->SessionState = CCSP_CWMPSO_SESSION_STATE_timeout;    /* retry session to redeliver this RPC */
            pCcspCwmpProcessor->SignalSession
                (
                    (ANSC_HANDLE)pCcspCwmpProcessor,
                    (ANSC_HANDLE)pMyObject
                );
        }
    }

    return  returnStatus;
}


