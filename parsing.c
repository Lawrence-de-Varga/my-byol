#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"
#include "mmath.c"
#include "print.c"

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
    mpc_parser_t* Integer   = mpc_new("integer");
    mpc_parser_t* Double    = mpc_new("double");
    mpc_parser_t* Symbol    = mpc_new("symbol");
    mpc_parser_t* Sexpr     = mpc_new("sexpr");
    mpc_parser_t* Qexpr     = mpc_new("qexpr");
    mpc_parser_t* Expr      = mpc_new("expr");
    mpc_parser_t* Lispy     = mpc_new("lispy");


    /* Define them with the following langauge */
    mpca_lang(MPCA_LANG_DEFAULT,
            "                                                                   \
                integer : /-?[0-9]+/;                                           \
                double  : /-?[0-9]+\\.[0-9]+/;                                  \
                symbol  : '+' | '-' | '*' | '/' | '%' | '^' | /min/ | /max/     \
                          | \"list\" | \"car\" | \"cdr\" | \"cons\"             \
                          | \"join\" | \"eval\" | \"len\";                                \
                sexpr   : '(' <expr>* ')';                                      \
                qexpr   : '{' <expr>* '}' ;                                     \
                expr    : <double> | <integer> | <symbol> | <sexpr> | <qexpr> ; \
                lispy   : /^/ <expr>* /$/;                                      \
            ",
            Integer, Double, Symbol, Sexpr, Qexpr, Expr, Lispy);


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

            mpc_ast_print(eval_h(r.output));
//            lval* x = lval_read(eval_h(r.output));
            lval* x = lval_eval(lval_read(eval_h(r.output)));
            lval_println(x);
            lval_del(x);
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
    mpc_cleanup(7, Integer, Double, Symbol, Sexpr, Qexpr, Expr, Lispy);
    return 0;
    }


