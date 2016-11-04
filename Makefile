NAME = gencc
CXXFLAGS = -g -O0 -std=c++11 -Wall
CXXFLAGS += -fsanitize=address

all: $(NAME)

$(NAME): main.cpp
	$(CXX) -o $@ $(CXXFLAGS) $<

clean:
	rm -f $(NAME)

dbg-%:
	@echo "Makefile: Value of $* = $($*)"
