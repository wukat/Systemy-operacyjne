#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "backward.h"

int main(int argc, char ** argv) {
	backward(argv[1]);
	return 0;	
}
