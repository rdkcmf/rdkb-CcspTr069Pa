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

    module:	ssp_debug.c

        For CCSP TR-069 PA SSP
        BroadWay Service Delivery System

    ---------------------------------------------------------------

    description:

        SSP implementation of test functions of PA.

        *   ssp_dumpPiTree
        *   ssp_testNsSyncWithCR
        *   ssp_testNsAgainstMapper
        *   ssp_testGPN
        *   ssp_testGPV
        *   ssp_testSPV
        *   ssp_testSPV_notFromPA
        *   ssp_testGPA
        *   ssp_testSPA
        *   ssp_testCDS
        *   ssp_testFirmwareDownload
        *   ssp_testGetMethods
        *   ssp_testAddObject
        *   ssp_testDeleteObject
        *   ssp_checkCachedParamAttr
        *   ssp_sendValueChangeSignal
        *   ssp_sendDiagCompleteSignal 
    
    ---------------------------------------------------------------

    environment:

        Platform dependent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/11/2011  initial revision.

**********************************************************************/


#include "ssp_global.h"


#ifdef   _DEBUG

extern  PCCSP_CWMP_CPE_CONTROLLER_OBJECT            g_pCcspCwmpCpeController;
extern  BOOL                                        bEngaged;

static  BOOL                                        s_bExcludeInvNamespaces = TRUE;
extern  char                                        g_Subsystem[32];

#define  MAX_NS_AL_LEN                              256   /*RDKB-7336, CID-33191, defining maximum length for NS and AL*/
#define  MAX_VALUE_ARRAY_LEN                        512   /*RDKB-7336, CID-32893, defining maximum length for NS and AL*/
#define  MAX_DATA_TYPR_LEN                          32    /*RDKB-7336, CID-32893, defining maximum length for NS and AL*/

#define  ssp_remove_trailing_CRLF(ss)                                               \
    do {                                                                            \
        int                         size, i;                                        \
        size = AnscSizeOfString(ss);                                                \
        for ( i = size - 1; i >= 0; i -- )                                          \
        {                                                                           \
            if ( ss[i] == '\r' || ss[i] == '\n' ) ss[i] = 0;                        \
                else break;                                                         \
        }                                                                           \
    } while (0)


void ssp_setLogLevel()
{
    ULONG                           ulTraceLevel = CCSP_TRACE_LEVEL_DEBUG;
    char                            ans[256];
    int                             choice;

    printf("Trace level (0(Emergency)/1(Alert)/2(Critical)/3(Error)/4(Warning)/5(Info)/6(Debug)): ");
    fflush(stdin);
    while(getc(stdin) != '\n') ;
    fgets(ans, 256, stdin);
    ssp_remove_trailing_CRLF(ans);

    choice = _ansc_atoi(ans);

    switch ( choice )
    {
        case    0:

                ulTraceLevel = CCSP_TRACE_LEVEL_EMERGENCY;

                break;

        case    1:

                ulTraceLevel = CCSP_TRACE_LEVEL_ALERT;

                break;

        case    2:

                ulTraceLevel = CCSP_TRACE_LEVEL_CRITICAL;

                break;

        case    3:

                ulTraceLevel = CCSP_TRACE_LEVEL_ERROR;

                break;

        case    4:

                ulTraceLevel = CCSP_TRACE_LEVEL_WARNING;

                break;

        case    5:

                ulTraceLevel = CCSP_TRACE_LEVEL_INFO;

                break;

        default:

                ulTraceLevel = CCSP_TRACE_LEVEL_DEBUG;

                break;

    }

    AnscSetTraceLevel(ulTraceLevel);
}



static 
CCSP_BOOL
ssp_dumpPiTreeNodeCB
    (
        CCSP_HANDLE                 pContext,
        PCCSP_TR069_PARAM_INFO      pParamInfo
    )
{
    UNREFERENCED_PARAMETER(pContext);
    char                            fullName[257] = {0};

    CcspTr069PA_GetPiFullName(pParamInfo, fullName, 257);        

    printf
        (
            "  namespace <%s>, sub-sys <%s>, cloud type <%d>\n",
            fullName,
            pParamInfo->Subsystem ? pParamInfo->Subsystem : "",
            pParamInfo->CloudType
        );

    return  CCSP_TRUE;
}


