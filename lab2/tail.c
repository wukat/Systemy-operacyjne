/** atuhor wukat */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>	
#include <string.h>
#include "backward.h"
#include "readall.h"
#include "writeall.h"

int mkostemp(char *template, int flags);

/* use './tail -5 path' to see 5 last lines of file (default 10) */
/* without any file given, uses stdin */
int getNumber(char * str) { /* from string like -5 makes integer 5*/
	int size = strlen(str);
	int i = 0;
	int result = 0;
	for (i = 1; i < size; i++) {
		result *= 10;
		result += str[i] - '0';
	}
	return result;
}

/** creates tmp file for stdin */
int createTempForUserInput() {
	char fileNameBuff[11];
	char s[2048];
	int nread = -1, nwrite = -1, fdTemp = -1;
	strncpy(fileNameBuff,"/tmp/tailXXXXXX",21);
    if ((fdTemp = mkostemp(fileNameBuff, O_APPEND)) > 1) {
    	while ((nread = read(STDIN_FILENO, s, sizeof(s))) > 0) {
   			if ((nwrite = writeall(fdTemp, s, nread)) == -1) {
    			perror("Błąd w zapisie");
    			return -1;
    		}
		}
	} else {
		perror("Nie udało się utworzyć pliku tymczasowego");
	}
	return fdTemp;
}

void tail(int fd, int line, char * name) {
	char s[2048], c;
    int i, fdTemp = -1;
    off_t where;
    
     
    if (fd == STDOUT_FILENO) { /* saves in temporary file user's input*/
		if ((fdTemp = createTempForUserInput()) <= 1)
			return; 
		else 
			fd = fdTemp; /*change fd to temp file*/
    }
    where = lseek(fd, 0, SEEK_END);
    i = sizeof(s) - 1;
    s[i] = '\0';
    do {
		switch (pread(fd, &c, 1, where)) {
    	case 1:
    		if (c == '\n') {
    			line--;
    		}
    		if (i <= 0) {
    			errno = E2BIG;
    			/* handle the error */
    		}
    		s[--i] = c;
    		break;
    	case -1:
    		/* handle the error */
    		break;
    	default: /* impossible */
    		errno = 0;
    		/* handle the error */
    	}
    } while (where-- > 0 && line >= 0 && i > 0);
    if (name != NULL) 
    	printf("==> %s <==", name);
    if (i == 0) { /* if buffer is full, inform*/
    	printf("Bufer pełny - w trakcie %d. linii \n", 9-line);
    }
   	printf("%s", &s[i]);
    close(fd);
    if (fdTemp != -1) {
    	close(fdTemp);
    }
    return;
}

/* many - if there were many input files; 
lines - if user wanted not default 10 */
void useTail(char * fileFrom, char * lines, int many) { 
	int fromfd = -1;
	if ((fromfd = open(fileFrom, O_RDONLY)) != -1) {
		if (many)
			tail(fromfd, getNumber(lines), fileFrom);
		else 
			tail(fromfd, getNumber(lines), NULL);
		close(fromfd);
	}
	else {
		if (errno == ENOENT)
			printf("Plik %s nie istnieje\n", fileFrom);
		else if (errno == EACCES)
			printf("Brak dostępu do pliku %s\n", fileFrom);
	}
}

int main(int argc, char **argv) {
	/* ugly, but works*/
	if (argc == 1) {
		tail(STDOUT_FILENO, 10, NULL);
	} else if (argc == 2) {
		if (argv[1][0] == '-') {
			tail(STDOUT_FILENO, getNumber(argv[1]), NULL);
		} else 
			useTail(argv[1], "-10", 0);
	} else if (argc == 3) {
		int i = 2;
		if (argv[1][0] == '-') {
			for (i = 2; i < argc; i++) {
				useTail(argv[i], argv[1], 0);
			}
		}
		else {
			for (i = 1; i < argc; i++) {
				useTail(argv[i], "-10", 1);
			}
		}
	} else {
		int i = 2;
		if (argv[1][0] == '-') {
			for (i = 2; i < argc; i++) {
				useTail(argv[i], argv[1], 1);
			}
		}
		else {
			for (i = 1; i < argc; i++) {
				useTail(argv[i], "-10", 1);
			}
		}
	}

	return 0;
}