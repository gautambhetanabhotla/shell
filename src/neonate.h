#include <stdio.h>

#ifndef NEONATE_H
#define NEONATE_H

int neonate(char** args, FILE* istream, FILE* ostream);
extern struct termios ORIGINAL_TERM;

#endif