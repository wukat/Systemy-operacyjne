/*
 * author wukat
 * reads and saves file from server
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "writeall.h"

#define PORT 58394 /* the port client will be connecting to */
#define BUFSIZE 256

int main(int argc, char *argv[]) {
    int sockfd, numbytes, fileFd, flag = 1;
    char buf[BUFSIZE];
    struct hostent *he;
    struct sockaddr_in their_addr; /* connector's address information */

    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    if ((he = gethostbyname(argv[1])) == NULL) { /* get the host info */
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET; /* host byte order */
    their_addr.sin_port = htons(PORT); /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *) he->h_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

    if (connect(sockfd, (struct sockaddr *) &their_addr, sizeof their_addr) == -1) {
        perror("connect");
        exit(1);
    }

    if ((numbytes = recv(sockfd, buf, BUFSIZE, 0)) == -1) {
        perror("recv");
        exit(1);
    } else {
        if ((fileFd = open(buf, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
            perror("File creating");
            exit(EXIT_FAILURE);
        }
    }
    if (send(sockfd, "O", 1, 0) == -1) {
        perror("Confirm");
        flag = 0;
    }

    while (flag) {
        if ((numbytes = recv(sockfd, buf, BUFSIZE, 0)) == -1) {
            perror("recv");
            flag = 0;
        } else if (numbytes == 1) {
            flag = 0;
        } else {
            writeall(fileFd, buf, numbytes);
            if (buf[numbytes - 1] == EOF) {
                flag = 0;
            }
        }
    }

    close(fileFd);
    close(sockfd);

    return 0;
}