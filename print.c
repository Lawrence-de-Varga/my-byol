#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"
#include "builtins.c"

void lval_print(lval* v);

void lval_expr_print(lval* v, char open, char close) {
    putchar(open);

    for (int i = 0; i < v->lval_p_count; i++) {
        lval_print(v->cell[i]);


        // Don't print trailing space if last element
        if (i < (v->lval_p_count-1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

void lval_print(lval* v) {

//    puts("Entering lval_print");

    // chose print representation based of LVAL type
    switch(v->type) {
        case LVAL_FUN:       printf("<function>: '%s'", v->fun_name); break;
        case LVAL_NUM:       printf("%li", v->num); break;
        case LVAL_DOUBLE:    printf("%Lf", v->doub); break;
        case LVAL_ERR:       puts("in case error"); printf("Error: %s", v->err); break;
        case LVAL_SYM:       printf("%s", v->sym); break;
        case LVAL_SEXPR:     lval_expr_print(v, '(', ')'); break;
        case LVAL_QEXPR:     lval_expr_print(v, '{', '}'); break;
    }
}


void lval_println(lval* v) { lval_print(v); putchar('\n'); }

