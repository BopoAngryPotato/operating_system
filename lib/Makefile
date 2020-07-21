.RECIPEPREFIX = >

CC = gcc
C_COMPILE_FLAGS =  -c -g -Wall -fpic
C_LINK_FLAGS = -shared

LINK_TARGET = libutil.so
OBJS = container.o parser.o
DEPS = $(subst .o,.h,$(OBJS))

.PHONY: all clean

all: $(LINK_TARGET)
>	@echo "Make libraries done!"

$(LINK_TARGET): $(OBJS)
> $(CC) $(C_LINK_FLAGS) $^ -o $@

%.o: %.c $(DEPS) 
> $(CC) $(C_COMPILE_FLAGS) $^ 

clean:
> rm -f *.o *.so
