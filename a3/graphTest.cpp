#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
using namespace std;

// Define maximum number of vertices in the graph
#define N 26
#define MAX_SIZE 1000

// Data structure to store graph
struct Graph
{
    // An array of pointers to Node to represent adjacency list
    struct Node *head[N];
};

// A data structure to store adjacency list nodes of the graph
struct Node
{
    char dest;
    int propDelay;
    int capacity;
    struct Node *next;
};

// data structure to store graph edges
struct Edge
{
    char src, dest;
    int propDelay;
    int capacity;
};

// Function to create an adjacency list from specified edges
struct Graph *createGraph(struct Edge edges[MAX_SIZE], int n)
{
    unsigned i;

    // allocate memory for graph data structure
    struct Graph *graph = (struct Graph *)malloc(sizeof(struct Graph));

    // initialize head pointer for all vertices
    for (i = 0; i < N; i++)
        graph->head[i] = NULL;

    // add edges to the directed graph one by one
    for (i = 0; i < n; i++)
    {
        // get source and destination vertex
        char src = edges[i].src;
        char dest = edges[i].dest;
        int propDelay = edges[i].propDelay;
        int capacity = edges[i].capacity;

        // allocate new node of Adjacency List from src to dest
        struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
        newNode->dest = dest;
        newNode->propDelay = propDelay;
        newNode->capacity = capacity;

        // point new node to current head
        newNode->next = graph->head[src];

        // point head pointer to new node
        graph->head[src] = newNode;

        // allocate new node of Adjacency List from dest to src
        newNode = (struct Node *)malloc(sizeof(struct Node));
        newNode->dest = src;
        newNode->propDelay = propDelay;
        newNode->capacity = capacity;
        // point new node to current head
        newNode->next = graph->head[dest];

        // change head pointer to point to the new node
        graph->head[dest] = newNode;
    }

    return graph;
}

// Function to print adjacency list representation of graph
void printGraph(struct Graph *graph, int numEdges)
{
    char i;
    int count = 1;
    for (i = 'A'; i < 'Z' && count < numEdges; i++)
    {
        // print current vertex and all ts neighbors
        struct Node *ptr = graph->head[i];
        while (ptr != NULL)
        {
            printf("%c -> %c (capacity is %d)(delay is %d)\t", i, ptr->dest, ptr->capacity, ptr->propDelay);
            ptr = ptr->next;
        }
        count++;
        printf("\n");
    }
}

// Weighted Directed Graph Implementation in C
int main(void)
{

    struct Edge edges[MAX_SIZE];

    FILE *file;
    file = fopen("topology2.dat", "r");
    int nEdge = 0;
    char src, dest;
    int delay, capacity;
    while (fscanf(file, "%c %c %d %d\n", &src, &dest, &delay, &capacity) == 4)
    {
        // cout << src << " " << dest << " " << delay << " " << capacity << endl;
        edges[nEdge].src = src;
        edges[nEdge].dest = dest;
        edges[nEdge].propDelay = delay;
        edges[nEdge].capacity = capacity;

        nEdge++;
    }
    //cout << "is is = " << nEdge << endl;

    fclose(file);

    // calculate number of edges

    int numEdges = nEdge;
    // construct graph from given edges
    struct Graph *graph = createGraph(edges, numEdges);

    // print adjacency list representation of graph
    printGraph(graph, numEdges);
    cout << "num of edges: " << numEdges << endl;
    return 0;
}
