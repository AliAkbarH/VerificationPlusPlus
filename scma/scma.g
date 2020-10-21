/** \file   scma.g
*   \brief  grammar for the specification construction input
*   \author fadi zaraket 
*   \date   march 26, 2013
*/
grammar scma;

options 
{
    backtrack	= true;
    memoize     = true;
    k           = 3;
    language	= C;
}

@lexer::header
{
#define	ANTLR3_INLINE_INPUT_ASCII
}

@parser::includes
{
  #include <scTheory.h>

  //needed to allow the return variable initialization to 
  //null by the antlr generated code
  class scstring : public string { public:
    scstring():string(){}
    scstring(const char * s) : string(s) {}
    scstring(const string & s) : string(s) {}
    scstring(void * v ) : string() {}
    scstring(long int v ) : string() {}
    scstring & operator = (void * v) {
      *this = "";
      return *this;
    }
  };
}

// Use @preincludes for things you want to appear in the output file
//     before #include <antlr3.h>
//     @includes to come after #include <antlr3.h>
//     @header for things that should follow on after all the includes.
@members 
{
  SCTheory * theory=0;
  int success = 1;
}

//translation_unit returns [PDTCodeGraph * _cg, Name2Node_t _name2node]
translation_unit returns [SCTheory * th]
@init{
  th = 0;
  theory = new SCTheory;
  success = 1;
}
@after{
  cout<<"Parsing Complete"<<endl;
  if (success == 1) {
    th = theory;
  } else {
    delete theory;
  }
}
    : 
'theory' (i=IDENTIFIER
        {
        theory->name = string((const char*)$i.text->chars);
        cout << "theory  is " << theory->name << endl;
        }
) '{' (d=declaration
        {

        }
)+  
( 'constants' '{' c1=constant 
        {
          theory->handleConstantDeclaration($c1.value);
        }
(',' c2=constant
       {
          theory->handleConstantDeclaration($c2.value);
        }
 )* '}' )?
( 'grammar' '{' (r=grammar_rule
        {
        }
)+ '}')?
( 'num_operations_per_clause_bound' '=' c=constant 
        {
          theory->numClauseOperationBound = $c.value;
        }
';')?
( 'num_quantifiers_bound' '=' c=constant 
        {
          theory->numQuantifierBound = $c.value;
        }
';')?

( 'vocab' '{' (v=clause
        {
        }
)+ '}')?
'}'
        {

        }
;


declaration returns [unsigned int varid ]
@init {
  int value = 0;
}
    : 
e=declaration_specifier j=declarator ('=' d=constant 
    {
			value = $d.value;
	 	} 
)? {
        // search for variable in map, if found return error
        // if not then create a new with the appropriate types 
        string name = ($j.varname);
        varid = theory->handleDeclaration(name, $e.type, 
            $e.mode, 
            $e.size, $e.rows, $e.cols,value);
    }
';'
    ;

declaration_specifier returns [string type, unsigned int mode, int size, int rows, int cols]
@init{
  $mode = 0;
}
    :   
(m=type_modifier 
 {
 $mode = $m.mode;
 }
)* (e=type_specifier 
        {
          $type = $e.type;
          $size = $e.size;
          $rows = $e.rows;
          $cols = $e.cols;
        }
  )
;

type_modifier returns [unsigned int mode]
@init{
  $mode = 0;
}
: 
'const' { $mode |= NODE_CONSTANT;} | 
'universal' { $mode |= NODE_GLOBAL;} |
'global' { $mode |= NODE_GLOBAL;} |
'existential' { $mode |= NODE_LOCAL;} | 
'local' { $mode |= NODE_LOCAL;}
;

/*
 declarator returns string pointer with variable name
 */
declarator returns [scstring varname]
    : e=direct_declarator {$varname = $e.varname;}
    ;

/*type spoecifier rule*/
type_specifier returns [scstring type, int size, int rows, int cols] 
// return type enum and array size if applicable
@init {
  $rows = $cols = $size = -1;
}
@after {
	if ($size == -1) {
		$size = 0;
	}
  if ($rows == -1) {
    $rows = 0;
  }
}
    :
'int' // integer type 
        {
            $type = "int"; //  return integer type
        }
| 'int' '[]' // array of integers
        {
            $type = "array"; // return integer array type 
        }
