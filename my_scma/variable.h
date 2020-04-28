#ifndef VARIABLE_H
#define VARIABLE_H
#include <string>

class Variable
{
public:
    Variable();
    Variable(std::string n); //for constants
    Variable(std::string t, std::string n);

    std::string type;
    std::string name;

    std::string print();
    bool isEqualTo(Variable v);



};

#endif // VARIABLE_H
