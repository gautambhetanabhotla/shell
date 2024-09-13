#include "fgbg.h"
#include "prompt.h"

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <time.h>
#include <linux/limits.h>

int fg(char** args) {
    if(args == NULL || args[1] == NULL) {
        fprintf(stderr, "ERROR: Invalid arguments\n");
        return -1;
    }
    int pid = atoi(args[1]);
    if(pid == 0) {
        fprintf(stderr, "Invalid PID\n");
        return -1;
    }
    if(kill(pid, SIGCONT) == -1) {
        #ifdef DEBUG
            perror("kill(SIGCONT)");
        #endif
        return -1;
    }
    int status;
    FG_PID = pid;
    time_t t_start, t_end;
    time(&t_start);
    if(waitpid(pid, &status, WUNTRACED) == -1) {
        #ifdef DEBUG
            perror("waitpid");
        #endif
        return -1;
    }
    FG_PID = 0;
    time(&t_end);
    if(difftime(t_end, t_start) >= 2) {
        snprintf(MOST_RECENT_FG_PROCESS, NAME_MAX, "%s: %d sec", args[0], (int)difftime(t_end, t_start));
    }
    else for(int i = 0; MOST_RECENT_FG_PROCESS[i]; i++) MOST_RECENT_FG_PROCESS[i] = '\0';
    return 0;
}

int bg(char** args) {
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