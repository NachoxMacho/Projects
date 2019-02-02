#pragma once
#include "graph.h"
#include <cassert>

// Set to true for debugging purposes
#define PRUFER_DEBUG false

struct p_chromosome
{
	int score;
	int pstring[STRING_LENGTH];

	// ===== CONSTRUCTOR =====
	// Create the chromosome
	// if no argument is passed or false, a blank chromosome is generated with -1 in every slot
	// if true is passed, pstring will be randomly filled with integers | NOTE: this will not score the chromosome
	p_chromosome() { score = 0; }

	// ===== copy =====
	void copy(p_chromosome * p)
	{
		std::copy(p->pstring, p->pstring + STRING_LENGTH, pstring);
		this->score = p->score;
	}

	// ===== print =====
	// output the string
	void print(ostream & out_s) { for (int i = 0; i < STRING_LENGTH; i++) { out_s << pstring[i] << " "; } out_s << endl; }

	// ===== degree =====
	// returns the degree of the vertex passed
	// in a prufer string the degree is equal to k + 1 where k is the number of times the vertex appears in the string
	int degree(int vertex)
	{
		int count = 1;
		for (int i = 0; i < STRING_LENGTH; i++) if (pstring[i] == vertex) count++;
		return count;
	}

	// ===== is_valid =====
	// a string is valid if each vertex has a degree <= MAX_DEGREE
	bool is_valid() 
	{ 
		int counts[GRAPH_VERTICES] = { 0 };
		for (int i = 0; i < STRING_LENGTH; i++) { if (++counts[pstring[i]] + 1 > MAX_DEGREE)return false; }
		return true; 
	}

	bool equal(p_chromosome * p) { for (int i = 0; i < STRING_LENGTH; i++) { if (pstring[i] != p->pstring[i]) return false; } return true; }
};

class prufer
{
public:
	Graph * base;
	int generations;
	int staleness;
	int bestFitness;
	int popSize;
	p_chromosome * population[max_popSize];
	p_chromosome * new_generation[max_popSize];

	prufer(Graph * original)
	{
		generations = 0;
		staleness = 0;
		popSize = max_popSize;
		base = new Graph();

		if (original != NULL)
			base->copy(original);

		assert(base->isGraphConnected());

		for (int i = 0; i < max_popSize; i++)
		{
			population[i] = new p_chromosome;
			randomize(population[i]);
			edge_mutate(population[i]);
			population[i]->score = score_fitness(population[i]);
			if (PRUFER_DEBUG) cout << "I: " << i << " | Score: " << population[i]->score << endl;

			new_generation[i] = new p_chromosome;
		}

		bestFitness = population[0]->score;
	}

	int score_fitness(p_chromosome * p)
	{
		int score = 0;
		int counts[GRAPH_VERTICES];
		for (int i = 0; i < GRAPH_VERTICES; i++) { counts[i] = 1; }
		for (int i = 0; i < STRING_LENGTH; i++) { counts[p->pstring[i]]++; }

		int min = 0;
		for (int i = 0; i < GRAPH_VERTICES; i++) { if (counts[i] == 1) { min = i; break; } }

		for (int i = 0; i < STRING_LENGTH; i++)
		{
			int pos = p->pstring[i];
			

			if (min > pos)
			{
				assert(base->vertices[pos].connected_vertices[min - 1]->id == min);
				score += base->vertices[pos].connected_vertices_weights[min - 1];
			}
			else
			{
				assert(base->vertices[pos].connected_vertices[min]->id == min);
				score += base->vertices[pos].connected_vertices_weights[min];
			}
			
			counts[pos]--;
			if (min > pos && counts[pos] == 1)
				min = pos;
			else
				for (int j = 0; j < GRAPH_VERTICES; j++) { if (counts[j] == 1) { min = j; break; } }

			/*for (int j = 0; j < GRAPH_VERTICES; j++)
			{
				if (counts[j] == 1)
				{
					if (j > p->pstring[i])
					{
						if (base->vertices[p->pstring[i]].connected_vertices[j - 1]->id != j)
						{
							p->print(cout);
							cout << "I: " << i << " < " << j << " :J" << endl;
						}
						assert(base->vertices[p->pstring[i]].connected_vertices[j - 1]->id == j);
						score += base->vertices[p->pstring[i]].connected_vertices_weights[j - 1];
					}
					else
					{
						assert(base->vertices[p->pstring[i]].connected_vertices[j]->id == j);
						score += base->vertices[p->pstring[i]].connected_vertices_weights[j];
					}
					counts[j]++, counts[p->pstring[i]]--;
					break;
				}
			}*/
		}
		int a;

		for (int i = 0; i < GRAPH_VERTICES; i++) { if (counts[i] == 1) { a = i; break; } }
		for (int i = GRAPH_VERTICES - 1; i > -1; i--) { if (counts[i] == 1) { score += base->find_weight(a, i); break; } }

		return score;
	}

