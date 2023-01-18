#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"
// Defines the lisp value type which will be extended as types other than 
// numbers and erros are added
typedef struct lval {
    int type;
    long num;
    long double doub;

    char* err;
    char* sym;

    int lval_p_count;
    struct lval** cell;
} lval;

// List of all LVAL types
enum lvals { LVAL_DOUBLE, LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR};

// Returns a lisp value obkject of type LVAL_NUM
lval* lval_num(long x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
} 

lval* lval_double(long double x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_DOUBLE;
    v->doub = x;
    return v;
}


// Returns a lisp value obkject of type LVAL_ERR
lval* lval_err(char* m) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m) + 1);
    strcpy(v->err, m);
    return v;
}

lval* lval_sym(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval* lval_sexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->lval_p_count = 0;
    v->cell = NULL;
    return v;
}

lval* lval_qexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
    v->lval_p_count = 0;
    v->cell = NULL;
    return v;
}

// Used to free memory allocated to any lval fields.
void lval_del(lval* v) {
    
//    puts("In lval_Del");
        
    switch (v->type) {
        // LVAL_NUM does not allocate memory aside from the memory for the lval
        // so no need to free extra
        case LVAL_NUM: break;
        case LVAL_DOUBLE: break;
        // LVAL_ERR allocates space for a string which we must free when we have finished with it
        case LVAL_ERR: free(v->err); break;
        // LVAL_SYM allocates space for a string which we must free when we have finished with it
        case LVAL_SYM: free(v->sym); break;
        // LVAL_SEXPR allocates memeory to cell which is a lval**
        // so we need to go through them all and free the memory allocated
        // to each of them.
        case LVAL_QEXPR:
        case LVAL_SEXPR:
            for (int i = 0; i < v->lval_p_count; i++) {
                lval_del(v->cell[i]);
            }
        free(v->cell);
        break;
    }

    free(v);
}



