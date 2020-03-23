/*
AUTHOR: Naweedullah Anwari
Created for CPSC 441 assignment 3
NOTE: one bonus has been implemented SHPO, the second bonus could be implemented by
simply combining LLC and SHPF but wasn't due to time constraints
  This code implements five routing algorithms from simulated events read in from
  two files, a topology file and a workload file.

  The code starts by reading in the file and generating a linked list of events
  from the workload file. The workload file is expected to be ordered
  Once this linked list is generated it is iterated through
  until the end is reached. Whenever a call is not dropped, a corresponding
  end event is added (in an ordered fashion) to free up resources that were used.
  Whenever a start event is reached, the call is routed based on the current algorithm.

  There are five algorithms being implemeted SHPF SDPF LLC MFC SHPO.
  SHPF SDPF and SHPO use the same algorithm but calculate the cost differently (set 1).
  LLC and MFC use the same algoirthm but calculate the cost different (set 2).

  set 1 calculates the corresponding cost, then using dijkstras to find the min
  cost (cumulative)from the source to the destination based on the cost. Then a
  pathing algorithm is used to find a path that has the same cost as what djikstras found
  (for SHPO this could be not possible in which case the call is blocked)
  then the path is returned with all edges (could be no path)
  and the corresponding calculations are performed

  set 2 calculates the corresponding cost, and then calculates the cost of each
  edge of the topology into a list. That list is then ordered from the lowest cost being at
  the first index. Then a pathing algorithm attempts to find the path with only edges
  that have a lower cost then the first element of the list, if this is impossible
  then it tries to find a path with only edges that have a lower cost than the
  second element of the list. This is continued until the the last element of the list
  is reached. It must be noted that the pathing algorithm often takes a longer path beacuse
  it does not consider any other costs like number of hops etc.
  Then the path is returned (could be no path) and the corresponding
  calculations are performed.
*/
#include <iostream>
#include <stack>

#define START_EVENT 0
#define END_EVENT 1
// max is number of letters in the alphabet
#define MAX_ROW 26
#define MAX_COL 26
#define MAX_EVENTS 10000
#define NUMBER_OF_ALGORITHMS 5
// this is considered a value that is treated as never being reached for cost
// number of edges, or number of nodes
#define MAX_VALUE 1000000
#define SHPF 0
#define SDPF 1
#define LLP 2
#define MFC 3
#define SHPO 4

#define PERCENT_CHARACTER 37
// These can be changed if a different file is desired
#define TOPOLOGY_FILE_NAME "topology.dat"
#define WORKLOAD_FILE_NAME "callworkload.dat"

// an edge between nodes
class Edge{
public:
  int hop1;
  int hop2;
  float cost;
};

// a series of edges
class Path{
public:
  Edge path[MAX_ROW];
  int availableIndex = 0;
  int cost = 0;
};

/* Event record */
class Event
{
public:
  Event(float start, char src, char dst, float dura, int type);

  float event_time;
  int event_type;
  int callid;
  char source;
  char destination;
  float duration;
  // only has a value for end events
  Path* pathTaken;
};

// a node is used to implement the linked list structure
// end events are added inbetween elements depending on their start time
class Node{
public:
  Node(float start, char src, char dst, float dura, int type);

  Node *nextNode;
  Event data;
};

// ---------------------------- FUNCTIONS ------------------------------------------------
// These functions are used by SHPF SDPF SHPO

