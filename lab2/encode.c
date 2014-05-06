#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>	
#include <string.h>
#include "writeall.h"
#include "readall.h"
#include <time.h>
#define BUFSIZE 512

/* encode sign */
int encodeSign(char sign, char * encoded) {
    int i;
    char c, n;
    n = rand() % 10;
    encoded[0] = sign;
    encoded[1] = (char) n + '0';
    for (i = 2; i < n + 2; i++) {
        c = rand() % 94 + 32;
        encoded[i] = c;
    }
    return n + 2;
}

/* encodes every sign and writes in file */
void encodeToFile(int tofd, char * buffer, int inBuffer) {
    char bufSmall[11];
    int i, length;
    for (i = 0; i < inBuffer; i++) {
        length = encodeSign(buffer[i], bufSmall);
        if (writeall(tofd, bufSmall, length) == -1) {
            perror("Error while writing to file");
            exit(EXIT_FAILURE);
            ;
        }
    }
}

/* read from file to buffer, for buffer invokes encodeToFile */
void encode(int fromfd, int tofd) {
    char buf[BUFSIZE];
    int n;
    while ((n = readall(fromfd, buf, BUFSIZE)) > 0) {
        encodeToFile(tofd, buf, n);
    }
    if (n == -1) {
        perror("Error while reading");
        exit(EXIT_FAILURE);
    }
}

/* open files and invokes encode */
void useEncode(char * fileFrom, char * fileTo) {
    int fromfd = -1, tofd = -1;
    if ((fromfd = open(fileFrom, O_RDONLY)) != -1) {
        if ((tofd = open(fileTo, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND)) != -1) {
            encode(fromfd, tofd);
            close(tofd);
        } else {
            if (errno == ENOENT)
                printf("File %s doesn't exist\n", fileTo);
            else if (errno == EACCES)
                printf("No access to %s\n", fileTo);
        }
        close(fromfd);
    } else {
        if (errno == ENOENT)
            printf("File %s doesn't exist\n", fileFrom);
        else if (errno == EACCES)
            printf("No access to %s\n", fileFrom);
    }

}

int main(int argc, char ** argv) {
    if (argc < 3) {
        printf("Use with 2 arguments - pathname to 2 files");
    } else {
        srand(time(NULL));
        useEncode(argv[1], argv[2]);
    }
    return 0;
}