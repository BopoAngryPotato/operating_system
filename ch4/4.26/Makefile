.RECIPEPREFIX = >

a.out: source.c ../../lib/libutil.so
> gcc -L/home/pozhang/projects/operating_systems/lib -Wl,-rpath=/home/pozhang/projects/operating_systems/lib -Wall -pthread source.c -lutil

../../lib/libutil.so:
> make -C ../../lib

clean: 
> rm -f *.o *.out
> make -C ../../lib clean
