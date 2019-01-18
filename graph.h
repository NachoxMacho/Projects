#pragma once
#include <stdio.h>
#include <ctime>
#include <math.h>
#include <cassert>
#include <iostream>
#include "parameters.h"
using namespace std;


#define GRAPH_DEBUG false

#define random_vertex (rand() % GRAPH_VERTICES)

// each vertex tracks which vertices it's connected to, the weight of each corresponding edge, and the number of connected vertices
struct vertex
{
	vertex * connected_vertices[GRAPH_VERTICES]; // vertices connected to this one
	int connected_vertices_weights[GRAPH_VERTICES]; // corresponding weights for vertex connections
	int connected_vertices_count; // number of connections
	int id; // ID number for vertex

	void fillGraph(bool * in_tree);
	~vertex();
	vertex()
	{
		// make the list of connections and weights blank
		for (int i = 0; i < GRAPH_VERTICES; i++)
		{
			connected_vertices[i] = NULL;
			connected_vertices_weights[i] = 0;
		}
		// set number of connections to 0
		connected_vertices_count = 0;
	}
};

void vertex::fillGraph(bool * in_tree)
{
	assert(in_tree != NULL);
	// assert(connected_vertices_count >= 0); // this fails sometimes, looking into
	// set this vertex as found
	if (!in_tree[id])
		in_tree[id] = true;

	for (int i = 0; i < connected_vertices_count; i++)
	{
		// if a vertex has not already been searched
		if (!in_tree[connected_vertices[i]->id] && connected_vertices[i] != NULL)
			connected_vertices[i]->fillGraph(in_tree); // populate that vertex
	}
}

vertex::~vertex()
{
	// set all connections from and to this vertex to NULL
	for (int i = 0; i < GRAPH_VERTICES - 1; i++)
	{
		if (connected_vertices[i] == NULL)
			continue;

		if (GRAPH_DEBUG) cout << "Deleting connection from " << id << " and " << connected_vertices[i]->id;

		for (int j = 0; j < connected_vertices[i]->connected_vertices_count; j++)
		{
			if (connected_vertices[i]->connected_vertices[j] == NULL)
				continue;

			// if we found the matching connection
			if (connected_vertices[i]->connected_vertices[j]->id == id)
				connected_vertices[i]->connected_vertices[j] = NULL;
		}
		connected_vertices[i] = NULL;
		if (GRAPH_DEBUG) cout << " | Done " << endl;
	}
}

class Graph
{
public:

	vertex vertices[GRAPH_VERTICES]; // an aray of vertices for the whole graph

	Graph(bool randomize = false); // constructor to make a random graph by default, call graph(false) for a blank graph
	// Graph();

	// Modify Graph
	void connect(vertex * a, vertex * b, int weight); // connect vertex a to vertex b
	void disconnect(vertex * a, vertex * b); // disconnect vertex a from vertex b
	void copy(Graph *g); // copy g into this graph
	void clear();
	
	// Output
	void print(ostream &out_s); // output the tree vertices & edges

	// Scoring
	int fitness(); // score the tree
	int find_weight(int vertex1, int vertex2); // get the weight between 2 vertices
	
	// Graph Conditions
	bool isGraphConnected(); // check if the graph is fully connected
	bool isInConstraint(); // check vertices for have > MAX_DEGREE connections
	bool isMinimalTree(); // check the size of the tree
};

