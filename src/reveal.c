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
#include <unistd.h>
#include <time.h>

struct directory {
    char perms[11]; // null-terminated string of permissions
    long long int size;
    char name[NAME_MAX + 2];
    char parent[PATH_MAX + 1];
    char linkto[PATH_MAX + 1];
    char last_modified_time[100];
};

int compare_directories(const void *a, const void* b) {
    return strcmp(((struct directory*)a)->name, ((struct directory*)b)->name);
}

char* name_from_path(char* input) {
    // char* result = calloc(NAME_MAX, sizeof(char));
    char* INPUT = strdup(input);
    char *p = INPUT, *q; char* save_ptr;
    p = __strtok_r(INPUT, "/", &save_ptr);
    while(p != NULL) {
        q = p;
        p = __strtok_r(NULL, "/", &save_ptr);
    }
    return q;
}

void print_dir(struct directory dir, bool l, bool a, FILE* ostream) {
    bool isdir = (dir.perms[0] == 'd');
    bool ishidden = (dir.name[0] == '.');
    if(a || !ishidden) {
        if(l) {
            fprintf(ostream, "%s", dir.perms);
            fprintf(ostream, " %10lld  ", dir.size);
            fprintf(ostream, "%s ", dir.last_modified_time);
        }
        if(dir.perms[0] == 'd') fprintf(ostream, "\033[0;34m%s\033[0m", dir.name);
        else if(dir.perms[3] == 'x') fprintf(ostream, "\033[0;32m%s\033[0m", dir.name);
        else fprintf(ostream, "%s", dir.name);
        
        if(l && (strlen(dir.linkto) != 0)) fprintf(ostream, " -> %s", dir.linkto);
        fprintf(ostream, "\n");
    }
}

int reveal(char** args) {
    long int blocks = 0;
    int blocksize = 0;
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
            char* x = convert_path_back(args[i], false);
            if(x == NULL) {
                fprintf(stderr, "ERROR: Invalid path!\n");
                return -1;
            }
            if(strlen(path) == 0) strcpy(path, x);
        }
        i++;
    }
    if(strlen(path) == 0) strcpy(path, CURRENT_DIRECTORY);

    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL) {
        if(errno == ENOTDIR) {
            struct stat statbuf;
            if(lstat(path, &statbuf) == -1) {
                #ifdef DEBUG
                    perror("lstat");
                #endif
            }
            struct directory dir;
            for(int i = 0; i <= PATH_MAX; i++) dir.linkto[i] = '\0';
            sprintf(dir.perms, "%c%c%c%c%c%c%c%c%c%c",
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
            dir.perms[10] = '\0';
            dir.size = statbuf.st_size;
            strcpy(dir.name, name_from_path(path));
            char shit[2 * PATH_MAX];
            sprintf(shit, "%s/%s", path, dir.name);
            if((S_ISLNK(statbuf.st_mode))) readlink(shit, dir.linkto, PATH_MAX);
            strcpy(dir.last_modified_time, ctime(&statbuf.st_mtime));
            dir.last_modified_time[strlen(dir.last_modified_time) - 1] = '\0';
            print_dir(dir, l, a, stdout);
            return 0;
        }
        return -1;
    }

    struct directory* directories = NULL;
    int num = 0;

    while((entry = readdir(dir)) != NULL) {
        directories = realloc(directories, (num + 1) * sizeof(struct directory));
        char path2[2 * PATH_MAX];
        sprintf(path2, "%s/%s", path, entry->d_name);
        #ifdef DEBUG
            // printf("file to reveal: %s\n", path2);
        #endif

        if(lstat(path2, &statbuf) == -1) {
            #ifdef DEBUG
                perror("lstat");
            #endif
            continue;
        }
        if(strcmp(entry->d_name, ".") == 0) {
            blocks = statbuf.st_blocks;
            blocksize = statbuf.st_blksize;
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
        for(int i = 0; i <= PATH_MAX; i++) directories[num].linkto[i] = '\0';
        if(S_ISDIR(statbuf.st_mode)) strcat(directories[num].name, "/");
        char shit[2 * PATH_MAX];
        sprintf(shit, "%s/%s", path, directories[num].name);
        strcpy(directories[num].last_modified_time, ctime(&statbuf.st_mtime));
        directories[num].last_modified_time[strlen(directories[num].last_modified_time) - 1] = '\0';
        if((S_ISLNK(statbuf.st_mode))) readlink(shit, directories[num].linkto, PATH_MAX);
        num++;
    }
    closedir(dir);

    if(l) fprintf(stdout, "Total blocks allocated: %ld blocks of %d kB each\n", blocks, blocksize);
    qsort(directories, num, sizeof(struct directory), compare_directories);
    for(int i = 0; i < num; i++) print_dir(directories[i], l, a, stdout);

    return 0;
}