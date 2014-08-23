/*
 * author wukat
 * reads and sends file to clients
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MYPORT 58394	/* the port users will be connecting to*/

#define BACKLOG 10	 /* how many pending connections queue will hold */
#define BUFSIZE 256

void sigchld_handler(int s) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[]) {
    int sockfd, new_fd, fileFd; /* listen on sock_fd, new connection on new_fd */
    struct sockaddr_in my_addr, their_addr; 
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1, nread, flag = 1;
    char buf[BUFSIZE];


    if (argc != 2) {
        fprintf(stderr, "usage: server filename\n");
        exit(EXIT_FAILURE);
    }
    
    if ((fileFd = open(argv[1], O_RDONLY)) == -1) {
        perror("File opening");
        exit(EXIT_FAILURE);
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    my_addr.sin_family = AF_INET; /* host byte order */
    my_addr.sin_port = htons(MYPORT); /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* automatically fill with my IP */
    memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof my_addr) == -1) {
        perror("bind");
        exit(1);
    }
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    sin_size = sizeof their_addr;
    while ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size)) != -1) {
        if (send(new_fd, argv[1], strlen(argv[1]), 0) == -1) {
            perror("send");
            flag = 0;
        }
        if (recv(new_fd, buf, 1, 0) == -1) {
            perror("confirm");
            flag = 0;
        }
        while (flag) {
            if ((nread = read(fileFd, buf, BUFSIZE)) == -1) {
                perror("File reading");
                flag = 0;
            }
            if (nread == 0) {
                flag = 0;
                buf[0] = EOF;
                if (send(new_fd, buf, 1, 0) == -1)
                    perror("send");
            } else {
                if (send(new_fd, buf, nread, 0) == -1) {
                    perror("send");
                    flag = 0;
                }
            }
        }
        lseek(fileFd, 0, SEEK_SET);
        flag = 1;
    }

    close(fileFd);
    close(new_fd);

    return 0;
}