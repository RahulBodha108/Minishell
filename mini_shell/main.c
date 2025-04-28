/***************************************************************************************************************************************************
 *Author	:RAHUL BODHA
 *Date		:WED 20 DEC 2024 14:04:05 IST
 *File		:main.c
 *Title		:MINISHELL
 ****************************************************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

char prompt[100] = "minishell$:- ";// Initialize prompt string
char input_string[100]; // Buffer to hold user input

int main() {

    // Clear the screen
    system("clear");
    scan_input(prompt, input_string);// Call function to handle user input

        
    return 0; // Exit program
}





