/***************************************************************************************************************************************************
 *Author	:RAHUL BODHA
 *Date		:WED 20 DEC 2024 14:04:05 IST
 *File		:EXECUTE_COMMANDS.c
 *Title		:MINISHELL
 ****************************************************************************************************************************************************/
#include "header.h" 
#include <string.h> 
#include <stdlib.h>



void execute_external_commands(char *input) { 
    char *argv[30]; // Array to hold command arguments
    int pos = 0;// Position in the argv array
    char *token = strtok(input, " "); // Tokenize the input string by spaces
    while (token != NULL) { // Loop through all tokens
        argv[pos] = malloc(20); // Allocate memory for the argument
        strcpy(argv[pos], token); // Copy the token into the argv array
        token = strtok(NULL, " "); // Get the next token
        pos++;
    }
    argv[pos] = NULL; // Null-terminate the argv array

    // Check for the pipe
    int found_pipe = 0;
    for (int i = 0; i < pos; i++) { // Loop through arguments to check for pipe
        if (strcmp(argv[i], "|") == 0) { // If pipe is found
            found_pipe = 1; // Set found_pipe flag
            break; // Exit the loop
        }
    }

    if (!found_pipe) {
        // No pipe found, execute the command directly
        printf(ANSI_COLOR_GREEN "Executing command: %s\n" ANSI_COLOR_RESET, argv[0]);
        if (execvp(argv[0], argv) == -1) { // Execute the command
            perror(ANSI_COLOR_RED "execvp failed" ANSI_COLOR_RESET); // Print error if execvp fails
        }
    } else {
        // Pipe found, handle piping
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) { // Create a pipe
            perror(ANSI_COLOR_RED "pipe failed" ANSI_COLOR_RESET); // Print error if pipe fails
            exit(EXIT_FAILURE); // Exit with failure
        }

        pid_t pid = fork(); // Fork the process
        if (pid == -1) { // If fork fails
            perror(ANSI_COLOR_RED "fork failed" ANSI_COLOR_RESET); // Print error if fork fails
            exit(EXIT_FAILURE); // Exit with failure
        } else if (pid == 0) { 
            // Child process for the first command
            printf(ANSI_COLOR_GREEN "Executing first command in pipe: %s\n" ANSI_COLOR_RESET, argv[0]);
            dup2(pipe_fd[1], 1);  // Redirect output to pipe
            close(pipe_fd[0]); // Close unused read end
            close(pipe_fd[1]); // Close write end after duplication

            char *first_command[30]; // Array for the first command
            int i;
            for (i = 0; strcmp(argv[i], "|") != 0; i++) { // Loop until the pipe symbol
                first_command[i] = argv[i];
            }
            first_command[i] = NULL; // Null-terminate the array

            execvp(first_command[0], first_command); // Execute the first command
            perror("execvp failed"); // Print error if execvp fails
            exit(EXIT_FAILURE); // Exit with failure
        } else {
            // Parent process for the second command
            printf(ANSI_COLOR_GREEN "Executing second command in pipe: %s\n" ANSI_COLOR_RESET, argv[2]);
            dup2(pipe_fd[0], 0);  // Redirect input from pipe
            close(pipe_fd[0]); // Close read end after duplication
            close(pipe_fd[1]); // Close unused write end

            char *second_command[30]; // Array for the second command
            int i = 0, j;
            for (j = 0; strcmp(argv[j], "|") != 0; j++); // Find the pipe symbol
            j++; // Move to the next argument after the pipe
            while (argv[j] != NULL) { // Loop through remaining arguments
                second_command[i++] = argv[j++];  // Copy arguments to second_command
            }
            second_command[i] = NULL; // Null-terminate the array

            execvp(second_command[0], second_command); // Execute the second command
            perror(ANSI_COLOR_RED "execvp failed" ANSI_COLOR_RESET); // Print error if execvp fails
            exit(EXIT_FAILURE); // Exit with failure
        }
    }

    for (int i = 0; i < pos; i++) {
        free(argv[i]); // Free allocated memory
    }
}


void execute_internal_commands(char *input_string, int status) {
    if (strcmp(input_string, "exit") == 0) { // Handle exit command
        exit(0);
    } else if (strncmp(input_string, "cd", 2) == 0) { // Handle cd command
        char *dir = NULL;
        for (int i = 2; i < strlen(input_string); i++) { // Loop to find directory argument
            if (input_string[i] != ' ') {
                dir = &input_string[i]; // Set dir to the argument
                break;
            }
        }
        if (dir != NULL) { // If directory argument is found
            if (chdir(dir) != 0) { // Change directory
                perror(ANSI_COLOR_RED "chdir failed" ANSI_COLOR_RESET); // Print error if chdir fails }
            }
        } else {
            fprintf(stderr, ANSI_COLOR_RED "cd: missing operand\n" ANSI_COLOR_RESET); // Print error if no operand
        }
    } else if (strncmp(input_string, "pwd", 3) == 0) { // Handle pwd command
        char buf[100];
        if (getcwd(buf, sizeof(buf)) != NULL) { // Get current working directory
            printf(ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, buf); // Print the working directory
        } else {
            perror(ANSI_COLOR_RED "getcwd failed" ANSI_COLOR_RESET);// Print error if getcwd fails
        }
    } else {
        fprintf(stderr, ANSI_COLOR_RED "Unknown internal command: %s\n" ANSI_COLOR_RESET, input_string); // Print error for unknown command
    }
}
