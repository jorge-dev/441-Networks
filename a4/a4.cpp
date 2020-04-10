/*
Name:   Mohammad Al Sharbati
ID:	    30027437
Source: budgie.cpp

Assumptoins:
- There are N budgies in the BLAN, where N is a finite positive integer.
- On average, budgies spend a fraction S of their day singing, and the other fraction 1-S in quiet mode 
	(e.g., staring, listening, grooming, eating, drinking, pooping, sleeping).
- Budgies start singing at random times (day or night), and the duration of a budgie song is random.
	Specifically, the song duration for each budgie is exponentially distributed with a mean duration of 3.0 minutes,
	and the silent period for each budgie is exponentially distributed with a mean duration of 30.0 minutes. 
	(With these settings, the value of S is 0.091, since on average there are 3 minutes of song followed by 30 minutes of quiet,
	a pattern that is repeated over and over all day long.)
- Each budgie operates independently of other budgies. That is, they can choose to burst into song whenever they want, regardless of what the other budgies are doing.
- When no budgies are singing, the BLAN is Quiet.
- When exactly one budgie is singing, the BLAN is Melodious (i.e., very beautiful music).
- When more than one budgie is singing at the same time, the BLAN is Squawky. 
	That is, the overlapping songs of the multiple budgies confuse the budgies, as they each try to sing even louder than the others.
	The result is painful to the ears, and hence the term "squawky".


Goal: Figure out what number of budgies maximises Melodious operation

Events
• Budgie Song currTime 
• Budgie Song End

Parameters
• Number of budgies (variable)
• Mean singing time (fixed) 
• Mean quiet time (fixed)

State Variables
• Number of budgies singing

Statistic Variables
• Quiet time
• Melodious time 
• Squawky time 
• Total time
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;
/* Budgie states */
#define QUIET 0
#define SINGING 1
#define SQUAWKY 2 // this can be any number other than 2 and it will be the same

// Simlatoin constant
#define QUIET_TIME_MEAN 30.0 // minutes

/* time constants for simmulation */
#define DAY 1440.0		  //minutes in a day
#define WEEK 10080.0	  //minutes in a week
#define MONTH 43800.0	  //minutes in a month
#define INFINITE 999999.0 // time for testing large perios of time

struct Event
{
	float timeNext;
	vector<float> next;
	float currTime;
	float prevTime;
};

struct Budgie
{
	vector<int> budgieStatus;
	int currSong;
	int currSongStart; //time
	int currSongEnd;   //time
};

/* Global variables */
Event budgieEvent;
Budgie budgie;

int currEventElement = 0;
int numBirdsSinging = 0;
int numPerfectSongs = 0;
int numAttemptedSongs = 0;

float quietTimeTotal = 0.0;
float melodiousTimeTotal = 0.0;
float squawkyTimeTotal = 0.0;
float perfectSongsTotalTime = 0.0;

/* Random Generator Variable */
#define RANDOM_NUM_INIT 104729

/* Max integer */
#define MAX_INT INT_MAX

/* Function prototypes */
float Uniform01();
float Exponential(float mu);
void BudgieEventHandler(int state, int N, float Song_Duration_Mean);
bool checkIfSongIsPerfect(int i);
void printSimulationResults(int N, float Song_Duration_Mean);
void printFinalTable(int N);

int main(int argc, char *argv[])
{
	//user can choose to change this two variables
	int N;					  // Number of Budgis in BLAN
	float Song_Duration_Mean; // mean duration for every song in BlAN
	if (argc == 1)
	{
		N = 3;
		Song_Duration_Mean = 3.0;
	}
	else if (argc == 2)
	{
		N = atoi(argv[1]);
		Song_Duration_Mean = 3.0;
	}
	else if (argc == 3)
	{
		N = atoi(argv[1]);
		Song_Duration_Mean = float(atoi(argv[2]));
	}
	else
	{
		N = 3;
		Song_Duration_Mean = 3.0;
	}

	//Init all budgies to quiet
	budgie.budgieStatus = vector<int>(N, QUIET);
	budgieEvent.next = vector<float>(N, (1.0 / QUIET_TIME_MEAN));

	// Set RANDOM_NUM_INIT for random number generation
	// got it form here http://www.cplusplus.com/reference/cstdlib/srand/
	//dont know why but i i remove this line it changes the answer somehow
	// even though im not using the variable random so thats why I left it
	srand(RANDOM_NUM_INIT);
	int random = Uniform01();

	/* Main simulation */
	while (budgieEvent.currTime < MONTH)
	{

		//Gets the necxt budgie event and also sets its time to "infinity"
		budgieEvent.timeNext = INFINITE;
		for (int i = 0; i < N; i++)
		{
			if (budgieEvent.next[i] < budgieEvent.timeNext)
			{
				budgieEvent.timeNext = budgieEvent.next[i];
				currEventElement = i;
			}
		}

		//decide whether to keep sim alive or terminate it
		if (budgieEvent.timeNext < MONTH)
		{
			budgieEvent.currTime = budgieEvent.timeNext;
			//Handle the budgie events according to its state
			if (budgie.budgieStatus[currEventElement] == SINGING)
				BudgieEventHandler(SINGING, N, Song_Duration_Mean);
			else
				BudgieEventHandler(QUIET, N, Song_Duration_Mean);
		}

		else
		{
			//Max time was reached and simulation is over
			budgieEvent.currTime = MONTH;
			
			//birds are quiet
			if (numBirdsSinging == QUIET)
				quietTimeTotal += budgieEvent.currTime - budgieEvent.prevTime;
			//birds are singing melodiosly
			else if (numBirdsSinging == SINGING)
				melodiousTimeTotal += budgieEvent.currTime - budgieEvent.prevTime;
			//birdsa re squawking
			else
				squawkyTimeTotal += budgieEvent.currTime - budgieEvent.prevTime;
			budgieEvent.prevTime = budgieEvent.currTime;
		}
	}
	printSimulationResults(N, Song_Duration_Mean);
	printFinalTable(N);
}

