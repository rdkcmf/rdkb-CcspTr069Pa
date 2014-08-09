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

    module:	ccsp_cwmp_tcpcrho_dstowo.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    copyright:

        Cisco Systems, Inc., 1997 ~ 2011
        All Rights Reserved.

    ---------------------------------------------------------------

    description:

        This module implements the advanced worker functions
        of CCSP CWMP TCP Connection Request Handler Object.

        *   CcspCwmpTcpcrhoDstowoAccept
        *   CcspCwmpTcpcrhoDstowoSetOut
        *   CcspCwmpTcpcrhoDstowoRemove
        *   CcspCwmpTcpcrhoDstowoQuery
        *   CcspCwmpTcpcrhoDstowoProcessSync
        *   CcspCwmpTcpcrhoDstowoProcessAsync
        *   CcspCwmpTcpcrhoDstowoSendComplete
        *   CcspCwmpTcpcrhoDstowoNotify

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/20/08    initial revision.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_tcpcrho_global.h"


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        CcspCwmpTcpcrhoDstowoAccept
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hSocket,
                PANSC_HANDLE                phClientContext
            );

    description:

        This function is called by the internal Server Object as part
        of the worker function set.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hSocket
                Specifies the socket object associated with the call.

                PANSC_HANDLE                phClientContext
                Specifies the client context to be returned.

    return:     TRUE or FALSE.

**********************************************************************/

BOOL
CcspCwmpTcpcrhoDstowoAccept
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PANSC_HANDLE                phClientContext
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject      = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty      = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;
    PANSC_DAEMON_SOCKET_TCP_OBJECT  pWebSocket     = (PANSC_DAEMON_SOCKET_TCP_OBJECT)hSocket;
    PANSC_BUFFER_DESCRIPTOR         pBufferDesp    = NULL;

#ifdef _ANSC_IPV6_COMPATIBLE_
    CcspTr069PaTraceDebug
        ((
            "CcspCwmpTcpcrhoDstowoAccept accepts a new client connection from %s / TCP %d\n",
            pWebSocket->PeerAddr,
            pWebSocket->PeerPort
        ));
#else
    CcspTr069PaTraceDebug
        ((
            "CcspCwmpTcpcrhoDstowoAccept accepts a new client connection from %d.%d.%d.%d / TCP %d\n",
            pWebSocket->PeerAddress.Dot[0],
            pWebSocket->PeerAddress.Dot[1],
            pWebSocket->PeerAddress.Dot[2],
            pWebSocket->PeerAddress.Dot[3],
            pWebSocket->PeerPort
        ));
#endif
    *phClientContext = (ANSC_HANDLE)NULL;

    /* we may need to give us a chance to check if we accept 
     * connection from the peer specified in hSocket if we
     * see the needs in future.
     */

    pBufferDesp =
        (PANSC_BUFFER_DESCRIPTOR)AnscAllocateBdo
            (
                HTTP_SSO_MAX_HEADERS_SIZE,
                0,
                0
            );

    if ( pBufferDesp )
    {
        pWebSocket->SetBufferContext
            (
                (ANSC_HANDLE)pWebSocket,
                AnscBdoGetEndOfBlock(pBufferDesp),
                AnscBdoGetLeftSize  (pBufferDesp),
                (ANSC_HANDLE)pBufferDesp
            );
    }

    return  TRUE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoDstowoSetOut
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hSocket
            );

    description:

        This function is called by the internal Server Object as part
        of the worker function set.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hSocket
                Specifies the socket object associated with the call.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoDstowoSetOut
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty     = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoDstowoRemove
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hSocket
            );

    description:

        This function is called by the internal Server Object as part
        of the worker function set.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hSocket
                Specifies the socket object associated with the call.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoDstowoRemove
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty     = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;
    PANSC_DAEMON_SOCKET_TCP_OBJECT  pWebSocket    = (PANSC_DAEMON_SOCKET_TCP_OBJECT)hSocket;
    PANSC_BUFFER_DESCRIPTOR         pBufferDesp   = (PANSC_BUFFER_DESCRIPTOR       )pWebSocket->GetBufferContext((ANSC_HANDLE)pWebSocket);

#ifdef _ANSC_IPV6_COMPATIBLE_
    CcspTr069PaTraceDebug
        ((
            "CcspCwmpTcpcrhoDstowoRemove removes the client connection of %s / TCP %d\n",
            pWebSocket->PeerAddr,
            pWebSocket->PeerPort
        ));
