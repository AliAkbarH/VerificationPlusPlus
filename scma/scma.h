
#ifndef _SCMA_H_
#define _SCMA_H_
#include <string>
#include <vector>
#include <map>
#include "scvocval.h"
#include "robdd/robdd.h"
//mohammad: using c header
#ifdef __cplusplus
extern "C"
{
#endif

#include "robdd/contracts.h"
#ifdef __cplusplus
}
#endif

class checkchoice;       //MS
#include "checkchoice.h" //MS

#if 0
#include <unordered_map> //needs -std=c++0x as an compiler flag. umcomment in the makefile
#endif

using namespace std;

#if 1

/**
   @name Examples
*/
/*@{*/
/**
   \brief "Hello world" example: create a Z3 logical context, and delete it.
*/

#define MAX_RETRACTABLE_ASSERTIONS 1024

/**
   \brief Very simple logical context wrapper with support for retractable constraints.
   A retractable constraint can be "removed" without using push/pop.
*/
typedef struct
{
  Z3_context m_context;
  // IMPORTANT: the fields m_answer_literals, m_retracted and m_num_answer_literals must be saved/restored
  // if push/pop operations are performed on m_context.
  Z3_ast m_answer_literals[MAX_RETRACTABLE_ASSERTIONS];
  Z3_bool m_retracted[MAX_RETRACTABLE_ASSERTIONS]; // true if the assertion was retracted.
  unsigned m_num_answer_literals;
} Z3_ext_context_struct;

typedef Z3_ext_context_struct *Z3_ext_context;

class SpecCheckVar
{
public:
  unsigned int dclIdx;
  Z3_func_decl func;
  Z3_symbol symbol;
  string name;
  string dclLine;
  Z3_ast ast;

  SpecCheckVar() : dclIdx(0), func(0), symbol(0), ast(0) {}
  SpecCheckVar(unsigned int idx, Z3_func_decl f, Z3_symbol s, string nm, string dcl, Z3_ast a);
};

typedef map<string, SpecCheckVar> Name2VarMap_t;

struct SideMapValue
{

  bool isVtt;
  bool isVff;
  string modelFormula;

  SideMapValue() : isVtt(false), isVff(false) {}
  SideMapValue(bool t, bool f) : isVtt(t), isVff(f) {}
  SideMapValue(bool t, bool f, string fm) : isVtt(t), isVff(f), modelFormula(fm) {}
};

typedef vector<vocab_value_t> SideMapKey;

struct SideMapKeyHash
{
  size_t operator()(const SideMapKey &k) const
  {
    size_t rc = 0;
    for (unsigned int j = 0; j < k.size(); j++)
    {
      rc = 3 * rc + k[j] + 1;
    }
    return rc;
  }
};

typedef pair<SideMapKey, SideMapValue> SideMapPair;
#if 0
typedef unordered_map< 
  SideMapKey, SideMapValue, SideMapKeyHash> SideMap;
#endif

struct VocabSide : public SideMapValue
{
  SideMapKey eval;
  vector<unsigned int> idxEqvEvalVec;
  VocabSide(SideMapKey &e, bool t = false, bool f = false, int eqvIdx = -1)
      : SideMapValue(t, f), eval(e)
  {
    if (eqvIdx != -1)
    {
      idxEqvEvalVec.push_back(eqvIdx);
    }
  }
  void addEqvSide(bool t, bool f, unsigned int eqvIdx)
  {
    isVtt = isVtt || t;
    isVff = isVff || f;
    idxEqvEvalVec.push_back(eqvIdx);
  }
  bool isVttVff()
  {
    return isVtt && isVff;
  }
};

class SCTheory;

typedef enum
{
  TIME_SPCC = 0x1,
  TIME_MA = 0x2,
  TIME_SAT = 0x4,
  TIME_MATCHUC = 0x8,
  TIME_ALL = 0xffff
} TIMING_FLAGS;

class SpecCheckVocab
{
public:
  // command line options
  string theoryFileName;
  bool isGui;
  bool genVocab;
  bool injectLocal;
  bool useQuantFree;

