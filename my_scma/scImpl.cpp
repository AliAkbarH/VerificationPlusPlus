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

#include <assert.h>

#include <scTheory.h>

#include <scz3helpers.h>

//mohammad
#include <future>
#include <chrono>

using namespace std;

void print_vector(vector<vocab_value_t> &choice)
{
  //	std::cout<<"SPCHK: choice is: "<<endl;
  std::cout << "[";
  unsigned int i = 0;
  for (; i < choice.size(); i++)
  {
    if (choice[i] == vuu)
    {
      std::cout << "- ";
    }
    else
    {
      std::cout << choice.at(i) << " ";
    }
  }
  std::cout << "]" << endl;
}

void SpecCheckVocab::printStats()
{
  std::cout << endl
            << endl;
  std::cout << "SPCHK: Timing and statistics report: " << endl;
  std::cout << "       number of vocab clauses: " << vocab.size() << endl;
  std::cout << "       number of variables: " << declarations.size() << endl;
  std::cout << endl;
  std::cout << "       number of queries to user: " << numOfQueries << endl;
  std::cout << "       number of vocab based partial assignments: " << numOfVocPartialAssignments << endl;
  std::cout << "       number of variable based partial assignments: " << numOfVarPartialAssignments << endl;
  std::cout << "       number of unsat cores: " << unsat_cores.size() << endl;

  std::cout << "       number of calls to sat solver (Z3): " << numOfSATCalls << endl;
  std::cout << endl;

  if (timingFlags == 0)
    return;
  if (timingFlags & TIME_SPCC)
  {
    std::cout << "       total traverse time (seconds): " << ((double)traverseTime) / 1000000 << endl;
  }
  if (timingFlags & TIME_SAT)
  {
    std::cout << "       total sat time (micorseconds): " << totalSatTime << endl;
  }
  if (timingFlags & TIME_MATCHUC)
  {
    std::cout << "       total match unsat core time (micorseconds): " << totalMatchUnsatCoreTime << endl;
  }
}

//Maya
void SpecCheckVocab::getVocabValuations()
{
  //GUI: should return this std::cout as string vocabs valuation vector
  for (unsigned int i = 0; i < vocab.size(); i++)
  {
    fprintf(stdout,
            "  +%2d+ %s is %s \n", i, vocab[i].c_str(),
            (choice[i] == vtt) ? "true" : "false");
  }
}
//Maya END

//Maya
//from getVocabPartialBadAssignments without user interaction for filling partial and without confirmation and repition question
void SpecCheckVocab::getVocabPartialBadAssignmentsMS(vector<int> partial)
{

  std::cout << "name test" << th->name << endl;
  if (!partial.empty())
  {
    vector<vocab_value_t> pattern;
    for (unsigned int i = 0; i < vocab.size(); i++)
    {
      pattern.push_back(vuu);
    }
    std::cout << "SPCHK: The reason for rejecting the model is the "
                 "partial assignment "
              << endl;
    unsat_core_t un_c;
    for (unsigned int i = 0; i < partial.size(); i++)
    {

      unsigned int idx = partial[i];
      voc_idx_val_t pr;
      pr.first = idx;
      std::cout << pr.first << endl;
      pr.second = choice[idx];
      std::cout << pr.second << endl;
      un_c.push_back(pr);

      pattern[idx] = choice[idx];
      fprintf(stdout, "+%2d+ %s is %s \n",
              idx, vocab[idx].c_str(),
              (choice[idx] == vtt) ? "true" : "false");
    }
    unsat_cores.push_back(un_c);
    std::cout << "SPCHK: ignore bad partial assignment: ";
    print_vector(pattern);
  }
}
//Maya END

void SpecCheckVocab::getVocabPartialBadAssignments()
{
  bool repeat = false;
  do
  {

    for (unsigned int i = 0; i < vocab.size(); i++)
    {
      fprintf(stdout,
              "  +%2d+ %s is %s \n", i, vocab[i].c_str(),
              (choice[i] == vtt) ? "true" : "false");
    }
    std::cout << "SPCHK: Type the ids of the vocab formulae that are the "
                 "reason for rejecting the satisfying example."
              << "Type '-1'  to finish, '-2' to restart, and '-3' to exit bad vocab formulae option." << endl;

    vector<int> partial;
    while (true)
    {
      int vid = 0;

      string strvid;
      std::cout << "USER > ";
      cin >> strvid;

      stringstream ss;
      ss << strvid;
      ss >> vid;

      if (ss.fail())
      {
        std::cout << "SPCHK: the value you entered is not a number. " << endl;
        std::cout << "SPCHK: please enter a formula id that is"
                     " part of the reason to reject the satisfying assignment, "
                  << endl;
        std::cout << "       -1 to finish, -2 to restart, or -3 to exit." << endl;
        continue;
      }

      if (0 <= vid && vid < vocab.size())
      {
        partial.push_back(vid);
        continue;
      }
      else if (vid == -1)
      {
        break;
      }
      else if (vid == -2)
      {
        repeat = true;
        partial.resize(0);
      }
      else if (vid == -3)
      {
        repeat = false;
        partial.resize(0);
      }
      else
      {
        std::cout << "SPCHK: Type the ids of the vocab formulae that are the "
                     "reason for rejecting the satisfying example."
                  << "Type '-1'  to finish, '-2' to restart, and '-3' to exit bad vocab formulae option." << endl;
      }
    }
    //GUI: needs code below
    if (!partial.empty())
    {
      vector<vocab_value_t> pattern;
      for (unsigned int i = 0; i < vocab.size(); i++)
      {
        pattern.push_back(vuu);
      }
      std::cout << "SPCHK: The reason for rejecting the model is the "
                   "partial assignment "
                << endl;
      unsat_core_t un_c;
      for (unsigned int i = 0; i < partial.size(); i++)
      {
        std::cout << i << endl;
        unsigned int idx = partial[i];
        voc_idx_val_t pr;
        pr.first = idx;
        pr.second = choice[idx];
        un_c.push_back(pr);

        pattern[idx] = choice[idx];
        fprintf(stdout, "+%2d+ %s is %s \n",
                idx, vocab[idx].c_str(),
                (choice[idx] == vtt) ? "true" : "false");
      }
      std::cout << "SPCHK: please confirm by typing 'Y'. Restart by typing 'R'. Ignore and continue by typing any other key." << endl;
      char c;
      std::cout << "USER > ";
      cin >> c;
      if (c == 'R' || c == 'r')
      {
        partial.resize(0);
        un_c.resize(0);
        repeat = true;
      }
      else if (c == 'Y' || c == 'y')
      { //GUI: always the case
        unsat_cores.push_back(un_c);
        std::cout << "SPCHK: ignore bad partial assignment: ";
        print_vector(pattern);
      }
      else
      {
        std::cout << "SPCHK: ignoring vocab selection and continuing!" << endl;
      }
    }
  } while (repeat);
}

string
SpecCheckVocab::buildDeclarationsString()
{
  //concatenate declarations
  string dec_str("extrafuns ( ");
  unsigned int i = 0;
  for (i = 0; i < declarations.size(); i++)
  {
    dec_str = dec_str + " " + declarations.at(i);
  }
  dec_str = dec_str + ") ";
  return dec_str;
}

string
SpecCheckVocab::buildVocabString()
{
  //concatenate vocab
  string voc_str("");
  unsigned int i = 0;
  for (i = 0; i < vocab.size(); i++)
  {
    voc_str = voc_str + " :formula " + vocab.at(i);
    voc_str = voc_str + " :formula ( not ( " + vocab.at(i) + ") ) ";
  }
  return voc_str;
}

