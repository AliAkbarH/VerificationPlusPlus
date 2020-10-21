#include <scz3helpers.h>
#include <assert.h>

string 
SpecCheckVocab::buildEquivString() {
  string eqv("");
  //build the string formulae from equivalence
  for(int i=0;i<equiv.size();i++) {
    eqv=eqv+" :formula "+equiv.at(i);
    eqv=eqv+" :formula ( not ( "+equiv.at(i)+") ) ";
  }
  return eqv;
}

void
SpecCheckVocab::buildEquivStringFormula() {
    cout << "SPCHK: building SMT formulae from vocab...";
    string header = ("(benchmark SMT: ");
    string dec = buildDeclarationsString();
    string eqv = buildEquivString();

    formula_str= header + dec + eqv + " )";
}


void
SpecCheckVocab::checkEquivChoice() 
{
    cout << "SPCHK: EQV: checking choice...";
    print_vector(choice);
    choiceAssertRetract();
    /*
       we create an array of Z3_ast called assumptions. 
       If we have previously indicated that we want to
       assert an ast, we push it into assumptions. 

       Initially, we create the array of a size=MAX_RETRACTABLE_ASSERTIONS
       so that it can contain the maximum number of assumptions. 

       the integer num_assumptions keeps track of the 
       effective number of assumptions inserted into the array
     */
    unsigned num_assumptions = 0;
    Z3_ast assumptions[MAX_RETRACTABLE_ASSERTIONS];
    for (int i = 0; i < ctx->m_num_answer_literals; i++) {
        if (ctx->m_retracted[i] == Z3_FALSE) {
            assumptions[num_assumptions] = ctx->m_answer_literals[i];
            num_assumptions ++;
        }
    }
    //check and get unsat_cores
    unsigned core_size;
    Z3_ast core[1024]={0,0,0,0,0,0,0,0,0,0,0,0};
    Z3_ast proof; 
    Z3_model model=0;
    Z3_lbool result;
    result=Z3_check_assumptions(ctx->m_context, 
            choice.size(), assumptions, 
            &model, &proof, &core_size, core);

    //Now what to do with the obtained result:
    //if the evaluation/assumptions are satisfiable
    // we show the model to the user 
    // and ask for feedback
    switch (result) {
      case Z3_L_UNDEF:
        cout << "SPCHK: unknown, potential model" << endl;
      case Z3_L_TRUE:
        {
          /*
             Ask the user if the model is a good model as per the 
             intended spec and store the model and the 
             user's judgment for later use.
             */
          cout<<"SPCHK: equiv evaluation is satisfiable."
            "Is the satisfying "
            "assignment below a good model for your property?" << endl;
          displayModel(model);
          cout<<"SPCHK: Answer by 'Y' to accept and "
            "'N' to reject the assignment." << endl;

          string answer;
          cout << "USER > ";
          cin >> answer;
          while(answer!="Y" && answer!="N") {
            cout<<"SPCHK: This is an invalid answer."
              "Please answer by 'Y' for yes or 'N' for no."<<endl;
            cout << "USER > ";
            cin>>answer;
          }

          //save the model as a formula because we need it
          //later in the sides computations.
          string modelFormula = modelToFormula(model);
          SideMapValue val(answer=="Y",answer=="N",modelFormula);
          SideMapPair kvpair(choice,val);
          eqvSides.push_back(kvpair);
          Z3_del_model(ctx->m_context, model);
        };
        break;
      case Z3_L_FALSE: // unsat is returned
        {
          cout << "SPCHK: choice is not satisfiable. " << endl;
          cout << "SPCHK: Adding unsat core to eliminated patterns...";
          printUnsatCore( core_size, core);
          computeEliminatedPatternsFromUnsatChoice (core_size, core);
        }
    }
}

