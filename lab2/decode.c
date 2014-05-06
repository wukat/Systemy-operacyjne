/** author wukat */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>	
#include <string.h>
#include "writeall.h"
#include "readall.h"
#define BUFSIZE 512 /*at least about 15*/

/* return number of signs to use next time, if buffer was full*/

/* decodes buffer and writes in file*/
int decodeToFile(int tofd, char * buffer, int inBuffer, int offset) {
    char bufSmall[BUFSIZE];
    int i = offset, length = 0, count = 0;
    if (offset == -1) {
        length = buffer[0] - '0';
        i = length + 1;
    }
    while (i < inBuffer) {
        bufSmall[count++] = buffer[i];
        if (i + 1 == inBuffer) {
            offset = -1;
            i += length + 1;
        } else {
            length = buffer[++i] - '0';
            i += length + 1;
            offset = i - BUFSIZE;
        }
    }
    if (writeall(tofd, bufSmall, count) == -1) {
        perror("Error while writing to file");
        exit(EXIT_FAILURE);
        ;
    }
    return offset;
}

/* read buffer and invoke decodeToFile for every */
void decode(int fromfd, int tofd) {
    char buf[BUFSIZE];
    int n, offset = 0;
    while ((n = readall(fromfd, buf, BUFSIZE)) > 0) {
        offset = decodeToFile(tofd, buf, n, offset);
    }
    if (n == -1) {
        perror("Error while reading");
        exit(EXIT_FAILURE);
    }
}

/* open files and invokes decode*/
void useDecode(char * fileFrom, char * fileTo) {
    int fromfd = -1, tofd = -1;
    if ((fromfd = open(fileFrom, O_RDONLY)) != -1) {
        if ((tofd = open(fileTo, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND)) != -1) {
            decode(fromfd, tofd);
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
        useDecode(argv[1], argv[2]);
    }
    return 0;
}