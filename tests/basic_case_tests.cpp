#include <cppunit/extensions/HelperMacros.h>

#include <CmdLineConfig.hh>

class BasicCase : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( BasicCase );
    CPPUNIT_TEST( MyTest );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {}

protected:
    void MyTest()
    {
        CmdLineOption bool_val("CustomBoolArgName", "-bool", "Help message" , false);
        CmdLineOption int_val("CustomIntegerArgName", "-int", "Help message" , 13);
        CmdLineOption double_val("CustomDoubleArgName", "-double", "Help message" , 3.1415);
        CmdLineOption string_val("CustomStringArgName", "-string", "Help message" , "pi");

        const char * argv[] = { "./prog", "positional1", "-bool", "-int", "112358", "positional2", "-double", "2.71", "-string", "test string", "positional3" };
        CmdLineConfig::instance()->ReadCmdLine(11, (char **)argv);

        CPPUNIT_ASSERT_EQUAL(true, CmdLineOption::GetBoolValue("CustomBoolArgName"));
        CPPUNIT_ASSERT_EQUAL(112358, CmdLineOption::GetIntValue("CustomIntegerArgName"));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(2.71, CmdLineOption::GetDoubleValue("CustomDoubleArgName"), 0.0005);
        CPPUNIT_ASSERT_EQUAL(std::string("test string"), std::string(CmdLineOption::GetStringValue("CustomStringArgName")));

        PositionalArgs pargs = CmdLineOption::GetPositionalArguments();
        CPPUNIT_ASSERT_EQUAL(TString("positional1"), pargs[0]);
        CPPUNIT_ASSERT_EQUAL(TString("positional2"), pargs[1]);
        CPPUNIT_ASSERT_EQUAL(TString("positional3"), pargs[2]);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( BasicCase );
