CPPFLAGS=-g -O2 -std=c++20 -Wall -Werror
INCLUDE=-Igrid -Iastar -Igrid_view -Ipath -Iastar_replanning -Iswsfp -I dstar_lite -Iargparse
EXE=output/main

all: $(EXE)

$(EXE): astar/* astar_replanning/* dstar_lite/* grid/* grid_view/* path/* swsfp/* main.cpp
	mkdir -p output
	g++ swsfp/SWSFP.cpp grid/Grid.cpp grid_view/GridView.cpp path/PathSearchResult.cpp main.cpp $(INCLUDE) $(CPPFLAGS) -o $(EXE)

.PHONY: run

run_example: $(EXE)
	$(EXE) den401d 150

run_huge_example: $(EXE)
	$(EXE) --map data/den401d.map --radius 5 --begin-x 244 --begin-y 25 --end-x 83 --end-y 25 --algorithm astar --search-result-output ./output/search-result-astar.json
	$(EXE) --map data/den401d.map --radius 5 --begin-x 244 --begin-y 25 --end-x 83 --end-y 25 --algorithm astar --search-result-output ./output/search-result-astar.json

run: $(EXE)
	$(EXE) --map data/arena.map --radius 100 --begin-x 4 --begin-y 32 --end-x 47 --end-y 19 $(ARGS)

clean:
	rm $(EXE)