#include <stdio.h>

#ifndef PING_H
#define PING_H

int ping(char** args, FILE* istream, FILE* ostream);
void set_handlers();

#endif