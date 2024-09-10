#include "fgbg.h"

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int fg(char** args, FILE* istream, FILE* ostream) {
    if(args == NULL || args[1] == NULL) {
        fprintf(stderr, "ERROR: Invalid arguments\n");
        return -1;
    }
    int pid = atoi(args[1]);
    if(pid == 0) {
        return -1;
    }
    kill(pid, SIGCONT);
    kill(pid, SIGSTOP);
    kill(pid, SIGCONT);
    return 0;
}

int bg(char** args, FILE* istream, FILE* ostream) {

}