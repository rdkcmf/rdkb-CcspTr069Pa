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

    module: ccsp_cwmp_soappo_process.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced operation functions
        of the CCSP CWMP SoapParser Object.

        *   CcspCwmpSoappoProcessSoapHeader
        *   CcspCwmpSoappoProcessRequest_SetParameterValues
        *   CcspCwmpSoappoProcessRequest_GetParameterValues
        *   CcspCwmpSoappoProcessRequest_GetParameterNames
        *   CcspCwmpSoappoProcessRequest_SetParameterAttributes
        *   CcspCwmpSoappoProcessRequest_GetParameterAttributes
        *   CcspCwmpSoappoProcessRequest_AddObject
        *   CcspCwmpSoappoProcessRequest_DeleteObject
        *   CcspCwmpSoappoProcessRequest_Reboot
        *   CcspCwmpSoappoProcessRequest_Download
        *   CcspCwmpSoappoProcessRequest_Upload
        *   CcspCwmpSoappoProcessRequest_ScheduleInform
        *   CcspCwmpSoappoProcessRequest_SetVouchers
        *   CcspCwmpSoappoProcessRequest_GetOptions
        *   CcspCwmpSoappoProcessRequest
        *   CcspCwmpSoappoProcessResponse_GetRPCMethods
        *   CcspCwmpSoappoProcessResponse_Inform
        *   CcspCwmpSoappoProcessResponse_Kicked
        *   CcspCwmpSoappoProcessResponse
        *   CcspCwmpSoappoProcessFault
        *   CcspCwmpSoappoProcessSingleEnvelope
        *   CcspCwmpSoappoProcessSoapEnvelopes

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
        06/21/11    make TR-069 work with CCSP architecture.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/

#include "ccsp_cwmp_soappo_global.h"
#include "ansc_ato_interface.h"
#include "ansc_ato_external_api.h"
#include <string.h>

/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpSoappoProcessSoapHeader
            (
                ANSC_HANDLE                 hCwmpSoapHeader,
                PCHAR                       pNameSpace,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to parse CWMP SOAP Header.

    argument:

                ANSC_HANDLE                 hCwmpSoapHeader,
                The handle of cwmp soap header defined in 
                "ccsp_cwmp_cwmp_definitions.h"

                PCHAR                       pNameSpace,
                The SOAP namespace name.

                ANSC_HANDLE                 hXmlHandle
                The SOAP HEADER XML handle

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessSoapHeader
    (
        ANSC_HANDLE                 hCwmpSoapHeader,
        PCHAR                       pNameSpace,
        ANSC_HANDLE                 hXmlHandle
    )
{
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode        = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode      = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PCCSP_CWMP_SOAP_HEADER          pCwmpHeader     = (PCCSP_CWMP_SOAP_HEADER)hCwmpSoapHeader;
    PCHAR                           pNodeName       = NULL;
    CHAR                            pAttrName[64]   = { 0 };
    ULONG                           uLongValue      = 0;
    CHAR                            pValueBuf[128]  = { 0 };
    ULONG                           ulValueSize     = 128;

    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    while(pChildNode != NULL)
    {
        pNodeName =
            CcspCwmpSoappoUtilGetNodeNameWithoutNS
                (
                	AnscXmlDomNodeGetName(pChildNode)
                );

        uLongValue = 0;

        /* check "mustUnderstand" attribute value */
        if( pNameSpace != NULL && AnscSizeOfString(pNameSpace) != 0)
        {
            _ansc_sprintf
                (
                    pAttrName,
                    "%s:%s",
                    pNameSpace,
                    SOAP_MUSTUNDERSTAND
                );
        }
        else
        {
            AnscCopyString(pAttrName, SOAP_MUSTUNDERSTAND);
        }

        returnStatus =
        	AnscXmlDomNodeGetAttrUlong
                (
                    pChildNode,
                    pAttrName,
                    &uLongValue
                );

        if( AnscEqualString(pNodeName, CCSP_CWMP_ID, TRUE))
        {
            /*
             * The "mustUnderstand" attribute MUST be set to "1"
             */
            if( returnStatus != ANSC_STATUS_SUCCESS || uLongValue != 1)
            {
                CcspTr069PaTraceError
                    ((
                        "'mustUnderstand' attribute must be set to '1' in cwmp:ID.\n"
                    ));


                return ANSC_STATUS_FAILURE;
            }

            returnStatus =
            	AnscXmlDomNodeGetDataString(pChildNode, NULL, pValueBuf, &ulValueSize);

            if( returnStatus != ANSC_STATUS_SUCCESS)
            {
                return returnStatus;
            }

            pCwmpHeader->ElementMask   |= CCSP_CWMP_SOAP_HEADER_ID;
            pCwmpHeader->ID             = CcspTr069PaCloneString(pValueBuf);
            ulValueSize                 = 128;
        }
        else if( AnscEqualString(pNodeName, CCSP_CWMP_HOLDREQUESTS, TRUE))
        {
            /*
             * The "mustUnderstand" attribute MUST be set to "1"
             */
            if( returnStatus != ANSC_STATUS_SUCCESS || uLongValue != 1)
            {
                CcspTr069PaTraceError
                    ((
                        "'mustUnderstand' attribute must be set to '1' in cwmp:HoldRequests.\n"
                    ));


                return ANSC_STATUS_FAILURE;
            }

            returnStatus =
            	AnscXmlDomNodeGetDataUlong(pChildNode, NULL, &uLongValue);

            if( returnStatus != ANSC_STATUS_SUCCESS)
            {
                return returnStatus;
            }

            pCwmpHeader->ElementMask   |= CCSP_CWMP_SOAP_HEADER_HoldRequests;

            if( uLongValue == 1)
            {
                pCwmpHeader->bHoldRequests = TRUE;
            }
            else
            {
                pCwmpHeader->bHoldRequests = FALSE;
            }
        }
        else if( AnscEqualString(pNodeName, CCSP_CWMP_NOMOREREQUESTS, TRUE))
        {
			/*
			 * According to WT151, "NoMoreRequests" field is deprecated, it will be
			 * ignored.
			 */
#if 0
            /*
             * The "mustUnderstand" attribute MUST be either absent or set to "0"
             */
            if( returnStatus == ANSC_STATUS_SUCCESS && uLongValue == 1)
            {
                CcspTr069PaTraceError
                    ((
                        "'mustUnderstand' attribute must either absent or set to '0'cwmp:NoMoreRequests.\n"
                    ));


                return ANSC_STATUS_FAILURE;
            }

            returnStatus =
            	AnscXmlDomNodeGetDataUlong(pChildNode, NULL, &uLongValue);

            if( returnStatus != ANSC_STATUS_SUCCESS)
            {
                return returnStatus;
            }

            pCwmpHeader->ElementMask   |= CCSP_CWMP_SOAP_HEADER_NoMoreRequests;

            if( uLongValue == 1)
            {
                pCwmpHeader->bNoMoreRequests = TRUE;
            }
            else
#endif
            {
                pCwmpHeader->bNoMoreRequests = FALSE;
            }
        }

        /* check the next one */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);
    }

    return returnStatus;
}


/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_SetParameterValues
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle

                char*                       pRequestID,
                The request id;

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest_SetParameterValues
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE             pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pKeyNode     = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
	PANSC_ATOM_TABLE_OBJECT         pAtoName     = NULL;
    PCCSP_CWMP_PARAM_VALUE          pParamArray  = NULL;
    PCCSP_CWMP_PARAM_VALUE          pParamValue  = NULL;
    ULONG                           uMaxParam    = 128;
    ULONG                           uParamCount  = 0;
    CHAR                            pValue[48]   = { 0 };
    ULONG                           length       = 32;
    ULONG                           i            = 0;
    ULONG                           ulSubError   = CCSP_CWMP_CPE_CWMP_FaultCode_invalidParamType;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * ParameterList|ParameterValue| Array of name-value pairs.                *
    *              |Struct[]      |                                           *
    *-------------------------------------------------------------------------*
    * ParameterKey | string(32)   | The value to set the ParameterKey param...*
    ***************************************************************************/

    pListNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    pKeyNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetTailChild(pXmlNode);

    if( pListNode == NULL || pKeyNode == NULL ||
        pKeyNode  != AnscXmlDomNodeGetNextChild(pXmlNode, pListNode))
    {
        goto EXIT2;
    }

    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pListNode);

    while( pChildNode != NULL)
    {
        uParamCount ++;

        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetNextChild(pListNode, pChildNode);
    }

    if( uParamCount >= uMaxParam)
    {
        CcspTr069PaTraceError(("Too many parameters (%lu) in 'SetParameterValues', maximum number set to %lu\n", uParamCount, uMaxParam));

        goto EXIT2;
    }

	if( uParamCount > 0)
	{
		pParamArray = (PCCSP_CWMP_PARAM_VALUE)
			CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_PARAM_VALUE) * uParamCount);

		if( pParamArray == NULL)
		{
			returnStatus = ANSC_STATUS_RESOURCES;

			goto EXIT2;
		}

		/*
		 * The get the parameter value one by one
		 */
		uParamCount = 0;
		pChildNode  = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetHeadChild(pListNode);

		pAtoName =
			(PANSC_ATOM_TABLE_OBJECT)AnscCreateAtomTable
				(
					NULL,
					NULL,
					NULL
				);

		while( pChildNode != NULL)
		{
			pParamValue = &pParamArray[uParamCount];
			uParamCount++;

			/* parse the value of ParameterValueStruct */
			returnStatus =
				CcspCwmpSoappoUtilGetParamValue
					(
                        hCcspCwmpMcoIf,
						pChildNode,
						pParamValue
					);

			if( returnStatus != ANSC_STATUS_SUCCESS)
			{
				goto EXIT2;
			}


			/*
			 * According to WT151, entries with the same parameter names are not
			 * allowed in the method, we need to check it.
			 */
			if( uParamCount > 0 && pAtoName != NULL)
			{
				if( pAtoName->GetAtomByName
						(
							pAtoName,
							pParamValue->Name
						) != NULL)
				{
					CcspTr069PaTraceError(("ERROR - Duplicated entry in 'SetParameterValues': %s\n", pParamValue->Name));

                    ulSubError = CCSP_CWMP_CPE_CWMP_FaultCode_invalidParamName;

					goto EXIT2;

				}

				pAtoName->AddAtomByName
					(
						pAtoName,
						pParamValue->Name,
						0,
						NULL
					);

			}

			pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
				AnscXmlDomNodeGetNextChild(pListNode, pChildNode);
		}
	}

    /*
     * Get the ParameterKey value
     */
    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pKeyNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the ParameterKey argument of method 'SetParameterValues'\n"));

        goto EXIT2;
    }

    if( pCcspCwmpMcoIf != NULL)
    {
        returnStatus =
            pCcspCwmpMcoIf->SetParameterValues
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)pParamArray,
                    uParamCount,
                    pValue
                );
    }

    goto EXIT;

EXIT2:

    returnStatus =
        CcspCwmpSoappoUtilProcessInvalidArgumentSPVRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               pParamValue->Name,
               ulSubError
            );