//Get a random number in between 0 and 1
//Copied from the Barber example

float Uniform01()
{
	/* Get a random (+) integer from rand() */
	float randomNum = (float)1.0 * rand();

	/* Divide by MAX_INT to get something in 0..1 */
	randomNum = (float)randomNum / (1.0 * MAX_INT);

	return randomNum;
}

//	Exponential Variate
//Copied from the Barber example

float Exponential(float mean)
{

	float randomNum = Uniform01();
	float answer = -(mean)*log(randomNum);

	return answer;
}

//THis will use the given event state and will update each time and state of that event
void BudgieEventHandler(int state, int N, float Song_Duration_Mean)
{

	if (state == SINGING)
	{
		if (numBirdsSinging == QUIET)
		{
			cout << "Oops there are no budgies singing somehow." << endl;
			quietTimeTotal += budgieEvent.currTime - budgieEvent.prevTime;
		}

		else if (numBirdsSinging == SINGING)
		{
			melodiousTimeTotal += budgieEvent.currTime - budgieEvent.prevTime;
			//if song is perfect increase the total number of perfect songs
			if (checkIfSongIsPerfect(currEventElement))
				numPerfectSongs++;

			//Update the perfect Songs total time
			perfectSongsTotalTime += budgieEvent.currTime - budgieEvent.prevTime;
		}
		else
		{
			//Birds are sqwaking so update total times accordingly
			squawkyTimeTotal += budgieEvent.currTime - budgieEvent.prevTime;
		}

		// This budgie went quiet, so mean for quiet time will be calculated
		//also budgie and budgie event will be updated to reflect this
		budgie.budgieStatus[currEventElement] = QUIET;
		budgieEvent.next[currEventElement] += Exponential(QUIET_TIME_MEAN);
		//budgie went quiet so theres one less singing
		numBirdsSinging--;

		//budgie went quiet and the event will be updated
		budgieEvent.prevTime = budgieEvent.currTime;
		budgie.currSongEnd = budgieEvent.currTime;
	}

	else
	{
		//birds are quiet
		if (numBirdsSinging == QUIET)
			quietTimeTotal += budgieEvent.currTime - budgieEvent.prevTime;
		//birds are singing melodiosly
		else if (numBirdsSinging == SINGING)
			melodiousTimeTotal += budgieEvent.currTime - budgieEvent.prevTime;
		//birdsa re squawking
		else
			squawkyTimeTotal += budgieEvent.currTime - budgieEvent.prevTime;

		// This budgie started singing, so mean for singing time will be calculated
		//also budgie and budgie event will be updated to reflect this
		budgie.budgieStatus[currEventElement] = SINGING;
		budgieEvent.next[currEventElement] += Exponential(Song_Duration_Mean);
		//budgie is singing so there's one more singing
		numBirdsSinging++;

		//budgie is singing and the event will be updated
		budgieEvent.prevTime = budgieEvent.currTime;
		budgie.currSongStart = budgieEvent.currTime;
		budgie.currSong = currEventElement;
		numAttemptedSongs++;
	}
}

//check if current song is perfect or not
bool checkIfSongIsPerfect(int i)
{
	bool isSongPerfect = (budgie.currSong == currEventElement) && (budgie.currSongEnd < budgie.currSongStart) ? true : false;
	return isSongPerfect;
}

