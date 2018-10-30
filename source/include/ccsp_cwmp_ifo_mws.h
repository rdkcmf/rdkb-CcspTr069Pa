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

    module:	ccsp_cwmp_ifo_mws.h

        For CCSP CWMP protocol implemenation

    ---------------------------------------------------------------

    description:

        This wrapper file defines the base class data structure and
        interface for CCSP CWMP Mini Web Service Objects.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan

    ---------------------------------------------------------------

    revision:

        02/17/2011      initial revision.
        10/12/2011      resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_IFO_MWS_
#define  _CCSP_CWMP_IFO_MWS_


/*
 * This object is derived a virtual base object defined by the underlying framework. We include the
 * interface header files of the base object here to shield other objects from knowing the derived
 * relationship between this object and its base class.
 */
#include "ansc_ifo_interface.h"


/***********************************************************
    CCSP CWMP MINI WEB SERVICE INTERFACE DEFINITION
***********************************************************/

/*
 * Define some const values that will be used in the os wrapper object definition.
 */
#define  CCSP_CWMP_MWS_INTERFACE_NAME                    "cwmpMiniWebSvcIf"
#define  CCSP_CWMP_MWS_INTERFACE_ID                      0


/*
 * Since we write all kernel modules in C (due to better performance and lack of compiler support), we
 * have to simulate the C++ object by encapsulating a set of functions inside a data structure.
 */
typedef  char*
(*PFN_CWMPMWSIF_GET_RES)
    (
        ANSC_HANDLE                 hThisObject,
        char*                       pUrlPath,           /* absolute URL path to web root */
        char*                       pMediaType,         /* OUT - media type associated with the URL, used to indicate content-type */
        PULONG                      pulMediaTypeLength, /* IN/OUT */
        PULONG                      pulResourceLen      /* OUT - length of resource */
    );


/*
 * As a general requirement, each module SHOULD provide an interface for external components to
 * configure its parameters and policies. Although the benefit of unifying the definition and usage
 * of such an interface is obvious, we DON'T want to impose any set of rules on the implementation.
 * Instead, we expect every module will implement its configuration interfaces independently.
 */
#define  CCSP_CWMP_MWS_INTERFACE_CLASS_CONTENT                                                   \
    /* duplication of the base object class content */                                      \
    ANSCIFO_CLASS_CONTENT                                                                   \
    /* start of object class content */                                                     \
    PFN_CWMPMWSIF_GET_RES           GetResource;                                            \
    /* end of object class content */                                                       \

typedef  struct
_CCSP_CWMP_MWS_INTERFACE
{
    CCSP_CWMP_MWS_INTERFACE_CLASS_CONTENT
}
CCSP_CWMP_MWS_INTERFACE,  *PCCSP_CWMP_MWS_INTERFACE;

#define  ACCESS_CCSP_CWMP_MWS_INTERFACE(p)               \
         ACCESS_CONTAINER(p, CCSP_CWMP_MWS_INTERFACE, Linkage)


#endif
