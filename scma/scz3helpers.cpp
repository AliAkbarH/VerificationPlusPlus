#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<memory.h>
#include<setjmp.h>

#include <scz3helpers.h>



#include <assert.h>


/** 
   \defgroup capi_ex C API examples
*/
/*@{*/
/**
   @name Auxiliary Functions
*/
/*@{*/

/**
   \brief exit gracefully in case of error.
*/
void exitf(const char* message) 
{
  fprintf(stderr,"BUG: %s.\n", message);
  exit(1);
}

/**
   \brief exit if unreachable code was reached.
*/
void unreachable() 
{
    exitf("unreachable code was reached");
}

/**
   \brief Simpler error handler.
 */
void error_handler(Z3_context ctx, Z3_error_code e) 
{
	printf("Error code: %d\n", e);
    exitf("incorrect use of Z3");
}

jmp_buf g_catch_buffer;
/**
   \brief Low tech exceptions. 
   
   In high-level programming languages, an error handler can throw an exception.
*/
void throw_z3_error(Z3_error_code c) 
{
    longjmp(g_catch_buffer, c);
}

/**
   \brief Create a logical context.  

   Enable model construction. Other configuration parameters can be passed in the cfg variable.

   Also enable tracing to stderr and register custom error handler.
*/
Z3_context mk_context_custom(Z3_config cfg, Z3_error_handler err) 
{
    Z3_context ctx;
    
    Z3_set_param_value(cfg, "MODEL", "true");
    ctx = Z3_mk_context(cfg);
#ifdef TRACING
    Z3_trace_to_stderr(ctx);
#endif
    Z3_set_error_handler(ctx, err);
    
    return ctx;
}

/**
   \brief Create a logical context.

   Enable model construction only.

   Also enable tracing to stderr and register standard error handler.
*/
Z3_context mk_context() 
{
    Z3_config  cfg;
    Z3_context ctx;
    cfg = Z3_mk_config();
    ctx = mk_context_custom(cfg, error_handler);
    Z3_del_config(cfg);
    return ctx;
}



/**
   \brief Create a variable using the given name and type.
*/
Z3_ast mk_var(Z3_context ctx, const char * name, Z3_sort ty) 
{
    Z3_symbol   s  = Z3_mk_string_symbol(ctx, name);
    return Z3_mk_const(ctx, s, ty);
}

/**
   \brief Create a boolean variable using the given name.
*/
Z3_ast mk_bool_var(Z3_context ctx, const char * name) 
{
    Z3_sort ty = Z3_mk_bool_sort(ctx);
    return mk_var(ctx, name, ty);
}


/**
   \brief Create the unary function application: <tt>(f x)</tt>.
*/
Z3_ast mk_unary_app(Z3_context ctx, Z3_func_decl f, Z3_ast x) 
{
    Z3_ast args[1] = {x};
    return Z3_mk_app(ctx, f, 1, args);
}

/**
   \brief Create the binary function application: <tt>(f x y)</tt>.
*/
Z3_ast mk_binary_app(Z3_context ctx, Z3_func_decl f, Z3_ast x, Z3_ast y) 
{
    Z3_ast args[2] = {x, y};
    return Z3_mk_app(ctx, f, 2, args);
}

/**
   \brief Display a symbol in the given output stream.
*/
void display_symbol(Z3_context c, FILE * out, Z3_symbol s) 
{
    switch (Z3_get_symbol_kind(c, s)) {
    case Z3_INT_SYMBOL:
        fprintf(out, "#%d", Z3_get_symbol_int(c, s));
        break;
    case Z3_STRING_SYMBOL:
        fprintf(out, "%s", Z3_get_symbol_string(c, s));
        break;
    default:
        unreachable();
    }
}

/**
   \brief Display the given type.
*/
void display_sort(Z3_context c, FILE * out, Z3_sort ty) 
{
    switch (Z3_get_sort_kind(c, ty)) {
    case Z3_UNINTERPRETED_SORT:
        display_symbol(c, out, Z3_get_sort_name(c, ty));
        break;
    case Z3_BOOL_SORT:
        fprintf(out, "bool");
        break;
    case Z3_INT_SORT:
        fprintf(out, "int");
        break;
    case Z3_REAL_SORT:
        fprintf(out, "real");
        break;
    case Z3_BV_SORT:
        fprintf(out, "bv%d", Z3_get_bv_sort_size(c, ty));
        break;
    case Z3_ARRAY_SORT: 
        fprintf(out, "[");
        display_sort(c, out, Z3_get_array_sort_domain(c, ty));
        fprintf(out, "->");
        display_sort(c, out, Z3_get_array_sort_range(c, ty));
        fprintf(out, "]");
        break;
    case Z3_DATATYPE_SORT:
		if (Z3_get_datatype_sort_num_constructors(c, ty) != 1) 
		{
			fprintf(out, "%s", Z3_sort_to_string(c,ty));
			break;
		}
		{
        unsigned num_fields = Z3_get_tuple_sort_num_fields(c, ty);
        unsigned i;
        fprintf(out, "(");
        for (i = 0; i < num_fields; i++) {
            Z3_func_decl field = Z3_get_tuple_sort_field_decl(c, ty, i);
            if (i > 0) {
                fprintf(out, ", ");
            }
            display_sort(c, out, Z3_get_range(c, field));
        }
        fprintf(out, ")");
        break;
    }
    default:
        fprintf(out, "unknown[");
        display_symbol(c, out, Z3_get_sort_name(c, ty));
        fprintf(out, "]");
        break;
    }
}

