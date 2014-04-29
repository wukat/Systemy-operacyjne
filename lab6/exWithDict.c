/*
 * author wukat
 */
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define BUFSIZE 128

int main(void) {
    int pfd1[2], pfd2[2], pfd1b[2], pfd2b[2];
    pid_t pid1, pid2;
    int nread0 = 0, nread1 = 0, nread2 = 0, fd = 0;
    int count1 = 0, i, count2 = 0;
    char buf0[BUFSIZE], buf1[BUFSIZE], buf2[BUFSIZE];

    if ((pipe(pfd1) == -1) || (pipe(pfd2) == -1) || (pipe(pfd1b) == -1) || (pipe(pfd2b) == -1)) {
        perror("Something went wrong with pipe:");
        exit(EXIT_FAILURE);
    }

    fd = open("dictionary.txt", O_RDONLY);
    pid1 = fork();
    if (pid1 == 0) {
        close(pfd1[1]);
        close(pfd1b[0]);
        while ((nread1 = read(pfd1[0], buf1, sizeof (buf1))) != 0) {
            for (i = 0; i < nread1; i++) {
                if (buf1[i] == '\n') {
                    count1++;
                }
            }
        }
        snprintf(buf1, sizeof (buf1), "%d", count1);
        write(pfd1b[1], buf1, strlen(buf1));
        close(pfd1b[1]);
        exit(EXIT_SUCCESS);
    } else {
        pid2 = fork();
        if (pid2 == 0) {
            close(pfd2[1]);
            close(pfd2b[0]);
            while ((nread2 = read(pfd2[0], buf2, sizeof (buf2))) != 0) {
                if (strtok(buf2, "pipe") != NULL) {
                    count2++;
                    write(STDOUT_FILENO, buf2, sizeof(buf2));
                    while (strtok(NULL, "pipe") != NULL) {
                        count2++;
                    }
                }
            }
            snprintf(buf2, sizeof (buf2), "%d", count2);
            write(pfd2b[1], buf2, strlen(buf2));
            close(pfd2b[1]);
            exit(EXIT_SUCCESS);
        } else {
            close(pfd1[0]);
            close(pfd2[0]);
            while ((nread0 = read(fd, buf0, sizeof (buf0))) > 0) {
                write(pfd1[1], buf0, nread0);
                write(pfd2[1], buf0, nread0);
            }
            close(pfd1[1]);
            close(pfd2[1]);
            /*close(pfd1b[1]);
            close(pfd2b[1]);*/
            /*read(pfd1b[0], buf0, sizeof (buf0));
            printf("Liczba linii: %s", buf0);*/
            /*while ((nread0 = read(pfd2b[0], buf0, sizeof (buf0))) != 0);*/
            /*printf("Liczba linii z pipe: %s", buf0);*/

        }
    }

    return 0;
}

