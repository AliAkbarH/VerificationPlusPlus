#include <scTheory.h>
#include<algorithm>
#include <sstream> //for stringstream

void
VocabNode::print(ostream & os) {
  os << "  node '" << name << "' of type " <<  type << " " <<
    ( isVar        ? "v" : "-" ) <<
    ( isConst()    ? "c" : "-" ) <<
    ( isGlobal()   ? "u" : (isLocal() ? "e" : "-" ) )<<
    ( isElement()   ? "d" : "-" ) <<
    " value " << value <<
    " sizes (" << size << "," << rows <<"," << cols << ") " <<
    " id " << id << " and " << operandIds.size() <<
    " operands." << endl;
}

string 
VocabNode::toString(SCTheory &th) {
  stringstream ss;
  printForClause(ss,th);
  return ss.str();
}

void VocabNode::printForClause(ostream & os, SCTheory & th) {
  if (isVar || type == "bliteral") {
    os << name;
    return;
  } 
  if (isConst()) {
    os << value;
    return;
  }
  if (type == "quantifier") {
    VocabNode & var = th.getNode(operandIds[0]);
    VocabNode & be  = th.getNode(operandIds[1]);
    os << "(" << name << " " << var.name << ". ";
    be.printForClause(os,th);
    os << ")";
    return;
  }
  if (type == "access") {
    VocabNode & var = th.getNode(operandIds[0]);
    VocabNode & term = th.getNode(operandIds[1]);
    os << var.name << "[";
    term.printForClause(os, th);
    os << "]";
    return;
  }
  if (type.find("binary") == 0) {
    VocabNode & op1 = th.getNode(operandIds[0]);
    VocabNode & op2 = th.getNode(operandIds[1]);
    os << "(";
    op1.printForClause(os,th);
    os << " " << name << " ";
    op2.printForClause(os,th);
    os << ")";
    return;
  }
  if (type.find("unary") == 0) {
    VocabNode & op1 = th.getNode(operandIds[0]);
    os << "(" << name << " ";
    op1.printForClause(os,th);
    os << ")";
    return;
  }
  os << "Unknown node ";
  print(os);
}


unsigned int
VocabNode::getUVars(vector<unsigned int> & uvars, SCTheory & th) {
  if (!isGlobal())
    return 0;

  if (isVar) {
    uvars.push_back(id);
    return uvars.size();
  } 
  for(unsigned int i = 0; i < operandIds.size(); i++) {
    VocabNode & node = th.getNode(operandIds[i]);
    node.getUVars(uvars,th);
  }
  return uvars.size();
}

void
VocabNode::getUVars(SCTheory & th) {
  if (!isGlobal())
    return;

  if (isVar) {
    th.uvars.insert(id);
    return;
  } 
  for(unsigned int i = 0; i < operandIds.size(); i++) {
    VocabNode & node = th.getNode(operandIds[i]);
    node.getUVars(th);
  }
}


string 
VocabNode::smtType(){
    if (type == "int") {
      return "Int";
    } else if (type == "array") {
      return "(Array Int Int)";
    } else if (type == "bool") {
      return "Bool";
    } else if (type == "barray") {
      return "(Array Int Bool)";
    }
    return "(unknown)";
}

