#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define GTEST_REPORT_DIRECTORY "/tmp/Gtest_Report"
#define GTEST_REPORT_FILEPATH "CcspTr069PaSsp_gtest_report.xml"
#define GTEST_REPORT_FILEPATH_SIZE 128

GTEST_API_ int main(int argc, char *argv[])
{
    char filePath[GTEST_REPORT_FILEPATH_SIZE] = {0}; // Test Results Full File Path
    snprintf(filePath, GTEST_REPORT_FILEPATH_SIZE, "xml:%s/%s",GTEST_REPORT_DIRECTORY,GTEST_REPORT_FILEPATH);
    ::testing::GTEST_FLAG(output) = filePath;
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
