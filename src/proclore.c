#include "proclore.h"
#include "prompt.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <string.h>

int print_process_data(int pid) {
    int rc = 0;
    char file_path[42];
    sprintf(file_path, "/proc/%d/stat", pid);
    FILE* status_file = fopen(file_path, "r");
    if(status_file == NULL) {
        printf("ERROR: Process does not exist\n");
        return -1;
    }
    sprintf(file_path, "/proc/%d/exe", pid);
    // FILE* exe_file = fopen(file_path, "r");
    // if(exe_file == NULL) return -1;

    int pgid = 0, n, controlling_terminal;
    long long unsigned int vmem = 0;
    char p_status[100], exe_path[PATH_MAX + 2] = {'\0'};
    
    n = 3;
    while(n--) fscanf(status_file, "%s", p_status);

    n = 2;
    while(n--) fscanf(status_file, "%d", &pgid);

    n = 2;
    while(n--) fscanf(status_file, "%d", &controlling_terminal);

    n = 16;
    while(n--) fscanf(status_file, "%llu", &vmem);

    if(readlink(file_path, exe_path, PATH_MAX) == -1) rc = -1;
    if(controlling_terminal != 0) strcat(p_status, "+");

    printf("PID: %d\nProcess status: %s\nProcess group: %d\nVirtual memory: %llu\nExecutable path: %s\n", pid, p_status, pgid, vmem, exe_path);
    return rc;
}

int proclore(char** args) {
    if(args[1] == NULL) {
        return print_process_data(SHELL_PID);
    }
    else {
        return print_process_data(atoi(args[1]));   
    }
}