/*
This function does a start event using dijkstras algorithm. dijkstras
algorithm find the minimum distance and find corresponding path looks for paths
that meets the required distance. update values is called if a path is found
COST CALCULATION:
  SHPF: cost is 1 if edge is available, else is 0
  SDPF: cost is propagation delay of the edge, if the edge is available, else is 0
  SHPO: cost is 1 if the edge has a capacity > 0, else is 0
*/
void doStartEvent_usingDijkstras(Event event, int currentAlgorithm, Node *headM);
/*
initializes some value and starts from the first possible edge from src
then calls tryNodeRecursive for each edge from src (unless a path was found to
dst that has a cost equal to the minimized cost specified by dijkstras)
returns NULL if the correponding path was not found (only possible with SHPO)
*/
Path* findCorrespondingPath(int dest[], int src, int dst);
/*
check if traveling to this node exceeds the cost (in which case false is returned)
checks if cost is met in which case if the dst node is reached true is returned
if this function ever returns true the path has been found and all calculations end
otherwise travels to next node that has not already been visted in this path (recursively)
*/
bool tryNodeRecursive(int distance[], int src, int current, int dst, Path *path, int alreadyVisted[]);

// helper function of dijkstras returns the next min distance in the includedArray
int minDistance(int dist[], bool includedArray[]);
// dijkstras algorithm, returns the minimum distance possible in the dist arguement
// pass by reference
void dijkstra(int graph_cost[MAX_ROW][MAX_COL], int src, int dist[MAX_ROW]);


// these functions are used by all algorithms

// free up resources
void doEndEvent(Event event);
// mark resources as being used (if call was allowed)
// add end event in an ordered fashion
void updateValues(Path *path, Node *headM, Event event);


//These functions are used by LLC and MFC

/*
does a start event by first created an ordered list of edges that correpond to the
cost specified by the routing algorithm. insertionSort is used to sort the list.
then finds path with only the first edge of the ordered list allowed, then if a path
could not be found that way, the second edge is included, etc.
NOTE: the first path is returned that is found, and that path is not optimized in any other way
COST CALCULATION:
  LLC: if availability != 0, cost is capacity - availability of the edge, else is -1
   NOTE: later actual utilization is calaculated by dividing by capacity
   (since cost is an int array this is done later because the result would be a float)
  MFC: if availability != 0, cost is MAX_VALUE - availability, else is -1
   NOTE: MFC does MAX_VALUE - availability because algorithms work based on
   the lower a cost is the better it is seen. This way the higher avaliability something
   has, the lower cost it will have, allowing algorithms to remain unchanged
*/
void doStartEvent_minimizeCost(Event event, int currentAlgorithm, Node *headM);
// attemps to find a path similarily to find corresponding path but is not a cumulative cost
Path* findPathWithOnlyTheseEdges(float highestCost, int src, int dst);
// this function is similar to findNodeRecursive except if an edge has a cost
// greater than highestCost, false is returned right away and that edge is not taken
bool checkEdgeRecursive(float highestCost, int src, int currentNode, int dst, Path *path, int alreadyVisted[]);
// puts all edges into a list and stores the number of edges, then sorts the list
// by calling insertionSort
void findMostLoadedEdges(int currentAlgorithm);
// orders the list created by findMostLoadedEdges
void insertionSort(Edge arr[], int n);

/* Global matrix data structure for network topology/state information */
int propdelay[MAX_ROW][MAX_COL];
int capacity[MAX_ROW][MAX_COL];
int available[MAX_ROW][MAX_COL];
int cost[MAX_ROW][MAX_COL];

// used by LLC and MFC to store the ordered list of edges
//
Edge mostCostlyEdges[MAX_VALUE];

int numEdges;
static int classCounter = 0;
int numBlocked = 0;
int numAllowed = 0;
int totalDelay = 0;
int totalHops = 0;
int totalCalls = 0;

Event::Event(float start, char src, char dst, float dura, int type)
{
  callid = classCounter++;
  source = src;
  destination = dst;
  duration = dura;
  event_type = type;
  event_time = start;
}

Node::Node(float start, char src, char dst, float dura, int type):data(start,src,dst,dura,type){
  this->nextNode = NULL;
}


