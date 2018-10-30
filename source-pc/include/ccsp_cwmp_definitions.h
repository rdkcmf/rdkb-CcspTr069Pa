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

    module:	ccsp_cwmp_definitions.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This file defines the configuration parameters that can be
        applied to the CCSP CWMP.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/30/05    initial revision.
        10/12/11    resolve name conflicts with DM library

**********************************************************************/


#ifndef  _CCSP_CWMP_DEFINITIONS_
#define  _CCSP_CWMP_DEFINITIONS_


#define  CCSP_CWMP_MAX_URL_SIZE                          256
#define  CCSP_CWMP_MAX_HOSTNAME_SIZE                     64
#define  CCSP_CWMP_MAX_CONNNAME_SIZE                     256
#define  CCSP_CWMP_MAX_ZONENAME_SIZE                     64
#define  CCSP_CWMP_MAX_LINKNAME_SIZE                     64
#define  CCSP_CWMP_MAX_USERNAME_SIZE                     256
#define  CCSP_CWMP_MAX_PASSWORD_SIZE                     256
#define  CCSP_CWMP_MAX_DESCRIPTION_SIZE                  256
#define  CCSP_CWMP_MAX_PROVIDER_SIZE                     256
#define  CCSP_CWMP_MAX_BRIDGENAME_SIZE                   64
#define  CCSP_CWMP_MAX_INTERFACE_SIZE                    256
#define  CCSP_CWMP_MAX_APPNAME_SIZE                      64
#define  CCSP_CWMP_MAX_DHCPOPTION_SIZE                   256
#define  CCSP_CWMP_MAX_CR_URL_PATH_SIZE                  64

#include "ccsp_cwmp_custom_definitions.h"
#include "ccsp_cwmp_definitions_cwmp.h"

#endif
