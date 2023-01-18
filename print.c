#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"
#include "eval.c"

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

    switch(v->type) {
        case LVAL_NUM:    printf("%li", v->num); break;
        case LVAL_ERR:    printf("Error: %s", v->err); break;
        case LVAL_SYM:    printf("%s", v->sym); break;
        case LVAL_SEXPR:  lval_expr_print(v, '(', ')'); break;
    }
}


void lval_println(lval* v) { lval_print(v); putchar('\n'); }
