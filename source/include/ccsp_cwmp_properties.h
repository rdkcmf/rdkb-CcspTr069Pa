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

    module:	ccsp_cwmp_properties.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This file defines the configuration parameters that can be
        applied to different CCSP CWMP objects.

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
        04/25/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP COSA 2.0 CDMMM.
        10/12/11    resolve name conflict with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_PROPERTIES_
#define  _CCSP_CWMP_PROPERTIES_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ccsp_cwmp_definitions.h"
#include "ccsp_cwmp_definitions_general.h"

/***********************************************************
         CCSP CWMP CPE DEVICE OBJECT PROPERTY DEFINITION
***********************************************************/

/*
 * The CPE WAN Management Protocol allows an ACS to provision a CPE or collection of CPE based on a
 * variety of criteria. The provisioning mechanism includes specific provisioning parameters and a
 * general mechanism for adding vendor-specific provisioning capabilities as needed. The ACS at any
 * time request that the CPE initiate a connection to the ACS using the Connection Request notifi-
 * cation mechanism. Support for this mechanism is REQUIRED in a CPE, and is RECOMMENDED in an ACS.
 */
#define  PA_PSM_DEF_ROOT_NODE_NAME                  "Cisco.CCSP.COSA20.TR069PA.Primary"

typedef  struct
_CCSP_CWMP_CPE_CONTROLLER_PROPERTY
{
    BOOL                            bStartCwmpRightAway;
}
CCSP_CWMP_CPE_CONTROLLER_PROPERTY,  *PCCSP_CWMP_CPE_CONTROLLER_PROPERTY;

/*
 * The CPE WAN Management Protocol defines the following mechanisms that may be used by a CPE to
 * discover the address of its associated ACS:
 *
 *      1. The CPE may be configured locally with the URL of the ACS.
 *      2. As part of the IP layer auto-configuration, a DHCP server on the access
 *         network may be configured to include the ACS URL as a DHCP option.
 *      3. The CPE may have a default ACS URL that it may use if no other URL is provided
 *         to it.
 *
 * The ACS URL MUST be in the form of a valid HTTP or HTTPS URL. Use of an HTTPS URL indicates that
 * the ACS supports SSL. If an HTTPS URL is given, and the CPE that does not support SSL, it MAY
 * attempt to use HTTP assuming the remainder of the URL is unchanged.
 */
typedef  struct
_CCSP_CWMP_ACS_BROKER_PROPERTY
{
    ULONG                           Dummy;
}
CCSP_CWMP_ACS_BROKER_PROPERTY,  *PCCSP_CWMP_ACS_BROKER_PROPERTY;

/*
 * SOAP messages are carried between a CPE and an ACS using HTTP 1.1, where the CPE acts as the
 * HTTP client and the ACS acts as the HTTP server. The encoding of SOAP over HTTP extends the
 * basic HTTP profile for SOAP, as follows:
 *
 *      - A SOAP request from an ACS to a CPE is sent over a HTTP response, while the
 *        CPE's SOAP response to an ACS request is sent over a subsequent HTTP post.
 *      - Each HTTP post or response may contain more than one SOAP envelope (within the
 *        negotiated limits). Each envelope may contain a SOAP request or response,
 *        independent from any other envelope.
 *      - When there is more than one envelope in a single HTTP Request, the SOAPAction
 *        header in the HTTP Request MUST have no value (with no quotes), indicating that
 *        this header provides no information as to the intent of the message.
 */
typedef  struct
_CCSP_CWMP_PROCESSOR_PROPERTY
{
    BOOL                            bInitialContact;
    BOOL                            bPeriodicInformEnabled;
    ULONG                           PeriodicInformInterval;
    ANSC_UNIVERSAL_TIME             PeriodicInformTime;
    ULONG                           FirstTimeInterval;
    ULONG                           FollowingInterval;
    char                            AcsUrl[CCSP_CWMP_MAX_URL_SIZE];
	ULONG							DefActiveNotifThrottle;
	ULONG							LastActiveNotifTime;
}
CCSP_CWMP_PROCESSOR_PROPERTY,  *PCCSP_CWMP_PROCESSOR_PROPERTY;


/*
 * STUN is a lightweight protocol that allows applications to discover the presence and types of
 * NATs and firewalls between them and the public Internet. It also provides the ability for
 * applications to determine the public Internet Protocol addresses allocated to them by NAT. STUN
 * works with many existing NATs, and does not require any special behavior from them. As a result,
 * it allows a wide variety of applications to work through existing NAT infrastructure.
 */
typedef  struct
_CCSP_CWMP_STUN_MANAGER_PROPERTY
{
    ANSC_IPV4_ADDRESS               ClientAddr;
    USHORT                          ClientPort;
}
CCSP_CWMP_STUN_MANAGER_PROPERTY,  *PCCSP_CWMP_STUN_MANAGER_PROPERTY;


#endif
