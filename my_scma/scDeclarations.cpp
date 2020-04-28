#include <iostream> // needed for NULL

const char * smtDcl[] = { //type theory, ie type declarations
    "(l Int)",
    "(r Int)",
    "(m Int)",
    NULL
};

const char * smtVocab[] = { //vocabulary 
    "(< l m )",
    "(< m r )",
    NULL
};

const char * smtEquiv[] = { //equivalence theory formulae
    "(< l m )",
    "(= l m )",
    "(< m r )",
    "(= m r )",
    "(< l r )",
    "(= l r )",
    NULL
};
