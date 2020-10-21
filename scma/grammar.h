#ifndef GRAMMAR_H
#define GRAMMAR_H
//#include<QStringList>
#include<vector>
#include<string>
#include "variable.h"

class Grammar
{
public:
    Grammar();
    Grammar(Variable left, Variable right, std::vector<std::string> op);

    Variable leftVar;
    Variable rightVar;
    std::vector<std::string> Op;

    std::string print();
    std::vector<std::string> printAll();

};

#endif // GRAMMAR_H
