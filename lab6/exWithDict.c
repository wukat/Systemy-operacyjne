/*
 * author wukat
*/
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 128

int main(void) {
    int pfd1[2], pfd2[2];
    pid_t pid1, pid2;
    int nread0 = 0, nread1 = 0, nread2 = 0;
    char buf0[BUFSIZE], buf1[BUFSIZE], buf2[BUFSIZE];

    if ((pipe(pfd1) == -1) || (pipe(pfd2) == -1)) {
        perror("Something went wrong with pipe:");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == 0) {
        close(pfd1[1]);
        while ((nread1 = read(pfd1[0], buf1, sizeof(buf1))) != 0) {
            
        }
        } else {
        pid2 = fork();
        if (pid2 == 0) {
            
        }
    }

    return 0;
}