void ssp_dumpPiTree()
{
    PCCSP_TR069_PARAM_INFO          pPiTree;

    printf("---------------------------------------------\n");
    printf("Dumping parameter tree ...\n");

    pPiTree = CcspTr069PA_GetPiTreeRoot(FALSE);

    CcspTr069PA_TraversePiTree
        (
            g_pCcspCwmpCpeController->hTr069PaMapper,
            pPiTree,
            FALSE,
            NULL,
            ssp_dumpPiTreeNodeCB,
            NULL
        );

    printf("parameter tree has been dumped \n\n");

    printf("---------------------------------------------\n");
    printf("Dumping invisible parameter tree ...\n");

    pPiTree = CcspTr069PA_GetPiTreeRoot(TRUE);

    CcspTr069PA_TraversePiTree
        (
            g_pCcspCwmpCpeController->hTr069PaMapper,
            pPiTree,
            FALSE,
            NULL,
            ssp_dumpPiTreeNodeCB,
            NULL
        );

    printf("parameter tree has been dumped \n\n");

}


void ssp_testNsSyncWithCR()
{
                printf("Simulating systemReady signal is received ...\n");

                if ( !bEngaged )
                {
                    printf("TR-069 PA SSP not engaged yet, please try again later!\n");
                }
                else
                {
                    PCCSP_CWMP_PROCESSOR_OBJECT  pCcspCwmpProcessor = NULL;

                    pCcspCwmpProcessor = (PCCSP_CWMP_PROCESSOR_OBJECT)g_pCcspCwmpCpeController->GetCcspCwmpProcessor((ANSC_HANDLE)g_pCcspCwmpCpeController);

/* 
* This callback process mechanism moved into CcspCwmppoProcessSysReadySignal() 
* API. Due to system ready signal handling for TR069 restart process from selfheal scripts.
*/
#if 0
                    CcspCwmppoSysReadySignalCB((void*)pCcspCwmpProcessor);
#else
					CcspCwmppoProcessSysReadySignal((void*)pCcspCwmpProcessor);
#endif /* 0 */
                }
}


void ssp_testNsAgainstMapper()
{
                    char            ns[256] = {0};
                    char            ss[32];

                    printf("Input namespace to check if it's supported: ");
                    fflush(stdin);
                    while(getc(stdin) != '\n') ;
                    fgets(ns, 256, stdin);

                    ssp_remove_trailing_CRLF(ns);

                    if ( ns[0] == 0 ) printf("Invalid namespace, please try it again.\n");
                    else
                    {
                        CCSP_BOOL   bSupported, bInvisible;

                        printf("Input sub-system name: ");
                        fflush(stdin);
                        fgets(ss, 32, stdin);

                        ssp_remove_trailing_CRLF(ss);

                        bSupported =
                            CcspTr069PA_IsNamespaceSupported
                                (
                                    g_pCcspCwmpCpeController->hTr069PaMapper,
                                    ns,
                                    ss[0] == 0 ? NULL : ss,
                                    TRUE
                                );

                        bInvisible =
                            CcspTr069PA_IsNamespaceInvisible
                                (
                                    g_pCcspCwmpCpeController->hTr069PaMapper,
                                    ns
                                );

                        printf("Supported - [%s], Visible - [%s]\n", bSupported ? "yes":"no", bInvisible ? "no":"yes");
                    }
}


