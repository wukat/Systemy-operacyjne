ex1: *.o
	gcc ex1open.o t.o -o ex1

*.o: ex1open.c t.c t.h
	gcc -Wall -ansi -pedantic -c ex1open.c t.c

.PHONY: clean

clean: 
	rm -f *.o
