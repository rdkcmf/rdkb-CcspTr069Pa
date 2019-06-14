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

    module: ccsp_cwmp_definitions_soap.h

        For DSL Home Project

    ---------------------------------------------------------------

    description:

        This header file contains the prototype definition for all
        the internal functions provided by the CCSP CWMP SoapParser
        Parser Object.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        10/28/05    initial revision.
        10/12/11    resolve name conflicts with DM library.

**********************************************************************/


#ifndef  _CCSP_CWMP_SOAPPO_DEFINITIONS_H
#define  _CCSP_CWMP_SOAPPO_DEFINITIONS_H

/***********************************************************
         CONST PARAMETERS FOR CCSP_CWMP_SOAP_PARSER
***********************************************************/
#define  CHAR_COLON                       ":"

#define  SOAP_ENVELOPE                    "Envelope"
#define  SOAP_ENVELOPE_2                  ":Envelope"
#define  SOAP_NAMESPACE_NAME              "xmlns" 
#define  DEFAULT_SOAP_NAMESPACE           "http://schemas.xmlsoap.org/soap/envelope/"
#define  NODE_SOAP_ENCODING               "soap-enc"
#define  DEFAULT_SOAP_ENCODING            "http://schemas.xmlsoap.org/soap/encoding/"
#define  SOAP_MUSTUNDERSTAND              "mustUnderstand"
#define  SOAP_VALUE_TRUE                  1  /* value for "mustUnderstand" */
#define  SOAP_FAULT                       "Fault"
#define  SOAP_FAULTCODE                   "faultcode"
#define  SOAP_FAULTSTRING                 "faultstring"
#define  SOAP_DETAIL                      "detail"
#define  SOAP_HEADER_NODE                 "Header"
#define  SOAP_BODY_NODE                   "Body"
#define  PART_RESPONSE_NAME               "Response"

#define  CCSP_CWMP_NAMESPACE_NAME         "xmlns:cwmp"
#if defined(CONFIG_CWMP_NAMESPACE_VALUE_1_2)
    #define  CCSP_CWMP_NAMESPACE_VALUE        "urn:dslforum-org:cwmp-1-2"
#else
    #define  CCSP_CWMP_NAMESPACE_VALUE        "urn:dslforum-org:cwmp-1-0"
#endif

#define  SOAP_XSI_NAMESPACE_NAME          "xsi"
#define  SOAP_XSI_NAMESPACE_VALUE         "http://www.w3.org/2001/XMLSchema-instance"
#define  SOAP_XSD_NAMESPACE_NAME          "xsd"
#define  SOAP_XSD_NAMESPACE_VALUE         "http://www.w3.org/2001/XMLSchema"


#define  CCSP_CWMP_ID                     "ID"
#define  CCSP_CWMP_HOLDREQUESTS           "HoldRequests"
#define  CCSP_CWMP_NOMOREREQUESTS         "NoMoreRequests"
#define  CCSP_CWMP_FAULT_NODE             "Fault"
#define  CCSP_CWMP_FAULTCODE              "FaultCode"
#define  CCSP_CWMP_FAULTSTRING            "FaultString"
#define  CCSP_CWMP_SETFAULT               "SetParameterValuesFault"
#define  CCSP_CWMP_PARAMETERNAME          "ParameterName"

#define  SOAP_PARAM_TYPE_ATTR_NAME        "xsi:type"
#define  SOAP_PARAM_TYPE_STRING           "xsd:string"

#define  CCSP_CWMP_COMMA                  ","

#define  CCSP_CWMP_NAMESPACE              "cwmp"
#define  SOAP_NAMESPACE_VALUE             "soap"

#define  SOAP_FAULTCODE_CLIENT            "Client"
#define  SOAP_FAULTCODE_SERVER            "Server"
#define  SOAP_FAULTSTRING_VALUE           "CWMP fault"


#endif
