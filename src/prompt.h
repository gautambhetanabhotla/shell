#include <stdbool.h>
#include <stdio.h>

#include "parser.h"

#ifndef PROMPT_H
#define PROMPT_H

#define MAX_COMMAND_LENGTH 4096

extern char *HOME_DIRECTORY, *USERNAME, *HOSTNAME, *CURRENT_DIRECTORY, *CURRENT_DIRECTORY_CONVERTED;
extern int (*CHILD_FUNCTIONS[])(char** args);
extern char* CHILD_STRINGS[];
extern int SHELL_PID;
extern int FG_PID;
extern char MOST_RECENT_FG_PROCESS[];

void prompt();
void init_shell();
void execute(struct command command, char** args, int number, int (*pipes)[2]);
int exit_shell(char** args);

#endif