#ifndef TRAVERSE_H
#define TRAVERSE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <setjmp.h>

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <utility>
#include <bitset>

#include <z3.h>

#include <scma.h>
#include <scTimer.h>
#include <scTheory.h>

#include <assert.h>

//mohammad
#include "robdd/robdd.h"
#include "robdd.c"
//gives an evaluation of vocabs through choice vector & ensures that given choices cover all evaluation tree
//+ repetition done after covering whole tree. When all are covered, allTraveresed is set to true
class traverse
{
private:
    void traverseUp(); //changes choice value to go up one level in the choice value tree
    vector<vocab_value_t> my_oneSAT(bdd B, bdd_node u);
    bool res; //to carry matchesUnsatCore(choice) value.
    int size;

public:
    SpecCheckVocab *SCV;
    vector<vocab_value_t> choice; // a vector that represents the evaluations of the vocabs
    int depth;                    //indicates the depth in choice value tree
    /* //used for checking choice evaluation generation regardless the speckcheckvocab
    traverse();//constructor 
    traverse(int size);//constructor
    */
    traverse();
    traverse(SpecCheckVocab &scv);
    void setTraverse(SpecCheckVocab &scv);
    bool allTraversed; //indicates when all choice values are covered assuming starting from default: choice as vector of unknowns
    vector<vocab_value_t> firstEvaluation();
    vector<vocab_value_t> nextEvaluation(); //updates choice to a new value according to the current value
    void reset();                           //resets choice to vector of unknowns, depth to 0, res to false, and allTraveresed to false
    vector<vocab_value_t> getEvaluation();  //returns choice

    //mohammad
    bdd* my_bdd;
    vector<bdd_node> my_nodes;
    bdd_node main_node=1;
    vector<vocab_value_t> current_assignment;
};
#endif
