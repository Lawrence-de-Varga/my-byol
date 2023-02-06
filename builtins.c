#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"
#include "eval.c"

#define LASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
        lval* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args); \
        return err; \
    }


#define INC_ARG_NO(lvalue, func_name, expected_arg_no, ...) \
    if (lvalue->lval_p_count != expected_arg_no) {  \
            int count = lvalue->lval_p_count; \
            lval_del(lvalue); \
            return lval_err("Function %s passed incorrect number of arguments. Expected %d, however recieved: %d.", func_name, expected_arg_no, count); \
    }

#define CALLED_W_NIL(lvalue, error) \
    if (lvalue->cell[0]->lval_p_count == 0) { \
        lval_del(lvalue); \
        return lval_err(error); \
    }

#define BAD_TYPE(lvalue, given_type, expected_type, ...) \
    if (!(given_type == expected_type)) { \
        lval_del(lvalue); \
        return lval_err("Function '%s' passed incorrect type. Expected: '%s', however recieved: '%s'.", ##__VA_ARGS__); \
    }
    


//Used to present type names in readable format
char* lval_type_name(int type) {
    switch(type) {
        case LVAL_DOUBLE: return "Double";                   //  0
        case LVAL_NUM: return "Integer";                     //  1 
        case LVAL_ERR: return "Error";                       //  2
        case LVAL_SYM: return "Symbol";                      //  3
        case LVAL_SEXPR: return "S-Expression";              //  4
        case LVAL_QEXPR: return "Q-Expression";              //  5  
        case  LVAL_FUN: return "Function";                   //  6
        default: return "Unknown Type";
    }
}

/******************************* LIST FUNCTIONS *******************************/

// takes a q-expr and returns a q-expr with only the first element of the input q-expr
lval* builtin_car(lenv* e, lval* a) {
//    puts("in car before erro check");
    BAD_TYPE(a, a->cell[0]->type, LVAL_QEXPR, "car",  lval_type_name(LVAL_QEXPR), lval_type_name(a->cell[0]->type));
    CALLED_W_NIL(a, "Function car passed NIL as argument");
    INC_ARG_NO(a, "car", 1);
//    puts("in car after erro check");

    // create a new lval frm the first element of a
    lval* v = lval_take(a, 0);
    // delete every other element of a
    while (v->lval_p_count > 1) { lval_del(lval_pop(v,1)); }
    return v;
}

lval* builtin_cdr(lenv* e, lval* a) {
    INC_ARG_NO(a, "cdr", 1);
    BAD_TYPE(a, a->cell[0]->type, LVAL_QEXPR, "cdr", lval_type_name(LVAL_QEXPR), lval_type_name(a->cell[0]->type));
    CALLED_W_NIL(a, "Function 'cdr' passed {}.");


    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_cons(lenv* e, lval* a) {
    INC_ARG_NO(a, "cons", 2);
    BAD_TYPE(a, a->cell[1]->type, LVAL_QEXPR, "cons", lval_type_name(LVAL_QEXPR), lval_type_name(a->cell[1]->type));

    // increase count and increase allocated memory
    a->cell[1]->lval_p_count++;
    a->cell[1]->cell = realloc(a->cell[1]->cell, sizeof(lval*) * a->cell[1]->lval_p_count);

    // move the current data up to  the new end of memory
    memmove(a->cell[1]->cell+1, a->cell[1]->cell,
            sizeof(lval*) * (a->cell[1]->lval_p_count));
    // set the first place inmemory equal to the object to be consed on
    a->cell[1]->cell[0] = a->cell[0];
    
    return a->cell[1];
}

lval* builtin_reverse(lenv* e, lval* a) {
    INC_ARG_NO(a, "reverse", 1);
    BAD_TYPE(a, a->cell[0]->type, LVAL_QEXPR, "reverse", lval_type_name(LVAL_QEXPR), lval_type_name(a->cell[0]->type));

    lval* v = lval_qexpr();
    lval* to_rev = a->cell[0];

    int a_length = to_rev->lval_p_count;
    v->cell = realloc(v->cell, sizeof(lval*) * a_length);
    

    for(int i = 0; i < a_length; i++) {
        v->lval_p_count++;
        v->cell[i] = to_rev->cell[a_length - (i + 1)];
    }

    return v;
}

lval* builtin_init(lenv* e, lval* a) {
    INC_ARG_NO(a,"init", 1);
    BAD_TYPE(a, a->cell[0]->type, LVAL_QEXPR, "init", lval_type_name(LVAL_QEXPR), lval_type_name(a->cell[0]->type));

    lval* v = lval_qexpr();
    // set to_map for convenience
    lval* to_map = a->cell[0];

    int count = to_map->lval_p_count -1;

    // allocate enough space for v->cell 
    v->cell = realloc(v->cell, sizeof(lval*) * (count));
    // decrease the space in to_map->cell, as otherwise when we go to copy 
    // it will try to copy too much data into v->cell
    to_map->cell = realloc(to_map->cell, sizeof(lval*) * (count));

    memcpy(v->cell, to_map->cell, sizeof(lval*) * (count));
    v->lval_p_count = count;

    return v;
}

lval* builtin_list(lenv* e, lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval* lval_join(lenv* e, lval* x, lval* y) {

    while (y->lval_p_count) {
        x = lval_add(x, lval_pop(y, 0));
    }

    lval_del(y);
    return x;
}


lval* builtin_join(lenv* e, lval* a) {

    for (int i = 0; i < a->lval_p_count; i++) {
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "Function 'join' passed incorrect type.");
    }

    lval* x = lval_pop(a, 0);

    while (a->lval_p_count) {
        x = lval_join(e, x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}


lval* builtin_len(lenv* e, lval* a) {
    BAD_TYPE(a, a->cell[0]->type, LVAL_QEXPR, "len", lval_type_name(LVAL_QEXPR), lval_type_name(a->cell[0]->type));
    INC_ARG_NO(a, "length", 1);

    switch(a->type) {
        case LVAL_QEXPR:
        case LVAL_SEXPR:
            return lval_num(a->cell[0]->lval_p_count);
        default:
            return lval_num(1);
    }
}

/********************* SPECIAL FUNCTIONS ************************************/

lval* builtin_eval(lenv* e, lval* a) {
    INC_ARG_NO(a, "eval", 1);
    BAD_TYPE(a, a->cell[0]->type, LVAL_QEXPR, "eval", lval_type_name(LVAL_QEXPR), lval_type_name(a->cell[0]->type));

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval* builtin_def(lenv* e, lval* a) {
    BAD_TYPE(a, a->cell[0]->type, LVAL_QEXPR, "def", lval_type_name(LVAL_QEXPR), lval_type_name(a->cell[0]->type));

    lval* syms = a->cell[0];

    for (int i = 0; i < syms->lval_p_count; i++) {
        LASSERT(a, syms->cell[i]->type == LVAL_SYM, "Fcuntion def coonot define non-symbols.");
    }

    LASSERT(a, syms->lval_p_count = a->lval_p_count-1, "Function 'def' cannot define innorect number of values to symbols.");
    
    for (int i = 0; i < syms->lval_p_count; i++) {
        lenv_put(e, syms->cell[i], a->cell[i+1]);
    }

    lval_del(a);
    return lval_sexpr();
}

/*********************** MATH FUNCTIONS **************************************/

lval* builtin_add(lenv* e, lval* a) {
    return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
    return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
    return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
    return builtin_op(e, a, "/");
}

/************************ REGISTERING BUILTINS FOR ENVIRONMENT ***************/

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
    lval* k = lval_sym(name);
    lval* v = lval_fun(func, name);
    lenv_put(e, k, v);
    lval_del(k); lval_del(v);
}

void lenv_add_builtins(lenv* e) {
    // List functions
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "car", builtin_car);
    lenv_add_builtin(e, "cdr", builtin_cdr);
    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "reverse", builtin_reverse);
    lenv_add_builtin(e, "init", builtin_init);
    lenv_add_builtin(e, "join", builtin_join);
    lenv_add_builtin(e, "length", builtin_len);

    // Variable Functions
    lenv_add_builtin(e, "def", builtin_def);

    // Other
    lenv_add_builtin(e, "eval", builtin_eval);

    // Math functions
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
}


