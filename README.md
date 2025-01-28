# Trajectory planning in a limited observable environment

![Alt Text](assets/demo.gif)

## Description

This repository contains all the materials for the reporting project on the
course of heuristic search algorithms. The purpose of this project is to implement and compare trajectory planning algorithms in a limited observable environment.

## Short problem statement
Given a grid with obstacles and a robot with limited observable area. The robot should pass from the starting point to the endpoint avoiding obstacles.

## Researched algorithms

1. A* with replanning at every turn
2. SWSFP
3. D* lite

## Repository structure

```
heuristic-search-project
    - code
        - astar (A* implementation)
        - astar_replanning (A* with replanning at every turn implementation)
        - dstar_lite (D* lite implementation)
        - swsfp (SWSFP implementation)
        - grid (Implementation of fully viewed grid)
        - grid_view (Adaptor for grid to simulate limited observable environment)
        - path (representation of found path)
        - argparse (header only library for command line arguments parsing)
        - main.cpp (entry point for algorithms)
        - animation.py (entry point for gif animations)
        - benchmarks.py (entry point for benchmarks)
    - report (progress report)
    - papers (articles describing algorithms)
    - assets (examples of planning)
```

## Run

The recommended way to your program is via python wrapper script, after compiling the project with `make all`.
The sample code to run a task can be found in file `code/benchmarks.py`, which generates `hello.gif` animation of a simple task.
Alternatives, you can explore command arguments of the C++ executable after calling `output/main --help`.

## Results

[report.pdf](Report)

## Literature
1. [A*](https://en.wikipedia.org/wiki/A*_search_algorithm)
2. [D* lite](papers/Dstar%20Lite-2002.pdf)
4. [SWSFP](papers/s10458-008-9061-x.pdf)

## Authors
1. Leo Sorvin [metametamoon](https://github.com/metametamoon)
2. Maxim Khabarov [haboh](https://github.com/haboh)