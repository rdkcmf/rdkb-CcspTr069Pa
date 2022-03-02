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

    module: ccsp_cwmp_soappo_build.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced operation functions
        of the CCSP CWMP SoapParser Object.

        *   CcspCwmpSoappoBuildSoapReq_GetRpcMethods
        *   CcspCwmpSoappoBuildSoapReq_Inform
        *   CcspCwmpSoappoBuildSoapReq_TransferComplete
        *   CcspCwmpSoappoBuildSoapReq_Kicked
        *   CcspCwmpSoappoBuildSoapReq_RequestDownload
        *   CcspCwmpSoappoBuildSoapReq_AutonomousTransferComplete
        *   CcspCwmpSoappoBuildSoapReq_DUStateChangeComplete
        *   CcspCwmpSoappoBuildSoapReq_AutonomousDUStateChangeComplete
        *   CcspCwmpSoappoBuildSoapRep_Error
        *   CcspCwmpSoappoBuildSoapRep_GetRPCMethods
        *   CcspCwmpSoappoBuildSoapRep_SetParameterValues
        *   CcspCwmpSoappoBuildSoapRep_GetParameterValues
        *   CcspCwmpSoappoBuildSoapRep_GetParameterNames
        *   CcspCwmpSoappoBuildSoapRep_SetParameterAttributes
        *   CcspCwmpSoappoBuildSoapRep_GetParameterAttributes
        *   CcspCwmpSoappoBuildSoapRep_AddObject
        *   CcspCwmpSoappoBuildSoapRep_DeleteObject
        *   CcspCwmpSoappoBuildSoapRep_Download
        *   CcspCwmpSoappoBuildSoapRep_Reboot
        *   CcspCwmpSoappoBuildSoapRep_GetQueuedTransfers
        *   CcspCwmpSoappoBuildSoapRep_ScheduleInform
        *   CcspCwmpSoappoBuildSoapRep_SetVouchers
        *   CcspCwmpSoappoBuildSoapRep_GetOptions
        *   CcspCwmpSoappoBuildSoapRep_Upload
        *   CcspCwmpSoappoBuildSoapRep_FactoryReset
        *   CcspCwmpSoappoBuildSoapRepFault

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin  Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        05/07/05    initial revision.
        01/02/11    Kang added support of following RPCs 
                        AutonomousTransferComplete
                        DUStateChangeComplete 
                        AutonomousDUStateChangeComplete
        07/21/11    Kangg modified Inform and 
                    GetParameterValuesResponse functions
                    to make use TR069 data type associated
                    with CWMP value structure instead of
                    checking SLAP variable's syntax since
                    under CCSP, all values are in string
                    format.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/

#include "ccsp_cwmp_soappo_global.h"

BOOL                                bAddDataTypeAttr             = TRUE;
BOOL                                bAddArrayTypeAttr            = TRUE;


#define  CcspCwmpSoappoBuildSoap_ParamDataTypeAttr(pChildNode, tr069DataType)               \
                                                                                        \
    if (bAddDataTypeAttr)                                                               \
    {                                                                                   \
        switch ( tr069DataType )                                                        \
        {                                                                               \
            case    CCSP_CWMP_TR069_DATA_TYPE_Int:                                           \
                                                                                        \
                    AnscXmlDomNodeSetAttrString                                         \
                        (                                                               \
                            pChildNode,                                                 \
                            "xsi:type",                                                 \
                            "xsd:int",                                                  \
                            AnscSizeOfString("xsd:int")                                 \
                        );                                                              \
                                                                                        \
                    break;                                                              \
                                                                                        \
            case    CCSP_CWMP_TR069_DATA_TYPE_UnsignedInt:                                   \
                                                                                        \
                    AnscXmlDomNodeSetAttrString                                         \
                        (                                                               \
                            pChildNode,                                                 \
                            "xsi:type",                                                 \
                            "xsd:unsignedInt",                                          \
                            AnscSizeOfString("xsd:unsignedInt")                         \
                        );                                                              \
                                                                                        \
                    break;                                                              \
                                                                                        \
            case    CCSP_CWMP_TR069_DATA_TYPE_Boolean:                                       \
                                                                                        \
                    AnscXmlDomNodeSetAttrString                                         \
                        (                                                               \
                            pChildNode,                                                 \
                            "xsi:type",                                                 \
                            "xsd:boolean",                                              \
                            AnscSizeOfString("xsd:boolean")                             \
                        );                                                              \
                                                                                        \
                    break;                                                              \
                                                                                        \
            case    CCSP_CWMP_TR069_DATA_TYPE_DateTime:                                      \
                                                                                        \
                    AnscXmlDomNodeSetAttrString                                         \
                        (                                                               \
                            pChildNode,                                                 \
                            "xsi:type",                                                 \
                            "xsd:dateTime",                                             \
                            AnscSizeOfString("xsd:dateTime")                            \
                        );                                                              \
                                                                                        \
                    break;                                                              \
                                                                                        \
            case    CCSP_CWMP_TR069_DATA_TYPE_Base64:                                        \
                                                                                        \
                    AnscXmlDomNodeSetAttrString                                         \
                        (                                                               \
                            pChildNode,                                                 \
                            "xsi:type",                                                 \
                            "soapenc:base64",                                           \
                            AnscSizeOfString("soapenc:base64")                          \
                        );                                                              \
                                                                                        \
                    break;                                                              \
                                                                                        \
            default:                                                                    \
                                                                                        \
                    AnscXmlDomNodeSetAttrString                                         \
                        (                                                               \
                            pChildNode,                                                 \
                            "xsi:type",                                                 \
                            "xsd:string",                                               \
                            AnscSizeOfString("xsd:string")                              \
                        );                                                              \
                                                                                        \
                    break;                                                              \
        }                                                                               \
    }


