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

    module:	ccsp_tr069pa_psm_keys.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This file defines PSM keys TR-069 PA is going to use to
        save runtime configuration.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        06/15/2011    initial revision.

**********************************************************************/

#ifndef  _CCSP_TR069_PSM_KEYS_DEF_
#define  _CCSP_TR069_PSM_KEYS_DEF_

#define  CCSP_TR069PA_PSM_NODE_NAME_MAX_LEN         512

#define  CCSP_TR069PA_PSM_KEY_InitialContact        "cwmp.initialContact"
#define  CCSP_TR069PA_PSM_KEY_InitialContactFactory "cwmp.initialContactFactory"
#define  CCSP_TR069PA_PSM_KEY_SavedEvents           "cwmp.savedEvents"
#define  CCSP_TR069PA_PSM_KEY_TriggerCommand        "cwmp.triggerCommand"
#define  CCSP_TR069PA_PSM_KEY_TriggerCommandKey     "cwmp.triggerCommandKey"

#define  CCSP_TR069PA_PSM_KEY_TransferComplete      "cwmp.TransferComplete"

#endif

