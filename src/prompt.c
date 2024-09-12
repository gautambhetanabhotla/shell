#include "prompt.h"
#include "parser.h"
#include "hop.h"
#include "log.h"
#include "proclore.h"
#include "reveal.h"
#include "seek.h"
#include "background.h"
#include "activities.h"
#include "ping.h"
#include "fgbg.h"
#include "iman.h"
#include "neonate.h"

#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

char *HOME_DIRECTORY = NULL, *USERNAME = NULL, *HOSTNAME = NULL, *CURRENT_DIRECTORY = NULL;
int (*USER_FUNCTIONS[])(char**) = {hop, exit_shell, Log, proclore, reveal, seek, activities, ping, fg, bg, neonate, iMan, NULL};
char* COMMAND_STRINGS[] = {"hop", "exit", "log", "proclore", "reveal", "seek", "activities", "ping", "fg", "bg", "neonate", "iMan", NULL};

char* CURRENT_DIRECTORY_CONVERTED = NULL;
int SHELL_PID;

char MOST_RECENT_FG_PROCESS[NAME_MAX];
int FG_PID = 0; // 0 if no foreground process is currently running, its pid otherwise

char* convert_path(char* input_string, char* home_dir, bool free_input) {
    // Converts a general path to one relative to the home directory.
    char* result = (char*) malloc((strlen(input_string) + 1) * sizeof(char));
    if(strcmp(input_string, home_dir) == 0) {
        strcpy(result, "~");
    }
    else if(strstr(input_string, home_dir) != NULL) {
        strcpy(result, "~/");
        strcat(result, input_string + strlen(home_dir) + 1);
    }
    else {
        strcpy(result, input_string);
    }
    if(free_input) free(input_string);
    return result;
}

void prompt() {
	printf("\e[0;31m%s@%s:%s %s $\e[0;37m ", USERNAME, HOSTNAME, CURRENT_DIRECTORY_CONVERTED, MOST_RECENT_FG_PROCESS);
    char* query = calloc(MAX_COMMAND_LENGTH + 2, sizeof(char));
    fgets(query, MAX_COMMAND_LENGTH + 1, stdin);
    if(feof(stdin)) exit_shell(NULL);
    if(query[0] == '\0' || query[1] == '\0') return;
    query[strlen(query) - 1] = '\0';
    #ifdef DEBUG
        // printf("QUERY: %s\n", query);
    #endif
    struct command* commands = separate_commands(query);
    if(strstr(query, "log") == NULL) add_to_log(query);
    int i = 0;
    while(commands[i].string != NULL) {
        i++;
    }
    if(commands && commands[i-1].sending_pipe == true) {
        fprintf(stderr, "Invalid usage of pipe!\n");
        return;
    }
    else if(commands && commands[0].receiving_pipe == true) {
        fprintf(stderr, "Invalid usage of pipe!\n");
        return;
    }
    int pipes[i][2];
    int j = 0;
    while(commands[j].string) {
        int save_stdin = dup(STDIN_FILENO), save_stdout = dup(STDOUT_FILENO);
        // FILE* istream = stdin, *ostream = stdout;
        char** args = get_args(commands[j].string, commands[j].background);
        if(commands[j].sending_pipe) {
            if(pipe(pipes[j])) {
                fprintf(stderr, "Failed to create pipe!\n");
                perror("pipe");
                return;
            }
            // ostream = fdopen(pipes[j][1], "w");
            dup2(pipes[j][1], STDOUT_FILENO);

        }
        if(commands[j].receiving_pipe) {
            // istream = fdopen(pipes[j-1][0], "r");
            dup2(pipes[j-1][0], STDIN_FILENO);
        }
        // istream = fdopen(STDIN_FILENO, "r");
        // ostream = fdopen(STDOUT_FILENO, "a");
        execute(args, commands[j].background);
        dup2(save_stdin, STDIN_FILENO);
        dup2(save_stdout, STDOUT_FILENO);
        j++;
    }
    for(j = 0; j < i; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
        // free(commands[j].string);
    }
    // free(commands);
    // free(query);
}

