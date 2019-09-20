##########################################################################
# Copyright 2018-2019 ARRIS Enterprises, LLC.
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
AM_CPPLAGS = -D_ANSC_LINUX -D_ANSC_USER -D_ANSC_LITTLE_ENDIAN_
AM_CFLAGS = -Wall -Werror

noinst_LTLIBRARIES = libCcspCommonLibrary_CCSP_AliasMgr.la
libCcspCommonLibrary_CCSP_AliasMgr_la_SOURCES = \
 ccsp_alias_mgr_base.c \
 ccsp_alias_mgr_helper.c

libCcspCommonLibrary_CCSP_AliasMgr_la_CPPFLAGS = \
 $(AM_CPPFLAGS) \
 -I$(top_srcdir)/source/cosa/include/linux \
 -I$(top_srcdir)/source/debug_api/include \
 -I$(top_srcdir)/source/ccsp/components/common/PoamIrepFolder \
 -I$(top_srcdir)/source/cosa/include \
 -I$(top_srcdir)/source/cosa/utilities/include \
 -I$(top_srcdir)/source/cosa/custom \
 -I$(top_srcdir)/source/cosa/package/bmw2/bwsp/include \
 -I$(top_srcdir)/source/cosa/package/bmw2/include \
 -I$(top_srcdir)/source/cosa/package/bmw2/bree/include \
 -I$(top_srcdir)/source/cosa/package/bmw2/beep/include \
 -I$(top_srcdir)/source/cosa/package/bmw2/bwrm/include \
 -I$(top_srcdir)/source/cosa/package/system/include \
 -I$(top_srcdir)/source/cosa/package/slap/include \
 -I$(top_srcdir)/source/cosa/package/slap/services/dslh/include \
 -I$(top_srcdir)/source/cosa/package/slap/services/bmc2/include \
 -I$(top_srcdir)/source/cosa/package/cli/include \
 -I$(top_srcdir)/source/cosa/package/bmc2/include \
 -I$(top_srcdir)/source/debug_api/include \
 -I$(top_srcdir)/source/ccsp/include \
 -I$(top_srcdir)/source/ccsp/components/include \
 -I$(top_srcdir)/source/ccsp/components/common/MessageBusHelper/include \
 -I$(top_srcdir)/source/ccsp/custom \
 -I$(top_srcdir)/source/util_api/tls/include \
 -I$(top_srcdir)/source/util_api/web/include \
 -I$(top_srcdir)/source/util_api/ansc/include \
 -I$(top_srcdir)/source/util_api/stun/include \
 -I$(top_srcdir)/source/util_api/http/include \
 -I$(top_srcdir)/source/util_api/http/utilities/include \
 -I$(top_srcdir)/source/util_api/asn.1/include
