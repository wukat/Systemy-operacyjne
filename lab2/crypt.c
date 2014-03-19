#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>	
#include <string.h>
#include "writeall.h"
#define BUFSIZE 512

void cryptToFile(int tofd, char * buffer, int inBuffer) {
	char bufSmall[11];
	int i;
	for (i = 0; i < inBuffer; i++) {
		
	}
}

void crypt(int fromfd, int tofd) {
	char buf[BUFSIZE];
	int n;
	while ((n = writeall(fromfd, buf, BUFSIZE)) > 0) {
		cryptToFile(tofd, buf, n);
	} else {
		if (n == -1)
			perror("Error while reading");
	}
}

void useCrypt(char * fileFrom, char * fileTo) {
	int fromfd = -1, tofd = -1;
	if ((fromfd = open(fileFrom, O_RDONLY)) != -1) {
		if ((tofd = open(fileTo, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND)) != -1) {
			crypt(fromfd, tofd);
			close(tofd);
		} 
		else {
			if (errno == ENOENT)
				printf("Plik %s nie istnieje\n", fileTo);
			else if (errno == EACCES)
				printf("Brak dostępu do pliku %s\n", fileTo);
		}
		close(fromfd);
	}
	else {
		if (errno == ENOENT)
			printf("Plik %s nie istnieje\n", fileFrom);
		else if (errno == EACCES)
			printf("Brak dostępu do pliku %s\n", fileFrom);
	}

}

int main(int argc, char ** argv) {
	if (argc < 3) {
		 printf("Use with 2 arguments - pathname to 2 files");
	} else {
		crypt(argv[1], argv[2]);
	}
	return 0;
}