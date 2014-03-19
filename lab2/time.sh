#!/bin/bash
#author wukat

dd if=/dev/zero of=bigfile count=10k
gcc -Wall -ansi -pedantic copy3.c t.c -o copy3
./copy3 bigfile second.second
echo "#define BUFSIZE 1" > special.h
echo "Buffer set size to 1"
gcc -Wall -ansi -pedantic checkTimes.c t.c -o copy2
./copy2 bigfile second1.second
echo "#define BUFSIZE 512" > special.h
echo "Buffer set size to 512"
gcc -Wall -ansi -pedantic checkTimes.c t.c -o copy2
./copy2 bigfile second2.second
echo "#define BUFSIZE 1024" > special.h
echo "Buffer size set to 1024"
gcc -Wall -ansi -pedantic checkTimes.c t.c -o copy2
./copy2 bigfile second3.second
echo "#define BUFSIZE 1100" > special.h
echo "Buffer size set to 1100"
gcc -Wall -ansi -pedantic checkTimes.c t.c -o copy2
./copy2 bigfile second4.second
rm *.second
rm bigfile