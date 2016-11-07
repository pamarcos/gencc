NAME = gencc
CXXFLAGS = -g -O0 -std=c++11 -Wall
CXXFLAGS += -fsanitize=address

all: $(NAME)

$(NAME): gencc.cpp
	$(CXX) -o $@ $(CXXFLAGS) $<

clean:
	rm -f $(NAME)

tests:
	cd tests && $(MAKE)

dbg-%:
	@echo "Makefile: Value of $* = $($*)"

.PHONY: clean tests all
