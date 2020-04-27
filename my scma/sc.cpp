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
#include <scTimer.h>
#include <scTheory.h>
#include"traverse.h" //MS
#include"checkchoice.h" //MS

using namespace std;

extern const char * smtDcl[];
extern const char * smtVocab[];

extern const char * smtEquiv[];


int spchk_main(int argc, char ** argv) {//today

  SpecCheckVocab spchk;
  spchk.isGui = false;
  spchk.th = NULL;
  spchk.quantified_formula_ans_lit = 0;
  spchk.quantified_formula = 0;

  spchk.compute_options(argc,argv);

  cout << spchk.theoryFileName;

   if (spchk.theoryFileName.empty()){
    spchk.fillDeclarations(smtDcl);
    spchk.fillVocab(smtVocab);
    spchk.fillEquiv(smtEquiv);
	} else {
    if (!spchk.readTheoryFile()){
      exit(-1);
    }
    SCTheory * th = spchk.th;
    //th->print(cout); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    spchk.genVocab |= th->vocabIds.empty();
    //th->print(cout); //<<1<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    if (spchk.genVocab) {
    	cout << "generating vocab" << endl;
      //generate vocab from type theory
      th->injectQuantifierVars(spchk.injectLocal);
      th->generateVocab();
    } else if (spchk.injectLocal) {
    	cout << "injecting local" << endl;
      th->injectQuantifierVars(spchk.injectLocal);
    }
    if (spchk.useQuantFree) {
    	cout << "generating quant free vocab" << endl;
      th->generateQuantFreeVocab(spchk.genVocab);
    }

           for(int i=0; i<th->smtDeclarations.size();i++)
           {
        	   cout << th->smtDeclarations[i] << endl;
           }
           for(int i=0; i<spchk.vocab.size();i++)
                      {
                   	   cout << spchk.vocab[i] << endl;
                      }
    spchk.vocab = th->buildSMTFormulae(spchk.genVocab, spchk.useQuantFree);

    cout<< spchk.vocab.size() << endl;

    for(int i=0; i<spchk.vocab.size();i++)
                          {
                       	   cout << spchk.vocab[i] << endl;
                          }

    spchk.declarations = th->smtDeclarations;
  }

//   cout<<"New Start"<<endl;
//   spchk.buildAndParseStringFormula();
//     spchk.printDeclarations();
//     spchk.assertParsedFormulae();
//     spchk.initTraversal();
//     cout<<"New END"<<endl;
   if(spchk.isGui)
   	{
   		//GUImain(argc,argv, spchk);
   		return 0;
   	}

   cout << "name : ";
   cout << spchk.theoryFileName;

  spchk.buildAndParseStringFormula();
  spchk.printDeclarations();
  spchk.assertParsedFormulae();
  spchk.initTraversal();
  cout << spchk.theoryFileName;

  cout<<"SPCHK: enumerating vocab evaluations using recursive traversal..."<<endl;
  TIME_IT(spchk.traverseTime,spchk.timingFlags,TIME_SPCC,
      spchk.traverse())

    cout<<"SPCHK: the formula is:"<<endl;
  cout<<Z3_ast_to_string(spchk.ctx->m_context, spchk.return_val);

  spchk.printStats();
  spchk.espressoSimplify();
  return 0;
}