string
SpecCheckVocab::buildVocabStringFormula()
{
  std::cout << "SPCHK: building SMT formulae from vocab...";
  string header = ("(benchmark SMT: ");
  string dec = buildDeclarationsString();
  string voc = buildVocabString();

  formula_str = header + dec + voc + " )";
  return formula_str;
}

void SpecCheckVocab::parseFormula()
{
  //get final form of assertion and do parsing
  formula_cstr = formula_str.c_str();
  std::cout << endl
            << formula_cstr << endl;

  std::cout << "SPCHK: parsing SMT formulae..." << endl;
  ctx = mk_ext_context();
  Z3_parse_smtlib_string(ctx->m_context, formula_cstr, 0, 0, 0, 0, 0, 0);
}

void SpecCheckVocab::buildAndParseStringFormula()
{
  std::cout << "SPCHK: building SMT formulae from vocab...";
  ctx = mk_ext_context();
  string voc_str("");
  string dec_str("(benchmark SMT: extrafuns(");

  //manipulate declarations
  unsigned int i = 0;
  for (i = 0; i < declarations.size(); i++)
  {
    dec_str = dec_str + " " + declarations.at(i);
  }
  //manipulate vocab
  for (i = 0; i < vocab.size(); i++)
  {
    voc_str = voc_str + " :formula " + vocab.at(i);
    voc_str = voc_str + " :formula ( not ( " + vocab.at(i) + ") ) ";
  }
  voc_str = voc_str + ")"; //the ')' of extrafuns

  formula_str = dec_str + ")" + voc_str; //total assertion

  //get final form of assertion and do parsing
  formula_cstr = formula_str.c_str();

  std::cout << endl
            << formula_cstr << endl;

  std::cout << "SPCHK: parsing SMT formulae..." << endl;
  Z3_parse_smtlib_string(ctx->m_context, formula_cstr, 0, 0, 0, 0, 0, 0);
}

void SpecCheckVocab::printDeclarations()
{
  /*
Z3_get_smtlib_num_formulas, Z3_get_smtlib_formula, 
Z3_get_smtlib_num_assumptions, Z3_get_smtlib_assumption, 
Z3_get_smtlib_num_decls, and Z3_get_smtlib_decl. 
*/
  unsigned int num_declarations = Z3_get_smtlib_num_decls(ctx->m_context);
  std::cout << "SPCHK: made " << num_declarations << " declarations as follows." << endl;

  unsigned int i = 0;
  for (; i < num_declarations; i++)
  {
    Z3_func_decl f = Z3_get_smtlib_decl(ctx->m_context, i);
    Z3_symbol name = Z3_get_decl_name(ctx->m_context, f);
    string key = Z3_get_symbol_string(ctx->m_context, name);
    if (key == "sk_hack")
      continue;
    Z3_ast a = Z3_mk_app(ctx->m_context, f, 0, 0);
    string dclLine = Z3_func_decl_to_string(ctx->m_context, f);

    printf("SPCHK:   %d; NAME:%s; DCL:%s; AST:%x\n",
           i, key.c_str(), dclLine.c_str(), a);
    SpecCheckVar v(i, f, name, key, dclLine, a);
    name2var[key] = v;
  }
#ifdef SPCHK_DEBUG
  Name2VarMap_t ::iterator it = name2var.begin();
  for (; it != name2var.end(); it++)
  {
    std::cout << "Map key is " << (*it).first << endl;
  }
#endif
}

//Constructor
SpecCheckVar ::SpecCheckVar(unsigned int idx, Z3_func_decl f, Z3_symbol s, string nm, string dcl, Z3_ast a)
    : dclIdx(idx), func(f), symbol(s), name(nm), dclLine(dcl), ast(a)
{
}

string
SpecCheckVocab ::modelToFormula(Z3_model model)
{
  /*
   * take the Z3_model and turn it into 
   * a conjunction of equalities between the 
   * variables in the model and their values.
   * Each equality goes into a string.
   * The string formulae is the concatenation of all
   * equalities. 
   */
  string formulae;

  Z3_context c = ctx->m_context; //shorthand name
  Z3_model m = model;            //shorthand name
  unsigned num_constants = Z3_get_model_num_constants(c, m);
  unsigned i = 0;
  for (; i < num_constants; i++)
  {
    Z3_func_decl f = Z3_get_model_constant(c, m, i);
    Z3_symbol name = Z3_get_decl_name(c, f);
    string key = Z3_get_symbol_string(c, name);
    if (key == "sk_hack")
      continue;
    Z3_ast a = Z3_mk_app(c, f, 0, 0);

    Z3_ast astVal;
    Z3_bool ok = Z3_eval(c, m, a, &astVal);

    //convert the value to a string
    string value = Z3_ast_to_string(c, astVal);

    //form the equality formula
    string equality(":formula (= ");
    equality = equality + key + " " + value + ") ";

    //concatenate all equalities
    formulae = formulae + equality;
  }
  return formulae;
}

void SpecCheckVocab::assertParsedFormulae()
{
  unsigned int i = 0;
  unsigned int num_formulas = Z3_get_smtlib_num_formulas(ctx->m_context);

  printf("SPCHK: asserting %d retractable formulas and their negations...\n", num_formulas / 2);
  unsigned int n = 0;
  for (i = 0; i < num_formulas; i++)
  {
    Z3_ast f = Z3_get_smtlib_formula(ctx->m_context, i);
    const char *f_cstr = Z3_ast_to_string(ctx->m_context, f);
    if (i % 2 == 0)
    {
      posVocClauses.push_back(f);
      n = printf("SPCHK:   formula %d: %s\n", i / 2, f_cstr);
    }
    else
    {
      negVocClauses.push_back(f);
      //          printf("%*s negated %d: %s\n", 45 - n," ", i, f_cstr);
    }
    assert_retractable_cnstr(ctx, f);
    //assert_retractable_cnstr(ctx,Z3_mk_not(ctx->m_context,f));
  }
}

/*This function checks whether the current choice represented in pattern
  matches one of the unsat_cores that we should ignore.
  If the pattern is a match, the function returns true.
  Otherwise the function returns false.
*/
bool SpecCheckVocab::matchesUnsatCore(vector<vocab_value_t> &pattern)
{
  std::cout << "name test" << th->name << endl;
  for (unsigned int i = 0; i < unsat_cores.size(); i++)
  {
    //skip is used to check if one node does
    //not agree the pattern, break
    bool skip = false;
    unsat_core_t &ithCore = unsat_cores[i];
    for (int j = 0; j < ithCore.size(); j++)
    {
      std::cout << ithCore[j].first << ":" << ithCore[j].second << endl;
    }
    for (int j = 0; j < ithCore.size(); j++)
    {
      voc_idx_val_t &corePair = ithCore[j];
      //for each node in the unsat_core
      unsigned int index = corePair.first;
      std::cout << "index : " << index << endl;
      unsigned int value = corePair.second;
      std::cout << "value : " << value << endl;
      if (pattern.at(index) != value)
      {
        std::cout << "skipped : " << pattern.at(index) << endl;
        skip = true;
        break;
      }
    }
    if (skip == false) //if it satisfies the unsat_core return true: it is a bad pattern
      return true;
  }
  return false; //if no unsat_core is satisfied, this is a good formula, try it!
}

/*
 * offset is zero in case we are coputing the vocab traversalm
 * and is vocab.size in case we are computing the equiv traversal.
 */
void SpecCheckVocab::choiceAssertRetract(unsigned int offset)
{
  /*
     go over the choice vector and we assert/retract the 
     associated formulas corresponding to the choice evaluation. 
     If choice[i] = vtt then formula [i] is asserted and (not formula[i]) is retracted. 
     if choice[i] = vff then formula [i] is retracted and (not formula[i]) is asserted.
     Note that even indices point to positive assertions, and odd indices point to negations
     */
  for (int i = 0; i < choice.size(); i++)
  {
    unsigned int cnstrIdx = offset + (2 * i);
    if (choice[i] == vff)
    {
      retract_cnstr(ctx, cnstrIdx);
      reassert_cnstr(ctx, cnstrIdx + 1);
    }
    if (choice[i] == vtt)
    {
      retract_cnstr(ctx, cnstrIdx + 1);
      reassert_cnstr(ctx, cnstrIdx);
    }
  }
}

