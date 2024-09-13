#include "ping.h"
#include "prompt.h"
#include "background.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <stdio.h>

extern struct sigaction act_sigchld;

int ping(char** args) {
    if(args == NULL || args[0] == NULL || args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "Invalid usage of ping!\n");
        return -1;
    }
    int pid = atoi(args[1]);
    int sig_no = atoi(args[2]);

    kill(pid, sig_no % 32);
    return 0;
}

void handle_sig_int() {
    // ctrl c
    // fprintf(stderr, "PID %d received SIGINT - Shell PID = %d\n", getpid(), SHELL_PID);
}

void handle_sig_quit() {
    // ctrl d
    exit_shell(NULL);
}

void handle_sig_tstp() {
    // ctrl z
    if(FG_PID) {
        // kill(FG_PID, SIGTSTP);
        setpgid(FG_PID, FG_PID);
        tcsetpgrp(0, FG_PID);
        FG_PID = 0;
    }
}

void set_handlers() {
    // act_sigchld.sa_sigaction = sigchld_handler;
    // act_sigchld.sa_flags = SA_SIGINFO;
    // sigaction(SIGCHLD, &act_sigchld, NULL);

    signal(SIGINT, handle_sig_int);
    // signal(SIGQUIT, handle_sig_quit);
    // signal(SIGTSTP, handle_sig_tstp);
}