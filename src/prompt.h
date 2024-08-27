#include <stdbool.h>
#include <stdio.h>

#ifndef PROMPT_H
#define PROMPT_H

#define MAX_COMMAND_LENGTH 1024

extern char *HOME_DIRECTORY, *USERNAME, *HOSTNAME, *CURRENT_DIRECTORY, *CURRENT_DIRECTORY_CONVERTED;
extern int (*USER_FUNCTIONS[])(char**, FILE*, FILE*);
extern char* COMMAND_STRINGS[];
extern int SHELL_PID;

char *convert_path(char*, char*, bool);
void prompt();
void init_shell();
void execute(char**);
int exit_shell(char**, FILE*, FILE*);

#endif