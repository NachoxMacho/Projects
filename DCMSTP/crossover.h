#pragma once

#ifndef CROSSOVER_H
#define CROSSOVER_H

#include "graph.h"
#include "prufer.h"

//#define POP_SIZE GRAPH_VERTICES*2
#define POP_SIZE P_POP_SIZE
#define MAX_STALENESS P_MAX_STALENESS

#define SIMILAR_ALTERNATING 0
#define SIMILAR_RANDOM 1
#define SLICES 3 // n point crossover
#define REPLACE_FIFTY_PERCENT 2 // SIMILAR_RANDOM with moving 50% of the current population to the next generation
#define MUTATION 4
#define SIMILAR_REPLACE_ONE 5
#define TOTAL_CROSSOVER_TYPES 6

#define PARENTS P_PARENTS // number of parents a new AL_chromosome is made from
#define SELECTION P_SELECTION // number of chromosomes in the tournament selection to choose parents -> 4 to get in top 25% of population

class Crossover
{
public:

	Crossover(Graph * original); // constructor	
	void run(void);
	void set_type(int type);
	~Crossover();

	int degree_counter[GRAPH_VERTICES];
	int x_type;
	Graph * base;
	Prufer * current_best_prufer;
	int generations;
};

Crossover::Crossover(Graph * original)
{
	base = new Graph(false);
	base->copy(original);
	generations = 0;
	current_best_prufer = new Prufer(base, false);
	x_type = SIMILAR_ALTERNATING;
}

