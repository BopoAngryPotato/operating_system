LIBDIR = $${HOME}/projects/operating_system/lib

CC = gcc
CFLAGS = -L$(LIBDIR) -Wl,-rpath=$(LIBDIR) -Wall -g -pthread
LIBS = -lutil

LINK_TARGET = a.out
SRCS = source.c

.PHONY: all clean $(LIBDIR)
all: $(LINK_TARGET) 
	@echo "Make done!"

$(LINK_TARGET): $(SRCS) $(LIBDIR)
	$(CC) $(CFLAGS) $(SRCS) $(LIBS)

$(LIBDIR):
	$(MAKE) -C $@ $(MAKECMDGOALS) 

clean: 
	rm -f *.o *.out
	$(MAKE) -C $(LIBDIR) clean
