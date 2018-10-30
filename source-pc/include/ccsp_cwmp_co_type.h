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

    module:	ccsp_cwmp_co_type.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This wrapper file defines the object types for the CCSP CWMP
        Component Objects.

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


#ifndef  _CCSP_CWMP_CO_TYPE_
#define  _CCSP_CWMP_CO_TYPE_


/***********************************************************
            GENERAL CCSP CWMP FEATURE OBJECTS DEFINITION
***********************************************************/

/*
 * Define the object names for all the Feature Objects that cannot be categorized. Feature Objects
 * are the objects that encapsulate certain features and provide services.
 */
#define  CCSP_CWMP_CPE_CONTROLLER_TYPE              ANSC_OBJECT_TYPE_NULL
#define  CCSP_CWMP_ACS_BROKER_TYPE                  ANSC_OBJECT_TYPE_NULL
#define  CCSP_CWMP_PROCESSOR_TYPE                   ANSC_OBJECT_TYPE_NULL
#define  CCSP_CWMP_SESSION_TYPE                     ANSC_OBJECT_TYPE_NULL
#define  CCSP_CWMP_ACS_CONNECTION_TYPE              ANSC_OBJECT_TYPE_NULL
#define  CCSP_CWMP_SOAP_PARSER_TYPE                 ANSC_OBJECT_TYPE_NULL
#define  CCSP_CWMP_STUN_MANAGER_TYPE                ANSC_OBJECT_TYPE_NULL

#endif