| 'int' '[' s=constant {$size = $s.value;} ']' // array of integers
        {
            $type = "array"; // return integer array type 
        }
| 'boolean' // boolean type
        {
            $type = "bool"; // return boolean type
        }
| 'boolean' '[' (s=constant {$size = $s.value;})? ']' // array of booleans
        {
            $type = "barray"; // return boolean array type
        }
| 'int'  '[' s=constant ']' '[' q=constant ']' // double array of integers 
		   {
         $type = "matrix";
         $rows = $s.value;
         $cols = $q.value;
       }
| 'boolean' '[' s=constant ']' '[' q=constant ']' // double array of booleans
		   {
         $type = "bmatrix"; // return boolean matrix type
         $rows = $s.value;
         $cols = $q.value;
       }
;


/*
 direct declarator return the variable name
 */
direct_declarator returns [scstring varname ]
    :   e=IDENTIFIER 
     {
       varname = string((const char*)$e.text->chars);
     }//declarator_suffix?
    ;

	
constant returns [ int value]
@init{
  bool sign = false;
}
    : 
( '-'
        {
        sign = true;
        }
)?
e=DECIMAL_LITERAL 
        {
             $value = atoi((const char*)$e.text->chars);
             if (sign) {
               $value = -$value;
             }
        }
    ;

grammar_rule //returns [int rulescstring v1, scstring v2, vector<string> ops ]
@init {
  int n1=-1;
  int n2=-1;
  set<string> ops;
}
: 
'(' i1=IDENTIFIER 
      {
      string name((const char*)$i1.text->chars);
      n1 = theory->getIdOfUsedVarible(name, SCTheory::SC_GRAMMAR);
      }

',' (i2=IDENTIFIER 
      {
      string name((const char*)$i2.text->chars);
      n2 = theory->getIdOfUsedVarible(name, SCTheory::SC_GRAMMAR);
      }
| c=constant
      {
      n2 = theory->getIdOfUsedConstant($c.value, SCTheory::SC_GRAMMAR);
      }
)
',' o=operation
      {
        string op((const char*)$o.text->chars);
        ops.insert(op);
      }
(',' o2=operation 
      {
        string op((const char*)$o2.text->chars);
        ops.insert(op);
      }
)*
')'
      {
        if (n1 == -1 || n2 == -1 ){
          cerr << "SPCHK: warning: ignoring grammar rule." << endl;
        } else {
          theory->handleGrammarRule(n1, n2, ops);
        }
      }
';'
;

clause returns [int nodeid] : 
  be=boolean_expression 
  {
    nodeid = $be.nodeid;
    theory->vocabIds.push_back(nodeid);
  }
  ';'
  |
  '(' be=boolean_expression ')'
  {
    nodeid = $be.nodeid;
    theory->vocabIds.push_back(nodeid);
  }
  ';'
  ;

boolean_expression returns [int nodeid] 
@init{
  string op;
}
: 'true' 
         {
           nodeid = theory->handleTrue();
         }
| 
'false' 
         {
           nodeid = theory->handleFalse();
         }
|
i=IDENTIFIER 
         {
           string name((const char*)$i.text->chars);
           nodeid = theory->getIdOfUsedVarible(name, SCTheory::SC_VOCAB);
           assert(theory->getNode(nodeid).type == "bool");
         }
|
'(' t1=term (o=RELATIONAL_OP 
         {
           op = string((const char*)$o.text->chars);
         }
      | '='
         {
           op = "=";
         }
      ) 
t2=term ')'
         {
           nodeid = theory->handleTermRelationalTerm(
               op, $t1.nodeid, $t2.nodeid);
         }
|
'(' e1=boolean_expression o=BOOLEAN_OP e2=boolean_expression ')'
         {
           string op((const char*)$o.text->chars);
           nodeid = theory->handleBEBoolBE(
               op, $e1.nodeid, $e2.nodeid);
         }
| 
'(' ('!'|'not') e=boolean_expression ')'
         {
           nodeid = theory->handleNot($e.nodeid);
         }
| 
'(' 'forall' i=IDENTIFIER '.' e=boolean_expression ')'
         {
           string name((const char*)$i.text->chars);
           int varid = theory->getIdOfUsedVarible(name, SCTheory::SC_VOCAB);
           nodeid = theory->handleForall(
               varid, $e.nodeid);
         }
