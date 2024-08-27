#include "reveal.h"
#include "prompt.h"
#include "hop.h"

#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <linux/limits.h>
#include <stdlib.h>

struct directory {
    char perms[11]; // null-terminated string of permissions
    long long int size;
    bool binary;
    char name[NAME_MAX + 2];
};

int compare_directories(const void *a, const void* b) {
    return strcmp(((struct directory*)a)->name, ((struct directory*)b)->name);
}

void print_dir(struct directory dir, bool l, bool a, FILE* ostream) {
    bool isdir = (dir.perms[0] == 'd');
    bool isbinary = false;
    if(l) {
        fprintf(ostream, "%s", dir.perms);
        fprintf(ostream, " %10lld  ", dir.size);
    }
    if(isbinary) fprintf(ostream, "\033[0;32m%s\033[0m\n", dir.name);
    else if(isdir) fprintf(ostream, "\033[0;34m%s\033[0m\n", dir.name);
    else fprintf(ostream, "%s\n", dir.name);
}

int reveal(char** args, FILE* istream, FILE* ostream) {
    bool l = false, a = false;
    char path[PATH_MAX] = {'\0'};
    int i = 1;
    while(args[i]) {
        if(args[i][0] == '-' && strlen(args[i]) != 1) {
            //interpret flags
            for(int j = 1; args[i][j]; j++) {
                if(args[i][j] == 'a') a = true;
                if(args[i][j] == 'l') l = true;
            }
        }
        else {
            if(strlen(path) == 0) strcpy(path, args[i]);
        }
        i++;
    }
    if(strlen(path) == 0) strcpy(path, CURRENT_DIRECTORY);

    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL) {
        #ifdef DEBUG
            perror("opendir");
        #endif
        return 1;
    }

    struct directory* directories = NULL;
    int num = 0;

    while((entry = readdir(dir)) != NULL) {
        directories = realloc(directories, (num + 1) * sizeof(struct directory));
        char path2[PATH_MAX];
        snprintf(path2, sizeof(path2), "%s/%s", CURRENT_DIRECTORY, entry->d_name);

        if(lstat(path2, &statbuf) == -1) {
            #ifdef DEBUG
                perror("lstat");
            #endif
            continue;
        }

        // printf("%c", (S_ISDIR(statbuf.st_mode)) ? 'd' : '.');
        sprintf(directories[num].perms, "%c%c%c%c%c%c%c%c%c%c",
            (S_ISDIR(statbuf.st_mode)) ? 'd' : '.',
            (statbuf.st_mode & S_IRUSR) ? 'r' : '-',
            (statbuf.st_mode & S_IWUSR) ? 'w' : '-',
            (statbuf.st_mode & S_IXUSR) ? 'x' : '-',
            (statbuf.st_mode & S_IRGRP) ? 'r' : '-',
            (statbuf.st_mode & S_IWGRP) ? 'w' : '-',
            (statbuf.st_mode & S_IXGRP) ? 'x' : '-',
            (statbuf.st_mode & S_IROTH) ? 'r' : '-',
            (statbuf.st_mode & S_IWOTH) ? 'w' : '-',
            (statbuf.st_mode & S_IXOTH) ? 'x' : '-');
        directories[num].perms[10] = '\0';

        directories[num].size = (long long int) statbuf.st_size;
        // printf(" %15ld", (long) statbuf.st_size);

        strcpy(directories[num].name, entry->d_name);
        if(S_ISDIR(statbuf.st_mode)) strcat(directories[num].name, "/");
        // printf(" %s", entry->d_name);
        num++;
    }
    closedir(dir);

    qsort(directories, num, sizeof(struct directory), compare_directories);
    for(int i = 0; i < num; i++) print_dir(directories[i], l, a, ostream);

    return 0;
}