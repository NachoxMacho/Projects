#pragma once
#include "graph.h"

// set to true to output info to console
#define DEBUG false

// set population size here
#define populationSize GRAPH_VERTICES

// max number of mutations
#define max_mutations 5
#define max_staleness 20

// a potential solution
struct chromosome
{
	int score;
	Graph * tree;

	// constructor and destructor for chromosome
	chromosome() { score = 0; tree = NULL; }
	~chromosome() { if (tree != NULL) delete tree; }
};

class mutationGA
{
public:
	void runGeneration(ostream &out_s);

	mutationGA(Graph original); // constructor
	~mutationGA()
	{
		// delete the base graph
		delete base;
		
		// delete all chromomsome & place holders
		for (int i = 0; i < populationSize; i++)
		{
			delete population[i];
			delete new_generation[i];
		}
	}

	int generations;
	int staleness;
	int bestFitness;
	int mutations;
	Graph * base;

	void edge_mutate(Graph * parent);
	void randomizeTree(Graph * tree);

	chromosome * population[populationSize];
	chromosome * new_generation[populationSize];
};

mutationGA::mutationGA(Graph original)
{
	// set statistics
	generations = 0;
	staleness = 0;
	mutations = 1;

	// copy the graph in
	base = new Graph(false);
	base->copy(&original);
	assert(base->isGraphConnected());

	// for each chromosome
	for (int i = 0; i < populationSize; i++)
	{
		// make a blank start
		if (DEBUG) cout << "Making " << i << " of " << populationSize << endl;
		population[i] = new chromosome;
		population[i]->tree = new Graph(false); // load a blank graph
		randomizeTree(population[i]->tree); // make a random tree
		population[i]->score = population[i]->tree->fitness(); // score and store the tree
	}

	for (int i = 0; i < populationSize; i++)
	{
		// make new_generation place holders
		new_generation[i] = new chromosome;
		new_generation[i]->tree = new Graph(false);
	}

	//reset bestFitness
	bestFitness = population[0]->score;
}

void mutationGA::randomizeTree(Graph * tree)
{
	bool vertex_in_tree[GRAPH_VERTICES]; // array of vertices present in the tree
	for (int i = 0; i < GRAPH_VERTICES; i++)
		vertex_in_tree[i] = false;

	vertex_in_tree[rand() % GRAPH_VERTICES] = true; // randomize root

	// while the graph is not finished
	while (!tree->isGraphConnected())
	{
		vertex * in, *out;
		bool good = false;
		int potential;
		int potential_vertices[GRAPH_VERTICES];

		// find a vertex in the graph
		while (!good)
		{
			good = true;

			// select a vertex
			in = &tree->vertices[rand() % GRAPH_VERTICES];

			// if the vertex is not in the tree, its no good
			if (!vertex_in_tree[in->id])
				good = false;

			// if the vertex is full, its no good
			if (in->connected_vertices_count == MAX_DEGREE)
				good = false;

			// number of adjacent vertices that aren't in the graph already
			potential = 0;
			for (int i = 0; i < base->vertices[in->id].connected_vertices_count; i++)
				if (!vertex_in_tree[base->vertices[in->id].connected_vertices[i]->id] && tree->vertices[in->id].connected_vertices_count < MAX_DEGREE)
					potential_vertices[potential++] = base->vertices[in->id].connected_vertices[i]->id;

			// If no options are available, pick a different vertex
			if (potential == 0)
				good = false;
		}
		
		if (DEBUG) cout << "IN vertex " << in->id << " in tree with potential " << potential << ", and " << in->connected_vertices_count << " connections." << endl;

		// pick a random connection from base graph
		out = &tree->vertices[potential_vertices[rand() % potential]];

		// find the edge weight
		int edge_weight;
		for (int i = 0; i < base->vertices[in->id].connected_vertices_count; i++)
			if (base->vertices[in->id].connected_vertices[i]->id == out->id)
				edge_weight = base->vertices[in->id].connected_vertices_weights[i];

		if (DEBUG) cout << "OUT vertex " << out->id << " outside tree with edge weight " << edge_weight << ", and " << out->connected_vertices_count << " connections." << endl;
		if (DEBUG) cout << "Connecting the edge between " << in->id << " and " << out->id << " with weight: " << edge_weight << endl;

		// add out vertex to tree
		tree->connect(&tree->vertices[in->id], &tree->vertices[out->id], edge_weight);
		vertex_in_tree[out->id] = true;

		in = out = NULL; // nullify pointers
	}
	
	// make sure the tree is connected and a tree
	if (DEBUG) cout << "Finished making tree." << endl;
	assert(tree->isMinimalTree());
	assert(tree->isInConstraint());
}

