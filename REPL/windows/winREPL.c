#include <stdio.h>
#include <stdbool.h>

/*
Interactive Prompt REPL (read, evaluate, print, loop) styles.
Intended for Sababa Programming language, created by Dor Rondél i Gallego.
All rights reserved Copyright 2014-2015.
Licensed under GPL Affero 3.0 license.
*/

static char input[2048];

int main(int argc,char** argv){
  puts("Sababa Version 0.0.0.1");
  puts("press ctrl+c to exit");

  //will always respond accordingly
  while(true){
      fputs("lispy> ", stdout);
      fgets(input, 2048, stdin);
      printf("%s\n", input);
  }
  return 0;
}
