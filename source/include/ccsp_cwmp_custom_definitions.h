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

    module:	ccsp_cwmp_custom_definitions.h

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

        Kang Quan

    ---------------------------------------------------------------

    revision:

        08/01/08    initial revision.

**********************************************************************/


#ifndef  _CCSP_CWMP_CUSTOM_DEFINITIONS_
#define  _CCSP_CWMP_CUSTOM_DEFINITIONS_

#include "dslgm_module_custom.h"

/* this flag must be turned off in future if
 * FCs save parameter attributes into PSM. As
 * an alternative, when PA is loading, it pushes
 * VC signal subscription on all parameters
 * on which  ACS has ever set active or passive
 * notification.
 */
#define  _CCSP_TR069PA_PUSH_VC_SIGNAL_ON_LOAD

#endif
