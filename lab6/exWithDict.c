#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	int pfd1[2], pfd2[2];
	pid_t pid1, pid2;
	
	if (pipe(pfd1) == -1) {
		perror("Something went wrong with pipe:");
		exit(EXIT_FAILURE);
	}
	if (pipe(pfd2) == -1) {
		perror("Something went wrong with pipe:");
		exit(EXIT_FAILURE);
	}
	pid1 = fork();
	if (pid1 == 0) {
		while ()
	} else {
		pid2 = fork();
	}

	return 0;
}

