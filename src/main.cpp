#include "MyASTTool.h"

int main(int argc, char** argv){
    TypeTheoryOutput out=ParseFunction(argc, argv);
    PrintTypeTheoryOutput(out);
    return 0;
}