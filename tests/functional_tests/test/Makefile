SRC = $(wildcard *.cpp *.c)
OBJ = $(SRC:.cpp=.o)
OBJ := $(OBJ:.c=.o)

all: $(OBJ)

$(OBJ): clean

clean:
	rm -f *.o

dbg-%:
	@echo "Makefile: Value of $* = $($*)"

.PHONY: clean