	Graph * decode(p_chromosome * p)
	{
		assert(p->is_valid());
		Graph * tree = new Graph();
		int counts[GRAPH_VERTICES];
		for (int i = 0; i < GRAPH_VERTICES; i++) { counts[i] = 1; }
		for (int i = 0; i < STRING_LENGTH; i++) { counts[p->pstring[i]]++; }

		for (int i = 0; i < STRING_LENGTH; i++)
		{
			for (int j = 0; j < GRAPH_VERTICES; j++)
			{
				if (counts[j] == 1)
				{
					if (j > p->pstring[i])
					{
						assert(base->vertices[p->pstring[i]].connected_vertices[j - 1]->id == j);
						tree->connect(&tree->vertices[p->pstring[i]], &tree->vertices[j], base->vertices[p->pstring[i]].connected_vertices_weights[j - 1]);
					}
					else
					{
						assert(base->vertices[p->pstring[i]].connected_vertices[j]->id == j);
						tree->connect(&tree->vertices[p->pstring[i]], &tree->vertices[j], base->vertices[p->pstring[i]].connected_vertices_weights[j]);
					}
					counts[j]++, counts[p->pstring[i]]--;
					break;
				}
			}
		}
		int a;

		for (int i = 0; i < GRAPH_VERTICES; i++) { if (counts[i] == 1) { a = i; break; } }
		for (int i = GRAPH_VERTICES - 1; i > -1; i--) 
		{ 
			if (counts[i] == 1) 
			{ 
				assert(base->vertices[a].connected_vertices[i - 1]->id == i);
				tree->connect(&tree->vertices[a], &tree->vertices[i], base->vertices[a].connected_vertices_weights[i - 1]);
				break; 
			} 
		}

		assert(tree->isInConstraint());

		return tree;

		//int connection, score = 0, rounds = 0;
		//int temp[STRING_LENGTH];
		//Graph * tree = new Graph();

		//for (int i = 0; i < STRING_LENGTH; i++) { temp[i] = p->pstring[i]; }

		//while (rounds < STRING_LENGTH)
		//{
		//	// get the minimum value not existing in the string
		//	connection = p->find_edge(temp);

		//	// get the edge between that and the first element
		//	tree->connect(&tree->vertices[temp[0]], &tree->vertices[connection], base->find_weight(temp[0], connection));

		//	// shift all elements in the array by -1
		//	for (int i = 0; i < STRING_LENGTH - 1; i++) { temp[i] = temp[i + 1]; }

		//	// add connection to end of string
		//	temp[STRING_LENGTH - 1] = connection;
		//	rounds++;
		//}

		//// figure out the last connection
		//connection = p->find_edge(temp);

		//tree->connect(&tree->vertices[temp[p->find_edge(temp, true)]], &tree->vertices[connection], base->find_weight(temp[0], connection));
		//return tree;
	}

	void run_generation()
	{
		// output data
		int totalFitness = population[0]->score;
		int new_best_fitness = population[0]->score;
		int bestIndex = 0;

		assert(popSize <= max_popSize && popSize > 0);

		// 1. find the best tree in the population
		for (int i = 1; i < max_popSize; i++)
		{
			totalFitness += population[i]->score;
			if (population[i]->score < new_best_fitness)
				bestIndex = i, new_best_fitness = population[i]->score;
		}

		// copy the best AL_chromosome over | elitism
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

			//while (two == one || population[two]->equal(population[one]))
			//{
			//	if (two != one)
			//		cout << one << " and " << two << " are the same string." << endl;
			//	two = rand() % max_popSize;
			//}

			new_generation[i]->copy( (population[one]->score < population[two]->score) ? population[one] : population[two] );

			if (!new_generation[i]->is_valid()) new_generation[i]->print(cout);
			assert(new_generation[i]->is_valid());

			edge_crossover(new_generation[i], (population[one]->score < population[two]->score) ? population[two] : population[one] );

			// make sure the mutated tree is connected
			if (!new_generation[i]->is_valid()) new_generation[i]->print(cout);
			assert(new_generation[i]->is_valid());

			// store and score the tree
			new_generation[i]->score = score_fitness(new_generation[i]);
		}

