/**
 * author wukat
 * cat function
*/

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "writeall.h"
#include "readall.h"

#define BUFSIZE 512

void cat(int fdFrom, char * option) {
	int number = 0;
	char buf[BUFSIZE];
	ssize_t nread, nwrite;
	if (option != NULL && option[0] == '-' && option[1] == 'n')
		number = 1;
	while ((nread = read(fdFrom, buf, sizeof(buf))) > 0) {
		if (number)
			fprintf(stdout, "%d. ", number++);
   		if ((nwrite = writeall(1, buf, nread)) == -1) {
    			perror("Błąd w zapisie");
    			return;
    		}
	}
}

void useCat(char * fileFrom, char * option) {
	int fromfd = -1;
	if (fileFrom != NULL) {
		if ((fromfd = open(fileFrom, O_RDONLY)) != -1) {
			cat(fromfd, option);
			close(fromfd);
		}
		else {
			if (errno == ENOENT)
				printf("Plik nie istnieje");
			else if (errno == EACCES)
				printf("Brak dostępu do pliku");
		}
	}
	else {
		cat(0, option);
	}
}

int main(int argc, char **argv) {
	if (argc == 1) {
		cat(0, NULL);
	}
	else {
		int i = 1;
		for (i = 1; i < argc; i++) {
			useCat(argv[i], NULL);
		}
	}
	
	return 0;
}
