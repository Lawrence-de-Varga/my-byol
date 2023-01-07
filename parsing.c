#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

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
                operator  : '+' | '-' | '*' | '/' | '%';           \
                expr      : <number> | '(' <operator> <expr>+ ')'; \
                lispy     : /^/ <operator> <expr>+ /$/;            \
            ",
            Number, Operator, Expr, Lispy);

    /* Print version and exit information */
    puts("Lispy Version 0.0.0.1");
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




    
