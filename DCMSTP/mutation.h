#pragma once
#include "graph.h"

// set to true to output info to console
#define MUTATE_DEBUG false

// Different operators
#define AL_MUTATION 0
#define AL_CROSSOVER 1
#define AL_OP_TYPES 2


// a potential solution
struct AL_chromosome
{
	int score;
	int op;
	Graph * tree;

	// constructor and destructor for AL_chromosome
	AL_chromosome() { score = 0; tree = NULL; }
	~AL_chromosome() { if (tree != NULL) delete tree; }
};

class adList
{
public:
	void run_generation();

	adList(Graph * original, int populationSize = max_popSize); // constructor
	~adList(); // destructor

	int generations;
	int staleness;
	int bestFitness;
	int mutations;
	int popSize;
	int op;
	int x_probability;
	bool mix;
	Graph * base;

	void edge_mutate(Graph * parent);
	void tree_crossover(Graph * parent1, Graph * parent2, Graph * offspring);
	void randomize_tree(Graph * tree);
	void connect_partitions(Graph * tree, int rootID);

	AL_chromosome * population[max_popSize];
	AL_chromosome * new_generation[max_popSize];
};

//=============== CONSTRUCTOR ===============
// 1. set basic statistics
// 2. copy original -> base
// 3. initalize blank chromosomes
adList::adList(Graph * original, int populationSize)
{
	// 1. set statistics
	generations = 0;
	staleness = 0;
	popSize = populationSize;
	mutations = 1;
	x_probability = 50;
	op = AL_MUTATION;
	mix = false;

	assert(popSize <= max_popSize && popSize > 0);

	// 2. copy the graph in
	base = new Graph(false);
	base->copy(original);
	assert(base->isGraphConnected());

	// 3. initalize chromosomes
	for (int i = 0; i < max_popSize; i++)
	{
		if (i < popSize)
		{
			// make a blank start & place holder
			if (MUTATE_DEBUG) cout << "Making " << i << " of " << popSize << endl;

			population[i] = new AL_chromosome;
			new_generation[i] = new AL_chromosome;
			population[i]->tree = new Graph(false); // load a blank graph
			randomize_tree(population[i]->tree); // make a random tree
			population[i]->score = population[i]->tree->fitness(); // score and store the tree
			new_generation[i]->tree = new Graph(false); // make a blank graph for the place holder
		}
		else
			population[i] = new_generation[i] = NULL;
	}

	// 1. reset bestFitness
	bestFitness = population[0]->score;
}

//=============== DESTRUCTOR ===============
// 1. delete the original graph
// 2. delete all chromosomes & place holders
adList::~adList()
{
	// 1. delete the base graph
	delete base;

	// 2. delete all chromomsome & place holders if they exist
	for (int i = 0; i < max_popSize; i++)
	{
		if (population[i] != NULL)
			delete population[i];
		if (new_generation[i])
			delete new_generation[i];
	}
}

//=============== randomize_tree ===============
// 1. pick a random root
// 2. while the tree isn't finished, connect a random vertex to the tree
// 3. verify the tree is within constraint and minimal
void adList::randomize_tree(Graph * tree)
{
	// 1. pick a random root
	int root = rand() % GRAPH_VERTICES;

	// 2. while the tree isn't finished, connect a random vertex to the tree
	while (!tree->isGraphConnected())
		connect_partitions(tree, root);

	// 3. verify the tree is within constraint and minimal
	if (MUTATE_DEBUG) cout << "Finished randomizing tree." << endl;
	assert(tree->isGraphConnected());
	assert(tree->isMinimalTree());
	assert(tree->isInConstraint());
}

//=============== edge_mutate ===============
// 1. pick an edge to disconnect
// 2. disconnect that edge
// 3. reconnect the tree (using any edge that connects the tree)
void adList::edge_mutate(Graph * parent)
{
	// make sure the original graph is connected and minimal
	assert(parent->isMinimalTree());
	assert(parent->isInConstraint());

	// 1. pick an edge to disconnect
	vertex * in = &parent->vertices[rand() % GRAPH_VERTICES];
	vertex * out = in->connected_vertices[rand() % in->connected_vertices_count];

	// 2. disconnect that edge
	if (MUTATE_DEBUG) cout << "Disconnecting the edge between " << in->id << " and " << in->id << endl;
	parent->disconnect(in, out);
	assert(!parent->isGraphConnected());

	// 3. reconnect the tree (using any edge that connects the tree)
	connect_partitions(parent, in->id);
}

