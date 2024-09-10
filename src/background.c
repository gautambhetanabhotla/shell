#include "background.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/limits.h>

struct sigaction act;

char* strings[10000000] = {NULL};

void sigchld_handler(int signo, siginfo_t* siginfo, void* context) {
	if(signo == SIGCHLD) {
		int status;
		pid_t pid;
		char* name;
		while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
			name = strings[pid];
			if(WIFEXITED(status)) {
				if(WEXITSTATUS(status) == 0) printf("%s exited normally (%d)\n", name, pid);
				else printf("%s exited abnormally with status %d (%d)\n", name, WEXITSTATUS(status), pid);
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
			free(name);
		}
	}
}