/**
   \brief Custom ast pretty printer. 

   This function demonstrates how to use the API to navigate terms.
*/
void display_ast(Z3_context c, FILE * out, Z3_ast v) 
{
    switch (Z3_get_ast_kind(c, v)) {
    case Z3_NUMERAL_AST: {
        Z3_sort t;
        fprintf(out, "%s", Z3_get_numeral_string(c, v));
        t = Z3_get_sort(c, v);
        fprintf(out, ":");
        display_sort(c, out, t);
        break;
    }
    case Z3_APP_AST: {
        unsigned i;
        Z3_app app = Z3_to_app(c, v);
        unsigned num_fields = Z3_get_app_num_args(c, app);
        Z3_func_decl d = Z3_get_app_decl(c, app);
        fprintf(out, "%s", Z3_func_decl_to_string(c, d));
        if (num_fields > 0) {
            fprintf(out, "[");
            for (i = 0; i < num_fields; i++) {
                if (i > 0) {
                    fprintf(out, ", ");
                }
                display_ast(c, out, Z3_get_app_arg(c, app, i));
            }
            fprintf(out, "]");
        }
        break;
    }
    case Z3_QUANTIFIER_AST: {
        fprintf(out, "quantifier");
        ;	
    }
    default:
        fprintf(out, "#unknown");
    }
}

/**
   \brief Custom function interpretations pretty printer.
*/
void display_function_interpretations(Z3_context c, FILE * out, Z3_model m) 
{
    unsigned num_functions, i;

    fprintf(out, "function interpretations:\n");

    num_functions = Z3_get_model_num_funcs(c, m);
    for (i = 0; i < num_functions; i++) {
        Z3_func_decl fdecl;
        Z3_symbol name;
        Z3_ast func_else;
        unsigned num_entries, j;
        
        fdecl = Z3_get_model_func_decl(c, m, i);
        name = Z3_get_decl_name(c, fdecl);
        display_symbol(c, out, name);
        fprintf(out, " = {");
        num_entries = Z3_get_model_func_num_entries(c, m, i);
        for (j = 0; j < num_entries; j++) {
            unsigned num_args, k;
            if (j > 0) {
                fprintf(out, ", ");
            }
            num_args = Z3_get_model_func_entry_num_args(c, m, i, j);
            fprintf(out, "(");
            for (k = 0; k < num_args; k++) {
                if (k > 0) {
                    fprintf(out, ", ");
                }
                display_ast(c, out, Z3_get_model_func_entry_arg(c, m, i, j, k));
            }
            fprintf(out, "|->");
            display_ast(c, out, Z3_get_model_func_entry_value(c, m, i, j));
            fprintf(out, ")");
        }
        if (num_entries > 0) {
            fprintf(out, ", ");
        }
        fprintf(out, "(else|->");
        func_else = Z3_get_model_func_else(c, m, i);
        display_ast(c, out, func_else);
        fprintf(out, ")}\n");
    }
}

/**
   \brief Custom model pretty printer.
*/
void display_model(Z3_context c, FILE * out, Z3_model m) 
{
    unsigned num_constants;
    unsigned i;

    num_constants = Z3_get_model_num_constants(c, m);
    for (i = 0; i < num_constants; i++) {
        Z3_symbol name;
        Z3_func_decl cnst = Z3_get_model_constant(c, m, i);
        Z3_ast a, v;
        Z3_bool ok;
        name = Z3_get_decl_name(c, cnst);
        display_symbol(c, out, name);
        fprintf(out, " = ");
        a = Z3_mk_app(c, cnst, 0, 0);
        v = a;
        ok = Z3_eval(c, m, a, &v);
        display_ast(c, out, v);
        fprintf(out, "\n");
    }
    display_function_interpretations(c, out, m);
}



