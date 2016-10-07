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
    std::ofstream ofstr("TEST-carolocup-control-example-ExampleTestSuite.h.xml");
    CxxTest::XUnitPrinter tmp(ofstr);
    CxxTest::RealWorldDescription::_worldName = "carolocup-control-example-ExampleTestSuite.h";
    status = CxxTest::Main< CxxTest::XUnitPrinter >( tmp, argc, argv );
    return status;
}
bool suite_ExampleTest_init = false;
#include "../../../../opendlv.carolocup.sources/code/control/example/testsuites/ExampleTestSuite.h"

static ExampleTest suite_ExampleTest;

static CxxTest::List Tests_ExampleTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_ExampleTest( "/opt/opendlv.carolocup.sources/code/control/example/testsuites/ExampleTestSuite.h", 31, "ExampleTest", suite_ExampleTest, Tests_ExampleTest );

static class TestDescription_suite_ExampleTest_testApplication : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_ExampleTest_testApplication() : CxxTest::RealTestDescription( Tests_ExampleTest, suiteDescription_ExampleTest, 37, "testApplication" ) {}
 void runTest() { suite_ExampleTest.testApplication(); }
} testDescription_suite_ExampleTest_testApplication;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
