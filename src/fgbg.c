#include "fgbg.h"

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

int fg(char** args, FILE* istream, FILE* ostream) {
    if(args == NULL || args[1] == NULL) {
        fprintf(stderr, "ERROR: Invalid arguments\n");
        return -1;
    }
    int pid = atoi(args[1]);
    if(pid == 0) {
        fprintf(stderr, "Invalid PID\n");
        return -1;
    }
    // Get the terminal file descriptor
    int fd = fileno(istream);

    // Set the process group ID of the specified process
    if (tcsetpgrp(fd, pid) == -1) {
        #ifdef DEBUG
        perror("tcsetpgrp");
        #endif
        return -1;
    }

    // Send SIGCONT to the process to continue its execution
    if (kill(pid, SIGCONT) == -1) {
        #ifdef DEBUG
        perror("kill(SIGCONT)");
        #endif
        return -1;
    }

    // Wait for the process to change state
    int status;
    if (waitpid(pid, &status, WUNTRACED) == -1) {
        #ifdef DEBUG
        perror("waitpid");
        #endif
        return -1;
    }

    // Restore the terminal control to the shell
    if (tcsetpgrp(fd, getpgrp()) == -1) {
        #ifdef DEBUG
        perror("tcsetpgrp");
        #endif
        return -1;
    }
    return 0;
}

int bg(char** args, FILE* istream, FILE* ostream) {
    if(args == NULL || args[1] == NULL) {
        fprintf(stderr, "ERROR: Invalid arguments\n");
        return -1;
    }
    int pid = atoi(args[1]);
    if(pid == 0) {
        fprintf(stderr, "Invalid PID\n");
        return -1;
    }
    kill(pid, SIGCONT);
    return 0;
}