// A C / C++ program for Dijkstra's single source shortest
//https://www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/?ref=rp
// path algorithm. The program is for adjacency matrix
// representation of the graph.
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
// Number of vertices in the graph
#define V 4

// struct pathsTaken{
// 	char src;
// 	int hops,propDelay,cost;
// 	vector<char>nodes;
// };

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
void printPath(int parent[], int j, string &paths)
{
	char node;
	// Base Case : If j is source
	if (parent[j] == -1)
	{
		return;
	}

	printPath(parent, parent[j], paths);

	printf("%c ", convertIntToABC(j));
	node = convertIntToABC(j);
	paths += node;
	//return paths;
}

// A utility function to print the constructed distance
// array
void printSolution(int dist[], int n, int parent[], int src, int end, vector<string> &pathsTaken)
{
	string path(1, convertIntToABC(src));
	//int src = 0;
	char source = convertIntToABC(src);
	printf("Vertex\t      Distance\t       Path");
	//for (int i = 0; i < V; i++)
	//{
	printf("\n%c -> %c \t\t %d\t\t%c ", convertIntToABC(src), convertIntToABC(end), dist[end], source);
	printPath(parent, end, path);
	cout << "\nthe path is " << path << endl;
	if (find(pathsTaken.begin(), pathsTaken.end(), path) != pathsTaken.end())
	{
		printf("path is already taken\n");
	}
	else
	{
		pathsTaken.push_back(path);
	}

	//}
	printf("\n");
}

// Funtion that implements Dijkstra's single source shortest path
// algorithm for a graph represented using adjacency matrix
// representation
void dijkstra(int graph[V][V], int src, int numEdges, int dist[], int parent[])
{
	// int dist[V]; // The output array. dist[i] will hold
	// the shortest distance from src to i

	// sptSet[i] will true if vertex i is included / in shortest
	// path tree or shortest distance from src to i is finalized
	bool sptSet[V];

	// Parent array to store shortest path tree
	// int parent[V];

	// Initialize all distances as INFINITE and stpSet[] as false
	for (int i = 0; i < V; i++)
	{
		parent[src] = -1;
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


}

// driver program to test above function
int main()
{
	int dist[V];
	int parent[V];
	vector<string> pathsTaken;
	/* Let us create the example graph discussed above */

	int graphSHPF[V][V] = {0};
	int graphSDPF[V][V] = {0};
	int graphLLP[V][V] = {0};
	int graphMFC[V][V] = {0};

	FILE *file;
	file = fopen("topology2.dat", "r");
	int nEdge = 0;
	char src, dest;
	int delay, capacity;
	while (fscanf(file, "%c %c %d %d\n", &src, &dest, &delay, &capacity) == 4)
	{
		char lastNode = 'A';

		int row = src - 'A';
		int col = dest - 'A';

		graphSHPF[row][col] = 1;
		graphSHPF[col][row] = 1;

		graphSDPF[row][col] = delay;
		graphSDPF[col][row] = delay;

		graphLLP[row][col] = capacity;
		graphLLP[col][row] = capacity;

		graphMFC[row][col] = capacity;
		graphMFC[col][row] = capacity;

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
	char input1, input2;
	int start, end;
	cout << "enter a starting point (A,B,C,D): ";
	cin >> input1;
	cout << "enter an end point (A,B,C,D): ";
	cin >> input2;
	start = input1 - 'A';
	end = input2 - 'A';
	while (input1 != '0' || input2 != '0')
	{
		dijkstra(graphSHPF, start, V, dist, parent);
		printSolution(dist, V, parent, start, end, pathsTaken);
		cout << "enter a starting point (A,B,C,D): ";
		cin >> input1;
		cout << "enter an end point (A,B,C,D): ";
		cin >> input2;
		start = input1 - 'A';
		end = input2 - 'A';
	}

	cout << "number of edges " << nEdge << endl;
	cout << "vecotr has this info:\n";
	for (int i = 0; i < pathsTaken.size(); i++)
	{
		cout << i << " " << pathsTaken[i] << endl;
	}

	return 0;
}