/*
Interactive Prompt REPL (read, evaluate, print, loop) styles.
Intended for Sababa Programming language, created by Dor Rondél i Gallego.
All rights reserved Copyright 2014-2015.
Licensed under GPL Affero 3.0 license.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
  puts("Sababa Version: 0.0.0.1");
  puts("Press ctrl+c to exit");

  while(true){ //keep prompting the user in a similar fashion
    char* input = readline("Sababa> ");
    add_history(input);
    printf("%s\n", input);
    free(input);
  } return 0;
}
