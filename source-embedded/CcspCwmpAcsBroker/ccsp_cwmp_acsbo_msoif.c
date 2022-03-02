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

    module:	ccsp_cwmp_acsbo_msoif.c

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This module implements the advanced interface functions
        of the CCSP CWMP Acs Broker Object.

        *   CcspCwmpAcsboMsoGetRpcMethods
        *   CcspCwmpAcsboMsoInform
        *   CcspCwmpAcsboMsoTransferComplete
        *   CcspCwmpAcsboMsoAutonomousTransferComplete
        *   CcspCwmpAcsboMsoDuStateChangeComplete
        *   CcspCwmpAcsboMsoAutonomousDuStateChangeComplete
        *   CcspCwmpAcsboMsoKicked
        *   CcspCwmpAcsboMsoRequestDownload
        *   CcspCwmpAcsboMsoValueChanged

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang    Quan

    ---------------------------------------------------------------

    revision:

        09/30/05    initial revision.
        01/04/2011  support AutonomousTransferComplete,
                    DuStateChangeComplete, and 
                    AutonomousDuStateChangeComplete

**********************************************************************/


#include "ccsp_cwmp_acsbo_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboMsoGetRpcMethods
            (
                ANSC_HANDLE                 hThisObject,
                char**                      ppMethodList,
                int*                        pNumOfMethods
            );

    description:

        This function is called to discover the set of methods
        supported by the Server or CPE it is in communication with.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char**                      ppMethodList
                Returns the array of strings containing the names of
                each of the RPC methods.

                int*                        pNumOfMethods
                Returns the number of methods.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcsboMsoGetRpcMethods
    (
        ANSC_HANDLE                 hThisObject,
        SLAP_STRING_ARRAY**         ppMethodList
    )
{
    ANSC_STATUS                      returnStatus           = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_BROKER_OBJECT     pMyObject              = (PCCSP_CWMP_ACS_BROKER_OBJECT    )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor     = (PCCSP_CWMP_PROCESSOR_OBJECT )pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession       = (PCCSP_CWMP_SESSION_OBJECT   )NULL;
    ULONG                            ulErrorCode            = (ULONG                      )0;

    *ppMethodList  = NULL;

    /*
     * The target WmpSession is the first available session...
     */
    pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession((ANSC_HANDLE)pCcspCwmpProcessor);

    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }
    else
    {
        pCcspCwmpProcessor->RelWmpSession((ANSC_HANDLE)pCcspCwmpProcessor, (ANSC_HANDLE)pCcspCwmpSession);
    }

    returnStatus =
        pCcspCwmpSession->GetRpcMethods
            (
                (ANSC_HANDLE)pCcspCwmpSession,
                ppMethodList,
                &ulErrorCode
            );

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboMsoInform
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pEventCode,
                char*                       pCommandKey,
                BOOL                        bConnectNow
            );

    description:

        This function is called to initiate a transaction sequence
        whenever a connection to an ACS is established.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pEventCode
                Specifies the event that caused the transaction session
                to be established.

                char*                       pCommandKey
                Specifies the value of the CommandKey was passed to the
                CPE by ACS in the previous request.

                BOOL                        bConnectNow
                Specifies whether a connection to the ACS should be
                established right now.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcsboMsoInform
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pEventCode,
        char*                       pCommandKey,
        BOOL                        bConnectNow
    )
{
    ANSC_STATUS                      returnStatus           = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_BROKER_OBJECT     pMyObject              = (PCCSP_CWMP_ACS_BROKER_OBJECT    )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor     = (PCCSP_CWMP_PROCESSOR_OBJECT     )pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession       = (PCCSP_CWMP_SESSION_OBJECT       )NULL;
    PCCSP_CWMP_EVENT            pCcspCwmpEvent     = (PCCSP_CWMP_EVENT           )NULL;
    errno_t rc = -1;
    int ind = -1;

    /*
     * The target WmpSession must be inactive...
     */
    pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession2((ANSC_HANDLE)pCcspCwmpProcessor);

    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }
    else
    {
        pCcspCwmpEvent = (PCCSP_CWMP_EVENT)AnscAllocateMemory(sizeof(CCSP_CWMP_EVENT));
    }

    if ( !pCcspCwmpEvent )
    {
        returnStatus = ANSC_STATUS_RESOURCES;

        goto  EXIT1;
    }
    else
    {
        pCcspCwmpEvent->EventCode  = AnscCloneString(pEventCode );
        pCcspCwmpEvent->CommandKey = pCommandKey ? AnscCloneString(pCommandKey) : NULL;

        /* Per TR-069 specification, if "0 BOOTSTRAP" event is included, all undelivered 
         * events must be discarded 
         */
        rc =strcasecmp_s(CCSP_CWMP_INFORM_EVENT_NAME_Bootstrap,strlen(CCSP_CWMP_INFORM_EVENT_NAME_Bootstrap),pEventCode,&ind);
        ERR_CHK(rc);
        if((!ind) && (rc == EOK))
        {
            pCcspCwmpSession->DelAllEvents
                (
                    (ANSC_HANDLE)pCcspCwmpSession
                );
        }
    }

    returnStatus =
        pCcspCwmpSession->AddCwmpEvent
            (
                (ANSC_HANDLE)pCcspCwmpSession,
                (ANSC_HANDLE)pCcspCwmpEvent,
                bConnectNow
            );

    goto  EXIT1;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    pCcspCwmpProcessor->RelWmpSession((ANSC_HANDLE)pCcspCwmpProcessor, (ANSC_HANDLE)pCcspCwmpSession);

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboMsoTransferComplete
            (
                ANSC_HANDLE                 hThisObject,
        		BOOL						bIsDownload,
                char*                       pCommandKey,
                ANSC_HANDLE                 hFault,
                ANSC_HANDLE                 hStartTime,
                ANSC_HANDLE                 hCompleteTime,
                BOOL                        bNewSession
            );

    description:

        This function is called to inform the server of the completion
        of a file transfer initiated by an earlier Download or Upload
        method call.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

				BOOL						bIsDownload,
				It's download or upload;

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

                BOOL                        bNewSession
                Specifies if a new CWMP session shall be created to 
                invoke this RPC.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcsboMsoTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        char*                       pCommandKey,
        ANSC_HANDLE                 hFault,
        ANSC_HANDLE                 hStartTime,
        ANSC_HANDLE                 hCompleteTime,
        BOOL                        bNewSession
    )
{
    ANSC_STATUS                      returnStatus           = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_BROKER_OBJECT     pMyObject              = (PCCSP_CWMP_ACS_BROKER_OBJECT    )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor     = (PCCSP_CWMP_PROCESSOR_OBJECT )pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession       = (PCCSP_CWMP_SESSION_OBJECT   )NULL;
    ULONG                            ulErrorCode            = (ULONG                      )0;

    /*
     * The target WmpSession is either active or idle...
     */
    if ( bNewSession )
    {
        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession3((ANSC_HANDLE)pCcspCwmpProcessor);
    }
    else
    {
        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession((ANSC_HANDLE)pCcspCwmpProcessor);
    }

    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }
    else
    {
        pCcspCwmpProcessor->RelWmpSession((ANSC_HANDLE)pCcspCwmpProcessor, (ANSC_HANDLE)pCcspCwmpSession);
    }

    returnStatus =
        pCcspCwmpSession->TransferComplete
            (
                (ANSC_HANDLE)pCcspCwmpSession,
				bIsDownload,
                pCommandKey,
                hFault,
                hStartTime,
                hCompleteTime,
                &ulErrorCode,
                bNewSession
            );

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboMsoAutonomousTransferComplete
            (
                ANSC_HANDLE                 hThisObject,
		        BOOL						bIsDownload,
                char*                       AnnounceURL,
                char*                       TransferURL,
                char*                       FileType,
                unsigned int                FileSize,
                char*                       TargetFileName,
                ANSC_HANDLE                 hFault,      
                ANSC_HANDLE                 hStartTime,  
                ANSC_HANDLE                 hCompleteTime,
                BOOL                        bNewSession
            );

    description:

        This function is called to inform the server of the completion
        of a file transfer initiated by an earlier Download or Upload
        method call.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

				BOOL						bIsDownload,
				It's download or upload;

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

                BOOL                        bNewSession
                Specifies if a new CWMP session shall be created to 
                invoke this RPC.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcsboMsoAutonomousTransferComplete
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        char*                       AnnounceURL,
        char*                       TransferURL,
        char*                       FileType,
        unsigned int                FileSize,
        char*                       TargetFileName,
        ANSC_HANDLE                 hFault,      
        ANSC_HANDLE                 hStartTime,  
        ANSC_HANDLE                 hCompleteTime,
        BOOL                        bNewSession
    )
{
    ANSC_STATUS                      returnStatus           = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_BROKER_OBJECT     pMyObject              = (PCCSP_CWMP_ACS_BROKER_OBJECT    )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor     = (PCCSP_CWMP_PROCESSOR_OBJECT )pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession       = (PCCSP_CWMP_SESSION_OBJECT   )NULL;
    ULONG                            ulErrorCode            = (ULONG                      )0;

    /*
     * The target WmpSession is either active or idle...
     */
    if ( bNewSession )
    {
        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession3((ANSC_HANDLE)pCcspCwmpProcessor);
    }
    else
    {
        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession((ANSC_HANDLE)pCcspCwmpProcessor);
    }

    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }
    else
    {
        pCcspCwmpProcessor->RelWmpSession((ANSC_HANDLE)pCcspCwmpProcessor, (ANSC_HANDLE)pCcspCwmpSession);
    }

    returnStatus =
        pCcspCwmpSession->AutonomousTransferComplete
            (
                (ANSC_HANDLE)pCcspCwmpSession,
				bIsDownload,
                hFault,
                hStartTime,
                hCompleteTime,
                AnnounceURL,
                TransferURL,
                FileType,
                FileSize,
                TargetFileName,
                &ulErrorCode,
                bNewSession
            );

    return  returnStatus;
}



