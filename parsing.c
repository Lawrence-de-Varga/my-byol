#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
#include "mmath.c"

/* Code for when running on windows */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* readline for windows */
char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy-1] = '\0';
    return cpy;
    }

/* add_history for windows */
void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif


// Defines the lisp value type which will be extended as types other than 
// numbers and erros are added
typedef struct {
    int type;
    long num;
    int err;
} lval;

// defines two enums, one a list of currently available types and the other
// a list of current erros
enum { LVAL_NUM, LVAL_ERR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

// Returns a lisp value obkject of type LVAL_NUM
lval lval_num(long x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
} 

// Returns a lisp value obkject of type LVAL_ERR
lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}


void lval_print(lval v) {

    puts("Entering lval_print");

    switch(v.type) {
        case LVAL_NUM: printf("%li", v.num); break;
        case LVAL_ERR:
            switch(v.err) {
                case LERR_DIV_ZERO: printf("Error: Diveision By Zero."); break;
                case LERR_BAD_OP: printf("Error: Invalid Operator."); break;
                case LERR_BAD_NUM: printf("Error: Invalid Number."); break;
                break; }
        break; }
}

void lval_println(lval v) { lval_print(v); putchar('\n'); }


// Used for basic debuging
void printast(mpc_ast_t* ast) {

    printf("TAG t: %s\n", ast->tag);
    printf("CONTENTS t: %s\n", ast->contents);
    printf("CHILDREN_NUM t: %i\n\n", ast->children_num);
}


// Used to evaluate expressions of one operator and one arguments
// such as (- 5) or (/ 5)
lval eval_op_1(char* op, lval x) {

    puts("Entering eval_op_1");

    if (x.type == LVAL_ERR) { return x;}

    if (strcmp(op, "+") == 0) {return lval_num(x.num);}
    if (strcmp(op, "-") == 0) {return lval_num(-x.num);}
    if (strcmp(op, "*") == 0) {return lval_num(x.num);}
    if (strcmp(op, "/") == 0) {return lval_num(1/x.num);}
    if (strcmp(op, "%") == 0) {return lval_num(x.num);}
    if (strcmp(op, "^") == 0) {return lval_num(expo(x.num, x.num));}
    if (strcmp(op, "min") == 0) {return lval_num(x.num);}
    if (strcmp(op, "max") == 0) {return lval_num(x.num);}
    return lval_err(LERR_BAD_OP);
}

// Main operator evaluation rule
lval eval_op_n(lval x, char* op, lval y) {

    puts("Entering eval_op_n");

    if (x.type == LVAL_ERR) { return x;}
    if (y.type == LVAL_ERR) { return y;}

    if (strcmp(op, "+") == 0) {return lval_num(x.num + y.num);}
    if (strcmp(op, "-") == 0) {return lval_num(x.num - y.num);}
    if (strcmp(op, "*") == 0) {return lval_num(x.num * y.num);}
    if (strcmp(op, "/") == 0) 
        {return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);}
    if (strcmp(op, "%") == 0) {return lval_num(x.num % y.num);}
    if (strcmp(op, "^") == 0) {return lval_num(expo(x.num, y.num));}
    if (strcmp(op, "min") == 0) {return lval_num(min(x.num, y.num));}
    if (strcmp(op, "max") == 0) {return lval_num(max(x.num, y.num));}

    return lval_err(LERR_BAD_OP);
}

// eval_h simply allows us to get straight at the expression
// in the ast that we want ignoring the '>' and 'regex' tags
mpc_ast_t* eval_h(mpc_ast_t* t) {
    return t->children[1];
}

// Evaluates any valid expression and returns an lval (currently only a number or 
// an error)
lval eval(mpc_ast_t* t) {

    puts("Entering eval");

    /* If tagged as number return it directly */
    if (strstr(t->tag, "number")) {
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    /* The operator is always the second child. */
    char* op = t->children[1]->contents;

    /* We store the third child in x. */
    lval x = eval(t->children[2]);

    // if the user gives (- 5) or (+ 5) a result will be returned by eval_op_1
    if ((strstr (t->children[3]->tag, "char")) && !(strstr(t->children[3]->tag, "operator"))) {
        return eval_op_1(op, x);
    }
    /* iterate over and combine the remaining childnre */
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op_n(x, op, eval(t->children[i]));
        i++;
    }

    return x;
}


int main(int argc, char** argv) {
    /* Create some parsers */
    mpc_parser_t* Number    = mpc_new("number");
    mpc_parser_t* Operator  = mpc_new("operator");
    mpc_parser_t* Expr      = mpc_new("expr");
    mpc_parser_t* Lispy     = mpc_new("lispy");

    /* Define them with the following langauge */
    mpca_lang(MPCA_LANG_DEFAULT,
            "                                                      \
                number    : /-?[0-9]+/;                            \
                operator  : '+' | '-' | '*' | '/' | '%' | '^' | /min/ | /max/;           \
                expr      : <number> | '(' <operator> <expr>+ ')'; \
                lispy     : /^/ <expr> /$/;            \
            ",
            Number, Operator, Expr, Lispy);

    /* Print version and exit information */
    puts("Lispy Version 0.0.0.3");
    puts("Press Ctrl+c to Exit\n");

    /* In a never eneding loop */
    while (1) {
        /* Output our prompt and get input*/
        char* input = readline("lispy> ");

        /* Add input to history */
        add_history(input);

        /* Attempt to parse the user input */
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            /* On success print the AST */
            mpc_ast_print(r.output);
            lval result = eval(eval_h(r.output));
            lval_println(result);
            mpc_ast_delete(r.output);
        } else {
            /* Otherwise print the error */
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        /* Free retrieved input */
        free(input);

    }
    /* Undefine and Delete our Parses */
    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    return 0;
    }


