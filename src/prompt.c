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
#include "alias.h"

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
int (*CHILD_FUNCTIONS[])(char**) = {proclore, reveal, seek, activities, ping, fg, bg, neonate, iMan, NULL};
char* CHILD_STRINGS[] = {"proclore", "reveal", "seek", "activities", "ping", "fg", "bg", "neonate", "iMan", NULL};
int (*PARENT_FUNCTIONS[])(char**) = {hop, exit_shell, Log, alias, NULL};
char* PARENT_STRINGS[] = {"hop", "exit", "log", "alias", NULL};

char* CURRENT_DIRECTORY_CONVERTED = NULL;
int SHELL_PID;

char MOST_RECENT_FG_PROCESS[NAME_MAX];
int FG_PID = 0; // 0 if no foreground process is currently running, its pid otherwise

bool is_child_command(char* str) {
    bool result = false;
    for(int i = 0; CHILD_STRINGS[i] != NULL; i++) {
        if(strcmp(CHILD_STRINGS[i], str) == 0) {
            result = true;
            break;
        }
    }
    return result;
}

bool is_parent_command(char* str) {
    bool result = false;
    for(int i = 0; PARENT_STRINGS[i] != NULL; i++) {
        if(strcmp(PARENT_STRINGS[i], str) == 0) {
            result = true;
            break;
        }
    }
    return result;
}



void prompt() {
	printf("\n\e[0;31m%s@%s:%s %s $\e[0;37m ", USERNAME, HOSTNAME, CURRENT_DIRECTORY_CONVERTED, MOST_RECENT_FG_PROCESS);
    char* query = calloc(MAX_COMMAND_LENGTH + 2, sizeof(char));
    fgets(query, MAX_COMMAND_LENGTH + 1, stdin);
    if(feof(stdin)) exit_shell(NULL);
    if(query[0] == '\0' || query[1] == '\0') return;
    query[strlen(query) - 1] = '\0';
    query = replace(query, ALIASES, true);
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
        char** args = get_args(commands[j].string, commands[j].background);
        if(commands[j].sending_pipe) {
            if(pipe(pipes[j])) {
                fprintf(stderr, "Failed to create pipe!\n");
                #ifdef DEBUG
                    perror("pipe");
                #endif
                return;
            }
            dup2(pipes[j][1], STDOUT_FILENO);
            stdout = fdopen(STDOUT_FILENO, "w");
            close(pipes[j][1]);

        }
        if(commands[j].receiving_pipe) {
            dup2(pipes[j-1][0], STDIN_FILENO);
            stdin = fdopen(STDIN_FILENO, "r");
            close(pipes[j-1][0]);
        }
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

void set_redirections(char** args, int* save_stdin, int* save_stdout, int* fd) {
    *save_stdin = dup(STDIN_FILENO);
    *save_stdout = dup(STDOUT_FILENO);
    int i = 0;
    while(args[i]) {
        if(strcmp(args[i], ">") == 0) {
            if(!args[i+1]) {
                fprintf(stderr, "Invalid redirection\n");
                return;
            }
            *fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(*fd, STDOUT_FILENO);
        }
        else if(strcmp(args[i], ">>") == 0) {
            *fd = open(args[i+1], O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0644);
            dup2(*fd, STDOUT_FILENO);
        }
        else if(strcmp(args[i], "<") == 0) {
            *fd = open(args[i+1], O_RDONLY, 0644);
            if(*fd < 0) {
                fprintf(stderr, "File does not exist\n");
                return;
            }
            dup2(*fd, STDIN_FILENO);
        }
        else {
            i++;
            continue;
        }
    }
    i = 0;
    while(args[i]) {
        if(strcmp(args[i], "<") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], ">") == 0) {
            free(args[i]);
            args[i] = NULL;
            free(args[i+1]);
            args[i+1] = NULL;
            i += 2;
        }
        else i++;
    }
}

// void execute(char** args, bool background) {
//     int save_stdin, save_stdout, fd = -1;
//     set_redirections(args, &save_stdin, &save_stdout, &fd);
//     time_t t_start, t_end;
//     for(int i = 0; CHILD_STRINGS[i]; i++) {
//         if(strcmp(args[0], CHILD_STRINGS[i]) == 0) {
//             time(&t_start);
//             int rc = USER_FUNCTIONS[i](args);
//             dup2(save_stdin, STDIN_FILENO);
//             dup2(save_stdout, STDOUT_FILENO);
//             close(save_stdin); close(save_stdout);
//             time(&t_end);
//             if(difftime(t_end, t_start) >= 2) {
//                 snprintf(MOST_RECENT_FG_PROCESS, NAME_MAX, "%s: %d sec", args[0], (int)difftime(t_end, t_start));
//             }
//             else for(int i = 0; MOST_RECENT_FG_PROCESS[i]; i++) MOST_RECENT_FG_PROCESS[i] = '\0';
//             if(rc) fprintf(stderr, "%s exited with status %d\n", CHILD_STRINGS[i], rc);
            
