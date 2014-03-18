/*
 * copy1.c
 * 
 * based on: 
 *
 * Miscellaneous examples for Chap. 2
 * AUP2, Chap. 2
 *
 * modified by WTA
 */
 
/*[copy-with-bug]*/
/* modified by wukat*/
 
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

 
#define BUFSIZE 512
 
void copy(char *from, char *to)  /* has a bug ??*/
{
	int fromfd = -1, tofd = -1;
	ssize_t nread;
	ssize_t nwrite;
	char buf[BUFSIZE];
 
	if ((fromfd = open(from, O_RDONLY)) != -1) {
		if ((tofd = open(to, O_WRONLY | O_CREAT | O_TRUNC,
				S_IRUSR | S_IWUSR)) != -1) {
			while ((nread = read(fromfd, buf, sizeof(buf))) > 0)
	    		if ((nwrite = write(tofd, buf, nread)) == -1) {
	    			perror("Błąd w zapisie");
	    			break;
	    		}
	    	if (nread == -1) {
	    		perror("Błąd w czytaniu pliku");
	    	}
        	close(fromfd);
    		}
        	else {
        		if (errno = ENOENT)
					printf("Plik drugi nie istnieje");	
				else if (errno = EACCES)
					printf("Brak dostępu do drugiego pliku");
			}
		close(tofd);
	}
	else {
		if (errno = ENOENT)
			printf("Plik pierwszy nie istnieje");
		else if (errno = EACCES)
			printf("Brak dostępu do pierwszego pliku");
	}
	return;
}
 
int main(int argc, char **argv){
	if (argc != 3)
	{
		fprintf(stderr,"usage: %s from_pathname to_pathname\n", argv[0]);
		return 1;
	}
	copy(argv[1], argv[2]);
	return 0;
}