/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboMsoDuStateChangeComplete
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hDsccReq,
                BOOL                        bNewSession
            );

    description:

        This function is called to inform the server of the completion
        of software package management operations initiated by an earlier 
        ChangeDUState method call.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hDsccReq
                DSCC request.

                BOOL                        bNewSession
                Specifies if a new CWMP session shall be created to 
                invoke this RPC.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcsboMsoDuStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hDsccReq,
        BOOL                        bNewSession
    )
{
    ANSC_STATUS                      returnStatus           = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_BROKER_OBJECT     pMyObject              = (PCCSP_CWMP_ACS_BROKER_OBJECT    )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor     = (PCCSP_CWMP_PROCESSOR_OBJECT )pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession       = (PCCSP_CWMP_SESSION_OBJECT   )NULL;
    ULONG                            ulErrorCode            = (ULONG                      )0;

    /*
     * The target WmpSession is either active or idle...
     */
    if ( bNewSession )
    {
        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession3((ANSC_HANDLE)pCcspCwmpProcessor);
    }
    else
    {
        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession((ANSC_HANDLE)pCcspCwmpProcessor);
    }

    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }
    else
    {
        pCcspCwmpProcessor->RelWmpSession((ANSC_HANDLE)pCcspCwmpProcessor, (ANSC_HANDLE)pCcspCwmpSession);
    }

    returnStatus =
        pCcspCwmpSession->DUStateChangeComplete
            (
                (ANSC_HANDLE)pCcspCwmpSession,
                hDsccReq,
                &ulErrorCode,
                bNewSession
            );

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboMsoAutonomousDuStateChangeComplete
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hAdsccReq,
                BOOL                        bNewSession
            );

    description:

        This function is called to inform the server of the completion
        of software package management operations initiated by an earlier 
        ChangeDUState method call.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pAdsccResults
                XML content of autonomous DU state change complete results
                as defined by PD-194.

                BOOL                        bNewSession
                Specifies if a new CWMP session shall be created to 
                invoke this RPC.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcsboMsoAutonomousDuStateChangeComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hAdsccReq,
        BOOL                        bNewSession
    )
{
    ANSC_STATUS                      returnStatus           = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_BROKER_OBJECT     pMyObject              = (PCCSP_CWMP_ACS_BROKER_OBJECT    )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor     = (PCCSP_CWMP_PROCESSOR_OBJECT )pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession       = (PCCSP_CWMP_SESSION_OBJECT   )NULL;
    ULONG                            ulErrorCode            = (ULONG                      )0;

    /*
     * The target WmpSession is either active or idle...
     */
    if ( bNewSession )
    {
        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession3((ANSC_HANDLE)pCcspCwmpProcessor);
    }
    else
    {
        pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession((ANSC_HANDLE)pCcspCwmpProcessor);
    }

    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }
    else
    {
        pCcspCwmpProcessor->RelWmpSession((ANSC_HANDLE)pCcspCwmpProcessor, (ANSC_HANDLE)pCcspCwmpSession);
    }

    returnStatus =
        pCcspCwmpSession->AutonomousDUStateChangeComplete
            (
                (ANSC_HANDLE)pCcspCwmpSession,
                hAdsccReq,
                &ulErrorCode,
                bNewSession
            );

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboMsoKicked
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pCommand,
                char*                       pReferer,
                char*                       pArg,
                char*                       pNext,
                char**                      ppNextUrl
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

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcsboMsoKicked
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCommand,
        char*                       pReferer,
        char*                       pArg,
        char*                       pNext,
        char**                      ppNextUrl
    )
{
    ANSC_STATUS                      returnStatus           = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_BROKER_OBJECT     pMyObject              = (PCCSP_CWMP_ACS_BROKER_OBJECT    )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor     = (PCCSP_CWMP_PROCESSOR_OBJECT )pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession       = (PCCSP_CWMP_SESSION_OBJECT   )NULL;
    ULONG                            ulErrorCode            = (ULONG                      )0;

    /*
     * The target WmpSession is either active or idle...
     */
    pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession3((ANSC_HANDLE)pCcspCwmpProcessor);

    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }
    else
    {
        pCcspCwmpProcessor->RelWmpSession((ANSC_HANDLE)pCcspCwmpProcessor, (ANSC_HANDLE)pCcspCwmpSession);
    }

    returnStatus =
        pCcspCwmpSession->Kicked
            (
                (ANSC_HANDLE)pCcspCwmpSession,
                pCommand,
                pReferer,
                pArg,
                pNext,
                ppNextUrl,
                &ulErrorCode
            );

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboMsoRequestDownload
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pFileType,
                ANSC_HANDLE                 hFileTypeArgArray,
                ULONG                       ulArraySize
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

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcsboMsoRequestDownload
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pFileType,
        ANSC_HANDLE                 hFileTypeArgArray,
        ULONG                       ulArraySize
    )
{
    ANSC_STATUS                      returnStatus           = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_BROKER_OBJECT     pMyObject              = (PCCSP_CWMP_ACS_BROKER_OBJECT    )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor     = (PCCSP_CWMP_PROCESSOR_OBJECT )pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession       = (PCCSP_CWMP_SESSION_OBJECT   )NULL;
    ULONG                            ulErrorCode            = (ULONG                      )0;

    /*
     * The target WmpSession is the first available session...
     */
    pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession((ANSC_HANDLE)pCcspCwmpProcessor);

    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }
    else
    {
        pCcspCwmpProcessor->RelWmpSession((ANSC_HANDLE)pCcspCwmpProcessor, (ANSC_HANDLE)pCcspCwmpSession);
    }

    returnStatus =
        pCcspCwmpSession->RequestDownload
            (
                (ANSC_HANDLE)pCcspCwmpSession,
                pFileType,
                hFileTypeArgArray,
                ulArraySize,
                &ulErrorCode
            );

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcsboMsoValueChanged
            (
                ANSC_HANDLE                 hThisObject,
                char*                       pParamName,
                char*                       pParamValue,
                ULONG                       CwmpDataType,
                BOOL                        bConnectNow
            );

    description:

        This function is called by underlying modules to indicate
        the value of the specified parameter has been modified.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                char*                       pParamName
                Specifies the name of the parameter whose value has
                just been modified.

                char*                       pParamValue
                Specifies the value of the parameter.

                ULONG                       CwmpDataType
                Specifies the parameter type.

                BOOL                        bConnectNow
                Specifies whether a connection to the ACS should be
                established right now.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcsboMsoValueChanged
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        char*                       pParamValue,
        ULONG                       CwmpDataType,
        BOOL                        bConnectNow
    )
{
    ANSC_STATUS                      returnStatus           = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_ACS_BROKER_OBJECT     pMyObject              = (PCCSP_CWMP_ACS_BROKER_OBJECT    )hThisObject;
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT pCcspCwmpCpeController = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT)pMyObject->hCcspCwmpCpeController;
    PCCSP_CWMP_PROCESSOR_OBJECT      pCcspCwmpProcessor     = (PCCSP_CWMP_PROCESSOR_OBJECT )pCcspCwmpCpeController->hCcspCwmpProcessor;
    PCCSP_CWMP_SESSION_OBJECT        pCcspCwmpSession       = (PCCSP_CWMP_SESSION_OBJECT   )NULL;

    /*
     * The target WmpSession must be inactive...
     */
    pCcspCwmpSession = (PCCSP_CWMP_SESSION_OBJECT)pCcspCwmpProcessor->AcqWmpSession2((ANSC_HANDLE)pCcspCwmpProcessor);

    if ( !pCcspCwmpSession )
    {
        return  ANSC_STATUS_INTERNAL_ERROR;
    }

    if ( bConnectNow )
    {
        /* check if delay inform is scheduled or session has being retried, 
         * if so simply add the event into queue 
         */

        if ( ( pCcspCwmpCpeController->bBootInformScheduled || pCcspCwmpCpeController->bBootstrapInformScheduled ) &&
             !pCcspCwmpCpeController->bDelayedInformCancelled )
        {
            bConnectNow = FALSE;
        }
        else
        {
            bConnectNow = (pCcspCwmpSession->RetryCount == 0);
        }
    }

    returnStatus =
        pCcspCwmpSession->AddModifiedParameter
            (
                (ANSC_HANDLE)pCcspCwmpSession,
                pParamName,
                pParamValue,
                CwmpDataType,
                bConnectNow && pCcspCwmpCpeController->bBootInformSent
            );

    goto  EXIT1;


    /******************************************************************
                GRACEFUL ROLLBACK PROCEDURES AND EXIT DOORS
    ******************************************************************/

EXIT1:

    pCcspCwmpProcessor->RelWmpSession((ANSC_HANDLE)pCcspCwmpProcessor, (ANSC_HANDLE)pCcspCwmpSession);

    return  returnStatus;
}


