/***************************************************************************************************************************************************
 *Author	:RAHUL BODHA
 *Date		:WED 20 DEC 2024 14:04:05 IST
 *File		:EXTRACT_EXTERNAL_COMM.c
 *Title		:MINISHELL
 ****************************************************************************************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include "header.h"

void extract_external_commands(char **external_commands){ // Function to extract external commands from a file and store them in an array
  int fd =open("external_command.txt",O_RDONLY); // Open the external_command.txt file in read-only mode
  if(fd == -1) // Check if file opening failed
  {
    perror(ANSI_COLOR_RED "open failed" ANSI_COLOR_RESET); // Print error message
    exit(EXIT_FAILURE); // Exit the program with failure status
  }
  int i = 0, index = 0; // Read the file character by character
  char buf[50]={0}, ch;
  
  while (read(fd, &ch, 1) > 0){
    if (ch != '\n'){ // If the character is not a newline
      buf[index]=ch; // Add the character to the buffer
      index++;
    }else{ // If a newline is encountered
      buf[index] = '\0'; // Null-terminate the buffer string
      external_commands[i]=malloc(strlen(buf) + 1); // Allocate memory for the command
      if(external_commands[i] == NULL){ // Check if memory allocation failed
        perror(ANSI_COLOR_RED "malloc failed" ANSI_COLOR_RESET);// Print error message
        exit(EXIT_FAILURE);// Exit the program with failure status
      }
      strcpy(external_commands[i],buf); // Copy the command from buffer to the array
      //printf("%s\n",buf);
      index=0;// Reset index for the next command
      i++;
    }
  }
  external_commands[i] = NULL;// Null-terminate the external commands array
  close(fd); // Close the file
  printf(ANSI_COLOR_GREEN "External commands loaded successfully.\n" ANSI_COLOR_RESET);// Print success message
}