void SpecCheckVocab::displayModel(Z3_model model)
{
  Name2VarMap_t ::iterator it = name2var.begin();
  for (; it != name2var.end(); it++)
  {
    SpecCheckVar &v = (*it).second;
    unsigned int kind = Z3_get_ast_kind(ctx->m_context, v.ast);
    Z3_ast val = v.ast;
    Z3_bool ok = Z3_eval(ctx->m_context, model, v.ast, &val);
    unsigned int numEntries = 0;
    int res = Z3_is_array_value(ctx->m_context, model, val, &numEntries);
    if (res == 1)
    {
      Z3_ast *indices = new Z3_ast[numEntries];
      Z3_ast *values = new Z3_ast[numEntries];
      Z3_ast elseVal;

      Z3_get_array_value(ctx->m_context,
                         model, val,
                         numEntries,
                         indices, values, &elseVal);
      fprintf(stdout, "  ");
      for (unsigned int i = 0; i < numEntries; i++)
      {
        display_symbol(ctx->m_context, stdout, v.symbol);
        fprintf(stdout, "[");
        display_ast(ctx->m_context, stdout, indices[i]);
        fprintf(stdout, "] = ");
        display_ast(ctx->m_context, stdout, values[i]);
        fprintf(stdout, ", ");
      }
      display_symbol(ctx->m_context, stdout, v.symbol);
      fprintf(stdout, "[otherwise]= ");
      display_ast(ctx->m_context, stdout, elseVal);
      fprintf(stdout, "\n");
    }
    else
    {
      if (th)
      {
        unsigned int id = th->var2idMap[v.name];
        VocabNode &node = th->getNode(id);
        if (node.isQuantified())
        {
          continue;
        }
      }
      fprintf(stdout, "  ");
      display_symbol(ctx->m_context, stdout, v.symbol);
      fprintf(stdout, " = ");
      display_ast(ctx->m_context, stdout, val);
      fprintf(stdout, "\n");
    }
  }
}

#if 0
void
SpecCheckVocab::displayModel( Z3_model model) {
    Name2VarMap_t :: iterator it = name2var.begin();
    for(; it != name2var.end(); it++) {
        SpecCheckVar & v = (*it).second;
        fprintf(stdout, "  ");
        display_symbol(ctx->m_context, stdout, v.symbol);

        fprintf(stdout, " = ");
        unsigned int kind = Z3_get_ast_kind(ctx->m_context, v.ast);
        Z3_ast val = v.ast;
        Z3_bool ok = Z3_eval(ctx->m_context, model, v.ast, &val);
        unsigned int numEntries=0;
        int res = Z3_is_array_value(ctx->m_context, model, val, &numEntries);
        if (res == 1) {
            Z3_ast * indices = new Z3_ast[numEntries];
            Z3_ast * values = new Z3_ast[numEntries];
            Z3_ast elseVal;

            Z3_get_array_value (ctx->m_context, 
                model, val, 
                numEntries,
                indices, values, &elseVal);
            fprintf(stdout, "{ ");
            for (unsigned int i = 0; i < numEntries; i++) {
                fprintf(stdout, "( ");
                display_ast(ctx->m_context, stdout, indices[i]);
                fprintf(stdout, "-> ");
                display_ast(ctx->m_context, stdout, values[i]);
                fprintf(stdout, "),");
            }
            fprintf(stdout, " (else -> ");
            display_ast(ctx->m_context, stdout, elseVal);
            fprintf(stdout, " )}\n");
        } else {
            display_ast(ctx->m_context, stdout, val);
            fprintf(stdout, "\n");
        }
    }
}
#endif

void SpecCheckVocab::addChoiceToSpec()
{
  std::cout << "SPCHK: Adding vocab choice to the specification." << endl;
  yesChoices.push_back(choice);

  Z3_ast args[2];
  Z3_ast args2[2];
  args[0] = Z3_mk_true(ctx->m_context);
  args2[0] = return_val;

  for (int st = 0; st < choice.size(); st++)
  {
    //Z3_ast a1;
    if (choice[st] == vtt)
    {
      //FZ: had to save the ASTs for the clauses in posVocClause and
      //FZ: negVocClause
      //FZ: and use them since we needed to
      //FZ: parse again for the quantified formula every
      //FZ: choice
      //          args[1]=Z3_get_smtlib_formula(ctx->m_context, 2*st);
      args[1] = posVocClauses[st];
      args[0] = Z3_mk_and(ctx->m_context, 2, args);
    }
    else if (choice[st] == vff)
    {
      //          args[1]=Z3_get_smtlib_formula(ctx->m_context, 2*st+1);
      args[1] = negVocClauses[st];
      args[0] = Z3_mk_and(ctx->m_context, 2, args);
      ;
    }
  }
  args2[1] = args[0]; //Z3_mk_and(ctx->m_context,choice.size(),args)
  return_val = Z3_mk_or(ctx->m_context, 2, args2);
}

void SpecCheckVocab::getPartialAssignment(vector<string> &partials)
{ //GUI
  //if the answer is no,
  // ask for the partial assignment to be tested
  std::cout << "Please indicate the variables that "
               "present unsatisfying values by name."
            << endl;
  std::cout << "Enter the names one at a time and when you're done, enter '0'." << endl;
  string variable;
  cin >> variable;
  //if the input is '0', we exit the loop since the user has indicated that he's done
  while (variable != "0")
  {
    bool found = false;
    //for each input we get, we loop throught the map table to see if it exist
    //if found as a symbol, we push it to the "POTENTIAL" partial assignment
    Name2VarMap_t::iterator it = name2var.find(variable);
    if (it == name2var.end())
    {
      //if input from user does not match any of the variables in the map, then it is invalid
      //display an error message to the user and wait for input again
      std::cout << "You entered '" << variable << "'. This is an "
                                                  "invalid variable name. Please enter a valid"
                                                  " variable name or '0' to finish."
                << endl;
    }
    else
    {
      //SpecCheckVar & v = (*it).second;
      partials.push_back(variable);
    }

    cin >> variable;
  }
}

// The following method takes an array value
// in val, defined under the model for the variable
// v.
// val is defined in a number of entries and an else
// value.
string
SpecCheckVocab::arrayValue2Formula(Z3_model model, SpecCheckVar &v, Z3_ast val, unsigned int numEntries)
{
  //            ctx->m_context, model, val, &numEntries);
  Z3_ast *indices = new Z3_ast[numEntries];
  Z3_ast *values = new Z3_ast[numEntries];
  Z3_ast elseVal;

  // this is an if then else definition for the
  // array values.
  // the result in interpreted
  // for all i in index[i].
  //   a[ i ] = values[i]
  // else
  //   a[ i ] = elseVal

  Z3_get_array_value(ctx->m_context,
                     model, val,
                     numEntries,
                     indices, values, &elseVal);

  // the following code writes an SMT formula
  // for the model of the array using the if-then-else
  // operator.
  string formula(" :formula ( forall (  (idx Int) )");
  formula += "( = (select " + v.name + " idx) ";
  string closeITE("");

  for (unsigned int i = 0; i < numEntries; i++)
  {
    string index = Z3_ast_to_string(ctx->m_context, indices[i]);
    string value = Z3_ast_to_string(ctx->m_context, values[i]);
    string term = " (ite (= idx " + index + " ) " + value + " ";
    formula += term;

    closeITE += ")";
  }
  string elseValue = Z3_ast_to_string(ctx->m_context, elseVal);
  formula += elseValue;
  formula += closeITE;
  formula += ")"; // close equal
  formula += ")"; // close forall

  std::cout << "SPCHK: array smt formula: " << formula << endl;
  return formula;
}