void
VocabNode::printSMT(ostream & os, SCTheory & th) {
  if (isVar || type == "bliteral") {
    os << name;
    return;
  } 
  if (isConst()) {
    os << value;
    return;
  }
  if (type == "quantifier") {
    os << "(" << name << " ((";
    VocabNode & var = th.getNode(operandIds[0]);
    os << var.name << " " << var.smtType() << "))";
    VocabNode & be  = th.getNode(operandIds[1]);
    be.printSMT(os,th);
    os << ")";
    return;
  }
  if (type == "access") {
    VocabNode & var = th.getNode(operandIds[0]);
    VocabNode & term = th.getNode(operandIds[1]);
    os << "(select " << var.name << " ";
    term.printSMT(os, th);
    os << ")";
    return;
  } 
  if (type.find("binary") == 0) {
    VocabNode & op1 = th.getNode(operandIds[0]);
    VocabNode & op2 = th.getNode(operandIds[1]);
    os << "(" << name << " ";
    op1.printSMT(os,th);
    os << " ";
    op2.printSMT(os,th);
    os << ")";
    return;
  }
  if (type.find("unary") == 0) {
    VocabNode & op1 = th.getNode(operandIds[0]);
    os << "(" << name << " ";
    op1.printSMT(os,th);
    os << ")";
    return;
  }
  os << "Unknown node ";
  print(os);
}

const char * 
SCTheory::whereTable[SC_WHERE_LAST] = {"declaration","grammar","vocab","equiv","spec","behavior"};

SCTheory::SCTheory() :
  numClauseOperationBound(3)
  , numQuantifierBound(1)
  , trueId(-1)
  , falseId(-1)
{
  op2Type["forall"] = "quantifier";
  op2Type["exists"] = "quantifier";

  //"bool"
  //"bliteral"
  //"literal"
  //"unary-arithmetic";
  //"matrix"
  //"bmatrix"
  //"array"
  //"barray"

  op2Type["<"] = "binary-relational";
  op2Type[">"] = "binary-relational";
  op2Type["="] = "binary-relational";
  op2Type["<="] = "binary-relational";
  op2Type[">="] = "binary-relational";

  op2Type["and"] = "binary-boolean";
  op2Type["&&"] = "binary-boolean";
  op2Type["or"] = "binary-boolean";
  op2Type["||"] = "binary-boolean";
  op2Type["xor"] = "binary-boolean";
  op2Type["implies"] = "binary-boolean";
  op2Type["iff"] = "binary-boolean";

  op2Type["not"] = "unary-boolean";
  op2Type["!"] = "unary-boolean";

  op2Type["+"] = "binary-arithmetic";
  op2Type["-"] = "binary-arithmetic";
  op2Type["*"] = "binary-arithmetic";
  op2Type["/"] = "binary-arithmetic";
  op2Type["%"] = "binary-arithmetic";

  op2Type["[]"] = "access";
}

int 
SCTheory::
handleDeclaration(const string & name, const string & type, 
    unsigned int mode, 
    unsigned int size, unsigned int rows, unsigned int cols,
      int val)
{

  if (var2idMap.find(name) != var2idMap.end()){
    cerr << "SPCHK: warning: variable `" << name << "' already declared. ignoring second declaration." << endl;
    int id = var2idMap[name];
    return id;
  }

  VocabNode & var = makeNode(name, type, true, mode, val);
  var2idMap[name] = var.id;
  var.leftVars.push_back(var.id);
  var.rightVars.push_back(var.id);
  varIds.push_back(var.id);

  if (type == "array" || type == "barray"){
    var.size = size;
    string szname = name + ".size_minus_1";
    cout << "SPCHK: array declaration: declaring a size - 1 variable `" << name << ".size_minus_1' associated with the array."<< endl;
    var.arrSizeMinusOneId = handleDeclaration(szname,"int");
//    handleConstantDeclaration(1);
//    unsigned int idOne = getIdOfUsedConstant(1, SC_GRAMMAR);
//    var.arrSizeMinusOneId = handleTermArithTerm("-",
//        var.arrSizeId, idOne);
  }else if (type == "matrix" || type == "bmatrix"){
    var.cols = cols;
    var.rows = rows;
  }
  
  return var.id;
}


