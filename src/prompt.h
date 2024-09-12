#include <stdbool.h>
#include <stdio.h>

#ifndef PROMPT_H
#define PROMPT_H

#define MAX_COMMAND_LENGTH 4096

extern char *HOME_DIRECTORY, *USERNAME, *HOSTNAME, *CURRENT_DIRECTORY, *CURRENT_DIRECTORY_CONVERTED;
extern int (*USER_FUNCTIONS[])(char** args);
extern char* COMMAND_STRINGS[];
extern int SHELL_PID;
extern int FG_PID;

char *convert_path(char* input_string, char* home_dir, bool free_input);
void prompt();
void init_shell();
void execute(char** args, bool background);
int exit_shell(char** args);

#endif