void SpecCheckVocab::computeEliminatedPatternsFromBadPartialAssignments(Z3_model model, vector<string> &partials)
{

  //Now that we have the partial assignments in the vector partials,
  //go over it and test if it could be useful for optimization

  //create new context
  //ctx_test is where we're going to assert the
  //potenttial partial assignments and test them
  Z3_context ctx_test = mk_context();
  Z3_model m;

  //the vector bad is used to save the partial assignments that
  // are bad i.e. to avoid(i.e. after testing in ctx_test)
  vector<int> bad;

  //the variable num_formulas is going to be used to get the
  // number of formulas in formula_cstr
  // each time we add a new one (new potential partial assignment to test)
  int num_formulas;

  //the string assertion is what we parse using Z3_parse_smtlib_string to get formulas
  //it is initialized to formula_cstr and each time we add a new formula from the
  //vector partials
  string assertion(formula_cstr);

  for (int i = 0; i < partials.size(); i++)
  {
    //add the partial[i] to test:
    //first we get the function declaration corresponding to partials[i]

    SpecCheckVar &v = name2var[partials[i]];

    Z3_func_decl var = v.func;
    Z3_symbol name = v.symbol;
    Z3_ast name_partial = v.ast;
    Z3_ast value_partial = name_partial;

    //then the value of this variable
    Z3_eval(ctx->m_context, model, name_partial, &value_partial);
    unsigned int numEntries = 0;
    int res = Z3_is_array_value(ctx->m_context, model, value_partial, &numEntries);
    string formula("");
    if (res == 1)
    {
      formula = arrayValue2Formula(model, v, value_partial, numEntries);
    }
    else
    {
      //convert the value obtained to a string
      string value = Z3_ast_to_string(ctx->m_context, value_partial);

      //enter the definition to ctx_test:
      //assert formula and get model with partial assignment
      string strNamePartial = Z3_ast_to_string(ctx->m_context, name_partial);
      formula = ":formula (= " + strNamePartial + " " + value + ") ";
    }
    assertion = assertion.substr(0, assertion.size() - 1);
    assertion = assertion + formula + " )";
    Z3_parse_smtlib_string(ctx_test, assertion.c_str(), 0, 0, 0, 0, 0, 0);
    //get the new number of formulas after adding the potential partial assignment
    num_formulas = Z3_get_smtlib_num_formulas(ctx_test);

    //get the last formula which is the last added partial assignment
    //NOTE: we do all that to get the partial assignment as an AST and be able to use it
    Z3_ast ass = Z3_get_smtlib_formula(ctx_test, num_formulas - 1);

    //assert formula and get model with partial assignment
    Z3_assert_cnstr(ctx_test, ass);
  }
  //check and get model with all partials added
  Z3_lbool res = Z3_check_and_get_model(ctx_test, &m);

  //now evaluate
  Z3_ast f;
  Z3_ast out_ast2;
  Z3_bool rt;

  //loop over all formulas
  // except the potential partials added at the end
  // and evaluate them in the
  // new model containing the potential partial assignment
  unsigned int num_voc_formulae = (num_formulas - partials.size()) / 2;
  for (int x = 0; x < num_voc_formulae; x++)
  {
    if (choice[x] == vtt)
    {
      //get the formula
      f = Z3_get_smtlib_formula(ctx_test, 2 * x);
    }
    else if (choice[x] == vff)
    {
      //get the formula
      f = Z3_get_smtlib_formula(ctx_test, 2 * x + 1);
    }
    //evaluate it in the model, the value is in out_ast2
    rt = Z3_eval(ctx_test, m, f, &out_ast2);

    //if potential partial assignment has led to some
    // formula in the original vocab
    // to evaluate to true or false, (constant)
    // then add it to the vector bad that will be at the end
    // added to the vector unsat_cores
    string outAstString = Z3_ast_to_string(ctx_test, out_ast2);
    if (outAstString.substr(0, 4) == "true")
    {
      bad.push_back(x);
    }
  }
  //delete model
  Z3_del_model(ctx_test, m);
  // delete logical context
  Z3_del_context(ctx_test);

  //add to unsat_cores
  unsat_core_t un_c;
  vector<vocab_value_t> pattern;
  for (unsigned int i = 0; i < vocab.size(); i++)
  {
    pattern.push_back(vuu);
  }
  //loop over the vector bad to put the unsat_cores in the
  // format of a vector of pairs
  for (int s = 0; s < bad.size(); s++)
  {
    voc_idx_val_t pr;

    // bad[s] is the index in the vocab
    //   of the formula that the partial assignment
    //   set to a constant
    //   the constant is choice[bad[s]]
    pr.first = bad[s];
    pr.second = choice[bad[s]];
    pattern[pr.first] = choice[bad[s]];
    un_c.push_back(pr);
  }

  //if there is a vector bad (holding partial assignments),
  // push it into the unsat_cores vector
  if (bad.size() > 0)
  {
    unsat_cores.push_back(un_c);
    std::cout << "SPCHK: ignore bad partial assignment: ";
    print_vector(pattern);
  }
}

void SpecCheckVocab::printUnsatCore(unsigned int core_size, Z3_ast *core)
{
  // if result is not 1:if it is unsat,
  // store the unsat_cores so as not to repeat them
  //store unsat cores into the vector unsat_cores

  //in the u_c pair: first is the index of the assertion
  // second is the polarity (T or F)
  vector<vocab_value_t> pattern;
  for (unsigned int i = 0; i < vocab.size(); i++)
  {
    pattern.push_back(vuu);
  }

  for (int q = 0; q < core_size; q++)
  {
    if (th)
    {
      if (core[q] == quantified_formula_ans_lit)
      {
        std::cout << "SPCHK: quantified clauses [ ";
        for (unsigned int i = 0; i < vocab.size(); i++)
        {
          VocabNode &node = th->getVocabNode(i, genVocab);
          if (node.isGlobal())
          {
            std::cout << i << " ";
            pattern[i] = choice[i];
          }
        }
        std::cout << "] are part of the unsat core." << endl;
        continue;
      }
    }
    unsigned j = 0;
    for (; j < ctx->m_num_answer_literals; j++)
    {
      if (core[q] == ctx->m_answer_literals[j])
      {
        unsigned int patIdx = j / 2;

        voc_idx_val_t p;
        p.first = j / 2;

        if (j % 2 == 0)
        {
          pattern[patIdx] = vtt;
        }
        else
        {
          pattern[patIdx] = vff;
        }
        break;
      }
    }
  }
  std::cout << "SPCHK: Adding unsat core to eliminated patterns...";
  print_vector(pattern);

  //mohammad: eliminating unsat cores
  my_ROBDD_eliminate_UNSATCORES(pattern);
}

//mohammad: eliminating unsat cores from ROBDD
void SpecCheckVocab::my_ROBDD_eliminate_UNSATCORES(vector<vocab_value_t> &choice)
{
  bdd_node current_bad_node = 1;
  bdd_node my_x;
  unsigned int i = 0;
  for (; i < choice.size(); i++)
  {
    //neglect nodes with vuu
    if (!choice[i] == vuu)
    {
      my_x = make(my_bdd, i + 1, (int)!choice[i], (int)choice[i]);       //transform current assignment to node
      current_bad_node = apply(my_bdd, &my_and, current_bad_node, my_x); // and important values of the vocabs
    }
  }
  main_node = apply(my_bdd, &my_and, main_node, apply(my_bdd, &my_xor, current_bad_node, 1)); // f & (~unsatcores)
}

