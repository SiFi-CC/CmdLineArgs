#include <cppunit/extensions/HelperMacros.h>

#include <CmdLineConfig.hh>

#include <TH1I.h>

class BasicCase : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(BasicCase);
  CPPUNIT_TEST(Principles);
  CPPUNIT_TEST(SingleParams);
  CPPUNIT_TEST(ComplexParams);
  CPPUNIT_TEST(Values);
  CPPUNIT_TEST(Defaults);
  CPPUNIT_TEST(Expand);
  CPPUNIT_TEST(Arrays);
  CPPUNIT_TEST(Others);
  CPPUNIT_TEST_SUITE_END();

private:
  CmdLineOption* bool_val;
  CmdLineOption* int_val;
  CmdLineOption* double_val;
  CmdLineOption* string_val;

public:
  virtual void setUp() override {
    bool_val =
        new CmdLineOption("BoolArg", "-bool", "Bool Help message", false);
    int_val = new CmdLineOption("IntegerArg", "-int", "Int Help message", 13);
    double_val = new CmdLineOption("DoubleArg", "-double",
                                   "Double Help message", 3.1415);
    string_val =
        new CmdLineOption("StringArg", "-string", "String Help message", "pi");
  }
  virtual void tearDown() override { CmdLineOption::ClearOptions(); }

