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

    module:	ccsp_cwmp_acsbo_global.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This header file includes all the header files required by
        the CCSP CWMP Acs Broker implementation.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Xuechen Yang
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/29/05    initial revision.
        06/20/11    decouple TR-069 PA from Data Model Manager
                    and make it work with CCSP architecture.

**********************************************************************/


#ifndef  _CCSP_CWMP_ACSBO_GLOBAL_
#define  _CCSP_CWMP_ACSBO_GLOBAL_


#include "ansc_platform.h"
#include "ccsp_tr069pa_wrapper_api.h"
#include "ansc_tso_interface.h"
#include "ansc_tso_external_api.h"

#include "ccsp_cwmp_co_oid.h"
#include "ccsp_cwmp_co_name.h"
#include "ccsp_cwmp_co_type.h"
#include "ccsp_cwmp_properties.h"

#include "ccsp_cwmp_acsbo_interface.h"
#include "ccsp_cwmp_acsbo_exported_api.h"
#include "ccsp_cwmp_acsbo_internal_api.h"

#include "ccsp_cwmp_cpeco_interface.h"
#include "ccsp_cwmp_cpeco_exported_api.h"
#include "ccsp_cwmp_proco_interface.h"
#include "ccsp_cwmp_proco_exported_api.h"
#include "ccsp_cwmp_sesso_interface.h"
#include "ccsp_cwmp_sesso_exported_api.h"
#include "ccsp_cwmp_soappo_interface.h"
#include "ccsp_cwmp_soappo_exported_api.h"

#endif
