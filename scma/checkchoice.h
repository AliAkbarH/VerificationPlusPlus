#ifndef CHECKCHOICE_H
#define CHECKCHOICE_H
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<memory.h>
#include<setjmp.h>

#include <vector>
#include<iostream>
#include<string>
#include<sstream>
#include<cstring>
#include <utility>
#include <bitset>

#include<z3.h>
class SpecCheckVocab;
#include "scma.h"
#include <scTimer.h>
#include <scTheory.h>

#include <assert.h>



class checkchoice

{
private:
SpecCheckVocab *SCV;
Z3_model model;//MS: used in all methods

public:
//checkchoice();
checkchoice(SpecCheckVocab scv);
checkchoice(SpecCheckVocab *scv);

bool Z3Check();

vector<string> displaymodel();

void ReadAnswer(string answer,vector<string> vars_partials,vector<int> vocab_partials);
};

#endif
