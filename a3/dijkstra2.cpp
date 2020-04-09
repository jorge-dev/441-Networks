//found the dijkstras algorithm isthis page but made several modifications to it
//https://www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/?ref=rp

//Jorge available
// 10123968
//Asg3

#include <stdio.h>
#include <limits.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
// Max Number of nodes in the graph
#define N 26
#define MAX_EVENTS 15000
#define START_EVENT 'S'
#define END_EVENT 'E'

int propdelay[N][N];
int pathCapacity[N][N];
int available[N][N];
int cost[N][N];

//store info about the paths
struct InfoPaths
{
	vector<string> pathsTaken;
	long int numOfHops=0;
	int totalPropDelay = 0;
};
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
void getShortPath(int parent[], int j, string &paths);
int storeShortPath(int dist[], int n, int parent[], int src, int end, InfoPaths &pathInfo);
int RouteCall(char src, char dst);
int ReleasedCall(char src, char dst);

// driver program to test above function
int main()
{
	int dist[N];
	int parent[N];
	int numEvents = 0;
	InfoPaths pathInfo;

	//Graphs for different algorithms
	vector<vector<int>> graphSHPF(N, vector<int>(N, 0));
	vector<vector<int>> graphSDPF(N, vector<int>(N, 0));
	vector<vector<int>> graphLLP(N, vector<int>(N, 0));
	vector<vector<int>> graphMFC(N, vector<int>(N, 0));

	//open Topology
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
	while (fscanf(file2, "%f %c %c %f\n", &arriveTime, &src2, &dst2, &durationTime) == 4)
	{
		EventList[j].event_time = arriveTime;
		EventList[j].event_type = 'S';
		EventList[j].callid = j;
		EventList[j].source = src2;
		EventList[j].destination = dst2;
		EventList[j].duration = arriveTime + durationTime;
		numEvents++;
		j++;
	}
	fclose(file2);

	//Main routing loop
	int successCall = 0, blockedCall = 0;
	for (int i = 0; i < numEvents; i++)
	{
		if (EventList[i].event_type == START_EVENT)
		{
			int start = EventList[i].source - 'A';
			int end = EventList[i].destination - 'A';
			dijkstra(graphSHPF, start, numNodes, dist, parent);
			if ( storeShortPath(dist, numNodes, parent, start, end, pathInfo)!= -1)
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

	

	cout << "Number of nodes: " << numNodes << endl;
	cout << "Total number of calls: " << numEvents << endl;
	printf("Success Calls = %d\nBlocked Calls = %d\nTotal number of hops = %ld\n",successCall,blockedCall,pathInfo.numOfHops);
	printf("Total propagation Delay = %.4f sec\n", (float(pathInfo.totalPropDelay)/float(1000)));
	printf("percentage of hops = %.4f %%\n",(float(pathInfo.numOfHops)/float(successCall)));
	
	//clear the vectors when exiting
	graphSHPF.clear();
	graphSDPF.clear();
	graphLLP.clear();
	graphMFC.clear();

	return 0;
}

//=======================================================================
//							Functions
//=======================================================================
//couldnt implement it 
int RouteCall(char src, char dst)
{
	return 1;
}
//couldnt implement it
int ReleasedCall(char src, char dst)
{
	return 1;
}
// A utility function to find the vertex with minimum distance
// value, from the set of vertices not yet included in shortest
// path tree
int minDistance(int dist[], bool sptSet[])
{
	// Initialize min value
	int min = INT_MAX, min_index;

	for (int v = 0; v < N; v++)
		if (sptSet[v] == false && dist[v] <= min)
			min = dist[v], min_index = v;

	return min_index;
}
//Converts integers to a specified alphabet letter starting from 0 to 26
char convertIntToABC(int src)
{
	char letter;
	string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (src >= 0 && src < 26)
		letter = alphabet[src];
	return letter;
}

// Function to get the shortest path from source to j
// using parent array
void getShortPath(int parent[], int j, string &paths)
{
	char node;
	// Base Case : If j is source
	if (parent[j] == -1)
		return;
	

	getShortPath(parent, parent[j], paths);


	node = convertIntToABC(j);
	paths += node;

}

// A utility function to store the shortest path into a struct and also decide which path
//paths have already been visited or are active
int storeShortPath(int dist[], int n, int parent[], int src, int end, InfoPaths &pathInfo)
{
	string path(1, convertIntToABC(src));
	
	char source = convertIntToABC(src);
	
	getShortPath(parent, end, path);
	
	//check if call is active or not
	if (find(pathInfo.pathsTaken.begin(), pathInfo.pathsTaken.end(), path) != pathInfo.pathsTaken.end())
		return - 1;
	
	else
	{	
		// add call to struct and thus make it busy
		pathInfo.pathsTaken.push_back(path);
		pathInfo.numOfHops += path.length();
		pathInfo.totalPropDelay += propdelay[src][end];
		return 1;
		
	}

	
}

// Funtion that implements Dijkstra's single source shortest path
// algorithm for a graph represented using adjacency matrix
// representation
void dijkstra(vector<vector<int>> graph, int src, int numEdges, int dist[], int parent[])
{

	// sptSet[i] will true if vertex i is included / in shortest
	// path tree or shortest distance from src to i is finalized
	bool sptSet[numEdges];

	
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