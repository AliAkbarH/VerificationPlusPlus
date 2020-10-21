#include <iostream> // needed for NULL



const char * smtDcl[] = { 
    "(l Int)",
    "(r Int)",
    "(m Int)",
    NULL
};

const char * smtVocab[] = {
    "(< 0 l)",
    "(< 0 m)",
    "(< 0 r)",

    "(= 0 m)",
    "(= 0 r)",
    "(= 0 l)",

    "(= r l)",
    "(= r m)",
    "(= l m)",

    "(< r l)",
    "(< r m)",
    "(< l m)",
    NULL
};

const char * smtEquiv[] = {
    "(< 0 l)",
    "(< 0 m)",
    "(< 0 r)",

    "(= 0 m)",
    "(= 0 r)",
    "(= 0 l)",

    "(= r l)",
    "(= r m)",
    "(= l m)",

    "(< r l)",
    "(< r m)",
    "(< l m)",
    NULL
};
