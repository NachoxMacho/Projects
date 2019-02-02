#pragma once
#include "graph.h" // for the Graph class
#include <cassert> // for assert
#include <algorithm> // for std::sort

// Set to true for debugging purposes
#define DANDELION_DEBUG false

// Length of a prufer string
#define STRING_LENGTH GRAPH_VERTICES - 2

struct d_chromosome
{
	int score;
	int dstring[STRING_LENGTH];

	d_chromosome() { score = 0; }

	void copy(d_chromosome * d)
	{
		std::copy(d->dstring, d->dstring + STRING_LENGTH, dstring);
		/*for (int i = 0; i < STRING_LENGTH; i++) { this->dstring[i] = d->dstring[i]; }*/
		score = d->score;
	}

	// ===== print =====
	// output the string
	void print(ostream & out_s) { for (int i = 0; i < STRING_LENGTH; i++) { out_s << dstring[i] << " "; } out_s << endl; }

	// ===== is_valid =====
	// a string is valid if each vertex has a degree <= MAX_DEGREE
	bool is_valid()
	{
		int counts[GRAPH_VERTICES] = { 0 };
		for (int i = 0; i < STRING_LENGTH; i++) { if (++counts[dstring[i]] + 1 > MAX_DEGREE)return false; }
		return true;
	}

};

struct cycle
{
	int largest;
	int other;

	bool operator> (cycle c)
	{
		return c.largest > largest;
	}

	bool operator< (cycle c)
	{
		return c.largest < largest;
	}
};

class dandelion
{
public:

	Graph * base;
	int generations;
	int staleness;
	int bestFitness;
	d_chromosome * population[max_popSize];
	d_chromosome * new_generation[max_popSize];

	dandelion(Graph * original)
	{
		generations = 0;
		staleness = 0;
		base = new Graph();

		if (original != NULL)
			base->copy(original);

		assert(base->isGraphConnected());

		for (int i = 0; i < max_popSize; i++)
		{
			population[i] = new d_chromosome;

			randomize(population[i]);
			edge_mutate(population[i]);
			population[i]->score = score_fitness(population[i]);
			if (DANDELION_DEBUG) cout << "I: " << i << " | Score: " << population[i]->score << endl;
		}

		bestFitness = population[0]->score;
	}

	/*~dandelion()
	{
		for (int i = 0; i < max_popSize; i++)
		{
			cout << "Deleting " << i << endl;
			if (population[i] != NULL)
			{
				assert(population[i]->score > GRAPH_VERTICES && population[i]->score < maximum_weight);
				delete population[i];
			}
			cout << "Deleted Population" << endl;
		}
	}*/

