CPPFLAGS=-g -O2 -std=c++20 -Wall -Werror
INCLUDE=-Igrid -Iastar

all: main

main: main.cpp grid/Grid.cpp
	g++ main.cpp grid/Grid.cpp $(INCLUDE) $(CPPFLAGS) -o main

.PHONY: run

run: main
	./main