void init_shell() {
    // Initialise the shell's home directory to the current working directory
    HOME_DIRECTORY = calloc(PATH_MAX, sizeof(char));
    CURRENT_DIRECTORY = calloc(PATH_MAX, sizeof(char));
    // LAST_DIRECTORY = calloc(PATH_MAX, sizeof(char));
    for(int i = 0; MOST_RECENT_FG_PROCESS[i]; i++) MOST_RECENT_FG_PROCESS[i] = '\0';

    getcwd(HOME_DIRECTORY, PATH_MAX);
    getcwd(CURRENT_DIRECTORY, PATH_MAX);
    // getcwd(LAST_DIRECTORY, PATH_MAX);

    CURRENT_DIRECTORY_CONVERTED = convert_path(CURRENT_DIRECTORY, HOME_DIRECTORY, false);

    init_log();

    USERNAME = calloc(257, sizeof(char));
    getlogin_r(USERNAME, 256);

    HOSTNAME = calloc(254, sizeof(char));
    gethostname(HOSTNAME, 253);

    SHELL_PID = (int) getpid();

    set_handlers();
    act.sa_sigaction = sigchld_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGCHLD, &act, NULL);

    tcgetattr(STDIN_FILENO, &ORIGINAL_TERM);
}

int exit_shell(char** args) {
    save_log();
    free(HOME_DIRECTORY);
    free(CURRENT_DIRECTORY);
    free(CURRENT_DIRECTORY_CONVERTED);
    free(USERNAME);
    free(HOSTNAME);
    exit(0);
}

void execute(char** args, bool background) {
    int i = 0;
    int save_stdin = dup(STDIN_FILENO), save_stdout = dup(STDOUT_FILENO);
    while(args[i]) {
        if(strcmp(args[i], ">") == 0) {
            if(!args[i+1]) {
                fprintf(stderr, "Invalid redirection\n");
                return;
            }
            dup2(STDOUT_FILENO, open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644));
        }
        else if(strcmp(args[i], ">>") == 0) {
            dup2(STDOUT_FILENO, open(args[i+1], O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0644));
        }
        else if(strcmp(args[i], "<") == 0) {
            int fd = open(args[i+1], O_RDONLY, 0644);
            if(fd < 0) {
                fprintf(stderr, "File does not exist\n");
                return;
            }
            dup2(STDIN_FILENO, fd);
        }
        else {
            i++;
            continue;
        }
        int j = i;
        while(args[j+2]) {
            free(args[j]);
            args[j] = args[j+2];
            j++;
        }
        args[j] = args[j+2];
        i++;
    }
    time_t t_start, t_end;
    for(int i = 0; COMMAND_STRINGS[i]; i++) {
        if(strcmp(args[0], COMMAND_STRINGS[i]) == 0) {
            time(&t_start);
            int rc = USER_FUNCTIONS[i](args);
            dup2(save_stdin, STDIN_FILENO);
            dup2(save_stdout, STDOUT_FILENO);
            time(&t_end);
            if(difftime(t_end, t_start) >= 2) {
                snprintf(MOST_RECENT_FG_PROCESS, NAME_MAX, "%s: %d sec", args[0], (int)difftime(t_end, t_start));
            }
            else for(int i = 0; MOST_RECENT_FG_PROCESS[i]; i++) MOST_RECENT_FG_PROCESS[i] = '\0';
            if(rc) fprintf(stderr, "%s exited with status %d\n", COMMAND_STRINGS[i], rc);
            
            return;
        }
    }
    int rc2 = fork();
    if(rc2 > 0) {
        int i = 0;
        while(args[i]) i++;
        int stat;
        time(&t_start);
        if(!background) FG_PID = rc2;
        if(!background) waitpid(rc2, &stat, WUNTRACED);
        else {
            printf("%d\n", rc2);
            strings[rc2] = strdup(args[0]);
        }
        time(&t_end);
        if(!background) FG_PID = 0;
        if(difftime(t_end, t_start) >= 2) {
            snprintf(MOST_RECENT_FG_PROCESS, NAME_MAX, "%s: %d sec", args[0], (int)difftime(t_end, t_start));
        }
        else for(int i = 0; MOST_RECENT_FG_PROCESS[i]; i++) MOST_RECENT_FG_PROCESS[i] = '\0';
    }
    else if(rc2 == 0) {
        if(execvp(args[0], args) != 0) {
            fprintf(stderr, "ERROR: Invalid system command \"%s\".\n", args[0]);
            dup2(save_stdin, STDIN_FILENO);
            dup2(save_stdout, STDOUT_FILENO);
            exit(1);
        }
    }
    else {
        fprintf(stderr, "ERROR: Could not execute command.\n");
        dup2(save_stdin, STDIN_FILENO);
        dup2(save_stdout, STDOUT_FILENO);
        return;
    }
    // fclose(istream); fclose(ostream);
    dup2(save_stdin, STDIN_FILENO);
    dup2(save_stdout, STDOUT_FILENO);
}

