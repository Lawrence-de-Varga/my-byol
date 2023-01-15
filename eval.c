#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"

// Used for basic debuging
void printast(mpc_ast_t* ast) {

    printf("TAG t: %s\n", ast->tag);
    printf("CONTENTS t: %s\n", ast->contents);
    printf("CHILDREN_NUM t: %i\n\n", ast->children_num);
}


// Used to evaluate expressions of one symbol and one arguments
// such as (- 5) or (/ 5)
//lval eval_op_1(char* op, lval x) {
//
//    puts("Entering eval_op_1");
//
//    if (x.type == LVAL_ERR) { return x;}
//
//    if (strcmp(op, "+") == 0) {return lval_num(x.num);}
//    if (strcmp(op, "-") == 0) {return lval_num(-x.num);}
//    if (strcmp(op, "*") == 0) {return lval_num(x.num);}
//    if (strcmp(op, "/") == 0) {return lval_num(1/x.num);}
//    if (strcmp(op, "%") == 0) {return lval_num(x.num);}
//    if (strcmp(op, "^") == 0) {return lval_num(expo(x.num, x.num));}
//    if (strcmp(op, "min") == 0) {return lval_num(x.num);}
//    if (strcmp(op, "max") == 0) {return lval_num(x.num);}
//    return lval_err(LERR_BAD_OP);
//}
//
//// Main symbol evaluation rule
//lval eval_op_n(lval x, char* op, lval y) {
//
//    puts("Entering eval_op_n");
//
//    if (x.type == LVAL_ERR) { return x;}
//    if (y.type == LVAL_ERR) { return y;}
//
//    if (strcmp(op, "+") == 0) {return lval_num(x.num + y.num);}
//    if (strcmp(op, "-") == 0) {return lval_num(x.num - y.num);}
//    if (strcmp(op, "*") == 0) {return lval_num(x.num * y.num);}
//    if (strcmp(op, "/") == 0) 
//        {return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);}
//    if (strcmp(op, "%") == 0) {return lval_num(x.num % y.num);}
//    if (strcmp(op, "^") == 0) {return lval_num(expo(x.num, y.num));}
//    if (strcmp(op, "min") == 0) {return lval_num(min(x.num, y.num));}
//    if (strcmp(op, "max") == 0) {return lval_num(max(x.num, y.num));}
//
//    return lval_err(LERR_BAD_OP);
//}

// eval_h simply allows us to get straight at the expression
// in the ast that we want ignoring the '>' and 'regex' tags
mpc_ast_t* eval_h(mpc_ast_t* t) {
    return t->children[1];
}

// Evaluates any valid expression and returns an lval (currently only a number or 
// an error)
//lval eval(mpc_ast_t* t) {
//
//    puts("Entering eval");
//
//    /* If tagged as number return it directly */
//    if (strstr(t->tag, "integer")) {
//        errno = 0;
//        long x = strtol(t->contents, NULL, 10);
//        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
//    }
//
//    /* The symbol is always the second child. */
//    char* op = t->children[1]->contents;
//
//    /* We store the third child in x. */
//    lval x = eval(t->children[2]);
//
//    // if the user gives (- 5) or (+ 5) a result will be returned by eval_op_1
//    if ((strstr (t->children[3]->tag, "char")) && !(strstr(t->children[3]->tag, "symbol"))) {
//        return eval_op_1(op, x);
//    }
//    /* iterate over and combine the remaining childnre */
//    int i = 3;
//    while (strstr(t->children[i]->tag, "expr")) {
//        x = eval_op_n(x, op, eval(t->children[i]));
//        i++;
//    }
//
//    return x;
//}

