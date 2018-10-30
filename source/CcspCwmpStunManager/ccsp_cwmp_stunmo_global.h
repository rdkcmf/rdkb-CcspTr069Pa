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

    module: ccsp_cwmp_stunmo_global.h

        For CCSP CWMP protocol 

    ---------------------------------------------------------------

    description:

        This header file includes all the header files required by
        the CCSP CWMP Stun Manager implementation.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/05/08    initial revision.
        11/02/11    migrate to CCSP framework

**********************************************************************/


#ifndef  _CCSP_CWMP_STUNMO_GLOBAL_
#define  _CCSP_CWMP_STUNMO_GLOBAL_


#include "ansc_platform.h"
#include "slap_definitions.h"

#include "ansc_crypto_interface.h"
#include "ansc_crypto_external_api.h"

#include "ccsp_tr069pa_wrapper_api.h"
#include "ccsp_cwmp_co_oid.h"
#include "ccsp_cwmp_co_name.h"
#include "ccsp_cwmp_stunmo_interface.h"
#include "ccsp_cwmp_stunmo_exported_api.h"
#include "ccsp_cwmp_stunmo_internal_api.h"

#include "ccsp_cwmp_cpeco_interface.h"
#include "ccsp_cwmp_cpeco_exported_api.h"
#include "ccsp_cwmp_proco_interface.h"
#include "ccsp_cwmp_proco_exported_api.h"
#include "ccsp_cwmp_sesso_interface.h"
#include "ccsp_cwmp_sesso_exported_api.h"
#include "ccsp_cwmp_acsbo_interface.h"
#include "ccsp_cwmp_acsbo_exported_api.h"
#include "ccsp_cwmp_ifo_mso.h"

#include "stun_sco_interface.h"
#include "stun_sco_exported_api.h"
#include "stun_definitions.h"

#include "ccsp_management_server_pa_api.h"

#endif
