/*
 * udp server for calendar application
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
#include <string>
#include <ctime>
#include <vector>
#include <list>
#include <iostream>
#include <pthread.h>

#define MYPORT 58394	// the port users will be connecting to
#define MAXBUFLEN 100

pthread_mutex_t mutex;

struct meeting {
    std::string name;
    time_t date;
    struct user* user;
    struct group* group;
};

struct user {
    std::string username;
    struct sockaddr_in addr;
    std::list<struct meeting> meetings;
    bool logged; /* if haven't sent logout message */
};

struct group {
    std::string name;
    std::list<struct user> users;
    std::list<struct meeting> meetings;
};

void send(int sockfd, const char * buf, struct sockaddr_in &their_addr) {
    int numbytes;
    if ((numbytes = sendto(sockfd, buf, strlen(buf), 0,
            (struct sockaddr *) &their_addr, sizeof their_addr)) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
}

/* sending meeting to given addr */
void sendMeetAddr(struct sockaddr_in& addr, time_t date, const char * name, int sockfd) {
    char buf[MAXBUFLEN];
    snprintf(buf, MAXBUFLEN - 1, "%s %s", ctime(&date), name);
    send(sockfd, buf, addr);
}

/* sending meeting to given group's users */
void sendMeetGroup(struct group& group, time_t date, const char * name, int sockfd) {
    for (std::list<struct user>::iterator iter = group.users.begin(); iter != group.users.end(); iter++) {
        sendMeetAddr(iter->addr, date, name, sockfd);
    }
}

struct user* getUser(struct sockaddr_in their_addr, std::vector<struct user>& users) {
    for (unsigned int i = 0; i < users.size(); i++) {
        if (users[i].addr.sin_addr.s_addr == their_addr.sin_addr.s_addr &&
                users[i].addr.sin_port == their_addr.sin_port) {
            return &users[i];
        }
    }
    return NULL;
}

struct group* getGroup(const char * name, std::vector<struct group>& groups) {
    for (unsigned int i = 0; i < groups.size(); i++) {
        if (strcmp(groups[i].name.c_str(), name) == 0) {
            return &groups[i];
        }
    }
    return NULL;
}

/* sending users to given addr */
void sendUsers(std::vector<struct user> users, struct sockaddr_in& their_addr, int sockfd) {
    pthread_mutex_lock(&mutex);
    for (std::vector<struct user>::iterator iter = users.begin(); iter != users.end(); iter++) {
        send(sockfd, iter->username.c_str(), their_addr);
    }
    pthread_mutex_unlock(&mutex);
}

/* sending groups to given addr */
void sendGroups(std::vector<struct group> groups, struct sockaddr_in& their_addr, int sockfd) {
    pthread_mutex_lock(&mutex);
    for (std::vector<struct group>::iterator iter = groups.begin(); iter != groups.end(); iter++) {
        send(sockfd, iter->name.c_str(), their_addr);
    }
    pthread_mutex_unlock(&mutex);
}

/* sending meetings to given addr */
void sendMeetings(struct user user, struct sockaddr_in& their_addr, int sockfd) {
    pthread_mutex_lock(&mutex);
    for (std::list<struct meeting>::iterator iter = user.meetings.begin(); iter != user.meetings.end(); iter++) {
        sendMeetAddr(their_addr, iter->date, iter->name.c_str(), sockfd);
    }
    pthread_mutex_unlock(&mutex);
}

/* adding user */
bool addIfNotIn(std::vector<struct user>& users, struct sockaddr_in their_addr, char * buf) {
    pthread_mutex_lock(&mutex);
    for (unsigned int i = 0; i < users.size(); i++) {
        if (strcmp(users[i].username.c_str(), buf) == 0) {
            if (!users[i].logged) {
                users[i].addr = their_addr;
                users[i].logged = true;
                pthread_mutex_unlock(&mutex);
                return true;
            }
            pthread_mutex_unlock(&mutex);
            return false;
        }
    }
    struct user newUser;
    newUser.username = buf;
    newUser.addr = their_addr;
    newUser.logged = true;
    users.push_back(newUser);
    pthread_mutex_unlock(&mutex);
    std::cout << "Added new user\n";
    return true;
}

/* adding user to group/creating group */
void addUserToGroup(struct user user, std::vector<struct group>& groups, const char* grName) {
    pthread_mutex_lock(&mutex);
    struct group * tempGr = getGroup(grName, groups);
    if (tempGr == NULL) {
        struct group newGr;
        newGr.name = grName;
        newGr.users.push_back(user);
        groups.push_back(newGr);
    } else {
        bool flag = true;
        for (std::list<struct user>::iterator iter = tempGr->users.begin(); iter !=
                tempGr->users.end() && flag; iter++) {
            if (user.username == iter->username) {
                flag = false;
            }
        }
        if (flag)
            tempGr->users.push_back(user);
    }
    pthread_mutex_unlock(&mutex);
    std::cout << "Added user to group (if wasn't in yet) \n";
}

/* inserting new meeting */
void insertInOrder(struct meeting meet, std::list<struct meeting>& meetings) {
    bool flag = false;
    for (std::list<struct meeting>::iterator iter = meetings.begin(); iter != meetings.end() && !flag; iter++) {
        if (meet.date < iter->date) {
            meetings.insert(iter, meet);
            flag = true;
        }
    }
    if (!flag) {
        meetings.push_back(meet);
    }
}

/* adding meeting (user meeting) */
void addMeeting(std::list<struct meeting>& meetings, std::vector<struct user>& users, char* buf,
        struct sockaddr_in their_addr) {
    time_t temp_time;
    char buf2[MAXBUFLEN];
    int j = 0;
    bool flag = true;

    if (sscanf(buf, "%ld", &temp_time) == 1) {
        for (unsigned int i = 0; i < strlen(buf); i++) {
            if (!flag) {
                buf2[j++] = buf[i];
            }
            if (buf[i] < '0' || buf[i] > '9') {
                flag = false;
            }
        }
        buf2[j] = '\0';
        pthread_mutex_lock(&mutex);
        if (temp_time > time(NULL)) {
            struct meeting meet;
            struct user* user = getUser(their_addr, users);
            meet.date = temp_time;
            meet.name = buf2;
            meet.user = user;
            meet.group = NULL;
            insertInOrder(meet, meetings);
            insertInOrder(meet, user->meetings);
        }
        pthread_mutex_unlock(&mutex);
    }
    std::cout << "Added meeting\n";
}

/* adding group meeting */
void addMeetingGr(std::list<struct meeting>& meetings, struct group groupArg, std::vector<struct group>& groups, char* buf) {
    time_t temp_time;
    char buf2[MAXBUFLEN];
    int j = 0;
    bool flag = true;

    if (sscanf(buf, "%ld", &temp_time) == 1) {
        for (unsigned int i = 0; i < strlen(buf); i++) {
            if (!flag) {
                buf2[j++] = buf[i];
            }
            if (buf[i] < '0' || buf[i] > '9') {
                flag = false;
            }
        }
        buf2[j] = '\0';
        pthread_mutex_lock(&mutex);
        if (temp_time > time(NULL)) {
            struct meeting meet;
            struct group* group = getGroup(groupArg.name.c_str(), groups);
            meet.date = temp_time;
            meet.name = buf2;
            meet.user = NULL;
            meet.group = group;
            insertInOrder(meet, meetings);
            insertInOrder(meet, group->meetings);
        }
        pthread_mutex_unlock(&mutex);
    }
    std::cout << "Added group meeting\n";
}

/* thread function */
void * sendActualizations(void * args) {
    void ** tab = (void **) args;
    int toRemove, sockfd = *((int *) tab[0]);
    std::vector<struct user>* users = (std::vector<struct user>*) tab[1];
    std::list<struct meeting>* meetings = (std::list<struct meeting>*) tab[2];
    bool flag = true;

    while (true) {
        flag = true;
        sleep(60);
        toRemove = 0;
        pthread_mutex_lock(&mutex);
        for (std::list<struct meeting>::iterator iter = meetings->begin(); iter != meetings->end() && flag; iter++) {
            if (iter->date < time(NULL) + 120) {
                if (iter->user != NULL) {
                    sendMeetAddr(iter->user->addr, iter->date, iter->name.c_str(), sockfd);
                    iter->user->meetings.pop_front();
                } else if (iter->group != NULL) {
                    sendMeetGroup(*(iter->group), iter->date, iter->name.c_str(), sockfd);
                    iter->group->meetings.pop_front();
                }
                toRemove++;
            } else {
                flag = false;
            }
        }
        for (int i = 0; i < toRemove; i++) {
            meetings->erase(meetings->begin());
        }

        time_t temp = time(NULL);
        struct tm* accTime = localtime(&temp);
        if (accTime->tm_hour == 0 && accTime->tm_min == 0) {
            for (std::vector<struct user>::iterator iter = users->begin(); iter != users->end(); iter++) {
                send(sockfd, "It's midnight!", iter->addr);
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

/* check if user is in vector and if so, remove it */
bool isInAndRemove(std::list<struct user>& lst, struct user user) {
    pthread_mutex_lock(&mutex);
    for (std::list<struct user>::iterator iter = lst.begin();
            iter != lst.end(); iter++) {
        if ((iter)->username == user.username) {
            lst.erase(iter);
            pthread_mutex_unlock(&mutex);
            return true;
        }
    }
    pthread_mutex_unlock(&mutex);
    return false;
}

int main(void) {
    int sockfd, numbytes, rc;
    struct sockaddr_in my_addr, their_addr;
    socklen_t addr_len;
    char buf[MAXBUFLEN];
    std::vector<struct user> users;
    std::vector<struct group> groups;
    std::list<struct meeting> meetings;
    std::list<struct user> usersAddingGroup; /* list of adding group, when somebody 
                                                 sends message that will be adding, 
                                                 is saved here*/
    std::list<struct user> usersAddingGroupMessage; /* list of adding group message, when somebody 
                                                 sends message that will be adding group message, 
                                                 is saved here*/
    std::list<std::pair<struct user, struct group> > toAddMessage;
    /* list of pairs - user and group - when somebody's adding new group meeting */
    void * args[3] = {&sockfd, &users, &meetings}; /* args for thread */
    pthread_t thread;

    setbuf(stdout, NULL);
    pthread_mutex_init(&mutex, NULL);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, getprotobyname("UDP")->p_proto)) == -1) {
        perror("socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof my_addr) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    rc = pthread_create(&thread, NULL, sendActualizations, (void *) &args);
    if (rc) {
        perror("Creating thread");
        exit(-1);
    }

    /* main loop, too complicated to explain */
    addr_len = sizeof my_addr;
    while (1) {
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0,
                (struct sockaddr *) &their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        buf[numbytes] = '\0';
        struct user * currUser = getUser(their_addr, users);
        if (buf[0] < '0' || buf[0] > '9') { /* logging in or adding group or choosing group */
            if (currUser == NULL || !isInAndRemove(usersAddingGroup, *currUser)) {
                if (currUser == NULL || !isInAndRemove(usersAddingGroupMessage, *currUser)) {
                    if (!addIfNotIn(users, their_addr, buf)) {
                        send(sockfd, "Username in use!", their_addr);
                    } else {
                        send(sockfd, "Welcome", their_addr);
                    }
                } else {
                    struct group* tempGr = getGroup(buf, groups);
                    if (tempGr == NULL) {
                        addUserToGroup(*currUser, groups, buf);
                    }
                    tempGr = getGroup(buf, groups);
                    std::pair<struct user, struct group> tempPair;
                    tempPair.first = *currUser;
                    tempPair.second = *tempGr;
                    toAddMessage.push_back(tempPair);
                }
            } else {
                addUserToGroup(*currUser, groups, buf);
            }
        } else if (currUser != NULL) {
            if (numbytes < 3) {
                if (buf[0] == '1') { /* show meetings */
                    sendMeetings(*currUser, their_addr, sockfd);
                } else if (buf[0] == '3') { /* show users */
                    sendUsers(users, their_addr, sockfd);
                } else if (buf[0] == '4') { /* logout */
                    currUser->logged = false;
                } else if (buf[0] == '5') { /* group op 1 */
                    usersAddingGroup.push_back(*currUser);
                    sendGroups(groups, their_addr, sockfd);
                } else if (buf[0] == '7') { /* group op 2 */
                    usersAddingGroupMessage.push_back(*currUser);
                    sendGroups(groups, their_addr, sockfd);
                }
            } else {
                bool adding = true;
                for (std::list<std::pair<struct user, struct group> >::iterator iter = toAddMessage.begin();
                        adding == true && iter != toAddMessage.end(); iter++) {
                    if (iter->first.username == currUser->username) {
                        adding = false;
                        addMeetingGr(meetings, iter->second, groups, buf);
                        toAddMessage.erase(iter);
                    }
                }
                if (adding) {
                    addMeeting(meetings, users, buf, their_addr);
                }
            }
        }
    }
    close(sockfd);
    pthread_mutex_destroy(&mutex);
    return 0;
}