/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapReq_GetRpcMethods
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID
            );

    description:

        This function is called to build SOAP request message of
        method "GetRPCMethods"

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapReq_GetRpcMethods
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the request node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:GetRPCMethods",
            CCSP_CWMP_NAMESPACE
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}
/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapReq_Inform
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hDeviceId,
                ANSC_HANDLE                 hEventArray,
                ULONG                       ulEventCount,
                ULONG                       ulMaxEnvelopes,
                ANSC_HANDLE                 hCurrentTime,
                int                         iTimeZoneOffset,
                ULONG                       ulRetryCount,
                ANSC_HANDLE                 hParamValueArray,
                ULONG                       ulArraySize
            );

    description:

        This function is called to build SOAP request message of
        method "Inform"

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hDeviceId,
                The DeviceIdStruct handle;

                ANSC_HANDLE                 hEventArray,
                The Event array handle;

                ULONG                       ulEventCount,
                The Event count;

                ULONG                       ulMaxEnvelopes,
                The maxi envelopes can handle;

                ANSC_HANDLE                 hCurrentTime,
                Current time;

                int                         iTimeZoneOffset,
                timezone information in seconds;

                ULONG                       ulRetryCount,
                Retry count;

                ANSC_HANDLE                 hParamValueArray,
                The parameter value array;

                ULONG                       ulArraySize
                The size of the array

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapReq_Inform
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hDeviceId,
        ANSC_HANDLE                 hEventArray,
        ULONG                       ulEventCount,
        ULONG                       ulMaxEnvelopes,
        ANSC_HANDLE                 hCurrentTime,
        int                         iTimeZoneOffset,
        ULONG                       ulRetryCount,
        ANSC_HANDLE                 hParamValueArray,
        ULONG                       ulArraySize
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pTempNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCCSP_CWMP_DEVICE_ID            pDeviceId    = (PCCSP_CWMP_DEVICE_ID)hDeviceId;
    PANSC_HANDLE                    pEventArray  = (PANSC_HANDLE)hEventArray;
    PCCSP_CWMP_EVENT                pCwmpEvent   = (PCCSP_CWMP_EVENT)NULL;
    PCCSP_CWMP_PARAM_VALUE          pValueArray  = (PCCSP_CWMP_PARAM_VALUE)hParamValueArray;
    PCCSP_CWMP_PARAM_VALUE          pCwmpValue   = (PCCSP_CWMP_PARAM_VALUE)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    PANSC_UNIVERSAL_TIME            pTime        = NULL;
    CHAR                            pNodeName[64]= { 0 };
	CHAR							pAttrName[64]= { 0 };
	CHAR							pAttrVal[64] = { 0 };
    ULONG                           length       = 0;
    ULONG                           i            = 0;
    ULONG                           eventCount   = 0;

    if( hDeviceId == NULL)
    {
        CcspTr069PaTraceError(("NULL DeviceId in Inform building function.\n"));
        return NULL;
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the request node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:Inform",
            CCSP_CWMP_NAMESPACE
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);


    if( TRUE )
    {
        /*
         *  Create and add "DeviceId" node
         */
        pTempNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pTempNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pTempNode, "DeviceId");
        AnscXmlDomNodeAddChild(pParentNode,pTempNode);

        if( pTempNode != NULL)
        {
            /*
             *  Create and add "Manufacturer" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "Manufacturer");
            AnscXmlDomNodeAddChild(pTempNode,pChildNode);

            if( pDeviceId->Manufacturer != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pDeviceId->Manufacturer,
                        AnscSizeOfString(pDeviceId->Manufacturer)
                    );
            }

            /*
             *  Create and add "OUI" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "OUI");
            AnscXmlDomNodeAddChild(pTempNode,pChildNode);

            if( pDeviceId->OUI != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pDeviceId->OUI,
                        AnscSizeOfString(pDeviceId->OUI)
                    );
            }

            /*
             *  Create and add "ProduceClass" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "ProductClass");
            AnscXmlDomNodeAddChild(pTempNode,pChildNode);

            if( pDeviceId->ProductClass != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pDeviceId->ProductClass,
                        AnscSizeOfString(pDeviceId->ProductClass)
                    );
            }

            /*
             *  Create and add "SerialNumber" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "SerialNumber");
            AnscXmlDomNodeAddChild(pTempNode,pChildNode);

            if( pDeviceId->SerialNumber != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pDeviceId->SerialNumber,
                        AnscSizeOfString(pDeviceId->SerialNumber)
                    );
            }

        }

        /*
         *  Create and add "Event" node
         */
        pListNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pListNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pListNode, "Event");
        AnscXmlDomNodeAddChild(pParentNode,pListNode);

		if( bAddArrayTypeAttr)
		{
			/*
			 *	According to WT151, the arrayType is supposed to be specified when
			 *  there's an array in the SOAP message
			 *
			 *  This attribute should be added here
			 *
			 *	soap-enc:arrayType="cwmp:EventStruct[nCount]"
			 */
			_ansc_sprintf
				(
					pAttrName,
					"%s:arrayType",
					NODE_SOAP_ENCODING
				);

			_ansc_sprintf
				(
					pAttrVal,
					"%s:EventStruct[%lu]",
					CCSP_CWMP_NAMESPACE,
					ulEventCount
				);

			AnscXmlDomNodeSetAttrString
				(
					pListNode,
					pAttrName,
					pAttrVal,
					AnscSizeOfString(pAttrVal)
				);

		}

        for( i = 0; i < ulEventCount; i ++)
        {
            pCwmpEvent = (PCCSP_CWMP_EVENT)pEventArray[i];

            if( pCwmpEvent->EventCode == NULL || AnscSizeOfString(pCwmpEvent->EventCode) == 0)
            {
                CcspTr069PaTraceWarning(("Empty EventCode '%lu' in Inform message, ignored!\n", i));

                continue;
            }

            /*
             *  Create and add "EventStruct" node
             */
            pTempNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pTempNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pTempNode, "EventStruct");
            AnscXmlDomNodeAddChild(pListNode,pTempNode);

            if( pTempNode != NULL)
            {
                /*
                 *  Create and add "EventCode" node
                 */
                pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

                if( pChildNode == NULL)
                {
                    goto EXIT;
                }

                AnscXmlDomNodeSetName(pChildNode, "EventCode");
                AnscXmlDomNodeAddChild(pTempNode,pChildNode);

                if( pCwmpEvent->EventCode != NULL)
                {
                	AnscXmlDomNodeSetDataString
                        (
                            pChildNode,
                            NULL,
                            pCwmpEvent->EventCode,
                            AnscSizeOfString(pCwmpEvent->EventCode)
                        );

                    CcspTr069PaTraceDebug(("CwmpEvent->EventCode = %s\n", pCwmpEvent->EventCode));
                }

                /*
                 *  Create and add "CommandKey" node
                 */
                pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

                if( pChildNode == NULL)
                {
                    goto EXIT;
                }

                AnscXmlDomNodeSetName(pChildNode, "CommandKey");
                AnscXmlDomNodeAddChild(pTempNode,pChildNode);

                if( pCwmpEvent->CommandKey != NULL)
                {
                	AnscXmlDomNodeSetDataString
                        (
                            pChildNode,
                            NULL,
                            pCwmpEvent->CommandKey,
                            AnscSizeOfString(pCwmpEvent->CommandKey)
                        );
                }
            }
        }

        /*
         *  Create and add "MaxEnvelopes" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "MaxEnvelopes");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        AnscXmlDomNodeSetDataUlong
            (
                pChildNode,
                NULL,
                ulMaxEnvelopes
            );

        /*
         *  Create and add "CurrentTime" node
         */
        pTime = (PANSC_UNIVERSAL_TIME)hCurrentTime;
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "CurrentTime");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pTime != NULL)
        {
            if( iTimeZoneOffset >= 0)
            {
                _ansc_sprintf
                    (
                        pNodeName,
                        "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d+%.2d:%.2d",
                        pTime->Year,
                        pTime->Month,
                        pTime->DayOfMonth,
                        pTime->Hour,
                        pTime->Minute,
                        pTime->Second,
                        iTimeZoneOffset / 3600,
                        (iTimeZoneOffset % 3600)/60
                    );
            }
            else
            {
                _ansc_sprintf
                    (
                        pNodeName,
                        "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d-%.2d:%.2d",
                        pTime->Year,
                        pTime->Month,
                        pTime->DayOfMonth,
                        pTime->Hour,
                        pTime->Minute,
                        pTime->Second,
                        (iTimeZoneOffset * (-1)) / 3600,
                        ((iTimeZoneOffset * (-1)) % 3600)/60
                    );
            }

            AnscXmlDomNodeSetDataString
                (
                    pChildNode,
                    NULL,
                    pNodeName,
                    AnscSizeOfString(pNodeName)
                );
        }

        /*
         *  Create and add "RetryCount" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "RetryCount");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        AnscXmlDomNodeSetDataUlong
            (
                pChildNode,
                NULL,
                ulRetryCount
            );

        /*
         *  Create and add "ParameterList" node
         */
        pListNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pListNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pListNode, "ParameterList");
        AnscXmlDomNodeAddChild(pParentNode,pListNode);

        for( i  = 0; i < ulArraySize; i ++)
        {
            pCwmpValue = &pValueArray[i];

            if( pCwmpValue->Name != NULL && AnscSizeOfString(pCwmpValue->Name) > 0)
            {

                eventCount ++;

                /*
                 *  Create and add "ParameterValueStruct" node
                 */
                pTempNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

                if( pTempNode == NULL)
                {
                    goto EXIT;
                }

                AnscXmlDomNodeSetName(pTempNode, "ParameterValueStruct");
                AnscXmlDomNodeAddChild(pListNode,pTempNode);

                if( pTempNode != NULL)
                {
                    /*
                     *  Create and add "Name" node
                     */
                    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

                    if( pChildNode == NULL)
                    {
                        goto EXIT;
                    }

                    AnscXmlDomNodeSetName(pChildNode, "Name");

                    if( pCwmpValue->Name != NULL)
                    {
                    	AnscXmlDomNodeSetDataString
                            (
                                pChildNode,
                                NULL,
                                pCwmpValue->Name,
                                AnscSizeOfString(pCwmpValue->Name)
                            );
                    }
                    else
                    {
                        CcspTr069PaTraceError(("ERROR:Empty parameter name encountered.\n"));

                        AnscXmlDomNodeRemove(pChildNode);

                        goto EXIT;
                    }

                    AnscXmlDomNodeAddChild(pTempNode,pChildNode);

                    /*
                     *  Create and add "Value" node
                     */
                    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

                    if( pChildNode == NULL)
                    {
                        goto EXIT;
                    }

                    AnscXmlDomNodeSetName(pChildNode, "Value");
                    AnscXmlDomNodeAddChild(pTempNode,pChildNode);

                    if( pCwmpValue->Value != NULL && pCwmpValue->Value->Syntax == SLAP_VAR_SYNTAX_string )
                    {
                        CcspCwmpSoappoBuildSoap_ParamDataTypeAttr
                            (
                                pChildNode,
                                pCwmpValue->Tr069DataType
                            );

                        AnscXmlDomNodeSetDataString
                            (
                                pChildNode,
                                NULL,
                                pCwmpValue->Value->Variant.varString,
                                AnscSizeOfString(pCwmpValue->Value->Variant.varString)
                            );
                    }
                    else
                    {
                    	AnscXmlDomNodeSetAttrString
                            (
                                pChildNode,
                                "xsi:type",
                                "xsd:string",
                                AnscSizeOfString("xsd:string")
                            );
                    }
                }
            }
        }

		if( bAddArrayTypeAttr)
		{
			/*
			 *	According to WT151, the arrayType is supposed to be specified when
			 *  there's an array in the SOAP message
			 *
			 *  This attribute should be added here
			 *
			 *	soap-enc:arrayType="cwmp:ParameterValueStruct[nCount]"
			 */
			_ansc_sprintf
				(
					pAttrName,
					"%s:arrayType",
					NODE_SOAP_ENCODING
				);

			_ansc_sprintf
				(
					pAttrVal,
					"%s:ParameterValueStruct[%lu]",
					CCSP_CWMP_NAMESPACE,
					eventCount
				);

			AnscXmlDomNodeSetAttrString
				(
					pListNode,
					pAttrName,
					pAttrVal,
					AnscSizeOfString(pAttrVal)
				);

		}

    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapReq_TransferComplete
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                char*                       pCommandKey,
                ANSC_HANDLE                 hFault,
                ANSC_HANDLE                 hStartTime,
                ANSC_HANDLE                 hCompleteTime
            );

    description:

        This function is called to build SOAP request message of
        method "TransferComplete"

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                char*                       pCommandKey,
                The command key;

                ANSC_HANDLE                 hFault,
                The Fault handle;

                ANSC_HANDLE                 hStartTime,
                The start time;

                ANSC_HANDLE                 hCompleteTime
                The complete time;

                ANSC_HANDLE                 hDeviceId,
                The device id;

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapReq_TransferComplete
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pCommandKey,
        ANSC_HANDLE                 hFault,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pTempNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCCSP_CWMP_FAULT                pCwmpFault   = (PCCSP_CWMP_FAULT)hFault;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    PANSC_UNIVERSAL_TIME            pTime        = NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the request node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:TransferComplete",
            CCSP_CWMP_NAMESPACE
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);


    if( TRUE )
    {
        /*
         *  Create and add "CommandKey" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "CommandKey");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pCommandKey != NULL)
        {
        	AnscXmlDomNodeSetDataString
                (
                    pChildNode,
                    NULL,
                    pCommandKey,
                    AnscSizeOfString(pCommandKey)
                );
        }

        /*
         *  Create and add "Fault" node
         */
        pTempNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pTempNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pTempNode, "FaultStruct");
        AnscXmlDomNodeAddChild(pParentNode,pTempNode);

        if( pTempNode != NULL)
        {
            /*
             *  Create and add "FaultCode" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "FaultCode");
            AnscXmlDomNodeAddChild(pTempNode,pChildNode);

            if( pCwmpFault != NULL)
            {
            	AnscXmlDomNodeSetDataUlong
                    (
                        pChildNode,
                        NULL,
                        pCwmpFault->FaultCode
                    );
            }
            else
            {
            	AnscXmlDomNodeSetDataUlong
                    (
                        pChildNode,
                        NULL,
                        0
                    );
            }

            /*
             *  Create and add "FaultString" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "FaultString");
            AnscXmlDomNodeAddChild(pTempNode,pChildNode);

            if( pCwmpFault != NULL && pCwmpFault->FaultString != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pCwmpFault->FaultString,
                        AnscSizeOfString(pCwmpFault->FaultString)
                    );
            }
        }

        /*
         *  Create and add "StartTime" node
         */
        pTime = (PANSC_UNIVERSAL_TIME)hStartTime;
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "StartTime");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pTime != NULL)
        {
            _ansc_sprintf
                (
                    pNodeName,
                    "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
                    pTime->Year,
                    pTime->Month,
                    pTime->DayOfMonth,
                    pTime->Hour,
                    pTime->Minute,
                    pTime->Second
                );
        }
        else
        {
            _ansc_sprintf(pNodeName, "%s", "0001-01-01T00:00:00Z");
        }

        AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pNodeName,
                AnscSizeOfString(pNodeName)
            );

        /*
         *  Create and add "CompleteTime" node
         */
        pTime = (PANSC_UNIVERSAL_TIME)hCompleteTime;
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "CompleteTime");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pTime != NULL)
        {
            _ansc_sprintf
                (
                    pNodeName,
                    "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
                    pTime->Year,
                    pTime->Month,
                    pTime->DayOfMonth,
                    pTime->Hour,
                    pTime->Minute,
                    pTime->Second
                );
        }
        else
        {
            _ansc_sprintf(pNodeName, "%s", "0001-01-01T00:00:00Z");
        }
        
        AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pNodeName,
                AnscSizeOfString(pNodeName)
            );
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    CcspTr069PaTraceDebug(("Building TransferComplete is done.\n"));

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapReq_Kicked
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                char*                       pCommand,
                char*                       pReferer,
                char*                       pArg,
                char*                       pNext
            );

    description:

        This function is called to build SOAP request message of
        method "Kicked".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                char*                       pCommand,
                The command string;

                char*                       pReferer,
                The referer string;

                char*                       pArg,
                The Argument string;

                char*                       pNext
                The next url;

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapReq_Kicked
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pCommand,
        char*                       pReferer,
        char*                       pArg,
        char*                       pNext
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the request node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:Kicked",
            CCSP_CWMP_NAMESPACE
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);


    if( TRUE )
    {
        /*
         *  Create and add "Command" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "Command");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pCommand != NULL)
        {
        	AnscXmlDomNodeSetDataString
                (
                    pChildNode,
                    NULL,
                    pCommand,
                    AnscSizeOfString(pCommand)
                );
        }

        /*
         *  Create and add "Referer" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "Referer");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pReferer != NULL)
        {
        	AnscXmlDomNodeSetDataString
                (
                    pChildNode,
                    NULL,
                    pReferer,
                    AnscSizeOfString(pReferer)
                );
        }

        /*
         *  Create and add "Arg" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "Arg");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pArg != NULL)
        {
        	AnscXmlDomNodeSetDataString
                (
                    pChildNode,
                    NULL,
                    pArg,
                    AnscSizeOfString(pArg)
                );
        }

        /*
         *  Create and add "Next" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "Next");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pCommand != NULL)
        {
        	AnscXmlDomNodeSetDataString
                (
                    pChildNode,
                    NULL,
                    pNext,
                    AnscSizeOfString(pNext)
                );
        }

    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}
/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapReq_RequestDownload
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                char*                       pFileType,
                ANSC_HANDLE                 hFileTypeArgArray,
                ULONG                       ulArraySize
            );

    description:

        This function is called to build SOAP response message of
        method "RequestDownload".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                char*                       pFileType,
                The FileType string;

                ANSC_HANDLE                 hFileTypeArgArray,
                The FileTypeArgument array;

                ULONG                       ulArraySize
                The count of the array;

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapReq_RequestDownload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pFileType,
        ANSC_HANDLE                 hFileTypeArgArray,
        ULONG                       ulArraySize
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCCSP_CWMP_FILE_TYPE            pTypeArray   = (PCCSP_CWMP_FILE_TYPE)hFileTypeArgArray;
    PCCSP_CWMP_FILE_TYPE            pCwmpType    = (PCCSP_CWMP_FILE_TYPE)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;
    ULONG                           i            = 0;

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:RequestDownload",
            CCSP_CWMP_NAMESPACE
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);

    /*
     *  Create and add "FileType" node;
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pChildNode, "FileType");
    AnscXmlDomNodeAddChild(pBodyNode,pChildNode);

    if( pFileType != NULL)
    {
    	AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pFileType,
                AnscSizeOfString(pFileType)
            );
    }

    /*
     *  Create and add the FileTypeArg node
     */
    pListNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pListNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pListNode, "FileTypeArg");
    AnscXmlDomNodeAddChild(pParentNode,pListNode);

    for( i = 0; i < ulArraySize; i ++)
    {
        pCwmpType  = &pTypeArray[i];

        /*
         *  Create and add the ArgStruct node
         */
        pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pParentNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pParentNode, "ArgStruct");
        AnscXmlDomNodeAddChild(pListNode,pParentNode);

        if( TRUE )
        {
            /*
             *  Create and add "Name" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "Name");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pCwmpType->Name != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pCwmpType->Name,
                        AnscSizeOfString(pCwmpType->Name)
                    );
            }

            /*
             *  Create and add "Value" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "Value");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pCwmpType->Value != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pCwmpType->Value,
                        AnscSizeOfString(pCwmpType->Value)
                    );
            }

        }
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}


/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapReq_AutonomousTransferComplete
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hFault,
                ANSC_HANDLE                 hStartTime,
                ANSC_HANDLE                 hCompleteTime,
		        BOOL						bIsDownload,
                char*                       pAnnounceURL,
                char*                       pTransferURL,
                char*                       pFileType,
                unsigned int                FileSize,
                char*                       pTargetFileName
            )

    description:

        This function is called to build SOAP request message of
        method "TransferComplete"

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hFault,
                The Fault handle;

                ANSC_HANDLE                 hStartTime,
                The start time;

                ANSC_HANDLE                 hCompleteTime
                The complete time;

		        BOOL						bIsDownload
                Specify download or upload was performed.

                char*                       pAnnounceURL
                Annouce URL.

                char*                       pTransferURL
                Transfer URL.

                char*                       pFileType
                File type.

                unsigned int                FileSize
                File size.

                char*                       pTargetFileName
                Target file name.

    return:     the status of the operation

**********************************************************************/

char*
CcspCwmpSoappoBuildSoapReq_AutonomousTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hFault,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
		BOOL						bIsDownload,
        char*                       pAnnounceURL,
        char*                       pTransferURL,
        char*                       pFileType,
        unsigned int                FileSize,
        char*                       pTargetFileName
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pTempNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCCSP_CWMP_FAULT                pCwmpFault   = (PCCSP_CWMP_FAULT)hFault;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    PANSC_UNIVERSAL_TIME            pTime        = NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle(pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the request node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:AutonomousTransferComplete",
            CCSP_CWMP_NAMESPACE
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);


    if( TRUE )
    {
        /*
         *  Create and add "AnnounceURL" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "AnnounceURL");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pAnnounceURL != NULL)
        {
        	AnscXmlDomNodeSetDataString
                (
                    pChildNode,
                    NULL,
                    pAnnounceURL,
                    AnscSizeOfString(pAnnounceURL)
                );
        }

        /*
         *  Create and add "TransferURL" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "TransferURL");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pTransferURL != NULL)
        {
        	AnscXmlDomNodeSetDataString
                (
                    pChildNode,
                    NULL,
                    pTransferURL,
                    AnscSizeOfString(pTransferURL)
                );
        }

        /*
         *  Create and add "IsDownload" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if ( pChildNode == NULL )
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "IsDownload");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        AnscXmlDomNodeSetDataBoolean
            (
                pChildNode,
                NULL,
                bIsDownload
            );

        /*
         *  Create and add "FileType" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if ( pChildNode == NULL )
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "FileType");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if ( pFileType != NULL )
        {
        	AnscXmlDomNodeSetDataString
                (
                    pChildNode,
                    NULL,
                    pFileType,
                    AnscSizeOfString(pFileType)
                );
        }

        /*
         *  Create and add "FileSize" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if ( pChildNode == NULL )
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "FileSize");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        AnscXmlDomNodeSetDataUlong
            (
                pChildNode,
                NULL,
                FileSize
            );

        /*
         *  Create and add "TargetFileName" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if ( pChildNode == NULL )
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "TargetFileName");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if ( pTargetFileName != NULL )
        {
        	AnscXmlDomNodeSetDataString
                (
                    pChildNode,
                    NULL,
                    pTargetFileName,
                    AnscSizeOfString(pTargetFileName)
                );
        }


        /*
         *  Create and add "Fault" node
         */
        pTempNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pTempNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pTempNode, "FaultStruct");
        AnscXmlDomNodeAddChild(pParentNode,pTempNode);

        if( pTempNode != NULL)
        {
            /*
             *  Create and add "FaultCode" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "FaultCode");
            AnscXmlDomNodeAddChild(pTempNode,pChildNode);

            if( pCwmpFault != NULL)
            {
            	AnscXmlDomNodeSetDataUlong
                    (
                        pChildNode,
                        NULL,
                        pCwmpFault->FaultCode
                    );
            }
            else
            {
            	AnscXmlDomNodeSetDataUlong
                    (
                        pChildNode,
                        NULL,
                        0
                    );
            }

            /*
             *  Create and add "FaultString" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "FaultString");
            AnscXmlDomNodeAddChild(pTempNode,pChildNode);

            if( pCwmpFault != NULL && pCwmpFault->FaultString != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pCwmpFault->FaultString,
                        AnscSizeOfString(pCwmpFault->FaultString)
                    );
            }
        }

        /*
         *  Create and add "StartTime" node
         */
        pTime = (PANSC_UNIVERSAL_TIME)hStartTime;
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "StartTime");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pTime != NULL)
        {
            _ansc_sprintf
                (
                    pNodeName,
                    "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
                    pTime->Year,
                    pTime->Month,
                    pTime->DayOfMonth,
                    pTime->Hour,
                    pTime->Minute,
                    pTime->Second
                );
        }
        else
        {
            _ansc_sprintf(pNodeName, "%s", "0001-01-01T00:00:00Z");
        }

        AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pNodeName,
                AnscSizeOfString(pNodeName)
            );

        /*
         *  Create and add "CompleteTime" node
         */
        pTime = (PANSC_UNIVERSAL_TIME)hCompleteTime;
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "CompleteTime");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pTime != NULL)
        {
            _ansc_sprintf
                (
                    pNodeName,
                    "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
                    pTime->Year,
                    pTime->Month,
                    pTime->DayOfMonth,
                    pTime->Hour,
                    pTime->Minute,
                    pTime->Second
                );
        }
        else
        {
            _ansc_sprintf(pNodeName, "%s", "0001-01-01T00:00:00Z");
        }
    
        AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pNodeName,
                AnscSizeOfString(pNodeName)
            );
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    CcspTr069PaTraceDebug(("Building AutonomousTransferComplete is done.\n"));

    return pXmlString;

}