/*
	Outputs simulation results
*/
void printSimulationResults(int N, float Song_Duration_Mean)
{

	//Calcaulation variables
	float singTimeMean = (Song_Duration_Mean / (QUIET_TIME_MEAN + Song_Duration_Mean));
	float quietTimePercent = (100.0 * quietTimeTotal) / budgieEvent.currTime;
	float melodiousTimePercent = (100.0 * melodiousTimeTotal) / budgieEvent.currTime;
	float sqwuakyTimePercent = (100.0 * squawkyTimeTotal) / budgieEvent.currTime;
	float perfectSongPercent = (100.0 * numPerfectSongs) / numAttemptedSongs;
	float perfectSongTimePercent = (100.0 * perfectSongsTotalTime) / budgieEvent.currTime;
	cout << fixed << setprecision(3);

	cout << "\t\t      +++++++++++++++++++++++++++++++++++" << endl;
	cout << "\t\t      + INITIAL SETTINGS FOR SIMULATION +" << endl;
	cout << "\t\t      +++++++++++++++++++++++++++++++++++" << endl
		 << endl;

	cout << "	N = " << N
		 << "			QUIET TIME MEAN = " << QUIET_TIME_MEAN << " minutes" << endl;
	cout << "	S = " << singTimeMean << "		SONG DURATION MEAN = " << Song_Duration_Mean << " minutes" << endl;

	cout << "___________________________________________________________________________________" << endl
		 << endl;

	cout << "\t\t           +++++++++++++++++++++++++" << endl;
	cout << "\t\t           + RESULTS OF SIMULATION +" << endl;
	cout << "\t\t           +++++++++++++++++++++++++" << endl
		 << endl;

	cout << fixed << setprecision(3);
	cout << "	BLAN budgieEvent total time:" << right << setw(20) << budgieEvent.currTime << " minutes" << endl;
	cout << "	----------------------------------------------------" << endl;

	cout << "	BLAN total Quiet time:" << right << setw(19) << quietTimeTotal << " minutes" << endl;
	cout << "	BLAN Quiet time percentage:" << right << setw(11) << quietTimePercent << "%" << endl;
	cout << "	----------------------------------------------------" << endl;

	cout << "	BLAN total Melodious time:" << right << setw(15) << melodiousTimeTotal << " minutes" << endl;
	cout << "	BLAN Melodious time percentage:" << right << setw(8) << melodiousTimePercent << "%" << endl;
	cout << "	----------------------------------------------------" << endl;

	cout << "	BLAN total Sqwuaky time:" << right << setw(18) << squawkyTimeTotal << " minutes" << endl;
	cout << "	BLAN Sqwuaky time percentage:" << right << setw(10) << sqwuakyTimePercent << "%" << endl;
	cout << "	----------------------------------------------------" << endl
		 << endl;

	cout << "\n	Total number of Attempted songs:" << right << setw(10) << numAttemptedSongs << endl;
	cout << "	----------------------------------------------------" << endl;

	cout << "	Total number of Perfect songs:" << right << setw(10) << numPerfectSongs << endl;
	cout << "	Perfect songs percentage:" << right << setw(17) << perfectSongPercent << "%" << endl;
	cout << "	----------------------------------------------------" << endl;

	cout << "	BLAN total Perfect song time: " << right << setw(15) << perfectSongsTotalTime << " minutes" << endl;
	cout << "	BLAN Perfect song time percentage: " << right << setw(7) << perfectSongTimePercent << "%"
		 << endl;
	cout << "	----------------------------------------------------" << endl
		 << endl;
	cout << "___________________________________________________________________________________" << endl
		 << endl;
}

/*
	Outputs results of each time, with the current N
*/
void printFinalTable(int N)
{

	//Calcaulation variables
	float quietTimePercent = (100.0 * quietTimeTotal) / budgieEvent.currTime;
	float melodiousTimePercent = (100.0 * melodiousTimeTotal) / budgieEvent.currTime;
	float sqwuakyTimePercent = (100.0 * squawkyTimeTotal) / budgieEvent.currTime;
	float perfectSongPercent = (100.0 * numPerfectSongs) / numAttemptedSongs;
	cout << fixed << setprecision(3);

	cout << "\t\t           ++++++++++++++++++++++++++" << endl;
	cout << "\t\t           + TABLE OF FINAL RESULTS +" << endl;
	cout << "\t\t           ++++++++++++++++++++++++++" << endl
		 << endl;
	cout << "	N" << setw(13) << "Quiet %" << setw(18) << "Melodious %" << setw(14) << "Sqwuaky%" << setw(20) << "Perfect Songs %" << endl;
	cout << "	______________________________________________________________" << endl
		 << endl;

	cout << "	" << N << setw(13);
	cout << quietTimePercent << right << setw(15);

	if (N > 0)
		cout << melodiousTimePercent << right << setw(15);
	else
		cout << "0.000000" << setw(15);

	if (N > 1)
		cout << sqwuakyTimePercent << right << setw(15);
	else
		cout << "0.000000" << setw(15);

	if (N > 0)
		cout << perfectSongPercent << "\n\n";
	else
		cout << "0.000000" << endl
			 << endl;
}
