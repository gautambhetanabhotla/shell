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
    if(FG_PID) {
        kill(FG_PID, SIGINT);
        FG_PID = 0;
    }
}

void handle_sig_tstp() {
    // ctrl z
    // fprintf(stderr, "RECEIVED SIG_TSTP\n");
    if(FG_PID) {
        bg_process_strings[FG_PID] = "hlo";
        setpgid(FG_PID, FG_PID);
        // tcsetpgrp(STDIN_FILENO, FG_PID);
        // tcsetpgrp(STDOUT_FILENO, FG_PID);
        // tcsetpgrp(STDERR_FILENO, FG_PID);
        kill(FG_PID, SIGTSTP);
    }
    else {
        // tcsetpgrp(STDIN_FILENO, FG_PID);
        // tcsetpgrp(STDOUT_FILENO, FG_PID);
        // tcsetpgrp(STDERR_FILENO, FG_PID);
    }
}

void set_handlers() {
    // act_sigchld.sa_sigaction = sigchld_handler;
    // act_sigchld.sa_flags = SA_SIGINFO;
    // sigaction(SIGCHLD, &act_sigchld, NULL);

    signal(SIGINT, handle_sig_int);
    // signal(SIGQUIT, handle_sig_quit);
    signal(SIGTSTP, handle_sig_tstp);
}