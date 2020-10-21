#include <scTheory.h>

VocabNode & SCTheory::makeNode(const string & name,
      const string & type,
      bool isVar, 
      unsigned int mode,
      int val) {
  unsigned int id = nodes.size();
  VocabNode & node = *(new VocabNode (name,isVar,mode,val));
  node.type = type;
  node.id = id;

  nodes.push_back(&node);
  assert(node.id == id);
  assert(id == nodes.size() -1);
  return *nodes[id];
}

#if 0
void 
SCTheory::addRule(VocabNode & v1, VocabNode& v2, ...) {
  rulekey_t key(v1.id,v2.id);
  va_list arguments; // A place to store the list of arguments
  va_start (arguments,v2); // Initializing arguments to store all values after num
  char * op= va_arg(arguments, char *);
  while(op) {
    rulemap[key].insert(op);
    op = va_arg(arguments, char *);
  }
  va_end ( arguments );       
}
#endif

void
SCTheory::makeTerms(
  vector<unsigned int > & termsN,//store the generated nodes here
  VocabNode & n1, VocabNode & n2, // the two i,j operations nodes
  set<string> & ops) {
  set<string>::iterator itr = ops.begin();
  for(;itr!=ops.end();itr++){
    const string & op = *itr;
    //generate (n1 op n2) as a node
    // add the id to termsN
    string type = op2Type[op];
    if ( type == "binary-arithmetic") {
      if (n1.type == "array" || n2.type == "barray"
          ||
          n2.type == "array" || n2.type == "barray"){
      } else {
        unsigned int id = handleTermArithTerm(op, n1.id, n2.id);
        termsN.push_back(id);
      }
    } else if (type == "access") {
      if (n1.type == "array" || n1.type == "barray"){
        unsigned int id = handleArrayAccess(n1.id, n2.id);
        termsN.push_back(id);
      }
    }
  }
}

void
SCTheory::generateTermsNIJ(unsigned int i, unsigned int j) {
  //generate terms with n operations where i+j = n
  unsigned int n = i+j+1;
  vector<unsigned int > & termsN = terms[n];
  vector<unsigned int > & termsI = terms[i];
  vector<unsigned int > & termsJ = terms[j];
  for (unsigned int k = 0; k <  termsI.size(); k++) {
    unsigned int idI = termsI[k];
    VocabNode & n1 = getNode(idI);

    for(unsigned int l = 0; l < termsJ.size(); l++) {
      unsigned int idJ = termsJ[l];
      if (idI == idJ) {continue;}
      VocabNode & n2 = getNode(idJ);

      //check if one of the rightVars of n1 connect to 
      // one of the leftVars of n2
      for( unsigned int i1 = 0; i1 < n1.rightVars.size();i1++) {
        unsigned int id1 = n1.rightVars[i1];
        for( unsigned int j1 = 0; j1 < n2.leftVars.size();j1++) {
          unsigned int id2 = n2.leftVars[j1];
          //check if the grammar connects the two
          // nodes
          rulekey_t key(id1,id2);
          rulemap_t::iterator itr = rulemap.find(key);
          if (itr == rulemap.end()){
            continue;
          }
          //in here the grammar connects the nodes n1 and n2
          // get the operations and 
          // perform the connections
          set<string> & ops = itr->second;
          makeTerms(termsN,n1,n2,ops);
        }
      }
    }
  }
}

void
SCTheory::generateTerms(unsigned int numOfOps) {
  //generate all terms with no operations
  if (numOfOps == 0) {
    //copy constantIds to terms[0]
    terms.push_back(constantIds);
    for (unsigned int i = 0; i < varIds.size(); i++) {
      unsigned int id = varIds[i];
      VocabNode & var = getNode(id);

      terms[0].push_back(id);
    }
    return;
  }

  //generate all terms with a smaller number of 
  //operations. 
  generateTerms(numOfOps - 1);

  //create an empty vector of operations
  terms.push_back(vector<unsigned int>());
  for (unsigned int i = 0; i < numOfOps; i++) {

    //look at terms with i < numOfOps operations
    // and at terms with numOfOps - 1 - i
    // and combine them via adding one legitimate
    // operation to build terms with 
    // (1+i+numOfOps-1-i)=numOfOps operations
    unsigned int j = (numOfOps - 1) - i;
    generateTermsNIJ(i, j);
  }
}

