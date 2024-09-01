#include <signal.h>

#ifndef BG_H
#define BG_H

void handler(int signo, siginfo_t* siginfo, void* ucontext);
extern struct sigaction act;
extern char* strings[];

#endif