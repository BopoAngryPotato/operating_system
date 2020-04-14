all: a.out
.PHONY: all

a.out: source.c ../../lib/libutil.so
	gcc -L/home/pozhang/projects/operating_systems/lib -Wl,-rpath=/home/pozhang/projects/operating_systems/lib -Wall -g -pthread source.c -lutil

../../lib/libutil.so: parser.c parser.h container.c container.h
	make -C ../../lib

clean: 
	rm -f *.o *.out
	make -C ../../lib clean
