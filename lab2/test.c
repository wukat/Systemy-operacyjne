#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>	
#include "backward.h"

int main(int argc, char ** argv) {
    backward(argv[1]);
    return 0;
}
