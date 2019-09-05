#include <CmdLineConfig.hh>

#include <iostream>

int main(int argc, char** argv) {
  CmdLineOption bool_val("CustomBoolArgName", "-bool", "Help message", true);
  CmdLineOption int_val("CustomIntegerArgName", "-int", "Help message", 13);
  CmdLineOption double_val("CustomDoubleArgName", "-double", "Help message",
                           3.1415);
  CmdLineOption string_val("CustomStringArgName", "-string", "Help message",
                           "pi");

  CmdLineConfig::instance()->ReadCmdLine(argc, argv);

  std::cout << "Bool value   = "
            << CmdLineOption::GetBoolValue("CustomBoolArgName") << std::endl;
  std::cout << "Int value    = "
            << CmdLineOption::GetIntValue("CustomIntegerArgName") << std::endl;
  std::cout << "Double value = "
            << CmdLineOption::GetDoubleValue("CustomDoubleArgName")
            << std::endl;
  std::cout << "String value = "
            << CmdLineOption::GetStringValue("CustomStringArgName")
            << std::endl;

  PositionalArgs pargs = CmdLineOption::GetPositionalArguments();
  std::cout << std::endl << "Positional arguments: " << std::endl;
  for (int i = 0; i < pargs.size(); ++i)
    std::cout << " " << pargs[i] << std::endl;

  return 0;
}
