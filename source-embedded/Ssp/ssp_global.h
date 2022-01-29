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

#ifndef  _CCSP_TR069PA_SSP_DEF_
#define  _CCSP_TR069PA_SSP_DEF_

#include <time.h>

#include "ansc_platform.h"
#include "ansc_string.h"

#include "ccsp_cwmp_cpeco_interface.h"
#include "ccsp_cwmp_cpeco_exported_api.h"

#include "ccsp_cwmp_proco_interface.h"
#include "ccsp_cwmp_proco_exported_api.h"

#ifdef   _CCSP_CWMP_TCP_CONNREQ_HANDLER
#include "ccsp_cwmp_tcpcrho_interface.h"
#include "ccsp_cwmp_tcpcrho_exported_api.h"
#endif

#include "web_ifo_acm.h"

#include "ccsp_cwmp_ifo_mso.h"
#include "ccsp_cwmp_ifo_cfg.h"
#include "ccsp_cwmp_ifo_sta.h"
#include "ccsp_cwmp_ifo_mpa.h"

#include "ssp_web_acm.h"

#include "ssp_ccsp_cwmp_cfg.h"


#include "ccsp_management_server_pa_api.h"
#include "ccsp_tr069pa_wrapper_api.h"
#include "ccsp_base_api.h"
#include "ccsp_trace.h"

#include "dslh_dmagnt_interface.h"

/*
 *  Define custom trace module ID
 */
#ifdef   ANSC_TRACE_MODULE_ID
    #undef  ANSC_TRACE_MODULE_ID
#endif

#define  ANSC_TRACE_MODULE_ID                       ANSC_TRACE_ID_SSP

int  cmd_dispatch(int  command);
int  init_interfaces();

int  engage_tr069pa();
int  cancel_tr069pa();


/* some unit test functions */
#ifdef   _DEBUG
#include "ccsp_types.h"
#include "ccsp_tr069pa_mapper_api.h"
#include "slap_definitions.h"
#include "slap_vho_exported_api.h"
#include "ccsp_cwmp_helper_api.h"
void ssp_dumpPiTree();
extern void CcspCwmppoSysReadySignalCB(void* user_data); /* for simulating reception of systemReady signal */
extern void CcspCwmppoProcessSysReadySignal (void*	cbContext);

void ssp_testNsSyncWithCR();
void ssp_testNsAgainstMapper();
void ssp_testGPN();
void ssp_testGPV();
void ssp_testSPV();
void ssp_testGPA();
void ssp_testSPA();
void ssp_testCDS();
void ssp_testFirmwareDownload();
void ssp_testGetMethods();
void ssp_testAddObject();
void ssp_testDeleteObject();
void ssp_testSPV_notFromPA();
void ssp_setLogLevel();
void ssp_checkCachedParamAttr();
void ssp_flipInvNamespaceChkFlag();
void ssp_sendValueChangeSignal();
void ssp_sendDiagCompleteSignal();
#endif

#endif /* _CCSP_TR069PA_SSP_DEF_ */