		for (int i = 0; i < max_popSize; i++)
		{
			// make sure the incoming tree is valid
			if (!new_generation[i]->is_valid()) new_generation[i]->print(cout);
			assert(new_generation[i]->is_valid());

			// copy over new tree & score | clearing the tree is handled in the copy function
			population[i]->copy(new_generation[i]);

			assert(population[i]->is_valid());
		}
	}

	// ===== edge_mutate =====
	// first version of mutation
	// will change a number in the chromosome 'p'
	// position of change and number are random
	// 1. validate the beginning chromosome
	// 2. count the degree of all vertices
	// 3. pick a vertex that is under the maximum
	// 4. change the string 
	// 5. validate the ending chromosome
	void edge_mutate(p_chromosome * p)
	{
		// 1. validate the beginning chromosome
		if (!p->is_valid()) p->print(cout);
		assert(p->is_valid());

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
		for (int i = 0; i < STRING_LENGTH; i++) { counts[p->pstring[i]]++; }

		// 3. pick a vertex that is under the maximum
		// go through the counts array, and store all vertices that are available and increase pot_vertices each time
		// NOTE: the vertex given by p->pstring[pos] is not added to the list do that change always happens
		for (int i = 0; i < GRAPH_VERTICES; i++) { if (counts[i] < MAX_DEGREE && i != p->pstring[pos]) vertices[pot_vertices++] = i; }

		// make sure there is at least one potential change
		assert(pot_vertices > 0);

		// 4. change the string 
		p->pstring[pos] = vertices[rand() % pot_vertices];

		// 5. validate the ending chromosome
		if (!p->is_valid()) p->print(cout);
		assert(p->is_valid());
	}

	// ===== edge_mutate =====
	// second version of mutation
	// will change the number at position 'vertex' of chromosome 'p'
	// will validate the chromosome is correct before and after unless 'validate' is set to false
	void edge_mutate(p_chromosome * p, int pos, bool validate = true)
	{
		if (validate && !p->is_valid()) p->print(cout);
		assert(!validate || p->is_valid());

		int potential[GRAPH_VERTICES], vertices[GRAPH_VERTICES];
		int pot_vertices = 0;

		for (int i = 0; i < GRAPH_VERTICES; i++) { potential[i] = 1; }
		for (int i = 0; i < STRING_LENGTH; i++) { potential[p->pstring[i]]++; }
		for (int i = 0; i < GRAPH_VERTICES; i++) { if (potential[i] < MAX_DEGREE && i != p->pstring[pos]) vertices[pot_vertices++] = i; }

		assert(pot_vertices > 0);

		p->pstring[pos] = vertices[rand() % pot_vertices];

		if (validate && !p->is_valid()) p->print(cout);
		assert(!validate || p->is_valid());
	}

	// ===== edge_crossover =====
	void edge_crossover(p_chromosome * o, p_chromosome * p)
	{
		assert(o->is_valid());
		assert(p->is_valid());

		int counts[GRAPH_VERTICES], potential[GRAPH_VERTICES];
		int pot_vertices = 0;
		for (int i = 0; i < GRAPH_VERTICES; i++) { counts[i] = 1; }
		for (int i = 0; i < STRING_LENGTH; i++) { counts[o->pstring[i]]++; }
		for (int i = 0; i < STRING_LENGTH; i++) { if (o->pstring[i] != p->pstring[i] && counts[p->pstring[i]] < MAX_DEGREE) potential[pot_vertices++] = i; }

		if (pot_vertices == 0)
		{
			edge_mutate(o);
			return;
			
			/*cout << "P: ";
			p->print(cout);
			cout << "O: ";
			o->print(cout);
			cout << endl;*/
		}
		assert(pot_vertices > 0);
		int pos = potential[rand() % pot_vertices];
		o->pstring[pos] = p->pstring[pos];

		assert(o->is_valid());
		assert(p->is_valid());
	}

	// ===== randomize =====
	// will generate a random chromosome given a pointer 'p'
	// uses the second version of edge mutate to not check for a valid chromosome till the end.
	void randomize(p_chromosome * p) { for (int i = 0; i < STRING_LENGTH; i++) { edge_mutate(p, i, false); } assert(p->is_valid()); }
};

