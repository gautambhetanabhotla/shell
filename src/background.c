#include "background.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/limits.h>

struct sigaction act;

void handler(int signo, siginfo_t* siginfo, void* context) {
	if(signo == SIGCHLD) {
		int status;
		pid_t pid;
		pid = waitpid(-1, &status, WNOHANG);
		char path[PATH_MAX], name[NAME_MAX];
			snprintf(path, PATH_MAX, "/proc/%d/comm", pid);
			FILE* f = fopen(path, "r");
			if(f) {
				fscanf(f, "%s", name);
				fclose(f);
			}
			else {
				#ifdef DEBUG
					perror("fopen");
				#endif
			}
		while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
			// char path[PATH_MAX], name[NAME_MAX];
			snprintf(path, PATH_MAX, "/proc/%d/comm", pid);
			FILE* f = fopen(path, "r");
			if(f) {
				fscanf(f, "%s", name);
				fclose(f);
			}
			else {
				#ifdef DEBUG
					perror("fopen");
				#endif
			}
			if(WIFEXITED(status)) {
				if(WEXITSTATUS(status) == 0) printf("%s exited normally (%d)\n", name, pid);
				else printf("%s exited with status %d (%d)\n", name, WEXITSTATUS(status), pid);
			}
			else if(WIFSIGNALED(status)) {
				printf("%s was terminated by signal %d (%d)\n", name, WTERMSIG(status), pid);
			}
			else if(WIFSTOPPED(status)) {
				printf("%s was stopped by the delivery of signal %d (%d)\n", name, WSTOPSIG(status), pid);
			}
			else {
				printf("%s has stopped/paused execution (%d)\n", name, pid);
			}
		}
	}
}