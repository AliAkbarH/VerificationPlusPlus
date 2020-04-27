#include "TypeTheoryGenerator.h"
#include <string>
#include <typeinfo>
using namespace std;
void TypeTheoryGeneratorConsumer::HandleTranslationUnit(clang::ASTContext &Context)
{
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    auto tt=Visitor.getOutput();
    if(!tt.Variables.empty()){
        outs()<<"Variables:\n\t";
        for(int i=0;i<tt.Variables.size();i++){
            if(!tt.Variables[i].isBOp){
                outs()<<tt.Variables[i].Name<<": "<<tt.Variables[i].Type<<"\n\t";
            }
        }
    }
    outs()<<"\n";
    if(!tt.BOperations.empty()){
        outs()<<"Binary Operations:\n\t";
        for(int i=0;i<tt.BOperations.size();i++){
            outs()<<tt.BOperations[i].Name<<"\n\t";
        }
    }
    //printTT(outs(), Visitor.getOutput());
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

    // auto type = Decl->getType().getAsString();
    // llvm::outs() << "VarDecl: " << VarNameGenerator(Decl) << " : " << type << "\n";

    string type = Decl->getType().getAsString();

        Variable *var = new Variable(VarNameGenerator(Decl), VarTypeConverter(type));
        TTOutput.AddVariable(var);

    return true;
}

bool TypeTheoryGeneratorVisitor::VisitBinaryOperator(BinaryOperator *Expr)
{

    if (Context->getSourceManager().isInSystemHeader(Expr->getBeginLoc()))
        return true;

    auto lhs = Expr->getLHS();
    auto rhs = Expr->getRHS();

    // outs() << "Binary Operation: ";
    // outs() << Expr->getOpcodeStr().str() << "\n\t";
    // outs() << "RHS: " << lhs->getType().getAsString() << ' ' << "\n\t";
    // HandleOperand(rhs);
    // outs() << "\n\t";
    // outs() << "LHS: " << rhs->getType().getAsString() << "\n\t";
    // HandleOperand(lhs);
    // outs() << "\n";
    BinaryOperation *bOp = new BinaryOperation(HandleOperand(Expr->getLHS()), HandleOperand(Expr->getRHS()), Expr ->getOpcodeStr());
    TTOutput.AddBOp(bOp);
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
        Variable *var=new Variable(name, type, valueStr);
        TTOutput.AddVariable(var);
        return var;
    }
    if (stmtClass == "ArraySubscriptExpr")
    {
        ArraySubscriptExpr *ind = (ArraySubscriptExpr *)operand;
        Expr *index = ind->getIdx();
        Expr *array = ind->getBase();
        Variable *indx=HandleOperand(index);
        TTOutput.UpdateToIndex(indx);
        BinaryOperation op(indx, HandleOperand(array), "[]");
        TTOutput.AddBOp(&op);
        return &op;
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
        Variable *rhs=HandleOperand(op->getRHS());
        Variable *lhs=HandleOperand(op->getLHS());
        if(op->isComparisonOp && rhs->Type=="ArrayIndex"){
            TTOutput.UpdateToBound(lhs);
        }
        if(op->isComparisonOp && lhs->Type=="ArrayIndex"){
            TTOutput.UpdateToBound(rhs);
        }
        BinaryOperation *bOp = new BinaryOperation(lhs, rhs, op->getOpcodeStr());
        
        TTOutput.AddBOp(bOp);
        return bOp;
    }
    if (stmtClass == "DeclRefExpr")
    {
        DeclRefExpr *ref = (DeclRefExpr *)operand;
        VarDecl *decl = (VarDecl*)ref->getDecl();
        string type = decl->getType().getAsString();
        Variable *var = new Variable(VarNameGenerator(decl), type);
        TTOutput.AddVariable(var);
        return var;
    }

    return nullptr;
}

// VariableType TypeTheoryGeneratorVisitor::VarTypeConverter(string type)
// {
//     if (type == "int")
//     {
//         return VariableType::Int;
//     }
//     if (type == "int *")
//     {
//         return VariableType::Array;
//     }
//     if (type == "char")
//     {
//         return VariableType::Char;
//     }
//     if (type == "_Bool")
//     {
//         return VariableType::Bool;
//     }
//     return VariableType::Unknown;
// }

TypeTheoryOutput TypeTheoryGeneratorVisitor::getOutput(){
    return TTOutput.DumpToOutput();
}