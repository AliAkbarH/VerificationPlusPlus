#include "TypeTheory.h"

Variable::Variable()
{
    Name = "";
    Type = "unknown";
    Value = "";
}

Variable::Variable(string name, string type, string value)
{
    Name = name;
    Type = type;
    Value = value;
    isBOp = false;
    isUOp=false;
}
BinaryOperation::BinaryOperation(Variable *left, Variable *right, string operation)
{
    Name = left->Name + right->Name + operation;
    Left = left;
    Right = right;
    Operation = operation;
    isBOp = true;
    isUOp=false;
}

UnaryOperation::UnaryOperation(Variable *var, string op, bool pre){
    
    Operand=var;
    Operation=op;
    isPrefix=pre;
    Name=isPrefix? op + var->Name  : var->Name +op; 
    isUOp=true;
    isBOp=false;
}

void TypeTheoryOutputInternal::AddVariable(Variable *var)
{

    if (VariablesByName.find(var->Name) == VariablesByName.end())
    {
        pair<string, Variable> pair(var->Name, *var);
        VariablesByName.insert(pair);
        Variables.push_back(*var);
    }
}
void TypeTheoryOutputInternal::UpdateToIndex(Variable *var)
{
    std::map<string, Variable>::iterator found = VariablesByName.find(var->Name);
    if (found == VariablesByName.end())
    {
        var->Type = "ArrayIndex";
        pair<string, Variable> pair(var->Name, *var);
        VariablesByName.insert(pair);
    }
    else
    {
        found->second.Type = "ArrayIndex";
    }
}
void TypeTheoryOutputInternal::UpdateToBound(Variable *var)
{
    std::map<string, Variable>::iterator found = VariablesByName.find(var->Name);
    if (found == VariablesByName.end())
    {
        var->Type = "ArrayBound";
        pair<string, Variable> pair(var->Name, *var);
        VariablesByName.insert(pair);
    }
    else if (found->second.Type != "ArrayIndex")
    {
        found->second.Type = "ArrayIndex";
    }
}

void TypeTheoryOutputInternal::AddBOp(BinaryOperation *bOp)
{
    AddVariable(bOp->Right);
    AddVariable(bOp->Left);
    BOperations.push_back(*bOp);
    AddVariable(bOp);
}

void TypeTheoryOutputInternal::AddUOp(UnaryOperation *uOp)
{
    AddVariable(uOp->Operand);
    UOperations.push_back(*uOp);
    AddVariable(uOp);
}


TypeTheoryOutput TypeTheoryOutputInternal::DumpToOutput()
{
    TypeTheoryOutput out;
    out.BOperations = BOperations;
    out.UOperations=UOperations;
    out.FunctionUT=FunctionUT;
    out.RetrurnedVariables=RetrurnedVariables;
    for (auto it = VariablesByName.begin(); it != VariablesByName.end(); ++it)
    {
        out.Variables.push_back(it->second);
    }
    return out;
}

void TypeTheoryOutputInternal::UpdateToOutput(Variable *var)
{
    std::map<string, Variable>::iterator found = VariablesByName.find(var->Name);
    if (found == VariablesByName.end())
    {
        var->isOutput = true;
        pair<string, Variable> pair(var->Name, *var);
        VariablesByName.insert(pair);
    }
    else
    {
        found->second.isOutput = true;
    }
}