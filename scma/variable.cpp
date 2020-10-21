#include "variable.h"

Variable::Variable()
{
    type="";
    name="";
}

Variable::Variable(std::string n)
{
    type="";
    name=n;
}

Variable::Variable(std::string t, std::string n)
{
    type=t;
    name=n;
}

std::string Variable::print()
{
    std::string r;
    if(type=="")
        r=name;
    else
        r=type+" "+name;

    return r;
}
bool Variable::isEqualTo(Variable v)
{
    return (v.name==name && v.type==type);
}