void ssp_testGPN()
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )g_pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_PARAM_INFO           pParamInfoArray    = (PCCSP_CWMP_PARAM_INFO      )NULL;
    ULONG                           ulArraySize        = (ULONG                      )0;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ULONG                           i                  = 0;
    ANSC_STATUS                     returnStatus;
    char                            ns[256];
    char                            nextLevel;
    BOOL                            bNextLevel         = TRUE;
    errno_t rc       = -1;

    printf("Input namespace: ");
    fflush(stdin);
    while(getc(stdin) != '\n') ;
    fgets(ns, 256, stdin);

    ssp_remove_trailing_CRLF(ns);
    if ( ns[0] == 0 )
    {
        printf("Namespace 'Device.' is used ...\n");
        rc = strcpy_s(ns,sizeof(ns),"Device.");
        ERR_CHK(rc); 
    }

    printf("Next level (y/n): ");
    fflush(stdin);
    nextLevel = getchar();

    if ( nextLevel == 'n' || nextLevel == 'N' ) bNextLevel = FALSE;

    returnStatus =
        pCcspCwmpMpaIf->GetParameterNames
        (
            pCcspCwmpMpaIf->hOwnerContext,
            ns,
            bNextLevel,
            (void**)&pParamInfoArray,
            &ulArraySize,
            (ANSC_HANDLE*)&pCwmpSoapFault,
            s_bExcludeInvNamespaces
        );

    printf("GPN returns code %d.\n", (int)returnStatus);
    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        printf("--> GPN returns %u namespaces\n", (unsigned int)ulArraySize);
        for ( i = 0; i < ulArraySize; i++ )
        {
            printf(" [%s] %s\n", pParamInfoArray[i].bWritable?"W":"-", pParamInfoArray[i].Name);
        }
        printf("GPN returns %u namespaces <--\n", (unsigned int)ulArraySize);
    }

    if ( pParamInfoArray )
    {
        for ( i = 0; i < ulArraySize; i++ )
        {
            CcspCwmpCleanParamInfo((&pParamInfoArray[i]));
        }   

        AnscFreeMemory(pParamInfoArray);
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
    }
}


static
void ssp_getTr069DataTypeString(int type, char *pBuf)
{
    errno_t rc = -1;
    switch ( type )
    {
        case    CCSP_CWMP_TR069_DATA_TYPE_String:
                rc = strcpy_s(pBuf,MAX_DATA_TYPR_LEN, "String");
                ERR_CHK(rc);
                break;

        case    CCSP_CWMP_TR069_DATA_TYPE_Int:
                rc = strcpy_s(pBuf,MAX_DATA_TYPR_LEN, "Int");
                ERR_CHK(rc);
                break;

        case    CCSP_CWMP_TR069_DATA_TYPE_UnsignedInt:
                rc = strcpy_s(pBuf,MAX_DATA_TYPR_LEN, "UnsignedInt");
                ERR_CHK(rc);
                break;

        case    CCSP_CWMP_TR069_DATA_TYPE_Boolean:
                rc = strcpy_s(pBuf,MAX_DATA_TYPR_LEN,"Boolean");
                ERR_CHK(rc);
                break;

        case    CCSP_CWMP_TR069_DATA_TYPE_DateTime:
                rc = strcpy_s(pBuf,MAX_DATA_TYPR_LEN,"DateTime");
                ERR_CHK(rc);
                break;

        case    CCSP_CWMP_TR069_DATA_TYPE_Base64:
                rc = strcpy_s(pBuf,MAX_DATA_TYPR_LEN,"Base64");
                ERR_CHK(rc);
                break;

        default:

                rc = strcpy_s(pBuf,MAX_DATA_TYPR_LEN,"Unknown");
                ERR_CHK(rc);
                break;
    }
}


