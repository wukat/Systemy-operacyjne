#!/bin/bash
#author wukat

dd if=/dev/zero of=bigfile count=10k
gcc -Wall -ansi -pedantic copy3.c t.c -o copy3
./copy3 bigfile second
for i in 1 512 1024 1100 ; do
	echo "#define BUFSIZE $i" > special.h
	echo "Buffer set size to $i"
	gcc -Wall -ansi -pedantic copy2.c t.c -o copy2
	./copy2 bigfile second
done

rm second
rm bigfile
rm copy3 
rm copy2