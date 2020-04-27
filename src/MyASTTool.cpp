#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include "TypeTheoryGenerator.h"

using namespace clang::tooling;
using namespace std;

static cl::OptionCategory MyToolCategory("my-ast-tool options");

int main(int argc, const char **argv)
{
  if (argc<3){
    outs()<<"Usage MyASTTool source.cpp -- funcName";
    return -1;
  }
  
  CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  //TypeTheoryGeneratorAction *action= new TypeTheoryGeneratorAction("max");
  return Tool.run(newTTFrontendActionFactory(argv[2]).get());
}