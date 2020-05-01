#include <string>
#include <iostream>

//mohammad: including code of haidoura
//#include "MyASTTool.h"
using namespace std;

extern int spchk_main(int argc, char ** argv);// defined in sc.cpp

extern int ma_main(int argc, char ** argv);// defined in ma.cpp

int main(int argc, char ** argv) {
  //mohammad: calling code of haidoura to call the parser
  // TypeTheoryOutput out=ParseFunction(argc, argv);
  // PrintTypeTheoryOutput(out);
  
  string program=argv[0];
  program+="#";

// checks if the program name ends with SC or MA and runs the program accordingly
  size_t found = program.rfind("sc#");
  if (found != string::npos) {
	  return spchk_main(argc, argv) ;
  }
  found = program.rfind("ma#");
  if (found != string::npos) {
    return ma_main(argc, argv);
  }
  cerr << "program name should be either `sc' for specification construction, or `ma' for make adequate." << endl;
  return 0;
}
