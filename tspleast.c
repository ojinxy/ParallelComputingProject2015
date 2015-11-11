/*

	Author:     			Oneal Anguin <onealanguin@gmail.com>,
	Supported by:   		Delano Gaskin <delanogaskin@gmail.com>
					Andre Graham <graham.andre.t@gmail.com>,
					Imam Idris <imamidris01@gmail.com>

	Lecturer:    	Dr Simon U Ewedafe

	Program:    	tspserial.c
	
	Code Repository: http://github.com/ojinxy/ParallelComputingProject2015

	Description:    Master program for implementation of Travelling Salesman in C using PVM

	Adopted/
	Extended from:  http://www.pracspedia.com/AOA/tsp.html

	Influence on    The C implementation of the Traveling Salesman Problem was parallelized
	work:

	Given an assignment to implement Travelling Salesman algorithm for matrix multiplication, the first step was to obtain such an algorithm.
	The next phase was to convert the algorithm to its implementation platform PVM/C.
	The dependancy of the child nodes needing to know what the last selected least value was forced the Master to pass information recieved from
	childred to other children.
*/

#include <stdio.h>
#ifdef HASSTDLIB
#include <stdlib.h>
#endif
#ifdef	SYSVSTR
#include <string.h>
#else
#include <strings.h>
#endif
#include "pvm3.h"
#define AMOUNTOFCITYS 25
#define MSGTYPE 55
#define MSGTYPEFROMSLAVE 56
#define VISITEDNODES 57

int visited[AMOUNTOFCITYS],cost=0,cityYaxis[AMOUNTOFCITYS],cityYaxisFirstCity[AMOUNTOFCITYS],cityXaxis[AMOUNTOFCITYS],city=0,
	solution[AMOUNTOFCITYS],parentTid,tids[AMOUNTOFCITYS],lastVisited,cityIndex,lastVisitedNodes[AMOUNTOFCITYS];

main()
{
	printf("Starting Main TSP Least");
	

	int i,ncity;
	int visited[25];
	int mytid = pvm_mytid();       /* my task id */
	parentTid = pvm_parent();
	
	/*Recieve Data from master*/
	pvm_recv(parentTid,MSGTYPE);
	pvm_upkint(cityYaxis,AMOUNTOFCITYS,1);
	pvm_upkint(cityXaxis,AMOUNTOFCITYS,1);

	pvm_upkint(cityYaxisFirstCity,AMOUNTOFCITYS,1);

	pvm_upkint(&city,1,1);

	pvm_upkint(&lastVisited,1,1);
	pvm_upkint(lastVisitedNodes,AMOUNTOFCITYS,1);
	pvm_upkint(&cityIndex,1,1);

	mincost(city);
	pvm_exit();

}

mincost(int city)
{
    	
    	int i,ncity;

	if(city != 0){
		/*If this is not the first city get the last selected city from the master*/
		pvm_recv(parentTid,VISITEDNODES);
		pvm_upkint(visited,AMOUNTOFCITYS,1);

	}
		
	visited[city]=1;

	solution[city] = city + 1;
	ncity=least(city);
	
	
	if(ncity==999)
	{
		ncity=0;
		cost+= cityYaxis[ncity];
	}
	
	/*Send the last visted and the visited citys to the master*/
	lastVisited = ncity;
	lastVisitedNodes[lastVisited] = 1;
	pvm_initsend(PvmDataDefault);
	pvm_pkint(visited,AMOUNTOFCITYS,1);
	pvm_pkint(&lastVisited,1,1);
	pvm_pkint(lastVisitedNodes,AMOUNTOFCITYS,1);
	pvm_send(parentTid,VISITEDNODES);


}

/*The least function will get the city the the lowest cost which has not yet been selected for the city X Axis*/
int least()
{
	int i,nc=999;
	int min=999,kmin;


	for(i=0;i < AMOUNTOFCITYS;i++)
	{
		
		if((cityXaxis[i]!=0)&&(checkIfCanidateForSelection(i) == 1))
		{
			
			if(cityXaxis[i] < min)
			{

				min=cityXaxis[i];
				if(cityIndex < (AMOUNTOFCITYS -1)){
					kmin=cityXaxis[i];
				}else{kmin=cityXaxis[0];}
				nc=i;
			}
		}
	}
	if(min!=999)
		cost+=kmin;

	pvm_initsend(PvmDataDefault);
	int costFromSlave = 0;
	int cityFromSlave = 0;
	if(nc != 999){
		costFromSlave = kmin;
		cityFromSlave = nc;
	}else{costFromSlave =cityXaxis[0];cityFromSlave=1;}
	
	/*Send selected city and cost to the master*/
	pvm_pkint(&costFromSlave,1,1);
	pvm_pkint(&cityFromSlave,1,1);
	pvm_send(parentTid,MSGTYPEFROMSLAVE);

	return nc;
}

int checkIfCanidateForSelection(int i){
	int candidate = 0;


	if(visited[i] == 0 && city != i){
		candidate = 1;
	} else if(lastVisitedNodes[i] != 1){
		candidate = 1;
	}

	return candidate;
}
