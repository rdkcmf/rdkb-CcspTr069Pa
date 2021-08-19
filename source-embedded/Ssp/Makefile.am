##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2015 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
AM_CFLAGS =
AM_CFLAGS += -D_CCSP_CWMP_TCP_CONNREQ_HANDLER

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = CcspTr069PaSsp

CcspTr069PaSsp_CPPFLAGS = -I$(top_srcdir)/source-embedded/DslhManagementServer -I$(top_srcdir)/source-embedded/include
CcspTr069PaSsp_SOURCES = ssp_main.c ssp_ccsp_cwmp_cfg.c ssp_web_acm.c ssp_debug.c ccsp_tr069_pa_custom_apis.c \
    ../CcspCwmpAcsConnection/ccsp_cwmp_acsco_process.c \
    ../CcspCwmpAcsConnection/ccsp_cwmp_acsco_states.c \
    ../CcspCwmpAcsConnection/ccsp_cwmp_acsco_base.c \
    ../CcspCwmpAcsConnection/ccsp_cwmp_acsco_httpbspif.c \
    ../CcspCwmpAcsConnection/ccsp_cwmp_acsco_httpacmif.c \
    ../CcspCwmpAcsConnection/ccsp_cwmp_acsco_interface.c \
    ../CcspCwmpProcessor/ccsp_cwmp_proco_operation.c \
    ../CcspCwmpProcessor/ccsp_cwmp_proco_management.c \
    ../CcspCwmpProcessor/ccsp_cwmp_proco_base.c \
    ../CcspCwmpProcessor/ccsp_cwmp_proco_states.c \
    ../CcspCwmpProcessor/ccsp_cwmp_proco_mpaif.c \
    ../CcspCwmpProcessor/ccsp_cwmp_proco_acs_access.c \
    ../CcspCwmpProcessor/ccsp_cwmp_proco_interface.c \
    ../CcspCwmpProcessor/ccsp_cwmp_proco_event.c \
    ../CcspCwmpProcessor/ccsp_cwmp_proco_process.c \
    ../CcspCwmpSession/ccsp_cwmp_sesso_management.c \
    ../CcspCwmpSession/ccsp_cwmp_sesso_mcoif.c \
    ../CcspCwmpSession/ccsp_cwmp_sesso_interface.c \
    ../CcspCwmpSession/ccsp_cwmp_sesso_states.c \
    ../CcspCwmpSession/ccsp_cwmp_sesso_process.c \
    ../CcspCwmpSession/ccsp_cwmp_sesso_control.c \
    ../CcspCwmpSession/ccsp_cwmp_sesso_base.c \
    ../CcspCwmpSession/ccsp_cwmp_sesso_operation.c \
    ../CcspCwmpSession/ccsp_cwmp_sesso_invoke.c \
    ../CcspCwmpStunManager/ccsp_cwmp_stunmo_states.c \
    ../CcspCwmpStunManager/ccsp_cwmp_stunmo_reg_stun.c \
    ../CcspCwmpStunManager/ccsp_cwmp_stunmo_base.c \
    ../CcspCwmpStunManager/ccsp_cwmp_stunmo_operation.c \
    ../CcspCwmpStunManager/ccsp_cwmp_stunmo_interface.c \
    ../CcspCwmpStunManager/ccsp_cwmp_stunmo_policy.c \
    ../CcspCwmpStunManager/ccsp_cwmp_stunmo_stunbsmif.c \
    ../DslhManagementServer/ccsp_management_server.c \
    ../DslhManagementServer/ccsp_management_server_api.c \
    ../DslhManagementServer/ccsp_management_server_pa_api.c \
    ../DslhManagementServer/ccsp_supported_data_model.c \
    ../CcspCwmpAcsBroker/ccsp_cwmp_acsbo_interface.c \
    ../CcspCwmpAcsBroker/ccsp_cwmp_acsbo_states.c \
    ../CcspCwmpAcsBroker/ccsp_cwmp_acsbo_msoif.c \
    ../CcspCwmpAcsBroker/ccsp_cwmp_acsbo_base.c \
    ../CcspCwmpAcsBroker/ccsp_cwmp_acsbo_operation.c \
    ../CcspCwmpTcpConnReqHandler/ccsp_cwmp_tcpcrho_auth.c \
    ../CcspCwmpTcpConnReqHandler/ccsp_cwmp_tcpcrho_base.c \
    ../CcspCwmpTcpConnReqHandler/ccsp_cwmp_tcpcrho_interface.c \
    ../CcspCwmpTcpConnReqHandler/ccsp_cwmp_tcpcrho_states.c \
    ../CcspCwmpTcpConnReqHandler/ccsp_cwmp_tcpcrho_dstowo.c \
    ../CcspCwmpTcpConnReqHandler/ccsp_cwmp_tcpcrho_operation.c \
    ../CcspCwmpTcpConnReqHandler/ccsp_cwmp_tcpcrho_msg.c \
    ../CcspCwmpTcpConnReqHandler/ccsp_cwmp_tcpcrho_process.c \
    ../CcspCwmpCpeController/ccsp_cwmp_cpeco_states.c \
    ../CcspCwmpCpeController/ccsp_cwmp_cpeco_operation.c \
    ../CcspCwmpCpeController/ccsp_tr069pa_mapper_api.c \
    ../CcspCwmpCpeController/ccsp_cwmp_cpeco_control.c \
    ../CcspCwmpCpeController/ccsp_cwmp_cpeco_access.c \
    ../CcspCwmpCpeController/ccsp_cwmp_cpeco_interface.c \
    ../CcspCwmpCpeController/ccsp_cwmp_cpeco_base.c \
    ../CcspCwmpCpeController/ccsp_cwmp_cpeco_staif.c \
    ../CcspCwmpSoapParser/ccsp_cwmp_soappo_base.c \
    ../CcspCwmpSoapParser/ccsp_cwmp_soappo_interface.c \
    ../CcspCwmpSoapParser/ccsp_cwmp_soappo_process.c \
    ../CcspCwmpSoapParser/ccsp_cwmp_soappo_utility.c \
    ../CcspCwmpSoapParser/ccsp_cwmp_soappo_states.c \
    ../CcspCwmpSoapParser/ccsp_cwmp_soappo_build.c

CcspTr069PaSsp_LDFLAGS = -lccsp_common -lsyscfg -lsysevent -lutapi -lutctx -ltelemetry_msgsender -lsecure_wrapper -lprint_uptime -lprivilege -lcjson -lcrypto