//MS
int
SCTheory::
handleRemoveDeclaration(const string & name)
{

  if ( var2idMap.find(name) == var2idMap.end()){ //didnt find the variable in the nodes

    cerr << "SPCHK: Variable " << name << "not found!" << endl;
    return -1;
  }
      //, const string & type,unsigned int mode,unsigned int size, unsigned int rows, unsigned int cols,int val)

   unsigned int id = var2idMap[name];
   VocabNode* var=nodes.at(id);


  if (var->type == "array" || var->type == "barray"){

      //remove the size created variable
      VocabNode* varSize=nodes.at(var->arrSizeMinusOneId);
      string szname = varSize->name;
      //string szname = name + ".size_minus_1";
      handleRemoveDeclaration(szname);
        }

  //remove grammar having this variable
/*
  rulemap_t::iterator mitr = rulemap.begin();
  for(; mitr!=rulemap.end(); mitr++) {
    const rulekey_t & key = mitr->first;
    if(key.first==id || key.second==id)
    {
        rulemap.erase(mitr);
    }
  }
*/


  //remove from varIds
  std::vector<unsigned int>::iterator it;
  it=std::find(varIds.begin(), varIds.end(),id);
  varIds.erase(it);

  //remove from map
  var2idMap.erase(var2idMap.find(name));

  //remove from nodes
  //nodes.erase(id);
  std::vector <VocabNode*>::iterator it_node;
  it_node=std::find(nodes.begin(), nodes.end(),var);
  nodes.erase(it_node);

  cout<<"removed variable "<<name<<" of node id "<<id<<endl;

  return id; //returns deleted node id
}



void 
SCTheory::print(ostream & os) {
  /*
  string name;
  vector <int> constantIds;
  vector <int> varIds;
  vector <VocabNode> nodes;
  map<string, int> var2idMap;
  rulemap_t rulemap;
  unsigned int numClauseOperationBound;
  unsigned int numQuantifierBound;
  */
  os << "theory " << name << "{" << endl;
  for(unsigned int i = 0; i < nodes.size(); i++) {
    VocabNode & vn = getNode(i);
    vn.print(os);
  }
  printRules(os);
  os << "  vocab {" << endl;
  printVocab(os,vocabIds);
  os << "  }" << endl;
  os << "  genvocab {" << endl;
  printVocab(os,generatedVocabIds);
  os << "  }" << endl;
  os << "  qfvocab { " << endl;
  printVocab(os,quantFreeVocIds);
  os << "  }" << endl;

  os<< "}" << endl;
}

void
SCTheory::printRules(ostream & os ) {
  rulemap_t::iterator mitr = rulemap.begin();
  for(; mitr!=rulemap.end(); mitr++) {
    const rulekey_t & key = mitr->first;
    set<string> & ops = mitr->second;
    os << "  ";
    printRule(os,key,ops);
  }
}

void
SCTheory::printRule(ostream & os, const rulekey_t & key, set<string> & ops) {
  os << "rule (" << getNode(key.first).name << "," << getNode(key.second).name ;
  set<string>::iterator oitr = ops.begin();
  for(;oitr!=ops.end();oitr++) {
    const string & op = *oitr;
    os << "," << op;
  }
  os << ")" << endl;
}

string
SCTheory::printRuleToTh(const rulekey_t & key, set<string> & ops) {
  string str = "";

  set<string>::iterator oitr = ops.begin();
  for(;oitr!=ops.end();oitr++) {
    const string & op = *oitr;
    //if(op.compare("") != 0)
    string a = "" + getNode(key.first).name;
    string b = "" + getNode(key.second).name;

    if((a.find("lit(") == 0 && a.find(")") == a.length()-1) || (b.find("lit(") == 0 && b.find(")") == b.length()-1))
    {
    	if(a.find("lit(") == 0 && a.find(")") == a.length()-1)
    	{
    		a = a.substr(4,a.length()-5);
    		str = str + "\t\t(" + b + "," + a ;
    		if(op == ">=")
    		{
    			str += ",<";
    		}
    		else if(op == "<=")
    		{
    			str += ",>";
    		}
    		else if(op == ">")
    		{
    			str += ",<=";
    		}
    		else if(op == "<")
    		{
    			str += ",>=";
    		}

    		//str += "," + op;
    		str += ");\n";
    	}
    	if(b.find("lit(") == 0 && b.find(")") == b.length()-1)
    	{
    		b = b.substr(4,b.length()-5);
    		str = str + "\t\t(" + a + "," + b ;
    		str += "," + op;
    		str += ");\n";
    	}
    }
    else{
    		str = str + "\t\t(" + a + "," + b ;
    		str += "," + op;
    		str += ");\n";
    	}
  }


  return str;
}