EXIT:

    if( pParamArray != NULL)
    {
        for( i = 0; i < uParamCount; i ++)
        {
            pParamValue = &pParamArray[i];

            if( pParamValue->Tr069DataType == CCSP_CWMP_TR069_DATA_TYPE_Unspecified )
            {
                pParamValue->Value = NULL;
            }

            CcspCwmpCleanParamValue(pParamValue);
        }

        CcspTr069PaFreeMemory(pParamArray);
    }

	if( pAtoName != NULL)
	{
		pAtoName->Remove(pAtoName);
	}

    return returnStatus;
}

/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_GetParameterValues
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest_GetParameterValues
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE             pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PSLAP_STRING_ARRAY              pStringArray = (PSLAP_STRING_ARRAY        )NULL;
    CHAR                            pValue[256]  = { 0 };
    ULONG                           length       = 256;
    ULONG                           uCount       = 0;
    ULONG                           i            = 0;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * ParameterName|string(256)[] | Array of strings.                         *
    ***************************************************************************/

    pListNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pListNode == NULL || pListNode != AnscXmlDomNodeGetTailChild(pXmlNode))
    {
        CcspTr069PaTraceError(("Invalid argument in method 'GetParameterValues'\n"));

        goto EXIT2;
    }

    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pListNode);

    while( pChildNode != NULL)
    {
        uCount ++;

        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetNextChild(pListNode, pChildNode);
    }

    if( uCount != 0)
    {
    	 SlapAllocStringArray2(uCount, pStringArray);

        if( pStringArray == NULL)
        {
            goto EXIT2;
        }

        i = 0;

        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetHeadChild(pListNode);

        while( pChildNode != NULL)
        {
            length = 256;
            AnscZeroMemory(pValue, length);

            returnStatus =
            	AnscXmlDomNodeGetDataString
                    (
                        pChildNode,
                        NULL,
                        pValue,
                        &length
                    );

            if( returnStatus != ANSC_STATUS_SUCCESS)
            {
                CcspTr069PaTraceError(("Failed to get the argument '%lu' of method 'GetParameterValues'\n", i));

                goto EXIT2;
            }

            if( length > 0)
            {
                pStringArray->Array.arrayString[i] = CcspTr069PaCloneString(pValue);
            }
            else
            {
                CcspTr069PaTraceWarning(("Empty Parameter Name in 'GetParameterValue'.\n"));
                pStringArray->Array.arrayString[i] = CcspTr069PaCloneString(DM_ROOTNAME);
            }

            i ++;

            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
				AnscXmlDomNodeGetNextChild(pListNode, pChildNode);
        }

    }

    if( pCcspCwmpMcoIf != NULL)
    {
        returnStatus =
            pCcspCwmpMcoIf->GetParameterValues
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    pStringArray
                );
    }

    goto EXIT;

EXIT2:

    returnStatus =
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "GetParameterValues"
            );

EXIT:

    if( pStringArray != NULL)
    {
        SlapFreeVarArray(pStringArray);
    }

    return returnStatus;
}

/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_GetParameterNames
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest_GetParameterNames
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE             pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pPathNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pNextNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    BOOL                            bNextLevel   = FALSE;
    CHAR                            pValue[256]  = { 0 };
    ULONG                           length       = 256;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * ParameterPath|string(256)   | A string of a complete parameter name.    *
    *-------------------------------------------------------------------------*
    * NextLevel    | boolean      | NextLevel only or not                     *
    ***************************************************************************/

    pPathNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    pNextNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetTailChild(pXmlNode);

    if( pPathNode == NULL || pNextNode == NULL || pPathNode == pNextNode)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'GetParameterNames'\n"));

        goto EXIT;
    }

    /*
     *  Get the ParameterPath
     */
    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pPathNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the ParameterPath argument of method 'GetParameterNames'\n"));

        goto EXIT;
    }

    /*
     *  Get the bNextLevel value
     */
     returnStatus =
    	 AnscXmlDomNodeGetDataBoolean
            (
                pNextNode,
                NULL,
                &bNextLevel
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the NextLevel argument of method 'GetParameterNames'\n"));

        goto EXIT;
    }


    if( pCcspCwmpMcoIf != NULL)
    {
        returnStatus =
            pCcspCwmpMcoIf->GetParameterNames
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    pValue,
                    bNextLevel
                );
    }

    return returnStatus;


EXIT:

    return
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "GetParameterNames"
            );
}
/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_SetParameterAttributes
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest_SetParameterAttributes
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_SET_PARAM_ATTRIB     pParamArray  = NULL;
    PCCSP_CWMP_SET_PARAM_ATTRIB     pParamAttr   = NULL;
    ULONG                           uMaxParam    = 128;
    ULONG                           uParamCount  = 0;
    ULONG                           i            = 0;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * ParameterList|ParameterAttr | List of attribute setairs.                *
    *              |Struct[]      |                                           *
    ***************************************************************************/
    pListNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pListNode == NULL || pListNode != AnscXmlDomNodeGetTailChild(pXmlNode))
    {
        CcspTr069PaTraceError(("Invalid argument in method 'SetParameterAttributes'\n"));

        goto EXIT2;
    }

    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pListNode);

    while( pChildNode != NULL)
    {
        uParamCount ++;

        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetNextChild(pListNode, pChildNode);
    }

    if( uParamCount >= uMaxParam)
    {
        CcspTr069PaTraceError(("Too many parameters (%lu) in 'SetParameterAttributes', maximum number set to %lu\n", uParamCount, uMaxParam));

        goto EXIT2;
    }

    if( uParamCount > 0)
    {
        pParamArray = (PCCSP_CWMP_SET_PARAM_ATTRIB)
            CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SET_PARAM_ATTRIB) * uParamCount);

        if( pParamArray == NULL)
        {
            goto EXIT2;
        }

        /*
         * The get the parameter attribute one by one
         */
        uParamCount = 0;
        pChildNode  = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetHeadChild(pListNode);

        while( pChildNode != NULL)
        {
            pParamAttr = &pParamArray[uParamCount];
            uParamCount ++;

            /* parse the value of ParameterAttributeStruct */
            returnStatus =
                CcspCwmpSoappoUtilGetParamAttribute
                    (
                        pChildNode,
                        pParamAttr
                    );

            if( returnStatus != ANSC_STATUS_SUCCESS)
            {
                goto EXIT2;
            }

            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
				AnscXmlDomNodeGetNextChild(pListNode, pChildNode);
        }
    }

    if( pCcspCwmpMcoIf != NULL)
    {
        returnStatus =
            pCcspCwmpMcoIf->SetParameterAttributes
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    (ANSC_HANDLE)pParamArray,
                    uParamCount
                );
    }

    goto EXIT;

EXIT2:

    returnStatus =
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "SetParameterAttributes"
            );

EXIT:

    if( pParamArray != NULL)
    {
        for( i = 0; i < uParamCount; i ++)
        {
            pParamAttr = &pParamArray[i];

            CcspCwmpCleanSetParamAttrib(pParamAttr);
        }

        CcspTr069PaFreeMemory(pParamArray);
    }

    return returnStatus;
}


/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_GetParameterAttributes
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest_GetParameterAttributes
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PSLAP_STRING_ARRAY              pStringArray = (PSLAP_STRING_ARRAY        )NULL;
    CHAR                            pValue[256]  = { 0 };
    ULONG                           length       = 256;
    ULONG                           uCount       = 0;
    ULONG                           i            = 0;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * ParameterName|string(256)[] | Array of strings.                         *
    ***************************************************************************/

    pListNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pListNode == NULL || pListNode != AnscXmlDomNodeGetTailChild(pXmlNode))
    {
        goto EXIT2;
    }

    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pListNode);

    while( pChildNode != NULL)
    {
        uCount ++;

        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetNextChild(pListNode, pChildNode);
    }

    if( uCount != 0)
    {
    	 SlapAllocStringArray2(uCount, pStringArray);

        if( pStringArray == NULL)
        {
            goto EXIT2;
        }

        i = 0;

        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetHeadChild(pListNode);

        while( pChildNode != NULL)
        {
            length = 256;
            AnscZeroMemory(pValue, length);

            returnStatus =
            	AnscXmlDomNodeGetDataString
                    (
                        pChildNode,
                        NULL,
                        pValue,
                        &length
                    );

            if( returnStatus != ANSC_STATUS_SUCCESS)
            {
                CcspTr069PaTraceError(("Failed to get the argument '%lu' of method 'GetParameterAttributes'\n", i));

                goto EXIT2;
            }

            if( length > 0)
            {
                pStringArray->Array.arrayString[i] = CcspTr069PaCloneString(pValue);
            }
            else
            {
				CcspTr069PaTraceWarning(("Empty Parameter Name in 'GetParameterAttributes'.\n"));

				pStringArray->Array.arrayString[i] = CcspTr069PaCloneString(DM_ROOTNAME);
            }

            i ++;

            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
				AnscXmlDomNodeGetNextChild(pListNode, pChildNode);
        }

    }

    if( pCcspCwmpMcoIf != NULL)
    {
        returnStatus =
            pCcspCwmpMcoIf->GetParameterAttributes
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    pStringArray
                );
    }

    goto EXIT;

EXIT2:

    returnStatus =
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "GetParameterAttributes"
            );

EXIT:

    if( pStringArray != NULL)
    {
        for ( i = 0; i < uCount; i++ )
        {
            if (pStringArray->Array.arrayString[i] != NULL)
            {
                CcspTr069PaFreeMemory(pStringArray->Array.arrayString[i]);
                pStringArray->Array.arrayString[i] = NULL;
            }
        }
        SlapFreeVarArray(pStringArray);
    }

    return returnStatus;
}
/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_AddObject
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest_AddObject
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    CHAR                            pObjName[256]= { 0 };
    CHAR                            pParamKey[32]= { 0 };
    ULONG                           length       = 256;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * ObjectName   | string(256)  | The path name of the collection of objects*
    *              |              | for which a new instance is to be created.*
    *              |              | The path name MUST end with a "." after   *
    *              |              | the last node in the ...                  *
    *-------------------------------------------------------------------------*
    * ParameterKey | string(32)   | The value to set the ParameterKey param...*
    ***************************************************************************/

    /*
     *  Get ObjectName
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'AddObject'\n"));

        goto EXIT;
    }

    length = 256;

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pObjName,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        goto EXIT;
    }

    /*
     * Get ParameterKey
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL || pChildNode != AnscXmlDomNodeGetTailChild(pXmlNode))
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'AddObject'\n"));

        goto EXIT;
    }

    length = 32;

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pParamKey,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the argument value of method 'AddObject'\n"));

        goto EXIT;
    }

    if( pCcspCwmpMcoIf != NULL)
    {
        returnStatus =
            pCcspCwmpMcoIf->AddObject
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    pObjName,
                    pParamKey
                );
    }

    return returnStatus;

EXIT:

    return
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "AddObject"
            );

}
/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_DeleteObject
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest_DeleteObject
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    CHAR                            pObjName[256]= { 0 };
    CHAR                            pParamKey[32]= { 0 };
    ULONG                           length       = 256;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * ObjectName   | string(256)  | The path name of the collection of objects*
    *              |              | for which a new instance is to be created.*
    *              |              | The path name MUST end with a "." after   *
    *              |              | the last node in the ...                  *
    *-------------------------------------------------------------------------*
    * ParameterKey | string(32)   | The value to set the ParameterKey param...*
    ***************************************************************************/

    /*
     *  Get ObjectName
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'DeleteObject'\n"));

        goto EXIT;
    }

    length = 256;

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pObjName,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the argument value of method 'DeleteObject'\n"));

        goto EXIT;
    }

    /*
     * Get ParameterKey
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL || pChildNode != AnscXmlDomNodeGetTailChild(pXmlNode))
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'DeleteObject'\n"));

        goto EXIT;
    }

    length = 32;

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pParamKey,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the argument value of method 'DeleteObject'\n"));

        goto EXIT;
    }

    if( pCcspCwmpMcoIf != NULL)
    {
        returnStatus =
            pCcspCwmpMcoIf->DeleteObject
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    pObjName,
                    pParamKey
                );
    }

    return returnStatus;

EXIT:

    return
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "AddObject"
            );

}

/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_Reboot
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest_Reboot
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    CHAR                            pValueBuf[64]= { 0 };
    ULONG                           length       = 64;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * CommandKey   | string(32)   | The string to return in the CommandKey... *
    ***************************************************************************/

    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pChildNode == NULL || pChildNode != AnscXmlDomNodeGetTailChild(pXmlNode))
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Reboot'\n"));

        goto EXIT;
    }

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValueBuf,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the argument value of method 'Reboot'\n"));

        goto EXIT;
    }

    if( pCcspCwmpMcoIf != NULL)
    {
        return
            pCcspCwmpMcoIf->Reboot
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    pValueBuf
                );
    }

    return returnStatus;

