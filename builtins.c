#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"
#include "eval.c"

#define LASSERT(args, cond, err) \
    if (!(cond)) { lval_del(args); return lval_err(err); }

#define INC_ARG_NO(args, arg_no, err) \
    if (!(args->lval_p_count == arg_no) { lval_del(args); return lval_err(err); }

// takes a q-expr and returns a q-expr with only the first element of the input q-expr
lval* builtin_car(lval* a) {
    LASSERT(a, a->lval_p_count == 1, "Function 'car' passed too many arguments.");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'car' pass inncorrecf type.");
    LASSERT(a, a->cell[0]->lval_p_count != 0, "Function 'car' pass {}");


    // create a new lval frm the first element of a
    lval* v = lval_take(a, 0);
    // delete every other element of a
    while (v->lval_p_count > 1) { lval_del(lval_pop(v,1)); }
    return v;
}


lval* builtin_cdr(lval* a) {
    LASSERT(a, a->lval_p_count == 1, "Function 'cdr' passed too many arguments.");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'cdr' passed incorrect type.");
    LASSERT(a, a->cell[0]->lval_p_count != 0, "Function 'cdr' passed {}.");


    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}


lval* builtin_list(lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_eval(lval* a) {
    LASSERT(a, a->lval_p_count == 1, "Function 'eval' passed too many arguments.");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'eval' passed incorrect type.");

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(x);
}


lval* lval_join(lval* x, lval* y) {

    while (y->lval_p_count) {
        x = lval_add(x, lval_pop(y, 0));
    }

    lval_del(y);
    return x;
}


lval* builtin_join(lval* a) {

    for (int i = 0; i < a->lval_p_count; i++) {
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "Function 'join' passed incorrect type.");
    }

    lval* x = lval_pop(a, 0);

    while (a->lval_p_count) {
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval* builtin(lval* a, char* func) {
    if (strcmp("list", func) == 0) { return builtin_list(a); }
    if (strcmp("car", func) == 0) { return builtin_car(a); }
    if (strcmp("cdr", func) == 0) { return builtin_cdr(a); }
    if (strcmp("join", func) == 0) { return builtin_join(a); }
    if (strcmp("eval", func) == 0) { return builtin_eval(a); }
    if (strstr("+-*/^", func)) { return builtin_op(a, func); }
    lval_del(a);
    return lval_err("Unknown function.");
}






























