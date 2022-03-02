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

    module:	ccsp_tr069_cds_definitions.h

        For supporting RPC ChangeDUState defined by TR-069
        Amemendment 3.

    ---------------------------------------------------------------

    description:

        Defines internal data structures relate to ChangeDUState
        RPC processing.

    ---------------------------------------------------------------

    environment:

        CCSP TR069 PA - platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan   

    ---------------------------------------------------------------

    revision:

        07/26/2011    initial revision.

    ---------------------------------------------------------------

    last modified:  
    
**********************************************************************/

#ifndef  _CCSP_TR069_CDS_DEF_
#define  _CCSP_TR069_CDS_DEF_


#define  CCSP_TR069_CDS_REQ_OBJECT_DELAY_SECONDS    300

/**********************************************************************
 * Data structures to convey parameters of RPCs - 
 *
 *      ChangeDUState
 *      DUStateChangeComplete
 *      AutonomousDUStateChangeComplete
 *
 **********************************************************************/

typedef 
enum _CCSP_TR069_CDS_OP
{
    CCSP_TR069_CDS_OP_Unknown  = 0,
    CCSP_TR069_CDS_OP_Install,
    CCSP_TR069_CDS_OP_Update,
    CCSP_TR069_CDS_OP_Uninstall
}
CCSP_TR069_CDS_OP, *PCCSP_TR069_CDS_OP;


/**********************************************************************
    RPC DUStateChange
 *********************************************************************/

typedef  
struct _CCSP_TR069_CDS_Install
{
    char*                           Url;
    char*                           Uuid;
    char*                           Username;
    char*                           Password;
    char*                           ExecEnvRef;     
}
CCSP_TR069_CDS_Install, *PCCSP_TR069_CDS_Install;


#define  CcspTr069CdsInstallClean(iop)                                                  \
    do {                                                                                \
        if ( (iop)->Url )                                                               \
            AnscFreeMemory((iop)->Url);                                                 \
        if ( (iop)->Uuid )                                                              \
            AnscFreeMemory((iop)->Uuid);                                                \
        if ( (iop)->Username )                                                          \
            AnscFreeMemory((iop)->Username);                                            \
        if ( (iop)->Password )                                                          \
            AnscFreeMemory((iop)->Password);                                            \
        if ( (iop)->ExecEnvRef )                                                        \
            AnscFreeMemory((iop)->ExecEnvRef);                                          \
    } while (0)


typedef  
struct _CCSP_TR069_CDS_Update
{
    char*                           Uuid;
    char*                           Version;
    char*                           Url;
    char*                           Username;
    char*                           Password;
}
CCSP_TR069_CDS_Update, *PCCSP_TR069_CDS_Update;


#define  CcspTr069CdsUpdateClean(uop)                                                   \
    do {                                                                                \
        if ( (uop)->Uuid )                                                              \
            AnscFreeMemory((uop)->Uuid);                                                \
        if ( (uop)->Version )                                                           \
            AnscFreeMemory((uop)->Version);                                             \
        if ( (uop)->Url )                                                               \
            AnscFreeMemory((uop)->Url);                                                 \
        if ( (uop)->Username )                                                          \
            AnscFreeMemory((uop)->Username);                                            \
        if ( (uop)->Password )                                                          \
            AnscFreeMemory((uop)->Password);                                            \
    } while (0)


typedef  
struct _CCSP_TR069_CDS_Uninstall
{
    char*                           Uuid;
    char*                           Version;
}
CCSP_TR069_CDS_Uninstall, *PCCSP_TR069_CDS_Uninstall;

#define  CcspTr069CdsUninstallClean(uiop)                                               \
    do {                                                                                \
        if ( (uiop)->Uuid )                                                             \
            AnscFreeMemory((uiop)->Uuid);                                               \
        if ( (uiop)->Version )                                                          \
            AnscFreeMemory((uiop)->Version);                                            \
    } while (0)


typedef  
struct _CCSP_TR069_CDS_Operation
{
    CCSP_TR069_CDS_OP                     Op;

    union {
        CCSP_TR069_CDS_Install          Install;
        CCSP_TR069_CDS_Update           Update;
        CCSP_TR069_CDS_Uninstall        Uninstall;
    } op;
}
CCSP_TR069_CDS_Operation, *PCCSP_TR069_CDS_Operation;


