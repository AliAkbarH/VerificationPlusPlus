#include "TypeTheoryGenerator.h"
#include <string>
#include <typeinfo>
using namespace std;

void TypeTheoryGeneratorConsumer::HandleTranslationUnit(clang::ASTContext &Context)
{
    outs() << "Looking for function " << Visitor.FunctionUT << "\n";
    Visitor.FirstVisit = true;
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    if (Visitor.functionDecl != NULL)
        outs() << "Found!\n";
    else
    {
        outs() << "No such function\n";
        return;
    }
    Visitor.FirstVisit = false;
    Visitor.TraverseFunctionDecl(Visitor.functionDecl);

    auto tt = Visitor.getOutput();
    if (!tt.Variables.empty())
    {
        outs() << "Variables:\n\t";
        for (int i = 0; i < tt.Variables.size(); i++)
        {
            if (!tt.Variables[i].isBOp)
            {
                outs() << tt.Variables[i].Name << ": " << tt.Variables[i].Type
                       << " " << ((tt.Variables[i].isInput) ? "Input " : "") << ((tt.Variables[i].isOutput) ? "Output " : "") << "\n\t";
            }
        }
    }
    outs() << "\n";
    if (!tt.BOperations.empty())
    {
        outs() << "Binary Operations:\n\t";
        for (int i = 0; i < tt.BOperations.size(); i++)
        {
            outs() << tt.BOperations[i].Name << "\n\t";
        }
    }
}

std::unique_ptr<clang::ASTConsumer> TypeTheoryGeneratorAction::CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile)
{
    return std::unique_ptr<clang::ASTConsumer>(
        new TypeTheoryGeneratorConsumer(&Compiler.getASTContext()));
}

string TypeTheoryGeneratorVisitor::VarNameGenerator(VarDecl *Decl)
{
    auto loc = Decl->getBeginLoc();
    auto fileAndLine = Context->getSourceManager().getDecomposedSpellingLoc(loc);
    auto name = Decl->getNameAsString();
    auto fileAndLineStr = to_string(fileAndLine.first.getHashValue()) + to_string(fileAndLine.second);
    auto nameWithLoc = name + "_" + fileAndLineStr;
    return nameWithLoc;
}

bool TypeTheoryGeneratorVisitor::VisitVarDecl(VarDecl *Decl)
{

    if (Context->getSourceManager().isInSystemHeader(Decl->getBeginLoc()))
        return true;

    if (FirstVisit)
        return true;

    string type = Decl->getType().getAsString();

    Variable *var = new Variable(VarNameGenerator(Decl), type);
    TTOutput.AddVariable(var);

    return true;
}

bool TypeTheoryGeneratorVisitor::VisitBinaryOperator(BinaryOperator *Expr)
{

    if (Context->getSourceManager().isInSystemHeader(Expr->getBeginLoc()))
        return true;

    if (FirstVisit)
        return true;

    auto lhs = Expr->getLHS();
    auto rhs = Expr->getRHS();
    BinaryOperation *bOp = new BinaryOperation(HandleOperand(Expr->getLHS()), HandleOperand(Expr->getRHS()), Expr->getOpcodeStr());
    TTOutput.AddBOp(bOp);
    return true;
}

bool TypeTheoryGeneratorVisitor::VisitFunctionDecl(FunctionDecl *Decl)
{

    if (Context->getSourceManager().isInSystemHeader(Decl->getBeginLoc()))
        return true;

    string name = Decl->getName();
    if (FirstVisit)
    {
        outs() << "Found Function: " << name << "\n";
        if (name == FunctionUT)
        {
            functionDecl = Decl;
        }
    }
    else
    {
        auto begin = Decl->param_begin();
        auto end = Decl->param_end();
        for (auto i = begin; i != end; i++)
        {
            string type = (*i)->getType().getAsString();

            Variable *var = new Variable(VarNameGenerator(*i), type);
            var->isInput = true;
            TTOutput.AddVariable(var);
        }
    }

    return true;
}

bool TypeTheoryGeneratorVisitor::VisitReturnStmt(ReturnStmt *Stmt)
{

    if (Context->getSourceManager().isInSystemHeader(Stmt->getBeginLoc()))
        return true;

    if (FirstVisit)
        return true;

    Variable *ret = HandleOperand(Stmt->getRetValue());
    TTOutput.UpdateToOutput(ret);
    return true;
}

Variable *TypeTheoryGeneratorVisitor::HandleOperand(clang::Expr *operand)
{
    auto stmtClass = operand->getStmtClassName();
    if (stmtClass == "ImplicitCastExpr")
    {
        Expr *child = (Expr *)(*(operand->child_begin()));
        return HandleOperand(child);
    }
    if (stmtClass == "IntegerLiteral")
    {
        IntegerLiteral *literal = (IntegerLiteral *)(operand);
        int value = literal->getValue().getSExtValue();
        string name = "IntegerLiteral" + to_string(value);
        string type = "int";
        string valueStr = to_string(value);
        Variable *var = new Variable(name, type, valueStr);
        TTOutput.AddVariable(var);
        return var;
    }
    if (stmtClass == "ArraySubscriptExpr")
    {
        ArraySubscriptExpr *ind = (ArraySubscriptExpr *)operand;
        Expr *index = ind->getIdx();
        Expr *array = ind->getBase();
        Variable *indx = HandleOperand(index);
        TTOutput.UpdateToIndex(indx);
        BinaryOperation *op = new BinaryOperation(indx, HandleOperand(array), "[]");
        TTOutput.AddBOp(op);
        return op;
    }
    if (stmtClass == "CXXBoolLiteralExpr")
    {
        CXXBoolLiteralExpr *literal = (CXXBoolLiteralExpr *)operand;
        bool value = literal->getValue();
        string name = "BoolLiteral" + to_string(value);
        string type = "bool";
        string valueStr = to_string(value);
        Variable var(name, type, valueStr);
        TTOutput.AddVariable(&var);
        return &var;
    }
    if (stmtClass == "BinaryOperator")
    {
        BinaryOperator *op = (BinaryOperator *)operand;
        Variable *rhs = HandleOperand(op->getRHS());
        Variable *lhs = HandleOperand(op->getLHS());
        if (op->isComparisonOp() && rhs->Type == "ArrayIndex")
        {
            TTOutput.UpdateToBound(lhs);
        }
        if (op->isComparisonOp() && lhs->Type == "ArrayIndex")
        {
            TTOutput.UpdateToBound(rhs);
        }
        BinaryOperation *bOp = new BinaryOperation(lhs, rhs, op->getOpcodeStr());

        TTOutput.AddBOp(bOp);
        return bOp;
    }
    if (stmtClass == "DeclRefExpr")
    {
        DeclRefExpr *ref = (DeclRefExpr *)operand;
        VarDecl *decl = (VarDecl *)ref->getDecl();
        string type = decl->getType().getAsString();
        Variable *var = new Variable(VarNameGenerator(decl), type);
        TTOutput.AddVariable(var);
        return var;
    }

    return nullptr;
}

TypeTheoryOutput TypeTheoryGeneratorVisitor::getOutput()
{
    return TTOutput.DumpToOutput();
}