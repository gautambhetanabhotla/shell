#include <stdio.h>

#ifndef REVEAL_H
#define REVEAL_H

struct directory;

int reveal(char** args, FILE* istream, FILE* ostream);
char* name_from_path(char* input);

#endif