	int score_fitness(d_chromosome * d)
	{
		int score = 0; // the score of the tree

		bool done[STRING_LENGTH + 1] = { false }; // a record of completed positions in the string 
		cycle cycles[STRING_LENGTH]; // store all the cycles 
		int cycle_count = 0; // the number of cycles that have been added to cycles array
		int degree[GRAPH_VERTICES] = { 0 };
		for (int i = 0; i < STRING_LENGTH; i++) { degree[d->dstring[i]]++; }

		// for each chain that isn't a cycle
		// EX: 2->3->1->3 would complete 2 and add the edge 2, 3

		for (int i = 1; i < GRAPH_VERTICES - 1; i++)
		{
			// for each leaf node
			if (degree[i] == 0 && !done[i])
			{
				int current_vertex = i;
				int next_vertex = d->dstring[current_vertex - 1];

				// mark this node as done
				done[current_vertex] = true;

				// decrease the next node's degrees
				degree[next_vertex]--;

				score += base->find_weight(current_vertex, next_vertex);
				while (degree[next_vertex] == 0 && next_vertex != GRAPH_VERTICES - 1 && next_vertex != 0 && !done[next_vertex])
				{
					// go to next vertex
					current_vertex = next_vertex;
					next_vertex = d->dstring[current_vertex - 1];

					// add the edge to the score
					score += base->find_weight(current_vertex, next_vertex);

					// mark this node as done
					done[current_vertex] = true;

					// decrease the next node's degrees
					degree[next_vertex]--;
				}
			}
		}

		for (int i = 1; i < STRING_LENGTH + 1; i++)
		{
			// if this position has already been added to the tree
			if (done[i]) { continue; }

			// all the nodes that aren't done at this point are part of a cycle

			int current_vertex = i; // the current vertex id
			int next_vertex = d->dstring[current_vertex - 1]; // the next position in the cycle | equivalent to s[current_vertex - 1]

			// if the vertex is in s[vertex - 1] position, it is a single node cycle, add it to the cycles and go to next spot
			if (i == next_vertex)
			{
				cycles[cycle_count].largest = i, cycles[cycle_count++].other = i, done[i] = true;
				continue;
			}

			// store the largest vertesx in the cycle
			int largest = current_vertex;

			// follow each cycle
			while (!done[next_vertex])
			{
				// add this edge to the score
				score += base->find_weight(current_vertex, next_vertex);

				// keep track of the largest vertex in the cycle
				if (next_vertex > largest)
					largest = next_vertex;

				// mark the nodes we have visited
				done[current_vertex] = true;

				// go to next vertex
				current_vertex = next_vertex;
				next_vertex = d->dstring[current_vertex - 1];
			}

			done[current_vertex] = true;

			// add the final edge in the cycle
			score += base->find_weight(current_vertex, next_vertex);

			// add the cycle to cycles
			cycles[cycle_count].largest = largest;
			cycles[cycle_count++].other = d->dstring[largest - 1]; // increment cycle_count
			score -= base->find_weight(largest, d->dstring[largest - 1]); // remove the edge between the largest and the next in the cycle
		}

		// if cycles are in the graph
		if (cycle_count > 0)
		{
			// sort the cycles by their largest value
			sort(cycles, cycles + cycle_count);

			// connect the cycles together
			// connect 0 to first cycle
			score += base->find_weight(0, cycles[0].other);
			// for the middle cycles connect the large end of one to small end of next
			for (int i = 0; i < cycle_count - 1; i++) { score += base->find_weight(cycles[i].largest, cycles[i + 1].other); }
			// finally connect the last large to the largest vertex
			score += base->find_weight(cycles[cycle_count - 1].largest, GRAPH_VERTICES - 1);
		}

		return score;
	}

	Graph * decode(d_chromosome * d)
	{
		Graph * tree = new Graph;

		bool done[STRING_LENGTH + 1] = { false }; // a record of completed positions in the string 
		cycle cycles[STRING_LENGTH]; // store all the cycles 
		int cycle_count = 0; // the number of cycles that have been added to cycles array
		int degree[GRAPH_VERTICES] = { 0 };
		for (int i = 0; i < STRING_LENGTH; i++) { degree[d->dstring[i]]++; }

		// for each chain that isn't a cycle
		// EX: 2->3->1->3 would complete 2 and add the edge 2, 3

		for (int i = 1; i < GRAPH_VERTICES - 1; i++)
		{
			// for each leaf node
			if (degree[i] == 0 && !done[i])
			{
				int current_vertex = i;
				int next_vertex = d->dstring[current_vertex - 1];

				// mark this node as done
				done[current_vertex] = true;

				// decrease the next node's degrees
				degree[next_vertex]--;

				// add this edge to the tree
				tree->connect(current_vertex, next_vertex, base->find_weight(current_vertex, next_vertex));
				while (degree[next_vertex] == 0 && next_vertex != GRAPH_VERTICES - 1 && next_vertex != 0 && !done[next_vertex])
				{
					// go to next vertex
					current_vertex = next_vertex;
					next_vertex = d->dstring[current_vertex - 1];

					// add the edge to the tree
					tree->connect(current_vertex, next_vertex, base->find_weight(current_vertex, next_vertex));

					// mark this node as done
					done[current_vertex] = true;

					// decrease the next node's degrees
					degree[next_vertex]--;
				}
			}
		}

		for (int i = 1; i < STRING_LENGTH + 1; i++)
		{
			// if this position has already been added to the tree
			if (done[i]) { continue; }

			// all the nodes that aren't done are part of a cycle

			int current_vertex = i; // the current vertex id
			int next_vertex = d->dstring[current_vertex - 1]; // the next position in the cycle | equivalent to s[current_vertex - 1]

			// if the vertex is in s[vertex - 1] position, it is a single node cycle, add it to the cycles and go to next spot
			if (i == next_vertex)
			{
				cycles[cycle_count].largest = i, cycles[cycle_count++].other = i, done[i] = true;
				continue;
			}

			// store the largest value
			int largest = current_vertex;

			// follow each cycle
			while (!done[next_vertex])
			{
				// add this edge to the tree
				tree->connect(current_vertex, next_vertex, base->find_weight(current_vertex, next_vertex));

				// keep track of the largest vertex in the cycle
				if (next_vertex > largest)
					largest = next_vertex;

				// mark the nodes we have visited
				done[current_vertex] = true;

				// go to next vertex
				current_vertex = next_vertex;
				next_vertex = d->dstring[current_vertex - 1];
			}

			done[current_vertex] = true;

			// add the final edge in the cycle
			tree->connect(current_vertex, next_vertex, base->find_weight(current_vertex, next_vertex));

			// add the cycle to cycles
			cycles[cycle_count].largest = largest;
			cycles[cycle_count++].other = d->dstring[largest - 1]; // increment cycle_count
			tree->disconnect(largest, d->dstring[largest - 1]); // remove the edge between the largest and the next in the cycle
		}

		// if cycles are in the graph
		if (cycle_count > 0)
		{
			// sort the cycles by their largest value
			std::sort(cycles, cycles + cycle_count);

			// connect the cycles together
			// connect 0 to first cycle
			tree->connect(0, cycles[0].other, base->find_weight(0, cycles[0].other));
			// for the middle cycles connect the large end of one to small end of next
			for (int i = 0; i < cycle_count - 1; i++) { tree->connect(cycles[i].largest, cycles[i + 1].other, base->find_weight(cycles[i].largest, cycles[i + 1].other)); }
			// finally connect the last large to the largest vertex
			tree->connect(cycles[cycle_count - 1].largest, GRAPH_VERTICES - 1, base->find_weight(cycles[cycle_count - 1].largest, GRAPH_VERTICES - 1));
		}

		return tree;
	}

