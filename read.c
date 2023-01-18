#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"
#include "lval.c"

// used only for testing purposes when needed
void printast(mpc_ast_t* ast) {

    printf("TAG t: %s\n", ast->tag);
    printf("CONTENTS t: %s\n", ast->contents);
    printf("CHILDREN_NUM t: %i\n\n", ast->children_num);
}

// read in a double value
lval* lval_read_double(mpc_ast_t* t) {
    errno = 0;
    char *end;
    long double x = strtold(t->contents, &end);
    return errno != ERANGE ? lval_double(x) : lval_err("invalid double");
}

//reads in a integer value
lval* lval_read_int(mpc_ast_t* t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invliad integer");
}

// takes twoi Lval*'s and combines into one
lval* lval_add(lval* v, lval* x) {
    v->lval_p_count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->lval_p_count);
    v->cell[v->lval_p_count-1] = x;
    return v;
}

lval* lval_read(mpc_ast_t* t) {

    if (strstr(t->tag, "integer")) { return lval_read_int(t); }
    if (strstr(t->tag, "double")) { return lval_read_double(t); }
    if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }
    
//    puts("Printing current ast inside lval_read.");
//    printast(t);
    
    lval* x;
    // create a base SEXPR or QEXPR to be built upon
    if (strstr(t->tag, "qexpr")) { x = lval_qexpr(); }
    if (strstr(t->tag, "sexpr")) { x = lval_sexpr(); }
    /*
    if (strstr(t->tag, ">") == 0) { x = lval_sexpr(); }
    if (strstr(t->tag, "sexpr") == 0) { puts("Setting x to non-null."); x = lval_sexpr(); }
    if (strstr(t->tag, "expr") == 0) { puts("Setting x to non-null."); x = lval_sexpr(); }
    */
        
    for (int i = 0; i < t->children_num; i++) {
        // the () or {} are added back in later when printing
        if (strcmp(t->children[i]->contents, "(" ) == 0) {continue; }
        if (strcmp(t->children[i]->contents, ")" ) == 0) {continue; }
        if (strcmp(t->children[i]->contents, "{" ) == 0) {continue; }
        if (strcmp(t->children[i]->contents, "}" ) == 0) {continue; }
        // I don't think I need this line
//        if (strcmp(t->children[i]->tag, "regex" ) == 0) {continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}
