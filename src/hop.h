#include <stdbool.h>
#include <stdio.h>

#ifndef HOP_H
#define HOP_H

extern char* LAST_DIRECTORY;

int hop(char** args, FILE* istream, FILE* ostream);
char* convert_path_back(char* input_string, bool free_input);

#endif