#include <stdio.h>

#ifndef NEONATE_H
#define NEONATE_H

int neonate(char** args);
extern struct termios ORIGINAL_TERM;
int most_recent_pid();

#endif