void ssp_testGPV()
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )g_pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_PARAM_VALUE          pParamValueArray   = (PCCSP_CWMP_PARAM_VALUE     )NULL;
    ULONG                           ulArraySize        = (ULONG                      )0;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ULONG                           i                  = 0;
    ANSC_STATUS                     returnStatus;
    char                            ns[256];
    ULONG                           ulMaxCount         = 10240;
    PSLAP_STRING_ARRAY              pParamNameArray    = NULL;
    PSLAP_VARIABLE                  pSlapVar;
    char                            dataType[32];
    errno_t rc = -1;

    printf("Input namespace: ");
    fflush(stdin);
    while(getc(stdin) != '\n') ;
    fgets(ns, 256, stdin);

    ssp_remove_trailing_CRLF(ns);
    if ( ns[0] == 0 )
    {
        printf("Namespace 'Device.' is used ...\n");
        rc =strcpy_s(ns,sizeof(ns),"Device.");
        ERR_CHK(rc);
    }

    pParamNameArray = (PSLAP_STRING_ARRAY)SlapVhoAllocStringArray2((ANSC_HANDLE)NULL, 1);
    if ( !pParamNameArray )
    {
        printf("Out of resources!\n");
        return;
    }
    pParamNameArray->Array.arrayString[0] = AnscCloneString(ns);

    returnStatus =
        pCcspCwmpMpaIf->GetParameterValues
        (
            pCcspCwmpMpaIf->hOwnerContext,
            pParamNameArray,
            ulMaxCount,
            (void**)&pParamValueArray,
            &ulArraySize,
            (ANSC_HANDLE*)&pCwmpSoapFault,
            s_bExcludeInvNamespaces
        );

    printf("GPV returns code %d.\n", (int)returnStatus);
    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        printf("--> GPV returns %u entries.\n", (unsigned int)ulArraySize);
        for ( i = 0; i < ulArraySize; i++ )
        {
            pSlapVar = pParamValueArray[i].Value;
            ssp_getTr069DataTypeString(pParamValueArray[i].Tr069DataType, dataType);
            printf
                (
                    " Name <%s>\n    Type <%s>\n    Value <%s>\n", 
                    pParamValueArray[i].Name, 
                    dataType,
                    pSlapVar->Variant.varString
                );
        }
        printf("GPV returns %u entries <--\n", (unsigned int)ulArraySize);
    }

    if ( pParamValueArray )
    {
        for ( i = 0; i < ulArraySize; i++ )
        {
            CcspCwmpCleanParamValue((&pParamValueArray[i]));
        }   

        AnscFreeMemory(pParamValueArray);
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
    }

    SlapFreeVarArray(pParamNameArray);
}


static void ssp_testSPV_withWriteID(ULONG ulWriteID)
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )g_pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    CCSP_CWMP_PARAM_VALUE           pv                 = {0};
    PCCSP_CWMP_PARAM_VALUE          pParamValueArray   = (PCCSP_CWMP_PARAM_VALUE     )&pv;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ANSC_STATUS                     returnStatus;
    char                            ns[256];
    char                            dataType[32];
    int                             iStatus; 
    char                            value[512];
    SLAP_VARIABLE                   slapVar            = {0};

    printf("SPV - writeID = %u.\n", (unsigned int)ulWriteID);

    printf("Input namespace: ");
    fflush(stdin);
    while(getc(stdin) != '\n') ;
    fgets(ns, 256, stdin);

    ssp_remove_trailing_CRLF(ns);
    if ( ns[0] == 0 )
    {
        printf("Invalid namespace, please try again!\n");
        return;
    }

    pv.Name = ns;

    printf("Input data type (1-String,2-Int,3-Uint,4-Boolean,5-DateTime,6-Base64): ");
    fflush(stdin);
    fgets(dataType, 32, stdin);

    ssp_remove_trailing_CRLF(dataType);

    pv.Tr069DataType = _ansc_atoi(dataType);
    if ( pv.Tr069DataType <= CCSP_CWMP_TR069_DATA_TYPE_Unspecified || pv.Tr069DataType > CCSP_CWMP_TR069_DATA_TYPE_Base64 )
    {
        pv.Tr069DataType = CCSP_CWMP_TR069_DATA_TYPE_String;
    }

    printf("Input value: ");
    fflush(stdin);
    fgets(value, 512, stdin);

    ssp_remove_trailing_CRLF(value);
    slapVar.Syntax = SLAP_VAR_SYNTAX_string;
    slapVar.Variant.varString = value;
    pv.Value = &slapVar;

    returnStatus =
        pCcspCwmpMpaIf->SetParameterValuesWithWriteID
        (
            pCcspCwmpMpaIf->hOwnerContext,
            pParamValueArray,
            1,
            ulWriteID,
            &iStatus,
            (ANSC_HANDLE*)&pCwmpSoapFault,
            s_bExcludeInvNamespaces
        );

    printf("SPV returns code %d.\n", (int)returnStatus);
    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        printf("SPV returns status code %d.\n", iStatus);
    }
    else if ( pCwmpSoapFault )
    {
        printf("SPV returns fault code %s.\n", pCwmpSoapFault->soap_faultcode); 
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
    }
}