/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapReq_DUStateChangeComplete
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hDsccReq,
                char*                       pCommandKey
            );

    description:

        This function is called to build SOAP request message of
        method "TransferComplete"

    argument:
                ANSC_HANDLE                 hThisObject
                The called object

                char*                       pRequestID
                The Reqest ID

                ANSC_HANDLE                 hDsccReq
                DSCC Request.

                char*                       pCommandKey
                The command key

    return:     the status of the operation

**********************************************************************/

#define  CcspCwmpSoappoAddXmlChildNodeString(pParent, pNode, name, value)                   \
    pNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);                      \
    if ( pNode )                                                                        \
    {                                                                                   \
        AnscXmlDomNodeSetName(pNode, name);                                             \
        AnscXmlDomNodeAddChild(pParent, pNode);                                         \
        if ( value )                                                                    \
        {                                                                               \
            AnscXmlDomNodeSetDataString                                                 \
                (                                                                       \
                    pNode,                                                              \
                    NULL,                                                               \
                    value,                                                              \
                    AnscSizeOfString(value)                                             \
                );                                                                      \
        }                                                                               \
    }

#define  CcspCwmpSoappoAddXmlChildNodeUlong(pParent, pNode, name, value)                    \
    pNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);                      \
    if ( pNode )                                                                        \
    {                                                                                   \
        AnscXmlDomNodeSetName(pNode, name);                                             \
        AnscXmlDomNodeAddChild(pParent, pNode);                                         \
        AnscXmlDomNodeSetDataUlong                                                      \
            (                                                                           \
                pNode,                                                                  \
                NULL,                                                                   \
                value                                                                   \
            );                                                                          \
    }