//
//class Prufer
//{
//public:
//
//	Graph * base;
//	int decode_method = method;
//	int fitness;
//	int prufer_string[GRAPH_VERTICES-2];
//
//	Prufer(Graph *original, bool random); // constructor
//	void randomize(void);
//	void get_fitness(void);
//	void copy(Prufer * p);
//	void print(ostream &out_s);
//	bool valid(void);
//	~Prufer();
//};
//
//Prufer::Prufer(Graph *original, bool random)
//{
//	base = new Graph(false);
//	base->copy(original);
//	if (random)
//	{
//		fitness = -1;
//		while (fitness == -1)
//		{
//			randomize();
//			get_fitness();
//		}
//	}
//	else
//	{
//		for (int i = 0; i < GRAPH_VERTICES - 2; i++)
//		{
//			prufer_string[i] = -1;
//		}
//		fitness = MAX_WEIGHT * (GRAPH_VERTICES - 1) + 1;
//	}
//	if(DEBUG)cout << ".";
//}
//
//void Prufer::randomize()
//{
//	bool flag = true;
//	int degree_counter[GRAPH_VERTICES][2];
//	for (int i = 0; i < GRAPH_VERTICES; i++)
//	{
//		degree_counter[i][0] = i;
//		degree_counter[i][1] = 1;
//	}
//
//	for (int i = 0; i < GRAPH_VERTICES - 2; i++) // generate random prufer string
//	{
//		flag = true;
//		while (flag)
//		{
//			prufer_string[i] = rand() % GRAPH_VERTICES;
//			if (degree_counter[prufer_string[i]][1] < MAX_DEGREE) // checks that max degree is not exceeded
//			{
//				degree_counter[prufer_string[i]][1]++;
//				flag = false;
//			}
//		}
//	}
//	if (DEBUG)
//	{
//		cout << "prufer string:";
//		for (int i = 0; i < GRAPH_VERTICES - 2; i++)
//		{
//			cout << prufer_string[i];
//		}
//		cout << endl;
//	}
//}
//
//void Prufer::get_fitness()
//{
//	int vertex_degree_list[GRAPH_VERTICES][2];
//	int cycles[GRAPH_VERTICES - 2][GRAPH_VERTICES - 1]; // -1 allows for delimiter -1 in the array
//	int vertex_already_checked[GRAPH_VERTICES]; // 0 for not checked, 1 for checked
//	int vertex_already_in_cycle[GRAPH_VERTICES]; // 0 for not already in a cycle, 1 already in cycle
//	int cycle_max_tracker[GRAPH_VERTICES - 2][3]; // cycles index, max value, max value index 
//	int cycle_max[3];
//	int vertex_list[GRAPH_VERTICES - 2];
//	int number_of_cycles;
//	int cycle_length;
//	int cycle_flag;
//	int cycle_index;
//	int vertex_index, vertex_index2;
//	int degree;
//	bool invalid_degree = false;
//	int weight;
//	bool invalid_edge = false;
//	Graph * prufer_tree = new Graph(false);
//
//	//////
//	if (decode_method == DANDELION)
//	{
//		for (int i = 0; i < GRAPH_VERTICES; i++)
//		{
//			vertex_already_checked[i] = 0;
//			vertex_already_checked[i] = 0;
//		}
//		number_of_cycles = 0; // start with 0 cycles
//		for (int i = 0; i < GRAPH_VERTICES-2; i++) // look for cycles for each element of the prufer string
//		{
//				vertex_index = i+1;
//				//cout << "*****" << vertex_index << endl;
//				/*for (int j = 0; j < GRAPH_VERTICES; j++)
//				{
//					cout << j << "(" << vertex_already_checked[j] << ") ";
//				}*/
//				//cout << endl;
//				if (vertex_already_checked[vertex_index] == 0) // only continue if this vertex hasn't been checked yet
//				{
//					cycle_length = 0;
//					cycle_flag = true;
//					while (cycle_flag)
//					{
//						//cout << "current vertex " << vertex_index << "(" << vertex_already_checked[vertex_index] << ")" << endl;
//						if (vertex_already_in_cycle[vertex_index] == 1)
//						{
//							//cout << "already in cycle\n";
//							cycle_flag = false;
//						}
//						else
//						{
//							vertex_already_checked[vertex_index] = 1;
//							if (prufer_string[vertex_index - 1] == 0 || prufer_string[vertex_index - 1] == (GRAPH_VERTICES - 1)) // if vertex cycle_index leads to vertex 0 or vertex GRAPH_VERTICES - 1, it's not on a cycle
//							{
//								//cout << "found min or max!\n";
//								cycle_flag = false;
//							}
//							else if (prufer_string[vertex_index - 1] == vertex_index) // cycle with itself
//							{
//								//cout << "found cycle of 1!\n";
//								cycles[number_of_cycles][0] = vertex_index;
//								cycles[number_of_cycles][1] = -1;
//								number_of_cycles++;
//								cycle_flag = false;
//								vertex_already_in_cycle[vertex_index] = 1;
//							}
//							else
//							{
//								cycles[number_of_cycles][cycle_length] = vertex_index; // add the vertex to the possible cycle
//								for (int j = 0; j < cycle_length; j++) // check if cycle was found
//								{
//									if (cycles[number_of_cycles][j] == cycles[number_of_cycles][cycle_length]) // cycle found
//									{
//										//cout << "found a cycle!\n";
//										//shift cycle down and append -1
//										for (int k = 0; k < (cycle_length - j); k++)
//										{
//											cycles[number_of_cycles][k] = cycles[number_of_cycles][k + j + 1];
//											vertex_already_in_cycle[cycles[number_of_cycles][k + j + 1]] = 1;
//										}
//										cycles[number_of_cycles][cycle_length - j] = -1;
//										cycle_flag = false;
//										number_of_cycles++;
//										j = cycle_length;
//									}
//								}
//								if (cycle_flag == true) // no cycle found in for loop
//								{
//									if (prufer_string[vertex_index - 1] < (i + 1)) // no cycle formed and vertex already checked
//									{
//										//cout << "lower vertex\n";
//										cycle_flag = false;
//									}
//									else
//									{
//										cycle_length++;
//										vertex_index = prufer_string[vertex_index - 1];
//									}
//								}
//							}
//						}
//					}
//				}
//		}
//		//cout << "cycles\n";
//		/*for (int i = 0; i < number_of_cycles; i++)
//		{
//			cycle_index = 0;
//			while (cycles[i][cycle_index] != -1 && cycle_index < (GRAPH_VERTICES-1))
//			{
//				//cout << cycles[i][cycle_index] << " ";
//				cycle_index++;
//			}
//			//cout << endl;
//		}*/
//		//examine cycles for maximums
//		for (int i = 0; i < number_of_cycles; i++)
//		{
//			cycle_index = 0;
//			cycle_max[0] = i;
//			cycle_max[1] = 0;
//			// find maximum of cycle i
//			while (cycles[i][cycle_index] != -1 && cycle_index < (GRAPH_VERTICES - 1))
//			{
//				if (cycles[i][cycle_index] > cycle_max[1])
//				{
//					cycle_max[1] = cycles[i][cycle_index];
//					cycle_max[2] = cycle_index;
//				}
//				cycle_index++;
//			}
//			// insert to cycle_max_tracker ordering from highest to lowest
//			cycle_index = 0;
//			while (cycle_index < i)
//			{
//				if (cycle_max_tracker[cycle_index][1] < cycle_max[1]) // new max
//				{
//					// shift other
//					for (int j = i; j > cycle_index; j--)
//					{
//						cycle_max_tracker[j][0] = cycle_max_tracker[j - 1][0];
//						cycle_max_tracker[j][1] = cycle_max_tracker[j - 1][1];
//						cycle_max_tracker[j][2] = cycle_max_tracker[j - 1][2];
//					}
//					cycle_max_tracker[cycle_index][0] = cycle_max[0];
//					cycle_max_tracker[cycle_index][1] = cycle_max[1];
//					cycle_max_tracker[cycle_index][2] = cycle_max[2];
//					cycle_index = i + 1;
//				}
//				cycle_index++;
//			}
//			if (cycle_index == i) // this is the lowest max
//			{
//				cycle_max_tracker[i][0] = cycle_max[0];
//				cycle_max_tracker[i][1] = cycle_max[1];
//				cycle_max_tracker[i][2] = cycle_max[2];
//			}
//		}
//		//cout << "cycles ordered\n";
//		/*for (int i = 0; i < number_of_cycles; i++)
//		{
//			cycle_index = 0;
//			while (cycles[cycle_max_tracker[i][0]][cycle_index] != -1 && cycle_index < (GRAPH_VERTICES - 1))
//			{
//				//cout << cycles[cycle_max_tracker[i][0]][cycle_index] << " ";
//				cycle_index++;
//			}
//			cout << endl;
//		}*/
//		// order into single list of vertices
//		////
//		//cout << "ordered string\n";
//		vertex_index = 0;
//		for (int i = 0; i < number_of_cycles; i++)
//		{
//			cycle_index = cycle_max_tracker[i][2]+1;
//			while (cycles[cycle_max_tracker[i][0]][cycle_index] != -1 && cycle_index < (GRAPH_VERTICES - 1))
//			{
//				/////
//				//cout << cycles[cycle_max_tracker[i][0]][cycle_index] << " ";
//				vertex_list[vertex_index] = cycles[cycle_max_tracker[i][0]][cycle_index];
//				vertex_index++;
//				cycle_index++;
//			}
//			for (int j = 0; j < cycle_max_tracker[i][2] + 1; j++)
//			{
//				//cout << cycles[cycle_max_tracker[i][0]][j] << " ";
//				vertex_list[vertex_index] = cycles[cycle_max_tracker[i][0]][j];
//				vertex_index++;
//			}
//		}
//		/////
//		//cout << endl;
//		for (int i = 0; i < GRAPH_VERTICES; i++)
//		{
//			vertex_already_checked[i] = 0;
//		}
//		//now form the tree
//
//
//		// add vertices from cycles to the list
//		if (vertex_index == 0)
//		{
//			// connect vertex 0 and vertex GRAPH_VERTICES - 1
//			//find the weight from vertex 0 to GRAPH_VERTICES - 1
//			for (int j = 0; j < base->vertices[0].connected_vertices_count; j++) // search for weight of connecting edge on base
//			{
//				if (base->vertices[0].connected_vertices[j]->id == (GRAPH_VERTICES - 1))
//				{
//					//cout << "found it\n";
//					weight = base->vertices[0].connected_vertices_weights[j];
//					j = base->vertices[0].connected_vertices_count;
//				}
//			}
//			// connect vertex 0 with first vertex of vertex_list
//			prufer_tree->connect(&prufer_tree->vertices[0], &prufer_tree->vertices[GRAPH_VERTICES - 1], weight);
//			//cout << "*connecting vertex 0 with vertex " << GRAPH_VERTICES - 1 << " with weight " << weight << endl;
//			vertex_already_checked[0] = 1;
//			vertex_already_checked[GRAPH_VERTICES-1] = 1;
//		}
//		else
//		{
//			//find the weight from vertex 0 to first vertex of vertex_list
//			for (int j = 0; j < base->vertices[0].connected_vertices_count; j++) // search for weight of connecting edge on base
//			{
//				if (base->vertices[0].connected_vertices[j]->id == vertex_list[0])
//				{
//					//cout << "found it\n";
//					weight = base->vertices[0].connected_vertices_weights[j];
//					j = base->vertices[0].connected_vertices_count;
//				}
//			}
//			// connect vertex 0 with first vertex of vertex_list
//			prufer_tree->connect(&prufer_tree->vertices[0], &prufer_tree->vertices[vertex_list[0]], weight);
//			//cout << "**connecting vertex 0 with vertex " << vertex_list[0] << " with weight " << weight << endl;
//			vertex_already_checked[0] = 1;
//			// connect vertices that are adjacent to each other in vertex_list
//			for (int k = 0; k < vertex_index - 1; k++)
//			{
//				//find the weight from vertex vertex_list[k] to vertex_list[k+1]
//				for (int j = 0; j < base->vertices[vertex_list[k]].connected_vertices_count; j++) // search for weight of connecting edge on base
//				{
//					if (base->vertices[vertex_list[k]].connected_vertices[j]->id == vertex_list[k + 1])
//					{
//						//cout << "found it\n";
//						weight = base->vertices[vertex_list[k]].connected_vertices_weights[j];
//						j = base->vertices[vertex_list[k]].connected_vertices_count;
//					}
//				}
//				// connect vertex vertex_list[i] with vertex vertex_list[i+1]
//				prufer_tree->connect(&prufer_tree->vertices[vertex_list[k]], &prufer_tree->vertices[vertex_list[k + 1]], weight);
//				//cout << "***connecting vertex " << vertex_list[k] << " with vertex " << vertex_list[k + 1] << " with weight " << weight << endl;
//				vertex_already_checked[vertex_list[k]] = 1;
//			}
//			//find the weight from vertex GRAPH_VERTICES-1 to last vertex of vertex_list
//			for (int j = 0; j < base->vertices[GRAPH_VERTICES - 1].connected_vertices_count; j++) // search for weight of connecting edge on base
//			{
//				if (base->vertices[GRAPH_VERTICES - 1].connected_vertices[j]->id == vertex_list[vertex_index - 1])
//				{
//					//cout << "found it\n";
//					weight = base->vertices[GRAPH_VERTICES - 1].connected_vertices_weights[j];
//					j = base->vertices[GRAPH_VERTICES - 1].connected_vertices_count;
//				}
//			}
//			// connect vertex GRAPH_VERTICES-1 with last vertex of vertex_list
//			prufer_tree->connect(&prufer_tree->vertices[GRAPH_VERTICES - 1], &prufer_tree->vertices[vertex_list[vertex_index - 1]], weight);
//			//cout << "****connecting vertex " << GRAPH_VERTICES - 1 << " with vertex " <<  vertex_list[vertex_index - 1] << " with weight " << weight << endl;
//			vertex_already_checked[vertex_list[vertex_index - 1]] = 1;
//			vertex_already_checked[GRAPH_VERTICES - 1] = 1;
//		}
//
//		// connect all vertices not in the cycles
//		for (int i = 0; i < GRAPH_VERTICES - 2; i++)
//		{
//			if (vertex_already_checked[i+1] == 0) // only add what hasn't been added already
//			{
//				//connect vertex i+1 with vertex prufer_string[i]
//				//find the weight from vertex vertex_list[i] to vertex_list[i+1]
//				for (int j = 0; j < base->vertices[i+1].connected_vertices_count; j++) // search for weight of connecting edge on base
//				{
//					if (base->vertices[i+1].connected_vertices[j]->id == prufer_string[i])
//					{
//						//cout << "found it\n";
//						weight = base->vertices[i+1].connected_vertices_weights[j];
//						j = base->vertices[i+1].connected_vertices_count;
//					}
//				}
//				// connect vertex vertex_list[i] with vertex vertex_list[i+1]
//				prufer_tree->connect(&prufer_tree->vertices[i+1], &prufer_tree->vertices[prufer_string[i]], weight);
//				//cout << "*****connecting vertex " <<i+1 <<" with vertex " << prufer_string[i]<< " with weight " << weight  << endl;
//
//			}
//		}
//		//fitness = 0;
//		fitness = prufer_tree->fitness();
//		//prufer_tree->print(cout);
//	}
//	else // decode_method = PRUFER
//	{
//		for (int i = 0; i < GRAPH_VERTICES; i++) // for each vertex in the graph
//		{
//			degree = 1;
//			for (int j = 0; j < GRAPH_VERTICES - 2; j++) // count the degree in tree
//			{
//				if (prufer_string[j] == i)
//				{
//					degree++;
//					if (degree > MAX_DEGREE)
//					{
//						invalid_degree = true;
//						j = GRAPH_VERTICES - 2;
//						i = GRAPH_VERTICES; // exit loops
//					}
//				}
//			}
//		}
//		if (invalid_degree == false)
//		{
//			//the following translates prufer string to tree
//			for (int i = 0; i < GRAPH_VERTICES; i++) // set up vertex degree list based on prufer string
//			{
//				vertex_degree_list[i][0] = i;
//				vertex_degree_list[i][1] = 1;
//				for (int j = 0; j < GRAPH_VERTICES - 2; j++) // increment degree for each instance of the vertex in the prufer string
//				{
//					if (prufer_string[j] == i)
//					{
//						vertex_degree_list[i][1]++;
//					}
//				}
//			}
//
//			if (DEBUG)
//			{
//				for (int i = 0; i < GRAPH_VERTICES; i++)
//				{
//					cout << vertex_degree_list[i][0] << ":" << vertex_degree_list[i][1] << endl;
//				}
//			}
//
//			// for each element in the prufer string
//			for (int i = 0; i < GRAPH_VERTICES - 2; i++)
//			{
//				if (DEBUG)cout << "****\n";
//				if (DEBUG)
//				{
//					for (int i = 0; i < GRAPH_VERTICES; i++)
//					{
//						cout << vertex_degree_list[i][0] << ":" << vertex_degree_list[i][1] << endl;
//					}
//				}
//				/////prufer_tree->print(cout);
//				// find appropriate vertex: lowest degree first then lowest index number
//				vertex_index = GRAPH_VERTICES - 1;
//				degree = MAX_DEGREE;
//				for (int j = GRAPH_VERTICES - 1; j >= 0; j--)
//				{
//					if (vertex_degree_list[j][1] <= degree && vertex_degree_list[j][1] > 0)
//					{
//						vertex_index = j;
//						degree = vertex_degree_list[j][1];
//					}
//				}
//				//
//				if (DEBUG) cout << "vertex1: " << vertex_index << " vertex2: " << prufer_string[i] << endl;
//				weight = -1;
//				for (int j = 0; j < base->vertices[vertex_index].connected_vertices_count; j++) // search for weight of connecting edge on base
//				{
//					if (base->vertices[vertex_index].connected_vertices[j]->id == prufer_string[i])
//					{
//						weight = base->vertices[vertex_index].connected_vertices_weights[j];
//						j = base->vertices[vertex_index].connected_vertices_count;
//					}
//				}
//				if (DEBUG)cout << "weight:" << weight << endl;
//				//assert(weight != -1);
//				if (weight == -1)
//				{
//					invalid_edge = true;
//				}
//				else
//				{
//					prufer_tree->connect(&prufer_tree->vertices[vertex_index], &prufer_tree->vertices[prufer_string[i]], weight);
//				}
//				vertex_degree_list[vertex_index][1]--;
//				vertex_degree_list[prufer_string[i]][1]--;
//			}
//			//add the last edge
//			vertex_index = -1;
//			for (int i = 0; i < GRAPH_VERTICES; i++)
//			{
//				if (vertex_degree_list[i][1] == 1)
//				{
//					if (vertex_index == -1)
//					{
//						vertex_index = i;
//					}
//					else
//					{
//						vertex_index2 = i;
//						i = GRAPH_VERTICES;
//					}
//				}
//			}
//			weight = -1;
//			for (int j = 0; j < base->vertices[vertex_index].connected_vertices_count; j++) // search for weight of connecting edge on base
//			{
//				if (base->vertices[vertex_index].connected_vertices[j]->id == vertex_index2)
//				{
//					weight = base->vertices[vertex_index].connected_vertices_weights[j];
//					j = base->vertices[vertex_index].connected_vertices_count;
//				}
//			}
//			if (DEBUG)cout << "weight:" << weight << endl;
//			//assert(weight != -1);
//			if (weight == -1)
//			{
//				invalid_edge = true;
//			}
//			else
//			{
//				prufer_tree->connect(&prufer_tree->vertices[vertex_index], &prufer_tree->vertices[vertex_index2], weight);
//			}
//			if (invalid_edge == true)
//			{
//				fitness = -1;
//			}
//			else
//			{
//				fitness = prufer_tree->fitness();
//			}
//			if (fitness != -1)
//			{
//				if (DEBUG)print(cout);
//				if (DEBUG)prufer_tree->print(cout);
//			}
//			if (DEBUG) cout << " fitness:" << fitness << endl;
//		}
//		else
//		{
//			fitness = -1;
//		}
//	}
//	delete prufer_tree;
//}
//
//void Prufer::copy(Prufer * p)
//{
//	for (int i = 0; i < GRAPH_VERTICES - 2; i++)
//	{
//		prufer_string[i] = p->prufer_string[i];
//	}
//	fitness = p->fitness;
//}
//
//void Prufer::print(ostream &out_s)
//{
//	out_s << "String:";
//	for (int i = 0; i < GRAPH_VERTICES - 2; i++)
//	{
//		out_s << " " << prufer_string[i];
//	}
//	out_s << "\nFitness: " << fitness << endl;
//}
//
//bool Prufer::valid(void)
//{
//	bool valid = true;
//	int counter[GRAPH_VERTICES];
//	for (int i = 0; i < GRAPH_VERTICES; i++) // initialize
//	{
//		counter[i] = 1;
//	}
//	for (int i = 0; i < GRAPH_VERTICES - 2; i++) // check how many times a vertex appears in the string
//	{
//		if (prufer_string[i] == -1)
//		{
//			valid = false;
//		}
//		else
//		{
//			counter[prufer_string[i]]++;
//			if (counter[prufer_string[i]] > MAX_DEGREE)
//			{
//				valid = false;
//			}
//		}
//	}
//	return valid;
//}
//Prufer::~Prufer()
//{
//	if(base != NULL)delete base;
//}

