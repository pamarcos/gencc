SRCS = $(wildcard *.cpp *.c)
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)

all: $(OBJS)

$(OBJS): clean

clean:
	rm -f *.o

dbg-%:
	@echo "Makefile: Value of $* = $($*)"