#define  CcspCwmpSoappoAddXmlChildNodeBool(pParent, pNode, name, value)                     \
    pNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);                      \
    if ( pNode )                                                                        \
    {                                                                                   \
        AnscXmlDomNodeSetName(pNode, name);                                             \
        AnscXmlDomNodeAddChild(pParent, pNode);                                         \
        AnscXmlDomNodeSetDataBoolean                                                    \
            (                                                                           \
                pNode,                                                                  \
                NULL,                                                                   \
                value                                                                   \
            );                                                                          \
    }


static
PANSC_XML_DOM_NODE_OBJECT
CcspCwmpSoappoBuildSoapReq_DUStateChangeComplete_Results
    (
        PCCSP_TR069_DSCC_REQ        pDsccReq
    )
{
    PANSC_XML_DOM_NODE_OBJECT       pRootNode           = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pResultNode         = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pNode               = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pFaultNode          = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    char                            buf[64];
    int                             i;

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);
    if( pRootNode == NULL)
    {
        CcspTr069PaTraceError(("Out of resource when preparing DUStateChangeComplete request!\n"));
        goto EXIT;
    }

    AnscXmlDomNodeSetName((ANSC_HANDLE)pRootNode, "Results");

    _ansc_sprintf
        (
            buf, 
            "cwmp:OpResultStruct[%u]", 
            pDsccReq->NumResults
        );

	AnscXmlDomNodeSetAttrString
		(
			pRootNode,
			"soap-enc:arrayType",
			buf,
			AnscSizeOfString(buf)
		);
    
    for ( i = 0; i < pDsccReq->NumResults; i ++ )
    {
        pResultNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if ( !pResultNode )
        {
            CcspTr069PaTraceError(("Out of resource when preparing DUStateChangeComplete request!\n"));
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pResultNode, "OpResultStruct");
        AnscXmlDomNodeAddChild(pRootNode,pResultNode);

        /* UUID */
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pNode, "UUID", pDsccReq->Results[i].Uuid);

        /* DeploymentUnitRef */
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pNode, "DeploymentUnitRef", pDsccReq->Results[i].DeploymentUnitRef);

        /* Version */
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pNode, "Version", pDsccReq->Results[i].Version);

        /* CurrentState */
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pNode, "CurrentState", pDsccReq->Results[i].CurrentState);

        /* Resolved */
        CcspCwmpSoappoAddXmlChildNodeBool(pResultNode, pNode, "Resolved", pDsccReq->Results[i].Resolved);

        /* ExecutionUnitRefList */
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pNode, "ExecutionUnitRefList", pDsccReq->Results[i].ExecutionUnitRefList);

        /* StartTime */
        CcspCwmpSoappoAddXmlChildNodeString
            (
                pResultNode, 
                pNode, 
                "StartTime", 
                (pDsccReq->Results[i].StartTime ? pDsccReq->Results[i].StartTime : "0001-01-01T00:00:00Z")
            );

        /* CompleteTime */
        CcspCwmpSoappoAddXmlChildNodeString
            (
                pResultNode, 
                pNode, 
                "CompleteTime", 
                (pDsccReq->Results[i].CompleteTime ? pDsccReq->Results[i].CompleteTime : "0001-01-01T00:00:00Z")
            );

        /* Fault */
        char * temp_null = NULL;
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pFaultNode, "Fault", temp_null);
        if ( pFaultNode )
        {
            /* FaultCode */
            CcspCwmpSoappoAddXmlChildNodeUlong(pFaultNode, pNode, "FaultCode", pDsccReq->Results[i].FaultCode);
            /* FaultString */
            CcspCwmpSoappoAddXmlChildNodeString(pFaultNode, pNode, "FaultString", pDsccReq->Results[i].FaultString);
        }
    }

    return  pRootNode;

EXIT:

    if ( pRootNode )
    {
        AnscXmlDomNodeRemove(pRootNode);
    }

    return  NULL;
}