//             return;
//         }
//     }
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
//             bg_process_strings[rc2] = strdup(args[0]);
//         }
//         time(&t_end);
//         if(!background) FG_PID = 0;
//         if(difftime(t_end, t_start) >= 2) {
//             snprintf(MOST_RECENT_FG_PROCESS, NAME_MAX, "%s: %d sec", args[0], (int)difftime(t_end, t_start));
//         }
//         else for(int i = 0; MOST_RECENT_FG_PROCESS[i]; i++) MOST_RECENT_FG_PROCESS[i] = '\0';
//     }
//     else if(rc2 == 0) {
//         if(execvp(args[0], args) != 0) {
//             fprintf(stderr, "ERROR: Invalid system command \"%s\".\n", args[0]);
//             dup2(save_stdin, STDIN_FILENO);
//             dup2(save_stdout, STDOUT_FILENO);
//             close(save_stdin); close(save_stdout);
//             if(fd >= 0) close(fd);
//             exit(1);
//         }
//     }
//     else {
//         fprintf(stderr, "ERROR: Could not execute command.\n");
//         dup2(save_stdin, STDIN_FILENO);
//         dup2(save_stdout, STDOUT_FILENO);
//         close(save_stdin); close(save_stdout);
//         if(fd >= 0) close(fd);
//         return;
//     }
//     dup2(save_stdin, STDIN_FILENO);
//     dup2(save_stdout, STDOUT_FILENO);
//     close(save_stdin); close(save_stdout);
//     if(fd >= 0) close(fd);
// }

void execute(char** args, bool background) {
    int save_stdin, save_stdout, fd = -1;
    set_redirections(args, &save_stdin, &save_stdout, &fd);
    time_t t_start, t_end;
    if(is_parent_command(args[0])) {
        for(int i = 0; PARENT_STRINGS[i]; i++) {
            if(strcmp(args[0], PARENT_STRINGS[i]) == 0) {
                time(&t_start);
                int rc = PARENT_FUNCTIONS[i](args);
                time(&t_end);
                if(difftime(t_end, t_start) >= 2) {
                    snprintf(MOST_RECENT_FG_PROCESS, NAME_MAX, "%s: %d sec", args[0], (int)difftime(t_end, t_start));
                }
                else for(int i = 0; MOST_RECENT_FG_PROCESS[i]; i++) MOST_RECENT_FG_PROCESS[i] = '\0';
                if(rc) fprintf(stderr, "%s exited with status %d\n", PARENT_STRINGS[i], rc);
                return;
            }
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
            bg_process_strings[rc2] = strdup(args[0]);
        }
        time(&t_end);
        if(!background) FG_PID = 0;
        if(difftime(t_end, t_start) >= 2) {
            snprintf(MOST_RECENT_FG_PROCESS, NAME_MAX, "%s: %d sec", args[0], (int)difftime(t_end, t_start));
        }
        else for(int i = 0; MOST_RECENT_FG_PROCESS[i]; i++) MOST_RECENT_FG_PROCESS[i] = '\0';
    }
    else if(rc2 == 0) {
        for(int i = 0; CHILD_STRINGS[i]; i++) {
            if(strcmp(args[0], CHILD_STRINGS[i]) == 0) {
                time(&t_start);
                int rc = CHILD_FUNCTIONS[i](args);
                time(&t_end);
                if(difftime(t_end, t_start) >= 2) {
                    snprintf(MOST_RECENT_FG_PROCESS, NAME_MAX, "%s: %d sec", args[0], (int)difftime(t_end, t_start));
                }
                else for(int i = 0; MOST_RECENT_FG_PROCESS[i]; i++) MOST_RECENT_FG_PROCESS[i] = '\0';
                if(rc) fprintf(stderr, "%s exited with status %d\n", CHILD_STRINGS[i], rc);
                exit(rc);
            }
        }
        if(execvp(args[0], args) != 0) {
            fprintf(stderr, "ERROR: Invalid system command \"%s\".\n", args[0]);
            exit(1);
        }
    }
    else {
        fprintf(stderr, "ERROR: Could not execute command.\n");
        return;
    }
}