void 
SCTheory::handleConstantDeclaration(int value) {
  for (unsigned int i = 0; i < constantIds.size(); i++) {
    unsigned int id = constantIds[i];
    VocabNode & vn = getNode(id);
    if (vn.type == "literal") {
      if (vn.value == value) {
        cerr << "SPCHK: warning: value `" << value << "' already declared." << endl;
        return;
      }
    }
  }
  stringstream namestream; 
  namestream << "lit(" << value << ")";
  string name = namestream.str();

  VocabNode & litNode = makeNode(name, "literal", false, NODE_CONSTANT, value) ;

  litNode.leftVars.push_back(litNode.id);
  litNode.rightVars.push_back(litNode.id);

  constantIds.push_back(litNode.id);
}


//MS
void
SCTheory::handleRemoveConstantDeclaration(int value) {
  for (unsigned int i = 0; i < constantIds.size(); i++) {
    unsigned int id = constantIds[i];
    VocabNode* vn = nodes[id];
    if (vn->type == "literal") {
      if (vn->value == value) {


          //remove from constantIds
          std::vector<unsigned int>::iterator it;
          it=std::find(constantIds.begin(), constantIds.end(),id);
          constantIds.erase(it);

          //remove from map
          //var2idMap.erase(var2idMap.find(vn->name));

          //found, remove from nodes
          std::vector <VocabNode*>::iterator it_node;
          it_node=std::find(nodes.begin(), nodes.end(),vn);
          nodes.erase(it_node);

          cout<<"removed constant of value "<<value<<endl;


        return;
      }
    }
  }
  cout<<"Constant "<<value<<" not found!";
  return;

}



int 
SCTheory::getIdOfUsedVarible(const string & name, unsigned int where){
  if (var2idMap.find(name) == var2idMap.end()){
    cerr << "SPCHK: warning: variable `" << name << "' used in " << 
      whereTable[where] << " is not declared." << endl;
    return -1;
  }
  int id = var2idMap[name];
}

int 
SCTheory::getIdOfUsedConstant(int value, unsigned int where){
  int id = -1;
  unsigned int i = 0; 
  for (; i < constantIds.size(); i++) {
    id = constantIds[i];
    VocabNode & vn = getNode(id);
    if (vn.type == "literal") {
      if (vn.value == value) {
        break;
      }
    }
  }
  if (i == constantIds.size()) {
    cerr << "SPCHK: warning: constant `" << value << "' used in " << 
      whereTable[where] << " is not declared." << endl;
    return -1;
  }
  return id;
}

void 
SCTheory::propagateIsGlobal(VocabNode & node, unsigned int t1, unsigned int t2, bool unary){
  node.mode |= getNode(t1).isGlobal() ? 
    NODE_GLOBAL : 0;
  if (unary || t2 == -1) return;
  node.mode |= getNode(t2).isGlobal() ? 
    NODE_GLOBAL : 0;
}


