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

TypeTheoryOutput ParseFunction(int argc, char **argv);

void PrintTypeTheoryOutput(TypeTheoryOutput tt);

