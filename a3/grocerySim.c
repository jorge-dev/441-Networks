/* Simulation of the Grocery Shop example in CPSC 531    */
/*                                                       */
/* Written by Carey Williamson, University of Calgary    */
/*                                                       */
/* Usage: cc -o grocer grocer.c -lm                      */
/*        ./grocer                                       */

#include <stdio.h>
#include <stdlib.h>
#include <math.h> /* use man 3 log for the math functions */

/* Event types */
#define NULL_EVENT 0
#define ARRIVAL 1
#define DEPARTURE 2
#define STOP 3

/* Server state */
#define IDLE 0
#define BUSY 1

/* Model parameters */
#define LAMBDA 0.2      /* Arrival rate of new customers */
#define RATE 0.333      /* Service rate of checkout server */
#define MAX_CUSTOMERS 5
#define MAX_Q 10
#define MAX_EVENTS 10
#define END_OF_TIME 60.0
/* #define RANDOMIZED 1 */

/* Debugging output */
//#define DEBUG 0
//#define DEBUG2 0        /* event list sorting */
/* #define DEBUG3 1        /* event list shifting */
/* #define DEBUG4 1        /* event list copying */

/* Event list global variables */
struct
  {
    int type;
    int idnum;
    float time;
  } eventlist[MAX_EVENTS];
int head, tail, events;
float now;

float interarrivaltimes[MAX_CUSTOMERS];
float arrivaltimes[MAX_CUSTOMERS];
float servicetimes[MAX_CUSTOMERS];
float delaytimes[MAX_CUSTOMERS];
float sojourntimes[MAX_CUSTOMERS];
float departuretimes[MAX_CUSTOMERS];

/***********************************************************************/
/*                 RANDOM NUMBER GENERATION STUFF                      */
/***********************************************************************/

/* Parameters for random number generation. */
#define MAX_INT 2147483647       /* Maximum positive integer 2^31 - 1 */

/* Generate a random floating point value uniformly distributed in [0,1] */
float Uniform01()
  {
    float randnum;

    /* get a random positive integer from random() */
    randnum = (float) 1.0 * random();

    /* divide by max int to get something in 0..1  */
    randnum = (float) randnum / (1.0 * MAX_INT); 

    return( randnum );
  }

/* Generate a random floating point number from an exponential    */
/* distribution with mean mu.                                     */

float Exponential(mu)
    float mu;
  {
    float randnum, ans;

    randnum = Uniform01();
    ans = -(mu) * log(randnum);

    return( ans );
  }

/* Event list shifting (gross and inefficient!) */
/* This resets the indices without changing content or cardinality of list */
int ShiftEventList()
  {
    int i, j;
#ifdef DEBUG3
    printf("    Event list BEFORE shifting: \n");
    PrintEventList();
#endif
    for( i = 0, j = head; i < events; i++, j++ )
      {
	eventlist[i].type = eventlist[j].type;
	eventlist[i].idnum = eventlist[j].idnum;
	eventlist[i].time = eventlist[j].time;
      }
    head = 0;
    tail = events - 1;
#ifdef DEBUG3
    printf("    Event list AFTER shifting: \n");
    PrintEventList();
#endif
  }

/* Event list sorting (gross and inefficient!) */
/* This puts the list into timestamp order with changing indices or cardinality of list */
int SortEventList()
  {
    int i, j, spot, index;
    float smallest;
#ifdef DEBUG2
    printf("  Event list BEFORE sorting: \n");
    PrintEventList();
#endif
    for( i = 0; i < events; i++ )
      {
	/* find the smallest timestamp */
	smallest = eventlist[i].time;
	spot = i;
	for( j = i+1; j < events; j++ )
	  {
	    if( eventlist[j].time < smallest )
	      {
		smallest = eventlist[j].time;
		spot = j;
	      }
	  }
#ifdef DEBUG4 
	printf("Copying event from spot %d to position %d\n", spot, i);
#endif
	/* copy that event to position i if needed */
	if( i != spot )
	  {
	    int temptype, tempidnum;
	    float temptime;
	    temptype = eventlist[i].type;
	    tempidnum = eventlist[i].idnum;
	    temptime = eventlist[i].time;
	    eventlist[i].type = eventlist[spot].type;
	    eventlist[i].idnum = eventlist[spot].idnum;
	    eventlist[i].time = eventlist[spot].time;
	    eventlist[spot].type = temptype;
	    eventlist[spot].idnum = tempidnum;
	    eventlist[spot].time = temptime;
	  }
      }
#ifdef DEBUG2
    printf("  Event list AFTER sorting: \n");
    PrintEventList();
#endif
  }

