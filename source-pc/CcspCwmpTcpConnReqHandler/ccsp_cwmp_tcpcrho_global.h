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

    module:	ccsp_cwmp_tcpcrho_global.h

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This header file includes all the header files required by
        CCSP CWMP TCP Connection Request Handler object implementation.

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


#ifndef  _CCSP_CWMP_TCPCRHO_GLOBAL_
#define  _CCSP_CWMP_TCPCRHO_GLOBAL_


#include "ansc_platform.h"
#include "ccsp_tr069pa_wrapper_api.h"
#include "ccsp_cwmp_helper_api.h"

#include "ansc_socket.h"
#include "ansc_socket_external_api.h"

#include "ansc_dsto_interface.h"
#include "ansc_dsto_external_api.h"
#include "ansc_deto_interface.h"
#include "ansc_deto_external_api.h"
#include "ansc_dkto_interface.h"
#include "ansc_dkto_external_api.h"
#include "ansc_crypto_external_api.h"
#include "ansc_crypto_interface.h"

#include "ccsp_cwmp_tcpcrho_interface.h"
#include "ccsp_cwmp_tcpcrho_exported_api.h"
#include "ccsp_cwmp_tcpcrho_internal_api.h"

#include "ccsp_cwmp_cpeco_interface.h"
#include "ccsp_cwmp_acsbo_interface.h"
#include "ccsp_cwmp_ifo_mso.h"

#include "web_ifo_acm.h"
#include "ccsp_cwmp_ifo_mws.h"


#endif
