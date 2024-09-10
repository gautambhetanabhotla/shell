#include "neonate.h"

#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>

struct termios ORIGINAL_TERM;

void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &ORIGINAL_TERM);
}

int neonate(char** args, FILE* istream, FILE* ostream) {
    int time_arg = 1;
    for(int i = 1; i < 3; i++) {
        if(!args[i]) {
            fprintf(stderr, "ERROR: Invalid arguments\n");
            return -1;
        }
        if(strcmp(args[i], "-n") != 0) time_arg = atoi(args[i]);
    }
    if(time_arg == 0) {
        fprintf(stderr, "Set a proper non-zero time.\n");
        return -1;
    }
    enable_raw_mode();
    while(1) {
        struct timeval tv;
        fd_set readfds;

        tv.tv_sec = time_arg;
        tv.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(fileno(istream), &readfds);

        int retval = select(fileno(istream) + 1, &readfds, NULL, NULL, &tv);

        if(retval == -1) {
            perror("select()");
            break;
        }
        else if(retval) {
            if (fgetc(istream) == 'x') {
                break;
            }
        }
        else {
            fprintf(ostream, "348234\n");
            fflush(ostream);
        }
    }
    disable_raw_mode(&ORIGINAL_TERM);
    fprintf(ostream, "\n");
    fflush(ostream);
    return 0;
}