  //pointer to theories read from theoryFileName
  SCTheory *th;

  //book-keeping variables for logging and statistics
  unsigned int numOfQueries;
  unsigned int numOfVocPartialAssignments;
  unsigned int numOfVarPartialAssignments;
  unsigned int numOfSATCalls;

  unsigned int timingFlags;
  unsigned long long traverseTime;
  unsigned long long totalSatTime;
  unsigned long long totalMatchUnsatCoreTime;

  // the declarations vector contains the
  // declarations of the variables needed for the specification
  // in smt format
  // those could be provided by the user or parsed from an
  // English requirement.
  vector<string> declarations;

  // map from a string name to a variable in specCheck
  Name2VarMap_t name2var;

  // the vocab vector contains the
  // subformulae to be used in the specification
  // in smt format
  // those could be provided by the user,
  // parsed and reconstructed from another format or
  // induced from an English requirement.
  vector<string> vocab;

  // The equivalence vector holds the
  // clauses in the equivalence theory.
  vector<string> equiv;

  //choice is a vector of evaluations to the vocab
  //formulae. it is parallel to the vocab vector.
  //we use it in traverse to make sure we are
  //going over all subformulae evaluations.
  vector<vocab_value_t> choice;

  vector<vector<vocab_value_t>> yesChoices;
  vector<vector<vocab_value_t>> noChoices;

  typedef pair<int, int> voc_idx_val_t;
  typedef vector<voc_idx_val_t> unsat_core_t;

  //unsat_cores is a vector of unsat_cores detected so far

  vector<unsat_core_t> unsat_cores;

  Z3_ext_context ctx;
  Z3_ast return_val;
  Z3_ast quantified_formula_ans_lit;
  Z3_ast quantified_formula;
  vector<Z3_ast> posVocClauses;
  vector<Z3_ast> negVocClauses;

  unsigned int depth;
  string formula_str;
  const char *formula_cstr;

  vector<SideMapPair> eqvSides;
  vector<VocabSide> vocSides;

  void resetTimingAndStats()
  {
    numOfQueries = 0;
    numOfVocPartialAssignments = 0;
    numOfVarPartialAssignments = 0;
    numOfSATCalls = 0;

    traverseTime = 0;
    totalSatTime = 0;
    totalMatchUnsatCoreTime = 0;
    timingFlags = TIME_ALL;
  }

  void printStats();
  void espressoSimplify();
  string espressoSimplifyR(); //MS
  void abcSimplify(const string &filename, vector<vector<vocab_value_t>> &kissChoices);
  string abcSimplifyR(const string &filename, vector<vector<vocab_value_t>> &kissChoices); //MS
  string abcSimplifyRNA();                                                                 //MS
  bool writeEspressoFile(const char *filename, bool bPrintUnsatCore = false, bool bPrintNoChoices = false);
  string readSimplifiedFormulaEqnToTT(const char *filename);
  string readSimplifiedFormulaKiss(const char *filename, vector<vector<vocab_value_t>> &kissChoices);

  void reset()
  {
    resetTimingAndStats();
    depth = 0;
    formula_str.clear();
    formula_cstr = NULL;
    unsat_cores.clear();
    name2var.clear();
  }

  //mohammad: addign ROBDD to the class
  bdd *my_bdd;
  vector<bdd_node> my_nodes;
  bdd_node main_node = 1;
  vector<vocab_value_t> current_assignment;
  bool finished_first_traversal = false;
  //mohammad: false to use ROBDD, true to use normal BDD
  bool timedout;

