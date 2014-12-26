/*
Interactive Prompt REPL (read, evaluate, print, loop) styles.
Intended for Sababa Programming language, created by Dor Rondél i Gallego.
All rights reserved Copyright 2014-2015.
Licensed under GPL Affero 3.0 license.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpc.h"

//For windows compilation
#ifdef _WIN32
#include <string.h>

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
  integer    : /-?[0-9]+/ ;                               \
  operator   : '+' | '-' | '*' | '/' ;                        \
  expression : <integer> | '(' <operator> <expression>+ ')' ; \
  sababa     : /^/ <operator> <expression>+ /$/ ;             \
  ",
  Integer, Operator, Expression, Sababa);
  //end of REGEX code to define Sababa's grammer
  puts("Sababa Version: 0.0.0.2");
  puts("Press ctrl+c to exit");

  while(true){ //keep prompting the user in a similar fashion
    char* input = readline("Sababa> ");
    add_history(input);

    /* Attempt to Parse the user Input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Sababa, &r)) {
      /* If successful it'll print the abstract syntax tree */
      mpc_ast_print(r.output);
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