void
SCTheory::generateFormulae(unsigned int n1,unsigned int n2){
  VocabNode & node1 = getNode(n1);
  VocabNode & node2 = getNode(n2);
  for (unsigned int i=0; i<node1.rightVars.size(); i++){
    unsigned int id1 = node1.rightVars[i];
    for (unsigned int j=0; j<node2.leftVars.size(); j++){
      unsigned int id2 = node2.leftVars[j];
      rulekey_t key(id1,id2);
      rulemap_t::iterator itr = rulemap.find(key);
      if (itr == rulemap.end()){
        continue;
      }
      set<string> & ops = itr->second;
      set<string>::iterator itr2 = ops.begin();
      for(;itr2!=ops.end();itr2++){
        const string & op = *itr2;
        if (op2Type[op] == "binary-relational") {
          if ((node1.type == "array" && node2.type == "array")
              ||
              (node1.type == "barray" && node2.type == "barray")) {
            if (op == "=") {
              unsigned int id = handleTermRelationalTerm(op,n1,n2);
              generatedVocabIds.push_back(id);
            }
          } else if (node1.type == "array" || node2.type == "array" || node1.type == "barray" || node2.type == "barray") {
          } else {
            unsigned int id = handleTermRelationalTerm(op,n1,n2);
            generatedVocabIds.push_back(id);
          }
        }
      }
    }
  }
}


void
SCTheory::generateFormulae() {
  //now apply binary relational operations amongst
  // terms where grammar (rulemap) allows.
  //
  for(unsigned int i = 0; i < terms.size(); i++) {
    vector<unsigned int > & termsI = terms[i];
    for(unsigned int j = 0; j <termsI.size(); j++) {
      unsigned int n1 = termsI[j];
      for (unsigned int k=0; k<terms.size(); k++) {
        vector<unsigned int > & termsK = terms[k];
        for(unsigned int l = 0; l <termsK.size(); l++) {
          unsigned int n2 = termsK[l];
          if (n1 == n2) { continue;};
          //check rightVars of n1 and leftVars of n2
          //for compatibility
          generateFormulae(n1,n2);
        }
      }
    }
  }
}


// a method that generates the vocab from the grammar rules and type theory
void
SCTheory::generateVocab () {
  // generateAllTerms of size numClauseOperationBound -1
#if SC_DEBUG
  cout << "number of nodes before genterms: " << nodes.size() << endl;
#endif
  generateTerms(numClauseOperationBound-1);
#if SC_DEBUG
  cout << "number of nodes after genterms: " << nodes.size() << endl;
#endif
  //now apply binary relational operations amongst
  // terms where grammar (rulemap) allows.
  //
  generateFormulae();
#if SC_DEBUG
  cout << "number of nodes after genFormulae: " << nodes.size() << endl;
#endif
}


#if 0
int
main(int argc, char ** argv){
  SCTheory th;

  th.numQuantifiersBound=1;
  th.numOpPerClause=2;

  VocabNode & left = th.addNode("left","int",true,false,false);
  VocabNode & right = th.addNode("right","int",true,false,false);
  VocabNode & e = th.addNode("e","int",true,false,false);
  VocabNode & rv = th.addNode("rv","int",true,false,false);
  VocabNode & a = th.addNode("a","array",true,false,false);
  VocabNode & i = th.addNode("a","int",true,false,true);

  th.addRule(left,right,"<","=",NULL);
  th.addRule(left,a,"bound",NULL);
  th.addRule(right,a,"bound",NULL);
  th.addRule(rv,i,"=",NULL);
  th.addRule(e,a,"=",NULL);
  th.addRule(i,a,"[]",NULL);
  th.addRule(i,left,"<","=",NULL);
  th.addRule(i,right,"<","=",NULL);

  the.generateVocab();

  return 0;
}
#endif

