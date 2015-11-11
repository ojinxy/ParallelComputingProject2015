/*

	Author:     			Oneal Anguin <onealanguin@gmail.com>,
	Supported by:   		Delano Gaskin <delanogaskin@gmail.com>
					Andre Graham <graham.andre.t@gmail.com>,
					Imam Idris <imamidris01@gmail.com>

	Lecturer:    	Dr Simon U Ewedafe

	Program:    	tspparallel.c

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
#include<stdio.h>
#include<stdlib.h>
#include <time.h> 
#include "pvm3.h"
#include <string.h>
#define LEAST "tspleast"
#define AMOUNTOFCITYS 25
#define MSGTYPE 55
#define MSGTYPEFROMSLAVE 56
#define VISITEDNODES 57


int a[AMOUNTOFCITYS][AMOUNTOFCITYS],visited[AMOUNTOFCITYS],n,cost=0,city=0,cityXaxis[AMOUNTOFCITYS],
	cityYaxis[AMOUNTOFCITYS],cityYaxisFirstCity[AMOUNTOFCITYS],nproc,solution[AMOUNTOFCITYS],
	lastVisited,cityIndex,lastVisitedNodes[AMOUNTOFCITYS];
char citynames[25][50];

/*Print out the total cost.*/
void put()
{
	printf("\n\nMinimum cost:");
	printf("%d",cost);
}

void main()
{

	int tid,cc;                 /* my task id */
	int tids[AMOUNTOFCITYS];   /* slave task ids */
    	tid = pvm_mytid();	  /* enroll in pvm */
	int nhost, narch;
	char buf[100];

	struct pvmhostinfo *hostp;

	/* Set number of slaves to start */
    	pvm_config( &nhost, &narch, &hostp );

	printf("Number of hosts available %d." , nhost);
    	
	cc = pvm_spawn(LEAST, (char**)0, 0, "", AMOUNTOFCITYS, tids);
	
	readCitys();
	printf("\n\nThe Path is:\n\n");

	lastVisited = -1;
	int i = 0;
	lastVisitedNodes[0] = 1;
	for(i ;i < AMOUNTOFCITYS; i++){
		city = i;
		cityIndex = i;
		if(i != 0){
			/*After the first city has been found its value has to be collected and passed to subsequent children
			  this needs to be done in order for the same values not to be selected.*/
			pvm_recv(tids[i -1],VISITEDNODES);
			pvm_upkint(visited,AMOUNTOFCITYS,1);
			pvm_upkint(&lastVisited,1,1);
			pvm_upkint(lastVisitedNodes,AMOUNTOFCITYS,1);

			if(lastVisited > 0){city = lastVisited;}
			
			
		}

		setCityYAxisArray(city);
		setCityXAxisArray(city);		
	
		pvm_initsend(PvmDataDefault);

		pvm_pkint(cityYaxis,AMOUNTOFCITYS , 1);

		pvm_pkint(cityXaxis,AMOUNTOFCITYS , 1);

		pvm_pkint(cityYaxisFirstCity,AMOUNTOFCITYS , 1);

		pvm_pkint(&city,1 , 1);

		pvm_pkint(&lastVisited,1,1);

		pvm_pkint(lastVisitedNodes,AMOUNTOFCITYS,1);

		pvm_pkint(&cityIndex,1,1);


		pvm_send(tids[i],MSGTYPE);

		
		if(i != 0){
			/*Pass values to children nodes to be used to prevent duplicates.*/
			pvm_initsend(PvmDataDefault);
			pvm_pkint(visited,AMOUNTOFCITYS,1);
			pvm_send(tids[i],VISITEDNODES);
			
		}
		
		
	}

	/*Evaluate costs from slaves.*/
	int costFromSlave = 0;
	int cityFromSlave = 0;
	printf("%s-->",citynames[0]);
	for(i =0; i < AMOUNTOFCITYS; i ++){
		pvm_recv(tids[i],MSGTYPEFROMSLAVE);
		pvm_upkint(&costFromSlave,1,1);
		pvm_upkint(&cityFromSlave,1,1);
		if(i != (AMOUNTOFCITYS -1)){
		printf("%s-->",citynames[cityFromSlave]);
		}else{printf("%s",citynames[0]);}
		cost = cost + costFromSlave;
	}

	put();
	pvm_exit();
}

/*Get Y axis for City*/
setCityYAxisArray(int city){
  int indexYAxis = 0;
  for(indexYAxis; indexYAxis < AMOUNTOFCITYS; indexYAxis++){
	cityYaxis[indexYAxis] = a[indexYAxis][city];
	if(city == 0){
	   cityYaxisFirstCity[indexYAxis] = a[indexYAxis][city];	
	}
  }
  
}

/*Get X axis for City*/
setCityXAxisArray(int city){
  int i = 0;
  for(i; i < AMOUNTOFCITYS; i++){
	cityXaxis[i] = a[city][i];
  }
}


/*Read City Values for CSV file (TSPData.csv). This file needs to be stored in the users home directory. */
readCitys(){
	char buf[1024];
	int lineCount = 0;
	FILE *file=fopen("TSPData.csv","r");

	if(!file){
		printf("Cannot open file.\n");
		exit(EXIT_FAILURE);
	}
	
	char * stringToken;
	while(fgets(buf, sizeof buf,file) != NULL){
		//printf("\nBUFF %s\n",buf);
		
		
		stringToken = strtok(buf,",");
		strcpy(citynames[lineCount],stringToken);
		int cityCount = 0;
		while(stringToken != NULL){
			
			stringToken = strtok(NULL,",");
			//printf("\nThe token is %s\n",stringToken);
			if(stringToken != NULL){
				a[lineCount][cityCount] = atoi(stringToken);
			}
			cityCount = cityCount + 1;
			
		}

		lineCount = lineCount + 1;
	
	}

	int i,j;
	printf("\n\nThe cost list is:\n\n");
	for( i=0;i < AMOUNTOFCITYS;i++)
	{
		printf("\n\n");
		for(j=0;j < AMOUNTOFCITYS;j++)
			printf("\t%d",a[i][j]);
	}
}

/*******************************End of Program************************************************/
