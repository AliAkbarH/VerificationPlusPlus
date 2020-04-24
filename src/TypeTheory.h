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
    bool isBOp;
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


class TypeTheoryOutput{
public:
    vector<Variable> Variables;
    vector<BinaryOperation> BOperations;
};


class TypeTheoryOutputInternal
{
public:
    vector<Variable> Variables;
    vector<BinaryOperation> BOperations;
    void AddVariable(Variable *);
    void AddBOp(BinaryOperation *);
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

