// Equal to one of three example
// intended condition: e is equal to at least one of a, b, or c

#include <iostream> // needed for NULL

const char * smtDcl[] = { //type theory, ie type declarations
    "(a Int)",
    "(b Int)",
    "(c Int)",
    "(e Int)",
    NULL
};

const char * smtVocab[] = { //vocabulary 
    "(= a e)",
    "(= b e)",
    "(= c e)",
    NULL
};

const char * smtEquiv[] = { //equivalence theory formulae
    "(< a e)",
    "(= a e)",
    "(< b e)",
    "(= b e)",
    "(< c e)",
    "(= c e)",
    NULL
};
