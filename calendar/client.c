/*
 * udp client for calendar application
 * author: wukat
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
#include <netdb.h>
#include <pthread.h>

#define SERVERPORT 58394	
#define MAXBUFLEN 100
#define h_addr h_addr_list[0]

/* thread function, receiving and printing */
void * receive(void * args) {
    void ** tab = (void **) args;
    int numbytes, sockfd = *((int *) tab[0]);
    char sendBuf[MAXBUFLEN];
    struct sockaddr_in* their_addr = (struct sockaddr_in*) tab[1];
    unsigned int addr_len = sizeof (*their_addr);
    while (1) {
        if ((numbytes = recvfrom(sockfd, sendBuf, MAXBUFLEN - 1, 0,
                (struct sockaddr *) their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        } else {
            sendBuf[numbytes] = '\0';
            printf("%s\n", sendBuf);
        }
    }
}

void sendMessage(int sockfd, const char* buf, struct sockaddr_in* their_addr, pthread_t thread) {
    int numbytes;
    if ((numbytes = sendto(sockfd, buf, strlen(buf), 0,
            (struct sockaddr *) their_addr, sizeof *their_addr)) == -1) {
        perror("sendto");
        pthread_cancel(thread);
        exit(EXIT_FAILURE);
    }
}

void clearStdin() {
    unsigned int c;
    while ((c = fgetc(stdin)) != '\n' && c != EOF) {
    }
}

int createMeeting(char * sendBuf) {
    unsigned int inputDay, inputMonth, inputYear, inputMinute, inputHour;
    char inputBuf[MAXBUFLEN - 20];
    struct tm meetTime;

    printf("Insert day: ");
    if ((scanf("%u", &inputDay) == 0) || (inputDay < 1) || (inputDay > 31)) {
        printf("Wrong day, insert from 1 up to 31 \n");
        clearStdin();
        return 0;
    };
    clearStdin();
    printf("Insert month: ");
    if ((scanf("%u", &inputMonth) == 0) || (inputMonth < 1) || (inputMonth > 12)) {
        printf("Wrong month, insert from 1 up to 12 \n");
        clearStdin();
        return 0;
    };
    clearStdin();
    printf("Insert year: ");
    if ((scanf("%u", &inputYear) == 0) || (inputYear < 2012) || (inputYear > 3001)) {
        printf("Wrong year, insert from 2012 up to 3001 \n");
        clearStdin();
        return 0;
    };
    clearStdin();
    printf("Insert hour: ");
    if ((scanf("%u", &inputHour) == 0) || (inputHour < 0) || (inputHour > 23)) {
        printf("Wrong hour, insert from 0 up to 23 \n");
        clearStdin();
        return 0;
    };
    clearStdin();
    printf("Insert minuts: ");
    if ((scanf("%u", &inputMinute) == 0) || (inputMinute < 0) || (inputMinute > 59)) {
        printf("Wrong hour, insert from 0 up to 59 \n");
        clearStdin();
        return 0;
    };
    printf("Meeting name: ");
    clearStdin();
    fgets(inputBuf, MAXBUFLEN - 20, stdin);
    clearStdin(); /* if input to long - clear up stdin */
    meetTime.tm_sec = 0;
    meetTime.tm_min = inputMinute;
    meetTime.tm_hour = inputHour;
    meetTime.tm_mday = inputDay;
    meetTime.tm_mon = inputMonth - 1;
    meetTime.tm_year = inputYear - 1900;
    snprintf(sendBuf, MAXBUFLEN, "%ld %s", mktime(&meetTime), inputBuf);
    return 1;
}

int main(int argc, char *argv[]) {
    int sockfd, flag;
    struct hostent *he;
    int numbytes, chosen;
    unsigned int addr_len;
    char inputBuf[MAXBUFLEN - 20], sendBuf[MAXBUFLEN];
    struct sockaddr_in their_addr;
    void *args[2];
    pthread_t thread;
    int rc;

    args[0] = &sockfd;
    args[1] = &their_addr;
    if (argc != 2) {
        fprintf(stderr, "usage: talker hostname\n");
        exit(EXIT_FAILURE);
    }

    if ((he = gethostbyname(argv[1])) == NULL) {
        perror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, getprotobyname("UDP")->p_proto)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(SERVERPORT);
    their_addr.sin_addr = *((struct in_addr *) he->h_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
    addr_len = sizeof their_addr;

    /* login */
    do {
        flag = 1;
        printf("Insert username: ");
        scanf("%s", inputBuf);
        clearStdin();
        if ((inputBuf[0] >= 'a' && inputBuf[0] <= 'z') || (inputBuf[0] >= 'A' && inputBuf[0] <= 'Z')) {
            sendMessage(sockfd, inputBuf, &their_addr, thread);
            if ((numbytes = recvfrom(sockfd, sendBuf, MAXBUFLEN - 1, 0,
                    (struct sockaddr *) &their_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(EXIT_FAILURE);
            }
            sendBuf[numbytes] = '\0';
            if (strcmp(sendBuf, "Welcome") == 0) {
                flag = 0;
            }
            printf("%s\n", sendBuf);
        } else {
            printf("Your nick must start with a letter!\n");
        }
    } while (flag);

    rc = pthread_create(&thread, NULL, receive, (void *) &args);
    if (rc) {
        perror("Creating thread");
        exit(-1);
    }

    /* sending, user interaction */
    while (1) {
        printf("Menu:\n"
                "1. Show meetings\n"
                "2. New meeting\n"
                "3. Show users\n"
                "4. Create or join users group\n"
                "5. New group meeting\n"
                "6. Exit\n");
        scanf("%d", &chosen);
        clearStdin();
        switch (chosen) {
            case 1: /* show meetings */
                sendMessage(sockfd, "1", &their_addr, thread);
                sleep(1);
                break;
            case 2: /* new meeting */
                while (!createMeeting(sendBuf));
                sendMessage(sockfd, sendBuf, &their_addr, thread);
                break;
            case 3: /* show users */
                sendMessage(sockfd, "3", &their_addr, thread);
                sleep(1);
                break;
            case 4: /* create or join */
                sendMessage(sockfd, "5", &their_addr, thread);
                sleep(1);
                do {
                    printf("Group name: ");
                    rc = 0;
                    fgets(inputBuf, MAXBUFLEN - 20, stdin);
                    clearStdin();
                    if (!((inputBuf[0] >= 'a' && inputBuf[0] <= 'z') || (inputBuf[0] >= 'A' && inputBuf[0] <= 'z'))) {
                        printf("Groupname must start with a letter! Insert again!\n");
                        rc = 1;
                    }
                } while (rc);
                sendMessage(sockfd, inputBuf, &their_addr, thread);
                break;
            case 5: /* new group meeting */
                sendMessage(sockfd, "7", &their_addr, thread);
                sleep(1);
                do {
                    printf("Group name: ");
                    rc = 0;
                    fgets(inputBuf, MAXBUFLEN - 20, stdin);
                    clearStdin();
                    if (!((inputBuf[0] >= 'a' && inputBuf[0] <= 'z') || (inputBuf[0] >= 'A' && inputBuf[0] <= 'z'))) {
                        printf("Groupname must start with a letter! Insert again!\n");
                        rc = 1;
                    }
                } while (rc);
                sendMessage(sockfd, inputBuf, &their_addr, thread);
                while (!createMeeting(sendBuf));
                sendMessage(sockfd, sendBuf, &their_addr, thread);
                break;
            case 6: /* exit */
                sendMessage(sockfd, "4", &their_addr, thread); /* logout */
                pthread_cancel(thread);
                close(sockfd);
                exit(EXIT_SUCCESS);
            default:
                printf("Choose one of below! \n\n");
        }
    }

    return 0;
}