void ssp_testSPV()
{
    ssp_testSPV_withWriteID(CCSP_TR069PA_WRITE_ID);
}


void ssp_testSPV_notFromPA()
{
    ssp_testSPV_withWriteID(~CCSP_TR069PA_WRITE_ID);
}


void ssp_testGPA()
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )g_pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_PARAM_ATTRIB         pParamAttrArray    = (PCCSP_CWMP_PARAM_ATTRIB    )NULL;
    ULONG                           ulArraySize        = (ULONG                      )0;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ULONG                           i                  = 0;
    ANSC_STATUS                     returnStatus;
    char                            ns[256];
    ULONG                           ulMaxCount         = 4096;
    PSLAP_STRING_ARRAY              pParamNameArray    = NULL;
    errno_t rc       = -1;

    printf("Input namespace: ");
    fflush(stdin);
    while(getc(stdin) != '\n') ;
    fgets(ns, 256, stdin);

    ssp_remove_trailing_CRLF(ns);
    if ( ns[0] == 0 )
    {
        printf("Namespace 'Device.' is used ...\n");
        rc = strcpy_s(ns,sizeof(ns),"Device.");
        ERR_CHK(rc);
    }

    pParamNameArray = (PSLAP_STRING_ARRAY)SlapVhoAllocStringArray2((ANSC_HANDLE)NULL, 1);
    if ( !pParamNameArray )
    {
        printf("Out of resources!\n");
        return;
    }
    pParamNameArray->Array.arrayString[0] = AnscCloneString(ns);

    returnStatus =
        pCcspCwmpMpaIf->GetParameterAttributes
        (
            pCcspCwmpMpaIf->hOwnerContext,
            pParamNameArray,
            ulMaxCount,
            (void **)&pParamAttrArray,
            &ulArraySize,
            (ANSC_HANDLE*)&pCwmpSoapFault,
            s_bExcludeInvNamespaces
        );

    printf("GPA returns code %d.\n", (int)returnStatus);
    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        printf("GPA returns %u entries.\n", (unsigned int)ulArraySize);
        for ( i = 0; i < ulArraySize; i++ )
        {
            printf
                (
                    " Name<%s>\n    Notification <%d>\n    AccessList <%s>\n", 
                    pParamAttrArray[i].Name, 
                    pParamAttrArray[i].Notification,
                    pParamAttrArray[i].AccessList
                );
        }
    }

    if ( pParamAttrArray )
    {
        for ( i = 0; i < ulArraySize; i++ )
        {
            CcspCwmpCleanParamAttrib((&pParamAttrArray[i]));
        }   

        AnscFreeMemory(pParamAttrArray);
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
    }

    SlapFreeVarArray(pParamNameArray);
}


void ssp_testSPA()
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )g_pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    CCSP_CWMP_SET_PARAM_ATTRIB      spa                = {0};
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ANSC_STATUS                     returnStatus;
    char                            ns[MAX_NS_AL_LEN];
    char                            c;
    BOOL                            bChangeNotif       = FALSE;
    BOOL                            bChangeAL          = FALSE;
    char                            al[MAX_NS_AL_LEN]; /*RDKB-7336, CID-33191, increasing the memory to avoid out of bound access*/

    printf("Input namespace: ");
    fflush(stdin);
    while(getc(stdin) != '\n') ;
    fgets(ns, MAX_NS_AL_LEN, stdin);

    ssp_remove_trailing_CRLF(ns);
    if ( ns[0] == 0 )
    {
        printf("Invalid namespace, please try again!\n");
        return;
    }

    spa.Name = ns;

    printf("Change Notification (y/n): ");
    fflush(stdin);
    c = getchar();
    while(getc(stdin) != '\n') ;
    bChangeNotif = (c == 'y' || c == 'Y');
    spa.bNotificationChange = bChangeNotif;

    if ( bChangeNotif )
    {
        printf("Notification (0/1/2): ");
        fflush(stdin);
        c = getchar();
        spa.Notification = (c=='2')?CCSP_CWMP_NOTIFICATION_active:(c=='1')?CCSP_CWMP_NOTIFICATION_passive:CCSP_CWMP_NOTIFICATION_off;
        while(getc(stdin) != '\n') ;
    }

    printf("Change AccessList (y/n): ");
    fflush(stdin);
    c = getchar();
    bChangeAL = (c == 'y' || c == 'Y');
    spa.bAccessListChange = bChangeAL;
    while(getc(stdin) != '\n') ;

    if ( bChangeAL )
    {
        printf("Input AccessList: ");
        fflush(stdin);
        fgets(al, MAX_NS_AL_LEN, stdin);

        ssp_remove_trailing_CRLF(ns);
        if ( al[0] == 0 )
        {
            spa.AccessList = NULL;
        }
        else
        {
            spa.AccessList = al;
        }
    }
    returnStatus =
        pCcspCwmpMpaIf->SetParameterAttributes
        (
            pCcspCwmpMpaIf->hOwnerContext,
            &spa,
            1,
            (ANSC_HANDLE*)&pCwmpSoapFault,
            s_bExcludeInvNamespaces
        );

    printf("SPA returns code %d.\n", (int)returnStatus);
    if ( returnStatus != ANSC_STATUS_SUCCESS && pCwmpSoapFault )
    {
        printf("SPA returns fault code %s.\n", pCwmpSoapFault->soap_faultcode); 
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
    }
}


