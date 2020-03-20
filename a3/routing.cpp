/* Here is a skeleton of a possible solution strategy. */
/* You are NOT required to follow it, but it might help you get started. */

/* Global matrix data structure for network topology/state information */
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
using namespace std;

//Testings
#define TOP1 0

#define ROUTE_SUCCESS 'S'
#define ROUTE_BLOCKED 'B'

#define MAX_NODE 26
#define MAX_EVENTS 11000

int propdelay[MAX_NODE];
int capacity[MAX_NODE];
int available[MAX_NODE];
int cost[MAX_NODE];

/* Event record */
struct Event
{
  float event_time;
  char event_type;
  int callid;
  char source;
  char destination;
  float duration;
} EventList[MAX_EVENTS];

int main()
{
  int numevents = 0;
  int numTopology = 0;

#ifndef TOP1
  FILE *fp1 = fopen("topology.dat", "r");
#else
  FILE *fp1 = fopen("topology2.dat", "r");

#endif
  int i, row, col;
  char src, dst;
  unsigned int delay, cap;
  while ((i = fscanf(fp1, "%c %c %d %d\n", &src, &dst, &delay, &cap)) == 4)
  {
    row = src - 'A';
    col = dst - 'A';
    propdelay[row][col] = delay;
    //propdelay[col][row] = delay;
    capacity[row][col] = cap;
    //capacity[col][row] = cap;
    //available[row][col] = cap;
    available[col][row] = cap;
    numTopology++;
  }
  fclose(fp1);

  /* Next read in the calls from "callworkload.dat" and set up events */
  FILE *fp2 = fopen("callworkload.dat", "r");
  int j = 0;
  char src2, dst2;
  float arriveTime, durationTime;
  while ((j = fscanf(fp1, "%f %c %c %f\n", &arriveTime, &src2, &dst2, &durationTime)) == 4)
  {
    EventList[j].event_time = arriveTime;
    EventList[j].event_type = 'S';
    EventList[j].callid = j;
    EventList[j].source = src2;
    EventList[j].destination = dst2;
    EventList[j].duration = durationTime;
    numevents++;
  }
  fclose(fp2);

  cout << "Number of topology events: " << numTopology << endl;
  cout << "NUmber of events: " << numevents << endl;

  cout << "Available Delay looks like this: " << endl;
  for (int i = 0; i < MAX_NODE; ++i)
  {
    for (int j = 0; j < MAX_NODE; ++j)
    {
      std::cout << available[i][j] << ' ';
    }
    std::cout << std::endl;
  }

  /* Now simulate the call arrivals and departures */
  // while (numevents > 0)
  // {
  //   /* Get information about the current event */
  //   printf("Event of type %d at time %8.6f (call %d from %c to %c)\n", type, start, callid, src, dst);

  //   if (type == CALL_ARRIVAL)
  //     if (RouteCall(src, dst, callid) > 0)
  //       success++;
  //     else
  //       blocked++;
  //   else
  //     ReleaseCall(src, dst, callid);
  // }
  /* Print final report here */
}