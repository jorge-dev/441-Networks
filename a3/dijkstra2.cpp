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
#define V 26
#define MAX_EVENTS 15000
#define START_EVENT 'S'
#define END_EVENT 'E'

int propdelay[V][V];
int pathCapacity[V][V];
int available[V][V];
int cost[V][V];

struct InfoPaths
{
	vector<string> pathsTaken;
	long int numOfHops;
} pathInfo;
struct Event
{
	float event_time;
	char event_type;
	int callid;
	char source;
	char destination;
	float duration;
} EventList[MAX_EVENTS];

//Function prototypes
void dijkstra(vector<vector<int>> graph, int src, int numEdges, int dist[], int parent[]);
int minDistance(int dist[], bool sptSet[]);
char convertIntToABC(int src);
void printPath(int parent[], int j, string &paths);
void printSolution(int dist[], int n, int parent[], int src, int end, InfoPaths &pathInfo);
int RouteCall(char src, char dst);
int ReleasedCall(char src, char dst);

// driver program to test above function
int main()
{
	int dist[V];
	int parent[V];
	int numEvents = 0;

	/* Let us create the example graph discussed above */

	vector<vector<int>> graphSHPF(V, vector<int>(V, 0));
	vector<vector<int>> graphSDPF(V, vector<int>(V, 0));
	vector<vector<int>> graphLLP(V, vector<int>(V, 0));
	vector<vector<int>> graphMFC(V, vector<int>(V, 0));

	FILE *file;
	file = fopen("topology.dat", "r");
	int numNodes = 1;
	char src, dest;
	int delay, capacity;
	char lastNode = 'A';
	int i = 0;
	while (fscanf(file, "%c %c %d %d\n", &src, &dest, &delay, &capacity) == 4)
	{

		if (dest > lastNode)
		{
			lastNode = dest;
			numNodes++;
		}

		int row = src - 'A';
		int col = dest - 'A';

		propdelay[row][col] = delay;
		propdelay[col][row] = delay;
		pathCapacity[row][col] = capacity;
		pathCapacity[col][row] = capacity;
		available[row][col] = capacity;
		available[col][row] = capacity;

		graphSHPF[row][col] = 1;
		graphSHPF[col][row] = 1;

		graphSDPF[row][col] = delay;
		graphSDPF[col][row] = delay;

		graphLLP[row][col] = capacity;
		graphLLP[col][row] = capacity;

		graphMFC[row][col] = capacity;
		graphMFC[col][row] = capacity;
		i++;
	}

	fclose(file);

	/* Next read in the calls from "callworkload.dat" and set up events */
	FILE *file2 = fopen("callworkload.dat", "r");
	int j = 0;

	char src2, dst2;
	float arriveTime, durationTime;
	while ((j = fscanf(file2, "%f %c %c %f\n", &arriveTime, &src2, &dst2, &durationTime)) == 4)
	{
		EventList[j].event_time = arriveTime;
		EventList[j].event_type = 'S';
		EventList[j].callid = j;
		EventList[j].source = src2;
		EventList[j].destination = dst2;
		EventList[j].duration = arriveTime + durationTime;
		numEvents++;
	}
	fclose(file2);

	int successCall = 0, blockedCall = 0;
	for (int i = 0; i < numEvents; i++)
	{
		if (EventList[i].event_type == START_EVENT)
		{
			if (RouteCall(EventList[i].source, EventList[i].destination) != -1)
			{
				successCall++;
			}
			else
			{
				blockedCall++;
			}
		}
		else
		{
			ReleasedCall(EventList[i].source, EventList[i].destination);
		}
	}

	char input1, input2;
	int start, end;
	cout << "enter a starting point (A,B,C,D): ";
	cin >> input1;
	cout << "enter an end point (A,B,C,D): ";
	cin >> input2;
	start = input1 - 'A';
	end = input2 - 'A';

	dijkstra(graphSHPF, start, numNodes, dist, parent);
	printSolution(dist, numNodes, parent, start, end, pathInfo);

	cout << "number of nodes " << numNodes << endl;
	cout << "number of events " << numEvents << endl;
	// cout << "The number of hops from "<< pathInfo.pathsTaken[0].at(0) << " to "<< pathInfo.pathsTaken[0].at(pathInfo.pathsTaken[0].length()-1)<<" is "<<
	// 		pathInfo.numOfHops << " for this path "<< pathInfo.pathsTaken[0]<<endl;
	//cout << "vecotr has this info:\n";

	graphSHPF.clear();
	graphSDPF.clear();
	graphLLP.clear();
	graphMFC.clear();

	return 0;
}

//=======================================================================
//							Functions
//=======================================================================

int RouteCall(char src,char dst){
	return 1;
}

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
void printSolution(int dist[], int n, int parent[], int src, int end, InfoPaths &pathInfo)
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
	if (find(pathInfo.pathsTaken.begin(), pathInfo.pathsTaken.end(), path) != pathInfo.pathsTaken.end())
	{
		printf("path is already taken\n");
	}
	else
	{
		pathInfo.pathsTaken.push_back(path);
		pathInfo.numOfHops = path.length();
		//printf("the number oh hops was %lu", path.length());
	}

	//}
	printf("\n");
}

// Funtion that implements Dijkstra's single source shortest path
// algorithm for a graph represented using adjacency matrix
// representation
void dijkstra(vector<vector<int>> graph, int src, int numEdges, int dist[], int parent[])
{

	// int dist[V]; // The output array. dist[i] will hold
	// the shortest distance from src to i

	// sptSet[i] will true if vertex i is included / in shortest
	// path tree or shortest distance from src to i is finalized
	bool sptSet[numEdges];

	// Parent array to store shortest path tree
	// int parent[V];

	// Initialize all distances as INFINITE and stpSet[] as false
	for (int i = 0; i < numEdges; i++)
	{
		parent[src] = -1;
		dist[i] = INT_MAX;
		sptSet[i] = false;
	}

	// Distance of source vertex from itself is always 0
	dist[src] = 0;

	// Find shortest path for all vertices
	for (int count = 0; count < numEdges - 1; count++)
	{
		// Pick the minimum distance vertex from the set of
		// vertices not yet processed. u is always equal to src
		// in first iteration.
		int u = minDistance(dist, sptSet);

		// Mark the picked vertex as processed
		sptSet[u] = true;

		// Update dist value of the adjacent vertices of the
		// picked vertex.
		for (int v = 0; v < numEdges; v++)
		{

			// Update dist[v] only if is not in sptSet, there is
			// an edge from u to v, and total weight of path from
			// src to v through u is smaller than current value of
			// dist[v]
			if (!sptSet[v] && graph[u][v] &&
				(dist[u] + graph[u][v]) < dist[v])
			{

				parent[v] = u;
				dist[v] = dist[u] + graph[u][v];
			}
		}
	}
}