//=============== run_generation ===============
// 1. find the best tree in the population for elitism, store in new_generation
// 2. check if staleness increased
// 3. tourament select a parent
// 4. mutate parent and add to new_generation
// 5. repeat to step 3 till new_generation is full
// 6. copy new_generation population over original population
void adList::run_generation()
{
	// output data
	int totalFitness = population[0]->score;
	int new_best_fitness = population[0]->score;
	int bestIndex = 0;

	assert(popSize <= max_popSize && popSize > 0);

	// 1. find the best tree in the population
	for (int i = 1; i < popSize; i++)
	{
		totalFitness += population[i]->score;
		if (population[i]->score < new_best_fitness)
			bestIndex = i, new_best_fitness = population[i]->score;
	}

	// copy the best AL_chromosome over | elitism
	new_generation[0]->tree->copy(population[bestIndex]->tree);
	assert(new_generation[0]->tree->isMinimalTree());

	// 2. check staleness
	if (bestFitness <= new_best_fitness)
		staleness++;
	else
	{
		//update best fitness and best tree
		staleness = 0, bestFitness = new_best_fitness;
		if (new_generation[0]->op == AL_MUTATION && mix && x_probability > 10)
			x_probability--;
		else if (new_generation[0]->op == AL_CROSSOVER && mix && x_probability < 90)
			x_probability++;

	}

	generations++;

	// 3. basic tournament selection
	for (int i = 1; i < popSize; i++)
	{
		int one = rand() % popSize;
		int two = rand() % popSize;
		int three = rand() % popSize;

		if (mix)
			op = (rand() % 100 <= x_probability) ? AL_CROSSOVER : AL_MUTATION;

		// perform the selected operator on the winner
		switch (op)
		{
		case AL_MUTATION:
			if (population[one]->score <= population[two]->score && population[three]->score <= population[two]->score)
				new_generation[i]->tree->copy(population[one]->tree);
			else if (population[three]->score <= population[one]->score)
				new_generation[i]->tree->copy(population[two]->tree);
			else
				new_generation[i]->tree->copy(population[three]->tree);

			for (int j = 0; j < mutations; j++)
				edge_mutate(new_generation[i]->tree);
			break;
		case AL_CROSSOVER:
			if (population[three]->score <= population[two]->score && population[three]->score <= population[one]->score)
				tree_crossover(population[one]->tree, population[two]->tree, new_generation[i]->tree);
			else if (population[two]->score <= population[one]->score && population[two]->score <= population[three]->score)
				tree_crossover(population[one]->tree, population[three]->tree, new_generation[i]->tree);
			else
				tree_crossover(population[three]->tree, population[two]->tree, new_generation[i]->tree);
			break;
		}
		

		// make sure the mutated tree is connected
		// store and score the tree
		assert(new_generation[i]->tree->isGraphConnected());
		assert(new_generation[i]->tree->isMinimalTree());
		assert(new_generation[i]->tree->isInConstraint());
		new_generation[i]->score = new_generation[i]->tree->fitness();
		new_generation[i]->op = op;
	}

	for (int i = 0; i < popSize; i++)
	{
		// make sure the incoming tree is valid
		assert(new_generation[i]->tree->isMinimalTree());

		// copy over new tree & score | clearing the tree is handled in the copy function
		population[i]->tree->copy(new_generation[i]->tree);
		population[i]->score = population[i]->tree->fitness();

		assert(population[i]->tree->isMinimalTree());
	}
}