char*
CcspCwmpSoappoBuildSoapReq_DUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hDsccReq,
        char*                       pCommandKey
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PCCSP_TR069_DSCC_REQ            pDsccReq     = (PCCSP_TR069_DSCC_REQ     )hDsccReq;
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pResultsNode = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the request node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:DUStateChangeComplete",
            CCSP_CWMP_NAMESPACE
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);


    if( TRUE )
    {
        /*
         *  Create and add "Results" node
         */
        pResultsNode = CcspCwmpSoappoBuildSoapReq_DUStateChangeComplete_Results(pDsccReq);

        if( pResultsNode == NULL)
        {
            CcspTr069PaTraceError(("DUStateChangeComplete results XML content is malformed\n"));
            goto EXIT;
        }

        AnscXmlDomNodeAddChild(pParentNode, pResultsNode);

        /*
         *  Create and add "CommandKey" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "CommandKey");
        AnscXmlDomNodeAddChild(pParentNode, pChildNode);

        if( pCommandKey != NULL)
        {
        	AnscXmlDomNodeSetDataString
                (
                    pChildNode,
                    NULL,
                    pCommandKey,
                    AnscSizeOfString(pCommandKey)
                );
        }
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    CcspTr069PaTraceDebug(("Building DUStateChangeComplete is done.\n"));

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapReq_AutonomousDUStateChangeComplete
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hAdsccReq
            );

    description:

        This function is called to build SOAP request message of
        method "TransferComplete"

    argument:
                ANSC_HANDLE                 hThisObject
                The called object

                char*                       pRequestID
                The Reqest ID

                ANSC_HANDLE                 hAdsccReq
                ADSCC Request.

    return:     the status of the operation

**********************************************************************/


PANSC_XML_DOM_NODE_OBJECT
CcspCwmpSoappoBuildSoapReq_AutonomousDUStateChangeComplete_Results
    (
        PCCSP_TR069_ADSCC_REQ       pAdsccReq
    )
{
    PANSC_XML_DOM_NODE_OBJECT       pRootNode           = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pResultNode         = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pNode               = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pFaultNode          = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    char                            buf[64];
    int                             i;
    ULONG                           NumUnFilteredRes    = 0;

    if ( !pAdsccReq || pAdsccReq->NumResults == 0 )
    {
        goto EXIT;
    }

    for ( i = 0; i < pAdsccReq->NumResults; i ++ )
    {
        if ( !pAdsccReq->Results[i].bFilteredOut )
        {
            NumUnFilteredRes ++;
        }
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);
    if( pRootNode == NULL)
    {
        CcspTr069PaTraceError(("Out of resource when preparing AutonomousDUStateChangeComplete request!\n"));
        goto EXIT;
    }

    AnscXmlDomNodeSetName((ANSC_HANDLE)pRootNode, "Results");

    _ansc_sprintf
        (
            buf, 
            "cwmp:AutonOpResultStruct[%lu]", 
            NumUnFilteredRes
        );

	AnscXmlDomNodeSetAttrString
		(
			pRootNode,
			"soap-enc:arrayType",
			buf,
			AnscSizeOfString(buf)
		);
    
    for ( i = 0; i < pAdsccReq->NumResults; i ++ )
    {
        if ( pAdsccReq->Results[i].bFilteredOut )
        {
            continue;
        }

        pResultNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if ( !pResultNode )
        {
            CcspTr069PaTraceError(("Out of resource when preparing AutonomousDUStateChangeComplete request!\n"));
            goto EXIT1;
        }

        AnscXmlDomNodeSetName(pResultNode, "AutonOpResultStruct");
        AnscXmlDomNodeAddChild(pRootNode,pResultNode);

        /* UUID */
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pNode, "UUID", pAdsccReq->Results[i].Uuid);

        /* DeploymentUnitRef */
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pNode, "DeploymentUnitRef", pAdsccReq->Results[i].DeploymentUnitRef);

        /* Version */
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pNode, "Version", pAdsccReq->Results[i].Version);

        /* OperationPerformed */
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pNode, "OperationPerformed", pAdsccReq->Results[i].OperationPerformed);

        /* CurrentState */
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pNode, "CurrentState", pAdsccReq->Results[i].CurrentState);

        /* Resolved */
        CcspCwmpSoappoAddXmlChildNodeBool(pResultNode, pNode, "Resolved", pAdsccReq->Results[i].Resolved);

        /* ExecutionUnitRefList */
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pNode, "ExecutionUnitRefList", pAdsccReq->Results[i].ExecutionUnitRefList);

        /* StartTime */
        CcspCwmpSoappoAddXmlChildNodeString
            (
                pResultNode, 
                pNode, 
                "StartTime", 
                (pAdsccReq->Results[i].StartTime ? pAdsccReq->Results[i].StartTime : "0001-01-01T00:00:00Z")
            );

        /* CompleteTime */
        CcspCwmpSoappoAddXmlChildNodeString
            (
                pResultNode, 
                pNode, 
                "CompleteTime", 
                (pAdsccReq->Results[i].CompleteTime ? pAdsccReq->Results[i].CompleteTime : "0001-01-01T00:00:00Z")
            );

        /* Fault */
        char * temp_null = NULL;
        CcspCwmpSoappoAddXmlChildNodeString(pResultNode, pFaultNode, "Fault", temp_null);
        if ( pFaultNode )
        {
            /* FaultCode */
            CcspCwmpSoappoAddXmlChildNodeUlong(pFaultNode, pNode, "FaultCode", pAdsccReq->Results[i].FaultCode);
            /* FaultString */
            CcspCwmpSoappoAddXmlChildNodeString(pFaultNode, pNode, "FaultString", pAdsccReq->Results[i].FaultString);
        }
    }

    goto EXIT;

EXIT1:

    if ( pRootNode )
    {
    	AnscXmlDomNodeRemove(pRootNode);
        pRootNode = NULL;
    }

EXIT:

    return pRootNode;
}


char*
CcspCwmpSoappoBuildSoapReq_AutonomousDUStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hAdsccReq
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PCCSP_TR069_ADSCC_REQ           pAdsccReq    = (PCCSP_TR069_ADSCC_REQ    )hAdsccReq;
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the request node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:AutonomousDUStateChangeComplete",
            CCSP_CWMP_NAMESPACE
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);


    if( TRUE )
    {
        /*
         *  Create and add "Results" node
         */
        PANSC_XML_DOM_NODE_OBJECT   pResultsNode = NULL;

        pResultsNode = CcspCwmpSoappoBuildSoapReq_AutonomousDUStateChangeComplete_Results(pAdsccReq);

        if( pResultsNode == NULL)
        {
            CcspTr069PaTraceError(("DUStateChangeComplete results XML content is malformed\n"));
            goto EXIT;
        }

        AnscXmlDomNodeAddChild(pParentNode, pResultsNode);
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    CcspTr069PaTraceDebug(("Building AutonomousDUStateChangeComplete is done.\n"));

    return pXmlString;
}