int 
SCTheory::handleTermRelationalTerm(const string & op, unsigned int t1, unsigned int t2){
    VocabNode & node = makeNode(op, "binary-relational", false);
    if ( (op == "=" || op == "<=") && t1 == t2) {
      return handleTrue();
    }
    if (op == "=" && t2 < t1) {
      // if the node is = use the lower id first
        node.operandIds.push_back(t2);
        node.operandIds.push_back(t1);
    } else {
      node.operandIds.push_back(t1);
      node.operandIds.push_back(t2);
    }
    propagateIsGlobal(node, t1, t2);
    VocabNode & node1 = getNode(t1);
    VocabNode & node2 = getNode(t2);
    if (node1.isElement()){
      node2.setElement();
    } else if (node2.isElement()){
      node1.setElement();
    } else {
#if 0
      // too early to call
      // need to wait till the end of parsing.
      // resolved in propagateElementAttribute
      if (!node1.isGlobal()) {
        boundTerms.insert(t1);
      }
      if (!node2.isGlobal()) {
        boundTerms.insert(t2);
      }
#endif
    }
    return node.id;
}

int 
SCTheory::handleTermArithTerm(const string & op, unsigned int t1, unsigned int t2){
    VocabNode & node = makeNode(op, "binary-arithmetic", false);
    if ( (op == "+" || op == "*" ) && (t2 < t1)) {
      node.operandIds.push_back(t2);
      node.operandIds.push_back(t1);
    } else {
      node.operandIds.push_back(t1);
      node.operandIds.push_back(t2);
    }

    node.leftVars = getNode(t1).leftVars;
    node.rightVars = getNode(t2).rightVars;
    propagateIsGlobal(node, t1, t2);

    return node.id;
}

int 
SCTheory::handleBEBoolBE( const string & op, unsigned int e1, unsigned int e2){
    VocabNode & node = makeNode(op, "binary-boolean", false);
    node.operandIds.push_back(e1);
    node.operandIds.push_back(e2);
    propagateIsGlobal(node, e1, e2);
    
    return node.id;
}

int
SCTheory::handleNot(unsigned int be) {
    VocabNode & node = makeNode("not", "unary-boolean", false);
    node.operandIds.push_back(be);
    propagateIsGlobal(node, be);
    
    return node.id;
}

void
SCTheory::handleGrammarRule(int id1, int id2, set<string> & ops) {
  rulekey_t key(id1,id2);
  set<string>::iterator itr = ops.begin();

  for (;itr!=ops.end();itr++) {
    const string & op = *itr;
    if (op == "index") {
      set<string> & ops1 = rulemap[key];
      ops1.insert("[]");

      VocabNode & n1 = getNode(id1);
      if (n1.type != "array" && n1.type != "barray") {
        cerr << "SPCHK: error: variable `" << n1.name << "' used in an index rule, should be an array and is not." << endl;
        exit(-1);
      }

      unsigned int zero = getIdOfUsedConstant(0,SC_GRAMMAR);
      rulemap[rulekey_t(zero,id2)].insert("<=");
      rulemap[rulekey_t(id2,zero)].insert("=");

      rulemap[rulekey_t(id2,n1.arrSizeMinusOneId)].insert("<=");
      rulemap[rulekey_t(id2,n1.arrSizeMinusOneId)].insert("=");

      for(unsigned int i=0;i<n1.boundIds.size();i++) {
        unsigned int bid = n1.boundIds[i];
        rulemap[rulekey_t(id2,bid)].insert("<=");
        rulemap[rulekey_t(id2,bid)].insert("=");
        //TODO: we may also need (bid,id2)  insertion for completion
      }
      n1.boundIds.push_back(id2);
    } else if (op == "bound") {
      VocabNode & n1 = getNode(id1);
      if (n1.type != "array" && n1.type != "barray") {
        cerr << "SPCHK: error: variable `" << n1.name << "' used in a bound rule, should be an array and is not." << endl;
        exit(-1);
      }

      unsigned int zero = getIdOfUsedConstant(0,SC_GRAMMAR);
      rulemap[rulekey_t(zero,id2)].insert("<=");
      rulemap[rulekey_t(id2,zero)].insert("=");

      rulemap[rulekey_t(id2,n1.arrSizeMinusOneId)].insert("<=");
      rulemap[rulekey_t(id2,n1.arrSizeMinusOneId)].insert("=");

      for(unsigned int i=0;i<n1.boundIds.size();i++) {
        unsigned int bid = n1.boundIds[i];
        rulemap[rulekey_t(id2,bid)].insert("<=");
        rulemap[rulekey_t(id2,bid)].insert("=");
      }
      n1.boundIds.push_back(id2);
    } else if (op == "arithmetic") {
      set<string> & ops1 = rulemap[key];
      ops1.insert("+");
      ops1.insert("-");
      //ops1.insert("*");
      //ops1.insert("/");
      //ops1.insert("%");
    } else if (op == "relational") {
      set<string> & ops1 = rulemap[key];
      ops1.insert("<");
      ops1.insert(">");
      ops1.insert("=");
      ops1.insert("<=");
      ops1.insert(">=");
    } else {
      set<string> & ops1 = rulemap[key];
      ops1.insert(op);
    }
  }
  return;
}

