#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "t.h"

/*modified by wukat*/
#define BUFSIZE 1024

int main(int argc, char **argv) {
    int f1, c;
    char b[BUFSIZE], *n1;

    c = 10;
    n1 = argv[1];

    timestart();
    if ((f1 = open(n1, O_RDONLY)) != -1) {
        if (read(f1, b, c) != -1) {

            printf("%s: Przeczytano %d znakow z pliku %s: \"%s\"\n",
                    argv[0], c, n1, b);
        } else {
            close(f1);
            printf("%s \n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        close(f1);
        timestop("koniec");
    } else {
        printf("%s \n", strerror(errno));
        timestop("błąd \n");
        exit(EXIT_FAILURE);
    }
    return (0);
}
