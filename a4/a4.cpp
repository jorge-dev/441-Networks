/*
Name:  Jorge Avila
ID:	   10123968
Asg:   4
Tit#:  1

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


// Simlatoin constant
#define QUIET_TIME_MEAN 30.0 // minutes

/* time constants for simmulation */
#define DAY 1440.0		  //minutes in a day
#define WEEK 10080.0	  //minutes in a week
#define MONTH 43800.0	  //minutes in a month
#define INFINITE 999999.0 // time for testing large perios of time

class BudgieClass
{
public:
	float eventTimeNext;
	vector<float> eventNext;
	float eventCurrTime;
	float eventPrevTime;

	vector<int> budgieStatus;
	int budgieCurrSong;
	int budgieCurrSongStart; //time
	int budgieCurrSongEnd;	 //time

public:
	BudgieClass(int N);
	float Uniform01();
	float Exponential(float mu);
	void BudgieEventHandler(int state, int N, float Song_Duration_Mean);
	bool checkIfSongIsPerfect(int i);
	void printSimulationResults(int N, float Song_Duration_Mean);
};

/* Global variables */
// Event
// Budgie budgie;

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
	BudgieClass testBudgie(N);



	/* Main simulation */
	while (testBudgie.eventCurrTime < MONTH)
	{
		
		//Gets the necxt budgie event and also sets its time to "infinity"
		testBudgie.eventTimeNext = INFINITE;
		for (int i = 0; i < N; i++)
		{
			if (testBudgie.eventNext[i] < testBudgie.eventTimeNext)
			{
				testBudgie.eventTimeNext = testBudgie.eventNext[i];
				currEventElement = i;
				
			}
		}

		//decide whether to keep sim alive or terminate it
		if (testBudgie.eventTimeNext < MONTH)
		{
			testBudgie.eventCurrTime = testBudgie.eventTimeNext;
			//Handle the budgie events according to its state
			if (testBudgie.budgieStatus[currEventElement] == SINGING){
				testBudgie.BudgieEventHandler(SINGING, N, Song_Duration_Mean);
			}
			else{
				testBudgie.BudgieEventHandler(QUIET, N, Song_Duration_Mean);
			}
		}

		else
		{
			
			if (testBudgie.checkIfSongIsPerfect(currEventElement))
			{
				numPerfectSongs++;

				//Update the perfect Songs total time
				perfectSongsTotalTime += testBudgie.eventCurrTime - testBudgie.eventPrevTime;
			}
			
			//Max time was reached and simulation is over
			testBudgie.eventCurrTime = MONTH;

			//birds are quiet
			if (numBirdsSinging == QUIET)
				quietTimeTotal += testBudgie.eventCurrTime - testBudgie.eventPrevTime;
			//birds are singing melodiosly
			else if (numBirdsSinging == SINGING)
				melodiousTimeTotal += testBudgie.eventCurrTime - testBudgie.eventPrevTime;
			//birdsa re squawking
			else
				squawkyTimeTotal += testBudgie.eventCurrTime - testBudgie.eventPrevTime;

			testBudgie.eventPrevTime = testBudgie.eventCurrTime;
			
		}
		
	}
	
	testBudgie.printSimulationResults(N, Song_Duration_Mean);
}

BudgieClass::BudgieClass(int N)
{
	budgieStatus = vector<int>(N, QUIET);
	eventNext = vector<float>(N, ( QUIET_TIME_MEAN));
}

//Get a random number in between 0 and 1
//Copied from the Barber example

float BudgieClass::Uniform01()
{
	/* Get a random (+) integer from rand() */
	float randomNum = (float)1.0 * rand();

	/* Divide by MAX_INT to get something in 0..1 */
	randomNum = (float)randomNum / (1.0 * MAX_INT);

	return randomNum;
}

//	Exponential Variate
//Copied from the Barber example

float BudgieClass::Exponential(float mean)
{

	float randomNum = Uniform01();
	float answer = -(mean)*log(randomNum);

	return answer;
}

//THis will use the given event state and will update each time and state of that event
void BudgieClass::BudgieEventHandler(int state, int N, float Song_Duration_Mean)
{
	

	if (state == SINGING)
	{
		
		
		if (numBirdsSinging == QUIET)
		{
			cout << "Oops there are no budgies singing somehow." << endl;
			quietTimeTotal += eventCurrTime - eventPrevTime;
			
		}

		else if (numBirdsSinging == SINGING)
		{
			melodiousTimeTotal += eventCurrTime - eventPrevTime;
			if (checkIfSongIsPerfect(currEventElement))
			{
				numPerfectSongs++;

				//Update the perfect Songs total time
				perfectSongsTotalTime += eventCurrTime - eventPrevTime;
			}
			
		}
		else
		{
			//Birds are sqwaking so update total times accordingly
			squawkyTimeTotal += eventCurrTime - eventPrevTime;
			
			
		}

		// This budgie went quiet, so mean for quiet time will be calculated
		//also budgie and budgie event will be updated to reflect this
		budgieStatus[currEventElement] = QUIET;
		eventNext[currEventElement] += Exponential(QUIET_TIME_MEAN);
		//budgie went quiet so theres one less singing
		numBirdsSinging--;

		//budgie went quiet and the event will be updated
		eventPrevTime = eventCurrTime;
		budgieCurrSongEnd = eventCurrTime;
		
	
	}

	else
	{
		
		
		//birds are quiet
		if (numBirdsSinging == QUIET)
			quietTimeTotal += eventCurrTime - eventPrevTime;
		//birds are singing melodiosly
		else if (numBirdsSinging == SINGING)
			melodiousTimeTotal += eventCurrTime - eventPrevTime;
		//birds are squawking
		else
			squawkyTimeTotal += eventCurrTime - eventPrevTime;

		// This budgie started singing, so mean for singing time will be calculated
		//also budgie and budgie event will be updated to reflect this
		budgieStatus[currEventElement] = SINGING;
		eventNext[currEventElement] += Exponential(Song_Duration_Mean);
		//budgie is singing so there's one more singing
		numBirdsSinging++;

		//budgie is singing and the event will be updated
		eventPrevTime = eventCurrTime;
		budgieCurrSongStart = eventCurrTime;
		budgieCurrSong = currEventElement;
		numAttemptedSongs++;

		
	}
	
	
}

//check if current song is perfect or not
bool BudgieClass::checkIfSongIsPerfect(int i)
{
	bool isSongPerfect = (budgieCurrSong == currEventElement) && (budgieCurrSongEnd < budgieCurrSongStart) ? true : false;
	return isSongPerfect;
}

/*
	Outputs simulation results
*/
void BudgieClass::printSimulationResults(int N, float Song_Duration_Mean)
{

	//Calcaulation variables
	float singTimeMean = (Song_Duration_Mean / (QUIET_TIME_MEAN + Song_Duration_Mean));
	float quietTimePercent = (100.0 * quietTimeTotal) / eventCurrTime;
	float melodiousTimePercent = (100.0 * melodiousTimeTotal) / eventCurrTime;
	float sqwuakyTimePercent = (100.0 * squawkyTimeTotal) / eventCurrTime;
	float perfectSongPercent = (100.0 * numPerfectSongs) / numAttemptedSongs;
	float perfectSongTimePercent = (100.0 * perfectSongsTotalTime) / eventCurrTime;
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
	cout << "	BLAN total time:" << right << setw(20) << eventCurrTime << " minutes" << endl;
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
