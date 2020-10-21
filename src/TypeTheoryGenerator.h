#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/Expr.h"
#include <vector>
#include <string>
#include "TypeTheory.h"

using namespace clang;
using namespace llvm;
using namespace std;
using namespace clang::tooling;

class TypeTheoryGeneratorVisitor
    : public RecursiveASTVisitor<TypeTheoryGeneratorVisitor>
{
public:
    explicit TypeTheoryGeneratorVisitor(ASTContext *Context, string funcName, TypeTheoryOutput &output)
        : Context(Context){
            FunctionUT=funcName;
            functionDecl=NULL;
            this->output=&output;
        };

    bool VisitVarDecl(VarDecl *Decl);
    bool VisitBinaryOperator(BinaryOperator *Expr);
    bool VisitFunctionDecl(FunctionDecl *Decl);
    bool VisitReturnStmt(ReturnStmt* Stmt);
    TypeTheoryOutput getOutput();
    bool FirstVisit;
    clang::FunctionDecl *functionDecl;
    string FunctionUT;
    TypeTheoryOutput *output;

private:
    ASTContext *Context;
    TypeTheoryOutputInternal TTOutput;
    Variable* HandleOperand(clang::Expr *operand);
    string VarNameGenerator(VarDecl *Decl);
    VariableType VarTypeConverter(string type);

};

class TypeTheoryGeneratorConsumer : public clang::ASTConsumer
{
public:
    explicit TypeTheoryGeneratorConsumer(ASTContext *Context, string funcName, TypeTheoryOutput &output)
        : Visitor(Context, funcName, output) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context);

private:
    TypeTheoryGeneratorVisitor Visitor;
};

class TypeTheoryGeneratorAction : public clang::ASTFrontendAction
{
public:
    string funcName;
    TypeTheoryOutput *output;
    TypeTheoryGeneratorAction(string funcName, TypeTheoryOutput &output);
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance &Compiler, llvm::StringRef InFile);
};


std::unique_ptr<FrontendActionFactory> newTTFrontendActionFactory(string funcName, TypeTheoryOutput &output);