// void execute(char** args, bool background, FILE* istream, FILE* ostream) {
//     time_t t_start, t_end;
//     if(strcmp(args[0], "exit") == 0) exit_shell(NULL, NULL, NULL);
//     int rc2 = fork();
//     if(rc2 > 0) {
//         int i = 0;
//         while(args[i]) i++;
//         int stat;
//         time(&t_start);
//         if(!background) FG_PID = rc2;
//         if(!background) waitpid(rc2, &stat, WUNTRACED);
//         else {
//             printf("%d\n", rc2);
//             strings[rc2] = strdup(args[0]);
//         }
//         time(&t_end);
//         if(!background) FG_PID = 0;
//         if(difftime(t_end, t_start) >= 2) {
//             snprintf(MOST_RECENT_FG_PROCESS, NAME_MAX, "%s: %d sec", args[0], (int)difftime(t_end, t_start));
//         }
//         else for(int i = 0; MOST_RECENT_FG_PROCESS[i]; i++) MOST_RECENT_FG_PROCESS[i] = '\0';
//         if(strcmp(args[0], "hop") == 0) {
//             char buf[PATH_MAX];
//             snprintf(buf, sizeof(buf), "%s/dirfile.txt", HOME_DIRECTORY);
//             FILE* dirfile = fopen(buf, "r");
//             fscanf(dirfile, "%s", CURRENT_DIRECTORY);
//             fclose(dirfile);
//             if(CURRENT_DIRECTORY_CONVERTED) (CURRENT_DIRECTORY_CONVERTED);
//             CURRENT_DIRECTORY_CONVERTED = convert_path(CURRENT_DIRECTORY, HOME_DIRECTORY, false);
//             chdir(CURRENT_DIRECTORY);
//         }
//         if(args[0] && args[1] && strcmp(args[0], "log") == 0 && strcmp(args[1], "purge") == 0) {
//             purge_log();
//         }
//     }
//     else if(rc2 == 0) {
//         for(int i = 0; COMMAND_STRINGS[i]; i++) {
//             if(strcmp(args[0], COMMAND_STRINGS[i]) == 0) {
//                 time(&t_start);
//                 int rc = USER_FUNCTIONS[i](args, istream, ostream);
//                 time(&t_end);
//                 if(difftime(t_end, t_start) >= 2) {
//                     snprintf(MOST_RECENT_FG_PROCESS, NAME_MAX, "%s: %d sec", args[0], (int)difftime(t_end, t_start));
//                 }
//                 else for(int i = 0; MOST_RECENT_FG_PROCESS[i]; i++) MOST_RECENT_FG_PROCESS[i] = '\0';
//                 if(rc) fprintf(stderr, "%s exited with status %d\n", COMMAND_STRINGS[i], rc);
//                 exit(rc);
//             }
//         }
//         if(execvp(args[0], args) != 0) {
//             fprintf(stderr, "ERROR: Invalid system command \"%s\".\n", args[0]);
//             exit(1);
//         }
//     }
//     else {
//         fprintf(stderr, "ERROR: Could not execute command.\n");
//         return;
//     }
// }