//mohammad: eliminating dontcares from ROBDD
// void SpecCheckVocab::my_ROBDD_eliminate_DONTCARES(vector<Variable> Variables)
// {
//   bdd_node current_dontcare_node = 1;
//   bdd_node my_x;
//   unsigned int i = 0;
//   for (; i < Variables.size(); i++)
//   {
//     //neglecting unread variables
//     //i.e. focusing on used ones
//     if (Variables[i]->Value != "")
//     {
//       vector<int> indices = findAtomsIndices(Variables[i]); //finding indices of the used atoms
//       for (int j = 0; j < indices.size(); j++)
//       {
//         my_x = make(my_bdd, j + 1, (int)!choice[j], (int)choice[j]);       //transform current assignment to node
//         current_dontcare_node = apply(my_bdd, &my_and, current_dontcare_node, my_x); // and important values of the vocabs
//       }
//     }
//   }
//   main_node = apply(my_bdd, &my_and, main_node, apply(my_bdd, &my_xor, current_dontcare_node, 1)); // f & (~unsatcores)
// }

void SpecCheckVocab::computeEliminatedPatternsFromUnsatChoice(unsigned int core_size, Z3_ast *core)
{
  // if result is not 1:if it is unsat,
  // store the unsat_cores so as not to repeat them
  //store unsat cores into the vector unsat_cores

  //in the u_c pair: first is the index of the assertion
  // second is the polarity (T or F)
  unsat_core_t u_c;
  for (int q = 0; q < core_size; q++)
  {
    if (th)
    {
      if (core[q] == quantified_formula_ans_lit)
      {
        //            std::cout << "SPCHK: quantified formula is part of the unsat core." << endl;
        for (unsigned int i = 0; i < vocab.size(); i++)
        {
          VocabNode &node = th->getVocabNode(i, genVocab);
          if (node.isGlobal())
          {
            //                std::cout << "node " << i << " is quantified";
            voc_idx_val_t p;
            p.first = i;
            if (choice[i] == vtt)
            {
              p.second = 1;
            }
            else
            {
              p.second = 0;
            }
            u_c.push_back(p);
          }
        }
        continue;
      }
    }
    unsigned j = 0;
    for (; j < ctx->m_num_answer_literals; j++)
    {
      if (core[q] == ctx->m_answer_literals[j])
      {
        voc_idx_val_t p;
        p.first = j / 2;

        if (j % 2 == 0)
        {
          p.second = 1;
          //            std::cout << "1 ";
        }
        else
        {
          p.second = 0;
          //             std::cout << "0 ";
        }

        u_c.push_back(p);
        break;
      }
      else
      {
        //              std::cout << "- ";
      }
    }
  }
  unsat_cores.push_back(u_c);
}

//Maya
void SpecCheckVocab::displayModel(Z3_model &model, vector<string> &Display)
{
  Name2VarMap_t ::iterator it = name2var.begin();

  char bigOutBuf[8192];

  for (; it != name2var.end(); it++)
  {

    fflush(stdout);
    setvbuf(stdout, bigOutBuf, _IOFBF, 8192); //stdout uses your buffer

    SpecCheckVar &v = (*it).second;
    unsigned int kind = Z3_get_ast_kind(ctx->m_context, v.ast);
    Z3_ast val = v.ast;

    Z3_bool ok = Z3_eval(ctx->m_context, model, v.ast, &val);
    unsigned int numEntries = 0;

    int res = Z3_is_array_value(ctx->m_context, model, val, &numEntries);

    if (res == 1)
    {
      Z3_ast *indices = new Z3_ast[numEntries];
      Z3_ast *values = new Z3_ast[numEntries];
      Z3_ast elseVal;
      Z3_get_array_value(ctx->m_context,
                         model, val,
                         numEntries,
                         indices, values, &elseVal);
      fprintf(stdout, "  ");
      for (unsigned int i = 0; i < numEntries; i++)
      {
        display_symbol(ctx->m_context, stdout, v.symbol);
        fprintf(stdout, "[");
        display_ast(ctx->m_context, stdout, indices[i]);
        fprintf(stdout, "] = ");
        display_ast(ctx->m_context, stdout, values[i]);
        fprintf(stdout, ", ");
      }
      display_symbol(ctx->m_context, stdout, v.symbol);
      fprintf(stdout, "[otherwise]= ");
      display_ast(ctx->m_context, stdout, elseVal);
      fprintf(stdout, "\n");
    }
    else
    {
      fprintf(stdout, "  ");
      display_symbol(ctx->m_context, stdout, v.symbol);
      fprintf(stdout, " = ");
      display_ast(ctx->m_context, stdout, val);
      fprintf(stdout, "\n");
    }

    string str(bigOutBuf);
    Display.push_back(str);

    memset(bigOutBuf, 0, 8192); //emptying bigOutBuf
  }
}

//Maya END