  void initTraversal(bool isVocTraversal = true)
  {
    resetTimingAndStats();

    return_val = Z3_mk_false(ctx->m_context);
    depth = 0;
    unsat_cores.clear();

    unsigned int sz = isVocTraversal ? vocab.size() : equiv.size();

    choice.resize(sz);
    for (int i = 0; i < choice.size(); i++)
    {
      choice.at(i) = vuu;
    }

    //mohammad
    if (finished_first_traversal)
    {
      my_bdd->bdd_free();
    }
    my_bdd = bdd_new(sz);
    bdd_node intermediate_node = 1;
    bdd_node my_x, my_both_x;
    for (int i = 0; i < sz; i++)
    {
      // my_nodes.push_back(make(my_bdd, i + 1, 0, 1));
      my_x = make(my_bdd, i+1, 0, 1); //transform current assignment to node
      my_both_x = apply(my_bdd, &my_or, my_x, apply(my_bdd, &my_xor, my_x, 1)); // x[i] \/ ~x[i]
      main_node = apply(my_bdd, &my_and, main_node, my_both_x);
    }
    // main_node = apply(my_bdd, &my_and, last_assign, my_both_x);
    timedout = true;
    finished_first_traversal = false;

    //std::cout<<"\nMohammad Haj Hussein\n";
  }

  //mohammad
  vector<vocab_value_t> my_oneSAT(bdd* B, bdd_node u); //mohammad

  void initEquivTraversal()
  {
    initTraversal(false);
  }

  void readDeclarations(const char *fname);
  void readVocab(const char *fname);
  void readEquivalence(const char *fname);

  void fillDeclarations(const char *smtDcl[])
  {
    unsigned int i = 0;
    for (; smtDcl[i] != NULL; i++)
    {
      declarations.push_back(smtDcl[i]);
    }
    cout << "SPCHK: Filled declarations vector with " << i << " declarations." << endl;
  }

  void fillVocab(const char *smtVocab[])
  {
    unsigned int i = 0;
    for (; smtVocab[i] != NULL; i++)
    {
      vocab.push_back(smtVocab[i]);
    }
    cout << "SPCHK: Filled vocab vector with " << i << " clauses." << endl;
  }

  void fillEquiv(const char *smtEquiv[])
  {
    unsigned int i = 0;
    for (; smtEquiv[i] != NULL; i++)
    {
      equiv.push_back(smtEquiv[i]);
    }
    cout << "SPCHK: Filled Equivalence vector with " << i << " equivalence clauses." << endl;
  }

  void printUnsatCore(unsigned int core_size, Z3_ast *core);

  string buildDeclarationsString();
  string buildVocabString();
  string buildVocabStringFormula();
  string buildEquivString();
  void buildEquivStringFormula();
  void parseFormula();

  void buildAndParseStringFormula();

  void printDeclarations();
  string modelToFormula(Z3_model model);

  void assertParsedFormulae();
  bool matchesUnsatCore(vector<vocab_value_t> &pattern);
  void choiceAssertRetract(unsigned int offset = 0); //added the default offset to allow using this for equiv and ignore vocab
  void displayModel(Z3_model model);
  void displayModel(Z3_model &model, vector<string> &Display); //MS

  void addChoiceToSpec();
  void getVocabPartialBadAssignments();
  void getVocabPartialBadAssignmentsMS(vector<int> partial); //MS (collects the user answer on vocab reason)
  void getVocabValuations();                                 //MS (prints the vocab valuations)
  void getPartialAssignment(vector<string> &partials);
  string arrayValue2Formula(Z3_model model, SpecCheckVar &v, Z3_ast val, unsigned int numEntries);
  void computeEliminatedPatternsFromBadPartialAssignments(Z3_model model, vector<string> &partials);
  void computeEliminatedPatternsFromUnsatChoice(unsigned int core_size, Z3_ast *core);

  void checkChoice();

  void checkChoiceMS(); //MS

  void traverse();

  //methods for mkadqt
  void equivTraverse();
  void checkEquivChoice();

  void vocabSidesTraverse();
  void checkVocabSidesChoice();
  void checkEqvModelWithVocabChoice(VocabSide &vs, unsigned int idx);
  string formulaFromChoice(vector<string> &th, SideMapKey &eval);
  void computePrintCorrection();
  int compute_options(int argc, char **argv);
  bool readTheoryFile();

  //mohammad
  void my_ROBDD_eliminate_UNSATCORES(vector<vocab_value_t> &choice);
  // void my_ROBDD_eliminate_DONTCARES(vector<Variable> Variables);
};
void print_vector(vector<vocab_value_t> &choice);
#else
extern const char *smtDcl[];
extern const char *smtVocab[];
#endif

#endif
