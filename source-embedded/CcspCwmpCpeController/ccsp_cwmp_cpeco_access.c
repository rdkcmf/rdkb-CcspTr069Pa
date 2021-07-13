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

    module: ccsp_cwmp_cpeco_access.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced state-access functions
        of the CCSP CWMP Cpe Controller Object.

        *   CcspCwmpCpecoGetCcspCwmpMcoIf
        *   CcspCwmpCpecoGetCcspCwmpMsoIf
        *   CcspCwmpCpecoGetParamValues
        *   CcspCwmpCpecoSetParamValues
        *   CcspCwmpCpecoSetPAName
        *   CcspCwmpCpecoGetPAName
        *   CcspCwmpCpecoSetSubsysName
        *   CcspCwmpCpecoGetSubsysName
        *   CcspCwmpCpecoSetCRName
        *   CcspCwmpCpecoGetCRName
        *   CcspCwmpCpecoSetCRBusPath
        *   CcspCwmpCpecoGetCRBusPath
        *   CcspCwmpCpecoSetPAMapperFile
        *   CcspCwmpCpecoSetSDMXmlFilename
        *   CcspCwmpCpecoGetSDMXmlFilename
        *   CcspCwmpCpecoSetOutboundIfName
        *   CcspCwmpCpecoGetOutboundIfName
        *   CcspCwmpCpecoGetMsgBusHandle
        *   CcspCwmpCpecoGetParamNotification
        *   CcspCwmpCpecoSetParamNotification
        *   CcspCwmpCpecoGetParamDataType
        *   CcspCwmpCpecoSaveCfgToPsm
        *   CcspCwmpCpecoLoadCfgFromPsm
        *   CcspCwmpCpecoSetParameterKey

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
        06/20/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_cpeco_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoGetCcspCwmpMcoIf
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
CcspCwmpCpecoGetCcspCwmpMcoIf
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject         = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession   = (PCCSP_CWMP_SESSION_OBJECT     )pCcspCwmpProcessor->AcqWmpSession((ANSC_HANDLE)pCcspCwmpProcessor);
    PCCSP_CWMP_MCO_INTERFACE             pCcspCwmpMcoIf        = (PCCSP_CWMP_MCO_INTERFACE          )NULL;

    if ( !pCcspCwmpSession )
    {
        return  (ANSC_HANDLE)NULL;
    }
    else
    {
        pCcspCwmpMcoIf = (PCCSP_CWMP_MCO_INTERFACE)pCcspCwmpSession->GetCcspCwmpMcoIf((ANSC_HANDLE)pCcspCwmpSession);
    }

    pCcspCwmpSession->RelAccess((ANSC_HANDLE)pCcspCwmpSession);

    return  (ANSC_HANDLE)pCcspCwmpMcoIf;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoGetCcspCwmpMsoIf
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
CcspCwmpCpecoGetCcspCwmpMsoIf
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_ACS_BROKER_OBJECT         pCcspCwmpAcsBroker = (PCCSP_CWMP_ACS_BROKER_OBJECT      )pMyObject->hCcspCwmpAcsBroker;

    return  pCcspCwmpAcsBroker->hCcspCwmpMsoIf;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoGetParamValues
            (
                ANSC_HANDLE                 hThisObject,
                char**                      ppParamNames,
                int                         NumOfParams,
                PCCSP_VARIABLE              pParamValues
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char**                      ppParamNames,
                Specifies the parameter names.

                int                         NumOfParams
                Specifies the number of parameters.

                PCCSP_VARIABLE              pParamValues
                On successful return, it contains the values of
                the specified parameters.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoGetParamValues
    (
        ANSC_HANDLE                 hThisObject,
        char**                      ppParamNames,
        int                         NumOfParams,
        PCCSP_VARIABLE              pParamValues
    )
{
    UNREFERENCED_PARAMETER(ppParamNames);
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf        = (PCCSP_CWMP_MPA_INTERFACE          )pCcspCwmpProcessor->GetCcspCwmpMpaIf((ANSC_HANDLE)pCcspCwmpProcessor);
    SLAP_STRING_ARRAY*              pSlapNameArray    = NULL;
    PCCSP_CWMP_PARAM_VALUE          pCwmpValArray     = NULL;
    ULONG                           ulCwmpValArraySize= 0;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault    = (PCCSP_CWMP_SOAP_FAULT        )NULL;
    ULONG                           i;

    /* 
     * Notes: this API only takes full parameter names, no validation
     * here since it's internal API
     */

    returnStatus = 
        pCcspCwmpMpaIf->GetParameterValues
            (
                pCcspCwmpMpaIf->hOwnerContext,
                pSlapNameArray,
                NumOfParams,
                (void**)&pCwmpValArray,
                &ulCwmpValArraySize,
                (ANSC_HANDLE*)&pCwmpSoapFault,
                FALSE
            );

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        if ( ulCwmpValArraySize != (ULONG)NumOfParams )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
        }
        else
        {
            for ( i = 0; i < (ULONG)NumOfParams; i ++ )
            {
                SlapCloneVariable(pCwmpValArray[i].Value, pParamValues+i);
            }
        }
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

    if ( pCwmpValArray )
    {
        for ( i = 0; i < ulCwmpValArraySize; i++ )
        {
            CcspCwmpCleanParamValue((&pCwmpValArray[i]));
        }

        CcspTr069PaFreeMemory(pCwmpValArray);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoGetParamStringValues
            (
                ANSC_HANDLE                 hThisObject,
                char**                      ppParamNames,
                int                         NumOfParams,
                char**                      pParamValues
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself. 

                char**                      ppParamNames,
                Specifies the parameter names. This API only
                takes full parameter names. If partial name is 
                used, call GPV implemented by CCSP CWMP MPA interface.

                int                         NumOfParams
                Specifies the number of parameters.

                char**                      pParamValues
                On successful return, it contains the values of
                the specified parameters.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoGetParamStringValues
    (
        ANSC_HANDLE                 hThisObject,
        char**                      ppParamNames,
        int                         NumOfParams,
        char**                      pParamValues
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject         = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf        = (PCCSP_CWMP_MPA_INTERFACE          )pCcspCwmpProcessor->GetCcspCwmpMpaIf((ANSC_HANDLE)pCcspCwmpProcessor);
    SLAP_STRING_ARRAY*              pSlapNameArray    = NULL;
    PCCSP_CWMP_PARAM_VALUE          pCwmpValArray     = NULL;
    ULONG                           ulCwmpValArraySize= 0;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault    = (PCCSP_CWMP_SOAP_FAULT        )NULL;
    ULONG                           i;

    /* 
     * Notes: this API only takes full parameter names, no validation
     * here since it's internal API
     */
    SlapAllocStringArray2(NumOfParams, pSlapNameArray);

    if ( !pSlapNameArray )
    {
        return  ANSC_STATUS_RESOURCES;
    }

    for ( i = 0; i < (ULONG)NumOfParams; i ++ )
    {
        pSlapNameArray->Array.arrayString[i] = ppParamNames[i];   
    }

    returnStatus = 
        pCcspCwmpMpaIf->GetParameterValues
            (
                pCcspCwmpMpaIf->hOwnerContext,
                pSlapNameArray,
                NumOfParams,
                (void**)&pCwmpValArray,
                &ulCwmpValArraySize,
                (ANSC_HANDLE*)&pCwmpSoapFault,
                FALSE
            );

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        if ( ulCwmpValArraySize != (ULONG)NumOfParams )
        {
            returnStatus = ANSC_STATUS_INTERNAL_ERROR;
        }
        else
        {
            for ( i = 0; i < (ULONG)NumOfParams; i ++ )
            {
                SLAP_VARIABLE*      pSlapVar = pCwmpValArray[i].Value;

                if ( pSlapVar->Syntax == SLAP_VAR_SYNTAX_string )
                {
                    pParamValues[i] = pSlapVar->Variant.varString;
                    pSlapVar->Variant.varString = NULL;
                }
                else
                {
                    pParamValues[i] = NULL;
                }
            }
        }
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
        pCwmpSoapFault = NULL;
    }

    if ( pCwmpValArray )
    {
        for ( i = 0; i < ulCwmpValArraySize; i++ )
        {
            CcspCwmpCleanParamValue((&pCwmpValArray[i]));
        }

        CcspTr069PaFreeMemory(pCwmpValArray);
    }

    if ( pSlapNameArray )
    {
        CcspTr069PaFreeMemory(pSlapNameArray);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoGetParamStringValue
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pParamNames,
                int                         NumOfParams,
                char**                      pParamValues
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pParamName,
                Specifies the parameter name.

                char**                      pParamValue
                On successful return, it contains the values of
                the specified parameters.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoGetParamStringValue
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamNames,
        char**                      pParamValues
    )
{
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    returnStatus = 
        pMyObject->GetParamStringValues
            (
                (ANSC_HANDLE)pMyObject,
                &pParamNames,
                1,
                pParamValues
            );

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetParamValues
            (
                ANSC_HANDLE                 hThisObject,
                char**                      ppParamNames,
                int                         NumOfParams,
                PCCSP_VARIABLE              pParamValues
            );

    description:

        This function is called to retrieve object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char**                      ppParamNames,
                Specifies the parameter names.

                int                         NumOfParams
                Specifies the number of parameters.

                PCCSP_VARIABLE              pParamValues
                Specifies the values of specified parameters.

    return:     operation state.

**********************************************************************/

#if 0
ANSC_STATUS
CcspCwmpCpecoSetParamValues
    (
        ANSC_HANDLE                 hThisObject,
        char**                      ppParamNames,
        int                         NumOfParams,
        PCCSP_VARIABLE              pParamValues
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject     = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_PROPERTY   pProperty     = (PCCSP_CWMP_CPE_CONTROLLER_PROPERTY)&pMyObject->Property;
    PCCSP_CWMP_PROCESSOR_OBJECT     pCcspCwmpProcessor = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE        pCcspCwmpMpaIf     = (PCCSP_CWMP_MPA_INTERFACE      )pCcspCwmpProcessor->GetCcspCwmpMpaIf((ANSC_HANDLE)pCcspCwmpProcessor);
    PCCSP_CWMP_PARAM_VALUE          pCwmpValArray     = NULL;
    int                             iStatus           = 0;
    SLAP_STRING_ARRAY*              pSlapNameArray    = NULL;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault    = (PCCSP_CWMP_SOAP_FAULT        )NULL;
    ULONG                           i;

    pCwmpValArray = (PCCSP_CWMP_PARAM_VALUE)CcspTr069PaAllocateMemory(sizeof(CCSP_CWMP_PARAM_VALUE) * NumOfParams);
    if ( !pCwmpValArray )
    {
        returnStatus = ANSC_STATUS_RESOURCES;
        goto EXIT;
    }

    for ( i = 0; i < NumOfParams; i ++ )
    {
        pCwmpValArray[i].Name          = ppParamNames[i];
        pCwmpValArray[i].Value         = pParamValues + i;
        pCwmpValArray[i].Tr069DataType = pMyObject->GetParamDataType((ANSC_HANDLE)pMyObject, ppParamNames[i]);
    }

    returnStatus =
        pCcspCwmpMpaIf->SetParameterValues
            (
                pCcspCwmpMpaIf->hOwnerContext,
                pCwmpValArray,
                NumOfParams,
                &iStatus,
                (ANSC_HANDLE*)&pCwmpSoapFault,
                FALSE
            );

EXIT:

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
    }

    if ( pCwmpValArray )
    {
        CcspTr069PaFreeMemory(pCwmpValArray);
    }

    return  returnStatus;
}
#endif


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoAddObjects
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pObjectName,
                int                         NumInstances,
                PULONG                      pInsNumbers,
                PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
            )

    description:

        This function is called to add the specified number of
        instances under the given object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pObjectName
                Object name.

                int                         NumInstances
                Number of instances to add.

                PULONG                      pInsNumbers
                Instance numbers of newly created entry objects.

                PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
                Indicate error.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoAddObjects
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pObjectName,
        int                         NumInstances,
        PULONG                      pInsNumbers,
        PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject         = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf        = (PCCSP_CWMP_MPA_INTERFACE          )pCcspCwmpProcessor->GetCcspCwmpMpaIf((ANSC_HANDLE)pCcspCwmpProcessor);
    int                             iStatus           = 0;
    PCCSP_CWMP_SOAP_FAULT           pCwmpFault        = NULL;
    int                             i;

    *ppCwmpFault = NULL;
    AnscZeroMemory(pInsNumbers, sizeof(ULONG) * NumInstances);
    
    for ( i = 0; i < NumInstances; i ++ )
    {
        returnStatus = 
            pCcspCwmpMpaIf->AddObject
                (
                    pCcspCwmpMpaIf->hOwnerContext,
                    pObjectName,
                    &pInsNumbers[i],
                    &iStatus,
                    (ANSC_HANDLE*)&pCwmpFault,
                    FALSE
                );

        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            if ( i > 0 )
            {
                PCCSP_CWMP_SOAP_FAULT   pDoCwmpFault  = NULL;

                pMyObject->DeleteObjects
                    (
                        (ANSC_HANDLE)pMyObject,
                        pObjectName,
						i,
                        pInsNumbers,
                        &pDoCwmpFault,
                        0
                    );

                if ( pDoCwmpFault )
                {
                    CcspCwmpFreeSoapFault(pDoCwmpFault);
                    pDoCwmpFault = NULL;
                }

                AnscZeroMemory(pInsNumbers, sizeof(ULONG) * i);
            }

            *ppCwmpFault = pCwmpFault;

            break;
        }
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoAddObjects
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pObjectName,
                int                         NumInstances,
                PULONG                      pInsNumbers,
                PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
            )

    description:

        This function is called to delete the specified instances
        of the given table object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pObjectName
                Object name.

                int                         NumInstances
                Number of instances to delete.

                PULONG                      pInsNumbers
                Instance numbers of entry objects to delete.

                PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
                Indicate error.

                ULONG                       DelaySeconds
                Delay seconds before removing the specified
                objects.

    return:     operation state.

**********************************************************************/

typedef  struct
_CCSP_CWMP_CPECO_DDO_TASK_CTX
{
    ANSC_HANDLE                     hCcspCwmpCpeController;
    char*                           pObjectName;
    int                             NumInstances;
    PULONG                          pInsNumbers;
    ULONG                           DelaySeconds;
}
CCSP_CWMP_CPECO_DDO_TASK_CTX, *PCCSP_CWMP_CPECO_DDO_TASK_CTX;


ANSC_STATUS
CcspCwmpCpecoDelayedDelObjectsTask
    (
        ANSC_HANDLE                 hTaskContext
    )
{
    PCCSP_CWMP_CPECO_DDO_TASK_CTX        pDdoTaskCtx       = (PCCSP_CWMP_CPECO_DDO_TASK_CTX     )hTaskContext;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject         = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )pDdoTaskCtx->hCcspCwmpCpeController;
    PCCSP_CWMP_SOAP_FAULT           pCwmpFault        = NULL;
    ULONG                           ulStart           = AnscGetTickInSeconds();
    ULONG                           ulNow             = ulStart;

    while ( pMyObject->bActive )
    {
        AnscSleep(2000);

        if ( ulNow > ulStart && ulNow - ulStart >= pDdoTaskCtx->DelaySeconds )
        {
            break;
        }
        else if ( ulNow <= ulStart && 0xFFFFFFFF - ulStart + ulNow >= pDdoTaskCtx->DelaySeconds )
        {
            break;
        }
    }

    if ( pMyObject->bActive )
    {
        pMyObject->DeleteObjects
            (
                (ANSC_HANDLE)pMyObject,
                pDdoTaskCtx->pObjectName,
                pDdoTaskCtx->NumInstances,
                pDdoTaskCtx->pInsNumbers,
                &pCwmpFault,
                0
            );
    }

    if ( pDdoTaskCtx->pObjectName )
    {
        CcspTr069PaFreeMemory(pDdoTaskCtx->pObjectName);
    }

    if ( pDdoTaskCtx->pInsNumbers )
    {
        CcspTr069PaFreeMemory(pDdoTaskCtx->pInsNumbers);
    }

    CcspTr069PaFreeMemory(pDdoTaskCtx);

    return  ANSC_STATUS_SUCCESS;
}


