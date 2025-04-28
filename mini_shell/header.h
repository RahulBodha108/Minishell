#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define BUILTIN    1
#define EXTERNAL   2
#define NO_COMMAND 3

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct Node { 
    char command[50]; 
    int pid; 
    struct Node *link; 
} sig_node;

extern sig_node *process_list;
extern pid_t child_pid;
//extern char command[100];

void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);

void copy_change(char *prompt, char *input_string);

int check_command_type(char *command);
void echo(char *input_string, int status);
void execute_internal_commands(char *input_string, int status);
void execute_external_commands(char *input_string);
void signal_handler(int sig_num);
void extract_external_commands(char **external_commands);
int execute_pipe(int argc, char *argv[]);
void execute_pipe_commands(char *commands[], int command_count);
void add_process(sig_node **head, int pid, char *command);
void remove_process(sig_node **head, int pid);
void display_jobs(sig_node *head);
void bring_to_foreground(sig_node **head, int pid);
void move_to_background(sig_node **head, int pid);
sig_node* find_job_by_id(sig_node *head, int job_id);

#endif
