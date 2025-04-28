/***************************************************************************************************************************************************
 *Author	:RAHUL BODHA
 *Date		:WED 20 DEC 2024 14:04:05 IST
 *File		:EXECUTE_PIPE.c
 *Title		:MINISHELL
 ****************************************************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Function to execute a sequence of piped commands
void execute_pipe(char *commands[], int command_count) { 
    int fd[2];  // File descriptors for the pipe
    pid_t pid;
    int fd_in = 0;  // Input file descriptor for the next command

    for (int i = 0; i < command_count; i++) { // Loop through each command in the sequence
        pipe(fd); // Create a pipe

        if ((pid = fork()) == -1) { // Fork the process
            perror("fork"); // Handle fork failure
            exit(EXIT_FAILURE); // Exit with failure status
        } else if (pid == 0) {
            // Child process
            dup2(fd_in, 0);  // Change the input according to the old one
            if (i < command_count - 1) {
                dup2(fd[1], 1);  // Change the output to the new pipe
            }
            close(fd[0]); // Close unused read end of the pipe

            // Prepare arguments for execvp
            char *argv[30]; // Array to hold command arguments
            char *token = strtok(commands[i], " "); // Tokenize the command by spaces
            int pos = 0;
            while (token != NULL) {
                argv[pos] = malloc(strlen(token) + 1); // Allocate memory for the argument
                strcpy(argv[pos], token); // Copy the token into argv array
                token = strtok(NULL, " "); // Get the next token
                pos++;
            }
            argv[pos] = NULL; // Null-terminate the argv array

            execvp(argv[0], argv); // Execute the command
            perror("execvp failed"); // Handle execvp failure
            exit(EXIT_FAILURE); // Exit with failure status
        } else {
            // Parent process
            wait(NULL); // Wait for the child process to finish
            close(fd[1]); // Close unused write end of the pipe
            fd_in = fd[0];  // Save the input for the next command
        }
    }
}


