/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <fstream>
#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/XUnitPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    std::ofstream ofstr("TEST-opendlv-core-system-proxy-camera-ProxyCameraTestSuite.h.xml");
    CxxTest::XUnitPrinter tmp(ofstr);
    CxxTest::RealWorldDescription::_worldName = "opendlv-core-system-proxy-camera-ProxyCameraTestSuite.h";
    status = CxxTest::Main< CxxTest::XUnitPrinter >( tmp, argc, argv );
    return status;
}
bool suite_ProxyCameraTest_init = false;
#include "../../../../../opendlv.carolocup.sources/code/proxy-camera/testsuites/ProxyCameraTestSuite.h"

static ProxyCameraTest suite_ProxyCameraTest;

static CxxTest::List Tests_ProxyCameraTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ProxyCameraTest( "/opt/opendlv.carolocup.sources/code/proxy-camera/testsuites/ProxyCameraTestSuite.h", 49, "ProxyCameraTest", suite_ProxyCameraTest, Tests_ProxyCameraTest );

static class TestDescription_suite_ProxyCameraTest_testApplication : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ProxyCameraTest_testApplication() : CxxTest::RealTestDescription( Tests_ProxyCameraTest, suiteDescription_ProxyCameraTest, 73, "testApplication" ) {}
 void runTest() { suite_ProxyCameraTest.testApplication(); }
} testDescription_suite_ProxyCameraTest_testApplication;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
