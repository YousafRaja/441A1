#include <iostream>
#include <random>
#include <limits>
#include <sstream>
#include <vector>
#include <algorithm>
#include <array>
#include <unordered_map>
#include <string>

#include <iostream>
#include <fstream>

#include <stdio.h>
#include <limits.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <iomanip>
#include <cstdlib>
#include <ctime>
using namespace std;

//# nodes at each level = 2^(level) = count

//# of stations for each node at a specific level = N/Count

//first iteration check from 1 to 8

//if it's greater than 1, have collision, collision++, need to continue
//if it's equal to 1, have success, sucess+

//for (k)
// for(i)
//   srandom()
//   for (step)
//
//for k it doesn't matter to reinitialize the srandom but for i it should be
//the random sequence should be the same for each column of K =1, K=2, ... K=1024

#define MAX_INPUTS 4
#define MAXINT 2147483647.0
#define N 1024
//int random();
int globalStats[4]; //0-succCount,1-probeCount,2-collCount,3-idleCount

void startProbe(int A[N], int start, int end) {
	globalStats[1]++;
	bool found = false;
	bool coll = false;
	bool idle = false;
	//cout <<"probing from: " << start << " to "<<end << endl;
	for (int i = start; i < end; i++) {


		if (A[i] & !found) {
			found = true;
		} else if (A[i] & found) {
			coll = true;
			globalStats[2]++;

			//cout<<"detected collision, next probe in "<<start<<"-"<<m<<" and "<<m<<"-"<<end<<endl;



		} else {
		}
	}



	if (coll){
		int m = start + ((end - start+1) / 2);
		startProbe(A, start, m);
		startProbe(A, m, end);
	} else if (found){
		globalStats[0]++;
	} else {
		globalStats[3]++;
	}

	/*
	 if (!found) {
	 globalStats[3]++;
	 } else if (coll) {
	 globalStats[2];
	 int m = start + ((end - start) / 2);
	 startProbe(A, start, m);
	 startProbe(A, m, end);
	 } else {
	 globalStats[0]++;
	 }
	 */

	//check for success, idle or congested
}

int main() {

	cout << std::fixed;
	cout << std::setprecision(4);

	//N=8             N=1024                   i\K|1 2 ... 1024    Success rate--------------->   |
	//K=3             K=1,2,4,...,1024         0    <---------- avg success rate of 100 runs      |
	//i=0             i=0,2,4,6,8,10           2                                                  |
	//Step=1          Steps=100                4                                                 |__________1024K__ need to see increase or decrease
	float results[6][11];
	float prob;
	float prob2;
	srandom(0);
	prob = random() / MAXINT;
	srandom(0);
	prob2 = random() / MAXINT;

	int inpkt[MAX_INPUTS], pktssent[MAX_INPUTS], pktslost[MAX_INPUTS];
	int totpktssent, totpktslost;
	int outputq, maxqsize;
	int i, j, n; //steps = number of times run
	int pktsthistime;
	int stations[N];
	float lambda; //get lambda and qsize from user

	int steps = 100;

	int ranPicked = 0;

	float localStats[4];
	int r = -1; //indicies
	int c = -1;
	for (int i = 1; i <= 1024; i *= 2) { //# of firing nodes
		r++;
		c = -1;
		for (int j = 0; j <= 10; j += 2) { //levels
			c++;
			for (int s = 0; s < N; s++) {
				stations[s] = 0;
			}

			for (int l = 0; l < 4; l++) { //reset local stats
				localStats[l] = 0;
			}
			srandom(0);
			ranPicked = 0;
			cout << "starting test runs at level: " << j << " for " << i
					<< " nodes" << endl;
			for (int s = 0; s < steps; s++) { //100 test runs
				while (ranPicked < i) {
					//cout <<"picking random number"<<endl;
					prob = random() / MAXINT;
					int selected = (N - 1) * prob;
					if (!stations[selected]) {
						stations[selected] = 1;
						ranPicked++;
					}
				}
				int segmentSize = (j == 0) ? N : N / pow(2,j);
				for (int start = 0; start < N;) {
					for (int a = 0; a < 4; a++) { //reset global stats
						globalStats[a] = 0;
					}
					startProbe(stations, start, start + segmentSize);
					for (int b = 0; b < 4; b++) { //add results to local stats
						localStats[b] += globalStats[b];
					}
					start += segmentSize;
				}
			}
			float successRate = localStats[0]
					/ localStats[1];
			results[c][r] = successRate; //c = i (levels), r = k (firing nodes)
		}
	}

	//print out the result table

	for (int y = 0; y < 6; y++) {
		for (int x = 0; x < 11; x++) {
			cout << results[y][x] << "   ";
		}
		cout << endl;
	}

	//1. initialize Ki randomly picked nodes in stations[N]
	/*2. 100 test scenarios, for each
	 * - count total number of probes required to successfully resolve transmiss
	 * - compute percentage of probes that were successful (suc/(suc+idl+col))
	 * - take average over 100 trials
	 */
	//3. repeat above tests for all i=0,2,4...10 level
	// 4. plot results from analysis on a graph, new line for each i
	//
	//trade off is with number of idles
	//i is Layer
	//K is # of nodes stations
	//start with main function
	//recursive step, full aray and indicies
	//srand(time(NULL));
	/* seed the pseudo-random number generator */

	//prob = random() / MAXINT;
	return 0;
}
