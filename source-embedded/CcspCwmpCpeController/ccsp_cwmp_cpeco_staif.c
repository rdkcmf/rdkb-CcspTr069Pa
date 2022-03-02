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

    module: ccsp_cwmp_cpeco_staif.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the advanced functions
        of the CCSP CWMP Stat Interface

        *   CcspCwmpCpecoStatIfConnectAcs
        *   CcspCwmpCpecoStatIfIncTcpSuccess
        *   CcspCwmpCpecoStatIfIncTcpFailure
        *   CcspCwmpCpecoStatIfIncTlsFailure
        *   CcspCwmpCpecoStatIfGetTcpSuccessCount
        *   CcspCwmpCpecoStatIfGetTcpFailureCount
        *   CcspCwmpCpecoStatIfGetTlsFailureCount
        *   CcspCwmpCpecoStatIfResetStats
        *   CcspCwmpCpecoStatIfGetLastConnectionTime
        *   CcspCwmpCpecoStatIfGetLastConnectionStatus
        *   CcspCwmpCpecoStatIfGetLastInformResponseTime
        *   CcspCwmpCpecoStatIfGetLastReceivedSPVTime

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        11/12/08    initial revision.
        02/12/09    add more statistics apis
        10/13/01    resolve name conflicts with DM library.

**********************************************************************/


#include "ccsp_cwmp_cpeco_global.h"

/**********************************************************************

    caller:     owner of this object

    prototype:

        void
        CcspCwmpCpecoStatIfConnectAcs
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to when the time CPE connecting to Acs

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     None

**********************************************************************/
void
CcspCwmpCpecoStatIfConnectAcs
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    AnscGetLocalTime(&pMyObject->cwmpStats.LastConnectionTime);
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmpCpecoStatIfIncTcpSuccess
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to increase the count of TCP success

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The new count of TCP Failure

**********************************************************************/
ULONG
CcspCwmpCpecoStatIfIncTcpSuccess
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    pMyObject->cwmpStats.uTcpSuccess ++;

    pMyObject->cwmpStats.uLastConnectionStatus = CCSP_CWMP_CONNECTION_STATUS_SUCCESS;

    return pMyObject->cwmpStats.uTcpSuccess;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmpCpecoStatIfIncTcpFailure
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to increase the count of TCP Failure

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The new count of TCP Failure

**********************************************************************/
ULONG
CcspCwmpCpecoStatIfIncTcpFailure
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    pMyObject->cwmpStats.uTcpFailures ++;

    pMyObject->cwmpStats.uLastConnectionStatus = CCSP_CWMP_CONNECTION_STATUS_FAILURE;

    return pMyObject->cwmpStats.uTcpFailures;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmpCpecoStatIfIncTlsFailure
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to increase the count of TLS Failure

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The new count of TCP Failure

**********************************************************************/
ULONG
CcspCwmpCpecoStatIfIncTlsFailure
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    pMyObject->cwmpStats.uTlsFailures ++;

    pMyObject->cwmpStats.uLastConnectionStatus = CCSP_CWMP_CONNECTION_STATUS_FAILURE;

    return pMyObject->cwmpStats.uTlsFailures;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmpCpecoStatIfGetTcpSuccessCount
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve the count of TCP Success

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The count of TCP Failure

**********************************************************************/
ULONG
CcspCwmpCpecoStatIfGetTcpSuccessCount
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return pMyObject->cwmpStats.uTcpSuccess;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmpCpecoStatIfGetTcpFailureCount
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve the count of TCP Failure

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The count of TCP Failure

**********************************************************************/
ULONG
CcspCwmpCpecoStatIfGetTcpFailureCount
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return pMyObject->cwmpStats.uTcpFailures;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmpCpecoStatIfGetTlsFailureCount
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to retrieve the count of TLS Failure

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The count of TLS Failure

**********************************************************************/
ULONG
CcspCwmpCpecoStatIfGetTlsFailureCount
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return pMyObject->cwmpStats.uTlsFailures;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CcspCwmpCpecoStatIfResetStats
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to reset the stats

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The status of the operation

**********************************************************************/
ULONG
CcspCwmpCpecoStatIfResetStats
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    AnscZeroMemory(&pMyObject->cwmpStats, sizeof(CCSP_CWMP_STATISTICS));

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoStatIfGetLastConnectionTime
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to return the last connect time

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The time

**********************************************************************/
ANSC_HANDLE
CcspCwmpCpecoStatIfGetLastConnectionTime
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return &pMyObject->cwmpStats.LastConnectionTime;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        char*
        CcspCwmpCpecoStatIfGetLastConnectionStatus
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to return last connect status

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The status string

**********************************************************************/
char*
CcspCwmpCpecoStatIfGetLastConnectionStatus
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    if(pMyObject->cwmpStats.uLastConnectionStatus == CCSP_CWMP_CONNECTION_STATUS_FAILURE)
    {
        return AnscCloneString("Failed");
    }
    else
    {
        return AnscCloneString("Successful");
    }
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoStatIfGetLastInformResponseTime
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to return the last "Inform" time

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The time

**********************************************************************/
ANSC_HANDLE
CcspCwmpCpecoStatIfGetLastInformResponseTime
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return &pMyObject->cwmpStats.LastInformResponseTime;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        CcspCwmpCpecoStatIfGetLastReceivedSPVTime
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function is called to return the last "Received SetParameterValues" time

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     The time

**********************************************************************/
ANSC_HANDLE
CcspCwmpCpecoStatIfGetLastReceivedSPVTime
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_CPE_CONTROLLER_OBJECT     pMyObject          = (PCCSP_CWMP_CPE_CONTROLLER_OBJECT  )hThisObject;

    return &pMyObject->cwmpStats.LastReceivedSPVTime;
}