ANSC_STATUS
CcspCwmpCpecoDeleteObjects
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pObjectName,
        int                         NumInstances,
        PULONG                      pInsNumbers,
        PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault,
        ULONG                       DelaySeconds
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject         = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf        = (PCCSP_CWMP_MPA_INTERFACE          )pCcspCwmpProcessor->GetCcspCwmpMpaIf((ANSC_HANDLE)pCcspCwmpProcessor);
    int                             iStatus           = 0;
    PCCSP_CWMP_SOAP_FAULT           pCwmpFault        = NULL;
    int                             i;
    char                            buf[512];

    *ppCwmpFault = NULL;

    if ( DelaySeconds != 0 )
    {
        PCCSP_CWMP_CPECO_DDO_TASK_CTX    pDdoTaskCtx = NULL;

        pDdoTaskCtx = 
            (PCCSP_CWMP_CPECO_DDO_TASK_CTX)CcspTr069PaAllocateMemory
                (
                    sizeof(CCSP_CWMP_CPECO_DDO_TASK_CTX)
                );

        if ( !pDdoTaskCtx )
        {
            return  ANSC_STATUS_RESOURCES;
        }

        pDdoTaskCtx->hCcspCwmpCpeController = (ANSC_HANDLE)pMyObject;
        pDdoTaskCtx->NumInstances       = NumInstances;
        pDdoTaskCtx->pObjectName        = AnscCloneString(pObjectName);
        pDdoTaskCtx->DelaySeconds       = DelaySeconds;
        pDdoTaskCtx->pInsNumbers        = (PULONG)CcspTr069PaAllocateMemory(sizeof(ULONG) * NumInstances);
        if ( pDdoTaskCtx->pInsNumbers )
        {
            AnscCopyMemory(pDdoTaskCtx->pInsNumbers, pInsNumbers, NumInstances * sizeof(ULONG));
        }

        AnscSpawnTask
            (
                (void*)CcspCwmpCpecoDelayedDelObjectsTask,
                (ANSC_HANDLE)pDdoTaskCtx,
                "CcspCwmpCpecoDelayedDelObjectsTask"
             );
    }
    else
    {
        for ( i = 0; i < NumInstances; i ++ )
        {
            if ( pInsNumbers[i] == 0 )
            {
                continue;
            }

            if ( !CcspCwmpIsPartialName(pObjectName) )
            {
                _ansc_sprintf(buf, "%s.%u.", pObjectName, (unsigned int)pInsNumbers[i]);
            }
            else
            {
                _ansc_sprintf(buf, "%s%u.", pObjectName, (unsigned int)pInsNumbers[i]);
            }

            returnStatus = 
                pCcspCwmpMpaIf->DeleteObject
                    (
                        pCcspCwmpMpaIf->hOwnerContext,
                        buf,
                        &iStatus,
                        (ANSC_HANDLE*)&pCwmpFault,
                        FALSE
                    );

            if ( *ppCwmpFault == NULL )
            {
                *ppCwmpFault = pCwmpFault;
            }
            else
            {
                CcspCwmpFreeSoapFault(pCwmpFault);
            }
        }
    }

    if ( *ppCwmpFault )
    {
        returnStatus = ANSC_STATUS_INTERNAL_ERROR;
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoMonitorOpState
            (
                ANSC_HANDLE                 hThisObject,
                BOOL                        bMonitor,
                char*                       pObjectName,
                int                         NumInstances,
                PULONG                      pInsNumbers,
                char*                       pStateParamName,
                PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
            );

    description:

        This function is called to delete the specified instances
        of the given table object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                BOOL                        bMonitor
                Specify what to do - start or finish monitoring.

                char*                       pObjectName
                Object name.

                int                         NumInstances
                Number of instances to delete.

                PULONG                      pInsNumbers
                Instance numbers of entry objects to delete.

                char*                       pStateParamName
                Specifies state parameter name to monitor on.

                PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
                Indicate error.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoMonitorOpState
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bMonitor,
        char*                       pObjectName,
        int                         NumInstances,
        PULONG                      pInsNumbers,
        char*                       pStateParamName,
        PCCSP_CWMP_SOAP_FAULT*      ppCwmpFault
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject         = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf        = (PCCSP_CWMP_MPA_INTERFACE          )pCcspCwmpProcessor->GetCcspCwmpMpaIf((ANSC_HANDLE)pCcspCwmpProcessor);
    PCCSP_CWMP_SOAP_FAULT           pCwmpFault        = NULL;
    int                             i;
    char                            buf[512];
    PCCSP_CWMP_SET_PARAM_ATTRIB     pParamAttrs       = NULL;
    ULONG                           notif             = bMonitor ? CCSP_CWMP_NOTIFICATION_passive : CCSP_CWMP_NOTIFICATION_off;
    BOOL                            bUnderTable       = ( NumInstances > 0 );

    *ppCwmpFault = NULL;

    if ( !bUnderTable )
    {
        NumInstances = 1;
    }

    pParamAttrs = 
        (PCCSP_CWMP_SET_PARAM_ATTRIB)CcspTr069PaAllocateMemory
            (
                sizeof(CCSP_CWMP_SET_PARAM_ATTRIB) * NumInstances
            );

    if ( !pParamAttrs )
    {
        return  ANSC_STATUS_RESOURCES;
    }

    for ( i = 0; i < NumInstances; i ++ )
    {
        if ( !pStateParamName || pStateParamName[0] == 0 )
        {
            AnscCopyString(buf, pObjectName);   
        }
        else if ( bUnderTable )
        {
            _ansc_sprintf
                (
                    buf, 
                    "%s%u.%s", 
                    pObjectName,
                    (unsigned int)pInsNumbers[i],
                    pStateParamName
                );
        }
        else
        {
            _ansc_sprintf
                (
                    buf, 
                    "%s%s", 
                    pObjectName,
                    pStateParamName
                );
        }

        pParamAttrs[i].Name                 = AnscCloneString(buf);
        pParamAttrs[i].bNotificationChange  = TRUE;
        pParamAttrs[i].Notification         = notif;
        pParamAttrs[i].bAccessListChange    = FALSE;
        pParamAttrs[i].AccessList           = NULL;

        if ( !pParamAttrs[i].Name )
        {
            break;
        }
    }

    if ( i < NumInstances )
    {
        for ( i = 0; i < NumInstances; i ++ )
        {
            CcspCwmpCleanSetParamAttrib((pParamAttrs+i));
        }

        CcspTr069PaFreeMemory(pParamAttrs);
    }
    else
    {
        returnStatus = 
            pCcspCwmpMpaIf->SetParameterAttributes
                (
                    pCcspCwmpMpaIf->hOwnerContext,
                    pParamAttrs,
                    NumInstances,
                    (ANSC_HANDLE*)&pCwmpFault,
                    FALSE
                );

        for (i = 0; i < NumInstances; ++ i)
        {
            CcspCwmpCleanSetParamAttrib((pParamAttrs + i));
        }
        CcspTr069PaFreeMemory(pParamAttrs);

        *ppCwmpFault = pCwmpFault;
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoRegisterPA
            (
                ANSC_HANDLE                 hThisObject,
                BOOL                        bRegister
            );

    description:

        This function is called to register PA into CDMM.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                BOOL                        bRegister
                Specifies if registeration or un-registeration
                is to perform.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoRegisterPA
    (
        ANSC_HANDLE                 hThisObject,
        BOOL                        bRegister
    )
{
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    int                             nRet;

    if ( bRegister )
    {
        /* built-in Management Server Functional Component will register PA along with
         * all CCSP namespaces supported by PA.
         */  

        /*
        CcspTr069PaTraceInfo(("Registering TR-069 PA into CR, PA name <%s> ...\n", pMyObject->PAName));
        nRet = 
            CcspBaseIf_registerCapabilities
                (
                    pMyObject->hMsgBusHandle,
                    pMyObject->CRName,
                    pMyObject->PAName,
                    CCSP_TR069PA_VERSION,
                    CCSP_TR069PA_DBUS_PATH,
                    NULL,
                    0
                );

        if ( nRet != CCSP_SUCCESS )
        {
            CcspTr069PaTraceError(("TR-069 PA fails to register with CR!\n"));
            returnStatus = ANSC_STATUS_FAILURE;
        }
        else
        {
            CcspTr069PaTraceInfo(("TR-069 PA has registered into CR successfully, PA name <%s>.\n", pMyObject->PAName));
        }
        */
    }
    else
    {
        /*
         * Unless Management Server Functional Component needs to un-register
         * all namespaces specifically, PA will unregister PA as a whole.
         */

        CcspTr069PaTraceInfo(("Unregistering TR-069 PA from CR ...\n"));

        nRet = 
            CcspBaseIf_unregisterComponent
                (
                    pMyObject->hMsgBusHandle,
                    pMyObject->CRNameWithPrefix,
                    pMyObject->PAName
                );

        if ( nRet != CCSP_SUCCESS )
        {
            CcspTr069PaTraceError(("TR-069 PA fails to unregister with CR!\n"));
            returnStatus = ANSC_STATUS_FAILURE;
        }
        else
        {
            CcspTr069PaTraceInfo(("TR-069 PA has been unregistered into CR successfully, PA name <%s>.\n", pMyObject->PAName));
        }
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetPAName
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pName
            );

    description:

        This function is called to set PA name.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pName
                Specifies PA name.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSetPAName
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    )
{
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    int                             nLen;

    if ( pMyObject->PAName )
    {
        CcspTr069PaFreeMemory(pMyObject->PAName);
    }

    if ( !pName )
    {
        pMyObject->PAName = NULL;
    }
    else
    {
        pMyObject->PAName = CcspTr069PaCloneString(pName);
        
        if ( !pMyObject->PAName )
        {
            return  ANSC_STATUS_RESOURCES;
        }
    }

    if ( pMyObject->PANameWithPrefix )
    {
        CcspTr069PaFreeMemory(pMyObject->PANameWithPrefix);
    }

    nLen = (pMyObject->PAName?AnscSizeOfString(pMyObject->PAName):0) + (pMyObject->SubsysName ? AnscSizeOfString(pMyObject->SubsysName) : 0) + 2;
    pMyObject->PANameWithPrefix = (char*)CcspTr069PaAllocateMemory(nLen);

    if ( !pMyObject->PANameWithPrefix )
    {
        return  ANSC_STATUS_RESOURCES;
    }

    if ( pMyObject->SubsysName )
    {
        _ansc_sprintf
            (
                pMyObject->PANameWithPrefix, 
                "%s%s", 
                pMyObject->SubsysName,
                pMyObject->PAName
            );
    }
    else
    {
        AnscCopyString(pMyObject->PANameWithPrefix, pMyObject->PAName);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmpCpecoGetPAName
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to get PA name.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     PA name.

**********************************************************************/

char*
CcspCwmpCpecoGetPAName
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return  pMyObject->PAName;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetSubsysName
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pName
            );

    description:

        This function is called to set sub-system name.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pName
                Specifies sub-system name PA is running on.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSetSubsysName
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    )
{
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    int                             nLen;

    if ( pMyObject->SubsysName )
    {
        CcspTr069PaFreeMemory(pMyObject->SubsysName);
    }

    if ( !pName )
    {
        pMyObject->SubsysName = NULL;
    }
    else
    {
        pMyObject->SubsysName = CcspTr069PaCloneString(pName);
        
        if ( !pMyObject->SubsysName )
        {
            return  ANSC_STATUS_RESOURCES;
        }
    }

    if ( pMyObject->PANameWithPrefix )
    {
        CcspTr069PaFreeMemory(pMyObject->PANameWithPrefix);
    }

    nLen = AnscSizeOfString(pMyObject->PAName) + (pMyObject->SubsysName ? AnscSizeOfString(pMyObject->SubsysName) : 0) + 2;
    pMyObject->PANameWithPrefix = (char*)CcspTr069PaAllocateMemory(nLen);

    if ( !pMyObject->PANameWithPrefix )
    {
        return  ANSC_STATUS_RESOURCES;
    }

    if ( pMyObject->SubsysName )
    {
        _ansc_sprintf
            (
                pMyObject->PANameWithPrefix, 
                "%s%s", 
                pMyObject->SubsysName,
                pMyObject->PAName
            );
    }
    else
    {
        AnscCopyString(pMyObject->PANameWithPrefix, pMyObject->PAName);
    }

    if (pMyObject->CRNameWithPrefix)
    {
        CcspTr069PaFreeMemory(pMyObject->CRNameWithPrefix);
        pMyObject->CRNameWithPrefix = NULL;
    }
    nLen = AnscSizeOfString(pMyObject->CRName) + (pMyObject->SubsysName ? AnscSizeOfString(pMyObject->SubsysName) : 0) + 2;
    pMyObject->CRNameWithPrefix = (char*)CcspTr069PaAllocateMemory(nLen);

    if ( !pMyObject->CRNameWithPrefix )
    {
        return  ANSC_STATUS_RESOURCES;
    }

    if ( pMyObject->SubsysName )
    {
        _ansc_sprintf
            (
                pMyObject->CRNameWithPrefix, 
                "%s%s", 
                pMyObject->SubsysName,
                pMyObject->CRName
            );
    }
    else
    {
        AnscCopyString(pMyObject->CRNameWithPrefix, pMyObject->CRName);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmpCpecoGetSubsysName
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to get sub-system name.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     sub-system name.

**********************************************************************/

char*
CcspCwmpCpecoGetSubsysName
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return  pMyObject->SubsysName;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetCRName
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pName
            );

    description:

        This function is called to set CR name.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pName
                Specifies CR name.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSetCRName
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    )
{
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    if ( pMyObject->CRName )
    {
        CcspTr069PaFreeMemory(pMyObject->CRName);
    }

    if ( !pName )
    {
        pMyObject->CRName = NULL;
    }
    else
    {
        int                         nLen;

        pMyObject->CRName = CcspTr069PaCloneString(pName);
        
        returnStatus = ( pMyObject->CRName != NULL );

        if ( pMyObject->CRNameWithPrefix )
        {
            CcspTr069PaFreeMemory(pMyObject->CRNameWithPrefix);
            pMyObject->CRNameWithPrefix = NULL;
        }     

        nLen = (pMyObject->CRName?AnscSizeOfString(pMyObject->CRName):0) + (pMyObject->SubsysName ? AnscSizeOfString(pMyObject->SubsysName) : 0) + 2;
        pMyObject->CRNameWithPrefix = (char*)CcspTr069PaAllocateMemory(nLen);

        if ( !pMyObject->CRNameWithPrefix )
        {
            return  ANSC_STATUS_RESOURCES;
        }

        if ( pMyObject->SubsysName )
        {
            _ansc_sprintf
                (
                    pMyObject->CRNameWithPrefix, 
                    "%s%s", 
                    pMyObject->SubsysName,
                    pMyObject->CRName
                );
        }
        else
        {
            AnscCopyString(pMyObject->CRNameWithPrefix, pMyObject->CRName);
        }
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmpCpecoGetCRName
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to get CR name.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     CR name.

**********************************************************************/

char*
CcspCwmpCpecoGetCRName
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return  pMyObject->CRName;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetCRBusPath
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pSubsystem,
                char*                       pBusPath
            );

    description:

        This function is called to set CR Bus Path.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pSubsystem
                Specifies sub-system name.

                char*                       pBusPath
                Specifies MBus Path.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSetCRBusPath
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pSubsystem,
        char*                       pBusPath
    )
{
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    int                             i;

    if ( pMyObject->NumSubsystems >= CCSP_SUBSYSTEM_MAX_COUNT )
    {
        return  ANSC_STATUS_RESOURCES;
    }

    for ( i = 0; i < pMyObject->NumSubsystems; i ++ )
    {
        if ( AnscEqualString(pSubsystem, pMyObject->Subsystem[i], TRUE) )
        {
            break;
        }
    }

    if ( i >= pMyObject->NumSubsystems )
    {
        pMyObject->Subsystem[pMyObject->NumSubsystems  ] = pSubsystem ? CcspTr069PaCloneString(pSubsystem) : NULL;
        pMyObject->MBusPath  [pMyObject->NumSubsystems++] = CcspTr069PaCloneString(pBusPath);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmpCpecoGetCRBusPath
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pSubsystem
            );

    description:

        This function is called to get CR name.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     CR name.

**********************************************************************/

char*
CcspCwmpCpecoGetCRBusPath
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pSubsystem
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    int                             i;

    for ( i = 0; i < pMyObject->NumSubsystems; i ++ )
    {
        if ( AnscEqualString(pSubsystem, pMyObject->Subsystem[i], TRUE) )
        {
            return pMyObject->MBusPath[i];
        }
    }

    return  NULL;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetPAMapperFile
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pName
            );

    description:

        This function is called to set PA mapper file location.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pName
                Specifies PA mapper file location.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSetPAMapperFile
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    )
{
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    if ( pMyObject->PAMapperFile )
    {
        CcspTr069PaFreeMemory(pMyObject->PAMapperFile);
    }

    if ( !pName )
    {
        pMyObject->PAMapperFile = NULL;
    }
    else
    {
        pMyObject->PAMapperFile = CcspTr069PaCloneString(pName);
        
        returnStatus = ( pMyObject->PAMapperFile != NULL );
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetPACustomMapperFile
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pName
            );

    description:

        This function is called to set PA custom mapper file location.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is the pointer of this object itself.

                char*                       pName
                Specifies PA custom mapper file location.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSetPACustomMapperFile
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    )
{
    ANSC_STATUS                          returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT) hThisObject;


    if ( pMyObject->PACustomMapperFile )
    {
        CcspTr069PaFreeMemory(pMyObject->PACustomMapperFile);
    }

    if ( !pName )
    {
        pMyObject->PACustomMapperFile = NULL;
    }
    else
    {
        pMyObject->PACustomMapperFile = CcspTr069PaCloneString(pName);

        returnStatus = ( pMyObject->PACustomMapperFile != NULL );
    }


    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetSDMXmlFilename
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pName
            );

    description:

        This function is called to set SDM XML file name.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pName
                Specifies SDM XML file name.

    return:     operation state.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSetSDMXmlFilename
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    )
{
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    pMyObject->SdmXmlFile = pName;

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmpCpecoGetSDMXmlFilename
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to get SDM XML file name.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     SDM XML file name.

**********************************************************************/

char*
CcspCwmpCpecoGetSDMXmlFilename
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return  pMyObject->SdmXmlFile;
}


