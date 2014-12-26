/*
Interactive Prompt REPL (read, evaluate, print, loop) styles.
Intended for Sababa Programming language, created by Dor Rondél i Gallego.
All rights reserved Copyright 2014-2015.
Licensed under the Eclipse Public License v1.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "mpc.h"

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
long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y;    }
  if (strcmp(op, "-") == 0) { return x - y;    }
  if (strcmp(op, "*") == 0) { return x * y;    }
  if (strcmp(op, "/") == 0) { return x / y;    }
  if (strcmp(op, "%") == 0) { return x % y;    }
  if (strcmp(op, "^") == 0) { return pow(x,y); }
  return 0;
}

//function to obtain numbers to evaluate
long eval(mpc_ast_t* t) {
  /* If tagged as integer return it directly. */
  if (strstr(t->tag, "integer")) {
    return atoi(t->contents); //turn string contents into a long
  }

  /* The operator is always second child. Since first is always '(' */
  char* op = t->children[1]->contents;

  /* We store the third child in `x` */
  long sum = eval(t->children[2]);

  /* Iterate the remaining children and operate on them accordingly. */
  int i = 3;
  while (strstr(t->children[i]->tag, "expression")) {
    sum = eval_op(sum, op, eval(t->children[i]));
    i++;
  }
  return sum;
}

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
  puts("Sababa Version: 0.0.0.3");
  puts("Press ctrl+c to exit");

  while(true){ //keep prompting the user in a similar fashion
    char* input = readline("Sababa> ");
    add_history(input);

    /* Attempt to Parse the user Input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Sababa, &r)) {
      /* If successful it'll print the result of the calculation */
      long result = eval(r.output);
      printf("%li\n", result);
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