EXIT:

    return
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "Reboot"
            );
}

/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_Download
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/

ANSC_STATUS
CcspCwmpSoappoProcessRequest_Download
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    CCSP_CWMP_MCO_DOWNLOAD_REQ      downloadReq  = { 0 };
    PCCSP_CWMP_MCO_DOWNLOAD_REQ     pDownReq     = &downloadReq;
    CHAR                            pValue[257]  = { 0 };
    ULONG                           length       = 256;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * CommandKey   | string(32)   | a string                                  *
    *-------------------------------------------------------------------------*
    * FileType     | string(64)   | such as "2 Web Content"                  .*
    *-------------------------------------------------------------------------*
    * URL          | string(256)  | dest file location n...                  .*
    *-------------------------------------------------------------------------*
    * Username     | string(256)  | UserName for authenticate                .*
    *-------------------------------------------------------------------------*
    * Password     | string(256)  | Password to authenticate                 .*
    *-------------------------------------------------------------------------*
    * FileSize     | UINT32       | the size of the file to be downloaded    .*
    *-------------------------------------------------------------------------*
    *TargetFileName| string(256)  | target file name                         .*
    *-------------------------------------------------------------------------*
    * DelaySeconds | UINT32       | in seconds                               .*
    *-------------------------------------------------------------------------*
    * SuccURL      | string(256)  | redirect url if succeeds                 .*
    *-------------------------------------------------------------------------*
    * FailureURL   | string(256)  | redirect url if fails                    .*
    ***************************************************************************/

    /*
     * Get CommandKey
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Download'\n"));

        goto EXIT2;
    }

    length = 32;

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'CommandKey' argument value of method 'Download'\n"));

        goto EXIT2;

    }

    pDownReq->CommandKey = CcspTr069PaCloneString(pValue);

    /*
     * Get FileType
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Download'\n"));

        goto EXIT2;
    }

    length = 64;
    AnscZeroMemory(pValue, length);

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'FileType' argument value of method 'Download'\n"));

        goto EXIT2;
    }

    pDownReq->FileType = CcspTr069PaCloneString(pValue);

    /*
     * Get URL
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Download'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    length = 256;
    AnscZeroMemory(pValue, length);

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'URL' argument value of method 'Download'\n"));

        goto EXIT2;

    }

    pDownReq->Url = CcspTr069PaCloneString(pValue);

    /*
     * Get UserName
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Download'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    length = 256;
    AnscZeroMemory(pValue, length);

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'UserName' argument value of method 'Download'\n"));

        goto EXIT2;
    }

    pDownReq->Username = CcspTr069PaCloneString(pValue);

    /*
     * Get Password
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Download'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    length = 256;
    AnscZeroMemory(pValue, length);

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'Password' argument value of method 'Download'\n"));

        goto EXIT2;
    }

    pDownReq->Password = CcspTr069PaCloneString(pValue);

    /*
     * Get FileSize
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Download'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    returnStatus =
    	AnscXmlDomNodeGetDataUlong
            (
                pChildNode,
                NULL,
                &pDownReq->FileSize
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'FileSize' argument value of method 'Download'\n"));

        goto EXIT2;
    }

    /*
     * Get TargetFileName
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Download'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    length = 256;
    AnscZeroMemory(pValue, length);

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'TargetFileName' argument value of method 'Download'\n"));

        goto EXIT2;
    }

    pDownReq->TargetFileName = CcspTr069PaCloneString(pValue);

    /*
     * Get DelaySeconds
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Download'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    returnStatus =
    	AnscXmlDomNodeGetDataUlong
            (
                pChildNode,
                NULL,
                &pDownReq->DelaySeconds
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'DelaySeconds' argument value of method 'Download'\n"));

        goto EXIT2;
    }

    /*
     * Get SuccessURL
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Download'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    length = 256;
    AnscZeroMemory(pValue, length);

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'SuccessURL' argument value of method 'Download'\n"));

        goto EXIT2;
    }

    pDownReq->SuccessUrl = CcspTr069PaCloneString(pValue);

    /*
     * Get FailureURL
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Download'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    length = 256;
    AnscZeroMemory(pValue, length);

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'FailureURL' argument value of method 'Download'\n"));

        goto EXIT2;
    }

    pDownReq->FailureUrl = CcspTr069PaCloneString(pValue);

    /*
     * Call the 'Download' of CcspCwmpMco Interface
     */
    if( pCcspCwmpMcoIf != NULL)
    {
        returnStatus =
            pCcspCwmpMcoIf->Download
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    pDownReq
                );
    }

    goto EXIT;

EXIT2:

    returnStatus =
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "Download"
            );

EXIT:

    CcspCwmpMcoCleanDownloadReq(pDownReq);

    return returnStatus;
}

/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_Upload
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest_Upload
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    CCSP_CWMP_MCO_UPLOAD_REQ        uploadReq    = { 0 };
    PCCSP_CWMP_MCO_UPLOAD_REQ       pUploadReq   = &uploadReq;
    CHAR                            pValue[257]  = { 0 };
    ULONG                           length       = 256;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * CommandKey   | string(32)   | a string                                  *
    *-------------------------------------------------------------------------*
    * FileType     | string(64)   | such as "2 Vendor Log File"              .*
    *-------------------------------------------------------------------------*
    * URL          | string(256)  | dest file location n...                  .*
    *-------------------------------------------------------------------------*
    * Username     | string(256)  | UserName for authenticate                .*
    *-------------------------------------------------------------------------*
    * Password     | string(256)  | Password to authenticate                 .*
    *-------------------------------------------------------------------------*
    * DelaySeconds | UINT32       | in seconds                               .*
    ***************************************************************************/

    /*
     * Get CommandKey
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Upload'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    length = 32;

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'CommandKey' argument value of method 'Upload'\n"));

        goto EXIT2;
    }

    pUploadReq->CommandKey = CcspTr069PaCloneString(pValue);

    /*
     * Get FileType
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Upload'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    length = 64;
    AnscZeroMemory(pValue, length);

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'FileType' argument value of method 'Upload'\n"));

        goto EXIT2;
    }

    pUploadReq->FileType = CcspTr069PaCloneString(pValue);

    /*
     * Get URL
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Upload'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    length = 256;
    AnscZeroMemory(pValue, length);

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'URL' argument value of method 'Upload'\n"));

        goto EXIT2;
    }

    pUploadReq->Url = CcspTr069PaCloneString(pValue);

    /*
     * Get UserName
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Upload'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    length = 256;
    AnscZeroMemory(pValue, length);

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'UserName' argument value of method 'Upload'\n"));

        goto EXIT2;
    }

    pUploadReq->Username = CcspTr069PaCloneString(pValue);

    /*
     * Get Password
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Upload'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    length = 256;
    AnscZeroMemory(pValue, length);

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'Password' argument value of method 'Upload'\n"));

        goto EXIT2;
    }

    pUploadReq->Password = CcspTr069PaCloneString(pValue);

    /*
     * Get DelaySeconds
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'Upload'\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT2;
    }

    returnStatus =
    	AnscXmlDomNodeGetDataUlong
            (
                pChildNode,
                NULL,
                &pUploadReq->DelaySeconds
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'DelaySeconds' argument value of method 'Upload'\n"));

        goto EXIT2;
    }

    /*
     * Call the 'Upload' of CcspCwmpMco Interface
     */
    if( pCcspCwmpMcoIf != NULL)
    {
        returnStatus =
            pCcspCwmpMcoIf->Upload
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    pUploadReq
                );
    }

    goto EXIT;

EXIT2:

    returnStatus =
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "Upload"
            );

EXIT:

    CcspCwmpMcoCleanUploadReq(pUploadReq);

    return returnStatus;
}


/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_ChangeDUState
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/

