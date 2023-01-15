#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


lval* lval_read_int(mpc_ast_t* t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid_number");
}

lval* lval_add(lval* v, lval* x) {
//    puts ("In lval_add.");
//    printf("v: %p.\n", v);
//    printf("x: %p.\n", x);
    v->lval_p_count++;
//    printf("lval_p_count: %d\n", v->lval_p_count);
//    puts("After lval_add count++");
    // Im not 100% clear about why I needed to add 1 to the lval_p_count as opposed to the book
    v->cell = realloc(v->cell, sizeof(lval*) * v->lval_p_count+1);
//    puts("After lval_add realloc.");
    v->cell[v->lval_p_count] = x;
//    puts("After lval_add cell change.");
    return v;
}

lval* lval_read(mpc_ast_t* t) {
//    puts("Starting to read lval");
//    mpc_ast_print(t);
//    puts("");
//    printf("t->tag: %s\n", t->tag);
//    puts("");
//    printf("t->contents: %s\n", t->contents);
//    puts("");
    if (strstr(t->tag, "integer")) { return lval_read_int(t); }
    if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

//    puts("After int and sym checks.");

    // Declare an empty list which will be filled with valied sexprs
    lval* x = lval_sexpr();
    //    We don't need this line from the book as i use eval_h to 
    //    remove > and regex from the ast that is being parsed
    if (strstr(t->tag, ">") == 0) { x = lval_sexpr(); }
    if (strstr(t->tag, "sexpr") == 0) { puts("Setting x to non-null."); x = lval_sexpr(); }
    if (strstr(t->tag, "expr") == 0) { puts("Setting x to non-null."); x = lval_sexpr(); }
        
    for (int i = 0; i < t->children_num; i++) {
//        printf("x: %p.\n", x);
//        printf("i: %d\n", i);
//        puts("In lval_read for loop.");
        if (strcmp(t->children[i]->contents, "(" ) == 0) {continue; }
        if (strcmp(t->children[i]->contents, ")" ) == 0) {continue; }
        // I don't think I need this line
//        if (strcmp(t->children[i]->tag, "regex" ) == 0) {continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

void lval_print(lval* v);

void lval_expr_print(lval* v, char open, char close) {
    putchar(open);
    for (int i = 1; i <= v->lval_p_count; i++) {
//        puts("In lvla_expr_print loop");
//        printf("v->type: %s\n", v->sym);
        lval_print(v->cell[i]);


        // Don't print trailing space if last element
        if (i <= (v->lval_p_count-1)) {
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