void mutationGA::edge_mutate(Graph * parent)
{
	// make sure the original graph is connected and no un-needed edges
	assert(parent->isMinimalTree());
	assert(parent->isInConstraint());

	// pick an edge to disconnect
	vertex * change = &parent->vertices[rand() % GRAPH_VERTICES];
	vertex * disconnect = change->connected_vertices[rand() % change->connected_vertices_count];

	// find the weight of the edge in case we need to reconnect
	int weight;
	for (int i = 0; i < change->connected_vertices_count; i++)
		if (change->connected_vertices[i]->id == disconnect->id)
			weight = change->connected_vertices_weights[i];

	if (DEBUG) cout << "Disconnecting the edge between " << change->id << " and " << disconnect->id << endl;

	// 3. disconnect that edge
	parent->disconnect(change, disconnect);

	// the graph is disconnected into 2 partitions here
	assert(!parent->isGraphConnected());

	bool in_tree[GRAPH_VERTICES];
	for (int i = 0; i < GRAPH_VERTICES; i++)
		in_tree[i] = false;

	in_tree[change->id] = true;

	// identify all vertices in one partition
	// true if connected to change vertex, false if connected to disconnect vertex
	change->fillGraph(in_tree);

	vertex *in = NULL, *out = NULL;
	bool good = false;
	int potential = 0;
	int potential_vertex[GRAPH_VERTICES];
	int potential_vertices[GRAPH_VERTICES];

	// search for all potential other connections
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		bool good = true;

		int connections = 0;
		in = &parent->vertices[i];

		if (!in_tree[in->id])
			good = false;

		if (in->connected_vertices_count == MAX_DEGREE)
			good = false;

		for (int j = 0; j < base->vertices[in->id].connected_vertices_count; j++)
			if (!in_tree[base->vertices[in->id].connected_vertices[j]->id] && parent->vertices[base->vertices[in->id].connected_vertices[j]->id].connected_vertices_count < MAX_DEGREE)
				connections++;

		if (good && connections > 0)
			potential_vertex[potential++] = in->id;
	}

	// make sure a mutation is possible, and pick a possible vertex
	assert(potential > 0);
	in = &parent->vertices[potential_vertex[rand() % potential]];

	// find all connected vertices that aren't in the graph
	potential = 0;
	for (int i = 0; i < base->vertices[in->id].connected_vertices_count; i++)
		if (!in_tree[base->vertices[in->id].connected_vertices[i]->id] && parent->vertices[base->vertices[in->id].connected_vertices[i]->id].connected_vertices_count < MAX_DEGREE)
			potential_vertices[potential++] = base->vertices[in->id].connected_vertices[i]->id;

	if (DEBUG) cout << "IN vertex " << in->id << " in tree with potential " << potential << ", and " << in->connected_vertices_count << " connections." << endl;

	// pick a random connectable vertex
	out = &parent->vertices[potential_vertices[rand() % potential]];

	// find the edge weight
	int edge_weight;
	for (int i = 0; i < base->vertices[in->id].connected_vertices_count; i++)
		if (base->vertices[in->id].connected_vertices[i]->id == out->id)
			edge_weight = base->vertices[in->id].connected_vertices_weights[i];

	if (DEBUG) cout << "OUT vertex " << out->id << " outside tree with edge weight " << edge_weight << ", and " << out->connected_vertices_count << " connections." << endl;
	if (DEBUG) cout << "Connecting the edge between " << in->id << " and " << out->id << " with weight: " << edge_weight << endl;

	// add the vertex and score the tree
	parent->connect(&parent->vertices[in->id], &parent->vertices[out->id], edge_weight);

	// nullify extra pointers
	in = out = NULL;

	// final check that the graph is connected and a tree
	assert(parent->isMinimalTree());
	assert(parent->isInConstraint());
}

void mutationGA::runGeneration(ostream &out_s)
{
	// output data
	int totalFitness = population[0]->score;
	int new_best_fitness = population[0]->score;
	int bestIndex = 0;

	// find the best tree in the population
	for (int i = 1; i < populationSize; i++)
	{
		totalFitness += population[i]->score;
		if (population[i]->score < new_best_fitness)
		{
			bestIndex = i;
			new_best_fitness = population[i]->score;
		}
	}

	if (bestFitness <= new_best_fitness)
		staleness++;
	else
	{
		//update best fitness and best tree
		staleness = 0;
		bestFitness = new_best_fitness;
	}

	out_s << generations << ", " << totalFitness << ", " << bestFitness << ", " << totalFitness / populationSize << ", " << staleness << endl;
	generations++;

	// basic tournament selection
	for (int i = 1; i < populationSize; i++)
	{
		int one = rand() % populationSize;
		int two = rand() % populationSize;

		// copy the winner's tree
		if (population[one]->score <= population[two]->score)
			new_generation[i]->tree->copy(population[one]->tree);
		else
			new_generation[i]->tree->copy(population[two]->tree);

		// mutate the winning tree mutations times
		for (int j = 0; j < mutations; j++)
			edge_mutate(new_generation[i]->tree);

		// make sure the mutated tree is connected
		// store and score the tree
		assert(new_generation[i]->tree->isMinimalTree());
		new_generation[i]->score = new_generation[i]->tree->fitness();
	}

	// copy the best chromosome over | elitism
	new_generation[0]->tree->copy(population[bestIndex]->tree);
	assert(new_generation[0]->tree->isMinimalTree());

	for (int i = 0; i < populationSize; i++)
	{
		// make sure the incoming tree is valid
		assert(new_generation[i]->tree->isMinimalTree());

		// copy over new tree & score | clearing the tree is handled in the copy function
		population[i]->tree->copy(new_generation[i]->tree);
		population[i]->score = population[i]->tree->fitness();

		assert(population[i]->tree->isMinimalTree());
	}
}