#else
    CcspTr069PaTraceDebug
        ((
            "CcspCwmpTcpcrhoDstowoRemove removes the client connection of %d.%d.%d.%d / TCP %d\n",
            pWebSocket->PeerAddress.Dot[0],
            pWebSocket->PeerAddress.Dot[1],
            pWebSocket->PeerAddress.Dot[2],
            pWebSocket->PeerAddress.Dot[3],
            pWebSocket->PeerPort
        ));
#endif

    pWebSocket->SetClientContext((ANSC_HANDLE)pWebSocket,          (ANSC_HANDLE)NULL);
    pWebSocket->SetBufferContext((ANSC_HANDLE)pWebSocket, NULL, 0, (ANSC_HANDLE)NULL);

    if ( pBufferDesp )
    {
        AnscFreeBdo((ANSC_HANDLE)pBufferDesp);
    }

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmpTcpcrhoDstowoQuery
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hSocket,
                PVOID                       buffer,
                ULONG                       ulSize,
                PANSC_HANDLE                phQueryContext
            );

    description:

        This function is called by the internal Server Object as part
        of the worker function set.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hSocket
                Specifies the socket object associated with the call.

                PVOID                       buffer
                Specifies the data buffer to be processed.

                ULONG                       ulSize
                Specifies the size of data buffer to be processed.

                PANSC_HANDLE                phQueryContext
                Specifies the recv context associated with buffer.

    return:     process mode.

**********************************************************************/