typedef  struct _CCSP_TR069_CDS_REQ
{
    int                             NumOperations;
    PCCSP_TR069_CDS_Operation       Operations;
    char*                           CommandKey;
}
CCSP_TR069_CDS_REQ, *PCCSP_TR069_CDS_REQ;


#define  CcspTr069CdsReqClean(CdsReq)                                                               \
    do {                                                                                            \
        int                         i;                                                              \
        if ( (CdsReq)->CommandKey )                                                                 \
            AnscFreeMemory((CdsReq)->CommandKey);                                                   \
        for ( i = 0; i < (CdsReq)->NumOperations; i ++ )                                            \
        {                                                                                           \
            if ( (CdsReq)->Operations[i].Op == CCSP_TR069_CDS_OP_Install )                          \
                CcspTr069CdsInstallClean(&((CdsReq)->Operations[i].op.Install));                    \
            else if ( (CdsReq)->Operations[i].Op == CCSP_TR069_CDS_OP_Update )                      \
                CcspTr069CdsUpdateClean(&((CdsReq)->Operations[i].op.Update));                      \
            else                                                                                    \
                CcspTr069CdsUninstallClean(&((CdsReq)->Operations[i].op.Uninstall));                \
        }                                                                                           \
    } while (0)


#define  CcspTr069CdsReqRemove(CdsReq)                                                              \
    do {                                                                                            \
        CcspTr069CdsReqClean(CdsReq);                                                               \
        AnscFreeMemory(CdsReq);                                                                     \
    } while (0)



typedef  
struct _CCSP_TR069_CDS_RESP
{
    unsigned int                    Dummy;
}
CCSP_TR069_CDS_RESP, *PCCSP_TR069_CDS_RESP;


/**********************************************************************
    RPC DUStateChangeComplete
 *********************************************************************/

typedef  
struct _CCSP_TR069_CDS_OpResult
{
    unsigned int                    FaultCode;
    char*                           FaultString;
    char*                           Uuid;
    char*                           DeploymentUnitRef;          
    char*                           Version;
    char*                           CurrentState;
    unsigned int                    Resolved;
    char*                           ExecutionUnitRefList;       
    char*                           StartTime;
    char*                           CompleteTime;
}
CCSP_TR069_CDS_OpResult, *PCCSP_TR069_CDS_OpResult;

#define  CcspTr069CdsOpResultClean(dscResult)                                           \
    do {                                                                                \
        if ( (dscResult)->FaultString )                                                 \
            AnscFreeMemory((dscResult)->FaultString);                                   \
        if ( (dscResult)->Uuid )                                                        \
            AnscFreeMemory((dscResult)->Uuid);                                          \
        if ( (dscResult)->DeploymentUnitRef )                                           \
            AnscFreeMemory((dscResult)->DeploymentUnitRef);                             \
        if ( (dscResult)->Version )                                                     \
            AnscFreeMemory((dscResult)->Version);                                       \
        if ( (dscResult)->CurrentState )                                                \
            AnscFreeMemory((dscResult)->CurrentState);                                  \
        if ( (dscResult)->ExecutionUnitRefList )                                        \
            AnscFreeMemory((dscResult)->ExecutionUnitRefList);                          \
        if ( (dscResult)->StartTime )                                                   \
            AnscFreeMemory((dscResult)->StartTime);                                     \
        if ( (dscResult)->CompleteTime )                                                \
            AnscFreeMemory((dscResult)->CompleteTime);                                  \
    } while (0)


typedef  
struct _CCSP_TR069_DSCC_REQ
{
    int                             NumResults;
    PCCSP_TR069_CDS_OpResult        Results;
    char*                           CommandKey;
}
CCSP_TR069_DSCC_REQ, *PCCSP_TR069_DSCC_REQ;


