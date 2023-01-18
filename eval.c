#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"
#include "read.c"

// Used for basic debuging

// eval_h simply allows us to get straight at the expression
// in the ast that we want ignoring the '>' and 'regex' tags
mpc_ast_t* eval_h(mpc_ast_t* t) {
    return t->children[1];
}

lval* builtin_op(lval* a, char* op);
lval* lval_eval(lval* v);

lval* lval_pop(lval* v, int i) {
    /* find the item at index i */
    lval* x = v->cell[i];

    /* shift memory after the item at i over the top */
    memmove(&v->cell[i], &v->cell[i+1],
            sizeof(lval*) * (v->lval_p_count-1));

    // Decrease the cou t of items in the list
    v->lval_p_count--;

    // Reallocate the memeory used
    v->cell = realloc(v->cell, sizeof(lval*) * v->lval_p_count);
    return x;
}


lval* lval_take(lval* v, int i) {
    /* Takes the item at index i and deletes the list it was in. */
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}



lval* builtin(lval* a, char* func);

lval* lval_eval_sexpr(lval* v) {

    /* Evaluate children */
    for (int i = 0; i < v->lval_p_count; i++) {
        v->cell[i] = lval_eval(v->cell[i]);
    }

    // CHeck for any errors in evaluated lval
    for (int i = 0; i < v->lval_p_count; i++) {
        if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
    }

    // evaluate null list ()
    if (v->lval_p_count == 0) {return v; }

    // Evalute single atom sexpr's
    if (v->lval_p_count == 1) { return lval_take(v, 0); }

    // Check that first element is a symbol
    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_SYM) {
        lval_del(f); lval_del(v);
        return lval_err("S-expression does not start with symbol.");
    }

    // call builtin_op with operator
    lval* result = builtin(v, f->sym);
    lval_del(f);
    return result;
}


lval* lval_eval(lval* v) {
    /* Evaluate s-expressions */
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(v); }
    // if not s-exp just return directly
    return v;
}


lval* builtin_op(lval* a, char* op) {

    // Ensure that al arguments are numbers
    for (int i = 0; i < a->lval_p_count; i++) {
        if ((a->cell[i]->type != LVAL_NUM) && (a->cell[i]->type != LVAL_DOUBLE)) {
            lval_del(a);
            return lval_err("Cannot operate on non-numbers.");
        }
    }

    // grab the first argument
    lval* x = lval_pop(a, 0);

//    if (x->num_type == 0) {


    // If only one argument and op is subtraction than negate arguments
    if ((strcmp(op, "-") == 0) && a->lval_p_count == 0) {
        switch(x->type) {
            case LVAL_NUM:
                x->num = -x->num;
            case LVAL_DOUBLE:
                x->num = -x->num;
        }
    }

    while (a->lval_p_count > 0) {

        // Pop the next argument
        lval* y = lval_pop(a, 0);
        
        // c is statically typed whilst the lisp is currently dynamically typed
        // thus I need to fiddle around with types as below.
        switch (x->type) {
            case LVAL_NUM: 
                if (y->type == LVAL_NUM) {
                    if (strcmp(op, "+") == 0) { x->num += y->num; }
                    if (strcmp(op, "-") == 0) { x->num -= y->num; }
                    if (strcmp(op, "*") == 0) { x->num *= y->num; }
                    if (strcmp(op, "/") == 0) {
                        if (y->num == 0) {
                        lval_del(x); lval_del(y);
                        x = lval_err("Division by zero."); break;
                        }
                        x->num /= y->num;
                    }
                } else { 

                    // here we set the currently unset value of x->doub
                    // as if y is a double we want the conversion to be automatic
                    if (strcmp(op, "+") == 0) { x->doub = (x->num + y->doub); }
                    if (strcmp(op, "-") == 0) { x->doub = (x->num - y->doub); }
                    if (strcmp(op, "*") == 0) { x->doub = (x->num * y->doub); }
                    if (strcmp(op, "/") == 0) {
                        if (y->doub == 0) {
                        lval_del(x); lval_del(y);
                        x = lval_err("Division by zero."); break;
                        }
                        x->doub = (x->num / y->doub);
                    }
//                    x->type is set to a double here because as soon as one
//                    double is incl;uded in the calculation the result must
//                    necessarily be a double.
                    x->type = LVAL_DOUBLE;
                } break;
            case LVAL_DOUBLE:
                if (y->type == LVAL_NUM) {
                    if (strcmp(op, "+") == 0) { x->doub += y->num; }
                    if (strcmp(op, "-") == 0) { x->doub -= y->num; }
                    if (strcmp(op, "*") == 0) { x->doub *= y->num; }
                    if (strcmp(op, "/") == 0) {
                        if (y->num == 0) {
                        lval_del(x); lval_del(y);
                        x = lval_err("Division by zero."); break;
                        }
                        x->doub /= y->num;
                    }
                } else { 
                    if (strcmp(op, "+") == 0) { x->doub += y->doub; }
                    if (strcmp(op, "-") == 0) { x->doub -= y->doub; }
                    if (strcmp(op, "*") == 0) { x->doub *= y->doub; }
                    if (strcmp(op, "/") == 0) {
                        if (y->doub == 0) {
                        lval_del(x); lval_del(y);
                        x = lval_err("Division by zero."); break;
                        }
                        x->doub /= y->doub;
                    }
                } break;
       }
        
    lval_del(y);
    }
    lval_del(a); return x;
}



