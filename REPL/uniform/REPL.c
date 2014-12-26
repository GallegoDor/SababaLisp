/*
Interactive Prompt REPL (read, evaluate, print, loop) styles.
Intended for Sababa Programming language, created by Dor Rondél i Gallego.
All rights reserved Copyright 2014-2015.
Licensed under the Apache License v2.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "mpc.h"

//min and max functions for later use, not built into C
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

//For windows compilation
#ifdef _WIN32

//buffer for user input up to 2048 characters
static char hueco[2048];

//fake editline and edit history
char* readline(char* prompt){
  fputs(prompt, stdout);
  fgets(hueco,2048,stdin);
  char* cpy = malloc(strlen(hueco)+1);
  strcpy(cpy,hueco);
  cpy[strlen(cpy)-1] = "\0";
  return cpy;
}

//fake add_history function
void add_history(char* unused){}

//For unix systems. Note may vary based on Linux distro and OSX version
//On arch linux for example, include histedit.h instead!
//Not all OSX versions come with editline.h in the Command Line Tools.
//But for now this will do.

#else
#include <editline/readline.h>
#endif

//Error Handling Portion
/* Create Enumeration of Possible Error Types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Create Enumeration of Possible lval Types (lval = lisp value) */
enum { LVAL_NUM, LVAL_ERR };

/* Declare New lval Struct */
typedef struct {
  int type;
  long num;
  int err;
} lval;

/* Create a new number type lval */
lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

/* Create a new error type lval */
lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

/* Print an "lval" */
void lval_print(lval v) {
  switch (v.type) {
    /* In the case the type is an integer, print it */
    /* Then 'break' out of the switch. */
    case LVAL_NUM: printf("%li", v.num); break;

    /* In the case the type is an error */
    case LVAL_ERR:
    /* Check what type of error it is and print it */
    if (v.err == LERR_DIV_ZERO) {
      printf("Error: Division By Zero!");
    }
    if (v.err == LERR_BAD_OP)   {
      printf("Error: Invalid Operator!");
    }
    if (v.err == LERR_BAD_NUM)  {
      printf("Error: Invalid Number!");
    }
    break;
  }
}

/* Print an "lval" followed by a newline */
void lval_println(lval v) { lval_print(v); putchar('\n'); }

//End error Handling portion

//Start arimethic code, and language grammar

/*  expression is *10 (+ 1 51) How it's represented in ast
regex
operator|char:1:1 '*'               <-- operator
expr|number|regex:1:3 '10'          <-- int
expr|>                              <-- node with branches
  char:1:6 '('                      <-- index 0 can ignore
  operator|char:1:7 '+'             <-- operator always index 1
  expr|number|regex:1:9 '1'         <-- saved as sum
  expr|number|regex:1:11 '51'       <-- added to create new x through recursion
  char:1:13 ')'                     <-- non-number may ignore
regex
*/

/* Uses strcmp to check for operator, return 0 if true */
// then evaluates x and y arguments
lval eval_op(lval x, char* op, lval y) {

  /* If either value is an error return it */
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }

  if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num);    }
  if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num);    }
  if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num);    }
  if (strcmp(op, "^") == 0) { return lval_num(pow(x.num,y.num)); }

  if (strcmp(op, "/") == 0) {
    /* If second operand is zero return error */
    return y.num == 0
    ? lval_err(LERR_DIV_ZERO)
    : lval_num(x.num / y.num);
  }

  if (strcmp(op, "%") == 0) {
    /* Same reasoning as above */
    return y.num == 0
    ? lval_err(LERR_DIV_ZERO)
    : lval_num(x.num % y.num);
  }
  return lval_err(LERR_BAD_OP);
}

//function to obtain numbers to evaluate
lval eval(mpc_ast_t* t) {
  /* If tagged as integer return it directly. */
  if (strstr(t->tag, "integer")) {
    /* Check if there is some error in conversion */
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  /* The operator is always second child. Since first is always '(' */
  char* op = t->children[1]->contents;

  /* We store the third child in `x` */
  lval sum = eval(t->children[2]);

  /* Iterate the remaining children and operate on them accordingly. */
  int i = 3;
  while (strstr(t->children[i]->tag, "expression")) {
    sum = eval_op(sum, op, eval(t->children[i]));
    i++;
  }
  return sum;
}

//end math operations functionality, below syntax and grammar is defined

int main(int argc, char** argv){
  //start of REGEX code for defining Sababa's grammer
  /* Parser creation */
  mpc_parser_t* Integer   = mpc_new("integer");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expression = mpc_new("expression");
  mpc_parser_t* Sababa = mpc_new("sababa");

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
  "                                                           \
  integer    : /-?[0.0-9.9]+/ ;                               \
  operator   : '+' | '-' | '*' | '/'| '%' | '^' ;             \
  expression : <integer> | '(' <operator> <expression>+ ')' ; \
  sababa     : /^/ <operator> <expression>+ /$/ ;             \
  ",
  Integer, Operator, Expression, Sababa);
  //end of REGEX code to define Sababa's grammer
  puts("Sababa Version: 0.0.0.4");
  puts("Press ctrl+c to exit");

  while(true){ //keep prompting the user in a similar fashion
    char* input = readline("Sababa> ");
    add_history(input);

    /* Attempt to Parse the user Input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Sababa, &r)) {
      /* If successful it'll print the result of the calculation */
      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    } else {
      /* Otherwise it'll print an error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    free(input);
  }

  /* Get rid of the parsers */
  mpc_cleanup(4, Integer, Operator, Expression, Sababa);
  return 0;
}