ULONG
CcspCwmpTcpcrhoDstowoQuery
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PVOID                       buffer,
        ULONG                       ulSize,
        PANSC_HANDLE                phQueryContext
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty     = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;
    PANSC_DAEMON_SOCKET_TCP_OBJECT  pWebSocket    = (PANSC_DAEMON_SOCKET_TCP_OBJECT)hSocket;
    PANSC_BUFFER_DESCRIPTOR         pBufferDesp   = (PANSC_BUFFER_DESCRIPTOR       )pWebSocket->GetBufferContext((ANSC_HANDLE)pWebSocket);
    ULONG                           ulDstoPmode   = ANSC_DSTOWO_PMODE_PROCESS_SYNC;
    ULONG                           ulSessQmode   = CCSP_CWMP_TCPCR_SESSION_MODE_FINISH;
    BOOL                            bComplete;

    pBufferDesp->BlockSize += ulSize;

    bComplete = 
        CcspCwmpTcpcrhoIsRequestComplete
            (
                (ANSC_HANDLE)pMyObject,
                AnscBdoGetBlock    (pBufferDesp), 
                AnscBdoGetBlockSize(pBufferDesp)
            );

    if ( bComplete )
    {
        ulSessQmode = CCSP_CWMP_TCPCR_SESSION_MODE_PROCESS;
    }
    else
    {
        ulSessQmode = CCSP_CWMP_TCPCR_SESSION_MODE_COLLECT;
    }

    switch ( ulSessQmode )
    {
        case    CCSP_CWMP_TCPCR_SESSION_MODE_COLLECT :

                pBufferDesp->BlockSize -= ulSize;
                ulDstoPmode             = ANSC_DSTOWO_PMODE_COLLECT;

                break;

        case    CCSP_CWMP_TCPCR_SESSION_MODE_PROCESS :

                ulDstoPmode = ANSC_DSTOWO_PMODE_PROCESS_SYNC;

                break;

        default :

                ulDstoPmode = ANSC_DSTOWO_PMODE_FINISH;

                break;
    }

    *phQueryContext = (ANSC_HANDLE)ulSessQmode;
    
    return  ulDstoPmode;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoDstowoProcessSync
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hSocket,
                PVOID                       buffer,
                ULONG                       ulSize,
                ANSC_HANDLE                 hQueryContext
            );

    description:

        This function is called by the internal Server Object as part
        of the worker function set.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hSocket
                Specifies the socket object associated with the call.

                PVOID                       buffer
                Specifies the data buffer to be processed.

                ULONG                       ulSize
                Specifies the size of data buffer to be processed.

                ANSC_HANDLE                 hQueryContext
                Specifies the recv context associated with buffer.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoDstowoProcessSync
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PVOID                       buffer,
        ULONG                       ulSize,
        ANSC_HANDLE                 hQueryContext
    )
{
    ANSC_STATUS                     returnStatus  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty     = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;
    PANSC_DAEMON_SOCKET_TCP_OBJECT  pWebSocket    = (PANSC_DAEMON_SOCKET_TCP_OBJECT)hSocket;
    PANSC_BUFFER_DESCRIPTOR         pBufferDesp   = (PANSC_BUFFER_DESCRIPTOR       )pWebSocket->GetBufferContext((ANSC_HANDLE)pWebSocket);
    ULONG                           ulSessQmode   = (ULONG)hQueryContext;
    BOOL                            bFinishSocket = FALSE;

#ifdef _ANSC_IPV6_COMPATIBLE_
    CcspTr069PaTraceDebug
        ((
            "CcspCwmpTcpcrhoDstowoProcessSync - Processing packet received from client connection of %s / TCP %d\n",
            pWebSocket->PeerAddr,
            pWebSocket->PeerPort
        ));
#else
    CcspTr069PaTraceDebug
        ((
            "CcspCwmpTcpcrhoDstowoProcessSync - Processing packet received from client connection of %d.%d.%d.%d / TCP %d\n",
            pWebSocket->PeerAddress.Dot[0],
            pWebSocket->PeerAddress.Dot[1],
            pWebSocket->PeerAddress.Dot[2],
            pWebSocket->PeerAddress.Dot[3],
            pWebSocket->PeerPort
        ));
#endif

    switch ( ulSessQmode )
    {
        case    CCSP_CWMP_TCPCR_SESSION_MODE_PROCESS :

                pWebSocket->SetBufferContext
                    (
                        (ANSC_HANDLE)pWebSocket,
                        NULL,
                        0,
                        (ANSC_HANDLE)NULL
                    );

                /* if this connection request is valid, we are going to
                 * notify connect to ACS and close the underlying socket
                 * for the connection request. If this request needs to
                 * to authenticated, challenge will be sent to client
                 * and the underlying socket would be kept open. 
                 * In reality, there might be some ACS servers which 
                 * may use different socket to send another request
                 * with authentication information. In this case, the
                 * previous socket would be cleanned after socket times
                 * out or closed by the remote peer.
                 */

                returnStatus = 
                    pMyObject->ProcessRequest
                        (
                            (ANSC_HANDLE)pMyObject,
                            (ANSC_HANDLE)pWebSocket,
                            buffer,
                            ulSize
                        );

                if ( pBufferDesp )
                {
                    AnscFreeBdo((ANSC_HANDLE)pBufferDesp);
                }

                if ( returnStatus == ANSC_STATUS_BAD_AUTH_DATA )
                {
                    /* ACS wasn't authenticated */
                    bFinishSocket = FALSE;
                }
                else
                {
                    bFinishSocket = TRUE;
                }

                break;

        default :

                returnStatus = ANSC_STATUS_UNAPPLICABLE;

                break;
    }

    if ( bFinishSocket )
    {
        returnStatus = ANSC_STATUS_SUCCESS;

#ifdef _ANSC_IPV6_COMPATIBLE_
        CcspTr069PaTraceDebug
            ((
                "CcspCwmpTcpcrhoDstowoProcessSync - Going to close client connection of %s / TCP %d\n",
                pWebSocket->PeerAddr,
                pWebSocket->PeerPort
            ));
#else
        CcspTr069PaTraceDebug
            ((
                "CcspCwmpTcpcrhoDstowoProcessSync - Going to close client connection of %d.%d.%d.%d / TCP %d\n",
                pWebSocket->PeerAddress.Dot[0],
                pWebSocket->PeerAddress.Dot[1],
                pWebSocket->PeerAddress.Dot[2],
                pWebSocket->PeerAddress.Dot[3],
                pWebSocket->PeerPort
            ));
#endif

        pWebSocket->ToClean((ANSC_HANDLE)pWebSocket, TRUE, HTTP_SSO_SOCKET_TTC);
    }

    return  returnStatus;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoDstowoProcessAsync
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hSocket,
                PVOID                       buffer,
                ULONG                       ulSize,
                ANSC_HANDLE                 hQueryContext
            );

    description:

        This function is called by the internal Server Object as part
        of the worker function set.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hSocket
                Specifies the socket object associated with the call.

                PVOID                       buffer
                Specifies the data buffer to be processed.

                ULONG                       ulSize
                Specifies the size of data buffer to be processed.

                ANSC_HANDLE                 hQueryContext
                Specifies the recv context associated with buffer.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoDstowoProcessAsync
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        PVOID                       buffer,
        ULONG                       ulSize,
        ANSC_HANDLE                 hQueryContext
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty     = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;
    PANSC_DAEMON_SOCKET_TCP_OBJECT  pWebSocket    = (PANSC_DAEMON_SOCKET_TCP_OBJECT)hSocket;
    PANSC_BUFFER_DESCRIPTOR         pBufferDesp   = (PANSC_BUFFER_DESCRIPTOR       )pWebSocket->GetBufferContext((ANSC_HANDLE)pWebSocket);

    return  ANSC_STATUS_UNAPPLICABLE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoDstowoSendComplete
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hSocket,
                ANSC_HANDLE                 hReserved,
                ANSC_STATUS                 status
            );

    description:

        This function is called by the internal Server Object as part
        of the worker function set.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hSocket
                Specifies the socket object associated with the call.

                ANSC_HANDLE                 hReserved
                Specifies the transparent context passed in send().

                ANSC_STATUS                 status
                Specifies the status of the send operation.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoDstowoSendComplete
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        ANSC_HANDLE                 hReserved,
        ANSC_STATUS                 status
    )
{
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty     = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;
    PANSC_DAEMON_SOCKET_TCP_OBJECT  pWebSocket    = (PANSC_DAEMON_SOCKET_TCP_OBJECT)hSocket;
    PANSC_BUFFER_DESCRIPTOR         pBufferDesp   = (PANSC_BUFFER_DESCRIPTOR       )pWebSocket->GetBufferContext((ANSC_HANDLE)pWebSocket);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpTcpcrhoDstowoNotify
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hSocket,
                ULONG                       ulEvent,
                ANSC_HANDLE                 hReserved
            );

    description:

        This function is called by the internal Server Object as part
        of the worker function set.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

                ANSC_HANDLE                 hSocket
                Specifies the socket object associated with the call.

                ULONG                       ulEvent
                Specifies the event to be notified.

                ANSC_HANDLE                 hReserved
                Specifies the transparent context associated with event.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpTcpcrhoDstowoNotify
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hSocket,
        ULONG                       ulEvent,
        ANSC_HANDLE                 hReserved
    )
{
    ANSC_STATUS                     returnStatus  = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT pMyObject     = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT    )hThisObject;
    PCCSP_CWMP_TCPCR_HANDLER_PROPERTY    pProperty     = (PCCSP_CWMP_TCPCR_HANDLER_PROPERTY  )&pMyObject->Property;
    PANSC_DAEMON_SOCKET_TCP_OBJECT  pWebSocket    = (PANSC_DAEMON_SOCKET_TCP_OBJECT)hSocket;
    PANSC_BUFFER_DESCRIPTOR         pBufferDesp   = (PANSC_BUFFER_DESCRIPTOR       )pWebSocket->GetBufferContext((ANSC_HANDLE)pWebSocket);

