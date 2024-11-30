CPPFLAGS=-g -O2 -std=c++20 -Wall -Werror
INCLUDE=-Igrid -Iastar -Igrid_view -Ipath -Iastar_replanning -Iswsfp -I dstar_lite

all: main

main: astar/* astar_replanning/* dstar_lite/* grid/* grid_view/* path/* swsfp/* main.cpp
	g++ swsfp/SWSFP.cpp grid/Grid.cpp grid_view/GridView.cpp path/PathSearchResult.cpp main.cpp $(INCLUDE) $(CPPFLAGS) -o main

.PHONY: run

run: main
	./main

clean:
	rm main