//MS
void
SCTheory::handleRemoveGrammarRule(int id1, int id2,string rule) {
    //won't remove ops, since most probably used/will be used somewhere else



    stringstream ss;
    rulemap_t::iterator mitr = rulemap.begin();
    for(; mitr!=rulemap.end(); mitr++) {

      const rulekey_t key = mitr->first;
      set<string> ops= mitr->second;
      printRule(ss,key,ops);
      string grammarRule = ss.str();
      if(grammarRule==rule)
      {
          rulemap.erase(mitr);
      }
    }

    /*
  rulemap_t::iterator mitr = rulemap.begin();
  for(; mitr!=rulemap.end(); mitr++) {
    const rulekey_t & key = mitr->first;
    if(key.first==id1 || key.second==id2)
    {
        rulemap.erase(mitr);
    }
  }*/

  return;
}




int 
SCTheory::handleTrue(){
  if (trueId == -1) {
    VocabNode & node = makeNode("true", "bliteral", false, NODE_CONSTANT, 1) ;

    trueId = node.id;
  }
  return trueId;
}

int 
SCTheory::handleFalse(){
  if (falseId == -1) {
    VocabNode & node = makeNode("false", "bliteral", false, NODE_CONSTANT, 1) ;
    falseId = node.id;
  }
  return falseId;
}

int
SCTheory::handleForall(unsigned int varid, unsigned int beid) {
  VocabNode & node = makeNode("forall", "quantifier",false);

  //TODO: check if the quantified variables in beid to decide whether it still
  //      contains quantified variables!

  VocabNode & var = getNode(varid);
  if (!var.isGlobal()) {
    cout << "SPCHK: warning: variable '" << var.name << "' is bound to a quantifier"
      " and is not declared as a quantified variable." << endl;
    cout << "SPCHK: warning: variable '" << var.name << "' is set to quantified." << endl;
    var.mode |= NODE_GLOBAL;
  }
  node.operandIds.push_back(varid);
  node.operandIds.push_back(beid);
  
  return node.id;
}


int
SCTheory::handleExists(unsigned int varid, unsigned int beid) {
  VocabNode & node = makeNode("exists", "quantifier",false);

  VocabNode & var = getNode(varid);
  if (!var.isLocal()) {
    cout << "SPCHK: warning: variable '" << var.name << "' is bound to a quantifier"
      " and is not declared as a quantified variable." << endl;
    cout << "SPCHK: warning: variable '" << var.name << "' is set to quantified." << endl;
    var.mode |= NODE_LOCAL;
  }
  
  node.operandIds.push_back(varid);
  node.operandIds.push_back(beid);

  return node.id;
}