/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_UnknownMethod
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                char*                       pMethodName,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP fault response message.

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                char*                       pMethodName,
                The method name;

                ANSC_HANDLE                 hSoapFault
                The fault handle;

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_Error
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        char*                       pMethodName,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(pMethodName);
    /*
     * Actually, the Soap Fault message doesn't have the MethodName information.
     */

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    return NULL;
}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_GetRPCMethods
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                SLAP_STRING_ARRAY*          pMethodList
            );

    description:

        This function is called to build SOAP response message of
        method "GetRPCMethods".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                SLAP_STRING_ARRAY*          pMethodList
                The Method list (Array)

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_GetRPCMethods
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        SLAP_STRING_ARRAY*          pMethodList
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
	CHAR							pAttrName[64]= { 0 };
	CHAR							pAttrVal[64] = { 0 };
    ULONG                           length       = 0;
    ULONG                           i            = 0;

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:GetRPCMethods%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);

    /*
     *  Create and add the MethodList node
     */
    pListNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pListNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pListNode, "MethodList");
    AnscXmlDomNodeAddChild(pParentNode,pListNode);

    if( pMethodList != NULL)
    {
		if( bAddArrayTypeAttr)
		{
			/*
			 *	According to WT151, the arrayType is supposed to be specified when
			 *  there's an array in the SOAP message
			 *
			 *  This attribute should be added here
			 *
			 *	soap-enc:arrayType="xsd:string[nCount]"
			 */
			_ansc_sprintf
				(
					pAttrName,
					"%s:arrayType",
					NODE_SOAP_ENCODING
				);

			_ansc_sprintf
				(
					pAttrVal,
					"%s:string[%lu]",
					SOAP_XSD_NAMESPACE_NAME,
					pMethodList->VarCount
				);

			AnscXmlDomNodeSetAttrString
				(
					pListNode,
					pAttrName,
					pAttrVal,
					AnscSizeOfString(pAttrVal)
				);
		}

        for( i = 0; i < pMethodList->VarCount; i ++)
        {
            /*
             *  Create and add the string node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "string");
            AnscXmlDomNodeAddChild(pListNode,pChildNode);

            if( pMethodList->Array.arrayString[i] != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pMethodList->Array.arrayString[i],
                        AnscSizeOfString(pMethodList->Array.arrayString[i])
                    );
            }
        }
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_SetParameterValues
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                int                         iStatus,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "SetParameterValues".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                int                         iStatus,
                The request status;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_SetParameterValues
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:SetParameterValues%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);


    if( TRUE )
    {
        /*
         *  Create and add "Status" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "Status");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        AnscXmlDomNodeSetDataUlong
            (
                pChildNode,
                NULL,
                iStatus
            );

    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_GetParameterValues
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hParamValueArray,
                ULONG                       ulArraySize,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "GetParameterValues".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hParamValueArray,
                The Param Value Array;

                ULONG                       ulArraySize,
                The count of the items in the array;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_GetParameterValues
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hParamValueArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCCSP_CWMP_PARAM_VALUE          pValueArray  = (PCCSP_CWMP_PARAM_VALUE)hParamValueArray;
    PCCSP_CWMP_PARAM_VALUE          pCwmpValue   = (PCCSP_CWMP_PARAM_VALUE)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[256]={ 0 };
	CHAR							pAttrName[64] ={ 0 };
	CHAR							pAttrVal[64]  ={ 0 };
    ULONG                           length       = 0;
    ULONG                           i            = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:GetParameterValues%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);

    /*
     *  Create and add the ParameterList node
     */
    pListNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pListNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pListNode, "ParameterList");
    AnscXmlDomNodeAddChild(pParentNode,pListNode);

	if(bAddArrayTypeAttr)
	{
		/*
		 *	According to WT151, the arrayType is supposed to be specified when
		 *  there's an array in the SOAP message
		 *
		 *  This attribute should be added here
		 *
		 *	soap-enc:arrayType="cwmp:ParameterValueStruct[nCount]"
		 */
		_ansc_sprintf
			(
				pAttrName,
				"%s:arrayType",
				NODE_SOAP_ENCODING
			);

		_ansc_sprintf
			(
				pAttrVal,
				"%s:ParameterValueStruct[%lu]",
				CCSP_CWMP_NAMESPACE,
				ulArraySize
			);

		AnscXmlDomNodeSetAttrString
			(
				pListNode,
				pAttrName,
				pAttrVal,
				AnscSizeOfString(pAttrVal)
			);

	}

    for( i = 0; i < ulArraySize; i ++)
    {
        pCwmpValue  = &pValueArray[i];

        /*
         *  Create and add the ParameterValueStruct node
         */
        pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pParentNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pParentNode, "ParameterValueStruct");
        AnscXmlDomNodeAddChild(pListNode,pParentNode);

        if( TRUE )
        {
            /*
             *  Create and add "Name" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "Name");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pCwmpValue->Name != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pCwmpValue->Name,
                        AnscSizeOfString(pCwmpValue->Name)
                    );
            }

            /*
             *  Create and add "Value" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "Value");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pCwmpValue->Value != NULL && pCwmpValue->Value->Syntax == SLAP_VAR_SYNTAX_string )
            {
                CcspCwmpSoappoBuildSoap_ParamDataTypeAttr
                    (
                        pChildNode,
                        pCwmpValue->Tr069DataType
                    );

                AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pCwmpValue->Value->Variant.varString,
                        AnscSizeOfString(pCwmpValue->Value->Variant.varString)
                    );
            }
            else
            {
                AnscXmlDomNodeSetAttrString
                    (
                        pChildNode,
                        "xsi:type",
                        "xsd:string",
                        AnscSizeOfString("xsd:string")
                    );
            }
        }
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_GetParameterNames
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hParamInfoArray,
                ULONG                       ulArraySize,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "GetParameterNames".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hParamInfoArray,
                The ParamInfo Array;

                ULONG                       ulArraySize,
                The count of the items in the array;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_GetParameterNames
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hParamInfoArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode     = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode     = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pListNode     = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCCSP_CWMP_PARAM_INFO           pInfoArray    = (PCCSP_CWMP_PARAM_INFO)hParamInfoArray;
    PCCSP_CWMP_PARAM_INFO           pCwmpInfo     = (PCCSP_CWMP_PARAM_INFO)NULL;
    PCHAR                           pXmlString    = (PCHAR)NULL;
    CHAR                            pNodeName[256]= { 0 };
    CHAR                            pAttrName[64] = { 0 };
    CHAR                            pAttrVal[64]  = { 0 };
    ULONG                           length        = 0;
    ULONG                           i             = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:GetParameterNames%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);

    /*
     *  Create and add the ParameterList node
     */
    pListNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pListNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pListNode, "ParameterList");
    AnscXmlDomNodeAddChild(pParentNode,pListNode);

	if(bAddArrayTypeAttr)
	{
		/*
		 *	According to WT151, the arrayType is supposed to be specified when
		 *  there's an array in the SOAP message
		 *
		 *  This attribute should be added here
		 *
		 *	soap-enc:arrayType="cwmp:ParameterInfoStruct[nCount]"
		 */
		_ansc_sprintf
			(
				pAttrName,
				"%s:arrayType",
				NODE_SOAP_ENCODING
			);

		_ansc_sprintf
			(
				pAttrVal,
				"%s:ParameterInfoStruct[%lu]",
				CCSP_CWMP_NAMESPACE,
				ulArraySize
			);

		AnscXmlDomNodeSetAttrString
			(
				pListNode,
				pAttrName,
				pAttrVal,
				AnscSizeOfString(pAttrVal)
			);
	}

    for( i = 0; i < ulArraySize; i ++)
    {
        pCwmpInfo  = &pInfoArray[i];

        /*
         *  Create and add the ParameterInfoStruct node
         */
        pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pParentNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pParentNode, "ParameterInfoStruct");
        AnscXmlDomNodeAddChild(pListNode,pParentNode);

        if( TRUE )
        {
            /*
             *  Create and add "Name" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "Name");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pCwmpInfo->Name != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pCwmpInfo->Name,
                        AnscSizeOfString(pCwmpInfo->Name)
                    );
            }

            /*
             *  Create and add "Writable" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "Writable");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pCwmpInfo->bWritable)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        "1",
                        AnscSizeOfString("1")
                    );
            }
            else
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        "0",
                        AnscSizeOfString("0")
                    );
            }

        }
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_SetParameterAttributes
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "SetParameterAttributes".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_SetParameterAttributes
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:SetParameterAttributes%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pChildNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pChildNode);


    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_GetParameterAttributes
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hParamAttrArray,
                ULONG                       ulArraySize,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "GetParameterAttributes".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hParamAttrArray,
                The ParamAttr Array;

                ULONG                       ulArraySize,
                The count of the items in the array;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_GetParameterAttributes
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hParamAttribArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCCSP_CWMP_PARAM_ATTRIB         pAttrArray   = (PCCSP_CWMP_PARAM_ATTRIB)hParamAttribArray;
    PCCSP_CWMP_PARAM_ATTRIB         pCwmpAttr    = (PCCSP_CWMP_PARAM_ATTRIB)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[256]= { 0 };
	CHAR							pAttrName[64] = { 0 };
	CHAR							pAttrVal[64]  = { 0 };
    PCHAR                           pAccess      = NULL;
    PCHAR                           pNext        = NULL;
    ULONG                           length       = 0;
    ULONG                           nAccess      = 0;
    ULONG                           i            = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:GetParameterAttributes%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);

    /*
     *  Create and add the ParameterList node
     */
    pListNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pListNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pListNode, "ParameterList");
    AnscXmlDomNodeAddChild(pParentNode,pListNode);

	if(bAddArrayTypeAttr)
	{
		/*
		 *	According to WT151, the arrayType is supposed to be specified when
		 *  there's an array in the SOAP message
		 *
		 *  This attribute should be added here
		 *
		 *	soap-enc:arrayType="cwmp:ParameterAttributeStruct[nCount]"
		 */
		_ansc_sprintf
			(
				pAttrName,
				"%s:arrayType",
				NODE_SOAP_ENCODING
			);

		_ansc_sprintf
			(
				pAttrVal,
				"%s:ParameterAttributeStruct[%lu]",
				CCSP_CWMP_NAMESPACE,
				ulArraySize
			);

		AnscXmlDomNodeSetAttrString
			(
				pListNode,
				pAttrName,
				pAttrVal,
				AnscSizeOfString(pAttrVal)
			);
	}

    for( i = 0; i < ulArraySize; i ++)
    {
        pCwmpAttr  = &pAttrArray[i];

        /*
         *  Create and add the ParameterAttributeStruct node
         */
        pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pParentNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pParentNode, "ParameterAttributeStruct");
        AnscXmlDomNodeAddChild(pListNode,pParentNode);

        if( TRUE )
        {
            /*
             *  Create and add "Name" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "Name");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pCwmpAttr->Name != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pCwmpAttr->Name,
                        AnscSizeOfString(pCwmpAttr->Name)
                    );
            }

            /*
             *  Create and add "Notification" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "Notification");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            AnscXmlDomNodeSetDataUlong
                (
                    pChildNode,
                    NULL,
                    pCwmpAttr->Notification
                );

            /*
             *  Create and add "AccessList" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "AccessList");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            pParentNode = pChildNode;
            nAccess     = 0;

            if( pCwmpAttr->AccessList != NULL && AnscSizeOfString(pCwmpAttr->AccessList) > 0)
            {
                /*
                 *  Parse and add the access node
                 */
                pAccess = pCwmpAttr->AccessList;
                pNext   = _ansc_strstr(pAccess, CCSP_CWMP_COMMA);

                while( pNext != NULL)
                {
                    pNext[0] = 0;

                    /*
                     *  Create and add "string" node
                     */
                    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

                    if( pChildNode == NULL)
                    {
                        goto EXIT;
                    }

                    AnscXmlDomNodeSetName(pChildNode, "string");
                    AnscXmlDomNodeAddChild(pParentNode,pChildNode);
                    nAccess ++;

                    AnscXmlDomNodeSetDataString
                        (
                            pChildNode,
                            NULL,
                            pAccess,
                            AnscSizeOfString(pAccess)
                        );

                    pAccess = pNext ++;

                    if( pAccess[0] == 0)
                    {
                        break;
                    }

                    pNext   = _ansc_strstr(pAccess, CCSP_CWMP_COMMA);
                }

                if( pAccess != NULL && AnscSizeOfString(pAccess) > 0)
                {
                    /*
                     *  Create and add "string" node
                     */
                    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

                    if( pChildNode == NULL)
                    {
                        goto EXIT;
                    }

                    AnscXmlDomNodeSetName(pChildNode, "string");
                    AnscXmlDomNodeAddChild(pParentNode,pChildNode);
                    nAccess ++;

                    AnscXmlDomNodeSetDataString
                        (
                            pChildNode,
                            NULL,
                            pAccess,
                            AnscSizeOfString(pAccess)
                        );
                }
            }

			/*
			 *	According to WT151, the arrayType is supposed to be specified when
			 *  there's an array in the SOAP message
			 *
			 *  This attribute should be added here
			 *
			 *	soap-enc:arrayType="xsd:string[nCount]"
			 */
			_ansc_sprintf
				(
					pAttrName,
					"%s:arrayType",
					NODE_SOAP_ENCODING
				);

			_ansc_sprintf
				(
					pAttrVal,
					"%s:string[%lu]",
					SOAP_XSD_NAMESPACE_NAME,
					nAccess
				);

			AnscXmlDomNodeSetAttrString
				(
					pParentNode,
					pAttrName,
					pAttrVal,
					AnscSizeOfString(pAttrVal)
				);
        }
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_AddObject
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ULONG                       ulInstanceNumber,
                int                         iStatus,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "AddObject".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ULONG                       ulInstanceNumber,
                The instance number;

                int                         iStatus,
                The request status;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_AddObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ULONG                       ulInstanceNumber,
        int                         iStatus,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:AddObject%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);


    if( TRUE )
    {
        /*
         *  Create and add "InstanceNumber" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "InstanceNumber");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        AnscXmlDomNodeSetDataUlong
            (
                pChildNode,
                NULL,
                ulInstanceNumber
            );

        /*
         *  Create and add "Status" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "Status");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        AnscXmlDomNodeSetDataUlong
            (
                pChildNode,
                NULL,
                iStatus
            );

    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_DeleteObject
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                int                         iStatus,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "DeleteObject".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                int                         iStatus,
                The request status;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_DeleteObject
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:DeleteObject%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);


    if( TRUE )
    {
        /*
         *  Create and add "Status" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "Status");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        AnscXmlDomNodeSetDataUlong
            (
                pChildNode,
                NULL,
                iStatus
            );

    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_Download
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                int                         iStatus,
                ANSC_HANDLE                 hStartTime,
                ANSC_HANDLE                 hCompleteTime,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "Download".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                int                         iStatus,
                The request status;

                ANSC_HANDLE                 hStartTime,
                The start downloading time;

                ANSC_HANDLE                 hCompleteTime,
                The complete downloading time;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_Download
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;
    PANSC_UNIVERSAL_TIME            pTime        = NULL;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:Download%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);


    if( TRUE )
    {
        /*
         *  Create and add "Status" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "Status");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        AnscXmlDomNodeSetDataUlong
            (
                pChildNode,
                NULL,
                iStatus
            );

        /*
         *  Create and add "StartTime" node
         */
        pTime = (PANSC_UNIVERSAL_TIME)hStartTime;
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "StartTime");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pTime != NULL)
        {
            _ansc_sprintf
                (
                    pNodeName,
                    "%.4d-%.2d-%.2dT%.2d:%.2d:%.2dZ",
                    pTime->Year,
                    pTime->Month,
                    pTime->DayOfMonth,
                    pTime->Hour,
                    pTime->Minute,
                    pTime->Second
                );
        }
        else
        {
            _ansc_sprintf(pNodeName, "%s", "0001-01-01T00:00:00Z");
        }
    
        AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pNodeName,
                AnscSizeOfString(pNodeName)
            );

        /*
         *  Create and add "CompleteTime" node
         */
        pTime = (PANSC_UNIVERSAL_TIME)hCompleteTime;
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "CompleteTime");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pTime != NULL)
        {
            _ansc_sprintf
                (
                    pNodeName,
                    "%.4d-%.2d-%.2dT%.2d:%.2d:%.2dZ",
                    pTime->Year,
                    pTime->Month,
                    pTime->DayOfMonth,
                    pTime->Hour,
                    pTime->Minute,
                    pTime->Second
                );
        }
        else
        {
            _ansc_sprintf(pNodeName, "%s", "0001-01-01T00:00:00Z");
        }

        AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pNodeName,
                AnscSizeOfString(pNodeName)
            );
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    CcspTr069PaTraceDebug(("Building DownloadResponse is done.\n"));

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_Reboot
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "Reboot".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_Reboot
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:Reboot%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pChildNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pChildNode);


    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_ChangeDUState
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "ChangeDUState".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/