void ssp_testCDS()
{
    printf("Not implemented yet!\n");
}


void ssp_testFirmwareDownload()
{
    printf("Not implemented yet!\n");
}


void ssp_testGetMethods()
{
    printf("Not implemented yet!\n");
}


void ssp_testAddObject()
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )g_pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ANSC_STATUS                     returnStatus;
    char                            ns[256];
    ULONG                           ulObjInstance      = 0;
    int                             iStatus            = 0;

    printf("Input namespace: ");
    fflush(stdin);
    while(getc(stdin) != '\n') ;
    fgets(ns, 256, stdin);

    ssp_remove_trailing_CRLF(ns);
    if ( ns[0] == 0 )
    {
        printf("Invalid namespace, please try again!\n");
        return;
    }

    returnStatus =
        pCcspCwmpMpaIf->AddObject
        (
            pCcspCwmpMpaIf->hOwnerContext,
            ns,
            &ulObjInstance,
            &iStatus,
            (ANSC_HANDLE*)&pCwmpSoapFault,
            s_bExcludeInvNamespaces
        );

    printf("AddObject returns code %d.\n", (int)returnStatus);
    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        printf("AddObject returns status code %d.\n", iStatus);
    }
    else if ( pCwmpSoapFault )
    {
        printf("AddObject returns fault code %s.\n", pCwmpSoapFault->soap_faultcode);
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
    }
}


void ssp_testDeleteObject()
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )g_pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_MPA_INTERFACE             pCcspCwmpMpaIf         = (PCCSP_CWMP_MPA_INTERFACE        )pCcspCwmpProcessor->hCcspCwmpMpaIf;
    PCCSP_CWMP_SOAP_FAULT           pCwmpSoapFault     = (PCCSP_CWMP_SOAP_FAULT      )NULL;
    ANSC_STATUS                     returnStatus;
    char                            ns[256];
    int                             iStatus            = 0;

    printf("Input namespace: ");
    fflush(stdin);
    while(getc(stdin) != '\n') ;
    fgets(ns, 256, stdin);

    ssp_remove_trailing_CRLF(ns);
    if ( ns[0] == 0 )
    {
        printf("Invalid namespace, please try again!\n");
        return;
    }

    returnStatus =
        pCcspCwmpMpaIf->DeleteObject
        (
            pCcspCwmpMpaIf->hOwnerContext,
            ns,
            &iStatus,
            (ANSC_HANDLE *)&pCwmpSoapFault,
            s_bExcludeInvNamespaces
        );

    printf("DeleteObject returns code %d.\n", (int)returnStatus);
    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        printf("DeleteObject returns status code %d.\n", iStatus);
    }
    else if ( pCwmpSoapFault )
    {
        printf("DeleteObject returns fault code %s.\n", pCwmpSoapFault->soap_faultcode);
    }

    if ( pCwmpSoapFault )
    {
        CcspCwmpFreeSoapFault(pCwmpSoapFault);
    }
}


