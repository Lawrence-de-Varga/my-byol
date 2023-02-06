#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mpc.h"


struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

// List of all LVAL types
enum lvals { LVAL_DOUBLE, LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR, LVAL_FUN};


typedef lval*(*lbuiltin)(lenv*, lval*);

// Defines the lisp value type which will be extended as types other than 
// numbers and erros are added
typedef struct lval {
    int type;
    long num;
    long double doub;

    char* err;
    char* sym;
    lbuiltin fun;
    char* fun_name;

    int lval_p_count;
    struct lval** cell;
} lval;


/************************* LVAL Constructors ***********************/ 

lval* lval_fun(lbuiltin func, char* fun_name) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FUN;
    v->fun = func;
    v->fun_name = fun_name;
    return v;
}


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
lval* lval_err(char* fmt, ...) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;

    va_list va;
    va_start(va, fmt);

    v->err = malloc(1024);

    vsnprintf(v->err, 1023, fmt, va);

    v->err = realloc(v->err, strlen(v->err)+1);

    va_end(va);

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


/**************** LVAL Utility Functions ***************************/

void print_lval(lval* v) {
    printf("lval type: %d\n", v->type);
    printf("lval num: %ld\n", v->num);
//    printf("lval doub: %Lf\n", v->doub);
//    printf("lval err: %s\n", v->err);
//    printf("lval sym: %s\n", v->sym);
    printf("lval fun: %p\n", v->fun);
    printf("lval count: %d\n", v->lval_p_count);
    puts("\n");
    for (int i = 0; i < v->lval_p_count; i++) {
        printf("lvla child %d.\n", i);
        print_lval(v->cell[i]);
    }
}

lval* lval_copy(lval* v) {

    lval* x = malloc(sizeof(lval));
    x->type = v->type;

    switch (v->type) {

        // Copy functions and numbers directly
        case LVAL_FUN: x->fun_name = v->fun_name; x->fun = v->fun; break;
        case LVAL_NUM: x->num = v->num; break;
        case LVAL_DOUBLE: x->doub = v->doub; break;

          /* Copy strings using malloc and strcpy */
        case LVAL_ERR: 
              x->err = malloc(strlen(v->err) +1);
              strcpy(x->err, v->err); break;

        case LVAL_SYM:
              x->sym = malloc(strlen(v->sym) +1);
              strcpy(x->sym, v->sym); break;

        case LVAL_SEXPR:
        case LVAL_QEXPR:
              x->lval_p_count = v->lval_p_count;
              x->cell = malloc(sizeof(lval*) * x->lval_p_count);
              for (int i = 0; i < x->lval_p_count; i++) {
                  x->cell[i] = lval_copy(v->cell[i]);
              } break;
    }

    return x;
}


char* lval_present(lval* v);

// Used to generate a string representation of a SEXPR or QEXPR
char* lval_expr_present(lval* v, char open, char close) {
//    puts("in lval_expr_present");
    char* expr = calloc(1024, sizeof(char));
//    int expr_size = 1;
    strcat(expr, &open);

    for (int i = 0; i < v->lval_p_count; i++) {
//        expr_size++;
//        expr = realloc(expr, expr_size);
        strcat(expr, lval_present(v->cell[i]));
//        printf("char: %s\n", lval_present(v->cell[i]));
//        printf("expr %s\n", expr);



        // Don't print trailing space if last element
//        expr_size++;
//        expr = realloc(expr, expr_size);
        if (i < (v->lval_p_count-1)) {
           strcat(expr, " "); 
        }
    }

    strcat(expr, &close);
//    char* string  = expr;
//    printf("%s\n", expr);
    expr = realloc(expr, sizeof(expr));
    return expr;
}

// used to return a string representation of any lval for erro messaging purposes.
char* lval_present (lval* v) {
//    puts("In lval_present");
    char* string_presentation = calloc(2048, sizeof(char));
//    puts ("after string_presentation assign");
    char* expr;
    switch (v->type) {
        case LVAL_SEXPR:    
            expr = lval_expr_present(v, '(', ')');
            strcat(string_presentation, expr); 
            free(expr);
            break;

        case LVAL_QEXPR:    
            expr = lval_expr_present(v, '{', '}');
            strcat(string_presentation, expr); 
            free(expr);
            break;
        case LVAL_FUN:      sprintf(string_presentation, "%s", v->fun_name); break;
        case LVAL_SYM:      sprintf(string_presentation, "%s", v->sym); break;
        case LVAL_ERR:      sprintf(string_presentation, "%s", v->err); break;
        case LVAL_NUM:      sprintf(string_presentation, "%ld", v->num); break;
        case LVAL_DOUBLE:   sprintf(string_presentation, "%Lf", v->doub); break;
        default:            sprintf(string_presentation, "Unknown value."); break;
    }
    
//    char* string = realloc(string_presentation, strlen(string_presentation) +1);
    string_presentation = realloc(string_presentation, sizeof(string_presentation));
    return string_presentation;
}


// Used to free memory allocated to any lval fields.
void lval_del(lval* v) {
    
//    puts("In lval_Del");
        
    switch (v->type) {
        // LVAL_NUM does not allocate memory aside from the memory for the lval
        // so no need to free extra
        case LVAL_NUM: break;
        case LVAL_DOUBLE: break;
        case LVAL_FUN: break;
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


/****************************** LENV STRUCTS AND FUNCTIONS *****************/

struct lenv {
    int count;
    char** syms;
    lval** vals;
};

lenv* lenv_new(void) {
    lenv* e = malloc(sizeof(lenv));
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

void lenv_del(lenv* e) {
    for (int i = 0; i < e->count; i++) {
        free(e->syms[i]);
        lval_del(e->vals[i]);
    }
    free(e->syms);
    free(e->vals);
   free(e);
}

// Finds a symbol in the env that matches the input or returns an erro
lval* lenv_get(lenv* e, lval* k) {
//    puts("in lval_get");
    for (int i = 0; i < e->count; i++) {

        if (strcmp(e->syms[i], k->sym) == 0) {
//            puts("found matching sym in lval_get");
            return lval_copy(e->vals[i]);
        }
    }


    return lval_err("symbol %s, is not bound in this environment.", k->sym);
}

// Checks if a symbol in in the given env
bool sym_in_env_p(lenv* e, char* str) {
    for (int i = 0; i < e->count; i++) {
        if (strcmp(str, e->syms[i]) == 0) {
            return true;
        }
    }
    return false;
}
        


// Adds a newly defined symbol to the env
void lenv_put(lenv* e, lval* k, lval* v) {

    for (int i = 0; i < e->count; i++) {

        if (strcmp(e->syms[i], k->sym) == 0) {
                lval_del(e->vals[i]);
                e->vals[i] = lval_copy(v);
                return;
        }
    }

    e->count++;
    e->syms = realloc(e->syms, sizeof(char*) * e->count);
    e->vals = realloc(e->vals, sizeof(lval*) * e->count);

    e->vals[e->count-1] = lval_copy(v);
    e->syms[e->count-1] = malloc(strlen(k->sym) +1);
    strcpy(e->syms[e->count-1], k->sym);
}








