CPPFLAGS=-g -O2 -std=c++20 -Wall -Werror
INCLUDE=-Igrid -Iastar -Igrid_view -Ipath -Iastar_replanning -Iswsfp -I dstar_lite
EXE=output/main

all: $(EXE)

$(EXE): astar/* astar_replanning/* dstar_lite/* grid/* grid_view/* path/* swsfp/* main.cpp
	g++ swsfp/SWSFP.cpp grid/Grid.cpp grid_view/GridView.cpp path/PathSearchResult.cpp main.cpp $(INCLUDE) $(CPPFLAGS) -o $(EXE)

.PHONY: run

run_example: $(EXE)
	$(EXE) den401d 150

run_huge_example: $(EXE)
	$(EXE) den401d 700

run: $(EXE)
	$(EXE) $(ARGS)

clean:
	rm $(EXE)