/**
   \brief Display Z3 version in the standard output.
*/
void display_version() 
{
    unsigned major, minor, build, revision;
    Z3_get_version(&major, &minor, &build, &revision);
    printf("Z3 %d.%d.%d.%d\n", major, minor, build, revision);
}
/*@}*/

/**
   \brief Create a logical context wrapper with support for retractable constraints.
 */
Z3_ext_context mk_ext_context() {
    Z3_ext_context ctx         = (Z3_ext_context) malloc(sizeof(Z3_ext_context_struct));
    ctx->m_context             = mk_context();
    ctx->m_num_answer_literals = 0;
    return ctx;
}

/**
   \brief Delete the given logical context wrapper.
*/
void del_ext_context(Z3_ext_context ctx) {
    Z3_del_context(ctx->m_context);
    free(ctx);
}

/**
   \brief Create a retractable constraint.
   
   \return An id that can be used to retract/reassert the constraint.
*/
unsigned assert_retractable_cnstr(Z3_ext_context ctx, Z3_ast c) {
    unsigned result;
    Z3_sort ty;
    Z3_ast ans_lit;
    Z3_ast args[2];
    if (ctx->m_num_answer_literals == MAX_RETRACTABLE_ASSERTIONS) {
        exitf("maximum number of retractable constraints was exceeded.");
    }
    ty      = Z3_mk_bool_sort(ctx->m_context);
    ans_lit = Z3_mk_fresh_const(ctx->m_context, "k", ty);
    result  = ctx->m_num_answer_literals;
    ctx->m_answer_literals[result] = ans_lit;
    ctx->m_retracted[result]       = Z3_FALSE;
    ctx->m_num_answer_literals++;
    // assert: c OR (not ans_lit)
    args[0] = c;
    args[1] = Z3_mk_not(ctx->m_context, ans_lit);
    Z3_assert_cnstr(ctx->m_context, Z3_mk_or(ctx->m_context, 2, args));
    return result;
}

/**
   \brief Retract an constraint asserted using #assert_retractable_cnstr.
*/
void retract_cnstr(Z3_ext_context ctx, unsigned id) {
    if (id >= ctx->m_num_answer_literals) {
        exitf("invalid constraint id.");
    }
    ctx->m_retracted[id] = Z3_TRUE;
}

/**
   \brief Reassert a constraint retracted using #retract_cnstr.
*/
void reassert_cnstr(Z3_ext_context ctx, unsigned id) {
    if (id >= ctx->m_num_answer_literals) {
        exitf("invalid constraint id.");
    }
    ctx->m_retracted[id] = Z3_FALSE;
}

/**
   \brief Check whether the logical context wrapper with support for retractable assertions is feasible or not.
*/
Z3_lbool ext_check(Z3_ext_context ctx) {
    Z3_lbool result;
    unsigned num_assumptions = 0;
    Z3_ast assumptions[MAX_RETRACTABLE_ASSERTIONS];
    Z3_ast core[MAX_RETRACTABLE_ASSERTIONS];
    unsigned core_size;
    Z3_ast   dummy_proof = 0; // we don't care about the proof in this example.
    Z3_model dummy_model = 0; // we don't care about the model in this example.
    unsigned i;
    for (i = 0; i < ctx->m_num_answer_literals; i++) {
        if (ctx->m_retracted[i] == Z3_FALSE) {
            // Since the answer literal was not retracted, we added it as an assumption.
            // Recall that we assert (C \/ (not ans_lit)). Therefore, adding ans_lit as an assumption has the effect of "asserting" C.
            // If the constraint was "retracted" (ctx->m_retracted[i] == Z3_true), then we don't really need to add (not ans_lit) as an assumption.
            assumptions[num_assumptions] = ctx->m_answer_literals[i];
            num_assumptions ++;
        }
    }
    result = Z3_check_assumptions(ctx->m_context, num_assumptions, assumptions, &dummy_model, &dummy_proof, &core_size, core);
    if (result == Z3_L_FALSE) {
        // Display the UNSAT core
        printf("unsat core: ");
        for (i = 0; i < core_size; i++) {
            // In this example, we display the core based on the assertion ids.
            unsigned j;
            for (j = 0; j < ctx->m_num_answer_literals; j++) {
                if (core[i] == ctx->m_answer_literals[j]) {
                    printf("%d ", j);
                    break;
                }
            }
            if (j == ctx->m_num_answer_literals) {
                exitf("bug in Z3, the core contains something that is not an assumption.");
            }
        }
        printf("\n");
    }

    if (dummy_model) {
        Z3_del_model(ctx->m_context, dummy_model);
    }

    return result;
}

