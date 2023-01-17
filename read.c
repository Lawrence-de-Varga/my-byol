#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"
#include "lval.c"

lval* lval_read_int(mpc_ast_t* t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid_number");
}

lval* lval_add(lval* v, lval* x) {
    v->lval_p_count++;
    // Im not 100% clear about why I needed to add 1 to the lval_p_count as opposed to the book
    v->cell = realloc(v->cell, sizeof(lval*) * v->lval_p_count+1);
    v->cell[v->lval_p_count-1] = x;
    return v;
}

lval* lval_read(mpc_ast_t* t) {
    if (strstr(t->tag, "integer")) { return lval_read_int(t); }
    if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }


    // Declare an empty list which will be filled with valied sexprs
    lval* x = lval_sexpr();
    
    if (strstr(t->tag, ">") == 0) { x = lval_sexpr(); }
    if (strstr(t->tag, "sexpr") == 0) { puts("Setting x to non-null."); x = lval_sexpr(); }
    if (strstr(t->tag, "expr") == 0) { puts("Setting x to non-null."); x = lval_sexpr(); }
        
    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(" ) == 0) {continue; }
        if (strcmp(t->children[i]->contents, ")" ) == 0) {continue; }
        // I don't think I need this line
//        if (strcmp(t->children[i]->tag, "regex" ) == 0) {continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}