void 
SpecCheckVocab::equivTraverse() {
    /* each time equivTraverse is called, 
     * check the assignment
     * against the unsatisfiable
     * cores computed so far, 
     * if the assignment matches one of the cores
     * we let the user know and ignore the assignment tree
     */
    if(matchesUnsatCore(choice))
    {
        cout<<"SPCHK: EQV: Ignore subtree satisfies an eliminated pattern.";
        print_vector(choice);
        return;
    }
    /* If depth is equal to choice.size, 
     * we have assigned a value of true or false to 
     * all the subformulae in the vocab. 
     * So we go over the choice vector and 
     * we assert/retract the associated formulas 
     * corresponding to the choice evaluation. 
     */
    if(depth==choice.size()) {
        checkEquivChoice() ;
        return;
    }

    //modify position at depth/2
    choice[depth]=vtt;
    depth = depth + 1;
    equivTraverse();
    depth = depth - 1;

    choice[depth]=vff;
    depth = depth + 1;
    equivTraverse();
    depth = depth - 1;
    choice[depth]=vuu;
}

void 
SpecCheckVocab::vocabSidesTraverse() {
    /* each time equivTraverse is called, 
     * check the assignment
     * against the unsatisfiable
     * cores computed so far, 
     * if the assignment matches one of the cores
     * we let the user know and ignore the assignment tree
     */
    if(matchesUnsatCore(choice))
    {
        cout<<"SPCHK: VcbSides: Ignore subtree satisfies an eliminated pattern.";
        print_vector(choice);
        return;
    }
    /* If depth is equal to choice.size, 
     * we have assigned a value of true or false to 
     * all the subformulae in the vocab. 
     * So we go over the choice vector and 
     * we assert/retract the associated formulas 
     * corresponding to the choice evaluation. 
     */
    if(depth==choice.size()) {
        checkVocabSidesChoice() ;
        return;
    }

    //modify position at depth/2
    choice[depth]=vtt;
    depth = depth + 1;
    vocabSidesTraverse();
    depth = depth - 1;

    choice[depth]=vff;
    depth = depth + 1;
    vocabSidesTraverse();
    depth = depth - 1;
    choice[depth]=vuu;
}

void
SpecCheckVocab::checkVocabSidesChoice() 
{
    cout << "SPCHK: EQV: checking choice...";
    print_vector(choice);
    choiceAssertRetract();
    /*
       we create an array of Z3_ast called assumptions. 
       If we have previously indicated that we want to
       assert an ast, we push it into assumptions. 

       Initially, we create the array of a size=MAX_RETRACTABLE_ASSERTIONS
       so that it can contain the maximum number of assumptions. 

       the integer num_assumptions keeps track of the 
       effective number of assumptions inserted into the array
     */
    unsigned num_assumptions = 0;
    Z3_ast assumptions[MAX_RETRACTABLE_ASSERTIONS];
    for (int i = 0; i < ctx->m_num_answer_literals; i++) {
        if (ctx->m_retracted[i] == Z3_FALSE) {
            assumptions[num_assumptions] = ctx->m_answer_literals[i];
            num_assumptions ++;
        }
    }
    //check and get unsat_cores
    unsigned core_size=0;
    Z3_ast core[1024]={0,0,0,0,0,0,0,0,0,0,0,0};
    Z3_ast proof; 
    Z3_model model=0;
    Z3_lbool result;
    result=Z3_check_assumptions(ctx->m_context, 
            choice.size(), assumptions, 
            &model, &proof, &core_size, core);

    //Now what to do with the obtained result:
    //if the evaluation/assumptions are satisfiable
    // we show the model to the user 
    // and ask for feedback
    switch (result) {
      case Z3_L_UNDEF:
        cout << "SPCHK: unknown, potential model" << endl;
      case Z3_L_TRUE:
        {

          /*
           * Go across all models in eqvSides and 
           * check whether they satisfy the current 
           * vocab-choice. 
           * If they do, add their side to the side of the
           * vocab-choice. 
           */
          cout<<"SPCHK: vocab evaluation is satisfiable."
            "Checking vtt and vff sides with " << 
            eqvSides.size() <<
            " satisfiable equivalence evaluations." ;
          VocabSide vs(choice);
          for (unsigned int i = 0; i < eqvSides.size(); i++) {
            checkEqvModelWithVocabChoice(vs,i);
          }
          if (vs.isVttVff() ){
            vocSides.push_back(vs);
          }
        };
        break;
      case Z3_L_FALSE: // unsat is returned
        {
          cout << "SPCHK: choice is not satisfiable. " << endl;
          cout << "SPCHK: Adding unsat core to eliminated patterns...";
          printUnsatCore( core_size, core);
          computeEliminatedPatternsFromUnsatChoice (core_size, core);
        }
    }
}

