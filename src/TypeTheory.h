#include <string>
#include <vector>
#include <map>
#include "llvm/Support/raw_ostream.h"
using namespace std;
using namespace llvm;
enum VariableType : int
{
    Int,
    Char,
    Array,
    ArrayIndex,
    ArrayBound,
    Bool,
    BinaryOp,
    Unknown
};


class Variable
{
public:
    Variable();

    Variable(string name, string type, string value="");

    string Name;
    string Type;
    string Value;
    int declLine;
    int declCol;
    bool isBOp;
    bool isUOp;
    bool isInput;
    bool isOutput;
    
};

class BinaryOperation : public Variable
{

public:
    BinaryOperation(Variable *left, Variable *right, string operation);

    Variable *Left, *Right;
    string Operation;
};

class UnaryOperation : public Variable
{
public:
    UnaryOperation(Variable*, string, bool );

    string Operation;
    Variable *Operand;
    bool isPrefix;

};

class TypeTheoryOutput
{
public:
    vector<Variable> Variables;
    vector<BinaryOperation> BOperations;
    vector<UnaryOperation> UOperations;
    vector<Variable> RetrurnedVariables;
    Variable FunctionUT;
};

class TypeTheoryOutputInternal
{
public:
    vector<Variable> Variables;
    vector<BinaryOperation> BOperations;
    vector<UnaryOperation> UOperations;
    vector<Variable> RetrurnedVariables;
    Variable FunctionUT;
    void AddVariable(Variable *);
    void AddBOp(BinaryOperation *);
    void AddUOp(UnaryOperation *);
    void UpdateToIndex(Variable*);
    void UpdateToBound(Variable*);
    void UpdateToOutput(Variable*);
    TypeTheoryOutput DumpToOutput();
private:
    map<string, Variable> VariablesByName;
};

/*
void printTT(raw_ostream &out, TypeTheoryOutput tt){
    if(!tt.Variables.empty()){
        out<<"Variables:\n\t";
        for(int i=0;i<tt.Variables.size();i++){
            if(!tt.Variables[i].isBOp){
                out<<tt.Variables[i].Name<<": "<<tt.Variables[i].Type<<"\n\t";
            }
        }
    }
    out<<"\n";
    if(!tt.BOperations.empty()){
        out<<"Binary Operations:\n\t";
        for(int i=0;i<tt.BOperations.size();i++){
            out<<tt.BOperations[i].Name<<"\n\t";
        }
    }
}*/

