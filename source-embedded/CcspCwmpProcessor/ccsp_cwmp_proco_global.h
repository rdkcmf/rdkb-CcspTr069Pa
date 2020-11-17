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

    module:	ccsp_cwmp_proco_global.h

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This header file includes all the header files required by
        the CCSP CWMP Processor implementation.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/12/05    initial revision.
        06/21/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.    
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_PROCO_GLOBAL_
#define  _CCSP_CWMP_PROCO_GLOBAL_


#include "ansc_platform.h"
#include "ccsp_tr069pa_wrapper_api.h"

#include "ansc_tso_interface.h"
#include "ansc_tso_external_api.h"

#include "ansc_ato_interface.h"
#include "ansc_ato_external_api.h"

#include "ccsp_cwmp_co_oid.h"
#include "ccsp_cwmp_co_name.h"
#include "ccsp_cwmp_co_type.h"
#include "ccsp_cwmp_properties.h"

#include "ccsp_cwmp_proco_interface.h"
#include "ccsp_cwmp_proco_exported_api.h"
#include "ccsp_cwmp_proco_internal_api.h"
#include "ccsp_cwmp_proco_layout.h"

#include "ccsp_cwmp_cpeco_interface.h"
#include "ccsp_cwmp_cpeco_exported_api.h"
#include "ccsp_cwmp_acsbo_interface.h"
#include "ccsp_cwmp_acsbo_exported_api.h"
#include "ccsp_cwmp_sesso_interface.h"
#include "ccsp_cwmp_sesso_exported_api.h"
#include "ccsp_cwmp_soappo_interface.h"
#include "ccsp_cwmp_soappo_exported_api.h"
#include "ccsp_cwmp_tcpcrho_interface.h"

#include "ccsp_cwmp_ifo_sta.h"
#include "ccsp_cwmp_ifo_cfg.h"
#include "ccsp_cwmp_ifo_mpa.h"

#include "ccsp_message_bus.h"
#include "ccsp_base_api.h"

/* used to sync namespaces with CR, search namespace etc. */
#include "ccsp_tr069pa_wrapper_api.h"
#include "ccsp_namespace_mgr.h"    
#include "ccsp_tr069pa_mapper_api.h"     
#include "ccsp_tr069pa_psm_keys.h"
#include "ccsp_psm_helper.h"
#include "ccsp_management_server_pa_api.h"
#include "ccsp_management_server.h"
#include "ccsp_tr069_cds_definitions.h"
#include "ccsp_rpc_ns_map.h"
#include "ccsp_cwmp_helper_api.h"

#ifdef   _CCSP_CWMP_STUN_ENABLED
    #include "ccsp_cwmp_stunmo_interface.h"
    #include "ccsp_cwmp_stunmo_exported_api.h"
    #include "stun_sco_interface.h"
#endif


#endif
