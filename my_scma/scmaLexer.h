/** \file
 *  This C header file was generated by $ANTLR version 3.2 Sep 23, 2009 12:02:23
 *
 *     -  From the grammar source file : scma.g
 *     -                            On : 2020-04-29 01:52:50
 *     -                 for the lexer : scmaLexerLexer *
 * Editing it, at least manually, is not wise. 
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 *
 * The lexer scmaLexer has the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 * 
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 * The parser entry points are called indirectly (by function pointer to function) via
 * a parser context typedef pscmaLexer, which is returned from a call to scmaLexerNew().
 *
 * As this is a generated lexer, it is unlikely you will call it 'manually'. However
 * the methods are provided anyway.
 * * The methods in pscmaLexer are  as follows:
 *
 *  -  void      pscmaLexer->T__15(pscmaLexer)
 *  -  void      pscmaLexer->T__16(pscmaLexer)
 *  -  void      pscmaLexer->T__17(pscmaLexer)
 *  -  void      pscmaLexer->T__18(pscmaLexer)
 *  -  void      pscmaLexer->T__19(pscmaLexer)
 *  -  void      pscmaLexer->T__20(pscmaLexer)
 *  -  void      pscmaLexer->T__21(pscmaLexer)
 *  -  void      pscmaLexer->T__22(pscmaLexer)
 *  -  void      pscmaLexer->T__23(pscmaLexer)
 *  -  void      pscmaLexer->T__24(pscmaLexer)
 *  -  void      pscmaLexer->T__25(pscmaLexer)
 *  -  void      pscmaLexer->T__26(pscmaLexer)
 *  -  void      pscmaLexer->T__27(pscmaLexer)
 *  -  void      pscmaLexer->T__28(pscmaLexer)
 *  -  void      pscmaLexer->T__29(pscmaLexer)
 *  -  void      pscmaLexer->T__30(pscmaLexer)
 *  -  void      pscmaLexer->T__31(pscmaLexer)
 *  -  void      pscmaLexer->T__32(pscmaLexer)
 *  -  void      pscmaLexer->T__33(pscmaLexer)
 *  -  void      pscmaLexer->T__34(pscmaLexer)
 *  -  void      pscmaLexer->T__35(pscmaLexer)
 *  -  void      pscmaLexer->T__36(pscmaLexer)
 *  -  void      pscmaLexer->T__37(pscmaLexer)
 *  -  void      pscmaLexer->T__38(pscmaLexer)
 *  -  void      pscmaLexer->T__39(pscmaLexer)
 *  -  void      pscmaLexer->T__40(pscmaLexer)
 *  -  void      pscmaLexer->T__41(pscmaLexer)
 *  -  void      pscmaLexer->T__42(pscmaLexer)
 *  -  void      pscmaLexer->T__43(pscmaLexer)
 *  -  void      pscmaLexer->T__44(pscmaLexer)
 *  -  void      pscmaLexer->T__45(pscmaLexer)
 *  -  void      pscmaLexer->T__46(pscmaLexer)
 *  -  void      pscmaLexer->T__47(pscmaLexer)
 *  -  void      pscmaLexer->T__48(pscmaLexer)
 *  -  void      pscmaLexer->T__49(pscmaLexer)
 *  -  void      pscmaLexer->RELATIONAL_OP(pscmaLexer)
 *  -  void      pscmaLexer->ADDITION_OP(pscmaLexer)
 *  -  void      pscmaLexer->PRODUCT_OP(pscmaLexer)
 *  -  void      pscmaLexer->BOOLEAN_OP(pscmaLexer)
 *  -  void      pscmaLexer->STRING(pscmaLexer)
 *  -  void      pscmaLexer->IDENTIFIER(pscmaLexer)
 *  -  void      pscmaLexer->LETTER(pscmaLexer)
 *  -  void      pscmaLexer->DECIMAL_LITERAL(pscmaLexer)
 *  -  void      pscmaLexer->WS(pscmaLexer)
 *  -  void      pscmaLexer->COMMENT(pscmaLexer)
 *  -  void      pscmaLexer->LINE_COMMENT(pscmaLexer)
 *  -  void      pscmaLexer->Tokens(pscmaLexer)
 *
 * The return type for any particular rule is of course determined by the source
 * grammar file.
 */
