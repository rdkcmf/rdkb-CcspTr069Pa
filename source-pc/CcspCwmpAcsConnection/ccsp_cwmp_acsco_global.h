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

    module: ccsp_cwmp_acsco_global.h

        For DSL Home Project

    ---------------------------------------------------------------

    description:

        This header file includes all the header files required by
        the CCSP CWMP ACS Connection implementation.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/21/05    initial revision.
        10/13/11    resolve name conflict with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_ACSCONN_GLOBAL_
#define  _CCSP_CWMP_ACSCONN_GLOBAL_

#include "ansc_platform.h"
#include "ccsp_tr069pa_wrapper_api.h"
#include "slap_definitions.h"

#include "ccsp_cwmp_co_oid.h"
#include "ccsp_cwmp_co_name.h"
#include "ccsp_cwmp_ifo_mco.h"
#include "ccsp_cwmp_acsco_interface.h"
#include "ccsp_cwmp_acsco_exported_api.h"
#include "ccsp_cwmp_acsco_internal_api.h"
#include "ccsp_cwmp_cpeco_interface.h"
#include "ccsp_cwmp_ifo_sta.h"
#include "ccsp_cwmp_ifo_cfg.h"
#include "ccsp_cwmp_proco_interface.h"

#include "http_hco_interface.h"
#include "http_hco_exported_api.h"
#include "http_sco_interface.h"
#include "http_sco_exported_api.h"
#include "http_properties.h"

#include "http_ifo_bsp.h"
#include "http_ifo_hfp.h"
#include "http_ifo_cas.h"

#include "http_authco_interface.h"
#include "http_authco_exported_api.h"

#include "http_wcso_interface.h"
#include "http_wcso_exported_api.h"
#include "http_wcto_interface.h"
#include "http_wcto_exported_api.h"

#include "http_bmo_interface.h"
#include "http_bmo_exported_api.h"
#include "http_bmoreq_interface.h"
#include "http_bmoreq_exported_api.h"
#include "http_bmorep_interface.h"
#include "http_bmorep_exported_api.h"

#include "ccsp_cwmp_sesso_interface.h"
#include "ccsp_cwmp_sesso_interface.h"

#endif
