all: a.out
.PHONY: all

a.out: source.o buffer.o ../../lib/libutil.so
	gcc -L/home/pozhang/projects/operating_system/lib -Wl,-rpath=/home/pozhang/projects/operating_system/lib -Wall -g -pthread source.o buffer.o -lutil

source.o: source.c
	gcc -c -g -Wall source.c

buffer.o: buffer.c buffer.h
	gcc -c -g -Wall buffer.c

../../lib/libutil.so: 
	make -C ../../lib

clean: 
	rm -f *.o *.out
	make -C ../../lib clean
