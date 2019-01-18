#ifndef GREEDY_H
#define GREEDY_H

#include "graph.h"

class Greedy
{
public:

	Greedy(Graph *original); // constructor
	~Greedy();

	int generations;
	int staleness;
	int bestFitness;
	Graph * best_tree;
	Graph * base;

	void run_greedy(void);
	void find_shortest_edge(void);
	void fitness(void);
};

Greedy::Greedy(Graph *original)
{
	base = new Graph(false);
	base->copy(original);
	best_tree = new Graph(false);
}

Greedy::~Greedy()
{
	delete base;
	delete best_tree;
}

void Greedy::run_greedy(void)
{
	;
}

void Greedy::find_shortest_edge(void) // pass in null to find shortest edge first time
{
	int temp;
	int tree_is_empty = 1;
	int already_connected = 0;
	int edge_already_found;
	int vertices_of_shortest_edges[GRAPH_VERTICES][2];
	int number_of_shortest_edges = 0;
	int shortest_edge_weight = MAX_WEIGHT + 1;
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		if (best_tree->vertices[i].connected_vertices_count != 0)
		{
			tree_is_empty = 0;
		}
	}
	if (tree_is_empty == 1)
	{
		for (int i = 0; i < GRAPH_VERTICES; i++) // for each vertex in the graph
		{
			for (int j = 0; j < base->vertices[i].connected_vertices_count; j++) // check the edge weight to each connected vertex
			{
				if (base->vertices[i].connected_vertices_weights[j] < shortest_edge_weight) // check if weight is less than current shortest edge
				{
					shortest_edge_weight = base->vertices[i].connected_vertices_weights[j];
					vertices_of_shortest_edges[0][0] = base->vertices[i].id;
					vertices_of_shortest_edges[0][1] = base->vertices[i].connected_vertices[j]->id;
					number_of_shortest_edges = 1;
				}
				else if (base->vertices[i].connected_vertices_weights[j] == shortest_edge_weight) // check if weight is equal to the current shortest edge
				{
					edge_already_found = 0;
					for (int k = 0; k < number_of_shortest_edges; k++)
					{
						if ((vertices_of_shortest_edges[k][0] == base->vertices[i].id && vertices_of_shortest_edges[k][1] == base->vertices[i].connected_vertices[j]->id)
							|| (vertices_of_shortest_edges[k][1] == base->vertices[i].id && vertices_of_shortest_edges[k][0] == base->vertices[i].connected_vertices[j]->id))
						{
							edge_already_found = 1;
						}
					}
					if (edge_already_found == 1)
					{
						vertices_of_shortest_edges[number_of_shortest_edges][0] = base->vertices[i].id;
						vertices_of_shortest_edges[number_of_shortest_edges][1] = base->vertices[i].connected_vertices[j]->id;
						number_of_shortest_edges++;
					}
				}
				else
				{
					;
				}
			}
		}
		temp = rand() % number_of_shortest_edges;
		best_tree->connect(&best_tree->vertices[vertices_of_shortest_edges[temp][0]], &best_tree->vertices[vertices_of_shortest_edges[temp][1]], shortest_edge_weight);
		base->disconnect(&base->vertices[vertices_of_shortest_edges[temp][0]], &base->vertices[vertices_of_shortest_edges[temp][1]]);
	}
	else
	{
		for (int i = 0; i < GRAPH_VERTICES; i++) // connect all vertices connected to the current best_tree that only have degree 1
		{
			if (best_tree->vertices[i].connected_vertices_count > 0) // only continue if this vertex is in best_tree already
			{
				for (int j = 0; j < base->vertices[i].connected_vertices_count; j++) // for each connected vertex
				{
					if (base->vertices[i].connected_vertices[j]->connected_vertices_count == 1) // if there are any vertices that are only connected to this vertex
					{
						best_tree->connect(&best_tree->vertices[i], best_tree->vertices[i].connected_vertices[j], best_tree->vertices[i].connected_vertices_weights[j]);
						base->disconnect(&base->vertices[i], base->vertices[i].connected_vertices[j]);
					}
				}
			}
		}

		for (int i = 0; i < GRAPH_VERTICES; i++) // now check all vertices for next shortest edge
		{
			if (best_tree->vertices[i].connected_vertices_count < MAX_DEGREE && best_tree->vertices[i].connected_vertices_count > 0) // only continue if this vertex is on the best_tree and isn't at MAX_DEGREE already
			{
				for (int j = 0; j < base->vertices[i].connected_vertices_count; j++) // check each connected edge against current shortest edge
				{
					already_connected = 0;
					for (int k = 0; k < GRAPH_VERTICES; k++) // for each vertex in best_tree
					{
						// if that vertex is already connected and the id of that vertex is the id of the target connected edge
						if (best_tree->vertices[k].connected_vertices_count > 0 && k == base->vertices[i].connected_vertices[j]->id)
						{
								already_connected = 1;
						}
					}
					
					if (already_connected == 1) // check if the given vertex creates a cycle
					{
						;
					}
					else if (base->vertices[i].connected_vertices_weights[j] < shortest_edge_weight) // check if weight is less than current shortest edge
					{
						shortest_edge_weight = base->vertices[i].connected_vertices_weights[j];
						vertices_of_shortest_edges[0][0] = base->vertices[i].id;
						vertices_of_shortest_edges[0][1] = base->vertices[i].connected_vertices[j]->id;
						number_of_shortest_edges = 1;
					}
					else if (base->vertices[i].connected_vertices_weights[j] == shortest_edge_weight) // check if weight is equal to the current shortest edge
					{
						edge_already_found = 0;
						for (int k = 0; k < number_of_shortest_edges; k++)
						{
							if ((vertices_of_shortest_edges[k][0] == base->vertices[i].id && vertices_of_shortest_edges[k][1] == base->vertices[i].connected_vertices[j]->id)
								|| (vertices_of_shortest_edges[k][1] == base->vertices[i].id && vertices_of_shortest_edges[k][0] == base->vertices[i].connected_vertices[j]->id))
							{
								edge_already_found = 1;
							}
						}
						if (edge_already_found == 0)
						{
							vertices_of_shortest_edges[number_of_shortest_edges][0] = base->vertices[i].id;
							vertices_of_shortest_edges[number_of_shortest_edges][1] = base->vertices[i].connected_vertices[j]->id;
							number_of_shortest_edges++;
						}
					}
					else
					{
						;
					}
				}
			}
		}
		temp = rand() % number_of_shortest_edges;
		best_tree->connect(&best_tree->vertices[vertices_of_shortest_edges[temp][0]], &best_tree->vertices[vertices_of_shortest_edges[temp][1]], shortest_edge_weight);
		base->disconnect(&base->vertices[vertices_of_shortest_edges[temp][0]], &base->vertices[vertices_of_shortest_edges[temp][1]]);
	}
}

void Greedy::fitness(void)
{
	bestFitness= best_tree->fitness();
}
#endif /* GREEDY_H*/