static
ANSC_STATUS
CcspCwmpSoappoParse_ChangeDUState_Req_Operations
    (
        PANSC_XML_DOM_NODE_OBJECT   pOperationsNode,
        PCCSP_TR069_CDS_REQ         pCdsReq
    )
{
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    PANSC_XML_DOM_NODE_OBJECT       pOpNode         = (PANSC_XML_DOM_NODE_OBJECT  )NULL;
    PANSC_XML_DOM_NODE_OBJECT       pNode;
    char                            buffer[1025];
    ULONG                           ulBufSize;
    ULONG                           ulOpCount       = 0;
    char*                           pNodeName       = NULL;

    pOpNode = (PANSC_XML_DOM_NODE_OBJECT)AnscXmlDomNodeGetHeadChild((ANSC_HANDLE)pOperationsNode);
    while ( pOpNode )
    {
        ulOpCount ++;
        pOpNode = AnscXmlDomNodeGetNextChild(pOperationsNode, pOpNode);
    }

    if ( ulOpCount == 0 )
    {
        CcspTr069PaTraceError(("Failed to parse RPC ChangeDUState - no operation specified.\n"));
        goto EXIT;
    }

    AnscZeroMemory(pCdsReq, sizeof(CCSP_TR069_CDS_REQ));
    pCdsReq->Operations = AnscAllocateMemory(ulOpCount * sizeof(CCSP_TR069_CDS_Operation));
    if ( !pCdsReq->Operations )
    {
        CcspTr069PaTraceError(("Failed to parse RPC ChangeDUState - no resources.\n"));
        returnStatus = ANSC_STATUS_RESOURCES;
        goto EXIT;
    }
    AnscZeroMemory(pCdsReq->Operations, sizeof(CCSP_TR069_CDS_Operation) * ulOpCount);

    ulOpCount = 0;
    pOpNode = (PANSC_XML_DOM_NODE_OBJECT)AnscXmlDomNodeGetHeadChild((ANSC_HANDLE)pOperationsNode);
    while ( pOpNode )
    {
        pNodeName = AnscXmlDomNodeGetName((ANSC_HANDLE)pOpNode);
        if ( AnscEqualString(pNodeName, "InstallOpStruct", FALSE) )
        {
            PCCSP_TR069_CDS_Install pOpInstall = &pCdsReq->Operations[ulOpCount].op.Install;

            pCdsReq->Operations[ulOpCount].Op = CCSP_TR069_CDS_OP_Install;

            /* parse URL */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "URL");
            if ( pNode )
            {
                ulBufSize = 1024;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpInstall->Url = CcspTr069PaCloneString(buffer);
            }

            /* parse UUID */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "UUID");
            if ( pNode )
            {
                ulBufSize = 36;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpInstall->Uuid = CcspTr069PaCloneString(buffer);
            }

            /* parse Username */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "Username");
            if ( pNode )
            {
                ulBufSize = 256;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpInstall->Username = CcspTr069PaCloneString(buffer);
            }

            /* parse Password */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "Password");
            if ( pNode )
            {
                ulBufSize = 256;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpInstall->Password = CcspTr069PaCloneString(buffer);
            }

            /* parse ExecutionEnvRef */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "ExecutionEnvRef");
            if ( pNode )
            {
                ulBufSize = 257;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpInstall->ExecEnvRef = CcspTr069PaCloneString(buffer);
            }
        }
        else if ( AnscEqualString(pNodeName, "UpdateOpStruct", FALSE) )
        {
            PCCSP_TR069_CDS_Update  pOpUpdate = &pCdsReq->Operations[ulOpCount].op.Update;

            pCdsReq->Operations[ulOpCount].Op = CCSP_TR069_CDS_OP_Update;

            /* parse UUID */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "UUID");
            if ( pNode )
            {
                ulBufSize = 36;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpUpdate->Uuid = CcspTr069PaCloneString(buffer);
            }

            /* parse Version */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "Version");
            if ( pNode )
            {
                ulBufSize = 32;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpUpdate->Version = CcspTr069PaCloneString(buffer);
            }

            /* parse URL */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "URL");
            if ( pNode )
            {
                ulBufSize = 1024;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpUpdate->Url = CcspTr069PaCloneString(buffer);
            }

            /* parse Username */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "Username");
            if ( pNode )
            {
                ulBufSize = 256;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpUpdate->Username = CcspTr069PaCloneString(buffer);
            }

            /* parse Password */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "Password");
            if ( pNode )
            {
                ulBufSize = 256;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpUpdate->Password = CcspTr069PaCloneString(buffer);
            }
        }
        else if ( AnscEqualString(pNodeName, "UninstallOpStruct", FALSE) )
        {
            PCCSP_TR069_CDS_Uninstall   pOpUninstall = &pCdsReq->Operations[ulOpCount].op.Uninstall;

            pCdsReq->Operations[ulOpCount].Op = CCSP_TR069_CDS_OP_Uninstall;

            /* parse UUID */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "UUID");
            if ( pNode )
            {
                ulBufSize = 36;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpUninstall->Uuid = CcspTr069PaCloneString(buffer);
            }

            /* parse Version */
            pNode = (PANSC_XML_DOM_NODE_OBJECT)
                AnscXmlDomNodeGetChildByName((ANSC_HANDLE)pOpNode, "Version");
            if ( pNode )
            {
                ulBufSize = 32;
                AnscXmlDomNodeGetDataString((ANSC_HANDLE)pNode, NULL, buffer, &ulBufSize);
                buffer[ulBufSize] = 0;
            
                pOpUninstall->Version = CcspTr069PaCloneString(buffer);
            }
        }

        ulOpCount ++;
        pOpNode = AnscXmlDomNodeGetNextChild(pOperationsNode, pOpNode);
    }

    pCdsReq->NumOperations = ulOpCount;

EXIT:
    if ( ulOpCount == 0 )
    {
        CcspTr069PaTraceError(("Failed to parse RPC ChangeDUState - no operations.\n"));
    }

    return  returnStatus;
}


ANSC_STATUS
CcspCwmpSoappoProcessRequest_ChangeDUState
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE             pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    CCSP_TR069_CDS_REQ              CcspCdsReq   = {0};
    PCCSP_TR069_CDS_REQ             pCcspCdsReq  = &CcspCdsReq;
    CHAR                            pValue[257]  = { 0 };
    ULONG                           length       = 256;

    /***************************************************************************
        Operations {OperationStruct[]}
            InstallOp {InstallOpStruct}
                URL {string(1024)}
                UUID {string(36)}
                Username {string(256)}
                Password {string(256)}
                ExecutionEnvRef {string(256)}
            UpdateOp {UpdateOpStruct}
                UUID {string(36)}
                Version {string(32)}
                URL {string(1024)}
                Username {string(256)}
                Password {string(256)}
            UninstallOp {UninstallOpStruct}
                UUID {string(36)}
                Version {string(32)}
        CommandKey {string(32)}
    ***************************************************************************/

    /*
     * Get CommandKey
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetChildByName(pXmlNode, "CommandKey");

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'ChangeDUState'\n"));
        goto EXIT2;
    }

    length = 32;

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValue,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the 'CommandKey' argument value of method 'ChangeDUState'\n"));
        goto EXIT2;
    }

    /*
     * Get Operations
     */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetChildByName(pXmlNode, "Operations");

    if ( pChildNode == NULL )
    {
        CcspTr069PaTraceError(("Invalid argument in method 'ChangeDUState'\n"));
        goto EXIT2;
    }

    returnStatus = CcspCwmpSoappoParse_ChangeDUState_Req_Operations(pChildNode, pCcspCdsReq);

    /*
     * Call the 'ChangeDUState' of CcspCwmpMco Interface
     */
    if ( returnStatus == ANSC_STATUS_SUCCESS && pCcspCwmpMcoIf != NULL )
    {
        pCcspCdsReq->CommandKey = CcspTr069PaCloneString(pValue);

        returnStatus =
            pCcspCwmpMcoIf->ChangeDUState
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    pCcspCdsReq
                );
    }

    goto EXIT;

EXIT2:

    returnStatus =
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "ChangeDUState"
            );

EXIT:

    CcspTr069CdsReqClean(pCcspCdsReq);

    return returnStatus;
}



/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_ScheduleInform
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/

ANSC_STATUS
CcspCwmpSoappoProcessRequest_ScheduleInform
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pDelayNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pKeyNode     = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    CHAR                            pValueBuf[64]= { 0 };
    ULONG                           length       = 64;
    ULONG                           uLongValue   = 0;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * DelaySeconds | UINT32       | The number of seconds from the time ....  *
    *-------------------------------------------------------------------------*
    * CommandKey   | string(32)   | The string to return...                  .*
    ***************************************************************************/

    pDelayNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    pKeyNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetTailChild(pXmlNode);

    if( pDelayNode == NULL || pKeyNode == NULL || pDelayNode == pKeyNode)
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'ScheduleInform'\n"));

        goto EXIT;
    }

    /*
     * Get DelaySeconds
     */
    returnStatus =
    	AnscXmlDomNodeGetDataUlong
            (
                pDelayNode,
                NULL,
                &uLongValue
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the DelaySeconds argument value of method 'ScheduleInform'\n"));

        goto EXIT;
    }

    /*
     * Get CommandKey
     */
    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pKeyNode,
                NULL,
                pValueBuf,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the CommandKey argument value of method 'ScheduleInform'\n"));

        goto EXIT;
    }

    if( pCcspCwmpMcoIf != NULL)
    {
        return
            pCcspCwmpMcoIf->ScheduleInform
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    uLongValue,
                    pValueBuf
                );
    }

    return returnStatus;

EXIT:

    return
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "ScheduleInform"
            );

}
/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_SetVouchers
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest_SetVouchers
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pListNode    = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PSLAP_STRING_ARRAY              pStringArray = (PSLAP_STRING_ARRAY        )NULL;
    CHAR                            pValue[1025] = { 0 };
    ULONG                           length       = 1024;
    ULONG                           uCount       = 0;
    ULONG                           i            = 0;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * VoucherList  | base64[]     | Array of Vouchers                         *
    ***************************************************************************/

    pListNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pListNode == NULL || pListNode != AnscXmlDomNodeGetTailChild(pXmlNode))
    {
        CcspTr069PaTraceError(("Invalid argument in method 'SetVouchers'\n"));

        goto EXIT;
    }

    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pListNode);

    while( pChildNode != NULL)
    {
        uCount ++;

        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetNextChild(pListNode, pChildNode);
    }

    if( uCount != 0)
    {
    	 SlapAllocStringArray2(uCount, pStringArray);

        if( pStringArray == NULL)
        {
            goto EXIT;
        }

        i = 0;

        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetHeadChild(pListNode);

        while( pChildNode != NULL)
        {
            length = 1024;
            AnscZeroMemory(pValue, length);

            returnStatus =
            	AnscXmlDomNodeGetDataString
                    (
                        pChildNode,
                        NULL,
                        pValue,
                        &length
                    );

            if( returnStatus != ANSC_STATUS_SUCCESS)
            {
                CcspTr069PaTraceError(("Failed to get the argument '%lu' of method 'SetVouchers'\n", i));

                goto EXIT;
            }

            if( length > 0)
            {
                pStringArray->Array.arrayString[i] = CcspTr069PaCloneString(pValue);
            }
            else
            {
                pStringArray->Array.arrayString[i] = CcspTr069PaCloneString("");
            }

            i ++;

            pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
				AnscXmlDomNodeGetNextChild(pListNode, pChildNode);
        }

    }

    if( pCcspCwmpMcoIf != NULL)
    {
        returnStatus =
            pCcspCwmpMcoIf->SetVouchers
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    pStringArray
                );
    }

    goto EXIT2;

EXIT:

    returnStatus =
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "SetVouchers"
            );

EXIT2:

    if( pStringArray != NULL)
    {
        SlapFreeVarArray(pStringArray);
    }

    return returnStatus;
}

/**********************************************************************

    prototype:

        CcspCwmpSoappoProcessRequest_GetOptions
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                char*                       pRequestID,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process a request method

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                char*                       pRequestID,

                ANSC_HANDLE                 hXmlHandle
                The Child XML node of SOAP Body.

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest_GetOptions
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        char*                       pRequestID,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    CHAR                            pValueBuf[64]= { 0 };
    ULONG                           length       = 64;

    /***************************************************************************
    * Argument     | Type         | Description                               *
    ***************************************************************************
    * OptionName   | string(64)   | a string of an option, name or ...        *
    ***************************************************************************/

    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pChildNode == NULL || pChildNode != AnscXmlDomNodeGetTailChild(pXmlNode))
    {
        CcspTr069PaTraceError(("Invalid arguments in method 'GetOptions'\n"));

        goto EXIT;
    }

    returnStatus =
    	AnscXmlDomNodeGetDataString
            (
                pChildNode,
                NULL,
                pValueBuf,
                &length
            );

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get the argument value of method 'GetOptions'\n"));

        goto EXIT;
    }

    if( pCcspCwmpMcoIf != NULL)
    {
        return
            pCcspCwmpMcoIf->GetOptions
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pRequestID,
                    pValueBuf
                );
    }

    return returnStatus;