int 
SCTheory::handleArrayAccess(unsigned int arrayid, unsigned int indextermid){
  //  VocabNode & array = nodes[arrayid];
  //  string name = array.name + "[term]";
  VocabNode & var = getNode(arrayid);
  if (var.type != "array" && var.type!="barray" ) {
    cout << "SPCHK: warning: use of variable '" << var.name << "' of type '" << 
      var.type << "' where an `array' or a `barray' should be used." << endl;
  }
  string name = "[]";
  VocabNode & node = makeNode(name, "access",false);
  node.setElement();//sets the element flag
  node.operandIds.push_back(arrayid);
  node.operandIds.push_back(indextermid);

  node.leftVars.push_back(arrayid);
  node.rightVars.push_back(arrayid);
  propagateIsGlobal(node, indextermid);
  if (node.isGlobal() ) {
    VocabNode & node = getNode(indextermid);
    uterms.insert(indextermid);
    node.getUVars(*this);
  } else {
    readIndexTerms.insert(indextermid);
  }
  
  return node.id;
}

int 
SCTheory::handleUnaryMinus(unsigned int termid) {
  VocabNode & node = makeNode("-", "unary-arithmetic",false);
  node.operandIds.push_back(termid);
  propagateIsGlobal(node, termid);

  return node.id;
}

void 
SCTheory::printVocab(ostream & os,vector<unsigned int> & useVocIds) {
  for (unsigned int i = 0; i < useVocIds.size(); i++) {
    unsigned int clauseId = useVocIds[i];
    VocabNode & clause = getNode(clauseId);
    os << "    " << clauseId << ": ";
    clause.printForClause(os,*this);
    os << endl;
  }
}

void
SCTheory::addQuantifiedIndex(unsigned int idx){
  set<string> ops;
  ops.insert("index");
  for(unsigned int i =0; i < varIds.size(); i++) {
    unsigned int arrayid = varIds[i];
    VocabNode & node = getNode(arrayid);
    if (node.type == "array"){
      handleGrammarRule(arrayid, idx, ops);
    }
  }
  if (numClauseOperationBound>1) {
    set<string> ops1;
    ops1.insert("+");
    handleConstantDeclaration(1);
    unsigned int idOne = getIdOfUsedConstant(1, SC_INJECT);
    handleGrammarRule(idx,idOne,ops1);
  }
}

void 
SCTheory::injectQuantifierVars(bool injectLocal/*default: false*/){
  string name = "i";
  unsigned int num = 0;
  unsigned int i = 0;
  while(num < numQuantifierBound) {
    map<string,unsigned int>::iterator itr = var2idMap.find(name);

    //check if variable name is used
    //  if so, generate a new name
    if (itr != var2idMap.end()) {
      if (num == 0) {
        name = "q" + name;
      } else {
        stringstream ss;
        ss << name << i;
        name = ss.str();
        i++;
      }
      continue;
    }
    //declare the variable and introduce it as index
    //  into all arrays
    unsigned int mode = injectLocal? NODE_LOCAL : NODE_GLOBAL;
    unsigned int id = handleDeclaration(name,"int",mode);
    addQuantifiedIndex(id);
    num++;
  }
}

string 
SCTheory::buildQuantifiedSMTFormulae(vector<vocab_value_t> & choice, bool fromGenerated){
  vector<unsigned int > & useIds = useVocIds(fromGenerated);
  string smtQuantified;
  smtQuantified = "";

  stringstream ss1;
  ss1 << "(benchmark SMT: extrafuns( "; //header

  unsigned int i = 0;
  for(;i<smtDeclarations.size(); i++) {
    //TODO:  maybe skip quantified?
    VocabNode & node = getNode(varIds[i]);
    if (node.isGlobal()) {
    } else {
      ss1 << smtDeclarations[i] << " ";
    }
  }
  ss1 << ")";//end of extrafuns

  //might also need declarations
  ss1 << " :formula ";
  unsigned int n = printQuantifierPrefix(ss1,true);
  if (n == 0) {
    return "";
  }
  n = 0;
  for (unsigned int i = 0; i < useIds.size(); i++) {
    unsigned int id = useIds[i];
    VocabNode & node = getNode(id);

    if (!node.isGlobal()) {
      continue;
    }
    n++;

    stringstream ss;
    if(!smtQuantified.empty() ) {
      ss << "(and " << smtQuantified << " ";
    }
    if (choice[i] == vff) {
      ss << "(not ";
    }
    node.printSMT(ss,*this);
    if (choice[i] == vff) {
      ss << " )";
    }
    if(!smtQuantified.empty() ) {
      ss << " )";
    }
    smtQuantified = ss.str();
  }
  if (n== 0) {
    return "";
  }

  ss1 << smtQuantified;
  ss1 << ")";// for the quantifier prefix
  ss1 << ")";// for the benchmark 

  smtQuantified = ss1.str();
  return smtQuantified;
}



