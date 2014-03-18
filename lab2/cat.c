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

void cat(int fdFrom, int fdTo) {
	char buf[BUFSIZE];
	ssize_t n;
	while((n = readall(fdFrom, buf, sizeof(buf))) != -1) { /*doesn't work like cat, because of enters */
		writeall(fdTo, buf, n);
		if (n == 0)
			return;
	}
}

void useCat(char * fileFrom) {
	int fromfd = -1;
	if ((fromfd = open(fileFrom, O_RDONLY)) != -1) {
		cat(fromfd, 1);
	}
	else {
		if (errno == ENOENT)
			printf("Pliknie istnieje");
		else if (errno == EACCES)
			printf("Brak dostępu do pliku");
	}
}

int main(int argc, char **argv) {
	if (argc == 1) {
		cat(0, 1);
	}
	else if (argc == 2) {
		useCat(argv[1]);
	}
	
	return 0;
}