char*
CcspCwmpSoappoBuildSoapRep_ChangeDUState
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:ChangeDUState%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pChildNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pChildNode);


    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_GetQueuedTransfers
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hQueuedTransferArray,
                ULONG                       ulArraySize,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "GetQueuedTransfers".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hQueuedTransferArray,
                The QueuedTransfer Array;

                ULONG                       ulArraySize,
                The count of the items in the array;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_GetQueuedTransfers
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hQueuedTransferArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCCSP_CWMP_QUEUED_TRANSFER      pTransArray  = (PCCSP_CWMP_QUEUED_TRANSFER)hQueuedTransferArray;
    PCCSP_CWMP_QUEUED_TRANSFER      pCwmpTrans   = (PCCSP_CWMP_QUEUED_TRANSFER)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;
    ULONG                           i            = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:GetQueuedTransfers%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);

    /*
     *  Create and add the TransferList node
     */
    pListNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pListNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pListNode, "TransferList");
    AnscXmlDomNodeAddChild(pParentNode,pListNode);

    for( i = 0; i < ulArraySize; i ++)
    {
        pCwmpTrans  = &pTransArray[i];

        /*
         *  Create and add the "QueuedTransferStruct" node
         */
        pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pParentNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pParentNode, "QueuedTransferStruct");
        AnscXmlDomNodeAddChild(pListNode,pParentNode);

        if( TRUE )
        {
            /*
             *  Create and add "CommandKey" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "CommandKey");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pCwmpTrans->CommandKey != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pCwmpTrans->CommandKey,
                        AnscSizeOfString(pCwmpTrans->CommandKey)
                    );
            }

            /*
             *  Create and add "State" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "State");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            AnscXmlDomNodeSetDataUlong
                (
                    pChildNode,
                    NULL,
                    pCwmpTrans->State
                );

        }
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_ScheduleInform
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "ScheduleInform".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_ScheduleInform
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:ScheduleInform%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pChildNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pChildNode);


    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_SetVouchers
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "SetVouchers".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_SetVouchers
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:SetVouchers%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pChildNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pChildNode);


    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_GetOptions
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hOptionArray,
                ULONG                       ulArraySize,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "GetOptions".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hOptionArray,
                The Option Array;

                ULONG                       ulArraySize,
                The count of the items in the array;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_GetOptions
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hOptionArray,
        ULONG                       ulArraySize,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCCSP_CWMP_OPTION               pOptionArray = (PCCSP_CWMP_OPTION)hOptionArray;
    PCCSP_CWMP_OPTION               pCwmpOption  = (PCCSP_CWMP_OPTION)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    PANSC_UNIVERSAL_TIME            pTime        = NULL;
    ULONG                           length       = 0;
    ULONG                           i            = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:GetOptions%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);

    /*
     *  Create and add the OptionList node
     */
    pListNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pListNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pListNode, "OptionList");
    AnscXmlDomNodeAddChild(pParentNode,pListNode);

    for( i = 0; i < ulArraySize; i ++)
    {
        pCwmpOption  = &pOptionArray[i];

        /*
         *  Create and add the OptionStruct node
         */
        pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pParentNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pParentNode, "OptionStruct");
        AnscXmlDomNodeAddChild(pListNode,pParentNode);

        if( TRUE )
        {
            /*
             *  Create and add "OptionName" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "OptionName");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pCwmpOption->OptionName != NULL)
            {
            	AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pCwmpOption->OptionName,
                        AnscSizeOfString(pCwmpOption->OptionName)
                    );
            }

            /*
             *  Create and add "VoucherSN" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "VoucherSN");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            AnscXmlDomNodeSetDataUlong
                (
                    pChildNode,
                    NULL,
                    pCwmpOption->VoucherSN
                );

            /*
             *  Create and add "State" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "State");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            AnscXmlDomNodeSetDataUlong
                (
                    pChildNode,
                    NULL,
                    pCwmpOption->State
                );

            /*
             *  Create and add "Mode" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "Mode");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            AnscXmlDomNodeSetDataUlong
                (
                    pChildNode,
                    NULL,
                    pCwmpOption->Mode
                );

            /*
             *  Create and add "StartDate" node
             */
            pTime = (PANSC_UNIVERSAL_TIME)&pCwmpOption->StartDate;
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "StartDate");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pTime != NULL)
            {
                _ansc_sprintf
                    (
                        pNodeName,
                        "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
                        pTime->Year,
                        pTime->Month,
                        pTime->DayOfMonth,
                        pTime->Hour,
                        pTime->Minute,
                        pTime->Second
                    );

                AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pNodeName,
                        AnscSizeOfString(pNodeName)
                    );
            }

            /*
             *  Create and add "ExpirationDate" node
             */
            pTime = (PANSC_UNIVERSAL_TIME)&pCwmpOption->ExpirationDate;
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "ExpirationDate");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pTime != NULL)
            {
                _ansc_sprintf
                    (
                        pNodeName,
                        "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
                        pTime->Year,
                        pTime->Month,
                        pTime->DayOfMonth,
                        pTime->Hour,
                        pTime->Minute,
                        pTime->Second
                    );

                AnscXmlDomNodeSetDataString
                    (
                        pChildNode,
                        NULL,
                        pNodeName,
                        AnscSizeOfString(pNodeName)
                    );
            }

            /*
             *  Create and add "IsTransferable" node
             */
            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pChildNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pChildNode, "IsTransferable");
            AnscXmlDomNodeAddChild(pParentNode,pChildNode);

            if( pCwmpOption->IsTransferable)
            {
            	AnscXmlDomNodeSetDataUlong
                    (
                        pChildNode,
                        NULL,
                        1
                    );
            }
            else
            {
            	AnscXmlDomNodeSetDataUlong
                    (
                        pChildNode,
                        NULL,
                        0
                    );
            }
        }
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_Upload
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                int                         iStatus,
                ANSC_HANDLE                 hStartTime,
                ANSC_HANDLE                 hCompleteTime,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "Upload".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                int                         iStatus,
                The request status;

                ANSC_HANDLE                 hStartTime,
                The start uploading time;

                ANSC_HANDLE                 hCompleteTime,
                The end uploading time;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_Upload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        int                         iStatus,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode  = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;
    PANSC_UNIVERSAL_TIME            pTime        = NULL;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pParentNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pParentNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:Upload%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pParentNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pParentNode);


    if( TRUE )
    {
        /*
         *  Create and add "Status" node
         */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "Status");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        AnscXmlDomNodeSetDataUlong
            (
                pChildNode,
                NULL,
                iStatus
            );

        /*
         *  Create and add "StartTime" node
         */
        pTime = (PANSC_UNIVERSAL_TIME)hStartTime;
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "StartTime");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pTime != NULL)
        {
            _ansc_sprintf
                (
                    pNodeName,
                    "%.4d-%.2d-%.2dT%.2d:%.2d:%.2dZ",
                    pTime->Year,
                    pTime->Month,
                    pTime->DayOfMonth,
                    pTime->Hour,
                    pTime->Minute,
                    pTime->Second
                );
        }
        else
        {
            _ansc_sprintf(pNodeName, "%s", "0001-01-01T00:00:00Z");
        }

        AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pNodeName,
                AnscSizeOfString(pNodeName)
            );

        /*
         *  Create and add "CompleteTime" node
         */
        pTime = (PANSC_UNIVERSAL_TIME)hCompleteTime;
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

        if( pChildNode == NULL)
        {
            goto EXIT;
        }

        AnscXmlDomNodeSetName(pChildNode, "CompleteTime");
        AnscXmlDomNodeAddChild(pParentNode,pChildNode);

        if( pTime != NULL)
        {
            _ansc_sprintf
                (
                    pNodeName,
                    "%.4d-%.2d-%.2dT%.2d:%.2d:%.2dZ",
                    pTime->Year,
                    pTime->Month,
                    pTime->DayOfMonth,
                    pTime->Hour,
                    pTime->Minute,
                    pTime->Second
                );
        }
        else
        {
            _ansc_sprintf(pNodeName, "%s", "0001-01-01T00:00:00Z");
        }

        AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pNodeName,
                AnscSizeOfString(pNodeName)
            );
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRep_FactoryReset
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        method "FactoryReset".

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRep_FactoryReset
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pXmlString   = (PCHAR)NULL;
    CHAR                            pNodeName[64]= { 0 };
    ULONG                           length       = 0;

    if( hSoapFault != NULL)
    {
        return
            CcspCwmpSoappoBuildSoapRepFault
                (
                    hThisObject,
                    pRequestID,
                    hSoapFault
                );
    }

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the response node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:FactoryReset%s",
            CCSP_CWMP_NAMESPACE,
            PART_RESPONSE_NAME
        );

    AnscXmlDomNodeSetName(pChildNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pChildNode);


    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;

}

