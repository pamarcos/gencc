SRCS = $(shell find . -iname "*.cpp")
OBJS = $(SRCS:.cpp=.o)

all: clean $(OBJS)

clean:
	rm -f *.o

dbg-%:
	@echo "Makefile: Value of $* = $($*)"
