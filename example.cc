#include <CmdLineConfig.hh>

#include <iostream>


int main(int argc, char ** argv) {

  CmdLineOption mdcspatref("xSpatialResolution","-rresol","MDC spatial resolution in mm" , 0.100);

//   CmdLineOption so;
  gCmdLineConfig->ReadCmdLine(argc, argv);
  
//   std::cout << gCmdLineConfig->GetHelp() << std::endl;

//   gCmdLineConfig->PrintHelp();

  return 0;
}