//Maya
void SpecCheckVocab::checkChoiceMS()
{
  checkchoice CC(this);

  if (CC.Z3Check() == true)
  {
    vector<string> Display;
    Display = CC.displaymodel();
    //write display to GUI
    string answer;
    cin >> answer; //collect answer from GUI
    vector<string> vars_partials;
    vector<int> vocab_partials;
    CC.ReadAnswer(answer, vars_partials, vocab_partials);
  }
}
//Maya END
void SpecCheckVocab::checkChoice()
{
  std::cout << "SPCHK: checking choice...";
  print_vector(choice);
  choiceAssertRetract();
  /*
     we create an array of Z3_ast called assumptions. 
     If we have previously indicated that we want to
     assert an ast, we push it into assumptions. 

     Initially, we create the array of a size=MAX_RETRACTABLE_ASSERTIONS
     so that it can contain the maximum number of assumtions. 

     the integer num_assumptions keeps track of the 
     effective number of assumptions inserted into the array
     */
  unsigned num_assumptions = 0;
  Z3_ast assumptions[MAX_RETRACTABLE_ASSERTIONS];
  for (int i = 0; i < ctx->m_num_answer_literals; i++)
  {
    if (ctx->m_retracted[i] == Z3_FALSE)
    {
      assumptions[num_assumptions] = ctx->m_answer_literals[i];
      num_assumptions++;
    }
  }

#if 0
  if (th && (!useQuantFree) ) {
    quantified_formula_ans_lit = 0;
    quantified_formula = 0;

    string smtQuantified = th->buildQuantifiedSMTFormulae(choice, genVocab);
    if (!smtQuantified.empty()){
      std::cout << "SPCHK: pushing quantified constraint. " << endl;
      Z3_push(ctx->m_context);
      const char * smtQuantified_cstr = smtQuantified.c_str();
      //  std::cout << "SPCHK: parsing quantified formula: " << endl;
      //  std::cout << "       " << smtQuantified;
      //  std::cout << endl;

      Z3_parse_smtlib_string(ctx->m_context,smtQuantified_cstr,0,0,0,0,0,0);
      unsigned int num_formulas = Z3_get_smtlib_num_formulas(ctx->m_context);
      //std::cout << "num of formula in quantified: " << num_formulas << endl;
      Z3_ast f = Z3_get_smtlib_formula(ctx->m_context, 0);
      const char * f_cstr = Z3_ast_to_string(ctx->m_context, f);
      std::cout << "SPCHK: " << f_cstr << endl;
      std::cout << endl;

      Z3_sort ty = Z3_mk_bool_sort(ctx->m_context);
      Z3_ast ans_lit = Z3_mk_fresh_const(ctx->m_context, "k", ty);
      Z3_ast args[2];
      args[0] = f;
      args[1] = Z3_mk_not(ctx->m_context, ans_lit);
      Z3_ast fORans_lit = Z3_mk_or(ctx->m_context, 2, args);
      Z3_assert_cnstr(ctx->m_context, fORans_lit);

      //push here, then pop when done with the check.
      //assert it and add it to the assumptions
      assumptions[num_assumptions] = ans_lit;
      num_assumptions ++;
      quantified_formula_ans_lit = ans_lit;
      quantified_formula = f;
    }
  }
#endif

  numOfSATCalls++;
  //check and get unsat_cores
  unsigned core_size = 0;
  Z3_ast core[1024] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  Z3_ast proof;
  Z3_model model = 0;
  Z3_lbool result;

  std::cout << "SPCHK: Calling SMT Solver. This may take time.." << endl;

  TIME_IT(totalSatTime, timingFlags, TIME_SAT,
          (result = Z3_check_assumptions(ctx->m_context,
                                         num_assumptions, assumptions,
                                         &model, &proof, &core_size, core)));

#if 0
  if (th) {
    if(quantified_formula_ans_lit) {
      std::cout << "SPCHK: popping quantified constraint. " << endl;
      Z3_pop(ctx->m_context,1);
    }
  }
#endif

  //Now what to do with the obtained result:
  //if the assumptions are satisfiable
  // we show the model to the user
  // and ask for feedback
  if (result == Z3_L_TRUE)
  {
    numOfQueries++;
    //we create a map table that will be used to map
    // the name of the variables to their IDs
    std::cout << "SPCHK: choice is satisfiable. \n"
                 "       Is the satisfying assignment below a good model for your property?"
              << endl;
    static int twice = 2;
    if (twice)
    {
      std::cout << "       Notice that a specification accepts a don't care assignment. (this notice will be issued only twice)." << endl;
      twice--;
    }

    displayModel(model);
    std::cout << "SPCHK: Answer by 'Y' to accept and 'N' to reject the assignment. 'M' to view more details from the solver about the model. " << endl;

    /*
       Ask the user if the model is a good model as per the 
       intended spec.
       If the answer is yes, add the vocab choice to the spec
       If the answer is no, ask him about the variables whose
       values are not satisfying and try to get a partial 
       assignment to avoid
       */

    string answer;
    std::cout << "USER > ";
    cin >> answer;

    if (answer == "M" || answer == "m")
    {
      //use the z3 original printing of the model.
      display_model(ctx->m_context, stdout, model);

      //show the whole model as per z3
      std::cout << "SPCHK: Answer by 'Y' to accept and 'N' to reject the assignment." << endl;
      std::cout << "USER > ";
      cin >> answer;
    }
    while (answer != "Y" && answer != "y" && answer != "n" && answer != "N")
    {
      std::cout << "SPCHK: This is an invalid answer."
                   "Please answer by 'Y' for yes or 'N' for no."
                << endl;
      std::cout << "USER > ";
      cin >> answer;
    }

    if (answer == "Y" || answer == "y")
    {
      addChoiceToSpec();
    }
    else if (answer == "N" || answer == "n")
    {
      noChoices.push_back(choice);
      std::cout << "SPCHK: If the rejection is due to part of the assignment, " << endl;
      std::cout << "       press 'V' to select the bad variables, " << endl;
      std::cout << "       press 'B' to select the bad vocabulary values, or" << endl;
      std::cout << "       press 'C' to continue." << endl;
      char c;
      std::cout << "USER > ";
      bool repeat = true;
      while (repeat)
      {
        cin >> c;
        repeat = false;
        if (c == 'V' || c == 'v')
        {
          numOfVarPartialAssignments++;
          vector<string> partials;
          getPartialAssignment(partials);
          if (!partials.empty())
          {
            computeEliminatedPatternsFromBadPartialAssignments(model, partials);
          }
        }
        else if (c == 'B' || c == 'b')
        {
          numOfVocPartialAssignments++;
          getVocabPartialBadAssignments();
        }
        else if (c == 'C' || c == 'c')
        {
        }
        else
        {
          repeat = true;
          std::cout << "SPCHK: please enter " << endl;
          std::cout << "       press 'V' to select the bad variables, " << endl;
          std::cout << "       press 'B' to select the bad vocabulary values, or" << endl;
          std::cout << "       press 'C' to continue." << endl;
        }
      }
    }
    Z3_del_model(ctx->m_context, model);
  }
  else if (result == Z3_L_FALSE)
  {
    std::cout << "SPCHK: choice is not satisfiable. " << endl;
    if (core_size > 0)
    {
      printUnsatCore(core_size, core);
      computeEliminatedPatternsFromUnsatChoice(core_size, core);
    }
  }
  else
  { //result == Z3_L_UNDEF
    std::cout << "SPCHK: Z3 failed to compute satisfiability. consider increasing resources. " << endl;
    exit(0);
  }
}

void SpecCheckVocab::traverse()
{

  /*    each time Traverse is called, we first check the assignments we made 
      till now against the unsatisfiable
      cores we have till now; if one of the cores is satisfied we let the 
      user know and return i.e.
      if our choice conform_unsat_cores(...), then skip */

  // std::cout<<timedout;
  if (timedout)
  {
    //using normal traversal

    bool res = false;

    TIME_IT(totalMatchUnsatCoreTime, timingFlags, TIME_MATCHUC,
            (res = matchesUnsatCore(choice)))

    if (res)
    {
      std::cout << "SPCHK: Ignore: subtree satisfies an eliminated pattern.";
      //check if dont care
      //read from the file
      //chexk the roBDD
      print_vector(choice);
      return;
    }

    /*     If depth is equal to choice.size, we have assigned a value of true or false to all
        the subformulas in the vocab. 
        So we go over the choice vector and we assert/retract the 
        associated formulas corresponding to the choice evaluation. 
    */
    if (depth == choice.size())
    {
      checkChoice();
      return;
    }

    //modify position at depth/2
    choice[depth] = vtt;
    depth = depth + 1;
    traverse();
    depth = depth - 1;

    choice[depth] = vff;
    depth = depth + 1;
    traverse();
    depth = depth - 1;
    choice[depth] = vuu;
  }
  else
  {
    //using ROBDD here

    //remove last used assignment
    if (finished_first_traversal)
    {
      std::cout << "removing last used assignment\n";
      bdd_node last_assign = 1;
      bdd_node my_x;
      for (int i = 0; i < choice.size(); i++)
      {
        if (choice[i] != vuu)
        {
          my_x = make(my_bdd, i, (int)!choice[i], (int)choice[i]); //transform current assignment to node
          last_assign = apply(my_bdd, &my_and, last_assign, my_x);
        }
      }
      main_node = apply(my_bdd, &my_and, main_node, apply(my_bdd, &my_xor, last_assign, 1)); // f & (~last assignment)
    }
    // get an assignment
    //adding a timeout

    std::cout << "waiting to get new assignment...\n";
    // std::future_status status;
    // std::future<int> future = std::async(std::launch::async, [this]() {
    //   choice = my_oneSAT(my_bdd, main_node);
    //   return 1;
    // });

    // if (my_bdd->satcount(main_node) != 0)
    // {
    // do
    // {
    //   status = future.wait_for(std::chrono::minutes(10));
    //   if (status == std::future_status::deferred)
    //   {
    //     std::cout << "deferred\n";
    //   }
    //   else if (status == std::future_status::timeout)
    //   {
    //     std::cout << "timeout \n returning to use old traversal ";
    //     timedout = true;
    //   }
    //   else if (status == std::future_status::ready)
    //   {
    //     std::cout << "new assignment is ready!\n";
    //   }
    // } while (status != std::future_status::ready || status != std::future_status::timeout);
    choice = my_oneSAT(my_bdd, main_node);
    // std::cout<<"size of choice "<<choice.size()<<endl;
    // print_vector(choice);
    if (timedout)
    {
      //reinitialize the choice vector
      //can be improved by enumerating the remainging assignments of the choice vector
      for (int i = 0; i < choice.size(); i++)
      {
        choice.at(i) = vuu;
      }
      traverse();
    }
    else
    {
      bool res = false;
      TIME_IT(totalMatchUnsatCoreTime, timingFlags, TIME_MATCHUC,
              (res = matchesUnsatCore(choice)))

      if (res)
      {
        std::cout << "SPCHK: Ignore: subtree satisfies an eliminated pattern.";
        //check if dont care
        //read from the file
        //chexk the roBDD
        print_vector(choice);
        traverse();
      }
      checkChoice();
      if (!finished_first_traversal)
        finished_first_traversal = true;
      traverse();
    }
    // }
    // else
    // {
    //   //no more new assignments
    //   return;
    // }
  }
}

