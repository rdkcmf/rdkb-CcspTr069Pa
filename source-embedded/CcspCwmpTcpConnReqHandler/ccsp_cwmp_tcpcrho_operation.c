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

    module:	ccsp_cwmp_tcpcrho_operation.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced operation functions
        of CCSP CWMP TCP Connection Request Handler Object.

        *   CcspCwmpTcpcrhoEngage
        *   CcspCwmpTcpcrhoCancel
        *   CcspCwmpTcpcrhoCreateTcpServers
        *   CcspCwmpTcpcrhoRemoveTcpServers
        *   CcspCwmpTcpcrhoWorkerInit
        *   CcspCwmpTcpcrhoWorkerUnload

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/19/08    initial revision.

**********************************************************************/


#include "ccsp_cwmp_tcpcrho_global.h"
#include "sysevent/sysevent.h"

extern int s_sysevent_connect (token_t *out_se_token);

/*
 *  RDKB-12305  Adding method to check whether comcast device or not
 *  Procedure     : bIsComcastImage
 *  Purpose       : return True for Comcast build.
 *  Parameters    :
 *  Return Values :
 *  1             : 1 for comcast images
 *  2             : 0 for other images
 */
#define DEVICE_PROPERTIES    "/etc/device.properties" 
static int bIsComcastImage( void)
{
    char PartnerId[255] = {'\0'};
    int isComcastImg = 1;
    errno_t rc  = -1;
    int     ind = -1;
    
    getPartnerId ( PartnerId ) ;
    rc = strcmp_s("comcast", strlen("comcast"), PartnerId, &ind);
    ERR_CHK(rc);
    if((rc == EOK) && (ind != 0))
    {
        isComcastImg = 0;
    }

   return isComcastImg;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoEngage
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to engage this object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoEngage
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty     = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;
    PANSC_DAEMON_SERVER_TCP_OBJECT  pTcpServer    = (PANSC_DAEMON_SERVER_TCP_OBJECT)pMyObject->hTcpServer;
    ULONG                           ulEngineCount = 1;
    ULONG                           ulSocketCount = 1;
    ULONG                           ulTcpDsoMode  = ANSC_DSTO_MODE_EVENT_SYNC | ANSC_DSTO_MODE_FOREIGN_BUFFER | ANSC_DSTO_MODE_COMPACT;

    if ( pMyObject->bActive )
    {
        return  ANSC_STATUS_SUCCESS;
    }
    else
    {
        pMyObject->bActive = TRUE;
    }

    returnStatus = pMyObject->CreateTcpServers((ANSC_HANDLE)pMyObject);
    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        CcspTr069PaTraceDebug(("Create TCP server failed\n"));

        pMyObject->bActive = FALSE;
        return  returnStatus;
    }

    if ( pProperty->ServerMode & CCSP_CWMP_TCPCR_HANDLER_MODE_useXsocket )
    {
        ulTcpDsoMode |= ANSC_DSTO_MODE_XSOCKET;
    }

    ulEngineCount = 1;
    ulSocketCount = 4;

    pTcpServer    = (PANSC_DAEMON_SERVER_TCP_OBJECT)pMyObject->hTcpServer;

    if ( pTcpServer )
    {
#ifdef _ANSC_IPV6_COMPATIBLE_
        errno_t             rc = -1;
        CcspTr069PaTraceDebug(("Tcp host addr=%s:%d\n", 
                               pProperty->HostAddr,
                               pProperty->HostPort));
       rc = strcpy_s(pTcpServer->HostName, sizeof(pTcpServer->HostName), pProperty->HostAddr);
       if(rc != EOK)
       {
          ERR_CHK(rc);
          return ANSC_STATUS_FAILURE;
       }
#else
        CcspTr069PaTraceInfo(("Tcp host addr=%d.%d.%d.%d:%d\n", 
                               pProperty->HostAddress.Dot[0],
                               pProperty->HostAddress.Dot[1],
                               pProperty->HostAddress.Dot[2],
                               pProperty->HostAddress.Dot[3],
                               pProperty->HostPort));

        pTcpServer->SetHostAddress   ((ANSC_HANDLE)pTcpServer, pProperty->HostAddress.Dot);
#endif
        pTcpServer->SetHostPort      ((ANSC_HANDLE)pTcpServer, pProperty->HostPort       );
        pTcpServer->SetMaxMessageSize((ANSC_HANDLE)pTcpServer, CCSP_CWMP_TCPCR_MAX_MSG_SIZE   );
        pTcpServer->SetEngineCount   ((ANSC_HANDLE)pTcpServer, ulEngineCount             );
        pTcpServer->SetMinSocketCount((ANSC_HANDLE)pTcpServer, 0                         );
        pTcpServer->SetMaxSocketCount((ANSC_HANDLE)pTcpServer, ulSocketCount             );
        pTcpServer->SetMode          ((ANSC_HANDLE)pTcpServer, ulTcpDsoMode              );

        returnStatus = pTcpServer->Engage((ANSC_HANDLE)pTcpServer);
        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            CcspTr069PaTraceError(("CcspCwmpTcpcrhoEngage - failed to be engaged, CWMP will not run properly!\n"));
            pMyObject->bActive = FALSE;

            return  returnStatus;
        }
    }

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoCancel
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to control the proxy's behavior.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoCancel
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PANSC_DAEMON_SERVER_TCP_OBJECT  pTcpServer    = (PANSC_DAEMON_SERVER_TCP_OBJECT)pMyObject->hTcpServer;

    if ( !pMyObject->bActive )
    {
        return  ANSC_STATUS_SUCCESS;
    }
    else
    {
        pMyObject->bActive = FALSE;
    }

    if ( pTcpServer )
    {
        returnStatus = pTcpServer->Cancel((ANSC_HANDLE)pTcpServer);
    }

    returnStatus = pMyObject->RemoveTcpServers((ANSC_HANDLE)pMyObject);

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoCreateTcpServers
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to create TCP servers.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoCreateTcpServers

    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty     = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;
    PANSC_DAEMON_SERVER_TCP_OBJECT  pTcpServer    = (PANSC_DAEMON_SERVER_TCP_OBJECT)pMyObject->hTcpServer;
    char  buf[64]      = {0};

    if ( pProperty->HostPort == 0 && pTcpServer )
    {
        CcspTr069PaTraceDebug(("Port is 0 on current TcpServer: Server will be removed!!!\n"));
        pTcpServer->Remove((ANSC_HANDLE)pTcpServer);
        pMyObject->hTcpServer   = (ANSC_HANDLE)NULL;
    }
    else if ( !pTcpServer && pProperty->HostPort != 0 )
    {
        if ( bIsComcastImage() ) {

           // If HostAddress value is zero, then bind the outbound interface's ip address
           if( pProperty->HostAddress.Value == 0)
           {
              CcspTr069PaTraceInfo(("%s, HostAddress value is 0\n",__FUNCTION__));
              token_t  se_token;
              int      se_fd = s_sysevent_connect(&se_token);
              if (0 > se_fd) 
              {
                 CcspTr069PaTraceError(("%s, sysevent_connect failed!!!\n",__FUNCTION__));
                 //return ERR_SYSEVENT_CONN;
              }
              else
              {
                 // Get ipv4 address from sysevent
                 if( 0 == sysevent_get(se_fd, se_token, "ipv4_wan_ipaddr", buf, sizeof(buf)) && '\0' != buf[0] )
                 {
                    CcspTr069PaTraceInfo(("%s, ipv4_wan_ipaddr got from sysevent is: %s\n",__FUNCTION__,buf));
                    pProperty->HostAddress.Value = _ansc_inet_addr(buf);
                    CcspTr069PaTraceInfo(("%s,pProperty->HostAddress.Value: %lu\n",__FUNCTION__,pProperty->HostAddress.Value));
                 }
                 else
                 {
                  // If sysevent fails, let TR69 bind on 0.0.0.0
                  CcspTr069PaTraceError(("%s, sysevent_get failed to get value of ipv4_wan_ipaddr!!!\n",__FUNCTION__));
                 }
              }
           }
       
           CcspTr069PaTraceInfo(("%s, Call AnscCreateDaemonServerTcp\n",__FUNCTION__));
        }
        pTcpServer =
            (PANSC_DAEMON_SERVER_TCP_OBJECT)AnscCreateDaemonServerTcp
                (
                    pMyObject->hContainerContext,
                    (ANSC_HANDLE)pMyObject,
                    (ANSC_HANDLE)NULL
                );

        if ( !pTcpServer )
        {
            CcspTr069PaTraceError(("Something wrong in AnscCreateDaemonServerTCP.\n"));
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hTcpServer = (ANSC_HANDLE)pTcpServer;
        }

        pTcpServer->SetWorker
            (
                (ANSC_HANDLE)pTcpServer,
                pMyObject->hDstoWorker,
                sizeof(ANSC_DSTO_WORKER_OBJECT)
            );
    }

    CcspTr069PaTraceInfo(("TCP server created successfully.\n"));

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoRemoveTcpServers
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to remove TCP servers.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoRemoveTcpServers
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PANSC_DAEMON_SERVER_TCP_OBJECT  pTcpServer = (PANSC_DAEMON_SERVER_TCP_OBJECT)pMyObject->hTcpServer;

    if ( pTcpServer )
    {
        pTcpServer->Remove((ANSC_HANDLE)pTcpServer);

        pMyObject->hTcpServer = (ANSC_HANDLE)NULL;
    }

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoWorkerInit
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to control the proxy's behavior.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoWorkerInit
    (
        ANSC_HANDLE                 hThisObject
    )
{
    UNREFERENCED_PARAMETER(hThisObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoWorkerUnload
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to control the proxy's behavior.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoWorkerUnload
    (
        ANSC_HANDLE                 hThisObject
    )
{
    UNREFERENCED_PARAMETER(hThisObject);

    return  ANSC_STATUS_SUCCESS;
}