#ifdef _ANSC_IPV6_COMPATIBLE_
    CcspTr069PaTraceDebug
        ((
            "CcspCwmpTcpcrhoDstowoNotify - event = %u is indicated on client connection of %s / TCP %d\n",
            (unsigned int)ulEvent,
            pWebSocket->PeerAddr,
            pWebSocket->PeerPort
        ));
#else
    CcspTr069PaTraceDebug
        ((
            "CcspCwmpTcpcrhoDstowoNotify - event = %u is indicated on client connection of %d.%d.%d.%d / TCP %d\n",
            (unsigned int)ulEvent,
            (int)pWebSocket->PeerAddress.Dot[0],
            (int)pWebSocket->PeerAddress.Dot[1],
            (int)pWebSocket->PeerAddress.Dot[2],
            (int)pWebSocket->PeerAddress.Dot[3],
            (int)pWebSocket->PeerPort
        ));
#endif

    switch ( ulEvent )
    {
        case    ANSC_DSTOWO_EVENT_SOCKET_ERROR :

                pWebSocket->ToClean((ANSC_HANDLE)pWebSocket, TRUE, HTTP_SSO_SOCKET_TTC);

                break;

        case    ANSC_DSTOWO_EVENT_SOCKET_CLOSED :

                if ( pBufferDesp )
                {
                    pBufferDesp->BlockSize += pWebSocket->RecvPacketSize;
                    returnStatus            =
                        pWebSocket->SetBufferContext
                            (
                                (ANSC_HANDLE)pWebSocket,
                                NULL,
                                0,
                                (ANSC_HANDLE)NULL
                            );

                    AnscFreeBdo((ANSC_HANDLE)pBufferDesp);
                }

                pWebSocket->ToClean((ANSC_HANDLE)pWebSocket, TRUE, HTTP_SSO_SOCKET_TTC);

                break;

        case    ANSC_DSTOWO_EVENT_RESOURCES :

                break;

        case    ANSC_DSTOWO_EVENT_TIME_OUT :

                break;

        default :

                break;
    }

    return  returnStatus;
}

