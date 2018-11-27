#include <CmdLineConfig.hh>

#include <iostream>


int main(int argc, char ** argv) {

  CmdLineOption mdcspatref("CustomArgument", "-custom", "Custom value (float)" , 0.100);

  CmdLineConfig::instance()->ReadCmdLine(argc, argv);

  std::cout << CmdLineOption::GetDoubleValue("CustomArgument") << std::endl;

  return 0;
}
