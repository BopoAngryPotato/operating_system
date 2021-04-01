LIBDIR = $${HOME}/projects/operating_system/lib

CC = g++
C_COMPILE_FLAGS = -Wall -g -c 
C_LINK_FLAGS = -L$(LIBDIR) -Wl,-rpath=$(LIBDIR) -Wall -pthread
LIBS = -lutil

LINK_TARGET = a.out
OBJS = main.o

.PHONY: all clean $(LIBDIR)
all: $(LINK_TARGET) 
	@echo "Make done!"

$(LINK_TARGET): $(OBJS) $(LIBDIR)
	$(CC) $(C_LINK_FLAGS) $(OBJS) $(LIBS)

$(OBJS): %.o: %.cpp
	$(CC) $(C_COMPILE_FLAGS) $<

$(LIBDIR):
	$(MAKE) -C $@ $(MAKECMDGOALS)

clean: 
	rm -f *.o *.out
	$(MAKE) -C $(LIBDIR) clean
