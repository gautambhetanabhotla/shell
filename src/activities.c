#include "activities.h"
#include "background.h"

#include <string.h>

int activities(char** args) {
    char** shit100 = NULL;
    for(int pid = 1; pid < 10000000; pid++) {
        if(bg_process_strings[pid]) {
            char buf[100];
            sprintf(buf, "/proc/%d/stat", pid);
            FILE* status_file = fopen(buf, "r");
            if(status_file == NULL) continue;
            char p_status[100];
            int n = 3;
            while(n--) fscanf(status_file, "%s", p_status);
            fclose(status_file);
            printf("%d: %s - %s\n", pid, bg_process_strings[pid], strcmp(p_status, "R") == 0 ? "Running" : (strcmp(p_status, "S") == 0 ? "Sleeping" : (strcmp(p_status, "T") == 0 ? "Stopped" : "Other")));
        }
    }
}