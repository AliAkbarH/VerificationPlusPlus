// forall i : 0 < i, j < asz ==> a[i] = a[j]

#include <iostream> // needed for NULL

const char * smtDcl[] = { //type theory, ie type declarations
    "(a (Array Int Int))",
    "(i Int)",
    "(j Int)",
    "(asz Int)",
    NULL
};

//"(= i j)",
const char * smtVocab[] = { //vocabulary 
    "(= (select a i) (select a j))",
    "(> asz 0)",
    "(< i 0)",
    "(< i asz)",
    "(< j 0)",
    "(< j asz)",
    NULL
};

const char * smtEquiv[] = { //equivalence theory formulae
    "(= (select a i) (select a j))",
    "(= i j)",
    "(> asz 0)",
    "(< i 0)",
    "(< i asz)",
    "(< j 0)",
    "(< j asz)",
    NULL
};
