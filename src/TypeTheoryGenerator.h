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

class TypeTheoryGeneratorVisitor
    : public RecursiveASTVisitor<TypeTheoryGeneratorVisitor>
{
public:
    explicit TypeTheoryGeneratorVisitor(ASTContext *Context)
        : Context(Context){
            FunctionUT="max";
            functionDecl=NULL;
        };

    bool VisitVarDecl(VarDecl *Decl);
    bool VisitBinaryOperator(BinaryOperator *Expr);
    bool VisitFunctionDecl(FunctionDecl *Decl);
    bool VisitReturnStmt(ReturnStmt* Stmt);
    TypeTheoryOutput getOutput();
    bool FirstVisit;
    clang::FunctionDecl *functionDecl;
    string FunctionUT;

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
    explicit TypeTheoryGeneratorConsumer(ASTContext *Context)
        : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context);

private:
    TypeTheoryGeneratorVisitor Visitor;
};

class TypeTheoryGeneratorAction : public clang::ASTFrontendAction
{
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance &Compiler, llvm::StringRef InFile);
};