EXIT:

    return
        CcspCwmpSoappoUtilProcessInvalidArgumentRequest
            (
               hCcspCwmpMcoIf,
               pRequestID,
               "GetOptions"
            );

}

/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpSoappoProcessRequest
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                PCHAR                       pNameSpace,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process single SOAP request message.

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                PCHAR                       pNameSpace,
                The soap namespace name

                ANSC_HANDLE                 hXmlHandle
                The SOAP Envelope XML Handle

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessRequest
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        PCHAR                       pNameSpace,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pNodeName    = (PCHAR)NULL;
    CCSP_CWMP_SOAP_REQUEST     ccspCwmpSoapReq;
    PCCSP_CWMP_SOAP_REQUEST    pCcspCwmpSoapReq = &ccspCwmpSoapReq;
    CHAR                            pXmlName[64] = { 0 };
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;

    memset(&ccspCwmpSoapReq, 0, sizeof(CCSP_CWMP_SOAP_REQUEST));
    /*
     *  check the CWMP SOAP header if it's available
     */
    _ansc_sprintf
        (
            pXmlName,
            "%s:%s",
            pNameSpace,
            SOAP_HEADER_NODE
        );

     pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, pXmlName);

     if( pChildNode != NULL)
     {
        returnStatus =
            CcspCwmpSoappoProcessSoapHeader
                (
                    (ANSC_HANDLE)&pCcspCwmpSoapReq->Header,
                    pNameSpace,
                    (ANSC_HANDLE)pChildNode
                );

        if( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CcspTr069PaTraceError(("Failed to process the CWMP SOAP Header.\n"));

            goto EXIT;
        }
     }

     /*
      * Get CWMP BODY XML handle
      */
    _ansc_sprintf
        (
            pXmlName,
            "%s:%s",
            pNameSpace,
            SOAP_BODY_NODE
        );

     pXmlNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, pXmlName);

     if( pXmlNode == NULL)
     {
         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetHeadChild(pXmlNode);

     /* The Body has and only has one child node */
     if( pChildNode == NULL || pChildNode != AnscXmlDomNodeGetTailChild(pXmlNode))
     {
        CcspTr069PaTraceError(("Invalid node counts under SOAP body.\n"));

         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     /*
      * Notify the status first
      */
     if( pCcspCwmpMcoIf != NULL)
     {
        returnStatus =
            pCcspCwmpMcoIf->NotifyAcsStatus
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pCcspCwmpSoapReq->Header.bNoMoreRequests,
                    pCcspCwmpSoapReq->Header.bHoldRequests
                );
     }

     /*
      * Get CWMP Response XML handle
      */
     pNodeName =
        CcspCwmpSoappoUtilGetNodeNameWithoutNS(AnscXmlDomNodeGetName(pChildNode));

     /*
      * Check the method value
      */
     pCcspCwmpSoapReq->Method = CcspCwmpSoappoUtilGetCwmpMethod(pNodeName, FALSE);

     if( pCcspCwmpSoapReq->Header.ID != NULL)
     {
         CcspTr069PaTraceDebug(("Process Request Method: %s (ID = %s)\n", pNodeName, pCcspCwmpSoapReq->Header.ID));
     }

     if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_GetRPCMethods)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * --           | void         | This method has no calling arguments.     *
          ***************************************************************************/
        if( AnscXmlDomNodeGetHeadChild(pChildNode) != NULL)
        {
            returnStatus =
                CcspCwmpSoappoUtilProcessInvalidArgumentRequest
                    (
                       hCcspCwmpMcoIf,
                       pCcspCwmpSoapReq->Header.ID,
                       "GetRPCMethods"
                    );
        }
        else if( pCcspCwmpMcoIf != NULL)
        {
            returnStatus =
                pCcspCwmpMcoIf->GetRpcMethods
                    (
                        pCcspCwmpMcoIf->hOwnerContext,
                        pCcspCwmpSoapReq->Header.ID
                    );
        }
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_SetParameterValues)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * ParameterList|ParameterValue| Array of name-value pairs.                *
          *              |Struct[]      |                                           *
          *-------------------------------------------------------------------------*
          * ParameterKey | string(32)   | The value to set the ParameterKey param...*
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_SetParameterValues
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_GetParameterValues)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          *ParameterNames|string(256)[] | Array of strings,each representing the    *
          *              |              | name of a requested parameter.            *
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_GetParameterValues
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_GetParameterNames)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * ParameterPath|string(256)   | A string of a complete parameter name.    *
          *-------------------------------------------------------------------------*
          * NextLevel    | boolean      | NextLevel only or not                     *
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_GetParameterNames
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_SetParameterAttributes)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * ParameterList|SetParameter  | List of changes to be made.               *
          *              |Struct[]      |                                           *
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_SetParameterAttributes
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_GetParameterAttributes)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * ParameterName|string(256)[] | Array of strings.                         *
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_GetParameterAttributes
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );

     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_AddObject)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * ObjectName   | string(256)  | The path name of the collection of objects*
          *              |              | for which a new instance is to be created.*
          *              |              | The path name MUST end with a "." after   *
          *              |              | the last node in the ...                  *
          *-------------------------------------------------------------------------*
          * ParameterKey | string(32)   | The value to set the ParameterKey param...*
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_AddObject
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );

     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_DeleteObject)
     {

         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * ObjectName   | string(256)  | The path name of the collection of objects*
          *              |              | for which a new instance is to be created.*
          *              |              | The path name MUST end with a "." after   *
          *              |              | the last node in the ...                  *
          *-------------------------------------------------------------------------*
          * ParameterKey | string(32)   | The value to set the ParameterKey param...*
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_DeleteObject
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_Download)
     {
          /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * CommandKey   | string(32)   | a string                                  *
          *-------------------------------------------------------------------------*
          * FileType     | string(64)   | such as "2 Web Content"                  .*
          *-------------------------------------------------------------------------*
          * URL          | string(256)  | dest file location n...                  .*
          *-------------------------------------------------------------------------*
          * Username     | string(256)  | UserName for authenticate                .*
          *-------------------------------------------------------------------------*
          * Password     | string(256)  | Password to authenticate                 .*
          *-------------------------------------------------------------------------*
          * FileSize     | UINT32       | the size of the file to be downloaded    .*
          *-------------------------------------------------------------------------*
          *TargetFileName| string(256)  | target file name                         .*
          *-------------------------------------------------------------------------*
          * DelaySeconds | UINT32       | in seconds                               .*
          *-------------------------------------------------------------------------*
          * SuccURL      | string(256)  | redirect url if succeeds                 .*
          *-------------------------------------------------------------------------*
          * FailureURL   | string(256)  | redirect url if fails                    .*
          ***************************************************************************/
         returnStatus =
             CcspCwmpSoappoProcessRequest_Download
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_Reboot)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * CommandKey   | string(32)   | The string to return in the CommandKey... *
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_Reboot
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );

     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_ChangeDUState)
     {
         /***************************************************************************
          * Argument     | Type              | Description                          *
          ***************************************************************************
          * Operations   | OperationStruct[] | Operations will be performed by CPE  *
          ***************************************************************************
          * CommandKey   | string(32)        | Command Key to identify this RPC     *
          ***************************************************************************/

        returnStatus =
            CcspCwmpSoappoProcessRequest_ChangeDUState
                (
                   hCcspCwmpMcoIf,
                   pCcspCwmpSoapReq->Header.ID,
                   pChildNode
                );
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_GetQueuedTransfers)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * --           | void         | This method has no calling arguments.     *
          ***************************************************************************/

        if( AnscXmlDomNodeGetHeadChild(pChildNode) != NULL)
        {
            returnStatus =
                CcspCwmpSoappoUtilProcessInvalidArgumentRequest
                    (
                       hCcspCwmpMcoIf,
                       pCcspCwmpSoapReq->Header.ID,
                       "GetQueuedTransfers"
                    );
        }
        else if( pCcspCwmpMcoIf != NULL)
        {
            returnStatus =
                pCcspCwmpMcoIf->GetQueuedTransfers
                    (
                        pCcspCwmpMcoIf->hOwnerContext,
                        pCcspCwmpSoapReq->Header.ID
                    );
        }
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_ScheduleInform)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * DelaySeconds | UINT32       | The number of seconds from the time ....  *
          *-------------------------------------------------------------------------*
          * CommandKey   | string(32)   | The string to return...                  .*
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_ScheduleInform
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_SetVouchers)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * VoucherList  | base64[]     | Array of Vouchers                         *
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_SetVouchers
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_GetOptions)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * OptionName   | string(64)   | a string of an option, name or ...        *
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_GetOptions
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );

     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_Upload)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * CommandKey   | string(32)   | a string                                  *
          *-------------------------------------------------------------------------*
          * FileType     | string(64)   | such as "2 Vendor Log File"              .*
          *-------------------------------------------------------------------------*
          * URL          | string(256)  | dest file location n...                  .*
          *-------------------------------------------------------------------------*
          * Username     | string(256)  | UserName for authenticate                .*
          *-------------------------------------------------------------------------*
          * Password     | string(256)  | Password to authenticate                 .*
          *-------------------------------------------------------------------------*
          * DelaySeconds | UINT32       | in seconds                               .*
          ***************************************************************************/

         returnStatus =
             CcspCwmpSoappoProcessRequest_Upload
                (
                    hCcspCwmpMcoIf,
                    pCcspCwmpSoapReq->Header.ID,
                    pChildNode
                );
     }
     else if( pCcspCwmpSoapReq->Method == CCSP_CWMP_METHOD_FactoryReset)
     {
         /***************************************************************************
          * Argument     | Type         | Description                               *
          ***************************************************************************
          * --           | void         | This method has no calling arguments.     *
          ***************************************************************************/

        if( AnscXmlDomNodeGetHeadChild(pChildNode) != NULL)
        {
            returnStatus =
                CcspCwmpSoappoUtilProcessInvalidArgumentRequest
                    (
                       hCcspCwmpMcoIf,
                       pCcspCwmpSoapReq->Header.ID,
                       "FactoryReset"
                    );
        }
        else if( pCcspCwmpMcoIf != NULL)
        {
            returnStatus =
                pCcspCwmpMcoIf->FactoryReset
                    (
                        pCcspCwmpMcoIf->hOwnerContext,
                        pCcspCwmpSoapReq->Header.ID
                    );
        }

     }
     else
     {
        CcspTr069PaTraceWarning(("Unknown method name '%s' in the request.\n", pNodeName));

        returnStatus =
            pCcspCwmpMcoIf->InvokeUnknownMethod
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pCcspCwmpSoapReq->Header.ID,
                    pNodeName
                );
     }

EXIT:

    CcspCwmpCleanSoapReq(pCcspCwmpSoapReq);

    return returnStatus;
}