void
SCTheory::propagateElementAttribute() {
  //go over all relational nodes: <,> <=,>=,=
  //if one of the operands is a data element
  //  make the second so unless it was a constant
  // -- report inconsistencies: index terms should not
  //    be elements!
  //stop when no more nodes are marked element    
  unsigned int marked = 0;
  do{
    marked = 0;
    for(unsigned int i=0; i < nodes.size(); i++){
      VocabNode & node = getNode(i);
      if (node.type != "binary-relational"){
        continue;
      }
      unsigned int id1 = node.operandIds[0];
      unsigned int id2 = node.operandIds[1];
      VocabNode & node1 = getNode(id1);
      VocabNode & node2 = getNode(id2);

      if (!node1.isElement() && !node2.isElement() ) {
        continue;
      }
      set<unsigned int>::iterator itr1 = readIndexTerms.find(id1);
      if (itr1!=readIndexTerms.end()){
        cerr << "SPCHK: error: ";
        cerr << "index term `";
        node1.print(cerr);
        cerr << "' is used as an element." << endl;
        exit(-1);
      }
      set<unsigned int>::iterator itr2 = readIndexTerms.find(id2);
      if (itr2!=readIndexTerms.end()){
        cerr << "SPCHK: error: ";
        cerr << "index term `";
        node2.print(cerr);
        cerr << "' is used as an element." << endl;
        exit(-1);
      }
      set<unsigned int>::iterator itr3 = uterms.find(id1);
      if (itr3!=uterms.end()){
        cerr << "SPCHK: error: ";
        cerr << "universal index term `";
        node1.print(cerr);
        cerr << "' is used as an element." << endl;
        exit(-1);
      }
      set<unsigned int>::iterator itr4 = uterms.find(id2);
      if (itr4!=uterms.end()){
        cerr << "SPCHK: error: ";
        cerr << "universal index term `";
        node2.print(cerr);
        cerr << "' is used as an element." << endl;
        exit(-1);
      }
      if (!node1.isElement()) {
        node1.setElement();
        marked++;
      } else if (!node2.isElement()){
        node2.setElement();
        marked++;
      } else {
      }
    }
  } while(marked > 0);
#if 1
  cout << "Element nodes: {" << endl;
  for(unsigned int i=0; i < nodes.size(); i++){
    VocabNode & node = getNode(i);
    if (node.isElement()){
      cout << "  " << node.toString(*this) << endl;
    }
  }
  cout << "}" << endl;   
#endif
}

void
SCTheory::fillBoundTerms() {
  //now elements in relational (<,<=,=>,>=) syntax with 
  //  array elements are marked as elements
  //The rest are the bound terms. 
  for(unsigned int i=0; i < nodes.size(); i++) {
    VocabNode & node = getNode(i);
    if (node.type != "binary-relational"){
      continue;
    }

    unsigned int id1 = node.operandIds[0];
    unsigned int id2 = node.operandIds[1];
    VocabNode & node1 = getNode(id1);
    VocabNode & node2 = getNode(id2);

    if (node1.isElement() || node1.isGlobal()) {
    } else {
      set<unsigned int > :: iterator itr = readIndexTerms.find(id1);
      if (itr == readIndexTerms.end()){
        boundTerms.insert(id1);
      }
    }

    if (node2.isElement() || node2.isGlobal()) {
    } else {
      set<unsigned int > :: iterator itr = readIndexTerms.find(id2);
      if (itr == readIndexTerms.end()){
        boundTerms.insert(id2);
      }
    }
  }

#if SC_DEBUG
  cout << "Bound terms {" << endl;
  set<unsigned int>::iterator itr = boundTerms.begin();
  for(; itr != boundTerms.end(); itr++){
    unsigned int id = *itr;
    VocabNode & node = getNode(id);
    cout << "  " << node.toString(*this) << endl;
  }
  cout << "}" << endl;   
#endif
}

