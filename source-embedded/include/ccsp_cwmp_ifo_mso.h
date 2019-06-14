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

    module:	ccsp_cwmp_ifo_mso.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This wrapper file defines the base class data structure and
        interface for the CCSP CWMP Management Server Operation Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/01/05    initial revision.
        06/21/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_IFO_MSO_
#define  _CCSP_CWMP_IFO_MSO_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_ifo_interface.h"
#include "slap_definitions.h"

/***********************************************************
    CCSP CWMP MANAGEMENT SERVER OPERATION INTERFACE DEFINITION
***********************************************************/

/*
 * Define some const values that will be used in the os wrapper object definition.
 */
#define  CCSP_CWMP_MSO_INTERFACE_NAME                    "cwmpManagementServerOperationIf"
#define  CCSP_CWMP_MSO_INTERFACE_ID                      0

/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support), we
 * have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  ANSC_STATUS
(*PFN_CWMPMSOIF_GET_METHODS)
    (
        ANSC_HANDLE                 hThisObject,
        SLAP_STRING_ARRAY**         ppMethodList
    );

typedef  ANSC_STATUS
(*PFN_CWMPMSOIF_INFORM)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pEventCode,
        char*                       pCommandKey,
        BOOL                        bConnectNow
    );

typedef  ANSC_STATUS
(*PFN_CWMPMSOIF_TRANSFER_COMP)
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        char*                       pCommandKey,        /* 'CommentKey' argument passed in Download() or Upload() */
        ANSC_HANDLE                 hFault,             /* non-zero FaultCode is specified if transfer failed     */
        ANSC_HANDLE                 hStartTime,         /* the date and time transfer was started in UTC          */
        ANSC_HANDLE                 hCompleteTime,      /* the date and time transfer compoleted in UTC           */
        BOOL                        bNewSession         /* to indicate if this RPC needs to be sent in a new CWMP session */
    );

typedef  ANSC_STATUS
(*PFN_CWMPMSOIF_AT_TRANSFER_COMP)
    (
        ANSC_HANDLE                 hThisObject,
		BOOL						bIsDownload,
        char*                       AnnounceURL,
        char*                       TransferURL,
        char*                       FileType,
        unsigned int                FileSize,
        char*                       TargetFileName,
        ANSC_HANDLE                 hFault,             /* non-zero FaultCode is specified if transfer failed     */
        ANSC_HANDLE                 hStartTime,         /* the date and time transfer was started in UTC          */
        ANSC_HANDLE                 hCompleteTime,      /* the date and time transfer compoleted in UTC           */
        BOOL                        bNewSession         /* to indicate if this RPC needs to be sent in a new CWMP session */
    );

typedef  ANSC_STATUS
(*PFN_CWMPMSOIF_DSC_COMP)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hDsccReq,
        BOOL                        bNewSession         /* to indicate if this RPC needs to be sent in a new CWMP session */
    );

typedef  ANSC_STATUS
(*PFN_CWMPMSOIF_ADSC_COMP)
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hAdsccReq,
        BOOL                        bNewSession         /* to indicate if this RPC needs to be sent in a new CWMP session */
    );

typedef  ANSC_STATUS
(*PFN_CWMPMSOIF_KICKED)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pCommand,
        char*                       pReferer,
        char*                       pArg,
        char*                       pNext,
        char**                      ppNextUrl
    );

typedef  ANSC_STATUS
(*PFN_CWMPMSOIF_REQ_DOWNLOAD)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pFileType,
        ANSC_HANDLE                 hFileTypeArgArray,
        ULONG                       ulArraySize
    );

typedef  ANSC_STATUS
(*PFN_CWMPMSOIF_CHANGED)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pParamName,
        char*                       pParamValue,
        ULONG                       CwmpDataType,
        BOOL                        bConnectNow
    );

/*
 * The CPE will initiate a transaction session to the ACS as a result of the conditions listed
 * below:
 *
 *      - The first time CPE establishes a connection to the access network on initial
 *        installation
 *      - On power-up or reset
 *      - Once every PeriodicInformInterval (for example, every 24-hours)
 *      - When so instructed by the optional ScheduleInform method
 *      - Whenever the CPE receives a valid Connection Request from an ACS
 *      - Whenever the URL of the ACS changes
 *      - Whenever a parameter is modified that is required to initiate an Inform on
 *        change.
 *      - Whenever the value of a parameter that the ACS has marked for "active
 *        notification" via the SetParameterAttributes method is modified by an external
 *        cause.
 *
 * Once the connection to the ACS is successfully established, the CPE initiates a session by
 * sending an initial Inform request to the ACS. This indicates to the ACS the current status of
 * the CPE and that the CPE is ready to accept request from the ACS.
 */
#define  CCSP_CWMP_MSO_INTERFACE_CLASS_CONTENT                                                   \
    /* duplication of the base object class content */                                      \
    ANSCIFO_CLASS_CONTENT                                                                   \
    /* start of object class content */                                                     \
    PFN_CWMPMSOIF_GET_METHODS       GetRpcMethods;                                          \
    PFN_CWMPMSOIF_INFORM            Inform;                                                 \
    PFN_CWMPMSOIF_TRANSFER_COMP     TransferComplete;                                       \
    PFN_CWMPMSOIF_AT_TRANSFER_COMP  AutonomousTransferComplete;                             \
    PFN_CWMPMSOIF_DSC_COMP          DuStateChangeComplete;                                  \
    PFN_CWMPMSOIF_ADSC_COMP         AutonomousDuStateChangeComplete;                        \
                                                                                            \
    PFN_CWMPMSOIF_KICKED            Kicked;                                                 \
    PFN_CWMPMSOIF_REQ_DOWNLOAD      RequestDownload;                                        \
    PFN_CWMPMSOIF_CHANGED           ValueChanged;                                           \
    /* end of object class content */                                                       \

typedef  struct
_CCSP_CWMP_MSO_INTERFACE
{
    CCSP_CWMP_MSO_INTERFACE_CLASS_CONTENT
}
CCSP_CWMP_MSO_INTERFACE,  *PCCSP_CWMP_MSO_INTERFACE;

#define  ACCESS_CCSP_CWMP_MSO_INTERFACE(p)               \
         ACCESS_CONTAINER(p, CCSP_CWMP_MSO_INTERFACE, Linkage)


#endif
