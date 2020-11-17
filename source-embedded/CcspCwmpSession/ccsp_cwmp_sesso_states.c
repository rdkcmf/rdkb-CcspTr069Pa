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

    module:	ccsp_cwmp_sesso_states.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced state-access functions
        of the CCSP CWMP Session Object.

        *   CcspCwmpsoGetCcspCwmpAcsConnection
        *   CcspCwmpsoGetCcspCwmpMcoIf
        *   CcspCwmpsoGetCcspCwmpCpeController
        *   CcspCwmpsoSetCcspCwmpCpeController
        *   CcspCwmpsoGetCcspCwmpProcessor
        *   CcspCwmpsoSetCcspCwmpProcessor
        *   CcspCwmpsoReset

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
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_sesso_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpsoGetCcspCwmpAcsConnection
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
CcspCwmpsoGetCcspCwmpAcsConnection
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject    = (PCCSP_CWMP_SESSION_OBJECT)hThisObject;

    return  pMyObject->hCcspCwmpAcsConnection;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpsoGetCcspCwmpMcoIf
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
CcspCwmpsoGetCcspCwmpMcoIf
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject    = (PCCSP_CWMP_SESSION_OBJECT)hThisObject;

    return  pMyObject->hCcspCwmpMcoIf;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpsoGetCcspCwmpCpeController
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
CcspCwmpsoGetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject    = (PCCSP_CWMP_SESSION_OBJECT)hThisObject;

    return  pMyObject->hCcspCwmpCpeController;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoSetCcspCwmpCpeController
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCpeController
            );

    description:

        This function is called to configure object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hCpeController
                Specifies the object state to be configured.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoSetCcspCwmpCpeController
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCpeController
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject    = (PCCSP_CWMP_SESSION_OBJECT)hThisObject;

    pMyObject->hCcspCwmpCpeController = hCpeController;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpsoGetCcspCwmpProcessor
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
CcspCwmpsoGetCcspCwmpProcessor
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject    = (PCCSP_CWMP_SESSION_OBJECT)hThisObject;

    return  pMyObject->hCcspCwmpProcessor;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoSetCcspCwmpProcessor
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hWmpProcessor
            );

    description:

        This function is called to configure object state.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hWmpProcessor
                Specifies the object state to be configured.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoSetCcspCwmpProcessor
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hWmpProcessor
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject    = (PCCSP_CWMP_SESSION_OBJECT)hThisObject;

    pMyObject->hCcspCwmpProcessor = hWmpProcessor;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpsoReset
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to reset object states.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpsoReset
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_SESSION_OBJECT        pMyObject      = (PCCSP_CWMP_SESSION_OBJECT  )hThisObject;
    PCCSP_CWMPSO_ASYNC_REQUEST       pWmpsoAsyncReq = (PCCSP_CWMPSO_ASYNC_REQUEST )NULL;
    PCCSP_CWMPSO_ASYNC_RESPONSE      pWmpsoAsyncRep = (PCCSP_CWMPSO_ASYNC_RESPONSE)NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry    = (PSINGLE_LINK_ENTRY        )NULL;

    pMyObject->StopRetryTimer(pMyObject);

    pMyObject->DelAllEvents    ((ANSC_HANDLE)pMyObject);
    pMyObject->DelAllParameters((ANSC_HANDLE)pMyObject);

    /*
     * Release all the async requests...
     */
    AnscAcquireLock(&pMyObject->AsyncReqQueueLock);

    pSLinkEntry = AnscQueuePopEntry(&pMyObject->AsyncReqQueue);

    while ( pSLinkEntry )
    {
        pWmpsoAsyncReq = ACCESS_CCSP_CWMPSO_ASYNC_REQUEST(pSLinkEntry);
        pSLinkEntry    = AnscQueuePopEntry(&pMyObject->AsyncReqQueue);

        /* CcspCwmpsoSignalAsyncRequest(pWmpsoAsyncReq, ANSC_STATUS_FAILURE); */
        CcspCwmpsoFreeAsyncRequest(pWmpsoAsyncReq);
    }

    AnscReleaseLock(&pMyObject->AsyncReqQueueLock);

    /*
     * Release all the saved requests...
     */
    AnscAcquireLock(&pMyObject->SavedReqQueueLock);

    pSLinkEntry = AnscQueuePopEntry(&pMyObject->SavedReqQueue);

    while ( pSLinkEntry )
    {
        pWmpsoAsyncReq = ACCESS_CCSP_CWMPSO_ASYNC_REQUEST(pSLinkEntry);
        pSLinkEntry    = AnscQueuePopEntry(&pMyObject->SavedReqQueue);

        /* CcspCwmpsoSignalAsyncRequest(pWmpsoAsyncReq, ANSC_STATUS_FAILURE); */
        CcspCwmpsoFreeAsyncRequest(pWmpsoAsyncReq);
    }

    AnscReleaseLock(&pMyObject->SavedReqQueueLock);

    /*
     * Release all the async responses...
     */
    AnscAcquireLock(&pMyObject->AsyncRepQueueLock);

    pSLinkEntry = AnscQueuePopEntry(&pMyObject->AsyncRepQueue);

    while ( pSLinkEntry )
    {
        pWmpsoAsyncRep = ACCESS_CCSP_CWMPSO_ASYNC_RESPONSE(pSLinkEntry);
        pSLinkEntry    = AnscQueuePopEntry(&pMyObject->AsyncRepQueue);

        if( pWmpsoAsyncRep )
        {
            CcspCwmpsoFreeAsyncResponse(pWmpsoAsyncRep);
            pWmpsoAsyncRep = NULL;
        }

    }

    AnscReleaseLock(&pMyObject->AsyncRepQueueLock);

    return  ANSC_STATUS_SUCCESS;
}