Graph::Graph(bool randomize)
{
	// initialize the vertices in the graph
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		vertices[i].connected_vertices_count = 0; // has 0 connections
		vertices[i].id = i; // assign id
	}

	// make sure the graph is empty
	assert(!isGraphConnected());

	// double check that after running the for loop, the graph is connected if it should be randomized
	while (!isGraphConnected() && randomize)
	{
		// make sure the graph is essentially blank
		assert(!isGraphConnected());

		for (int i = 0; i < GRAPH_VERTICES-1; i++) // cycle through, forming complete graph by connecting each vertex to all other vertices
		{
			int connections = GRAPH_VERTICES-1-i; // connect to all other vertices not already connected
			vertex * current_vertex = &vertices[i]; // for easier naming/assignment

			// for all connections not made
			for (int j = GRAPH_VERTICES-connections; j < GRAPH_VERTICES; j++)
			{
				vertex * connected_vertex = &vertices[j]; //vertex to connect to
				// connect the vertices together since this is a undirected graph and add weight to the edge
				connect(current_vertex, connected_vertex, rand() % MAX_WEIGHT + 1);
			}
		}
	}
	// make sure that if creation took place, the graph is connected
	assert(isGraphConnected() || !randomize);
}

//Graph::Graph()
//{
//	for (int i = 0; i < GRAPH_VERTICES; i++)
//	{
//		vertices[i].connected_vertices_count = 0; // has 0 connections
//		vertices[i].id = i; // assign id
//	}
//}

void Graph::connect(vertex * a, vertex * b, int weight)
{
	bool a_connected = false, b_connected = false;
	
	// search a's connections for b
	for (int i = 0; i < a->connected_vertices_count; i++)
		if (a->connected_vertices[i] == b)
			a_connected = true;

	// search b's connections for a
	for (int i = 0; i < b->connected_vertices_count; i++)
		if (b->connected_vertices[i] == a)
			b_connected = true;

	// if b is not already in a, add it
	if (!a_connected)
	{
		a->connected_vertices[a->connected_vertices_count] = b;
		a->connected_vertices_weights[a->connected_vertices_count++] = weight;
	}

	// if a is not already in b, add it
	if (!b_connected)
	{
		b->connected_vertices[b->connected_vertices_count] = a;
		b->connected_vertices_weights[b->connected_vertices_count++] = weight;
	}
}

void Graph::disconnect(vertex * a, vertex * b)
{
	// remove b from vertex a
	bool found = false;
	for (int i = 0; i < a->connected_vertices_count; i++)
	{
		// found the vertex
		if (a->connected_vertices[i] == b)
			found = true;

		// shift the array down after finding the vertex
		if (found)
		{
			a->connected_vertices[i] = a->connected_vertices[i + 1];
			a->connected_vertices_weights[i] = a->connected_vertices_weights[i + 1];
		}
	}

	// if found, decrease the number of connections
	if (found)
		a->connected_vertices[a->connected_vertices_count--] = NULL;

	// remove a from b if it is there
	found = false;
	for (int i = 0; i < b->connected_vertices_count; i++)
	{
		// found the vertex
		if (b->connected_vertices[i] == a)
			found = true;

		// shift the array down after finding the vertex
		if (found)
		{
			b->connected_vertices[i] = b->connected_vertices[i + 1];
			b->connected_vertices_weights[i] = b->connected_vertices_weights[i + 1];
		}
	}

	// if found, decrease the number of connections
	if (found)
		b->connected_vertices[b->connected_vertices_count--] = NULL;
}

bool Graph::isGraphConnected()
{
	// Start with 1 found vertex, all others are out of the tree
	bool found_vertices[GRAPH_VERTICES];
	for (int i = 0; i < GRAPH_VERTICES; i++)
		found_vertices[i] = false;
	int root = rand() % GRAPH_VERTICES;
	found_vertices[root] = true;
	
	// fill in the graph from the vertex's perspective
	vertices[root].fillGraph(found_vertices);

	// Check if any vertex was not found, if so the graph is not connected
	for (int i = 0; i < GRAPH_VERTICES; i++)
		if (!found_vertices[i])
			return false;

	// otherwise all vertices are connected
	return true;
}