/**********************************************************************

    prototype:

        ANSC_HANDLE
        CcspCwmpSoappoProcessResponse_GetRPCMethods
            (
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to retrieve the response of GetRPCMethods.

    argument:
                ANSC_HANDLE                 hXmlHandle
                The SOAP Response XML Handle

    return:     the output parameter handle;

**********************************************************************/
ANSC_HANDLE
CcspCwmpSoappoProcessResponse_GetRPCMethods
    (
        ANSC_HANDLE                 hXmlHandle
    )
{
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PSLAP_STRING_ARRAY              pStringArray = (PSLAP_STRING_ARRAY        )NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    CHAR                            pBuffer[64]  = { 0 };
    ULONG                           length       = 64;
    ULONG                           uCount       = 0;
    ULONG                           i            = 0;

    /*
     *  The output should be a MethodList and it's the only child node
     */
    pChildNode = AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pChildNode == NULL || pChildNode != AnscXmlDomNodeGetTailChild(pXmlNode))
    {
        CcspTr069PaTraceError(("Invalid response parameters for method 'GetRPCMethods'\n"));

        return pStringArray;
    }

    pXmlNode   = pChildNode;
    pChildNode = AnscXmlDomNodeGetHeadChild(pXmlNode);

    while( pChildNode != NULL)
    {
        uCount ++;

        pChildNode = AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);
    }

    if( uCount == 0)
    {
        return pStringArray;
    }

     SlapAllocStringArray2(uCount, pStringArray);

    pChildNode = AnscXmlDomNodeGetHeadChild(pXmlNode);

    while( pChildNode != NULL)
    {
        AnscZeroMemory(pBuffer, 64);
        length  = 64;

        returnStatus =
        	AnscXmlDomNodeGetDataString(pChildNode, NULL, pBuffer, &length);


        if( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CcspTr069PaTraceError(("Failed to get item (%lu) of GetRPCMethods response.\n", i));
        }
        else if( length > 0)
        {
            pStringArray->Array.arrayString[i] = CcspTr069PaCloneString(pBuffer);
        }

        i ++;

        /* next one */
        pChildNode = AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);
    }

    return pStringArray;
}

/**********************************************************************

    prototype:

        ULONG
        CcspCwmpSoappoProcessResponse_Inform
            (
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to retrieve the response of Inform

    argument:
                ANSC_HANDLE                 hXmlHandle
                The SOAP Response XML Handle

    return:     the output parameter ULONG value;

**********************************************************************/
ULONG
CcspCwmpSoappoProcessResponse_Inform
    (
        ANSC_HANDLE                 hXmlHandle
    )
{
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    ULONG                           uLongValue   = 0;

    pChildNode = AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pChildNode == NULL || pChildNode != AnscXmlDomNodeGetTailChild(pXmlNode))
    {
        CcspTr069PaTraceError(("The reponse of 'Inform' should have only one output parameter.\n"));

        return 0xFFFFFFFF;
    }

    returnStatus =
    	AnscXmlDomNodeGetDataUlong(pChildNode, NULL, &uLongValue);

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get output of Inform response.\n"));

        return 0xFFFFFFFF;
    }

    return uLongValue;
}

/**********************************************************************

    prototype:

        PCHAR
        CcspCwmpSoappoProcessResponse_Kicked
            (
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to retrieve the response of GetKicked

    argument:
                ANSC_HANDLE                 hXmlHandle
                The SOAP Response XML Handle

    return:     the output parameter string value;

**********************************************************************/
PCHAR
CcspCwmpSoappoProcessResponse_Kicked
    (
        ANSC_HANDLE                 hXmlHandle
    )
{
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    CHAR                            pBuffer[1024]= { 0 };
    ULONG                           length       = 1024;

    pChildNode = AnscXmlDomNodeGetHeadChild(pXmlNode);

    if( pChildNode == NULL || pChildNode != AnscXmlDomNodeGetTailChild(pXmlNode))
    {
        CcspTr069PaTraceError(("The reponse of 'Kicked' should have only one output parameter.\n"));

        return NULL;
    }

    returnStatus =
    	AnscXmlDomNodeGetDataString(pChildNode, NULL, pBuffer, &length);

    if( returnStatus != ANSC_STATUS_SUCCESS)
    {
        CcspTr069PaTraceError(("Failed to get output of Kicked response.\n"));

        return NULL;
    }
    else if( length > 0)
    {
        return CcspTr069PaCloneString(pBuffer);
    }
    else
    {
        return CcspTr069PaCloneString("");
    }

    return NULL;
}

/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpSoappoProcessResponse
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                PCHAR                       pNameSpace,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process single SOAP response message.

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                PCHAR                       pNameSpace,
                The soap namespace name;

                ANSC_HANDLE                 hXmlHandle
                The SOAP Envelope XML Handle

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessResponse
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        PCHAR                       pNameSpace,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf   = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode     = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode   = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pNodeName    = (PCHAR)NULL;
    CHAR                            pXmlName[64] = { 0 };
    PCCSP_CWMP_SOAP_RESPONSE        pCcspCwmpSoapRep = (PCCSP_CWMP_SOAP_RESPONSE)NULL;
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;

    pCcspCwmpSoapRep = (PCCSP_CWMP_SOAP_RESPONSE)
        CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_RESPONSE));

    if( pCcspCwmpSoapRep == NULL)
    {
        return ANSC_STATUS_RESOURCES;
    }

    /*
     *  check the CWMP SOAP header if it's available
     */
    _ansc_sprintf
        (
            pXmlName,
            "%s:%s",
            pNameSpace,
            SOAP_HEADER_NODE
        );

     pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, pXmlName);

     if( pChildNode != NULL)
     {
        returnStatus =
            CcspCwmpSoappoProcessSoapHeader
                (
                    (ANSC_HANDLE)&pCcspCwmpSoapRep->Header,
                    pNameSpace,
                    (ANSC_HANDLE)pChildNode
                );

        if( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CcspTr069PaTraceError(("Failed to process the CWMP SOAP Header.\n"));

            goto EXIT;
        }
     }

     /*
      * Get CWMP BODY XML handle
      */
    _ansc_sprintf
        (
            pXmlName,
            "%s:%s",
            pNameSpace,
            SOAP_BODY_NODE
        );

     pXmlNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, pXmlName);

     if( pXmlNode == NULL)
     {
         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetHeadChild(pXmlNode);

     /* The Body has and only has one child node */
     if( pChildNode == NULL || pChildNode != AnscXmlDomNodeGetTailChild(pXmlNode))
     {
        CcspTr069PaTraceError(("Invalid node counts under SOAP body.\n"));

         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     /*
      * Get CWMP Response XML handle
      */
     pNodeName =
        CcspCwmpSoappoUtilGetNodeNameWithoutNS(AnscXmlDomNodeGetName(pChildNode));

     AnscZeroMemory(pXmlName, 64);
     AnscCopyMemory(pXmlName, pNodeName, AnscSizeOfString(pNodeName) - AnscSizeOfString(PART_RESPONSE_NAME));

     /*
      * Check the method value
      */
     pCcspCwmpSoapRep->Method = CcspCwmpSoappoUtilGetCwmpMethod(pXmlName, TRUE);

     if( pCcspCwmpSoapRep->Method == 0)
     {
        CcspTr069PaTraceError(("Unknown method response name: %s\n", pNodeName));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT;

     }
     else
     {
        CcspTr069PaTraceDebug(("Process Response Method: %s\n", pXmlName));
     }

    /*
     * Only 5 methods are supported by the server, they are:
     *
     * GetRPCMethods
     * Inform
     * TransferComplete
     * RequestDownload
     * Kicked
     */
     if( pCcspCwmpSoapRep->Method == CCSP_CWMP_METHOD_GetRPCMethods)
     {
        pCcspCwmpSoapRep->hRepArguments =
            CcspCwmpSoappoProcessResponse_GetRPCMethods
                (
                    pChildNode
                );

        if( pCcspCwmpSoapRep->hRepArguments == NULL)
        {
            returnStatus = ANSC_STATUS_FAILURE;

            goto EXIT;
        }
     }
     else if( pCcspCwmpSoapRep->Method == CCSP_CWMP_METHOD_Inform)
     {
        pCcspCwmpSoapRep->hRepArguments = (ANSC_HANDLE)
            CcspCwmpSoappoProcessResponse_Inform
                (
                    pChildNode
                );

        if( pCcspCwmpSoapRep->hRepArguments == (ANSC_HANDLE)0xFFFFFFFF)
        {
            returnStatus = ANSC_STATUS_FAILURE;

            goto EXIT;
        }

     }
     else if( pCcspCwmpSoapRep->Method == CCSP_CWMP_METHOD_Kicked)
     {
        pCcspCwmpSoapRep->hRepArguments = (ANSC_HANDLE)
            CcspCwmpSoappoProcessResponse_Kicked
                (
                    pChildNode
                );

        if( pCcspCwmpSoapRep->hRepArguments == NULL)
        {
            returnStatus = ANSC_STATUS_FAILURE;

            goto EXIT;
        }
     }
     else
     {
        /* we don't have any output parameter for other methods */
     }

    /*
     * Ask pCcspCwmpMcoIf to process the response
     */
     if( pCcspCwmpMcoIf != NULL)
     {
        returnStatus =
            pCcspCwmpMcoIf->NotifyAcsStatus
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pCcspCwmpSoapRep->Header.bNoMoreRequests,
                    pCcspCwmpSoapRep->Header.bHoldRequests
                );

        returnStatus =
            pCcspCwmpMcoIf->ProcessSoapResponse
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    (ANSC_HANDLE)pCcspCwmpSoapRep
                );

        return returnStatus;
     }

EXIT:

    if( pCcspCwmpSoapRep )
    {
        CcspCwmpFreeSoapResponse(pCcspCwmpSoapRep);
        pCcspCwmpSoapRep = NULL;
    }

    return returnStatus;
}

