#include <fstream>
#include "graph.h"
#include "mutation.h"
#include "greedy.h"
// #include "crossover.h"
#include "prufer.h"
#include "dandelion.h"
using namespace std;

#define runs P_RUNS

int main()
{
	ofstream out_file, trees, analysis;
	out_file.open("results.csv");
	trees.open("trees.txt");
	analysis.open("analysis.csv");
	int start, end;
	int overall_best = GRAPH_VERTICES * MAX_WEIGHT;
	int totalGenerations;
	int totalBestFitness;
	int overallBestFitness;
	int runStart;

	// This is all you need to seed based on time
	// Will now output seed to screen and file for troubleshooting
	int seed = time(NULL);
	cout << "Seed: " << seed << endl;
	out_file << "Seed: " << seed << endl;
	srand(seed);

	// makes a new base graph
	Graph * original = new Graph(true);
	Graph * best_tree = new Graph(false);

	// display graph in output file
	trees << endl << "Base graph vertices:\n";
	original->print(trees);
	trees << endl;

	// ===== Dandelion Run =====

	

	// Stats for GA
	totalGenerations = 0;
	totalBestFitness = 0;
	overallBestFitness = maximum_weight;
	runStart = time(NULL);

	out_file << " Dandelion Mutation Run, Best Fitness, Generations, Time" << endl;

	// run the GA 'runs' amount of times
	for (int j = 0; j < runs; j++)
	{
		// instance of the prufer
		dandelion * d = new dandelion(original);

		// record start time
		start = time(NULL);

		// run the GA
		while (d->staleness < max_staleness)
			d->run_generation();

		// get the end time
		end = time(NULL);

		// record stats
		totalGenerations += d->generations;
		totalBestFitness += d->bestFitness;
		if (d->bestFitness < overallBestFitness)
		{
			overallBestFitness = d->bestFitness;
			assert(d->population[0]->is_valid());
			delete best_tree;
			best_tree = d->decode(d->population[0]);
			assert(best_tree->isInConstraint());
		}

		// output stats
		out_file << j << ", " << d->bestFitness << ", " << d->generations << ", " << end - start << endl;
		cout << "Dandelion Mutation Run: " << j << " | Best Fitness: " << d->bestFitness << " | Generations: " << d->generations << " | Time: " << end - start << endl;

		// delete and recreate GA
		delete d;
		//d = new dandelion(original);
	}
	// output final stats & delete GA
	cout << "Finished Dandelion Mutation Run | Best Fitness: " << overallBestFitness << " | Avg End Fitness: " << (float)totalBestFitness / runs << " | Avg Generations: " << (float)totalGenerations / runs << " | Time: " << time(NULL) - runStart << endl;
	out_file << endl;
	// delete d;

	// ===== Prufer Run =====

	// instance of the prufer
	prufer * p = new prufer(original);

	// Stats for GA
	totalGenerations = 0;
	totalBestFitness = 0;
	overallBestFitness = p->bestFitness;
	runStart = time(NULL);

	out_file << " Prufer Mutation Run, Best Fitness, Generations, Time" << endl;

	// run the GA 'runs' amount of times
	for (int j = 0; j < runs; j++)
	{
		// record start time
		start = time(NULL);

		// run the GA
		while (p->staleness < max_staleness)
			p->run_generation();

		// get the end time
		end = time(NULL);

		// record stats
		totalGenerations += p->generations;
		totalBestFitness += p->bestFitness;
		if (p->bestFitness < overallBestFitness)
		{
			overallBestFitness = p->bestFitness;
			assert(p->population[0]->is_valid());
			delete best_tree;
			best_tree = p->decode(p->population[0]);
			assert(best_tree->isInConstraint());
		}

		// output stats
		out_file << j << ", " << p->bestFitness << ", " << p->generations << ", " << end - start << endl;
		cout << "Prufer Mutation Run: " << j << " | Best Fitness: " << p->bestFitness << " | Generations: " << p->generations << " | Time: " << end - start << endl;

		// delete and recreate GA
		delete p;
		p = new prufer(original);
	}
	// output final stats & delete GA
	cout << "Finished Prufer Mutation Run | Best Fitness: " << overallBestFitness << " | Avg End Fitness: " << (float)totalBestFitness / runs << " | Avg Generations: " << (float)totalGenerations / runs << " | Time: " << time(NULL) - runStart << endl;
	out_file << endl;
	delete p;

	//output best tree to file
	trees << endl << "Best tree vertices (Prufer Mutation): " << endl;
	best_tree->print(trees);
	trees << endl;
	analysis << best_tree->fitness() << ",";

	// ===== Greedy Run =====

	//instance of Greedy
	Greedy * greedy = new Greedy(original);

	// Stats for greedy algorithm
	int counter;
	float g_totalBestFitness = 0;
	int g_overallBestFitness = GRAPH_VERTICES * MAX_WEIGHT;
	int g_trees_found = 0;
	Graph * g_overall_best_tree = new Graph(false);
	start = time(NULL);

	for (int i = 0; i < runs; i++)
	{
		counter = 0;
		while (!greedy->best_tree->isGraphConnected() && counter < GRAPH_VERTICES + 1) // counter allows loop to exit if connected graph not possible
		{
			greedy->find_shortest_edge();
			counter++;
		}

		greedy->fitness();
		if (greedy->bestFitness > 0) // valid graph found
		{
			g_totalBestFitness += greedy->bestFitness;
			g_trees_found++;
			if (greedy->bestFitness < g_overallBestFitness)
			{
				g_overallBestFitness = greedy->bestFitness;
				g_overall_best_tree->copy(greedy->best_tree);
			}
		}
		delete greedy;
		greedy = new Greedy(original);
	}

	end = time(NULL);

	out_file << "Best Overall (greedy): " << g_overallBestFitness << " | Average Best: " << g_totalBestFitness / g_trees_found << " | Valid trees found: " << g_trees_found << " | Runs: " << runs << " | Time: " << end - start << endl;
	cout << "Best Overall (greedy): " << g_overallBestFitness << " | Average Best: " << g_totalBestFitness / g_trees_found << " | Valid trees found: " << g_trees_found << " | Runs: " << runs << " | Time: " << end - start << endl;


	trees << endl << "Best tree vertices (greedy):\n";
	g_overall_best_tree->print(trees);
	trees << endl;
	analysis << g_overall_best_tree->fitness() << ",";

	// ===== AL Run =====

	// instance of the adList
	adList * AL = new adList(original, max_popSize);
	AL->mutations = 1;
	AL->op = AL_MUTATION;

	// Stats for GA
	totalGenerations = 0;
	totalBestFitness = 0;
	overallBestFitness = AL->bestFitness;
	runStart = time(NULL);

	out_file << "1 Tree Mutation AL Run, Best Fitness, Generations, Time" << endl;

	// run the GA run amount of times
	for (int j = 0; j < runs; j++)
	{
		AL->mutations = 1;
		start = time(NULL);

		while (AL->staleness < max_staleness)
			AL->run_generation();
		end = time(NULL);
		totalGenerations += AL->generations;
		totalBestFitness += AL->bestFitness;
		if (AL->bestFitness < overallBestFitness)
		{
			overallBestFitness = AL->bestFitness;
			best_tree->copy(AL->population[0]->tree);
		}

		out_file << j << ", " << AL->bestFitness << ", " << AL->generations << ", " << end - start << endl;
		cout << "1 Tree Mutation AL Run: " << j << " | Best Fitness: " << AL->bestFitness << " | Generations: " << AL->generations << " | Time: " << end - start << endl;

		delete AL;
		AL = new adList(original, max_popSize);
		AL->op = AL_MUTATION;
	}
	out_file << endl;
	delete AL;

	cout << "Finished 1 Tree Mutation AL Run | Best Fitness: " << overallBestFitness << " | Avg End Fitness: " << (float)totalBestFitness / runs << " | Avg Generations: " << (float)totalGenerations / runs << " | Time: " << time(NULL) - runStart << endl;


	//output best tree to file
	trees << endl << "Best tree vertices (1 Tree Mutation): " << endl;
	best_tree->print(trees);
	trees << endl;
	analysis << best_tree->fitness() << ",";
	// delete best_tree;

	cout << "Deleting Extras" << endl;

	// cleanup
	delete original;
	delete best_tree;
	delete g_overall_best_tree;
	delete greedy;

	cout << "Deleted Extras" << endl;

	out_file.close();
	trees.close();
	analysis.close();

	cout << "Closed Files" << endl;

	return 0;
}
