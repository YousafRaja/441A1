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
#include <stdio.h>

#define MAX_INPUTS 4
#define MAXINT 2147483647.0

int random();
int main() {                                                                       
	//N=8             N=1024                   i\K|1 2 ... 1024    Success rate--------------->   |
	//K=3             K=1,2,4,...,1024         0    <---------- avg success rate of 100 runs      | 
	//i=0             i=0,2,4,6,8,10           2                                                  |
	//Step=1          Steps=100                4                                                 |__________1024K__ need to see increase or decrease            
	int inpkt[MAX_INPUTS], pktssent[MAX_INPUTS], pktslost[MAX_INPUTS];
	int totpktssent, totpktslost;
	int outputq, maxqsize;
	int i, j, n, steps; //steps = number of times run
	int pktsthistime;
	float lambda; //get lambda and qsize from user
	float prob;

	//initialize random 

	//trade off is with number of idles 
	//i is Layer
	//K is # of nodes stations
	//start with main function 
	//recursive step, full aray and indicies

	//srand(time(NULL));

	srand(static_cast <unsigned> (time(0)));

	//prob = random() / MAXINT;
	return 0;
}