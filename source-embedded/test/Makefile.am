AM_CFLAGS =

AM_CXXFLAGS = -std=c++11

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = CcspTr069PaSsp_gtest.bin

CcspTr069PaSsp_gtest_bin_CPPFLAGS = -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/gtest -I${top_srcdir}/gtest/include -I${top_srcdir}/source -I${top_srcdir}/source/include
CcspTr069PaSsp_gtest_bin_SOURCES = CcspTr069PaSspTest.cpp gtest_main.cpp
CcspTr069PaSsp_gtest_bin_LDFLAGS = -lgtest -lgmock -lgcov
