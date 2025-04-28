/***************************************************************************************************************************************************
 *Author	:RAHUL BODHA
 *Date		:WED 20 DEC 2024 14:04:05 IST
 *File		:SCAN_INPUT.C
 *Title		:MINISHELL
 ****************************************************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "header.h"

char *external_commands[200];
char command[100];
pid_t child_pid = -1;
sig_node *process_list = NULL;
int status;

void signal_handler(int signum) { // Signal handler function for handling SIGINT and SIGTSTP signals
    sig_node *current = process_list;// Pointer to traverse the process list

    if (signum == SIGINT) {// Handle Ctrl+C (SIGINT) signal
        if (child_pid == -1) { // Pointer to traverse the process lis
            // Print the prompt if the child is not running
            printf(ANSI_COLOR_BLUE "\nminishell$:- " ANSI_COLOR_RESET);// Print the prompt
            fflush(stdout); // Ensure output is flushed
        } else {  // If a child process is running
            // Child is running
            kill(child_pid, SIGINT);  // Send SIGINT to the child process
        }
    } else if (signum == SIGTSTP) {  // Handle Ctrl+Z (SIGTSTP) signal
        if (child_pid == -1) { // If no child process is running
            // Print the prompt if the child is not running
            printf(ANSI_COLOR_BLUE "\nminishell$:- " ANSI_COLOR_RESET);
            fflush(stdout);  // Ensure output is flushed
        } else {      // If a child process is running       
            // Child is running
            kill(child_pid, SIGTSTP);   // Send SIGTSTP to the child process
            // Store PID and command names in the linked list (SSL)
            add_process(&process_list, child_pid, command); // Add the stopped process to the process list
            child_pid = -1; // Reset child_pid since the process is now stopped
        }
    }
}

// Function to scan and process user input
void scan_input(char *prompt, char *input_string) {
    signal(SIGINT, signal_handler);// Register signal handler for SIGINT
    signal(SIGTSTP, signal_handler);  // Register signal handler for SIGTSTP

    extract_external_commands(external_commands);  // Populate the external commands array

    while (1) { // Infinite loop to continuously read user input
        printf(ANSI_COLOR_BLUE " %s" ANSI_COLOR_RESET, prompt);
        fgets(input_string, 100, stdin);// Read input from the user
        input_string[strcspn(input_string, "\n")] = '\0';

        if (strncmp(input_string, "PS1", 3) == 0) {  // Check for PS1 command
            if ((input_string[3] == ' ' && input_string[4] == '=') || strstr(input_string + 4, " ") != NULL) {
                printf(ANSI_COLOR_RED "Error: Prompt command cannot contain spaces between PS1 and =\n" ANSI_COLOR_RESET);
                printf(ANSI_COLOR_RED "Error: Prompt cannot contain spaces\n" ANSI_COLOR_RESET);
            } else if (strncmp(input_string, "PS1=", 4) == 0) {
                strcpy(prompt, input_string + 4);// Update the prompt
                strcat(prompt, "$:- "); // Append shell indicator
            }
            continue; // Skip to the next iteration
        }

        // Handling special echo cases
        if (strncmp(input_string, "echo ", 5) == 0) { // Handle echo command
            char *echo_arg = input_string + 5;  // Get the argument after echo

            if (strcmp(echo_arg, "$$") == 0) {
                 printf(ANSI_COLOR_GREEN "%d\n" ANSI_COLOR_RESET, getpid()); // Print the shell's PID
            } else if (strcmp(echo_arg, "$?") == 0) {
                printf(ANSI_COLOR_GREEN "%d\n" ANSI_COLOR_RESET, status);   // Print the last command's exit status
            } else if (strcmp(echo_arg, "$SHELL") == 0) {
                printf(ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, getenv("SHELL"));  // Print the shell environment variable
            } else {
                printf(ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, echo_arg); // Print the rest of the string
            }
            continue; // Skip to the next iteration as the command has been handled
        }

        char *cmd = get_command(input_string);
        if (cmd == NULL) {
            printf(ANSI_COLOR_RED "Command allocation failed\n" ANSI_COLOR_RESET);
            continue;
        }

        strncpy(command, input_string, sizeof(command)); // Save the command globally for signal_handler
        command[sizeof(command) - 1] = '\0';  // Ensure null termination
        int ret = check_command_type(cmd);  // Determine if the command is internal or external

        if (ret == EXTERNAL) { // If the command is external
            pid_t pid = fork(); // Create a child process
            child_pid = pid; // Store child process ID

            if (pid > 0) { // Parent process
                waitpid(pid, &status, WUNTRACED); // Wait for the child process to finish
                child_pid = -1; // Reset child_pid once the process is done
            } else if (pid == 0) { // Child process
                // Reset signal handlers to default behavior in child process
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                execute_external_commands(input_string); // Execute external command
                exit(EXIT_SUCCESS); // Ensure child exits after executing the command
            }
        } else if (ret == BUILTIN) {
            if (!strcmp(cmd, "bg")) { // Handle background job control
                printf(ANSI_COLOR_YELLOW "Handling background job control\n" ANSI_COLOR_RESET);;
                int job_id;
                if (sscanf(input_string + 3, "%%%d", &job_id) == 1) { // Include the `%` for job IDs
                    printf(ANSI_COLOR_YELLOW "Moving job %d to background\n" ANSI_COLOR_RESET, job_id);
                    sig_node *job = find_job_by_id(process_list, job_id);
                    if (job != NULL) {
                        move_to_background(&process_list, job->pid); // Move job to background
                    } else {
                       printf(ANSI_COLOR_RED "Invalid job ID %d for bg command\n" ANSI_COLOR_RESET, job_id);
                    }
                } else {
                   printf(ANSI_COLOR_RED "Invalid job ID for bg command\n" ANSI_COLOR_RESET);
                }
            } else if (!strcmp(cmd, "fg")) { // Handle foreground job control
                printf(ANSI_COLOR_YELLOW "Handling foreground job control\n" ANSI_COLOR_RESET);
                int job_id;
                if (sscanf(input_string + 3, "%%%d", &job_id) == 1) { // Include the `%` for job IDs
                    printf(ANSI_COLOR_YELLOW "Bringing job %d to foreground\n" ANSI_COLOR_RESET, job_id);
                    sig_node *job = find_job_by_id(process_list, job_id);
                    if (job != NULL) {
                        bring_to_foreground(&process_list, job->pid);  // Bring job to foreground 
                    } else {
                       printf(ANSI_COLOR_RED "Invalid job ID %d for fg command\n" ANSI_COLOR_RESET, job_id);
                    }
                } else {
                    printf(ANSI_COLOR_RED "Invalid job ID for fg command\n" ANSI_COLOR_RESET);
                }
            } else if (!strcmp(cmd, "jobs")) { // Display list of all jobs
                printf(ANSI_COLOR_CYAN "Displaying jobs\n" ANSI_COLOR_RESET);
                display_jobs(process_list); // List all jobs
            } else {
                execute_internal_commands(input_string, status);  // Execute internal command
            }
        } else { // Command not found
            printf(ANSI_COLOR_RED "No such command: %s\n" ANSI_COLOR_RESET, cmd);
        }
        free(cmd); // Free allocated memory
    }
}

char *get_command(char *input_string)
{
  // char *first_word = (char *)malloc(sizeof(char)*100);
  char *first_word = malloc(100); // Allocate memory for the command
   if (first_word == NULL) {  // Check for allocation failure
       printf(ANSI_COLOR_RED "Memory allocation failed\n" ANSI_COLOR_RESET);
      exit(EXIT_FAILURE);
      }

   // printf("Enter Command: \n");
   // fgets(input_string,100,stdin);
   // input_string[strcspn(input_string, "\n")] = '\0';

    sscanf(input_string, "%s", first_word); // Extract the first word from input
    //printf("Enter Command: \n");

    return first_word;// Return the command


}






