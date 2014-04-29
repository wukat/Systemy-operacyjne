/*author wukat*/
/*uses copied functions*/

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "writeall.h"
#include "special.h" /*includes only definition of buffersize*/
#include "copy1.h"
#include "t.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s from_pathname to_pathname\n", argv[0]);
        return 1;
    }
    timestart();
    copy(argv[1], argv[2]);
    timestop("Copy2");
    return 0;
}