/***************************************************************************************************************************************************
 *Author	:RAHUL BODHA
 *Date		:WED 20 DEC 2024 14:04:05 IST
 *File		:PROCESS_LIST.C
 *Title		:MINISHELL
 ****************************************************************************************************************************************************/
#include "header.h"


void add_process(sig_node **head, int pid, char *command) {
    sig_node *new_node = (sig_node *)malloc(sizeof(sig_node)); // Allocate memory for a new node
    if (!new_node) { // Check if memory allocation failed
        perror("malloc failed"); // Print error message
        exit(EXIT_FAILURE); // Exit the program with failure status
    }
    strcpy(new_node->command, command); // Copy the command to the new node
    new_node->pid = pid; // Assign the process ID to the new node
    new_node->link = *head; // Point the new node to the current head
    *head = new_node; // Update the head to the new node
    printf("Added process: %d, command: %s\n", pid, command); // Debugging statement
}
void remove_process(sig_node **head, int pid) { // Function to remove a process from the linked list of processes
    sig_node *temp = *head, *prev = NULL; // Initialize pointers for traversal
    while (temp != NULL && temp->pid != pid) { // Traverse the list until the matching PID is found
        prev = temp;
        temp = temp->link; // Move to the next node
    }
    if (temp == NULL) return; // If no matching node is found, exit the function
    if (prev == NULL) *head = temp->link; // If the head node is to be removed
    else prev->link = temp->link; // Free the memory of the removed node
    free(temp);  // Free the memory of the removed node
    printf("Removed process: %d\n", pid); // Debugging statement
}
void display_jobs(sig_node *head) {  // Function to display the list of jobs (processes) in the linked list
    sig_node *current = head; // Pointer to traverse the list
    int job_id = 1; // Initialize job ID counter
    while (current != NULL) { // Traverse the list
        printf(ANSI_COLOR_GREEN "[%d] %s (PID: %d)\n" ANSI_COLOR_RESET, job_id, current->command, current->pid); // Print job details
        current = current->link;  // Move to the next node
        job_id++; // Increment the job ID
    }
}
void bring_to_foreground(sig_node **head, int pid) {
    if (*head == NULL) { // If the list is empty
        fprintf(stderr, ANSI_COLOR_RED "No jobs to bring to foreground\n" ANSI_COLOR_RESET);
        return;
    }

    sig_node *current = *head;  // Pointer to traverse the list
    while (current != NULL) {  // Traverse the list
        if (current->pid == pid) {  // If the matching PID is found
            tcsetpgrp(STDIN_FILENO, pid); // Set terminal control to the process
            kill(pid, SIGCONT); // Continue the process

            int status;
            waitpid(pid, &status, WUNTRACED); // Wait for the process

            tcsetpgrp(STDIN_FILENO, getpgrp()); // Reset terminal control to the shell
            printf("Process %d moved to foreground\n", pid);

            // If the process was stopped again, update job list
            if (WIFSTOPPED(status)) {
                printf("Process %d was stopped again\n", pid);
            } else {
                remove_process(head, pid); // Remove the process from the list if it completed
            }
            return;
        }
        current = current->link; // Move to the next node
    }
    fprintf(stderr, ANSI_COLOR_RED "No such process: %d\n" ANSI_COLOR_RESET, pid);// Print error if PID not found
}


// Function to move a process to the background
void move_to_background(sig_node **head, int pid) {
    sig_node *current = *head;  // Pointer to traverse the list
    while (current != NULL) { // Traverse the list
        if (current->pid == pid) {  // If the matching PID is found
            kill(pid, SIGCONT); // Continue the process
            printf("Process %d moved to background\n", pid); // Print success message
            return;
        }
        current = current->link; // Move to the next node
    }
    fprintf(stderr, ANSI_COLOR_RED "No such process: %d\n" ANSI_COLOR_RESET, pid); // Print error if PID not found
}
sig_node* find_job_by_id(sig_node *head, int job_id) { // Function to find a job by its ID in the linked list
    sig_node *current = head;   // Pointer to traverse the list
    int id = 1; // Initialize job ID counter
    while (current != NULL) {   // Traverse the list
        if (id == job_id) { // If the matching job ID is found
            return current; // Return the matching node
        }
        current = current->link;  // Return the matching node
        id++;// Increment the job ID
    }
    return NULL;// Return NULL if no matching job ID is fou
}