vector<string> & 
SCTheory::buildSMTFormulae(bool fromGenerated, bool useQuantFree){
  for(unsigned int i = 0; i < varIds.size(); i++) {
    unsigned int id = varIds[i];
    VocabNode & node = getNode(id);
    string smtdecl = "(" + node.name + " " + node.smtType() +")";
    smtDeclarations.push_back(smtdecl);
  }

  vector<unsigned int > & useIds = 
    useVocIds(fromGenerated, useQuantFree);
  vector<string> & smtFormulae = 
    useSMTVocab(fromGenerated, useQuantFree);

  for (unsigned int i = 0; i < useIds.size(); i++) {
    unsigned int id = useIds[i];
    VocabNode & node = getNode(id);
    stringstream ss;
    node.printSMT(ss,*this);
    string smtvoc = ss.str();
    cout << "SPCHK: formula " << i << ": " << smtvoc << "." << endl;
    smtFormulae.push_back(smtvoc);
  }
  return smtFormulae;
}

unsigned int 
SCTheory::decoratedQuantifierPrefix(string & forall, string & exists) {
  unsigned int an = 0;
  unsigned int en = 0;

  for(unsigned int i = 0; i < varIds.size(); i++) {
    unsigned int id = varIds[i];
    VocabNode & node = getNode(id);
    if (node.isGlobal()){
      if( an == 0) {
        forall = "forall ";
      } else {
        forall += ", ";
      }
      forall += node.name;
      an++;
    } else if (node.isLocal()){
      if( en == 0) {
        exists = "exists ";
      } else {
        exists += ", ";
      }
      exists += node.name;
      en++;
    }
  }
  if (an != 0) {
    forall += ". ";
  }
  if (en != 0) {
    exists += ". ";
  }
  return an+en;
}

unsigned int 
SCTheory::printQuantifierPrefix(ostream & os, bool isSmt) {
  unsigned int n = 0;
  if (isSmt) {
    os << "(forall (";
  } else {
    os << "forall ";
  }

  for(unsigned int i = 0; i < varIds.size(); i++) {
    unsigned int id = varIds[i];
    VocabNode & node = getNode(id);
    if (!node.isGlobal()){
      continue;
    }
    n++;

    if(isSmt) {
      os << "(" << node.name << " " ;
      if (node.type == "int") {
        os << "Int";
      } else if (node.type == "array") {
        os << "(Array Int Int)";
      } else if (node.type == "bool") {
        os << "Bool";
      } else if (node.type == "barray") {
        os << "(Array Int Bool)";
      }
      os << ")";
    } else {
      os << " " << node.name  ;
    }
  }
  if (isSmt) {
    os << ") ";// close the quantifier vars and leave space for the formula
  } else {
    os << ". ";
  }
  return n;
}


VocabNode & 
SCTheory::getVocabNode(unsigned int idx, bool genVocab, bool useQuantFree){
  unsigned int id=0;
  vector<unsigned int> & useIds = useVocIds(genVocab,useQuantFree);
  id = useIds[idx];
  VocabNode & node = getNode(id);
  return node;
}