/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpSoappoProcessFault
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                PCHAR                       pNameSpace,
                ANSC_HANDLE                 hXmlHandle
            );

    description:

        This function is called to process single SOAP Fault response.

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                PCHAR                       pNameSpace,
                The soap namespace name

                ANSC_HANDLE                 hXmlHandle
                The SOAP Envelope XML Handle

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessFault
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        PCHAR                       pNameSpace,
        ANSC_HANDLE                 hXmlHandle
    )
{
    PCCSP_CWMP_MCO_INTERFACE        pCcspCwmpMcoIf      = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pXmlNode        = (PANSC_XML_DOM_NODE_OBJECT)hXmlHandle;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode      = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pTempNode       = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpFault      = (PCCSP_CWMP_SOAP_FAULT)NULL;
    PCCSP_CWMP_SET_PARAM_FAULT      pCwmpParamFault = (PCCSP_CWMP_SET_PARAM_FAULT)NULL;
    PCHAR                           pNodeName       = NULL;
    CHAR                            pXmlName[64]    = { 0 };
    PCCSP_CWMP_SOAP_RESPONSE        pCcspCwmpSoapRep    = (PCCSP_CWMP_SOAP_RESPONSE)NULL;
    CHAR                            pValueBuf[128]  = { 0 };
    ULONG                           ulValueSize     = 128;
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;

    pCcspCwmpSoapRep = (PCCSP_CWMP_SOAP_RESPONSE)
        CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_RESPONSE));

    if( pCcspCwmpSoapRep == NULL)
    {
        return ANSC_STATUS_RESOURCES;
    }

    /*
     *  check the CWMP SOAP header if it's available
     */
    _ansc_sprintf
        (
            pXmlName,
            "%s:%s",
            pNameSpace,
            SOAP_HEADER_NODE
        );

     pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, pXmlName);

     if( pChildNode != NULL)
     {
        returnStatus =
            CcspCwmpSoappoProcessSoapHeader
                (
                    (ANSC_HANDLE)&pCcspCwmpSoapRep->Header,
                    pNameSpace,
                    (ANSC_HANDLE)pChildNode
                );

        if( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CcspTr069PaTraceError(("Failed to process the CWMP SOAP Header.\n"));

            goto EXIT;
        }
     }

     /*
      * Get CWMP BODY XML handle
      */
    _ansc_sprintf
        (
            pXmlName,
            "%s:%s",
            pNameSpace,
            SOAP_BODY_NODE
        );

     pXmlNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, pXmlName);

     if( pXmlNode == NULL)
     {
         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     /*
      * Get CWMP SOAP FAULT XML handle
      */
    _ansc_sprintf
        (
            pXmlName,
            "%s:%s",
            pNameSpace,
            SOAP_FAULT
        );

     pXmlNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, pXmlName);

     if( pXmlNode == NULL)
     {
         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     /*
      * Process the Fault message
      */
     pCwmpFault = (PCCSP_CWMP_SOAP_FAULT)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_SOAP_FAULT));

     if( pCwmpFault == NULL)
     {
         returnStatus = ANSC_STATUS_RESOURCES;

         goto EXIT;
     }

     pCcspCwmpSoapRep->Fault = (ANSC_HANDLE)pCwmpFault;

     /* Get "faultcode" value */
     pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, SOAP_FAULTCODE);

     if( pChildNode == NULL)
     {
         CcspTr069PaTraceError(("Failed to find node '%s' in SOAP FAULT message.\n", SOAP_FAULTCODE));

         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     ulValueSize = 128;
     AnscZeroMemory(pValueBuf, ulValueSize);

     returnStatus =
    	 AnscXmlDomNodeGetDataString(pChildNode, NULL, pValueBuf, &ulValueSize);

     if( returnStatus != ANSC_STATUS_SUCCESS)
     {
        CcspTr069PaTraceError(("Failed to get value of node '%s'\n", SOAP_FAULTCODE));

        goto EXIT;
     }

    pCwmpFault->soap_faultcode = CcspTr069PaCloneString(pValueBuf);

     /* Get "faultstring" value */
     pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, SOAP_FAULTSTRING);

     if( pChildNode == NULL)
     {
         CcspTr069PaTraceError(("Failed to find node '%s' in SOAP FAULT message.\n", SOAP_FAULTSTRING));

         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     ulValueSize = 128;
     AnscZeroMemory(pValueBuf, ulValueSize);

     returnStatus =
    	 AnscXmlDomNodeGetDataString(pChildNode, NULL, pValueBuf, &ulValueSize);

     if( returnStatus != ANSC_STATUS_SUCCESS)
     {
        CcspTr069PaTraceError(("Failed to get value of node '%s'\n", SOAP_FAULTSTRING));

        goto EXIT;
     }

     pCwmpFault->soap_faultstring = CcspTr069PaCloneString(pValueBuf);

     /* Get the "detail" information */
     pXmlNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, SOAP_DETAIL);

     if( pXmlNode == NULL)
     {
         CcspTr069PaTraceError(("Failed to find node '%s' in SOAP FAULT message.\n", SOAP_DETAIL));

         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     /* Get the "cwmp:Fault" node */
     pXmlNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetHeadChild(pXmlNode);

     if( pXmlNode == NULL)
     {
         CcspTr069PaTraceError(("Failed to find node '%s' in SOAP FAULT message.\n", CCSP_CWMP_FAULT_NODE));

         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     pNodeName  =
         CcspCwmpSoappoUtilGetNodeNameWithoutNS
            (
            	AnscXmlDomNodeGetName(pXmlNode)
            );

     if( !AnscEqualString(pNodeName, CCSP_CWMP_FAULT_NODE, TRUE))
     {
        CcspTr069PaTraceError(("Unknown node name '%s' in CWMP Fault message.\n", pNodeName));

         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

    /* get "FaultCode" value */
     pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, CCSP_CWMP_FAULTCODE);

     if( pChildNode == NULL)
     {
         CcspTr069PaTraceError(("Failed to find node '%s' in CWMP FAULT message.\n", CCSP_CWMP_FAULTCODE));

         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     returnStatus =
    	 AnscXmlDomNodeGetDataUlong
            (
                pChildNode,
                NULL,
                &pCwmpFault->Fault.FaultCode
            );

     if( returnStatus != ANSC_STATUS_SUCCESS)
     {
         CcspTr069PaTraceError(("Failed to get CWMP FaultCode value.\n"));

         goto EXIT;
     }

    /* get "FaultString" value */
     pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		 AnscXmlDomNodeGetChildByName(pXmlNode, CCSP_CWMP_FAULTSTRING);

     if( pChildNode == NULL)
     {
         CcspTr069PaTraceError(("Failed to find node '%s' in CWMP FAULT message.\n", CCSP_CWMP_FAULTSTRING));

         returnStatus = ANSC_STATUS_FAILURE;

         goto EXIT;
     }

     ulValueSize = 128;
     AnscZeroMemory(pValueBuf, ulValueSize);

     returnStatus =
    	 AnscXmlDomNodeGetDataString(pChildNode, NULL, pValueBuf, &ulValueSize);

     if( returnStatus != ANSC_STATUS_SUCCESS)
     {
        CcspTr069PaTraceError(("Failed to get value of node '%s'\n", CCSP_CWMP_FAULTSTRING));

        goto EXIT;
     }

    pCwmpFault->Fault.FaultString = CcspTr069PaCloneString(pValueBuf);

    /* check "SetParameterValuesFault */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);

    pCwmpFault->SetParamValuesFaultCount = 0;

    while( pChildNode != NULL)
    {
         pNodeName  =
             CcspCwmpSoappoUtilGetNodeNameWithoutNS
                (
                	AnscXmlDomNodeGetName(pChildNode)
                );

         if( AnscEqualString(pNodeName, CCSP_CWMP_SETFAULT, TRUE))
         {
            pCwmpParamFault = &pCwmpFault->SetParamValuesFaultArray[pCwmpFault->SetParamValuesFaultCount++];

            /* get "ParameterName" value */
             pTempNode = (PANSC_XML_DOM_NODE_OBJECT)
				 AnscXmlDomNodeGetChildByName(pChildNode, CCSP_CWMP_PARAMETERNAME);

             if( pTempNode == NULL)
             {
                 CcspTr069PaTraceError(("Failed to find node '%s' in CWMP FAULT message.\n", CCSP_CWMP_PARAMETERNAME));

                 returnStatus = ANSC_STATUS_FAILURE;

                 goto EXIT;
             }

             ulValueSize = 128;
             AnscZeroMemory(pValueBuf, ulValueSize);

             returnStatus =
            	 AnscXmlDomNodeGetDataString(pTempNode, NULL, pValueBuf, &ulValueSize);

             if( returnStatus != ANSC_STATUS_SUCCESS)
             {
                CcspTr069PaTraceError(("Failed to get value of node '%s'\n", CCSP_CWMP_PARAMETERNAME));

                goto EXIT;
             }

            pCwmpParamFault->ParameterName = CcspTr069PaCloneString(pValueBuf);

            /* get "FaultCode" value */
             pTempNode = (PANSC_XML_DOM_NODE_OBJECT)
				 AnscXmlDomNodeGetChildByName(pChildNode, CCSP_CWMP_FAULTCODE);

             if( pTempNode == NULL)
             {
                 CcspTr069PaTraceError(("Failed to find node '%s' in CWMP FAULT message.\n", CCSP_CWMP_FAULTCODE));

                 returnStatus = ANSC_STATUS_FAILURE;

                 goto EXIT;
             }

             returnStatus =
            	 AnscXmlDomNodeGetDataUlong
                    (
                        pTempNode,
                        NULL,
                        &pCwmpParamFault->FaultCode
                    );

             if( returnStatus != ANSC_STATUS_SUCCESS)
             {
                CcspTr069PaTraceError(("Failed to get value of node '%s'\n", CCSP_CWMP_FAULTCODE));

                goto EXIT;
             }

            /* get "FaultString" value */
             pTempNode = (PANSC_XML_DOM_NODE_OBJECT)
				 AnscXmlDomNodeGetChildByName(pChildNode, CCSP_CWMP_FAULTSTRING);

             if( pTempNode == NULL)
             {
                 CcspTr069PaTraceError(("Failed to find node '%s' in CWMP FAULT message.\n", CCSP_CWMP_FAULTSTRING));

                 returnStatus = ANSC_STATUS_FAILURE;

                 goto EXIT;
             }

             ulValueSize = 128;
             AnscZeroMemory(pValueBuf, ulValueSize);

             returnStatus =
            	 AnscXmlDomNodeGetDataString(pTempNode, NULL, pValueBuf, &ulValueSize);

             if( returnStatus != ANSC_STATUS_SUCCESS)
             {
                CcspTr069PaTraceError(("Failed to get value of node '%s'\n", CCSP_CWMP_FAULTSTRING));

                goto EXIT;
             }

            pCwmpParamFault->FaultString = CcspTr069PaCloneString(pValueBuf);

         }
         else
         {
            CcspTr069PaTraceWarning(("Unknown node name '%s' in CWMP Fault message.\n", pNodeName));
         }

        /* try next one */
        pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
			AnscXmlDomNodeGetNextChild(pXmlNode, pChildNode);
    }

    /*
     * After parsing the SOAP FAULT message, ask hCcspCwmpMcoIf to handle the response
     */
     if( pCcspCwmpMcoIf != NULL)
     {
        CcspTr069PaTraceWarning
            ((
                "Process Fault Response:\r\n(FaultCode=%lu)%s\r\n",
                pCcspCwmpSoapRep->Fault->Fault.FaultCode,
                pCcspCwmpSoapRep->Fault->Fault.FaultString
            ));

        returnStatus =
            pCcspCwmpMcoIf->NotifyAcsStatus
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    pCcspCwmpSoapRep->Header.bNoMoreRequests,
                    pCcspCwmpSoapRep->Header.bHoldRequests
                );

        returnStatus =
            pCcspCwmpMcoIf->ProcessSoapResponse
                (
                    pCcspCwmpMcoIf->hOwnerContext,
                    (ANSC_HANDLE)pCcspCwmpSoapRep
                );

        return returnStatus;
     }

EXIT:

    if( pCcspCwmpSoapRep )
    {
        CcspCwmpFreeSoapResponse(pCcspCwmpSoapRep);
        pCcspCwmpSoapRep = NULL;
    }

    return returnStatus;
}