#define  CcspTr069DsccReqClean(DsccReq)                                                 \
    do {                                                                                \
        int                         i;                                                  \
        if ( (DsccReq)->CommandKey )                                                    \
            AnscFreeMemory((DsccReq)->CommandKey);                                      \
        for ( i = 0; i < (DsccReq)->NumResults; i ++ )                                  \
        {                                                                               \
            CcspTr069CdsOpResultClean(&((DsccReq)->Results[i]));                        \
        }                                                                               \
        if ( (DsccReq)->Results )                                                       \
        {                                                                               \
            AnscFreeMemory((DsccReq)->Results);                                         \
        }                                                                               \
    } while (0)

#define  CcspTr069DsccReqRemove(DsccReq)                                                \
    do {                                                                                \
        CcspTr069DsccReqClean(DsccReq);                                                 \
        AnscFreeMemory(DsccReq);                                                        \
    } while (0)


typedef  
struct _CCSP_TR069_DSCC_RESP
{
    unsigned int                    Dummy;
}
CCSP_TR069_DSCC_RESP, *PCCSP_TR069_DSCC_RESP;


/**********************************************************************
    RPC AutonomousDUStateChangeComplete
 *********************************************************************/

typedef  
struct _CCSP_TR069_ADSCC_OpResult
{
    unsigned int                    FaultCode;
    char*                           FaultString;
    char*                           Uuid;
    char*                           DeploymentUnitRef;      
    char*                           Version;
    char*                           OperationPerformed;
    char*                           CurrentState;
    unsigned int                    Resolved;
    char*                           ExecutionUnitRefList;   
    char*                           StartTime;
    char*                           CompleteTime;

    BOOL                            bFilteredOut;       /* indicates this result is not expected to be sent to ACS according to policy */
}
CCSP_TR069_ADSCC_OpResult, *PCCSP_TR069_ADSCC_OpResult;


#define  CcspTr069AdsccOpResultClean(adscResult)                                        \
    do {                                                                                \
        if ( (adscResult)->FaultString )                                                \
            AnscFreeMemory((adscResult)->FaultString);                                  \
        if ( (adscResult)->Uuid )                                                       \
            AnscFreeMemory((adscResult)->Uuid);                                         \
        if ( (adscResult)->DeploymentUnitRef )                                          \
            AnscFreeMemory((adscResult)->DeploymentUnitRef);                            \
        if ( (adscResult)->Version )                                                    \
            AnscFreeMemory((adscResult)->Version);                                      \
        if ( (adscResult)->OperationPerformed )                                         \
            AnscFreeMemory((adscResult)->OperationPerformed);                           \
        if ( (adscResult)->CurrentState )                                               \
            AnscFreeMemory((adscResult)->CurrentState);                                 \
        if ( (adscResult)->ExecutionUnitRefList )                                       \
            AnscFreeMemory((adscResult)->ExecutionUnitRefList);                         \
        if ( (adscResult)->StartTime )                                                  \
            AnscFreeMemory((adscResult)->StartTime);                                    \
        if ( (adscResult)->CompleteTime )                                               \
            AnscFreeMemory((adscResult)->CompleteTime);                                 \
    } while (0)



typedef  
struct _CCSP_TR069_ADSCC_REQ
{
    int                             NumResults;
    PCCSP_TR069_ADSCC_OpResult      Results;
}
CCSP_TR069_ADSCC_REQ, *PCCSP_TR069_ADSCC_REQ;


#define  CcspTr069AdsccReqClean(AdsccReq)                                               \
    do {                                                                                \
        int                         i;                                                  \
        for ( i = 0; i < (AdsccReq)->NumResults; i ++ )                                 \
        {                                                                               \
            CcspTr069AdsccOpResultClean(&((AdsccReq)->Results[i]));                     \
        }                                                                               \
        if ( (AdsccReq)->Results )                                                      \
        {                                                                               \
            AnscFreeMemory((AdsccReq)->Results);                                        \
        }                                                                               \
    } while (0)


#define  CcspTr069AdsccReqRemove(AdscReq)                                               \
    do {                                                                                \
        CcspTr069AdsccReqClean(AdscReq);                                                \
        AnscFreeMemory(AdscReq);                                                        \
    } while (0)


typedef  
struct _CCSP_TR069_ADSCC_RESP
{
    unsigned int                    Dummy;
}
CCSP_TR069_ADSCC_RESP, *PCCSP_TR069_ADSCC_RESP;


#endif  /* _CCSP_TR069_CDS_DEF_ */