void Crossover::run()
{
	//Prufer * p = new Prufer(base);
	Prufer * pop[2][POP_SIZE]; // matrix for 2 generations
	Prufer * gen_best_prufer = new Prufer(base, false);
	int child_gen = 0;
	int parent_gen = 1;
	int chromosomes = 0;
	int staleness = 0;
	int selection[SELECTION];
	int parent[PARENTS];
	int chosen_parent, chosen_selection, counter, temp_vertex, copied_counter, copied_parent;
	int not_similar[GRAPH_VERTICES - 2];
	int not_similar_total;
	bool parents_similar, proceed;


	// generate 1st gen population
	while(chromosomes < POP_SIZE)
	{
		pop[parent_gen][chromosomes] = new Prufer(base, false); // generate empty prufer strings to be populated by first child generation
		pop[child_gen][chromosomes] = new Prufer(base, true); // generate random starting chromosomes
		//check fitness against generation's best fitness
		if (pop[child_gen][chromosomes]->fitness < gen_best_prufer->fitness)
		{
			gen_best_prufer->copy(pop[child_gen][chromosomes]);
		}
		if(DEBUG) pop[child_gen][chromosomes]->print(cout);
		chromosomes++;
	}
	generations++;

	//update current best fitness
	if (gen_best_prufer->fitness < current_best_prufer->fitness)
	{
		current_best_prufer->copy(gen_best_prufer);
	}

	if(DEBUG) cout << "Current best: ";
	if(DEBUG) current_best_prufer->print(cout);

	// run next gen while not stale
	while (staleness < MAX_STALENESS)
	{
		generations++;
		if(DEBUG)cout << "DEBUG staleness: " << staleness << endl;
		// reset chromosome counter
		chromosomes = 0;
		// toggle which array is child pop
		if (child_gen == 0)
		{
			child_gen = 1;
			parent_gen = 0;
		}
		else
		{
			child_gen = 0;
			parent_gen = 1;
		}
		// clear out child generation
		for (int i = 0; i < POP_SIZE; i++)
		{
			if (pop[child_gen][chromosomes] != NULL)
			{
				delete pop[child_gen][i];
			}
		}
		// copy over gen_best to next gen
		pop[child_gen][chromosomes] = new Prufer(base, false);
		pop[child_gen][chromosomes]->copy(current_best_prufer);
		chromosomes++;

		// create remainder of next generation
		while (chromosomes < POP_SIZE)
		{
			// generate new chromosome and add it to the child population

			for (int i = 0; i < PARENTS; i++) // choose parents
			{
				parents_similar = true;
				while (parents_similar == true)
				{
					for (int j = 0; j < SELECTION; j++) // get SELECTION many chromosomes of parent gen
					{
						selection[j] = rand() % POP_SIZE;
					}
					chosen_selection = 0;
					for (int j = 1; j < SELECTION; j++) // find best fitness from selections
					{
						if (pop[parent_gen][selection[j]]->fitness < pop[parent_gen][selection[chosen_selection]]->fitness)
						{
							chosen_selection = j;
						}
					}
					//assign parent to best random selection
					parent[i] = selection[chosen_selection];
					parents_similar = false;
					for (int j = 0; j < i; j++)
					{
						if (parent[j] == parent[i])
						{
							parents_similar = true;
						}
					}
				}
			}

			// now populate the new string...
			pop[child_gen][chromosomes] = new Prufer(base, false); // create new empty prufer string
			//pop[child_gen][chromosomes]->fitness = -1;
			//while (pop[child_gen][chromosomes]->fitness == -1) // while invalid string is generated
			while(pop[child_gen][chromosomes]->valid() == false)
			{
				//initialize degree counter
				for (int i = 0; i < GRAPH_VERTICES; i++)
				{
					degree_counter[i] = 1;
				}
				//initialize prufer string
				for (int i = 0; i < GRAPH_VERTICES - 2; i++)
				{
					pop[child_gen][chromosomes]->prufer_string[i] = -1;
				}
				switch (x_type) // determines how crossover is done
				{
					case SIMILAR_ALTERNATING:
					{
						chosen_parent = rand() % PARENTS; // choose random starting parent when alternating
						for (int i = 0; i < GRAPH_VERTICES - 2; i++) // for each element in the prufer string copy it if it's similar between parents
						{
							parents_similar = true;
							counter = 1;
							while (parents_similar == true && counter < PARENTS)
							{
								if (pop[parent_gen][parent[0]]->prufer_string[i] != pop[parent_gen][parent[counter]]->prufer_string[i]) // check if parents differ at the i'th element
								{
									parents_similar = false;
								}
								counter++;
							}
							if (parents_similar) // if i'th element of all parents is same, copy to child
							{
								pop[child_gen][chromosomes]->prufer_string[i] = pop[parent_gen][parent[0]]->prufer_string[i];
								degree_counter[pop[parent_gen][parent[0]]->prufer_string[i]]++;
							}
							/*else // copy an element from one of the parents, alternating between parents as elements are copied
							{
								pop[child_gen][chromosomes]->prufer_string[i] = pop[parent_gen][parent[chosen_parent]]->prufer_string[i];
								chosen_parent++;
								if (chosen_parent == PARENTS) // wrap around when max is reached
								{
									chosen_parent = 0;
								}
							}*/
						}
						for (int i = 0; i < GRAPH_VERTICES - 2; i++) // for each element, pull element from a parent if it wasn't similar (if value is still -1)
						{
							if (pop[child_gen][chromosomes]->prufer_string[i] == -1)
							{
								if (degree_counter[pop[parent_gen][parent[chosen_parent]]->prufer_string[i]] < MAX_DEGREE) // check if max degree has been reached already
								{
									pop[child_gen][chromosomes]->prufer_string[i] = pop[parent_gen][parent[chosen_parent]]->prufer_string[i];
									degree_counter[pop[parent_gen][parent[chosen_parent]]->prufer_string[i]]++;
								}
								chosen_parent++;
								if (chosen_parent == PARENTS) // wrap around when max is reached
								{
									chosen_parent = 0;
								}
							}
						}
						for (int i = 0; i < GRAPH_VERTICES - 2; i++) // for each remaining element, assign a random vertex
						{
							if (pop[child_gen][chromosomes]->prufer_string[i] == -1)
							{
								temp_vertex = rand() % GRAPH_VERTICES;
								while (degree_counter[temp_vertex] >= MAX_DEGREE) // ensure max degree is observed
								{
									temp_vertex = rand() % GRAPH_VERTICES;
								}
								pop[child_gen][chromosomes]->prufer_string[i] = temp_vertex;
							}
						}
						break;
					}
					case SIMILAR_RANDOM:
					{
						for (int i = 0; i < GRAPH_VERTICES - 2; i++) // for each element in the prufer string copy it if it's similar between parents
						{
							parents_similar = true;
							counter = 1;
							while (parents_similar == true && counter < PARENTS)
							{
								if (pop[parent_gen][parent[0]]->prufer_string[i] != pop[parent_gen][parent[counter]]->prufer_string[i]) // check if parents differ at the i'th element
								{
									parents_similar = false;
								}
								counter++;
							}
							if (parents_similar) // if i'th element of all parents is same, copy to child
							{
								pop[child_gen][chromosomes]->prufer_string[i] = pop[parent_gen][parent[0]]->prufer_string[i];
								degree_counter[pop[parent_gen][parent[0]]->prufer_string[i]]++;
							}
						}
						for (int i = 0; i < GRAPH_VERTICES - 2; i++) // for each remaining element, assign a random vertex
						{
							if (pop[child_gen][chromosomes]->prufer_string[i] == -1)
							{
								temp_vertex = rand() % GRAPH_VERTICES;
								while (degree_counter[temp_vertex] >= MAX_DEGREE) // ensure max degree is observed
								{
									temp_vertex = rand() % GRAPH_VERTICES;
								}
								pop[child_gen][chromosomes]->prufer_string[i] = temp_vertex;
							}
						}
						break;
					}
					case SLICES:
					{
						counter = 0;
						chosen_parent = 0; // choose starting parent
						for (int i = 0; i < GRAPH_VERTICES - 2; i++) // for each element in the prufer string copy it from parent
						{
							if (degree_counter[pop[parent_gen][parent[chosen_parent]]->prufer_string[i]] < MAX_DEGREE) // if possible to copy parent
							{
								pop[child_gen][chromosomes]->prufer_string[i] = pop[parent_gen][parent[chosen_parent]]->prufer_string[i]; // copy element from chosen parent string
								degree_counter[pop[parent_gen][parent[chosen_parent]]->prufer_string[i]]++;
							}
							counter++;
							if (counter >= (GRAPH_VERTICES / PARENTS)) // after a chosen_parent's slice is done, change chosen_parent
							{
								counter = 0;
								chosen_parent++;
								if (chosen_parent >= PARENTS)
								{
									chosen_parent = 0;
								}
							}
						}
						for (int i = 0; i < GRAPH_VERTICES - 2; i++) // for each remaining element, assign a random vertex
						{
							if (pop[child_gen][chromosomes]->prufer_string[i] == -1)
							{
								temp_vertex = rand() % GRAPH_VERTICES;
								while (degree_counter[temp_vertex] >= MAX_DEGREE) // ensure max degree is observed
								{
									temp_vertex = rand() % GRAPH_VERTICES;
								}
								pop[child_gen][chromosomes]->prufer_string[i] = temp_vertex;
							}
						}
						/*counter = 0;
						chosen_parent = 0;
						for (int i = 0; i < GRAPH_VERTICES-2; i++) // for each element in the parent string
						{
							if (degree_counter[pop[parent_gen][parent[chosen_parent]]->prufer_string[i]] < MAX_DEGREE) // if possible to copy parent
							{
								pop[child_gen][chromosomes]->prufer_string[i] = pop[parent_gen][parent[chosen_parent]]->prufer_string[i]; // copy element from chosen parent string
								degree_counter[pop[parent_gen][parent[chosen_parent]]->prufer_string[i]]++;
							}
							else // generate random vertex to assign
							{
								temp_vertex = rand() % GRAPH_VERTICES;
								while (degree_counter[temp_vertex] >= MAX_DEGREE) // get vaild vertex
								{
									pop[child_gen][chromosomes]->prufer_string[i] = temp_vertex;
								}
							}
							counter++;
							if (counter >= (GRAPH_VERTICES / PARENTS)) // after a chosen_parent's slice is done, change chosen_parent
							{
								counter = 0;
								chosen_parent++;
								if (chosen_parent >= PARENTS)
								{
									chosen_parent = 0;
								}
							}
						}*/
						break;
					}
					case REPLACE_FIFTY_PERCENT:
					{
						for (int i = 0; i < GRAPH_VERTICES - 2; i++) // for each element in the prufer string copy it if it's similar between parents
						{
							parents_similar = true;
							counter = 1;
							while (parents_similar == true && counter < PARENTS)
							{
								if (pop[parent_gen][parent[0]]->prufer_string[i] != pop[parent_gen][parent[counter]]->prufer_string[i]) // check if parents differ at the i'th element
								{
									parents_similar = false;
								}
								counter++;
							}
							if (parents_similar) // if i'th element of all parents is same, copy to child
							{
								pop[child_gen][chromosomes]->prufer_string[i] = pop[parent_gen][parent[0]]->prufer_string[i];
								degree_counter[pop[parent_gen][parent[0]]->prufer_string[i]]++;
							}
						}
						for (int i = 0; i < GRAPH_VERTICES - 2; i++) // for each remaining element, assign a random vertex
						{
							if (pop[child_gen][chromosomes]->prufer_string[i] == -1)
							{
								temp_vertex = rand() % GRAPH_VERTICES;
								while (degree_counter[temp_vertex] >= MAX_DEGREE) // ensure max degree is observed
								{
									temp_vertex = rand() % GRAPH_VERTICES;
								}
								pop[child_gen][chromosomes]->prufer_string[i] = temp_vertex;
							}
						}
						break;
					}
					case MUTATION:
					{
						pop[child_gen][chromosomes]->copy(pop[parent_gen][parent[rand() % PARENTS]]); // copy random parent
						for (int i = 0; i < GRAPH_VERTICES - 2; i++)
						{
							degree_counter[pop[child_gen][chromosomes]->prufer_string[i]]++;
						}
						temp_vertex = rand() % GRAPH_VERTICES;
						while (degree_counter[temp_vertex] >= MAX_DEGREE)
						{
							temp_vertex = rand() % GRAPH_VERTICES;
						}
						pop[child_gen][chromosomes]->prufer_string[rand() % (GRAPH_VERTICES - 2)] = temp_vertex; // change random element to random vertex
						break;
					}
					case SIMILAR_REPLACE_ONE:
					{
						/////cout << ".";
						not_similar_total = 0;
						chosen_parent = rand() % PARENTS; // choose random starting parent when alternating
						/////cout << "parents " << parent[0] << ' ' << parent[1] << endl;
						/////pop[parent_gen][parent[0]]->print(cout);
						/////pop[parent_gen][parent[1]]->print(cout);
						for (int i = 0; i < GRAPH_VERTICES - 2; i++) // for each element in the prufer string copy it if it's similar between parents
						{
							parents_similar = true;
							counter = 1;
							while (parents_similar == true && counter < PARENTS)
							{
								if (pop[parent_gen][parent[0]]->prufer_string[i] != pop[parent_gen][parent[counter]]->prufer_string[i]) // check if parents differ at the i'th element
								{
									parents_similar = false;
								}
								counter++;
							}
							if (parents_similar == false)
							{
								not_similar[not_similar_total] = i;
								not_similar_total++;
							}
						}
						chosen_parent = rand() % PARENTS;
						pop[child_gen][chromosomes]->copy(pop[parent_gen][parent[chosen_parent]]); // copy random parent
						for (int i = 0; i < GRAPH_VERTICES - 2; i++)
						{
							degree_counter[pop[child_gen][chromosomes]->prufer_string[i]]++;
						}
						if (not_similar_total == 0) // mutate random edge
						{
							temp_vertex = rand() % GRAPH_VERTICES;
							while (degree_counter[temp_vertex] >= MAX_DEGREE)
							{
								temp_vertex = rand() % GRAPH_VERTICES;
							}
							pop[child_gen][chromosomes]->prufer_string[rand() % (GRAPH_VERTICES - 2)] = temp_vertex; // change random element to random vertex
						}
						else
						{
							// select random element that is different among parents
							// choose random parent and replace that respective element if it is different and if it does not violate degree constraint
							counter = 0;
							proceed = false;
							while (!proceed && counter < 10)
							{
								copied_counter = rand() % not_similar_total; // random element index for not_similar[]
								copied_parent = rand() % PARENTS; // parent index to copy from
								while (copied_parent == chosen_parent)
								{
									copied_parent = rand() % PARENTS;
								}
								while (pop[child_gen][chromosomes]->prufer_string[not_similar[copied_counter]] == pop[parent_gen][parent[copied_parent]]->prufer_string[not_similar[copied_counter]]) // while the element to copy is similar in the child to the chosen parent
								{
									copied_counter = rand() % not_similar_total;
									copied_parent = rand() % PARENTS; // parent index to copy from
									while (copied_parent == chosen_parent)
									{
										copied_parent = rand() % PARENTS;
									}
								}
								if (pop[parent_gen][parent[copied_parent]]->prufer_string[not_similar[copied_counter]] < MAX_DEGREE)
								{
									proceed = true;
								}
								counter++;
							}
							if (counter >= 10)
							{
								temp_vertex = rand() % GRAPH_VERTICES;
								while (degree_counter[temp_vertex] >= MAX_DEGREE)
								{
									temp_vertex = rand() % GRAPH_VERTICES;
								}
								pop[child_gen][chromosomes]->prufer_string[rand() % (GRAPH_VERTICES - 2)] = temp_vertex; // change random element to random vertex

							}
							else
							{
								pop[child_gen][chromosomes]->prufer_string[not_similar[copied_counter]] = pop[parent_gen][parent[copied_parent]]->prufer_string[not_similar[copied_counter]];
							}
						}
						break;
					}
					default: // randomize all
					{
						pop[child_gen][chromosomes]->randomize();
						break;
					}
				}
				//pop[child_gen][chromosomes]->get_fitness();
			}
			pop[child_gen][chromosomes]->get_fitness();
			//check fitness of new chromosome against gen best
			if (pop[child_gen][chromosomes]->fitness < gen_best_prufer->fitness)
			{
				gen_best_prufer->copy(pop[child_gen][chromosomes]);
			}

			chromosomes++;

			if (x_type == REPLACE_FIFTY_PERCENT && chromosomes < POP_SIZE) 
			{
				pop[child_gen][chromosomes] = new Prufer(base, false);
				pop[child_gen][chromosomes]->copy(pop[parent_gen][parent[(rand() % PARENTS)]]);// copy over one of the parent chromosomes
				chromosomes++;
			}
		}

		//check staleness of generation
		if (gen_best_prufer->fitness < current_best_prufer->fitness)
		{
			current_best_prufer->copy(gen_best_prufer);
			staleness = 0;
		}
		else
		{
			staleness++;
		}
		if(DEBUG)cout << "Current best: ";
		if(DEBUG)current_best_prufer->print(cout);
	}
	//cleanup
	for (int i = 0; i < POP_SIZE; i++)
	{
		if(pop[child_gen][i] != NULL)delete pop[child_gen][i];
		if(pop[parent_gen][i] != NULL) delete pop[parent_gen][i];
	}
	if(gen_best_prufer != NULL)delete gen_best_prufer;
}

void Crossover::set_type(int type)
{
	x_type = type;
}

Crossover::~Crossover()
{
	if(base != NULL) delete base;
	if(current_best_prufer != NULL)delete current_best_prufer;
}


#endif /* CROSSOVER_H*/
