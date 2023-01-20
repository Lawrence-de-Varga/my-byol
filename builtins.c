#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"
#include "eval.c"

#define LASSERT(args, cond, err) \
    if (!(cond)) { lval_del(args); return lval_err(err); }


#define INC_ARG_NO(lvalue, expected_arg_no,  error) \
    if (lvalue->lval_p_count != expected_arg_no) {  \
            lval_del(lvalue); \
            return lval_err(error); \
    }

#define CALLED_W_NIL(lvalue, error) \
    if (lvalue->cell[0]->lval_p_count == 0) { \
        lval_del(lvalue); \
        return lval_err(error); \
    }


// takes a q-expr and returns a q-expr with only the first element of the input q-expr
lval* builtin_car(lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'car' passed incorrect type.");
    CALLED_W_NIL(a, "Function car passed NIL as argument");
    INC_ARG_NO(a, 1, "Function 'car' passed too many arguments.");

    // create a new lval frm the first element of a
    lval* v = lval_take(a, 0);
    // delete every other element of a
    while (v->lval_p_count > 1) { lval_del(lval_pop(v,1)); }
    return v;
}


lval* builtin_cdr(lval* a) {
    INC_ARG_NO(a, 1, "Function 'cdr' passed too many arguments.");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'cdr' passed incorrect type.");
    CALLED_W_NIL(a, "Function 'cdr' passed {}.");


    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_cons(lval* a) {
    INC_ARG_NO(a, 2, "Function 'cons' passed incorrect number of arguments.");
    LASSERT(a, a->cell[1]->type == LVAL_QEXPR, "Function 'cons' needs a q-expr as its second argument.");

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

lval* builtin_reverse(lval* a) {
    INC_ARG_NO(a, 1, "Function 'reverse' called with incorrect number of arguments.");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'reverse' needs a q-expr as its argument.");

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

lval* builtin_init(lval* a) {
    INC_ARG_NO(a, 1, "Function 'init' called with incorrect number of arguments.");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'init' needs a q-expr as its argument.");

    lval* v = lval_qexpr();
    lval* to_map = a->cell[0];
    int count = to_map->lval_p_count -1;

    v->cell = realloc(v->cell, sizeof(lval*) * (count));

    for (int i = 0; i < count; i++) {
        v->cell[i] = to_map->cell[i];
    }
    v->lval_p_count = count;
    return v;
}




lval* builtin_list(lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_eval(lval* a) {
    INC_ARG_NO(a, 1, "Function 'eval' passed too many arguments.");
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


lval* builtin_len(lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'len' passed incorrect type.");
    INC_ARG_NO(a, 1, "Function 'len' passed too many arguments.");

    switch(a->type) {
        case LVAL_QEXPR:
        case LVAL_SEXPR:
            return lval_num(a->cell[0]->lval_p_count);
        default:
            return lval_num(1);
    }
}


lval* builtin(lval* a, char* func) {
    if (strcmp("list", func) == 0) { return builtin_list(a); }
    if (strcmp("len", func) == 0) { return builtin_len(a); }
    if (strcmp("car", func) == 0) { return builtin_car(a); }
    if (strcmp("cons", func) == 0) { return builtin_cons(a); }
    if (strcmp("cdr", func) == 0) { return builtin_cdr(a); }
    if (strcmp("join", func) == 0) { return builtin_join(a); }
    if (strcmp("reverse", func) == 0) { return builtin_reverse(a); }
    if (strcmp("init", func) == 0) { return builtin_init(a); }
    if (strcmp("eval", func) == 0) { return builtin_eval(a); }
    if (strstr("+-*/^", func)) { return builtin_op(a, func); }
    lval_del(a);
    return lval_err("Unknown function.");
}





