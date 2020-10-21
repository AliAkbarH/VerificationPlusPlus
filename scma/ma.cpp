#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<memory.h>
#include<setjmp.h>

#include <vector>
#include<iostream>
#include<string>
#include<cstring>
#include <utility>
#include <bitset>

#include<z3.h>

#include <scma.h>

using namespace std;


extern const char * smtDcl[];
extern const char * smtVocab[];
extern const char * smtEquiv[];

int ma_main(int argc, char ** argv) 
{
	SpecCheckVocab spchk;

	spchk.fillDeclarations(smtDcl);
	spchk.fillVocab(smtVocab);
	spchk.fillEquiv(smtEquiv);

  // build formula for equivalence evaluations
  // sat checking
  spchk.buildEquivStringFormula();
  spchk.parseFormula();
	spchk.printDeclarations();
	spchk.assertParsedFormulae();

	spchk.initEquivTraversal();

	cout << "SPCHK: enumerating equiv evaluations using recursive traversal..." << endl;
	spchk.equivTraverse();

	cout << "SPCHK: enumerating vocab evaluations using recursive traversal..." << endl;

  //build formula to check vocab evaluations against 
  //sat equiv-evaluations and their models
	spchk.reset();
  spchk.buildVocabStringFormula();
  spchk.parseFormula();
	spchk.printDeclarations();
	spchk.assertParsedFormulae();
	spchk.initTraversal();
  spchk.vocabSidesTraverse();

  // now we have the vocab evaluations with 
  // multiple sides in vocSides for each vocab evaluation
  //
  // if vocSides is empty then vocab is adequate
  if (spchk.vocSides.empty()) {
    cout << "SPCHK: vocab is adequate under given equivalence." << endl;
  } else {
    // compute correction formulae
    cout << "SPCHK: vocab is NOT adequate under given equivalence." << endl;
    spchk.computePrintCorrection();
  }

#if 0
	cout<<"SPCHK: the formula is:"<<endl;
	cout<<Z3_ast_to_string(spchk.ctx->m_context, spchk.return_val);
#endif

	return 0;
}
