#ifndef _SC_THEORY_H_
#define _SC_THEORY_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cassert>

#include <vector>
#include <map>
#include <set>

#include <utility>

#include <cstdarg>
#include <scvocval.h>

#define SC_DEBUG 0

using namespace std;

typedef pair<int,int> rulekey_t;
typedef map <rulekey_t,set<string> > rulemap_t;

class SCTheory;

typedef enum {
  NODE_CONSTANT=0x01, 
  NODE_GLOBAL=0x02, NODE_UNIVERSAL=0x02,
  NODE_LOCAL=0x04,NODE_EXISTENTIAL=0x04, 
  NODE_VAR=0x8, 
  NODE_BOOL=0x10, NODE_INT=0x20, NODE_ARRAY=0x40,
  // the following line is for masks that combine flags
  NODE_BOOL_VAR=0x18, NODE_INT_VAR=0x28,NODE_INT_ARRAY=0x68, NODE_BOOL_ARRAY=0x58,

  NODE_BIN_ARTH=0x80,//+,-,...
  NODE_BIN_REL=0x80,//<,>,=,...
  NODE_BIN_BOOL=0x100,//and,or
  NODE_UNARY_ARTH=0x200,//-
  NODE_UNARY_BOOL=0x400,// not
  NODE_QUANTIFIER=0x800,//forall,exists

  NODE_ACCESS=0x1000,// [], array access

  NODE_ELEMENT=0x2000,

  NOSE_LAST
} TypeModifier_t; 

class VocabNode {public:
  unsigned int id; 
  bool isVar;
  unsigned int mode;

  int value;//when the node is a constant node, int or const
  int size;
  int rows;
  int cols;

//  unsigned int arrSizeId;
  unsigned int arrSizeMinusOneId;
  vector<unsigned int > boundIds;
  vector<unsigned int> operandIds;
  // for term construction
  vector<unsigned int> leftVars;
  vector<unsigned int> rightVars;
  string name;
  string type;

  //vector<unsigned int > indexIds;
  //"quantifier"
  //"bool"
  //"bliteral"
  //"literal"
  //"operation"
  //
  //"binary-relational";
  //"binary-arithmetic";
  //"binary-boolean";
  //"unary-boolean";
  //"unary-arithmetic";
  //"access"
  //
  //"matrix"
  //"bmatrix"
  //"array"
  //"barray"


  VocabNode(const string & s, bool v=true, unsigned int m=0, int val=0) :
    name(s), isVar(v), mode(m), value(val), 
//    numOps(0), 
//    arrSizeId(0), arrSizeMinusOneId(0)
    arrSizeMinusOneId(0)
  {
    type = "int";
    size = rows = cols = 0;
  }

  bool isConst() const {return mode &  NODE_CONSTANT;}
  bool isLocal() const {return mode &  NODE_LOCAL;}
  bool isGlobal() const {return mode &  NODE_GLOBAL;}
  bool isQuantified() const { return mode & (NODE_LOCAL | NODE_GLOBAL);}
  bool isElement() const { return mode & (NODE_ELEMENT);}
  void setElement(bool b = true) { 
    if (b) { mode |= NODE_ELEMENT;
    } else { mode &= ~NODE_ELEMENT; }
  }

  string smtType();

  void print(ostream & os);
  void printSMT(ostream & os, SCTheory & th);
  void printForClause(ostream & os, SCTheory & th);
  string toString(SCTheory &th) ;

  void getUVars(SCTheory & th);
  unsigned int getUVars(vector<unsigned int> & uvars, SCTheory & th);
};

struct combination_struct {
  VocabNode & node;
  vector<unsigned int> & uvars;
  vector<unsigned int> & indexTerms;
  vector<unsigned int> combination;

  combination_struct( VocabNode & n, vector<unsigned int> & u, vector<unsigned int> & idxt) :
    node(n), uvars(u), indexTerms(idxt) 
  {
  }

};


class SCTheory { public:
  string name;
  vector <unsigned int> constantIds; // ID's for all the constants
  vector <unsigned int> varIds; // ID's for all the variables
  vector <unsigned int> vocabIds; // ID's for all the vocab rules
                                  // contains both the theory file rules as well as the ones generated by the system
  vector <unsigned int> generatedVocabIds; // ID's for all the generated Vocab ID's by the system

  vector <string> smtVocab;
  vector <string> smtGenVocab;
  vector <string> smtQuantFreeVocab;
  vector <string> smtDeclarations;

//  string smtQuantifiedFormula;
//  string smtGenQuantifiedFormula;

  vector <VocabNode*> nodes; // stores the variables
  map<string, unsigned int> var2idMap;
  map<string, string> op2Type;

  rulemap_t rulemap; // stores the grammar rules

//  multimap<string, rulekey_t> opVarVarMMap;