// *****************************************************
// *****************************************************************
// These functions are used by SHPF SDPF SHPO

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
int minDistance(int dist[], bool includedArray[])
{
    // Initialize min value
    int min = MAX_VALUE, min_index;

    for (int v = 0; v < MAX_ROW; v++)
        if (includedArray[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}

// returns a array of the shortest path of paths that currently have circuits
// available
void dijkstra(int graph_cost[MAX_ROW][ MAX_COL], int src, int dist[MAX_ROW])
{
  // for(int i = 0; i < MAX_ROW; i++)
  // {
  //   for(int j = 0; j < MAX_ROW; j++){
  //     printf("%d-%d ", graph_cost[i][j], available[i][j]);
  //   }
  //   printf("\n");
  // }
  bool includedArray[MAX_ROW]; // includedArray[i] will be true if vertex i is included in shortest
  // path tree or shortest distance from src to i is finalized

  // Initialize all distances as INFINITE and stpSet[] as false
  for (int i = 0; i < MAX_ROW; i++){
    dist[i] = MAX_VALUE;
    includedArray[i] = false;
  }

  // Distance of source vertex from itself is always 0
  dist[src] = 0;
  // Find shortest path for all vertices
  for (int count = 0; count < MAX_ROW - 1; count++)
  {
    // Pick the minimum distance vertex from the set of vertices not
    // yet processed. u is always equal to src in the first iteration.
    int startVertex = minDistance(dist, includedArray);

    // Mark the picked vertex as processed
    includedArray[startVertex] = true;

    // Update dist value of the adjacent vertices of the picked vertex.
    for (int compareVertex = 0; compareVertex < MAX_ROW; compareVertex++)
    {
      // Update dist[v] only if is not in includedArray, there is an edge from
      // u to v, and total weight of path from src to  v through u is
      // smaller than current value of dist[v]
      if (!includedArray[compareVertex] && graph_cost[startVertex][compareVertex] && dist[startVertex] != MAX_VALUE
        && dist[startVertex] + graph_cost[startVertex][compareVertex] < dist[compareVertex])
        {
          dist[compareVertex] = dist[startVertex] + graph_cost[startVertex][compareVertex];
        }
    }
  }
}

Path* findCorrespondingPath(int distance[], int src, int dst)
{
  // there is no available path to this node, block this call
  if(distance[dst] == MAX_VALUE){
    return NULL;
  }

  // an array indicating if the node has already been visted in the path
  int alreadyVisted[MAX_ROW];

  // construct a new already visted array for each new path;
  for(int i = 0; i < MAX_ROW; i++){
    alreadyVisted[i] = 0;
  }

  // src is considered already visted
  alreadyVisted[src] = 1;

  // the path traversed
  Path *path = new Path();

  for(int currentNode = 0; currentNode < MAX_ROW; currentNode++)
  {
    if(available[src][currentNode] != 0 && currentNode != src)
    {
      path = new Path();
      if(tryNodeRecursive(distance, src, currentNode, dst, path, alreadyVisted)){
        return path;
      }
    }
  }

  return NULL;
}

bool tryNodeRecursive(int distance[], int src, int currentNode, int dst, Path *path, int alreadyVisted[])
{
  // if the cost now exceeds the expeceted cost (distance[dst]) then return
  // false, traveling to this node is not the valid path
  if(path -> cost + cost[src][currentNode] > distance[dst]){
      return false;
  }

  // if the cost is equal to expected cost but dst has not been reacehd return false
  if(path -> cost + cost[src][currentNode] == distance[dst] && currentNode != dst)
  {
    return false;
  }

  // add this node to the path (and it's cost)
  path->cost += cost[src][currentNode];
  Edge edge;
  edge.hop1 = src;
  edge.hop2 = currentNode;
  path -> path[path->availableIndex++] = edge;
  // mark this node as already visted
  alreadyVisted[currentNode] = 1;

  // store the available index and cost in case this path is proved invalid later
  int myAvailableIndex = path->availableIndex;
  int myCost = path -> cost;

  // if the current node is the destination node, you can return this path,
  // this path is the valid path to the destination node from the starting node
  if(currentNode == dst && path -> cost <= distance[dst])
  {
    return true;
  }

  for(int nextNode = 0; nextNode < MAX_ROW; nextNode++)
  {
    // if there is an available path
    // and the node has not already been visted in this path
    if(available[currentNode][nextNode] != 0 && alreadyVisted[nextNode] != 1)
    {
      if(tryNodeRecursive(distance, currentNode, nextNode, dst, path, alreadyVisted))
      {
        return true;
      }

      // otherwise false was returned, and traveling to that node was
      // not the valid path, thus ignore all path changes made by the
      // recursive search
      path -> availableIndex = myAvailableIndex;
      path -> cost = myCost;
    }
  }

  // mark as not already visted (alreadyVisted marks parent nodes that have
  // already been visted)
  alreadyVisted[currentNode] = 0;
  return false;
}

void doStartEvent_usingDijkstras(Event event, int currentAlgorithm, Node *headM)
{
  // select what the cost is considered to be based on which algorithm is
  // currently being implemented
  Path *path;

  switch(currentAlgorithm)
  {
    case SHPF:
      for(int row = 0; row < MAX_ROW; row++)
      {
        for(int col = 0; col < MAX_COL; col++)
        {
          if(available[row][col] != 0){
            // each hop traveled is considered to have a cost of one
            cost[row][col] = 1;
            cost[col][row] = 1;
          }else{
            // as in there is no path from that node to the other node
            cost[row][col] = 0;
            cost[col][row] = 0;
          }
        }
      }
      break;

    case SDPF:
      for(int row = 0; row < MAX_ROW; row++)
      {
        for(int col = 0; col < MAX_COL; col++)
        {
          if(available[row][col] != 0){
            // each hop traveled is considered to have a cost of one
            cost[row][col] = propdelay[row][col];
          }else{
            // as in there is no path from that node to the other node
            cost[row][col] = 0;
          }
        }
      }
      break;

    case SHPO:
      for(int row = 0; row < MAX_ROW; row++)
      {
        for(int col = 0; col < MAX_COL; col++)
        {
          if(capacity[row][col] != 0){
            // each hop traveled is considered to have a cost of one
            cost[row][col] = 1;
            cost[col][row] = 1;
          }else{
            // as in there is no path from that node to the other node
            cost[row][col] = 0;
            cost[col][row] = 0;
          }
        }
      }
  }


  int dist[MAX_ROW];
  dijkstra(cost, event.source - 'A', dist);
  int srcI = event.source - 'A';
  int dstI = event.destination - 'A';
  path = findCorrespondingPath(dist, srcI, dstI);

  if(path == NULL){
    numBlocked++;
  }else{
    updateValues(path, headM, event);
  }

}
// END OF FUNCTIONS FOR SHPF SDPF SHPO
// ***********************************************************************
// *****************************************************************

// this function is used by all algorithms
// *****************************************************************
// *****************************************************************
void updateValues(Path *path, Node *headM, Event event)
{
  numAllowed++;
  totalHops += path->availableIndex;
  // decrease availble status for each edge used
  for(int i = 0; i < path->availableIndex; i++)
  {
    Edge edge = path->path[i];
    totalDelay += propdelay[edge.hop1][edge.hop2];
    available[edge.hop1][edge.hop2] -= 1;
    available[edge.hop2][edge.hop1] -= 1;
  }

  Node *currentNode = headM -> nextNode;
  Node *previousNode = headM;
  Node *newNode = new Node(event.event_time + event.duration, event.source, event.destination, event.duration, END_EVENT);
  newNode -> data.callid = event.callid;
  newNode -> data.pathTaken = path;

  while(true)
  {
    if(currentNode == NULL)
    {
      previousNode -> nextNode = newNode;
      break;
    }
    // insert this end event when an event is supposed to happen
    // after this end event
    if(currentNode -> data.event_time > newNode -> data.event_time)
    {
      previousNode -> nextNode = newNode;
      newNode -> nextNode = currentNode;
      break;
    }

    previousNode = previousNode -> nextNode;
    currentNode = currentNode -> nextNode;
  }
}

void doEndEvent(Event event)
{
  Path *path = event.pathTaken;

  // release resources being used by event
  for(int i = 0; i < path -> availableIndex; i++)
  {
    Edge edge = path->path[i];
    available[edge.hop1][edge.hop2] += 1;
    available[edge.hop2][edge.hop1] += 1;
  }
}
// END OF FUNCTIONS USED BY ALL ALGORITHMS
// *****************************************************************
// *****************************************************************


// *****************************************************************
// *****************************************************************
// The following functions are all for do start event using minmax (MFC and LLC)

// this algorithm will try to minimize the number of hops, but prioritizes minimizing
// the path load (busiest link load)
void doStartEvent_minimizeCost(Event event, int currentAlgorithm, Node *headM)
{
  int src = event.source - 'A';
  int dst = event.destination - 'A';
  Path *path;

  switch(currentAlgorithm)
  {
    case LLP:
      for(int i = 0; i < MAX_ROW; i++)
      {
        for(int j = 0; j < MAX_ROW; j++)
        {
          if(available[i][j] != 0)
          {
            cost[i][j] = capacity[i][j] - available[i][j];
          }else{
            cost[i][j] = -1;
          }
        }
      }
      break;
    case MFC:
    for(int i = 0; i < MAX_ROW; i++)
    {
      for(int j = 0; j < MAX_ROW; j++)
      {
        if(available[i][j] != 0)
        {
          // this is done to reduce the amount of code needed, most of the code
          // was written to minimize value, so this way the larger a number is the
          // smaller of a cost it will have, allowing reuse of code
          cost[i][j] = MAX_VALUE - available[i][j];
        }else{
          cost[i][j] = -1;
        }
      }
    }
      break;
  }

  // updates mostCostlyEdges array with all edges ordered by how loaded the edge is
  // (largest at end)
  findMostLoadedEdges(currentAlgorithm);

  Edge *highestCostEdge;

  for(int allowedEdges = 0; allowedEdges < numEdges; allowedEdges++)
  {
    highestCostEdge = &mostCostlyEdges[allowedEdges];
    path = findPathWithOnlyTheseEdges(highestCostEdge->cost, src, dst);
    if(path != NULL){
      break;
    }
  }

  if(path == NULL){
    numBlocked++;
  }else{
    updateValues(path, headM, event);
  }
}

Path* findPathWithOnlyTheseEdges(float highestCost, int src, int dst)
{
  // an array indicating if the node has already been visted in the path
  int alreadyVisted[MAX_ROW];

  for(int i = 0; i < MAX_ROW; i++){
    alreadyVisted[i] = 0;
  }

  // src is considered already visted
  alreadyVisted[src] = 1;

  // the path traversed
  Path *path = new Path();

  for(int currentNode = 0; currentNode < MAX_ROW; currentNode++)
  {
    if(available[src][currentNode] != 0 && currentNode != src)
    {
      path = new Path();
      if(checkEdgeRecursive(highestCost, src, currentNode, dst, path, alreadyVisted)){
        return path;
      }
    }
  }

  return NULL;
}

bool checkEdgeRecursive(float highestCost, int src, int currentNode, int dst, Path *path, int alreadyVisted[])
{
  // if the cost now exceeds the expeceted cost (distance[dst]) then return
  // false, traveling to this node is not the valid path
  if((float)cost[src][currentNode] / (float)capacity[src][currentNode] > highestCost || cost[src][currentNode] == -1){
      return false;
  }

  // if the cost is equal to expected cost but dst has not been reacehd return false
  if((float)cost[src][currentNode] / (float)capacity[src][currentNode] == highestCost && currentNode != dst)
  {
    return false;
  }

  // add this node to the path (and it's cost)
  if(path->cost < (float)cost[src][currentNode] / (float)capacity[src][currentNode])
  {
    path->cost = (float)cost[src][currentNode] / (float)capacity[src][currentNode];
  }

  Edge edge;
  edge.hop1 = src;
  edge.hop2 = currentNode;
  path -> path[path->availableIndex++] = edge;
  // mark this node as already visted
  alreadyVisted[currentNode] = 1;

  // store the available index and cost in case this path is proved invalid later
  int myAvailableIndex = path->availableIndex;
  int myCost = path -> cost;

  // if the current node is the destination node, you can return this path,
  // this path is the valid path to the destination node from the starting node
  if(currentNode == dst && path -> cost <= highestCost)
  {
    return true;
  }

  for(int nextNode = 0; nextNode < MAX_ROW; nextNode++)
  {
    // if there is an available path
    // and the node has not already been visted in this path
    if(available[currentNode][nextNode] != 0 && alreadyVisted[nextNode] != 1)
    {
      if(checkEdgeRecursive(highestCost, currentNode, nextNode, dst, path, alreadyVisted))
      {
        return true;
      }

      // otherwise false was returned, and traveling to that node was
      // not the valid path, thus ignore all path changes made by the
      // recursive search
      path -> availableIndex = myAvailableIndex;
      path -> cost = myCost;
    }
  }

  // mark as not already visted (alreadyVisted marks parent nodes that have
  // already been visted)
  alreadyVisted[currentNode] = 0;
  return false;
}

void findMostLoadedEdges(int currentAlgorithm)
{
  // reset edges list
  numEdges = 0;

  for(int row = 0; row < MAX_ROW; row++)
  {
    for(int col = 0; col < MAX_COL; col++)
    {
      // so no duplicate edges are included
      if(row < col)
      {
        if(available[row][col] != 0 && cost[row][col] != -1)
        {
          // create edge
          Edge *edge = new Edge();
          edge -> hop1 = row;
          edge -> hop2 = col;
          switch(currentAlgorithm)
          {
            case LLP:
              edge -> cost = (float)cost[row][col] / (float) capacity[row][col];
              break;
            case MFC:
              edge -> cost = (float)cost[row][col];
              break;
          }
          // insert edge (sort later)
          mostCostlyEdges[numEdges++] = *edge;
        }
      }
    }
  }

  // sorts edges with most loaded at the end (at numEdges)
  // load is either utilization(LLP) or MAX_VALUE - availablility of the edge(MFC)
  insertionSort(mostCostlyEdges, numEdges);
}

void insertionSort(Edge arr[], int n)
{
  int i, j;
  float key;
  Edge edge;

  for(int i = 0; i < n; i++)
  {
    key = arr[i].cost;
    edge = arr[i];
    j = i - 1;

    while(j >= 0 && arr[j].cost > key)
    {
      arr[j + 1] = arr[j];
      j = j - 1;
    }

    arr[j + 1] = edge;
  }
}
// END OF FUNCTIONS FOR MFC AND LLC
// ********************************************************************************
// *****************************************************************

int main()
{
  int status, row, col;
  char src, dst;
  int delay, cap;
  float startTime, duration;

  // READ TOPOLOGY FILE
  FILE *fp1 = fopen(TOPOLOGY_FILE_NAME, "r");
  while( (status = fscanf(fp1, "%c %c %d %d\n", &src, &dst, &delay, &cap)) == 4 )
  {
    row = src - 'A'; col = dst - 'A';
    propdelay[row][col] = delay; propdelay[col][row] = delay;
    capacity[row][col] = cap; capacity[col][row] = cap;
    available[row][col] = cap; available[col][row] = cap;
  }
  fclose(fp1);

  // redone for each routing algorithm
  // so the same workload is tested
  for(int currentAlgorithm = 0; currentAlgorithm < NUMBER_OF_ALGORITHMS; currentAlgorithm++)
  {
    // Next read in the calls from WORKLOAD_FILE_NAME (defined at top of this file)
    // and set up events

    // headM is the front of the linked list
    Node *headM, *currentNode;
    FILE *workloadFileP = fopen(WORKLOAD_FILE_NAME, "r");
    // read headM first
    status = fscanf(workloadFileP, "%f %c %c %f\n", &startTime, &src, &dst, &duration);
    if(status != 4){printf("Error intializing headM\n"); exit(-1);}

    headM = new Node(startTime, src, dst, duration, START_EVENT);
    currentNode = headM;
    // the file is already ordered so we can just insert into the linked list
    while((status = fscanf(workloadFileP, "%f %c %c %f\n", &startTime, &src, &dst, &duration)) == 4)
    {
      currentNode -> nextNode = new Node(startTime, src, dst, duration, START_EVENT);
      currentNode = currentNode -> nextNode;
    }

    // "callworkload.dat" has now been converted into a linked list

    Event *currentEvent;
    /* Now simulate the call arrivals and departures */
    while(headM != NULL)
    {
      currentEvent = &(headM -> data);
      if(currentEvent->event_type == END_EVENT) // end events free up resources
      {
        doEndEvent(*currentEvent);
      }else if(currentEvent->event_type == START_EVENT){ // start events take up reources
        //reset cost array
        for(int i = 0; i < MAX_ROW; i++)
        {
          for(int j = 0; j < MAX_ROW; j++){
            cost[i][j] = 0;
          }
        }
        // increment total number of calls
        totalCalls++;

        // depending on which routing is being used, call the corresponding algorithm
        if(currentAlgorithm < 2){
          doStartEvent_usingDijkstras(*currentEvent, currentAlgorithm, headM);
        }else if(currentAlgorithm < 4){
          doStartEvent_minimizeCost(*currentEvent, currentAlgorithm, headM);
        }else{
          doStartEvent_usingDijkstras(*currentEvent, currentAlgorithm, headM);
        }
      }else{
        printf("Error, invalid event type requested\n");
        exit(-1);
      }

      headM = headM -> nextNode;
    }

    // print the result for this algorithm
    std::cout << "Policy    Calls    Succ(" << (char)PERCENT_CHARACTER <<")      Block(" << (char)PERCENT_CHARACTER;
    std::cout << ")      Hops    Delay\n";
    std:: cout << "--------------------------------------------------------------\n";
    switch(currentAlgorithm)
    {
      case SHPF:
        std::cout << "SHPF      ";
        break;
      case SDPF:
        std::cout << "SDPF      ";
        break;
      case LLP:
        std::cout << "LLP       ";
        break;
      case MFC:
        std::cout << "MFC       ";
        break;
      case SHPO:
        std::cout << "SHPO      ";
        break;

    }
    float averageHops = (float)totalHops / (float)numAllowed;
    float blockedPercent = (float)numBlocked / (float)totalCalls * 100;
    float allowedPercent = (float)numAllowed / float(totalCalls) * 100;
    float averageDelay = (float)totalDelay / (float)numAllowed;

    std::cout << totalCalls << "      " << numAllowed << "(";
    printf("%.3f", allowedPercent);
    std::cout << (char)PERCENT_CHARACTER << ")  ";

    std::cout <<  numBlocked << "(";
    printf("%.3f", blockedPercent);
    std::cout << (char)PERCENT_CHARACTER << ")  ";
    printf("%.3f", averageHops);
    printf("   %.3f\n\n\n", averageDelay);

    // reset global variables
    numBlocked = 0;
    numAllowed = 0;
    totalDelay = 0;
    totalHops = 0;
    totalCalls = 0;
    // loops back to top and currentAlgorithm is incremented, leading to the
    // next routing algorithm being implemented
  }
}
