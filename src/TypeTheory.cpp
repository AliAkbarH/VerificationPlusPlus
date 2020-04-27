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
}
BinaryOperation::BinaryOperation(Variable *left, Variable *right, string operation)
{
    Name = left->Name + right->Name + operation;
    Left = left;
    Right = right;
    Operation = operation;
    isBOp = true;
}

void TypeTheoryOutputInternal::AddVariable(Variable *var)
{

    if (VariablesByName.find(var->Name) == VariablesByName.end())
    {
        pair<string, Variable> pair(var->Name, *var);
        VariablesByName.insert(pair);
    }
}
void TypeTheoryOutputInternal::UpdateToIndex(Variable *var){
    std::map<string, Variable>::iterator found = VariablesByName.find(var->Name);
    if (found == VariablesByName.end())
    {
        var->Type="ArrayIndex";
        pair<string, Variable> pair(var->Name, *var);
        VariablesByName.insert(pair);
    }
    else{
        found->second.Type="ArrayIndex";
    }
}
void TypeTheoryOutputInternal::UpdateToBound(Variable *var){
    std::map<string, Variable>::iterator found = VariablesByName.find(var->Name);
    if (found == VariablesByName.end())
    {
        var->Type="ArrayBound";
        pair<string, Variable> pair(var->Name, *var);
        VariablesByName.insert(pair);
    }
    else if(found->second.Type!="ArrayIndex"){
        found->second.Type="ArrayIndex";
    }
}

void TypeTheoryOutputInternal::AddBOp(BinaryOperation *bOp)
{
    AddVariable(bOp->Right);
    AddVariable(bOp->Left);
    BOperations.push_back(*bOp);
    AddVariable(bOp);
}

TypeTheoryOutput TypeTheoryOutputInternal::DumpToOutput(){
    TypeTheoryOutput out;
    out.BOperations=BOperations;
    for( auto it = VariablesByName.begin(); it != VariablesByName.end(); ++it ) {
        out.Variables.push_back( it->second );
    }
    return out;
}