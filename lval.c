#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"
// Defines the lisp value type which will be extended as types other than 
// numbers and erros are added
typedef struct lval {
    int type;
    long num;

    char* err;
    char* sym;

    int lval_p_count;
    struct lval** cell;
} lval;

// defines two enums, one a list of currently available types and the other
// a list of current erros
enum lvals { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };

// Returns a lisp value obkject of type LVAL_NUM
lval* lval_num(long x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
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
//    puts("Constructing a lval_sym.");
    lval* v = malloc(sizeof(lval));
//    puts("After malloc for sym.");
    v->type = LVAL_SYM;
//    puts("After setting type for sym.");
    v->sym = malloc(strlen(s) + 1);
//    puts("After malloc for v->sym.");
    strcpy(v->sym, s);
//    puts("After strcpy.");
    return v;
}

lval* lval_sexpr(void) {
//    puts("In lval_sexpr.");
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->lval_p_count = 0;
    v->cell = NULL;
    return v;
}

// Used to free memory allocated to any lvals
void lval_del(lval* v) {
    
//    puts("In lval_Del");
        
    switch (v->type) {
        // LVAL_NUM does not allocate memory so no need to free
        case LVAL_NUM: break;
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;
        // LVAL_SEXPR allocates memeory to cell which is a lval**
        // so we need to go through them all and free the memory allocated
        // to each of them.
        case LVAL_SEXPR:
            for (int i = 1; i <= v->lval_p_count; i++) {
                lval_del(v->cell[i]);
            }
        free(v->cell);
        break;
    }

    free(v);
}


