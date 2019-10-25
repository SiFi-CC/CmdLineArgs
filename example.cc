#include <CmdLineConfig.hh>

#include <iostream>

int main(int argc, char** argv) {
  CmdLineOption bool_val("CustomBoolArgName", "-bool", "Help message", true);
  CmdLineOption int_val("CustomIntegerArgName", "-int", "Help message", 13);
  CmdLineOption double_val("CustomDoubleArgName", "-double", "Help message",
                           3.1415);
  CmdLineOption string_val("CustomStringArgName", "-string", "Help message",
                           "pi");

  CmdLineArg pos1("name", "name of file", CmdLineArg::kString);
  CmdLineArg pos_g("", "more files", CmdLineArg::kString);
  CmdLineArg pos2("trick", "name of trick", CmdLineArg::kString);

  CmdLineConfig::SetPositionalText("[...]");

  CmdLineConfig* cmdcfg = CmdLineConfig::instance();
  cmdcfg->ReadCmdLine(argc, argv);

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

  const Positional& pargs = cmdcfg->GetPositionalArguments();
  std::cout << std::endl << "Positional arguments: " << std::endl;
  for (Positional::const_iterator it = pargs.begin(); it != pargs.end(); ++it)
    std::cout << " " << it->second->GetName() << std::endl;

  return 0;
}