// mohammad: my_onesat(B, u) returns one satisfying assignment for node u in B
vector<vocab_value_t> SpecCheckVocab::my_oneSAT(bdd *B, bdd_node u)
{

  REQUIRES(is_bdd(B));
  assert(0 <= u && u < B->size);
  assert(u != 0); // bdd not satisfiable!
  vector<vocab_value_t> my_choice;
  if (u == 0)
  {
    printf("No solutions\n");
  }
  else
  {
    node *a = B->T[u];
    int v = a->var;
    std::cout << B->num_vars << endl;
    while (v <= B->num_vars)
    {
      //printf("x[%d]=", v);
      std::cout << "Mohammad check\n";
      if (a->low != 0)
      {
        //printf("0\n");
        my_choice.push_back(vff);
        u = a->low;
      }
      else
      {
        //printf("1\n");
        my_choice.push_back(vtt);
        u = a->high;
      }
      a = B->T[u];
      v = a->var;
    }
  }
  return my_choice;
}

#include <fstream>

bool SpecCheckVocab ::writeEspressoFile(const char *filename, bool bPrintUnsatCore, bool bPrintNoChoices)
{
  ofstream oEspresso;

  // this variable denotes that the file did not open
  // and the user responded with invalid input
  bool invalidInputRIX = false;
  while (true)
  {
    if (!invalidInputRIX)
    {
      oEspresso.open(filename);
    }
    if (!oEspresso.is_open())
    {
      cerr << "SPCHK: Could not open file `yes.tt' for writing to Espresso." << endl;
      cerr << "       Clear the problem and press 'R' to retry." << endl;
      cerr << "       Press 'I' to ignore the simplification step and continue." << endl;
      cerr << "       Press 'X' to exit the program." << endl;

      invalidInputRIX = false;
      char c;
      cin >> c;
      switch (c)
      {
      case 'r':
      case 'R':
        break;
      case 'i':
      case 'I':
        return false;
        break;
      case 'x':
      case 'X':
        exit(0);
        break;
      default:
        cerr << "SPCHK: invalid input" << endl;
        invalidInputRIX = true;
        break;
      }
    }
    else
    {
      break;
    }
  }

  stringstream vocNames;
  for (unsigned int i = 0; i < vocab.size(); i++)
  {
    vocNames << "v" << i << " ";
  }

  oEspresso << "# truth table from specification construction\n"
               ".i "
            << vocab.size() << "\n"
                               ".o 1 \n"
                               ".ilb "
            << vocNames.str() << "\n"
                                 ".ob Spec"
            << endl;

  for (unsigned int i = 0; i < yesChoices.size(); i++)
  {
    vector<vocab_value_t> &yesChoice = yesChoices[i];
    for (unsigned j = 0; j < choice.size(); j++)
    {
      if (yesChoice[j] == vuu)
      {
        oEspresso << "- ";
      }
      else
      {
        oEspresso << yesChoice[j] << " ";
      }
    }
    oEspresso << " 1" << endl;
  }

  if (bPrintUnsatCore)
  {
    char *pattern = new char[2 * vocab.size() + 1];
    pattern[2 * vocab.size()] = '\0';
    for (unsigned int k = 1; k < vocab.size(); k += 2)
    {
      pattern[k] = ' ';
    }

    for (unsigned int i = 0; i < unsat_cores.size(); i++)
    {
      for (unsigned int k = 1; k < vocab.size(); k += 2)
      {
        pattern[k] = '-';
      }

      unsat_core_t &ithCore = unsat_cores[i];
      for (int j = 0; j < ithCore.size(); j++)
      {
        voc_idx_val_t &corePair = ithCore[j];
        //for each node in the unsat_core
        unsigned int idx = corePair.first;
        unsigned int value = corePair.second;
        pattern[idx] = (value == 0) ? '0' : (value == 1) ? '1' : '-';
      }
      oEspresso << pattern << " 0" << endl;
    }
    delete[] pattern;
  }

  if (bPrintNoChoices)
  {
    for (unsigned int i = 0; i < noChoices.size(); i++)
    {
      vector<vocab_value_t> &noChoice = noChoices[i];
      for (unsigned j = 0; j < choice.size(); j++)
      {
        if (noChoice[j] == vuu)
        {
          oEspresso << "- ";
        }
        else
        {
          oEspresso << noChoice[j] << " ";
        }
      }
      oEspresso << " 0" << endl;
    }
  }

  oEspresso << ".e" << endl;
  oEspresso.close();
  return true;
}

string
SpecCheckVocab::readSimplifiedFormulaEqnToTT(const char *filename)
{
  ifstream iEspresso;
  iEspresso.open(filename);
  if (!iEspresso.is_open())
  {
    cerr << "SPCHK: can not open simplified formula file in '" << filename << "'. " << endl;
    return "";
  }

  string line;
  string formula;
  unsigned int linenum = 0;

  while (iEspresso.good())
  {
    getline(iEspresso, line);
    linenum++;
    if (line[0] == '#')
    {
      continue;
    }
    unsigned int i = 0;
    enum
    {
      FOUNDV,
      INVNUM,
      INFORMULA
    };
    unsigned int state = INFORMULA;

    string number;
    for (unsigned int i = 0; i < line.size(); i++)
    {
      unsigned char c = line[i];
      switch (state)
      {
      case INFORMULA:
        if (c == 'v')
        {
          state = FOUNDV;
        }
        else
        {
          formula += c;
        }
        break;
      case FOUNDV:
        if (c >= '0' && c <= '9')
        {
          number = c;
          state = INVNUM;
        }
        else
        {
          cerr << "SPCHK: error expecting number in formula from " << filename << " and met " << c << "at line " << linenum << " column " << i << "." << endl;
          return "";
        }
        break;
      case INVNUM:
        if (c >= '0' && c <= '9')
        {
          number += c;
        }
        else
        {
          unsigned int idx = atoi(number.c_str());
          if (idx < vocab.size())
          {
            if (th)
            {
              VocabNode &node = th->getVocabNode(idx, genVocab);
              formula += node.toString(*th);
            }
            else
            {
              formula += vocab[idx];
            }
          }
          else
          {
            cerr << "SPCHK: error with variable name in formula from espresso. " << endl;
            return "";
          }
          state = INFORMULA;
          formula += c;
          number.clear();
        }
        break;
      }
    }
    formula += '\n';
  }
  iEspresso.close();
  if (th)
  {
    unsigned int i = formula.find("Spec =");
    if (i != string::npos)
    {
      string prefix = "Spec = ";
      string forall;
      string exists;

      th->decoratedQuantifierPrefix(forall, exists);
      prefix += forall + exists;
      formula.replace(i, 6, prefix);
    }
  }
  std::cout << "SPCHK: formula after esprersso simplify." << endl;
  std::cout << "  " << formula << endl;
  return formula;
}