void ssp_checkCachedParamAttr()
{
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor  = (PCCSP_CWMP_PROCESSOR_OBJECT )g_pCcspCwmpCpeController->hCcspCwmpProcessor;
    char                            ns[256];
    ULONG                           ulNotification;

    printf("Input namespace: ");
    fflush(stdin);
    while(getc(stdin) != '\n') ;
    fgets(ns, 256, stdin);

    ssp_remove_trailing_CRLF(ns);
    if ( ns[0] == 0 )
    {
        printf("Invalid namespace, please try again!\n");
        return;
    }

    ulNotification =
        pCcspCwmpProcessor->CheckParamAttrCache
            (
                (ANSC_HANDLE)pCcspCwmpProcessor,
                ns
            );

    printf("Cached notification is %u.\n", (unsigned int)ulNotification);
}


void ssp_flipInvNamespaceChkFlag()
{
    s_bExcludeInvNamespaces = !s_bExcludeInvNamespaces;

    printf("Invisible namespace tree will %s be checked!\n", s_bExcludeInvNamespaces ? "NOT" : "");
}


void ssp_sendValueChangeSignal()
{
    char                            ns[MAX_NS_AL_LEN];
    char                            oldValue[MAX_VALUE_ARRAY_LEN]      = {0};
    char                            newValue[MAX_VALUE_ARRAY_LEN]      = {0};
    parameterSigStruct_t            vcSig              = {0};
    char                            dataType[MAX_DATA_TYPR_LEN]       = {0};
    int                             ret;
    void*                           hBusHandle         = g_pCcspCwmpCpeController->hMsgBusHandle;

    printf("Input namespace whose value has changed: ");
    fflush(stdin);
    while(getc(stdin) != '\n') ;
    fgets(ns, MAX_NS_AL_LEN, stdin);

    ssp_remove_trailing_CRLF(ns);
    if ( ns[0] == 0 )
    {
        printf("Invalid namespace, please try again!\n");
        return;
    }

    vcSig.parameterName = ns;

    printf("New value: ");
    fflush(stdin);
    fgets(newValue, (MAX_VALUE_ARRAY_LEN-1), stdin);

    ssp_remove_trailing_CRLF(newValue);
    
    vcSig.newValue = newValue[0] ? newValue : NULL;
    
    printf("old value: ");
    fflush(stdin);
    fgets(oldValue, (MAX_VALUE_ARRAY_LEN-1), stdin);

    ssp_remove_trailing_CRLF(oldValue);
    
    vcSig.oldValue = oldValue[0] ? oldValue : NULL;

    printf("Input data type (1-String,2-Int,3-Uint,4-Boolean,5-DateTime,6-Base64): ");
    fflush(stdin);
    fgets(dataType, MAX_DATA_TYPR_LEN, stdin); /*RDKB-7336, CID-32893, defining maximum length for NS and AL*/

    ssp_remove_trailing_CRLF(dataType);

    vcSig.type = _ansc_atoi(dataType) - 1;
    if ( vcSig.type >= ccsp_base64 ) vcSig.type = ccsp_string;

    if ( g_Subsystem[0] )
    {
        vcSig.subsystem_prefix = g_Subsystem;
    }

    vcSig.writeID = (UINT)-1;

    ret =
        CcspBaseIf_SendparameterValueChangeSignal
            (
                hBusHandle,
                &vcSig,
                1
            );

    if ( ret != CCSP_SUCCESS )
    {
        printf("parameterValueChangedSignal was NOT sent out correctly!\n");
    }
    else
    {
        printf("parameterValueChangedSignal has been sent on bus.\n");
    }
}


void ssp_sendDiagCompleteSignal()
{
    int                             ret;
    void*                           hBusHandle         = g_pCcspCwmpCpeController->hMsgBusHandle;

    ret = CcspBaseIf_SenddiagCompleteSignal(hBusHandle);

    if ( ret != CCSP_SUCCESS )
    {
        printf("diagCompleteSignal was NOT sent out correctly!\n");
    }
    else
    {
        printf("diagCompleteSignal has been sent on bus.\n");
    }
}


#endif