//=============== connect_partition ===============
// 1. identify all vertices in each partition
// 2. search for all potential edges that would connect the tree (will include the disconnected edge)
// 3. pick a potential vertex
// 4. find all connected vertices that aren't in the same partition
// 5. pick a random connectable vertex
// 6. find the edge weight
// 7. connect the vertex
void adList::connect_partitions(Graph * tree, int rootID)
{
	// array of vertices, will seperate which vertices are in each partition of the tree
	bool in_tree[GRAPH_VERTICES];
	for (int i = 0; i < GRAPH_VERTICES; i++)
		in_tree[i] = false;

	// 1. identify all vertices in each partition
	in_tree[rootID] = true;
	tree->vertices[rootID].fillGraph(in_tree);

	vertex *in = NULL, *out = NULL;
	int potential = 0;
	int potential_vertex[GRAPH_VERTICES];
	int potential_vertices[GRAPH_VERTICES];

	// 2. search for all potential edges that would connect the tree
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		int connections = 0;
		in = &tree->vertices[i];

		// only conisder vertices in one partition
		if (!in_tree[in->id])
			continue;

		// can't add an edge if the vertex is full
		if (in->connected_vertices_count == MAX_DEGREE)
			continue;

		// count the number of vertices it is connected to that are in a different partition
		for (int j = 0; j < base->vertices[in->id].connected_vertices_count; j++)
			if (!in_tree[base->vertices[in->id].connected_vertices[j]->id] && tree->vertices[base->vertices[in->id].connected_vertices[j]->id].connected_vertices_count < MAX_DEGREE)
				connections++;

		// if all criteria met
		if (connections > 0)
			potential_vertex[potential++] = in->id;
	}

	// 3. pick a potential vertex
	assert(potential > 0);
	in = &tree->vertices[potential_vertex[rand() % potential]];

	// 4. find all connected vertices that aren't in the same partition
	potential = 0;
	for (int i = 0; i < base->vertices[in->id].connected_vertices_count; i++)
		if (!in_tree[base->vertices[in->id].connected_vertices[i]->id] && tree->vertices[base->vertices[in->id].connected_vertices[i]->id].connected_vertices_count < MAX_DEGREE)
			potential_vertices[potential++] = base->vertices[in->id].connected_vertices[i]->id;

	if (MUTATE_DEBUG) cout << "IN vertex " << in->id << " in tree with potential " << potential << ", and " << in->connected_vertices_count << " connections." << endl;

	// 5. pick a random connectable vertex
	out = &tree->vertices[potential_vertices[rand() % potential]];

	// 6. find the edge weight
	int edge_weight = base->find_weight(in->id, out->id);

	if (MUTATE_DEBUG) cout << "OUT vertex " << out->id << " outside tree with edge weight " << edge_weight << ", and " << out->connected_vertices_count << " connections." << endl;
	if (MUTATE_DEBUG) cout << "Connecting the edge between " << in->id << " and " << out->id << " with weight: " << edge_weight << endl;

	// 7. connect the vertex
	tree->connect(&tree->vertices[in->id], &tree->vertices[out->id], edge_weight);
	assert(tree->find_weight(in->id, out->id, true) == base->find_weight(in->id, out->id));
}

void adList::tree_crossover(Graph * parent1, Graph * parent2, Graph * offspring)
{
	// clear the offspring graph
	offspring->clear();
	// int root = 0;
	int similarity = 0;

	// connect all edges that are the same in both parents
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		for (int j = 0; j < parent1->vertices[i].connected_vertices_count; j++)
		{
			if (parent2->find_weight(parent1->vertices[i].id, parent1->vertices[i].connected_vertices[j]->id) != -1)
				offspring->connect(&offspring->vertices[parent1->vertices[i].id], &offspring->vertices[parent1->vertices[i].connected_vertices[j]->id],
					parent2->find_weight(parent1->vertices[i].id, parent1->vertices[i].connected_vertices[j]->id)), similarity++;
		}
	}
	
	// randomize the rest
	// int root = rand() % GRAPH_VERTICES;
	randomize_tree(offspring);

	assert(offspring->isGraphConnected());
	assert(offspring->isMinimalTree());
	assert(offspring->isInConstraint());
	// cout << "Similarity: " << similarity / 2 << endl;
}