|
'(' 'exists' i=IDENTIFIER '.' e=boolean_expression ')'
         {
           string name((const char*)$i.text->chars);
           int varid = theory->getIdOfUsedVarible(name, SCTheory::SC_VOCAB);
           nodeid = theory->handleExists(
               varid, $e.nodeid);
         }
;

term returns [int nodeid] : 
a=additionop 
{
  nodeid = $a.nodeid;
}
;

// additionop
additionop returns [int nodeid]
@init{
  int lastnodeid;
}
:
t1=productop 
        {
          lastnodeid = $t1.nodeid;
        }
(o=ADDITION_OP t2=productop
        {
           string op((const char*)$o.text->chars);
           lastnodeid = theory->handleTermArithTerm(op, lastnodeid, $t2.nodeid);
        }
)?
        {
          nodeid = lastnodeid;
        }
;


// productop
productop returns [int nodeid]
@init{
  int lastnodeid;
}
:
t1=unaryop 
        {
          lastnodeid = $t1.nodeid;
        }
(o=PRODUCT_OP t2=unaryop 
        {
           string op((const char*)$o.text->chars);
           lastnodeid = theory->handleTermArithTerm(op, lastnodeid, $t2.nodeid);
        }
)*
        {
          nodeid = lastnodeid;
        }
;

// unary op
unaryop returns [int nodeid] :
'-' a=atom
        {
          nodeid = theory->handleUnaryMinus($a.nodeid);
        }
| a=atom 
        {
          nodeid = $a.nodeid;
        }
;

//atome
atom returns [int nodeid] :
'(' t=term ')' 
        {
          nodeid = $t.nodeid;
        }
|
d=DECIMAL_LITERAL
        {
          int val = atoi((const char*)$d.text->chars);
          theory->handleConstantDeclaration(val);
          nodeid = theory->getIdOfUsedConstant(val, SCTheory::SC_VOCAB);
        }
|
i=IDENTIFIER 
        {
           string name((const char*)$i.text->chars);
           nodeid = theory->getIdOfUsedVarible(name, SCTheory::SC_VOCAB);
           VocabNode & var = theory->getNode(nodeid);
           if (var.type != "int") {
             cout << "SPCHK: warning: used variable '" << var.name << "' of type '" << 
               var.type << "' where an `int' should be used." << endl;
           }
        }
|
i=IDENTIFIER '[' t=term ']'
        {
           string name((const char*)$i.text->chars);
           unsigned int arrayid = theory->getIdOfUsedVarible(name, SCTheory::SC_VOCAB);
           VocabNode & var = theory->getNode(arrayid);
           if (! (var.type == "array" || var.type =="barray" )) {
             cout << "SPCHK: warning: used variable '" << var.name << "' of type '" << 
               var.type << "' where an `array' or a `barray' should be used." << endl;
           }
           nodeid = theory->handleArrayAccess(arrayid, $t.nodeid);
        }
;

operation: 'bound' | 'index' | 'arithmetic' | 'relational' | //group of ops
  RELATIONAL_OP | '=' |
  ADDITION_OP | PRODUCT_OP |
   '[]' // indexing operation
   ;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// end parser rules

// L E X E R  R U L E S

RELATIONAL_OP: '<' | '>' | '<=' | '>=' | '=' ;

//ARITHMETIC_OP: ADDITION_OP | PRODUCT_OP;

ADDITION_OP: '+' | '-';

PRODUCT_OP: '*' | '/' | '%';

BOOLEAN_OP:  'and' | 'or' | 'implies' | 'equiv' | 'xor' | 'xnor' | 
  '->' | '&&' | '||'; 


STRING: '"' .* '"' ;

IDENTIFIER
    :	LETTER (LETTER|'!'|'.'|'0'..'9')*
    ;
	
fragment
LETTER
    :	'$'
    |	'A'..'Z'
    |	'a'..'z'
    |	'_'
    ;

DECIMAL_LITERAL : ('0' | '1'..'9' '0'..'9'*);

WS  :  (' '|'\r'|'\t'|'\u000C'|'\n') {$channel=HIDDEN;}
    ;

COMMENT
    : '/*' ( options {greedy = false;} : .)* '*/' {$channel=HIDDEN;}
    ;

LINE_COMMENT
    : '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    ;


/////////////////////////////////////////////////////////
// end MINI.g
