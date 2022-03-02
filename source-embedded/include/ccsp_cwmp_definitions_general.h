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

    module:	ccsp_cwmp_definitions_general.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This file defines the configuration parameter model per
        TR-069 (CPE WAN Management Protocol).

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/02/05    initial revision.
        10/12/11    resolve name conflicts with DM library

**********************************************************************/


#ifndef  _CCSP_CWMP_DEFINITIONS_GENERAL_
#define  _CCSP_CWMP_DEFINITIONS_GENERAL_


/*
 * The CPE WAN Management Protocol can be used to remotely manage CPE Devices that are connected
 * via a LAN through a Gateway. When an ACS manages a Device connected via a NAT Gateway (where the
 * Device has been allocated a private IP address), the CPE WAN Management Protocol can still be
 * used for management of the Device, but with the limitation that the Connection Request mechanism
 * over TCP cannot be used.
 */
typedef  struct
_CCSP_CWMP_STUN_INFO
{
    ULONG                           UDPConnectionRequestAddressNotificationLimit;
    BOOL                            STUNEnable;
    char                            STUNServerAddress[257];
    USHORT                          STUNServerPort;
    char                            STUNUsername[257];
    char                            STUNPassword[257];
    int                             STUNMaximumKeepAlivePeriod;
    ULONG                           STUNMinimumKeepAlivePeriod;
    BOOL                            NATDetected;
}
CCSP_CWMP_STUN_INFO,  *PCCSP_CWMP_STUN_INFO;

#define  CcspCwmpInitStunInfo(stun_info)                                                    \
         {                                                                                  \
            AnscZeroMemory(stun_info, sizeof(CCSP_CWMP_STUN_INFO));                         \
                                                                                            \
            stun_info->UDPConnectionRequestAddressNotificationLimit = 0;                    \
            stun_info->STUNEnable                                   = FALSE;                \
            stun_info->STUNServerPort                               = STUN_PORT;            \
            stun_info->STUNMaximumKeepAlivePeriod                   = -1;                   \
            stun_info->STUNMinimumKeepAlivePeriod                   = 15;                   \
            stun_info->NATDetected                                  = FALSE;                \
         }

#define  CcspCwmpCloneStunInfo(src_stun_info, dst_stun_info)                                \
         {                                                                                  \
            *dst_stun_info = *src_stun_info;                                                \
         }

#define  CcspCwmpCleanStunInfo(stun_info)                                                   \
         {                                                                                  \
         }

#define  CcspCwmpFreeStunInfo(stun_info)                                                    \
         {                                                                                  \
            CcspCwmpCleanStunInfo(stun_info);                                               \
            AnscFreeMemory   (stun_info);                                                   \
         }


#endif
