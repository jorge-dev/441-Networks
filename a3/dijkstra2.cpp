// A C / C++ program for Dijkstra's single source shortest
//https://www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/?ref=rp
// path algorithm. The program is for adjacency matrix
// representation of the graph.
#include <stdio.h>
#include <limits.h>
#include <iostream>
using namespace std;
// Number of vertices in the graph
#define V 4

// A utility function to find the vertex with minimum distance
// value, from the set of vertices not yet included in shortest
// path tree
int minDistance(int dist[], bool sptSet[])
{
	// Initialize min value
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++)
		if (sptSet[v] == false && dist[v] <= min)
			min = dist[v], min_index = v;

	return min_index;
}
char convertIntToABC(int src)
	{
		char letter;
		string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		if (src >= 0 && src < 26)
			letter = alphabet[src];
		return letter;
	}

// Function to print shortest path from source to j
// using parent array
void printPath(int parent[], int j)
{
	// Base Case : If j is source
	if (parent[j] == -1)
		return;

	printPath(parent, parent[j]);

	printf("%d ", j);
}

// A utility function to print the constructed distance
// array
void printSolution(int dist[], int n, int parent[])
{
	int src = 0;
	printf("Vertex\t      Distance\t       Path");
	for (int i = 0; i < V; i++)
	{
		printf("\n%c -> %c \t\t %d\t\t%d ", convertIntToABC(src), convertIntToABC(i), dist[i], src);
		printPath(parent, i);
	}
	printf("\n");
}

// Funtion that implements Dijkstra's single source shortest path
// algorithm for a graph represented using adjacency matrix
// representation
void dijkstra(int graph[V][V], int src, int numEdges)
{
	int dist[V]; // The output array. dist[i] will hold
				 // the shortest distance from src to i

	// sptSet[i] will true if vertex i is included / in shortest
	// path tree or shortest distance from src to i is finalized
	bool sptSet[V];

	// Parent array to store shortest path tree
	int parent[V];

	// Initialize all distances as INFINITE and stpSet[] as false
	for (int i = 0; i < V; i++)
	{
		parent[0] = -1;
		dist[i] = INT_MAX;
		sptSet[i] = false;
	}

	// Distance of source vertex from itself is always 0
	dist[src] = 0;

	// Find shortest path for all vertices
	for (int count = 0; count < V - 1; count++)
	{
		// Pick the minimum distance vertex from the set of
		// vertices not yet processed. u is always equal to src
		// in first iteration.
		int u = minDistance(dist, sptSet);

		// Mark the picked vertex as processed
		sptSet[u] = true;

		// Update dist value of the adjacent vertices of the
		// picked vertex.
		for (int v = 0; v < V; v++)

			// Update dist[v] only if is not in sptSet, there is
			// an edge from u to v, and total weight of path from
			// src to v through u is smaller than current value of
			// dist[v]
			if (!sptSet[v] && graph[u][v] &&
				dist[u] + graph[u][v] < dist[v])
			{
				parent[v] = u;
				dist[v] = dist[u] + graph[u][v];
			}
	}

	// print the constructed distance array
	printSolution(dist, V, parent);
}

// driver program to test above function
int main()
{
	/* Let us create the example graph discussed above */

	int graph2[V][V]={0};

	FILE *file;
	file = fopen("topology2.dat", "r");
	int nEdge = 0;
	char src, dest;
	int delay, capacity;
	while (fscanf(file, "%c %c %d %d\n", &src, &dest, &delay, &capacity) == 4)
	{
		 int row = src - 'A';
		 int col = dest - 'A';
		 //delay = 1;
		graph2[row][col] = delay;
		graph2[col][row] = delay;
	
		nEdge++;
	}
	// cout << "myArray" << endl;
	// for (int i = 0; i < 4; i++)
	// {
	// 	for (int j = 0; j < 4; j++)
	// 	{

	// 		// Prints ' ' if j != n-1 else prints '\n'
	// 		cout << graph2[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }
	//cout << "is is = " << nEdge << endl;

	fclose(file);

	dijkstra(graph2, 0,(nEdge-1));
	cout << "number of edges "<< nEdge<< endl;

	return 0;
}