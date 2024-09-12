#include <stdio.h>

#ifndef REVEAL_H
#define REVEAL_H

struct directory;

int reveal(char** args);
char* name_from_path(char* input);

#endif