string
SpecCheckVocab::readSimplifiedFormulaKiss(const char *filename,
                                          vector<vector<vocab_value_t>> &kissChoices)
{
  ifstream iEspresso;
  iEspresso.open(filename);
  if (!iEspresso.is_open())
  {
    cerr << "SPCHK: can not open simplified formula file in '" << filename << "'. " << endl;
    return "";
  }

  string line;
  string formula = "";
  unsigned int linenum = 0;

  while (iEspresso.good())
  {
    getline(iEspresso, line);
    if (line.size() == 0)
    {
      continue;
    }
    linenum++;
    if (line[0] == '#')
    {
      continue;
    }

    string term = "";
    unsigned int terms = 0;

    vector<vocab_value_t> choice;
    for (unsigned int i = 0; i < vocab.size(); i++)
    {
      unsigned char c = line[i];
      switch (c)
      {
      case '\n':
      case '\0':
      case '\r':
        break;
      case '-':
        choice.push_back(vuu);
        break;
      case '1':
        choice.push_back(vtt);
        term += vocab[i];
        term += " ";
        terms++;
        break;
      case '0':
        choice.push_back(vff);
        term += " (not " + vocab[i] + ") ";
        terms++;
        break;
      default:
      {
        cerr << "SPCHK: expecting '-', '0', or '1' at line " << linenum << " column " << i << endl;
        cerr << "       and got '" << c << "'." << endl;
        return "";
      }
      break;
      }
    }
    if (terms > 1)
    {
      term = "(and " + term + ") ";
    }
    formula += term + "\n   ";

    assert(choice.size() == vocab.size());
    kissChoices.push_back(choice);
  }
  if (linenum > 1)
  {
    formula = "(or \n   " + formula + ") ";
  }
  formula = ":formula " + formula;
  iEspresso.close();
  std::cout << "SPCHK: SMT formula after esprersso simplify." << endl;
  std::cout << "  " << formula << endl;
  return formula;
}

// for getpid
#include <sys/types.h>
#include <unistd.h>

void SpecCheckVocab::abcSimplify(const string &filename,
                                 vector<vector<vocab_value_t>> &kissChoices)
{
  string f = abcSimplifyR(filename, kissChoices);
}

string
SpecCheckVocab::abcSimplifyR(const string &filename,
                             vector<vector<vocab_value_t>> &kissChoices)
{
  const char *abcCommandTemplate =
      "read_blif %s\n"
      "balance\n"
      "renode\n"
      "fx\n"
      "strash\n"
      "refactor\n"
      "refactor\n"
      "collapse\n"
      "balance\n"
      "collapse\n"
      "write_eqn %s\n";

  string abcFName = filename + ".abc.blif";
  string abcSpecFName = filename + ".abc.simplify";
  char abcCommand[2096];
  sprintf(abcCommand, abcCommandTemplate, abcFName.c_str(), abcSpecFName.c_str());
  ofstream abcCommandFile;
  abcCommandFile.open("abccmd");
  if (!abcCommandFile.is_open())
  {
    cerr << "SPCHK: error: can not create `abccmd' file" << endl;
    return "1";
  }
  abcCommandFile << abcCommand;
  abcCommandFile.close();

  ofstream abcFile;
  abcFile.open(abcFName.c_str());
  if (!abcFile.is_open())
  {
    cerr << "SPCHK: error: could not open file for writing abc blif." << endl;
    return "-1";
  }
  abcFile << "#ABC blif model for spec " << filename << endl;
  abcFile << ".model " << filename << endl;
  stringstream inputNames;
  for (unsigned int i = 0; i < vocab.size(); i++)
  {
    inputNames << "v" << i << " ";
  }
  abcFile << ".inputs " << inputNames.str() << endl;
  abcFile << ".outputs Spec " << endl;
  abcFile << ".names " << inputNames.str() << " Spec" << endl;

  for (unsigned int i = 0; i < kissChoices.size(); i++)
  {
    vector<vocab_value_t> &choice = kissChoices[i];
    for (unsigned int j = 0; j < choice.size(); j++)
    {
      switch (choice[j])
      {
      case vtt:
        abcFile << "1";
        break;
      case vff:
        abcFile << "0";
        break;
      case vuu:
        abcFile << "-";
        break;
      }
    }
    abcFile << " 1" << endl;
  }
  abcFile << ".end" << endl;
  abcFile.close();

  std::cout << "SPCHK: using abc for simplification." << endl;
  std::cout << "SPCHK: abc command is " << abcCommand << endl;
  system("./abc -F abccmd");

  std::cout << "SPCHK: simplified abc formula is: " << endl;
  std::cout << endl;

  string catCommand = "cat " + abcSpecFName;
  system(catCommand.c_str());

  string formula = readSimplifiedFormulaEqnToTT(
      abcSpecFName.c_str());
  return formula;
}

string
SpecCheckVocab::abcSimplifyRNA()
{

  stringstream filename;
  filename << "yes." << getpid();

  string ofname = filename.str() + ".tt";
  bool res = writeEspressoFile(ofname.c_str(), false, false);
  if (!res)
  {
    std::cout << "SPCHK: abcSimplifyRN:Ignore espresso simplifications." << endl;
    return "-1";
  }

  string espKissSimplifiedFName = filename.str() + "kiss.simplified";

  string espEqnSimplifiedFName = filename.str() + "eqntott.simplified";
  string formula = readSimplifiedFormulaEqnToTT(espEqnSimplifiedFName.c_str());
  vector<vector<vocab_value_t>> kissChoices;
  formula = readSimplifiedFormulaKiss(espKissSimplifiedFName.c_str(),
                                      kissChoices);
  string f = abcSimplifyR(filename.str(), kissChoices);
  return f;
}

void SpecCheckVocab::espressoSimplify()
{
  string f = espressoSimplifyR();
}

string
SpecCheckVocab::espressoSimplifyR()
{
  std::cout << "SPCHK: Calling logic minimization (Espresso) to simplify formula." << endl;
  stringstream filename;
  filename << "yes." << getpid();

  string ofname = filename.str() + ".tt";
  bool res = writeEspressoFile(ofname.c_str(), false, false);
  if (!res)
  {
    std::cout << "SPCHK: Ignore espresso simplifications." << endl;
    return "-1";
  }
  //
  //output options
  //-o
  //  f, fd, fr, fdr, pleasure, eqntott, kiss, cons
  //
  //Commands
  //-D
  // ESPRESSO, many, exact, qm, single_output, so, so_both,
  // simplify, echo, signature, opo, opoall, pair, pairall,
  // check, stats, verify, PLAverify, equiv, map, mapdc, fsm,
  // contain, d1merge, d1merge_in, disjoint, dsharp, intersect,
  // minterms, primes, separate, sharp, union, xor, essen,
  // expand, gasp, irred, make_sparse, reduce, taut, super_gasp,
  // lexsort, test
  //
  string espEqnSimplifiedFName = filename.str() + "eqntott.simplified";
  string espKissSimplifiedFName = filename.str() + "kiss.simplified";

  string eqnCommand = "./espresso/espresso -Dsimplify -oeqntott " + ofname + " > " + espEqnSimplifiedFName;
  string kissCommand = "./espresso/espresso -Dsimplify -okiss " + ofname + " > " + espKissSimplifiedFName;

  std::cout << "SPCHK: issuing command " << eqnCommand << endl;
  system(eqnCommand.c_str());
  std::cout << "SPCHK: issuing command " << kissCommand << endl;
  system(kissCommand.c_str());

  std::cout << "SPCHK: output from espresso" << endl;
  string catCommand = "cat " + espEqnSimplifiedFName;
  system(catCommand.c_str());

  string formula = readSimplifiedFormulaEqnToTT(espEqnSimplifiedFName.c_str());
  vector<vector<vocab_value_t>> kissChoices;
  formula = readSimplifiedFormulaKiss(espKissSimplifiedFName.c_str(),
                                      kissChoices);
  abcSimplify(filename.str(), kissChoices);
  return formula;
}
