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

/*copy*/

/* modified by wukat*/

void copy(char *from, char *to) {
    int fromfd = -1, tofd = -1;
    ssize_t nread;
    ssize_t nwrite;
    char buf[BUFSIZE];

    if ((fromfd = open(from, O_RDONLY)) != -1) {
        if ((tofd = open(to, O_WRONLY | O_CREAT | O_TRUNC,
                S_IRUSR | S_IWUSR)) != -1) {
            while ((nread = read(fromfd, buf, sizeof (buf))) > 0) {
                if ((nwrite = writeall(tofd, buf, nread)) == -1) {
                    perror("Błąd w zapisie");
                    return;
                }
            }
            if (nread == -1) {
                perror("Błąd w czytaniu pliku");
            }
            close(fromfd);
        } else {
            if (errno == ENOENT)
                printf("Plik drugi nie istnieje");
            else if (errno == EACCES)
                printf("Brak dostępu do drugiego pliku");
        }
        close(tofd);
    } else {
        if (errno == ENOENT)
            printf("Plik pierwszy nie istnieje");
        else if (errno == EACCES)
            printf("Brak dostępu do pierwszego pliku");
    }
    return;
}