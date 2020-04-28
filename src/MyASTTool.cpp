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

  const char **a=new const char*[2];
  a[0]=argv[0];
  a[1]=argv[1];
  int ac=2;
  CommonOptionsParser OptionsParser(ac, a, MyToolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  //TypeTheoryGeneratorAction *action= new TypeTheoryGeneratorAction("max");
  Tool.run(newTTFrontendActionFactory(argv[2]).get());
  return 0;
}