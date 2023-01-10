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

long eval_op_1(char* op, long x) {
    if (strcmp(op, "+") == 0) {return x;}
    if (strcmp(op, "-") == 0) {return -x;}
    if (strcmp(op, "*") == 0) {return x;}
    if (strcmp(op, "/") == 0) {return 1/x;}
    if (strcmp(op, "%") == 0) {return x;}
    if (strcmp(op, "^") == 0) {return expo(x, x);}
    if (strcmp(op, "min") == 0) {return x;}
    if (strcmp(op, "max") == 0) {return x;}
    return 0;
}

long eval_op_n(long x, char* op, long y) {
    printf("X from eval_op: %ld\n", x);
    printf("Y from eval_op: %ld\n", y);
    if (strcmp(op, "+") == 0) {return x + y;}
    if (strcmp(op, "-") == 0) {return x - y;}
    if (strcmp(op, "*") == 0) {return x * y;}
    if (strcmp(op, "/") == 0) {return x / y;}
    if (strcmp(op, "%") == 0) {return x % y;}
    if (strcmp(op, "^") == 0) {return expo(x, y);}
    if (strcmp(op, "min") == 0) {return min(x, y);}
    if (strcmp(op, "max") == 0) {return max(x, y);}
    return 0;
}


long eval(mpc_ast_t* t) {

    /* If tagged as number return it directly */
    if (strstr(t->tag, "number")) {
        return atoi(t->contents);
    }

    /* The operator is always the second child. */
    char* op = t->children[1]->contents;
    printf("OP: %s\n", op);

    /* We store the third child in x. */
    long x = eval(t->children[2]);
    printf("X: %ld\n", x);
    
    printf("TAG: %s\n", t->children[3]->tag);
    if ((strstr (t->children[3]->tag, "regex")) && !(strstr(t->children[3]->tag, "number"))) {
        return eval_op_1(op, x);
    }

    /* iterate over and combine the remaining childnre */
    int i = 3;
    printf("5th item in ast: %s\n", t->children[i]->tag);
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
                lispy     : /^/  <operator> <expr>+  /$/;            \
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
            long result = eval(r.output);
            printf("%li\n", result);
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


