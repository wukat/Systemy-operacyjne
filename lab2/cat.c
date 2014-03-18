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
/*
void cat(int fdFrom) {
	char buf[BUFSIZE];
	ssize_t n;
	while((n = readall(fdFrom, buf, sizeof(buf))) != -1) { 
		writeall(1, buf, n);
		if (n == 0)
			return;
	}
}*/

/* second version */
void cat2(char * fileFrom, char * option) {
	char buf[BUFSIZE];
	FILE* f = NULL;
	char * line = NULL;
	int number = 0;
	if (option[0] == '-' && option[1] == 'n')
		number = 1;
	if (fileFrom != NULL) {
		if ((f = fopen(fileFrom, "r")) != NULL) {
			while((line = fgets(buf, 512, f)) != NULL) {
				if (number)
					fprintf(stdout, "%d. ", number++);
				fputs(line, stdout);
			}
		} else 
			perror("Opening file failed");
	}
	else {
		while((line = fgets(buf, 512, stdin)) != NULL) {
			if (number)
				fprintf(stdout, "%d", number++);
			fputs(line, stdout);
		}
	}

}
/*
void useCat(char * fileFrom) {
	int fromfd = -1;
	if ((fromfd = open(fileFrom, O_RDONLY)) != -1) {
		cat(fromfd);
		close(fromfd);
	}
	else {
		if (errno == ENOENT)
			printf("Pliknie istnieje");
		else if (errno == EACCES)
			printf("Brak dostępu do pliku");
	}
}*/

int main(int argc, char **argv) {
	if (argc == 1) {
		/*cat(0);*/
		cat2(NULL, NULL);
	}
	else if (argc == 2) {
		if (argv[1][0] != '-')
			cat2(argv[1], NULL);
		else
			cat2(NULL, argv[1]);
		/*useCat(argv[1]);*/
	} else if (argc == 3) {
		cat2(argv[2], argv[1]);
	}
	
	return 0;
}