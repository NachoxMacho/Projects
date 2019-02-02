#pragma once
#include <cstdlib>
//graph.h
#define GRAPH_VERTICES 100
#define MAX_WEIGHT 50
#define MAX_DEGREE 5

// general
#define max_staleness 100
#define max_popSize 100
#define maximum_weight GRAPH_VERTICES * MAX_WEIGHT

//crossover.h
#define P_PARENTS 2
#define P_SELECTION 4

//mutation.h
#define max_mutations 5

//main.cpp
#define P_RUNS 30

// prufer.h && dandelion.h
// Length of a prufer string
#define STRING_LENGTH GRAPH_VERTICES - 2