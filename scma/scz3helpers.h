#ifndef _SC_Z3_HELPERS_
#define _SC_Z3_HELPERS_

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

void exitf(const char* message);
void unreachable() ;

void error_handler(Z3_context ctx, Z3_error_code e) ;

extern jmp_buf g_catch_buffer;

void throw_z3_error(Z3_error_code c) ;

Z3_context mk_context_custom(Z3_config cfg, Z3_error_handler err) ;

Z3_context mk_context() ;

Z3_ast mk_var(Z3_context ctx, const char * name, Z3_sort ty) ;

Z3_ast mk_bool_var(Z3_context ctx, const char * name) ;

Z3_ast mk_unary_app(Z3_context ctx, Z3_func_decl f, Z3_ast x) ;

Z3_ast mk_binary_app(Z3_context ctx, Z3_func_decl f, Z3_ast x, Z3_ast y) ;

void display_symbol(Z3_context c, FILE * out, Z3_symbol s) ;

void display_sort(Z3_context c, FILE * out, Z3_sort ty) ;

void display_ast(Z3_context c, FILE * out, Z3_ast v) ;

void display_function_interpretations(Z3_context c, FILE * out, Z3_model m) ;

void display_model(Z3_context c, FILE * out, Z3_model m) ;

void display_version() ;
Z3_ext_context mk_ext_context() ;

void del_ext_context(Z3_ext_context ctx) ;

unsigned assert_retractable_cnstr(Z3_ext_context ctx, Z3_ast c) ;

void retract_cnstr(Z3_ext_context ctx, unsigned id) ;

void reassert_cnstr(Z3_ext_context ctx, unsigned id) ;

Z3_lbool ext_check(Z3_ext_context ctx) ;

#endif
