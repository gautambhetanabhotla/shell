#include <signal.h>

#ifndef BG_H
#define BG_H

void sigchld_handler(int signo, siginfo_t* siginfo, void* ucontext);
extern struct sigaction act;
extern char* bg_process_strings[];

#endif