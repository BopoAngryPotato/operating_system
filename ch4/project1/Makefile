all: a.out
.PHONY: all

a.out: source.c ../../lib/libutil.so
	gcc -L/home/pozhang/projects/operating_system/lib -Wl,-rpath=/home/pozhang/projects/operating_system/lib -Wall -g -pthread source.c -lutil

../../lib/libutil.so:
	make -C ../../lib

clean: 
	rm -f *.o *.out
	make -C ../../lib clean