unsigned int
SCTheory::cloneAndSubstituteRec(combination_struct & c, VocabNode & node) {
  if (node.isConst()){
    return node.id;
  }
  if (node.isVar) {
    if (node.isGlobal()){
      unsigned int i = 0; 
      for(; i < c.uvars.size(); i++) {
        if (c.uvars[i] == node.id) {
          break;
        } }
      // the node is a var and it is global
      // it must be in uvars
      assert(i != c.uvars.size());
      unsigned int subid = c.combination[i];
      return subid;
    }
    return node.id;
  }
  if (!node.isGlobal()) {
    // if the intermediate node does not contain
    //  a universally quantified variable,
    //  no need to substitute in it. 
    return node.id;
  }
  assert(node.type != "quantifier");
  // node here is either a binary or a unary op
  unsigned int id1 = node.operandIds[0];
  VocabNode & node1 = getNode(id1);
  unsigned int subid1 = cloneAndSubstituteRec(c,node1);

  unsigned int subid2=0;
  unsigned int retid =0;

  if (node.type.find("binary") == 0 || 
      node.type == "access" ) {
    unsigned int id2 = node.operandIds[1];
    VocabNode & node2 = getNode(id2);
    subid2 = cloneAndSubstituteRec(c,node2);
    if (node.type == "binary-relational") {
      retid = handleTermRelationalTerm(node.name, subid1, subid2);
      return retid;
    } else if (node.type == "binary-arithmetic") {
      retid = handleTermArithTerm(node.name, subid1, subid2);
      return retid;
    } else if (node.type == "binary-boolean") {
      retid = handleBEBoolBE(node.name, subid1,subid2);
      return retid;
    } else if (node.type == "access") {
      retid = handleArrayAccess(subid1,subid2);
      return retid;
    }
    const int paranoid_not_binary_check=0;
    cout << "PANIC: paranoid_not_binary_check should not be here " << node.toString(*this) << endl;
    assert(paranoid_not_binary_check);
  } else {
    if (node.type == "unary-boolean") {
      retid = handleNot(subid1);
      return retid;
    } else if (node.type == "unary-arithmetic") {
      retid = handleUnaryMinus(subid1);
      return retid;
    }
    const int paranoid_not_unary_check=0;
    cout << "PANIC: paranoid_not_unary_check should not be here " << node.toString(*this) << endl;
    assert(paranoid_not_unary_check);
  }
  const int should_not_be_here = 0;
  cout << "PANIC: should not be here " << node.toString(*this) << endl;
  assert(should_not_be_here);
  return 0;
}

void 
SCTheory::insertQFNoDuplicates(unsigned int id){
  VocabNode & node = getNode(id);
  string key = node.toString(*this);
  if (uniqueQFVocab.find(key) == uniqueQFVocab.end()){
    quantFreeVocIds.push_back(id);
    uniqueQFVocab[key] = id;
  }
}

void
SCTheory::cloneAndSubstitute(combination_struct & c){
  unsigned int subid = cloneAndSubstituteRec(c,c.node);
  insertQFNoDuplicates(subid);
}

void
SCTheory::combinationClone(unsigned int offset, unsigned int k, combination_struct & c) {
  if (k == 0) {
    assert(c.combination.size() == c.uvars.size());
    cloneAndSubstitute(c);
    return;
  }
  for(unsigned int i = offset; i < c.indexTerms.size(); i++) {
    c.combination.push_back(c.indexTerms[i]);
    combinationClone(i+1,k-1,c);
    c.combination.pop_back();
  }
}

void
SCTheory::generateQuantFreeVocab(bool fromGenerated) {
  // now all terms going into index are either 
  //  in uterms or in readIndexterms
  //
  // we need to propagate the element attribute
  propagateElementAttribute();

  //now elements in relational (<,<=,=>,>=) syntax with 
  //  array elements are marked as elements
  //The rest are the bound terms. 
  fillBoundTerms();

#ifdef SC_DEBUG
  cout << "Read index terms {" << endl;
  set<unsigned int>::iterator itr = readIndexTerms.begin();
  for(; itr != readIndexTerms.end(); itr++){
    unsigned int id = *itr;
    VocabNode & node = getNode(id);
    cout << "  " << node.toString(*this) << endl;
  }
  cout << "}" << endl;   
#endif

  vector<unsigned int> & useIds = 
    fromGenerated? generatedVocabIds : vocabIds;

  // the indexterms vector should have both the
  //  bounds and the readindexterms
  vector<unsigned int> indexTerms;
  indexTerms.insert(indexTerms.end(),readIndexTerms.begin(), readIndexTerms.end());
  indexTerms.insert(indexTerms.end(),boundTerms.begin(), boundTerms.end());
#ifdef SC_DEBUG
  cout << "|indexterms=bound+readindex|= " << indexTerms.size() << endl;
#endif

  for(unsigned int i = 0; i < useIds.size(); i++) {
    unsigned int id = useIds[i];
    VocabNode & node = getNode(id);
    if (!node.isGlobal()){
      insertQFNoDuplicates(id);
      continue;
    }
    vector<unsigned int> nodeUVars;
    node.getUVars(nodeUVars,*this);
    //generate n choose k combinations of indexterms
    //  where n is the number of terms and 
    //  k is the number of uvars in the vocab clause
    //
    //  for each combination, clone the vocab clause
    //  and substitue
    combination_struct c(node,nodeUVars,indexTerms);
    combinationClone(0, nodeUVars.size(), c);
  }
}