protected:
  void Principles() {
    CPPUNIT_ASSERT_EQUAL(std::string("Bool Help message"),
                         std::string(bool_val->GetHelp()));
    CPPUNIT_ASSERT_EQUAL(std::string("Int Help message"),
                         std::string(int_val->GetHelp()));
    CPPUNIT_ASSERT_EQUAL(std::string("Double Help message"),
                         std::string(double_val->GetHelp()));
    CPPUNIT_ASSERT_EQUAL(std::string("String Help message"),
                         std::string(string_val->GetHelp()));

    CmdLineOption::PrintHelp();
  }

  void SingleParams() {
    {
      const char* argv[] = {"./prog", "-bool", "1"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(true, CmdLineOption::GetBoolValue("BoolArg"));
    }

    {
      const char* argv[] = {"./prog", "-bool", "0"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(false, CmdLineOption::GetBoolValue("BoolArg"));
    }

    {
      const char* argv[] = {"./prog"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(13, CmdLineOption::GetIntValue("IntegerArg"));
    }

    {
      const char* argv[] = {"./prog", "-int", "112358"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(112358, CmdLineOption::GetIntValue("IntegerArg"));
    }

    {
      const char* argv[] = {"./prog"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(3.1415, CmdLineOption::GetDoubleValue("DoubleArg"));
    }

    {
      const char* argv[] = {"./prog", "-double", "2.71"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(2.71, CmdLineOption::GetDoubleValue("DoubleArg"));
    }

    {
      const char* argv[] = {"./prog"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(
          std::string("pi"),
          std::string(CmdLineOption::GetStringValue("StringArg")));
    }

    {
      const char* argv[] = {"./prog", "-string", "test string"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(
          std::string("test string"),
          std::string(CmdLineOption::GetStringValue("StringArg")));
    }

    {
      const char* argv[] = {"./prog", "positional1", "positional2",
                            "positional3"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);

      PositionalArgs pargs = CmdLineOption::GetPositionalArguments();
      CPPUNIT_ASSERT_EQUAL(3, (int)pargs.size());
      CPPUNIT_ASSERT_EQUAL(TString("positional1"), pargs[0]);
      CPPUNIT_ASSERT_EQUAL(TString("positional2"), pargs[1]);
      CPPUNIT_ASSERT_EQUAL(TString("positional3"), pargs[2]);
    }
  }

  void ComplexParams() {
    {
      const char* argv[] = {"./prog", "-bool",   "1",           "positional1",
                            "-int",   "112358",  "positional2", "-double",
                            "2.71",   "-string", "test string", "positional3"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(true, CmdLineOption::GetBoolValue("BoolArg"));
      CPPUNIT_ASSERT_EQUAL(112358, CmdLineOption::GetIntValue("IntegerArg"));
      CPPUNIT_ASSERT_EQUAL(2.71, CmdLineOption::GetDoubleValue("DoubleArg"));
      CPPUNIT_ASSERT_EQUAL(
          std::string("test string"),
          std::string(CmdLineOption::GetStringValue("StringArg")));

      CPPUNIT_ASSERT_EQUAL(true, bool_val->GetBoolValue());
      CPPUNIT_ASSERT_EQUAL(112358, int_val->GetIntValue());
      CPPUNIT_ASSERT_EQUAL(2.71, double_val->GetDoubleValue());
      CPPUNIT_ASSERT_EQUAL(std::string("test string"),
                           std::string(string_val->GetStringValue()));

      PositionalArgs pargs = CmdLineOption::GetPositionalArguments();
      CPPUNIT_ASSERT_EQUAL(3, (int)pargs.size());
      CPPUNIT_ASSERT_EQUAL(TString("positional1"), pargs[0]);
      CPPUNIT_ASSERT_EQUAL(TString("positional2"), pargs[1]);
      CPPUNIT_ASSERT_EQUAL(TString("positional3"), pargs[2]);
    }

    {
      const char* argv[] = {"./prog",      "-bool",      "0",
                            "positional1", "-int",       "positional2",
                            "-double",     "2.71",       "-string",
                            "test string", "positional3"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(false, CmdLineOption::GetBoolValue("BoolArg"));
      CPPUNIT_ASSERT_EQUAL(13, CmdLineOption::GetIntValue("IntegerArg"));
      CPPUNIT_ASSERT_EQUAL(2.71, CmdLineOption::GetDoubleValue("DoubleArg"));
      CPPUNIT_ASSERT_EQUAL(
          std::string("test string"),
          std::string(CmdLineOption::GetStringValue("StringArg")));

      PositionalArgs pargs = CmdLineOption::GetPositionalArguments();
      CPPUNIT_ASSERT_EQUAL(2, (int)pargs.size());
      CPPUNIT_ASSERT_EQUAL(TString("positional1"), pargs[0]);
      CPPUNIT_ASSERT_EQUAL(TString("positional3"), pargs[1]);
    }
  }

  void Values() {
    {
      const char* argv[] = {"./prog", "-p"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
    }

    CPPUNIT_ASSERT_EQUAL(false, CmdLineOption::GetBoolValue("BoolArg"));
    CPPUNIT_ASSERT_EQUAL(13, CmdLineOption::GetIntValue("IntegerArg"));
    CPPUNIT_ASSERT_EQUAL(3.1415, CmdLineOption::GetDoubleValue("DoubleArg"));
    CPPUNIT_ASSERT_EQUAL(
        std::string("pi"),
        std::string(CmdLineOption::GetStringValue("StringArg")));

    CPPUNIT_ASSERT_EQUAL(false, bool_val->GetBoolValue());
    CPPUNIT_ASSERT_EQUAL(0, bool_val->GetIntValue());
    CPPUNIT_ASSERT_EQUAL(0.0, bool_val->GetDoubleValue());
    CPPUNIT_ASSERT_EQUAL((char*)nullptr, (char*)bool_val->GetStringValue());

    CPPUNIT_ASSERT_EQUAL(false, int_val->GetBoolValue());
    CPPUNIT_ASSERT_EQUAL(13, int_val->GetIntValue());
    CPPUNIT_ASSERT_EQUAL(0.0, int_val->GetDoubleValue());
    CPPUNIT_ASSERT_EQUAL((char*)nullptr, (char*)int_val->GetStringValue());

    CPPUNIT_ASSERT_EQUAL(false, double_val->GetBoolValue());
    CPPUNIT_ASSERT_EQUAL(0, double_val->GetIntValue());
    CPPUNIT_ASSERT_EQUAL(3.1415, double_val->GetDoubleValue());
    CPPUNIT_ASSERT_EQUAL((char*)nullptr, (char*)double_val->GetStringValue());

    CPPUNIT_ASSERT_EQUAL(false, string_val->GetBoolValue());
    CPPUNIT_ASSERT_EQUAL(0, string_val->GetIntValue());
    CPPUNIT_ASSERT_EQUAL(0.0, string_val->GetDoubleValue());
    CPPUNIT_ASSERT_EQUAL(std::string("pi"),
                         std::string(string_val->GetStringValue()));

    PositionalArgs pargs = CmdLineOption::GetPositionalArguments();
    CPPUNIT_ASSERT_EQUAL(0, (int)pargs.size());
  }

  void Defaults() {
    CPPUNIT_ASSERT_EQUAL(false, CmdLineOption::GetDefaultBoolValue("BoolArg"));
    CPPUNIT_ASSERT_EQUAL(13, CmdLineOption::GetDefaultIntValue("IntegerArg"));
    CPPUNIT_ASSERT_EQUAL(3.1415,
                         CmdLineOption::GetDefaultDoubleValue("DoubleArg"));
    CPPUNIT_ASSERT_EQUAL(
        std::string("pi"),
        std::string(CmdLineOption::GetDefaultStringValue("StringArg")));

    CPPUNIT_ASSERT_EQUAL(false, bool_val->GetDefaultBoolValue());
    CPPUNIT_ASSERT_EQUAL(0, bool_val->GetDefaultIntValue());
    CPPUNIT_ASSERT_EQUAL(0.0, bool_val->GetDefaultDoubleValue());
    CPPUNIT_ASSERT_EQUAL((char*)nullptr,
                         (char*)bool_val->GetDefaultStringValue());

    CPPUNIT_ASSERT_EQUAL(true, int_val->GetDefaultBoolValue());
    CPPUNIT_ASSERT_EQUAL(13, int_val->GetDefaultIntValue());
    CPPUNIT_ASSERT_EQUAL(0.0, int_val->GetDefaultDoubleValue());
    CPPUNIT_ASSERT_EQUAL((char*)nullptr,
                         (char*)int_val->GetDefaultStringValue());

    CPPUNIT_ASSERT_EQUAL(false, double_val->GetDefaultBoolValue());
    CPPUNIT_ASSERT_EQUAL(0, double_val->GetDefaultIntValue());
    CPPUNIT_ASSERT_EQUAL(3.1415, double_val->GetDefaultDoubleValue());
    CPPUNIT_ASSERT_EQUAL((char*)nullptr,
                         (char*)double_val->GetDefaultStringValue());

    CPPUNIT_ASSERT_EQUAL(false, string_val->GetDefaultBoolValue());
    CPPUNIT_ASSERT_EQUAL(0, string_val->GetDefaultIntValue());
    CPPUNIT_ASSERT_EQUAL(0.0, string_val->GetDefaultDoubleValue());
    CPPUNIT_ASSERT_EQUAL(std::string("pi"),
                         std::string(string_val->GetDefaultStringValue()));

    PositionalArgs pargs = CmdLineOption::GetPositionalArguments();
    CPPUNIT_ASSERT_EQUAL(0, (int)pargs.size());
  }

  void Expand() {
    TH1I* h1 = new TH1I("hist1", "hist1", 10, 0, 10);
    CmdLineOption* hist1_bool_val = bool_val->Expand(h1);
    CmdLineOption* hist1_int_val = int_val->Expand(h1);
    CmdLineOption* hist1_double_val = double_val->Expand(h1);
    CmdLineOption* hist1_string_val = string_val->Expand(h1);

    CPPUNIT_ASSERT_EQUAL(false, hist1_bool_val->GetDefaultBoolValue());
    CPPUNIT_ASSERT_EQUAL(13, hist1_int_val->GetDefaultIntValue());
    CPPUNIT_ASSERT_EQUAL(3.1415, hist1_double_val->GetDefaultDoubleValue());
    CPPUNIT_ASSERT_EQUAL(
        std::string("pi"),
        std::string(hist1_string_val->GetDefaultStringValue()));
  }

  void Arrays() {
    {
      const char* argv[] = {"./prog", "-int", "1112358,1234,1248"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(1112358, CmdLineOption::GetIntValue("IntegerArg"));

      CPPUNIT_ASSERT_EQUAL(3, CmdLineOption::GetArraySize("IntegerArg"));

      CPPUNIT_ASSERT_EQUAL(1112358,
                           CmdLineOption::GetIntArrayValue("IntegerArg", 1));
      CPPUNIT_ASSERT_EQUAL(1234,
                           CmdLineOption::GetIntArrayValue("IntegerArg", 2));
      CPPUNIT_ASSERT_EQUAL(1248,
                           CmdLineOption::GetIntArrayValue("IntegerArg", 3));

      CPPUNIT_ASSERT_EQUAL(
          0, CmdLineOption::GetDefaultIntArrayValue("IntegerArg", 1));
      CPPUNIT_ASSERT_EQUAL(
          0, CmdLineOption::GetDefaultIntArrayValue("IntegerArg", 2));
      CPPUNIT_ASSERT_EQUAL(
          0, CmdLineOption::GetDefaultIntArrayValue("IntegerArg", 3));
    }

    {
      const char* argv[] = {"./prog", "-double", "12.71,3.1415 1.137"};
      CmdLineConfig::instance()->ReadCmdLine(sizeof(argv) / sizeof(char*),
                                             (char**)argv);
      CPPUNIT_ASSERT_EQUAL(12.71, CmdLineOption::GetDoubleValue("DoubleArg"));

      CPPUNIT_ASSERT_EQUAL(3, CmdLineOption::GetArraySize("DoubleArg"));
      CPPUNIT_ASSERT_EQUAL(12.71,
                           CmdLineOption::GetDoubleArrayValue("DoubleArg", 1));
      CPPUNIT_ASSERT_EQUAL(3.1415,
                           CmdLineOption::GetDoubleArrayValue("DoubleArg", 2));
      CPPUNIT_ASSERT_EQUAL(1.137,
                           CmdLineOption::GetDoubleArrayValue("DoubleArg", 3));

      CPPUNIT_ASSERT_EQUAL(
          0.0, CmdLineOption::GetDefaultDoubleArrayValue("DoubleArg", 1));
      CPPUNIT_ASSERT_EQUAL(
          0.0, CmdLineOption::GetDefaultDoubleArrayValue("DoubleArg", 2));
      CPPUNIT_ASSERT_EQUAL(
          0.0, CmdLineOption::GetDefaultDoubleArrayValue("DoubleArg", 3));
    }
  }

  void Others() {}
};

CPPUNIT_TEST_SUITE_REGISTRATION(BasicCase);