void 
SpecCheckVocab::checkEqvModelWithVocabChoice(VocabSide & vs, unsigned int idx) {
  SideMapPair & eqvChoiceModel = eqvSides[idx];
  SideMapKey & eqvEval = eqvChoiceModel.first;
  SideMapValue & eqvValue = eqvChoiceModel.second;

  //save the context that asserts the vocab choice
  Z3_push(ctx->m_context);

  //place the formulae from the model into 
  //a benchmark 
  string formula("(benchmark tst ");
  formula= formula + eqvValue.modelFormula + " )";

  //parse the formulae representing the equivalence model
  Z3_parse_smtlib_string(ctx->m_context, formula.c_str(),0,0,0,0,0,0);
  //go through all formulae and assert them
  unsigned int num_formulas = Z3_get_smtlib_num_formulas(ctx->m_context);
  unsigned int i = 0;
  for (i = 0; i < num_formulas; i++) {
    Z3_ast f = Z3_get_smtlib_formula(ctx->m_context, i);
    Z3_assert_cnstr(ctx->m_context, f);
  }

  //check if the vocab choice and the model are sat
  Z3_model m;
  Z3_lbool res=Z3_check_and_get_model(ctx->m_context, &m);
  switch(res) {
    case Z3_L_UNDEF:
      cout << "SPCHK: unknown, potential model" << endl;
    case Z3_L_TRUE:
      // SAT so add equivSides to the vocabSides
      vs.addEqvSide(eqvValue.isVtt, eqvValue.isVff, idx);
      break;
    case Z3_L_FALSE:
      break;
  }

  Z3_del_model(ctx->m_context, m);

  //restore the vocab choice context without the 
  //equivalence model formula constraints
  Z3_pop(ctx->m_context, 1);

}

string 
SpecCheckVocab::formulaFromChoice(vector<string> &th, SideMapKey &eval) {
  string formula("(and ");
  assert(th.size() == eval.size());
  unsigned int i = 0;
  for (; i < eval.size(); i++) {
    if (eval[i] == vtt) {
      formula += th[i];
      formula += " ";
    } else if (eval[i] == vff) {
      formula += " (not ( ";
      formula += th[i];
      formula += " ) ) ";
    } else {
      assert(0);
    }
  }
  formula += ")";
  return formula;
}

void 
SpecCheckVocab::computePrintCorrection(){
  unsigned int i = 0;
  for(; i < vocSides.size(); i++) {
    VocabSide & vs = vocSides[i];
    string vocEvalFormula = formulaFromChoice(vocab, vs.eval);
    cout << "SPCHK: Vocab evaluation " << vocEvalFormula << " intersects"
      "vtt and vff." << endl;

    string correction("(or ");
    unsigned int count = 0;
    unsigned int j = 0;
    for (;j<vs.idxEqvEvalVec.size();j++) {
      SideMapPair & eqvEvalPair = eqvSides[j];
      SideMapValue & eqvEvalValue = eqvEvalPair.second;

      //we are only interested in the vtt
      if (eqvEvalValue.isVff) {
        continue;
      }
      count++;
      SideMapKey & eqvEval = eqvEvalPair.first;
      string eqvEvalFormula = formulaFromChoice(equiv, eqvEval);
      correction += "\n  ";
      correction += eqvEvalFormula;
    }
    if (count == 1) {
      correction += "\n  true";
    }
    correction += ")";
    cout << "SPCHK: correction is a disjunction of " << count
      << " vtt equivalence evaluations as follows." << endl;
    cout << correction;
  }
}
