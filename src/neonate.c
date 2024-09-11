#include "neonate.h"

#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include <dirent.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

struct termios ORIGINAL_TERM;

void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &ORIGINAL_TERM);
}

int most_recent_pid() {
    time_t most_recent_time = 0;
    int result = 0;
    DIR* dir = opendir("/proc");
    if(dir == NULL) {
        return -1;
    }
    struct dirent* entry;
    struct stat buf;
    while((entry = readdir(dir)) != NULL) {
        char entry_path[PATH_MAX];
        sprintf(entry_path, "/proc/%s", entry->d_name);
        if(entry->d_name[0] > '9' || entry->d_name[0] < '0') continue;
        if(lstat(entry_path, &buf)) {
            continue;
        }
        if((S_ISDIR(buf.st_mode))) {
            if(difftime(buf.st_ctime, most_recent_time) > 0) {
                most_recent_time = buf.st_mtime;
                result = atoi(entry->d_name);
            }
        }
    }
    closedir(dir);
    return result;
}

int neonate(char** args, FILE* istream, FILE* ostream) {
    int time_arg = 1;
    bool correct_args = false;
    for(int i = 1; i < 3; i++) {
        if(!args[i]) {
            fprintf(stderr, "ERROR: Invalid arguments\n");
            return -1;
        }
        if(strcmp(args[i], "-n") != 0) time_arg = atoi(args[i]);
        else correct_args = true;
    }
    if(!correct_args) {
        fprintf(stderr, "ERROR: Invalid arguments\n");
        return -1;
    }
    if(time_arg == 0) {
        fprintf(stderr, "Set a proper non-zero integer time.\n");
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
            #ifdef DEBUG
            perror("select()");
            #endif
            break;
        }
        else if(retval) {
            if(fgetc(istream) == 'x') {
                break;
            }
        }
        else {
            fprintf(ostream, "%d\n", most_recent_pid());
            fflush(ostream);
        }
    }
    disable_raw_mode(&ORIGINAL_TERM);
    fprintf(ostream, "\n");
    fflush(ostream);
    return 0;
}