/**********************************************************************

    prototype:

        ANSC_STATUS
        CcspCwmpSoappoProcessSingleEnvelope
            (
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                PCHAR                       pNameSpace,
                PCHAR                       pEnvelopeMessage,
                ULONG                       uMsgSize
            );

    description:

        This function is called to process single SOAP Envelope message.

    argument:
                ANSC_HANDLE                 hCcspCwmpMcoIf,
                The CcspCwmpMcoIf handle;

                PCHAR                       pNameSpace,
                The namespace name such as "soap"

                PCHAR                       pEnvelopeMessage,
                The input Soap envelope message;

                ULONG                       uMsgSize,
                The length of this message;

                PBOOLEAN                    pIsFault
                The output notify it's a fault message or not;

    return:     the status of the operation;

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessSingleEnvelope
    (
        ANSC_HANDLE                 hCcspCwmpMcoIf,
        PCHAR                       pNameSpace,
        PCHAR                       pEnvelopeMessage,
        ULONG                       uMsgSize,
        PBOOLEAN                    pIsFault
    )
{
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_MCO_INTERFACE             pCcspCwmpMcoIf      = (PCCSP_CWMP_MCO_INTERFACE)hCcspCwmpMcoIf;
    PANSC_XML_DOM_NODE_OBJECT       pRootNode       = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pBodyNode       = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PANSC_XML_DOM_NODE_OBJECT       pChildNode      = (PANSC_XML_DOM_NODE_OBJECT)NULL;
    PCHAR                           pBackBuffer     = pEnvelopeMessage;
    PCHAR                           pNodeName       = (PCHAR)NULL;
    PCHAR                           pTempName       = (PCHAR)NULL;
    CHAR                            pValueBuf[256]  = { 0 };
    ULONG                           ulValueSize     = 256;
    CHAR                            pNameBuf[32]    = { 0 };

    pRootNode = (PANSC_XML_DOM_NODE_OBJECT)
        AnscXmlDomParseString((ANSC_HANDLE)NULL, (PCHAR*)&pBackBuffer, uMsgSize);

    if( pRootNode == NULL)
    {
        CcspTr069PaTraceError(("Failed to parse the SOAP Envelope message.\n"));

        return ANSC_STATUS_FAILURE;
    }

    /* check the root node name */
    _ansc_sprintf
        (
            pNameBuf,
            "%s:%s",
            pNameSpace,
            SOAP_ENVELOPE
        );

    pNodeName = AnscXmlDomNodeGetName(pRootNode);

    if( !AnscEqualString(pNodeName, pNameBuf, TRUE))
    {
        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT;
    }

    /* check the xml ns value */
    _ansc_sprintf
        (
            pNameBuf,
            "%s:%s",
            SOAP_NAMESPACE_NAME,
            pNameSpace
        );

    returnStatus =
    	AnscXmlDomNodeGetAttrString(pRootNode, pNameBuf, pValueBuf, &ulValueSize);

    if( returnStatus != ANSC_STATUS_SUCCESS ||
        !AnscEqualString(pValueBuf, DEFAULT_SOAP_NAMESPACE, TRUE))
    {
        CcspTr069PaTraceError(("Invalid Envelope name space url .\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT;
    }

#if 0
    /* check the xml encoding style */
    _ansc_sprintf
        (
            pNameBuf,
            "%s:%s",
            pNameSpace,
            NODE_SOAP_ENCODING_STYLE
        );

    ulValueSize = 256;
    AnscZeroMemory(pValueBuf, ulValueSize);

    returnStatus =
    	AnscXmlDomNodeGetAttrString(pRootNode, pNameBuf, pValueBuf, &ulValueSize);

    if( returnStatus != ANSC_STATUS_SUCCESS ||
        !AnscEqualString(pValueBuf, DEFAULT_SOAP_ENCODING_STYLE, TRUE))
    {
        CcspTr069PaTraceError
            ((
                "Invalid SOAP Encoding Style.\n"
            ));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT;
    }
#endif

    /* Find SOAP Body */
    _ansc_sprintf
        (
            pNameBuf,
            "%s:%s",
            pNameSpace,
            SOAP_BODY_NODE
        );

    pBodyNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetChildByName(pRootNode, pNameBuf);

    if( pBodyNode == NULL)
    {
        CcspTr069PaTraceDebug(("Empty Envelope Body, no request/reponse at all.\n"));

        if(pCcspCwmpMcoIf != NULL)
        {
            returnStatus =
                pCcspCwmpMcoIf->NotifyAcsStatus
                    (
                        pCcspCwmpMcoIf->hOwnerContext,
                        TRUE,           /* no more requests */
                        FALSE
                    );

        }

        goto EXIT;
    }

    /* check it's a Request, Response or FAULT message */
    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)
		AnscXmlDomNodeGetHeadChild(pBodyNode);

    if( pChildNode == NULL)
    {
        CcspTr069PaTraceError(("Empty CWMP SOAP Body message.\n"));

        returnStatus = ANSC_STATUS_FAILURE;

        goto EXIT;
    }

    pNodeName =
        CcspCwmpSoappoUtilGetNodeNameWithoutNS(AnscXmlDomNodeGetName(pChildNode));

    if( AnscEqualString(pNodeName, SOAP_FAULT, TRUE))
    {
        returnStatus =
            CcspCwmpSoappoProcessFault
                (
                    hCcspCwmpMcoIf,
                    pNameSpace,
                    pRootNode
                );

        if( pIsFault != NULL)
        {
            *pIsFault = TRUE;
        }
    }
    else
    {
        if( pIsFault != NULL)
        {
            *pIsFault = FALSE;
        }

        /*
         * If the name ends with "Response", it's a response.
         */
        pTempName = _ansc_strstr(pNodeName, PART_RESPONSE_NAME);

        if( pTempName != NULL &&
            (ULONG)(pTempName - pNodeName) + AnscSizeOfString(PART_RESPONSE_NAME) == AnscSizeOfString(pNodeName))
        {
            returnStatus =
                CcspCwmpSoappoProcessResponse
                    (
                        hCcspCwmpMcoIf,
                        pNameSpace,
                        pRootNode
                    );
        }
        else
        {
            returnStatus =
                CcspCwmpSoappoProcessRequest
                    (
                        hCcspCwmpMcoIf,
                        pNameSpace,
                        pRootNode
                    );
        }
    }

EXIT:

    if( pRootNode != NULL)
    {
    	AnscXmlDomNodeRemove(pRootNode);
    }

    return returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpSoappoProcessSoapEnvelopes
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pSoapEnvelopes,
                ULONG                       uMaxEnvelope,
                ANSC_HANDLE                 hCcspCwmpMcoIf
            );

    description:

        This function is called to process SOAP envelope messages.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pSoapEnvelopes
                The content of the SoapEnvelope messages. It may contain
                multiple messages.

                ULONG                       uMaxEnvelope,
                Maxi Envelope can support;

                ANSC_HANDLE                 hCcspCwmpMcoIf
                The CcspCwmpMco Interface Handle;

    return:     status of operation.

**********************************************************************/
ANSC_STATUS
CcspCwmpSoappoProcessSoapEnvelopes
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pSoapEnvelopes,
        ULONG                       uMaxEnvelope,
        ANSC_HANDLE                 hCcspCwmpMcoIf
   )
{
    UNREFERENCED_PARAMETER(hThisObject);
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCHAR                           pMsgBegin    = NULL;
    CHAR                            pEnveNode[32]= { 0 };
    CHAR                            pNSpace[16]  = { 0 };
    ULONG                           pPosition[32]= { 0 };
    ULONG                           uCount       = 0;
    ULONG                           i            = 0;
    PCHAR                           pStart       = pSoapEnvelopes;
    BOOLEAN                         bIsFault     = FALSE;

    if( pSoapEnvelopes == NULL || AnscSizeOfString(pSoapEnvelopes) <= 10 )
    {
        CcspTr069PaTraceError(("CcspCwmpSoapParser is not inited, failed to process soap messages.\n"));

        return ANSC_STATUS_FAILURE;
    }

    /* get rid of the xml header if have */
    if( _ansc_strstr(pSoapEnvelopes, "<?xml") == pSoapEnvelopes)
    {
        pStart = pSoapEnvelopes + 4;
        pStart = _ansc_strstr(pStart, "<");

        if( pStart == NULL)
        {
            return ANSC_STATUS_FAILURE;
        }

        while( _ansc_strstr(pStart, "<!--") == pStart) /* comments */
        {
            pStart = _ansc_strstr(pStart, "-->");

            if( pStart == NULL)
            {
                return ANSC_STATUS_FAILURE;
            }

            pStart += 4;
            pStart = _ansc_strstr(pStart, "<");

            if( pStart == NULL)
            {
                return ANSC_STATUS_FAILURE;
            }
        }
    }


    /* check the soap namespace name */
    if( ANSC_STATUS_SUCCESS !=
         CcspCwmpSoappoUtilGetSoapNamespace(pStart, pNSpace))
    {

        CcspTr069PaTraceError(("CcspCwmpSoapParser failed to get the SOAP namespace value.\n"));

        return ANSC_STATUS_FAILURE;
    }

    /* check how many Envelopes in the message first */
    _ansc_sprintf
        (
            pEnveNode,
            "<%s:%s",
            pNSpace,
            SOAP_ENVELOPE
        );

    pMsgBegin = _ansc_strstr(pSoapEnvelopes, pEnveNode);

    while( pMsgBegin != NULL )
    {
        pPosition[uCount++] = (ULONG)(pMsgBegin - pSoapEnvelopes);

        pMsgBegin += AnscSizeOfString(pEnveNode);
        pMsgBegin = _ansc_strstr(pMsgBegin, pEnveNode);

        if( uCount > uMaxEnvelope)
        {
            CcspTr069PaTraceError(("Too many SOAP Envelopes.\n"));

            CcspCwmpSoappoUtilProcessTooManyEnvelopes(hCcspCwmpMcoIf);

            return ANSC_STATUS_FAILURE;
        }
    }

    pPosition[uCount] = AnscSizeOfString(pSoapEnvelopes);

    if( uCount == 0)
    {
        return ANSC_STATUS_SUCCESS;
    }

	/*
	 * According to WT151, there's only one envelope is allowed
	 * in the message.
	 */
    CcspTr069PaTraceDebug(("There're %lu SOAP Envelope messages.\n", uCount));

	if( uCount > 1)
	{
        CcspTr069PaTraceError(("Too many SOAP Envelopes.\n"));

        CcspCwmpSoappoUtilProcessTooManyEnvelopes(hCcspCwmpMcoIf);

        return ANSC_STATUS_FAILURE;
	}


    /*
     * Process the SOAP Envelope Messages one by one.
     */

    for( i = 0; i < uCount; i ++)
    {
        pMsgBegin = pSoapEnvelopes + pPosition[i];

        bIsFault = FALSE;

        returnStatus =
            CcspCwmpSoappoProcessSingleEnvelope
                (
                    hCcspCwmpMcoIf,
                    pNSpace,
                    pMsgBegin,
                    pPosition[i + 1] - pPosition[i],
                    &bIsFault
                );

        if( bIsFault)
        {
            CcspTr069PaTraceDebug(("Envelope #%lu is a fault message.\n", (i+1)));

            /* return ANSC_STATUS_FAILURE; */
        }

        if( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CcspTr069PaTraceError(("Failed to process Envelope number %lu\n", (i+1)));

            return ANSC_STATUS_FAILURE;
        }
    }

    return ANSC_STATUS_SUCCESS;
}