// [The "BSD licence"]
// Copyright (c) 2005-2009 Jim Idle, Temporal Wave LLC
// http://www.temporal-wave.com
// http://www.linkedin.com/in/jimidle
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	_scmaLexer_H
#define _scmaLexer_H
/* =============================================================================
 * Standard antlr3 C runtime definitions
 */
#include    <antlr3.h>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */
 
#ifdef __cplusplus
extern "C" {
#endif

// Forward declare the context typedef so that we can use it before it is
// properly defined. Delegators and delegates (from import statements) are
// interdependent and their context structures contain pointers to each other
// C only allows such things to be declared if you pre-declare the typedef.
//
typedef struct scmaLexer_Ctx_struct scmaLexer, * pscmaLexer;



#define	ANTLR3_INLINE_INPUT_ASCII


#ifdef	ANTLR3_WINDOWS
// Disable: Unreferenced parameter,							- Rules with parameters that are not used
//          constant conditional,							- ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable					- tree rewrite variables declared but not needed
//          Unreferenced local variable						- lexer rule declares but does not always use _type
//          potentially unitialized variable used			- retval always returned from a rule 
//			unreferenced local function has been removed	- susually getTokenNames or freeScope, they can go without warnigns
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4701 )
#endif

/** Context tracking structure for scmaLexer
 */
struct scmaLexer_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_LEXER    pLexer;


     void (*mT__15)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__16)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__17)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__18)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__19)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__20)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__21)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__22)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__23)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__24)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__25)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__26)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__27)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__28)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__29)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__30)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__31)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__32)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__33)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__34)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__35)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__36)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__37)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__38)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__39)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__40)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__41)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__42)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__43)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__44)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__45)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__46)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__47)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__48)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mT__49)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mRELATIONAL_OP)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mADDITION_OP)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mPRODUCT_OP)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mBOOLEAN_OP)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mSTRING)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mIDENTIFIER)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mLETTER)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mDECIMAL_LITERAL)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mWS)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mCOMMENT)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mLINE_COMMENT)	(struct scmaLexer_Ctx_struct * ctx);
     void (*mTokens)	(struct scmaLexer_Ctx_struct * ctx);    const char * (*getGrammarFileName)();
    void	    (*free)   (struct scmaLexer_Ctx_struct * ctx);
        
};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//
ANTLR3_API pscmaLexer scmaLexerNew         (pANTLR3_INPUT_STREAM instream);
ANTLR3_API pscmaLexer scmaLexerNewSSD      (pANTLR3_INPUT_STREAM instream, pANTLR3_RECOGNIZER_SHARED_STATE state);

/** Symbolic definitions of all the tokens that the lexer will work with.
 * \{
 *
 * Antlr will define EOF, but we can't use that as it it is too common in
 * in C header files and that would be confusing. There is no way to filter this out at the moment
 * so we just undef it here for now. That isn't the value we get back from C recognizers
 * anyway. We are looking for ANTLR3_TOKEN_EOF.
 */
#ifdef	EOF
#undef	EOF
#endif
#ifdef	Tokens
#undef	Tokens
#endif 
#define PRODUCT_OP      9
#define T__29      29
#define T__28      28
#define T__27      27
#define T__26      26
#define T__25      25
#define T__24      24
#define T__23      23
#define LETTER      11
#define T__22      22
#define T__21      21
#define T__20      20
#define EOF      -1
#define T__19      19
#define T__16      16
#define T__15      15
#define ADDITION_OP      8
#define T__18      18
#define T__17      17
#define IDENTIFIER      4
#define COMMENT      13
#define T__42      42
#define T__43      43
#define T__40      40
#define T__41      41
#define T__46      46
#define T__47      47
#define T__44      44
#define T__45      45
#define LINE_COMMENT      14
#define T__48      48
#define T__49      49
#define RELATIONAL_OP      6
#define BOOLEAN_OP      7
#define T__30      30
#define T__31      31
#define T__32      32
#define WS      12
#define T__33      33
#define T__34      34
#define T__35      35
#define T__36      36
#define T__37      37
#define T__38      38
#define T__39      39
#define DECIMAL_LITERAL      5
#define STRING      10
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

#ifndef TOKENSOURCE
#define TOKENSOURCE(lxr) lxr->pLexer->rec->state->tokSource
#endif

/* End of token definitions for scmaLexer
 * =============================================================================
 */
/** \} */

#ifdef __cplusplus
}
#endif

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
