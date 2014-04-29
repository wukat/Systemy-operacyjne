/** atuhor wukat */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>	
#include <string.h>
#include "backward.h"
#include "readall.h"
#include "writeall.h"

#define BUFSIZE 512
int mkostemp(char *template, int flags);

/* use './tail -5 path' to see 5 last lines of file (default 10) */
/* without any file given, uses stdout */

/* example: from string like -5 makes integer 5*/
int getNumber(char * str) {
    int size = strlen(str);
    int i = 0;
    int result = 0;
    for (i = 1; i < size; i++) {
        result *= 10;
        if (str[i] >= '0' && str[i] <= '9')
            result += str[i] - '0';
        else {
            printf("Option %s unknown", str);
            return 10;
        }
    }
    return result;
}

/** creates tmp file for stdout (user input) */
int createTempForUserInput() {
    char fileNameBuff[15];
    char s[BUFSIZE];
    int nread = -1, nwrite = -1, fdTemp = -1;
    strncpy(fileNameBuff, "/tmp/tailXXXXXX", 15);
    if ((fdTemp = mkostemp(fileNameBuff, O_APPEND)) > 1) {
        while ((nread = read(STDIN_FILENO, s, sizeof (s))) > 0) {
            if ((nwrite = writeall(fdTemp, s, nread)) == -1) {
                perror("Błąd w zapisie");
                return -1;
            }
        }
    } else {
        perror("Nie udało się utworzyć pliku tymczasowego");
    }
    return fdTemp;
}

/* reads and writes to stdout line lines for the bottom, if name specified, writes before output*/

/* for stdout creates temporary file */
void tail(int fd, int line, char * name) {
    char s[BUFSIZE], c;
    int fdTemp = -1, nread = -1;
    off_t where;
    char* beg = "\n\n==> ";
    char* end = " <==\n";

    if (fd == STDOUT_FILENO) { /* saves in temporary file user's input*/
        if ((fdTemp = createTempForUserInput()) <= 1)
            return;
        else
            fd = fdTemp; /*change fd to temp file*/
    }
    where = lseek(fd, 0, SEEK_END);
    line--;
    do {
        switch (pread(fd, &c, 1, where)) {
            case 1:
                if (c == '\n') {
                    line--;
                }
                break;
            case -1:
                /* handle the error */
                break;
            default: /* impossible */
                errno = 0;
                /* handle the error */
        }
    } while (where-- > 0 && line >= 0);
    if (name != NULL) {
        writeall(STDOUT_FILENO, beg, strlen(beg));
        writeall(STDOUT_FILENO, name, strlen(name));
        writeall(STDOUT_FILENO, end, strlen(end));
    }
    lseek(fd, ++where, SEEK_SET);
    while ((nread = readall(fd, s, BUFSIZE)) > 0) {
        writeall(STDOUT_FILENO, s, nread);
    }
    close(fd);
    if (fdTemp != -1) {
        close(fdTemp);
    }
    return;
}

/* many - if there were many input files; 
lines - if user wanted not default 10 */

/* open files, check options/posibilities, invokes tail in a proper way*/
void useTail(char * fileFrom, char * lines, int many) {
    int fromfd = -1;
    if ((fromfd = open(fileFrom, O_RDONLY)) != -1) {
        if (many)
            tail(fromfd, getNumber(lines), fileFrom);
        else
            tail(fromfd, getNumber(lines), NULL);
        close(fromfd);
    } else {
        if (errno == ENOENT)
            printf("Plik %s nie istnieje\n", fileFrom);
        else if (errno == EACCES)
            printf("Brak dostępu do pliku %s\n", fileFrom);
    }
}

int main(int argc, char **argv) {
    /* ugly, but works*/
    if (argc == 1) {
        tail(STDOUT_FILENO, 10, NULL);
    } else if (argc == 2) {
        if (argv[1][0] == '-') {
            tail(STDOUT_FILENO, getNumber(argv[1]), NULL);
        } else
            useTail(argv[1], "-10", 0);
    } else if (argc == 3) {
        int i = 2;
        if (argv[1][0] == '-') {
            for (i = 2; i < argc; i++) {
                useTail(argv[i], argv[1], 0);
            }
        } else {
            for (i = 1; i < argc; i++) {
                useTail(argv[i], "-10", 1);
            }
        }
    } else {
        int i = 2;
        if (argv[1][0] == '-') {
            printf("Unsupported option with many files");
        } else {
            for (i = 1; i < argc; i++) {
                useTail(argv[i], "-10", 1);
            }
        }
    }

    return 0;
}