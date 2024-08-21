#include <stdbool.h>
#include <stdio.h>

#ifndef PROMPT_H
#define PROMPT_H

extern char *HOME_DIRECTORY, *USERNAME, *HOSTNAME, *CURRENT_DIRECTORY, *CURRENT_DIRECTORY_CONVERTED;
extern int (*USER_FUNCTIONS[])(char**);
extern char* COMMAND_STRINGS[];

char *convert_path(char* input_string, char* home_dir, bool free_input), *current_directory();
void prompt();
void init_sysvars();
void execute(char** args);

#endif