ANSC_STATUS
CcspCwmpCpecoSetOutboundIfName
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pName
    )
{
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    pMyObject->OutboundIfName = pName;

    return  returnStatus;
}

char*
CcspCwmpCpecoGetOutboundIfName
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return  pMyObject->OutboundIfName;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoGetMsgBusHandle
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to get Msg Bus handle.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     Msg Bus handle.

**********************************************************************/

ANSC_HANDLE
CcspCwmpCpecoGetMsgBusHandle
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return  pMyObject->hMsgBusHandle;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        int
        CcspCwmpCpecoGetParamNotification
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pParamName
            );

    description:

        This function is called to get notification attribute
        of the given parameter.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pParamName
                Parameter name.

    return:     notification attribute.

**********************************************************************/

int
CcspCwmpCpecoGetParamNotification
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject           = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor   = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;

    return 
        (int)pCcspCwmpProcessor->CheckParamAttrCache
            (
                (ANSC_HANDLE)pCcspCwmpProcessor, 
                pParamName
            );
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetParamNotification
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pParamName,
                int                         Notification
            );

    description:

        This function is called to get notification attribute
        of the given parameter.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pParamName
                Parameter name.

                int                         Notification
                notification attribute.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSetParamNotification
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        int                         Notification
    )
{
    ANSC_STATUS                     returnStatus        = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject           = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor   = (PCCSP_CWMP_PROCESSOR_OBJECT   )pMyObject->hCcspCwmpProcessor;
    CCSP_CWMP_SET_PARAM_ATTRIB      cwmpSetParamAttr    = {0};

    cwmpSetParamAttr.Name                = pParamName;
    cwmpSetParamAttr.bNotificationChange = TRUE;
    cwmpSetParamAttr.Notification        = Notification;

    returnStatus = 
        pCcspCwmpProcessor->UpdateParamAttrCache
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                &cwmpSetParamAttr,
                1
            );

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        int
        CcspCwmpCpecoGetParamDataType
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pParamName
            );

    description:

        This function is called to get data type
        of the given parameter.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pParamName
                Parameter name.

    return:     data type - one simple data type TR-069 defines.

