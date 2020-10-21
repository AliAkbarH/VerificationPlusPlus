#include "checkchoice.h"
using namespace std;

checkchoice::checkchoice(SpecCheckVocab scv)
{
SCV=&scv;
model=0;
}

checkchoice::checkchoice(SpecCheckVocab *scv)
{
SCV=scv;
model=0;
}

/*
checkchoice::checkchoice()
{
model=0;
}
*/

bool checkchoice::Z3Check()
{
  cout << "SPCHK: checking choice...";
  print_vector(SCV->choice);
  SCV->choiceAssertRetract();
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
  for (int i = 0; i < SCV->ctx->m_num_answer_literals; i++) {
    if (SCV->ctx->m_retracted[i] == Z3_FALSE) {
      assumptions[num_assumptions] = SCV->ctx->m_answer_literals[i];
      num_assumptions ++;
    }
  }

  SCV->numOfSATCalls ++;
  //check and get unsat_cores
  unsigned core_size;
  Z3_ast core[1024]={0,0,0,0,0,0,0,0,0,0,0,0};
  Z3_ast proof; 
  Z3_lbool result;
model=0;
  TIME_IT(SCV->totalSatTime,SCV->timingFlags,TIME_SAT,
      (result=Z3_check_assumptions(SCV->ctx->m_context, 
                                   SCV->choice.size(), assumptions, 
                                   &model, &proof, &core_size, core)) );

if(result==Z3_L_FALSE)//if checkassumption result is false
  {
    cout << "SPCHK: choice is not satisfiable. " << endl;
    cout << "SPCHK: Adding unsat core to eliminated patterns...";
    SCV->printUnsatCore( core_size, core);
    SCV->computeEliminatedPatternsFromUnsatChoice (core_size, core);
    return false;
  }
else if(result==Z3_L_TRUE)
{
	return true;
}

else{
	cout<<"choice is not decidable by Z3 solver.";
	return false;
}


}


vector<string> checkchoice::displaymodel()
{
    assert(model!=0);
    SCV->numOfQueries++;
    //we create a map table that will be used to map 
    // the name of the variables to their IDs
    cout<<"SPCHK: choice is satisfiable. \n"
      "       Is the satisfying assignment below a good model for your property?" << endl;
    static int twice = 2;
    if (twice ) {
      cout<<
        "       Notice that a specification accepts a don't care assignment. (this notice will be issued only twice)." << endl;
      twice --;
    }

    vector<string> Display;

    SCV->displayModel(model, Display);
    cout<<"Display:"<<endl;

      for (int i=0; i<Display.size(); i++) {
        cout<< Display[i]<<endl;
      }

    //cout<<"SPCHK: Answer by 'Y' to accept and 'N' to reject the assignment." << endl;

    /*
       Ask the user if the model is a good model as per the 
       intended spec.
       If the answer is yes, add the vocab choice to the spec
       If the answer is no, ask him about the variables whose
       values are not satisfying and try to get a partial 
       assignment to avoid
       */

return Display;
}

void checkchoice::ReadAnswer(string answer,vector<string> vars_partials,vector<int> vocab_partials)
{
    cout << "USER > ";
    cout <<answer;

    if(answer=="Y" || answer == "y") {
      SCV->addChoiceToSpec();
    } else if(answer=="N" || answer == "n" ) {
      SCV->noChoices.push_back(SCV->choice);
    }else if(answer=="V" || answer == "v"){
      SCV->noChoices.push_back(SCV->choice);
      SCV->numOfVarPartialAssignments++;
      //vector<string> partials;
      //SCV->getPartialAssignment(partials);
//MS: no need for getPartialAssignment function. pass partials as parameter in ReadAnswer. partials: rejected variables
      if (!vars_partials.empty()) {
        SCV->computeEliminatedPatternsFromBadPartialAssignments(model, vars_partials);
      }
    }else if(answer=="B" || answer == "b"){
    	cout << "name test" << SCV->th->name <<endl;
      SCV->noChoices.push_back(SCV->choice);
      SCV->numOfVocPartialAssignments++;
      SCV->getVocabValuations();
      SCV->getVocabPartialBadAssignmentsMS(vocab_partials) ;//MS: similar to getVocabPartialBadAssignments, need to pass vector <int > partial as paremeter in ReadAnswer function (int: 0<= # < size of vocab, represents idx of vocab)


    //

    }
  
    Z3_del_model(SCV->ctx->m_context,model);
model=0;
}











