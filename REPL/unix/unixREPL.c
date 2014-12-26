#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <editline/readline.h>

/*
Interactive Prompt REPL (read, evaluate, print, loop) styles.
Intended for Sababa Programming language, created by Dor Rondél i Gallego.
All rights reserved Copyright 2014-2015.
Licensed under GPL Affero 3.0 license.
*/

int main(int argc,char** argv){
  puts("Sababa Version 0.0.0.1");
  puts("press ctrl+c to exit");

  //will always respond accordingly
  while(true){
    char* input = readline("Sababa> ");
    add_history(input);
    printf("%s\n", input);
    free(input);
  }
  return 0;
}
