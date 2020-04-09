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
• Number of birds (variable)
• Mean singing time (fixed) 
• Mean quiet time (fixed)

State Variables
• Number of birds singing

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
#define SQUAWKY 2

/* Simlatoin constants */
#define N 3					   // number of Budgies in BLAN
#define QUIET_TIME_MEAN 30.0   // minutes
#define SONG_DURATION_MEAN 3.0 // minutes

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
	vector<int> birdStatus;
	int currSong;
	int currSongStart; //time
	int currSongEnd;   //time
};

/* Global variables */
Event event;
Budgie bird;

int currEventElement = 0,
	numBirdsSinging = 0,
	numPerfectSongs = 0,
	numAttemptedSongs = 0;

float quietTimeTotal = 0.0,
	  melodiousTimeTotal = 0.0,
	  squawkyTimeTotal = 0.0,
	  perfectSongsTotalTime = 0.0;

/* Random Generator Variable */
#define RANDOM_NUM_INIT 104729

/* Max integer */
#define MAX INT_MAX

/* Function prototypes */
float Uniform01();
float Exponential(float mu);

void initAllBirdsToQuiet();
void findnext();
void handleBirdEvent(int state);

bool isPerfectSong(int i);
void updatePerfectTime();
void updateStateTimes();
void updateEventFrom(int stateIndex);
void updatenumBirdsSinging(int currentBird);

void calcMeanTimes(int state);
void finalizeAllTimes();

void outputSimResults();
void outputFinal();

int main()
{

	// initAllBirdsToQuiet(); /* Before starting simulation, initialize all birds to
	//   the QUIET state */
	bird.birdStatus = vector <int>(N,QUIET);
	float test = (1.0/QUIET_TIME_MEAN);
	event.next = vector<float>(N,test);
	

	srand(RANDOM_NUM_INIT); /* Set RANDOM_NUM_INIT for random number generation (for consistency) */
	int r = Uniform01();	/* Call srand() before this line */

	/* Main simulation */
	while (event.currTime < MONTH)
	{

		findnext();
		/* Keep simulation running */
		if (event.timeNext < MONTH)
		{

			event.currTime = event.timeNext;
			/* Update corresponding variables and state changes */
			if (bird.birdStatus[currEventElement] == SINGING)
				handleBirdEvent(SINGING);
			else
				handleBirdEvent(QUIET);
		}

		else
			finalizeAllTimes(); /* Maximum time reached */
	}
	outputSimResults();
	outputFinal();
}

/*
	Get a random number in between 0 and 1
*/
float Uniform01()
{
	/* Get a random (+) integer from rand() */
	float randomNum = (float)1.0 * rand();

	/* Divide by MAX to get something in 0..1 */
	randomNum = (float)randomNum / (1.0 * MAX);

	return randomNum;
}

/*
	Exponential Variate
*/
float Exponential(float mean)
{

	float randomNum = Uniform01();
	float answer = -(mean)*log(randomNum);

	return answer;
}

/*
	Initializes all birds to the quiet state before starting simulation.
*/
void initAllBirdsToQuiet()
{

	for (int i = 0; i < N; i++)
	{
		bird.birdStatus[i] = QUIET;
		event.next[i] = Exponential(1.0 / QUIET_TIME_MEAN);
	}
}

/*
	Finds the next event to happen but setting the next event time to 'infinity'
*/
void findnext()
{

	event.timeNext = INFINITE; /* Find next event to occur */

	for (int i = 0; i < N; i++)
	{
		if (event.next[i] < event.timeNext)
		{
			event.timeNext = event.next[i];
			currEventElement = i;
		}
	}
}

/*
	Handles each corresponding event, depending on the current state of the bird, and will
	update each corressponding time and then update the state of the bird
*/
void handleBirdEvent(int state)
{

	if (state == SINGING)
	{
		switch (numBirdsSinging)
		{
		case QUIET:
			cout << "ERROR: No birds singing." << endl; /* Lost track of the current bird somehow */
			break;
		case SINGING:
			updateStateTimes(); /* Melodious, 1 bird singing */
			if (isPerfectSong(currEventElement))
			{
				numPerfectSongs++;
			}
			updatePerfectTime(); /* Cumulatively update the perfect time total */
			break;
		default:				/* Sqwuaky */
			updateStateTimes(); /* Sqwuaky, 2 or more birds singing */
			break;
		}
		calcMeanTimes(QUIET);		  /* Calculate exponentially mean quiet time  */
		updatenumBirdsSinging(QUIET); /* One less singing bird                    */
		updateEventFrom(SINGING);	  /* This bird is now quiet                   */
	}

	else
	{
		switch (numBirdsSinging)
		{
		case QUIET:
			updateStateTimes(); /* Idle, no bird singing */
			break;
		case SINGING:
			updateStateTimes(); /* Melodious, 1 bird singing */
			break;
		default:				/* Sqwuaky */
			updateStateTimes(); /* Sqwuaky, 2 or more birds singing */
			break;
		}
		calcMeanTimes(SINGING);			/* Calculate exponentially mean singing time */
		updatenumBirdsSinging(SINGING); /* One more singing bird                     */
		updateEventFrom(QUIET);			/* This bird is now singing				  */
	}
}

