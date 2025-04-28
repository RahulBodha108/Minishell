/***************************************************************************************************************************************************
 *Author	:RAHUL BODHA
 *Date		:WED 20 DEC 2024 14:04:05 IST
 *File		:CHECK_COMMAND.c
 *Title		:MINISHELL
 ****************************************************************************************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> // Unix standard functions library
#include <sys/wait.h>  // Library for process control functions
#include "header.h"

extern char *external_commands[200];// Declare external command array
char *builtin_commands[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
                        "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
                        "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "bg", "fg", "jobs", NULL
            };// Array of built-in commands


int check_command_type(char *command) // Function to check the type of command (external or built-in)
{
 // printf("------\n");
  for(int i=0;external_commands[i] != NULL;i++){ // Loop through external commands
   // printf("___%s______%s____\n",command,external_commands[i]);
    if (strncmp(command, external_commands[i],strlen(command)) == 0) { // Compare command with each external command in the array
    return EXTERNAL; // If match found, return EXTERNAL
  }
}
  //  printf("------\n"); // Loop through built-in commands
  for(int i=0;builtin_commands[i] != NULL;i++){ // Compare command with each built-in command in the array
    if(strcmp(command, builtin_commands[i])== 0){
      return BUILTIN;// If match found, return BUILTIN
    }
  }
   return NO_COMMAND; // If no match found, return NO_COMMAND
}