/**********************************************************************

    prototype:

        char*
        CcspCwmpSoappoBuildSoapRepFault
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pRequestID,
                ANSC_HANDLE                 hSoapFault
            );

    description:

        This function is called to build SOAP response message of
        Fault string only.

    argument:
                ANSC_HANDLE                 hThisObject,
                The called object;

                char*                       pRequestID
                The Reqest ID;

                ANSC_HANDLE                 hSoapFault,
                The Fault handle

    return:     the status of the operation;

**********************************************************************/
char*
CcspCwmpSoappoBuildSoapRepFault
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pRequestID,
        ANSC_HANDLE                 hSoapFault
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PANSC_XML_DOM_NODE_OBJECT       pRootNode     = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode     = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParentNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pParamNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCCSP_CWMP_SOAP_FAULT           pSoapFault    = (PCCSP_CWMP_SOAP_FAULT)hSoapFault;
    PCCSP_CWMP_SET_PARAM_FAULT      pSetParamFault= (PCCSP_CWMP_SET_PARAM_FAULT)NULL;
    CHAR                            pNodeName[128]= { 0 };
    PCHAR                           pXmlString    = (PCHAR)NULL;
    ULONG                           length        = 0;
    ULONG                           i             = 0;

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        CcspCwmpSoappoUtilCreateSoapEnvelopeHandle( pRequestID);

    if( pRootNode == NULL)
    {
        return NULL;
    }

    /*
     *  Create and add the body node
     */
    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pBodyNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_BODY_NODE
        );

    AnscXmlDomNodeSetName(pBodyNode, pNodeName);
    AnscXmlDomNodeAddChild(pRootNode,pBodyNode);

    /*
     *  Create and add the fault content
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            SOAP_NAMESPACE_VALUE,
            SOAP_FAULT
        );

    AnscXmlDomNodeSetName(pChildNode, pNodeName);
    AnscXmlDomNodeAddChild(pBodyNode,pChildNode);

    /* start from the Fault node */
    pParentNode = pChildNode;

    /*
     * Create and add <faultcode> node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pChildNode, SOAP_FAULTCODE);
    AnscXmlDomNodeAddChild(pParentNode,pChildNode);

    if( pSoapFault->soap_faultcode != NULL)
    {
    	AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pSoapFault->soap_faultcode,
                AnscSizeOfString(pSoapFault->soap_faultcode)
            );
    }

    /*
     * Create and add <faultstring> node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pChildNode, SOAP_FAULTSTRING);
    AnscXmlDomNodeAddChild(pParentNode,pChildNode);

    if( pSoapFault->soap_faultstring != NULL)
    {
    	AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pSoapFault->soap_faultstring,
                AnscSizeOfString(pSoapFault->soap_faultstring)
            );
    }

    /*
     * Create and add <detail> node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pChildNode, SOAP_DETAIL);
    AnscXmlDomNodeAddChild(pParentNode,pChildNode);

    /* start from the detail node */
    pParentNode = pChildNode;

    /*
     * Create and add <cwmp:Fault> node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    _ansc_sprintf
        (
            pNodeName,
            "%s:%s",
            CCSP_CWMP_NAMESPACE,
            CCSP_CWMP_FAULT_NODE
        );

    AnscXmlDomNodeSetName(pChildNode, pNodeName);
    AnscXmlDomNodeAddChild(pParentNode,pChildNode);

    /* start from the <cwmp:Fault> node */
    pParentNode = pChildNode;

    /*
     * Create and add <FaultCode> node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pChildNode, CCSP_CWMP_FAULTCODE);
    AnscXmlDomNodeAddChild(pParentNode,pChildNode);

    AnscXmlDomNodeSetDataUlong
        (
            pChildNode,
            NULL,
            pSoapFault->Fault.FaultCode
        );

    /*
     * Create and add <FaultString> node
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

    if( pChildNode == NULL)
    {
        goto EXIT;
    }

    AnscXmlDomNodeSetName(pChildNode, CCSP_CWMP_FAULTSTRING);
    AnscXmlDomNodeAddChild(pParentNode,pChildNode);

    if( pSoapFault->Fault.FaultString != NULL)
    {
    	AnscXmlDomNodeSetDataString
            (
                pChildNode,
                NULL,
                pSoapFault->Fault.FaultString,
                AnscSizeOfString(pSoapFault->Fault.FaultString)
            );
    }

    /*
     * Create and add <SetParameterValueFault> node if required.
     */
    if( pSoapFault->SetParamValuesFaultCount > 0)
    {
        for( i = 0; i < pSoapFault->SetParamValuesFaultCount; i ++)
        {
            pSetParamFault = &pSoapFault->SetParamValuesFaultArray[i];

            if( pSetParamFault == NULL)
            {
                continue;
            }

            /*
             * Create and add <SetParameterValuesFault> node
             */
            pParamNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

            if( pParamNode == NULL)
            {
                goto EXIT;
            }

            AnscXmlDomNodeSetName(pParamNode, CCSP_CWMP_SETFAULT);
            AnscXmlDomNodeAddChild(pParentNode,pParamNode);

            if( TRUE)
            {
                /*
                 * Create and add <ParameterName> node
                 */
                pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

                if( pChildNode == NULL)
                {
                    goto EXIT;
                }

                AnscXmlDomNodeSetName(pChildNode, CCSP_CWMP_PARAMETERNAME);
                AnscXmlDomNodeAddChild(pParamNode,pChildNode);

                if( pSetParamFault->ParameterName != NULL)
                {
                	AnscXmlDomNodeSetDataString
                        (
                            pChildNode,
                            NULL,
                            pSetParamFault->ParameterName,
                            AnscSizeOfString(pSetParamFault->ParameterName)
                        );
                }

                /*
                 * Create and add <FaultCode> node
                 */
                pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

                if( pChildNode == NULL)
                {
                    goto EXIT;
                }

                AnscXmlDomNodeSetName(pChildNode, CCSP_CWMP_FAULTCODE);
                AnscXmlDomNodeAddChild(pParamNode,pChildNode);

                AnscXmlDomNodeSetDataUlong
                    (
                        pChildNode,
                        NULL,
                        pSetParamFault->FaultCode
                    );

                /*
                 * Create and add <FaultString> node
                 */
                pChildNode = (PANSC_XML_DOM_NODE_OBJECT)AnscCreateXmlDomNode(NULL);

                if( pChildNode == NULL)
                {
                    goto EXIT;
                }

                AnscXmlDomNodeSetName(pChildNode, CCSP_CWMP_FAULTSTRING);
                AnscXmlDomNodeAddChild(pParamNode,pChildNode);

                if( pSetParamFault->FaultString != NULL)
                {
                	AnscXmlDomNodeSetDataString
                        (
                            pChildNode,
                            NULL,
                            pSetParamFault->FaultString,
                            AnscSizeOfString(pSetParamFault->FaultString)
                        );
                }

            }
        }
    }

    /* encode the xml handle */
    length = AnscXmlDomNodeGetEncodedSize(pRootNode);

    if( length == 0)
    {
        goto EXIT;
    }

    pXmlString = (PCHAR)AnscAllocateMemory(length + 16);

    if( pXmlString == NULL)
    {
        goto EXIT;
    }

    if( AnscXmlDomNodeEncode(pRootNode, (PVOID)pXmlString, &length) != ANSC_STATUS_SUCCESS)
    {
        AnscFreeMemory(pXmlString);
        pXmlString = NULL;
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return pXmlString;
}

