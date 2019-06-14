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

    module: ccsp_cwmp_soappo_global.h

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This header file includes all the header files required by
        the CCSP CWMP SoapParser implementation.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/06/05    initial revision.
        07/26/2011  add parsing/building ChangeDUState related
                    RPC requests and responses.
        10/13/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_SOAPPO_GLOBAL_
#define  _CCSP_CWMP_SOAPPO_GLOBAL_

#include "ansc_platform.h"
#include "ccsp_tr069pa_wrapper_api.h"

#include "slap_definitions.h"

#include "ccsp_cwmp_co_oid.h"
#include "ccsp_cwmp_co_name.h"
#include "ccsp_cwmp_ifo_mco.h"
#include "ccsp_cwmp_definitions_database.h"
#include "ccsp_cwmp_soappo_interface.h"
#include "ccsp_cwmp_soappo_exported_api.h"
#include "ccsp_cwmp_soappo_internal_api.h"

#include "ansc_crypto_interface.h"
#include "ansc_crypto_external_api.h"

#include "ansc_xml_dom_parser_interface.h"
#include "ansc_xml_dom_parser_external_api.h"
#include "ansc_xml_dom_parser_status.h"

#include "ccsp_tr069_cds_definitions.h"


#endif
