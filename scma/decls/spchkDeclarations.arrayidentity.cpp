#include <iostream> // needed for NULL



const char * smtDcl[] = { 
    "(i Int)",
    "(j Int)",
    "(asz Int)",
    "(a (Array Int Int))",
    NULL
};

const char * smtVocab[] = {
    "(< 0 i)",
    "(= 0 i)",
    "(= i asz)",
    "(< 0 j)",
    "(= 0 j)",
    "(= j asz)",
    "(= i j)",
    "(< i j)",
    "(> asz 0)",
    "(= asz 0)",
    "(= (select a i) (select a j))",
    "(< (select a i) (select a j))",
    NULL
};

const char * smtEquiv[] = {
    "(< 0 i)",
    "(= 0 i)",
    "(= i asz)",
    "(< 0 j)",
    "(= 0 j)",
    "(= j asz)",
    "(= i j)",
    "(< i j)",
    "(> asize 0)",
    "(= asize 0)",
    "(= (select a i) (select a j))",
    "(< (select a i) (select a j))",
    NULL
};