	void randomize(d_chromosome * d) { for (int i = 0; i < STRING_LENGTH; i++) { edge_mutate(d, i, false); } assert(d->is_valid()); }

	// ===== edge_mutate =====
	// first version of mutation
	// will change a number in the chromosome 'd'
	// position of change and number are random
	// 1. validate the beginning chromosome
	// 2. count the degree of all vertices
	// 3. pick a vertex that is under the maximum
	// 4. change the string 
	// 5. validate the ending chromosome
	d_chromosome * edge_mutate(d_chromosome * d)
	{
		// 1. validate the beginning chromosome
		if (!d->is_valid()) d->print(cout);
		assert(d->is_valid());

		d_chromosome * chr = new d_chromosome;
		chr->copy(d);

		// vertices is a list of vertices that are not at the maximum count
		// pot_vertices is how many vertices are in vertices
		// pos is the position in p->pstring for the change
		// counts is a list of all vertices and their degree
		int counts[GRAPH_VERTICES], vertices[GRAPH_VERTICES];
		int pot_vertices = 0, pos = rand() % STRING_LENGTH;

		// setup the counts array, each vertex has a degree at least 1
		for (int i = 0; i < GRAPH_VERTICES; i++) { counts[i] = 1; }

		// 2. count the degree of all vertices
		// the degree of a vertex is directly related to how many times it appears in the string
		// if 'k' is how many times the vertex appears in the string, then 'k' + 1 is it's degree
		for (int i = 0; i < STRING_LENGTH; i++) { counts[d->dstring[i]]++; }

		// 3. pick a vertex that is under the maximum
		// go through the counts array, and store all vertices that are available and increase pot_vertices each time
		// NOTE: the vertex given by p->pstring[pos] is not added to the list do that change always happens
		for (int i = 0; i < GRAPH_VERTICES; i++) { if (counts[i] < MAX_DEGREE && i != d->dstring[pos]) vertices[pot_vertices++] = i; }

		// make sure there is at least one potential change
		assert(pot_vertices > 0);

		// 4. change the string 
		chr->dstring[pos] = vertices[rand() % pot_vertices];

		// 5. validate the ending chromosome
		if (!chr->is_valid()) chr->print(cout);
		assert(chr->is_valid());

		return chr;
	}