  unsigned int numClauseOperationBound;
  unsigned int numQuantifierBound;

  unsigned int trueId;
  unsigned int falseId;

  vector< vector<unsigned int> > terms;

  set<unsigned int> uvars;
  set<unsigned int> uterms;
  set<unsigned int> readIndexTerms;
  set<unsigned int> boundTerms;
  vector<unsigned int> quantFreeVocIds;
  map<string,unsigned int> uniqueQFVocab;

  SCTheory();

  VocabNode & getNode(unsigned int id) {
    assert(id < nodes.size());
    return *nodes[id];
  }

  VocabNode & makeNode(const string & name,
      const string & type="int",
      bool isVar=true, 
      unsigned int mode=0, 
      int val=0); 
#if 0
//  void addRule(VocabNode & v1, VocabNode& v2, ...);
#endif
  void propagateElementAttribute();
  void fillBoundTerms();
  unsigned int cloneAndSubstituteRec(combination_struct & c, VocabNode & node) ;
  void cloneAndSubstitute(combination_struct & c);
  void combinationClone(unsigned int offset, unsigned int k, combination_struct & c);
  void insertQFNoDuplicates(unsigned int id);
  void generateQuantFreeVocab(bool fromgenerated=true);


  vector<unsigned int> &
    useVocIds(bool fromGenerated=true, bool useQuantFree=false) {
      return useQuantFree? quantFreeVocIds :
        (fromGenerated? generatedVocabIds : vocabIds);
    }
  vector<string> & 
    useSMTVocab(bool fromGenerated=true, bool useQuantFree=false) {
      return useQuantFree?  smtQuantFreeVocab :
        (fromGenerated? smtGenVocab : smtVocab);
    }


  //functions used to generate the vocab from 
  //the type theory and the grammar
  void generateVocab();
  void generateFormulae();
  string buildQuantifiedSMTFormulae( vector< vocab_value_t> & choice, bool fromGenerated = true);
  vector<string> & buildSMTFormulae(bool fromGenerated=true, bool useQuantFree=false);

  void generateFormulae(unsigned int n1, unsigned int n2);
  void generateTerms(unsigned int numOfOps);
  void generateTermsNIJ(unsigned int i, unsigned int j);
  void makeTerms(vector<unsigned int > & termsN,//store the generated nodes here
      VocabNode & n1, VocabNode & n2, // the two i,j operations nodes
      set<string> & ops);
  void injectQuantifierVars(bool injectLocal=false);
  void addQuantifiedIndex(unsigned int idx);

  // functions called from the grammar file scma.g
  // to create nodes as appropriate
  int handleDeclaration(const string & name, const string & type="int", 
      unsigned int mode=0, 
      unsigned int size=0, unsigned int rows=0, unsigned int cols=0,
      int val=0);

  int handleRemoveDeclaration(const string & name); //MS

  int handleTrue();
  int handleFalse();
  int handleTermRelationalTerm(const string & op, unsigned int t1, unsigned int t2);
  int handleTermArithTerm(const string & op, unsigned int t1, unsigned int t2);
  int handleBEBoolBE( const string & op, unsigned int e1, unsigned int e2);
  int handleNot(unsigned int be);

  void handleConstantDeclaration(int value);
  void handleRemoveConstantDeclaration(int value);//MS
  void handleGrammarRule(int id1, int id2, set<string> & ops);
  void handleRemoveGrammarRule(int id1, int id2, string rule);//MS
  int handleForall(unsigned int varid, unsigned int beid);
  int handleExists(unsigned int varid, unsigned int beid);
  int handleArrayAccess(unsigned int arrayid, unsigned int indexterm);
  int handleUnaryMinus(unsigned int termid);
  void propagateIsGlobal(VocabNode & node, unsigned int t1, unsigned int t2=-1,bool isUnary = false);


  enum {SC_DECL, SC_GRAMMAR, SC_VOCAB, SC_EQUIV, SC_SPEC, SC_BEHAVIOR,SC_INJECT,
    SC_WHERE_LAST
  }; 
  static const char * whereTable[SC_WHERE_LAST];

  int getIdOfUsedVarible (const string & name, unsigned int where);
  int getIdOfUsedConstant(int value, unsigned int where);

  // 
  VocabNode & getVocabNode(unsigned int id,
      bool genVocab=true,bool useQuantFree=false);

  void print(ostream & os);
  void printRules(ostream & os );
  void printRule(ostream & os, const rulekey_t & key, set<string> & ops);
  string printRuleToTh(const rulekey_t & key, set<string> & ops);
  void printVocab(ostream & os,vector<unsigned int> & useVocIds);
  unsigned int printQuantifierPrefix(ostream & os, bool isSMT=false);
  unsigned int decoratedQuantifierPrefix(string & forall, string & exists);

};
#endif