**********************************************************************/

int
CcspCwmpCpecoGetParamDataType
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject           = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)hThisObject;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor   = (PCCSP_CWMP_PROCESSOR_OBJECT )pMyObject->hCcspCwmpProcessor;
    PCCSP_NAMESPACE_MGR_OBJECT      pCcspNsMgr          = (PCCSP_NAMESPACE_MGR_OBJECT )pCcspCwmpProcessor->hCcspNamespaceMgr;
    int                             dataType            = CCSP_CWMP_TR069_DATA_TYPE_Unspecified;
    CCSP_STRING                     Subsystems[CCSP_SUBSYSTEM_MAX_COUNT];
    int                             NumSystems;
    CCSP_STRING                     pSubsystem          = NULL;
    componentStruct_t**             ppComp              = NULL;
    ULONG                           NumComp             = 0;

    /* 
     * 1. identify which sub-system this parameter is located against mapper API
     * 2. check against namespace manager 
     */

    if ( !pParamName || *pParamName == 0 || CcspCwmpIsPartialName(pParamName) )
    {
        return  dataType;
    }

    NumSystems = CCSP_SUBSYSTEM_MAX_COUNT;

    CcspTr069PA_GetNamespaceSubsystems
        (
            pMyObject->hMsgBusHandle,
            pParamName,
            Subsystems,
            &NumSystems,
            CCSP_TRUE
        );

    if ( NumSystems == 0 )
    {
        Subsystems[0] = CcspTr069PaCloneString(pMyObject->SubsysName);
        NumSystems    = 1;
        pSubsystem    = Subsystems[0];
    }
    else if ( NumSystems == 1 )
    {
        pSubsystem    = Subsystems[0];
    }

    if ( TRUE )
    {
        int                         nRet;

        nRet = 
            pCcspNsMgr->DiscoverNamespace
                (
                    (ANSC_HANDLE)pCcspNsMgr,
                    pParamName,
                    pSubsystem,
                    FALSE,
                    (void***)&ppComp,
                    &NumComp
                );

        if ( nRet == CCSP_SUCCESS || NumComp == 1 )
        {
            dataType = CcspTr069PA_Ccsp2CwmpType(ppComp[0]->type);
        }
    }

    CcspTr069FreeStringArray(Subsystems, NumSystems, FALSE);

    if ( ppComp )
    {
        free_componentStruct_t(pMyObject->hMsgBusHandle, NumComp, ppComp);
    }

    return  dataType;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSaveCfgToPsm
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pCfgKey,
                char*                       pCfgValue
            );

    description:

        This function is called to save configuration into
        PSM.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pCfgKey
                Configuration key.

                char*                       pCfgValue
                Configuration value.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSaveCfgToPsm
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCfgKey,
        char*                       pCfgValue
    )
{
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    int                             nCcspError;

    /* save parameter notification into from PSM */
    nCcspError = 
        PSM_Set_Record_Value2
            (
                pMyObject->hMsgBusHandle,
                pMyObject->SubsysName,
                pCfgKey,
                ccsp_string,
                pCfgValue
            );

    if ( nCcspError != CCSP_SUCCESS )
    {
        returnStatus = ANSC_STATUS_FAILURE;
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmpCpecoLoadCfgFromPsm
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pCfgKey
            );

    description:

        This function is called to load configuration value
        on the specified key.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pCfgKey
                Configuration key.

    return:     configuration value to the key.

**********************************************************************/

char*
CcspCwmpCpecoLoadCfgFromPsm
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCfgKey
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;
    char*                           pCfgValue      = NULL;
    int                             nCcspError;

    /*
     *   This function loads configuration value on the given key from PSM
     */
    nCcspError = 
        PSM_Get_Record_Value2
            (
                pMyObject->hMsgBusHandle,
                pMyObject->SubsysName,
                pCfgKey,
                NULL,
                &pCfgValue
            );

	if ( nCcspError != CCSP_SUCCESS && pCfgValue ) pCfgValue = NULL;

    return  pCfgValue;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpCpecoSetParameterKey
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pParameterKey
            );

    description:

        This function is called to save parameter key.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pParameterKey
                Parameter key.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CcspCwmpCpecoSetParameterKey
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParameterKey
    )
{
    ANSC_STATUS                     returnStatus   = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject      = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    /* 
     * This function saves ParameterKey into ManagementServer 
     * functional component through internal interface (not through
     * SetParameterValues namely).
     */

    CcspManagementServer_SetParameterKey
        (
            pMyObject->PANameWithPrefix,
            pParameterKey
        );

    return  returnStatus;
}

