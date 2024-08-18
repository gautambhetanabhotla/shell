#include <stdbool.h>

#ifndef PROMPT_H
#define PROMPT_H

char *convert_path(char* input_string, char* home_dir, bool free_input), *current_directory();
void prompt(char* current_dir);
void init_sysvars();

#endif
