.RECIPEPREFIX = >

libutil.so: container.o parser.o
> gcc -shared parser.o container.o -o libutil.so

parser.o: parser.c parser.h
> gcc -c -g -Wall -fpic parser.c

container.o: container.c container.h
> gcc -c -g -Wall -fpic container.c

clean:
> rm -f *.o *.so