bool Graph::isMinimalTree()
{
	// can't be a tree if not connected
	if (!isGraphConnected())
	{
		cout << "Graph is not connected" << endl;
		return false;
	}

	assert(isGraphConnected());

	// count the number of connections in the graph
	int totalConnections = 0;
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		totalConnections += vertices[i].connected_vertices_count - 1;
	}
	
	// verify that it is the correct number
	if(GRAPH_DEBUG) cout << "Connections: " << totalConnections << " | Vertices: " << GRAPH_VERTICES << endl;
	if (totalConnections == GRAPH_VERTICES - 2)
		return true;

	return false;
}

int Graph::fitness()
{
	// if the graph is not connected, return -1
	if (!isGraphConnected())
	{
		if(GRAPH_DEBUG) cout << "Failed in fitness" << endl;
		return -1;
	}
	
	// make sure the graph is connected
	assert(isGraphConnected());
	
	// initialize a blank list of vertices that have been found
	int total = 0;
	bool in_tree[GRAPH_VERTICES];
	for (int i = 0; i < GRAPH_VERTICES; i++)
		in_tree[i] = false;

	// pick a random start
	in_tree[rand() % GRAPH_VERTICES] = true;

	// add up each edge in the graph
	for (int k = 0; k < GRAPH_VERTICES; k++)
	{
		for (int i = 0; i < GRAPH_VERTICES; i++)
		{
			for (int j = 0; j < vertices[i].connected_vertices_count; j++)
			{
				if (in_tree[i] && !in_tree[vertices[i].connected_vertices[j]->id])
				{
					total += vertices[i].connected_vertices_weights[j];
					in_tree[vertices[i].connected_vertices[j]->id] = true;
				}
			}
		}
	}
	
	// return the fitness
	return total;
}

void Graph::print(ostream &out_s)
{
	// output the connections and weights for each vertex as a tuple (connection, weight)
	for (int m = 0; m < GRAPH_VERTICES; m++)
	{
		out_s << "Vertex " << m << ": ";
		for (int n = 0; n < vertices[m].connected_vertices_count; n++)
			out_s << "( " << vertices[m].connected_vertices[n]->id << ", " << vertices[m].connected_vertices_weights[n] << ") ";
		out_s << endl;
	}

	out_s << "Fitness: " << this->fitness() << endl;
}

void Graph::copy(Graph * g)
{
	// make sure the graph we are copying is a tree
	// assert(g->isGraphConnected());
	clear(); // clear this 

	bool done[GRAPH_VERTICES], done2[GRAPH_VERTICES];
	for (int i = 0; i < GRAPH_VERTICES; i++)
		done[i] = done2[i] = false;
	done[0] = true;

	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		for (int j = 0; j < g->vertices[i].connected_vertices_count; j++)
		{
			connect(&vertices[i], &vertices[g->vertices[i].connected_vertices[j]->id], g->vertices[i].connected_vertices_weights[j]);
		}
	}

	// assert(isGraphConnected());
	return;
}

void Graph::clear()
{
	for (int i = 0; i < GRAPH_VERTICES; i++)
	{
		while (vertices[i].connected_vertices_count > 0)
		{
			vertices[i].connected_vertices[--vertices[i].connected_vertices_count] = NULL;
			vertices[i].connected_vertices_weights[vertices[i].connected_vertices_count] = 0;
		}
	}
}

bool Graph::isInConstraint()
{
	for (int i = 0; i < GRAPH_VERTICES; i++)
		if (vertices[i].connected_vertices_count > MAX_DEGREE)
			return false;

	return true;
}

int Graph::find_weight(int vertex1, int vertex2)
{
	// search for the edge between both vertices
	// Only need to check one direction because edges are added to both vertices
	for (int i = 0; i < vertices[vertex1].connected_vertices_count; i++)
	{
		// Skip if connection no longer exists
		if (vertices[vertex1].connected_vertices[i] == NULL)
			continue;

		// return the weight if found
		if (vertices[vertex1].connected_vertices[i]->id == vertex2)
			return vertices[vertex1].connected_vertices_weights[i];
	}

	// weight not found
	return -1;
}