/*
	Updates the corresponding type of time depending on how many birds are currently
	singing at a time
*/
void updateStateTimes()
{
	if (numBirdsSinging == 0) /* QUIET */
		quietTimeTotal += event.currTime - event.prevTime;

	else if (numBirdsSinging == 1) /* MELODIOUS */
		melodiousTimeTotal += event.currTime - event.prevTime;

	else /* SQWUAKY */
		squawkyTimeTotal += event.currTime - event.prevTime;
}

/*
	Determines if the current song is a perfect song if the current song end time is
	before the next 'scheduled' song start time
*/
bool isPerfectSong(int i)
{

	bool perfectSong;
	if ((bird.currSong == i) && (bird.currSongEnd < bird.currSongStart))
		perfectSong = true;

	else
		perfectSong = false;

	return perfectSong;
}

/*
	Updates cumulatively the perfect time total
*/
void updatePerfectTime()
{
	perfectSongsTotalTime += event.currTime - event.prevTime;
}

/*
	Updates the number of currently singing birds, depending on what state change
	the bird is transitioning to
*/
void updatenumBirdsSinging(int currentBird)
{

	if (currentBird == QUIET)
		numBirdsSinging--;

	else
		numBirdsSinging++;
}

/*
	Calculate cumulatively the exponential distribution depending on the status of
	the current bird at the current event
*/
void calcMeanTimes(int state)
{

	if (state == QUIET)
	{
		bird.birdStatus[currEventElement] = QUIET; /* This bird is quiet, so calculate using mean for quiet time */
		event.next[currEventElement] += Exponential(QUIET_TIME_MEAN);
	}

	else
	{
		bird.birdStatus[currEventElement] = SINGING; /* This bird is singing, so calculate using mean for singing time */
		event.next[currEventElement] += Exponential(SONG_DURATION_MEAN);
	}
}

/*
	Finalizes and adds all the corresponding times one last time before the simulation
	is done
*/
void finalizeAllTimes()
{

	event.currTime = MONTH; /* Event time has reached the end */
	updateStateTimes();
	event.prevTime = event.currTime;
}

/*
	Updates the state of the current bird depending on what state it is
	currently in
*/
void updateEventFrom(int stateIndex)
{
	if (stateIndex == QUIET)
	{
		event.prevTime = event.currTime;
		bird.currSongStart = event.currTime;
		bird.currSong = currEventElement;
		numAttemptedSongs++;
	}
	else
	{
		event.prevTime = event.currTime;
		bird.currSongEnd = event.currTime;
	}
}

/*
	Outputs simulation results
*/
void outputSimResults()
{

	//Calcaulation variables
	float singTimeMean = (SONG_DURATION_MEAN / (QUIET_TIME_MEAN + SONG_DURATION_MEAN));
	float quietTimePercent = (100.0 * quietTimeTotal) / event.currTime;
	float melodiousTimePercent = (100.0 * melodiousTimeTotal) / event.currTime;
	float sqwuakyTimePercent = (100.0 * squawkyTimeTotal) / event.currTime;
	float perfectSongPercent = (100.0 * numPerfectSongs) / numAttemptedSongs;
	float perfectSongTimePercent = (100.0 * perfectSongsTotalTime) / event.currTime;
	cout << fixed << setprecision(3);

	cout << "\t\t      +++++++++++++++++++++++++++++++++++" << endl;
	cout << "\t\t      + INITIAL SETTINGS FOR SIMULATION +" << endl;
	cout << "\t\t      +++++++++++++++++++++++++++++++++++" << endl
		 << endl;

	cout << "	N = " << N
		 << "			QUIET_TIME_MEAN = " << QUIET_TIME_MEAN << " minutes" << endl;
	cout << "	S = " << singTimeMean << "		SONG_DURATION_MEAN = " << SONG_DURATION_MEAN << " minutes" << endl;

	cout << "___________________________________________________________________________________" << endl
		 << endl;

	cout << "\t\t           +++++++++++++++++++++++++" << endl;
	cout << "\t\t           + RESULTS OF SIMULATION +" << endl;
	cout << "\t\t           +++++++++++++++++++++++++" << endl
		 << endl;

	cout << fixed << setprecision(3);
	cout << "	BLAN event total time:" << right << setw(20) << event.currTime << " minutes" << endl;
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
void outputFinal()
{

	//Calcaulation variables
	float quietTimePercent = (100.0 * quietTimeTotal) / event.currTime;
	float melodiousTimePercent = (100.0 * melodiousTimeTotal) / event.currTime;
	float sqwuakyTimePercent = (100.0 * squawkyTimeTotal) / event.currTime;
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