/* Event list debugging */
int PrintEventList()
  {
    int i;
#ifdef DEBUG
    printf("      Event list at time %8.6f currently has %d events: head %d tail %d\n",
	   now, events, head, tail);
#endif
    for( i = head; i <= tail; i++ )
      {
	int who, what;
	float when;
	char whatchar;
	what = eventlist[i].type;
	who = eventlist[i].idnum;
	when = eventlist[i].time;
	if( what == ARRIVAL )
	  whatchar = 'A';
	else if( what == DEPARTURE )
	  whatchar = 'D';
	else if( what == NULL_EVENT )
	  whatchar = 'N';
	else if( what == STOP )
	  whatchar = 'E';
	else whatchar = '?';
	printf("      eventlist[%d]: %c, %d, %8.6f\n",
	       i, whatchar, when, who);
      }
  }

/***********************************************************************/
/*                 MAIN PROGRAM                                        */
/***********************************************************************/

int main()
  {
    int i, index, eventtype, who, maxq;
    int arrivals, departures, active, qsize, state;
    float when, busytime, startbusy;

    /* Initialization */
    state = IDLE;
    qsize = 0;
    maxq = 0;
    active = 0;
    arrivals = 0;
    departures = 0;
    busytime = 0.0;
    now = 0.0;
    for( i = 0; i < MAX_CUSTOMERS; i++ )
      {
	interarrivaltimes[i] = 0.0;
	arrivaltimes[i] = 0.0;
	servicetimes[i] = 0.0;
	delaytimes[i] = 0.0;
	sojourntimes[i] = 0.0;
	departuretimes[i] = 0.0;
      }
#ifndef RANDOMIZED
    interarrivaltimes[0] = 0.0;
    interarrivaltimes[1] = 8.0;
    interarrivaltimes[2] = 6.0;
    interarrivaltimes[3] = 1.0;
    interarrivaltimes[4] = 8.0;
    servicetimes[0] = 4.0;
    servicetimes[1] = 1.0;
    servicetimes[2] = 4.0;
    servicetimes[3] = 3.0;
    servicetimes[4] = 5.0;
#endif
    for( i = 0; i < MAX_EVENTS; i++ )
      {
	eventlist[i].type = NULL_EVENT;
	eventlist[i].idnum = -1;
	eventlist[i].time = END_OF_TIME;
      }

    /* Enqueue two initial seed events */
    events = 0;
    index = 0;
    eventlist[index].type = ARRIVAL;
    eventlist[index].idnum = arrivals;
#ifdef RANDOMIZED
    eventlist[index].time = now + Exponential(1.0 / LAMBDA);
#else
    eventlist[index].time = now + interarrivaltimes[arrivals];
#endif
    events++;
    index++;
    eventlist[index].type = STOP;
    eventlist[index].idnum = -1;
    eventlist[index].time = END_OF_TIME;
    events++;
    head = 0;
    tail = index;
    index++;
    PrintEventList();

    /* Extract first event */
    eventtype = eventlist[head].type;
#ifdef DEBUG
    printf("Extracting event %d of type %d from head of event list\n",
	   head, eventtype);
#endif


    /* Main simulation loop */
    while( eventtype != STOP )
      {
	/* Extract relevant info about event */
	who = eventlist[head].idnum;
	when = eventlist[head].time;

	/* Remove event */
	eventlist[head].type = NULL_EVENT;
	eventlist[head].idnum = -1;
	eventlist[head].time = END_OF_TIME;
	head++;
	events--;

	/* Determine event type to handle */
	if( eventtype == ARRIVAL )
	  {
#ifdef DEBUG
	    printf("Event is arrival of customer %d at time %8.6f\n", who, when);
#endif
	    now = when;
	    arrivaltimes[arrivals] = now;
	    arrivals++;
	    active++;
	    /* Update system state */
	    if( state == IDLE )
	      {
		state = BUSY;
		startbusy = now;
#ifdef DEBUG
		printf("Becoming BUSY at time %8.6f\n", now);
#endif
		/* Schedule the departure event for this customer */
		index = tail;
		index++;
		if( index < MAX_EVENTS )
		  {
		    /* Create the event */
		    eventlist[index].type = DEPARTURE;
		    eventlist[index].idnum = who;
#ifdef RANDOMIZED
		    servicetimes[who] = Exponential(1.0 / RATE);
#endif
		    when = now + servicetimes[who];
		    eventlist[index].time = when;
		    events++;
		    tail++;
#ifdef DEBUG
		    printf("Scheduled the departure of customer %d at time %8.6f\n", who, when);
#endif
		    /* Put it in the correct position */
		    if( head > 0 )
		      ShiftEventList();
		    SortEventList();
		  }
		else
		  {
		    printf("Yikes!! Event list is already full!\n");
		    exit(-1);
		  }
	      }
	    else /* BUSY */
	      {
		if( qsize < MAX_Q )
		  {
		    qsize++;
#ifdef DEBUG
		    printf("Queue at time %8.6f now has %d customers!\n", now, qsize);
#endif
		    if( qsize > maxq )
		      maxq = qsize;
		  }
		else
		  {
		    fprintf(stderr, "Too many to fit in queue! Time %8.6f Customer %d  qsize %d\n",
			    now, who, qsize);
		    exit(-1);
		  }
	      }
	    /* Schedule next arrival event (if any) */
	    if( arrivals < MAX_CUSTOMERS )
	      {
		index = tail;
		index++;
		if( index < MAX_EVENTS )
		  {
		    /* Create the event */
		    eventlist[index].type = ARRIVAL;
		    eventlist[index].idnum = arrivals;
#ifdef RANDOMIZED
		    interarrivaltimes[arrivals] = Exponential(1.0 / LAMBDA);
#endif
		    when = now + interarrivaltimes[arrivals];
		    eventlist[index].time = when;
		    events++;
		    tail++;
#ifdef DEBUG
		    printf("Scheduled the arrival of customer %d at time %8.6f\n", arrivals, when);
#endif
		    /* Put it in the correct position */
		    if( head > 0 )
		      ShiftEventList();
		    SortEventList();
		  }
		else
		  {
		    printf("Yikes!! Event list is full!\n");
		    exit(-1);
		  }
	      }
	    else printf("No more customers at this point!\n");
	  }
	else if( eventtype == DEPARTURE )
	  {
#ifdef DEBUG
	    printf("Event is departure of customer %d at time %8.6f\n", who, when);
#endif
	    now = when;
	    departuretimes[who] = now;
	    sojourntimes[who] = departuretimes[who] - arrivaltimes[who];
	    delaytimes[who] = sojourntimes[who] - servicetimes[who];
	    departures++;
	    active--;
	    if( qsize > 0 )
	      {
		/* Find the next customer and schedule their service */
		who = departures;
		index = tail;
		index++;
		if( index < MAX_EVENTS )
		  {
		    /* Create the event */
		    eventlist[index].type = DEPARTURE;
		    eventlist[index].idnum = who;
#ifdef RANDOMIZED
		    servicetimes[who] = Exponential(1.0 / RATE);
#endif
		    when = now + servicetimes[who];
		    eventlist[index].time = when;
		    events++;
		    tail++;
#ifdef DEBUG
		    printf("Staying busy! Next scheduled departure is customer %d at time %8.6f\n", who, when);
#endif
		    /* Put it in the correct position */
		    if( head > 0 )
		      ShiftEventList();
		    SortEventList();
		  }
		else
		  {
		    printf("Yikes!! Event list is totally full!\n");
		    exit(-1);
		  }
		qsize--;
	      }
	    else
	      {
		state = IDLE;
		busytime += now - startbusy;
#ifdef DEBUG
		printf("Nobody else waiting. Becoming IDLE at time %8.6f...\n", now);
#endif
	      }
	  }
	else
	  {
	    fprintf(stderr, "Unknown event type %d!!!\n", eventtype);
	    exit(-1);
	  }

	/* Extract front event */
	eventtype = eventlist[head].type;
#ifdef DEBUG
	printf("Extracting event %d of type %d from head of event list\n",
	       head, eventtype);
#endif


      }

    /* Record end time of stop event */
    now = eventlist[head].time;
    if( state == BUSY )
      busytime += now - startbusy;

    /* Final output after STOP event is seen */
    printf("\n");
#ifdef RANDOMIZED
    printf("Grocery Shop Simulation: LAMBDA %3.2f RATE %3.2f MAX_Q %d\n",
	   LAMBDA, RATE, MAX_Q);
#else
    printf("Grocery Shop Simulation:\n");
#endif
    printf("Time: %8.6f  Arrivals: %d  Departures: %d  In system: %d\n",
	   now, arrivals, departures, active);
    printf("Cust   Arrive  Delay  Service  Sojourn  Depart\n");   
    for( i = 0; i < departures; i++ )
      {
	printf(" %d    %6.1f  %6.1f  %6.1f  %6.1f  %6.1f\n",
	       i, arrivaltimes[i], delaytimes[i],
	       servicetimes[i], sojourntimes[i], departuretimes[i]);
      }
    printf("Server busy time: %8.6f  Utilization: %8.6f\n",
	   busytime, busytime/now);
    printf("Maximum queue size observed: %d\n", maxq);
  }
