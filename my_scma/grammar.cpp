#include "grammar.h"

Grammar::Grammar()
{
}

Grammar::Grammar(Variable left, Variable right, std::vector<std::string> op)
{
    leftVar=left;
    rightVar=right;
    Op=op;
}

std::string Grammar::print()
{
    std::string r;
    r=leftVar.name+", "+rightVar.name+", "+Op[0]+";";

}

std::vector<std::string> Grammar::printAll()
{
    std::vector<std::string> r;
    for(int i=0; i<Op.size(); i++)
    {
        r.push_back(leftVar.name+", "+rightVar.name+", "+Op[i]+";");
    }
    return r;
}
