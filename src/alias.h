#include <stdbool.h>

#ifndef ALIAS_H
#define ALIAS_H

struct Alias {
    char *name, *replacement;
};

int alias(char** args);
char* replace(char* input, struct Alias* aliases, bool free_input);
void purge_aliases();

extern struct Alias* ALIASES;
extern int N_ALIASES;

#endif