	// ===== edge_mutate =====
	// second version of mutation
	// will change the number at position 'vertex' of chromosome 'd'
	// will validate the chromosome is correct before and after unless 'validate' is set to false
	void edge_mutate(d_chromosome * d, int pos, bool validate = true)
	{

		if (validate && !d->is_valid()) d->print(cout);
		assert(!validate || d->is_valid());

		int potential[GRAPH_VERTICES], vertices[GRAPH_VERTICES];
		int pot_vertices = 0;

		for (int i = 0; i < GRAPH_VERTICES; i++) { potential[i] = 1; }
		for (int i = 0; i < STRING_LENGTH; i++) { potential[d->dstring[i]]++; }
		for (int i = 0; i < GRAPH_VERTICES; i++) { if (potential[i] < MAX_DEGREE && i != d->dstring[pos]) vertices[pot_vertices++] = i; }

		assert(pot_vertices > 0);

		d->dstring[pos] = vertices[rand() % pot_vertices];

		if (validate && !d->is_valid()) d->print(cout);
		assert(!validate || d->is_valid());
	}

	// ===== edge_crossover =====
	// will attempt to insert a random value from d that is not in o
	// this will overwrite the value in o that is already in there
	// this will adhere to the constraints and only produce valid offspring
	// if no possible subsitution is found, it will call the random edge mutate function on o
	d_chromosome * edge_crossover(d_chromosome * o, d_chromosome * d)
	{
		assert(o->is_valid());
		assert(d->is_valid());
		d_chromosome * chr = new d_chromosome;
		chr->copy(o);

		int counts[GRAPH_VERTICES], potential[GRAPH_VERTICES];
		int pot_vertices = 0;
		for (int i = 0; i < GRAPH_VERTICES; i++) { counts[i] = 1; }
		for (int i = 0; i < STRING_LENGTH; i++) { counts[o->dstring[i]]++; }
		for (int i = 0; i < STRING_LENGTH; i++) { if (o->dstring[i] != d->dstring[i] && counts[d->dstring[i]] < MAX_DEGREE) potential[pot_vertices++] = i; }

		if (pot_vertices == 0) { return edge_mutate(chr); }

		assert(pot_vertices > 0);
		int pos = potential[rand() % pot_vertices];
		chr->dstring[pos] = d->dstring[pos];

		assert(o->is_valid());
		assert(d->is_valid());

		return chr;
	}

	void run_generation()
	{
		// output data
		int totalFitness = population[0]->score;
		int new_best_fitness = population[0]->score;
		int bestIndex = 0;

		// 1. find the best tree in the population
		for (int i = 1; i < max_popSize; i++)
		{
			totalFitness += population[i]->score;
			if (population[i]->score < new_best_fitness)
				bestIndex = i, new_best_fitness = population[i]->score;
		}

		// copy the best AL_chromosome over | elitism
		new_generation[0] = new d_chromosome;
		new_generation[0]->copy(population[bestIndex]);
		assert(new_generation[0]->is_valid());

		// 2. check staleness
		if (bestFitness <= new_best_fitness)
			staleness++;
		else
		{
			//update best fitness and best tree
			staleness = 0, bestFitness = new_best_fitness;
		}

		generations++;

		// 3. basic tournament selection
		for (int i = 1; i < max_popSize; i++)
		{
			int one = rand() % max_popSize;
			int two = rand() % max_popSize;

			// new_generation[i] = new d_chromosome;
			// new_generation[i]->copy((population[one]->score < population[two]->score) ? population[one] : population[two]);

			new_generation[i] = edge_crossover((population[one]->score < population[two]->score) ? population[one] : population[two], // keep the better one as the base
				(population[one]->score < population[two]->score) ? population[two] : population[one]); // the other one will be drawn from

			// store and score the tree
			new_generation[i]->score = score_fitness(new_generation[i]);
		}

		for (int i = 0; i < max_popSize; i++)
		{
			// make sure the incoming tree is valid
			if (!new_generation[i]->is_valid()) new_generation[i]->print(cout);
			assert(new_generation[i]->is_valid());

			delete population[i];

			population[i] = new_generation[i];

			// copy over new tree & score | clearing the tree is handled in the copy function
			/*population[i]->copy(new_generation[i]);*/

			new_generation[i] = NULL;

			assert(population[i]->is_valid());
		}
	}

};