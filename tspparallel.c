/*
	C Program for Travelling Salesman Problem using Dynamic Method
	Author: PracsPedia		www.pracspedia.com
*/
/*
* If we are able to split this work amoungest the amout of processors the the work should be faster. The least value can be
*broadcast to the master node to maintain the minimum value. When one city has finished the next city can be processed and parallize the least function
*again.
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

void get()
{	
	
	int i,j;
	n = AMOUNTOFCITYS;
	nproc = AMOUNTOFCITYS;
	printf("No. of Cities travelling salesman program is %d: ",n);

	printf("\nRandomizing Cost Matrix\n");
	for( i=0;i < n;i++)
	{
		//printf("\nEnter Elements of Row # : %d\n",i+1);
		/*Code added by OA 23 Oct 2015 - Use random numbers to generate distance between points.*/
		for( j=0;j < n;j++)
			a[i][j] = randInRange(1, 100) + i + j;
			//scanf("%d",&a[i][j]);
		visited[i]=0;
	}
	printf("\n\nThe cost list is:\n\n");
	for( i=0;i < n;i++)
	{
		printf("\n\n");
		for(j=0;j < n;j++)
			printf("\t%d",a[i][j]);
	}
}


void put()
{
	printf("\n\nMinimum cost:");
	printf("%d",cost);
}

void main()
{
	//printf("I'm t%x\n", pvm_mytid());
	
	
	int tid,cc;                 /* my task id */
	int tids[AMOUNTOFCITYS];   /* slave task ids */
    	//tid = pvm_mytid();	  /* enroll in pvm */
	int nhost, narch;
	char buf[100];

	struct pvmhostinfo *hostp;

	/* Set number of slaves to start */
    	pvm_config( &nhost, &narch, &hostp );

	printf("Number of hosts available %d." , nhost);
    	
	cc = pvm_spawn(LEAST, (char**)0, 0, "", AMOUNTOFCITYS, tids);
	
	//get();
	readCitys();
	printf("\n\nThe Path is:\n\n");

	lastVisited = -1;
	int i = 0;
	lastVisitedNodes[0] = 1;
	for(i ;i < AMOUNTOFCITYS; i++){
		city = i;
		cityIndex = i;
		if(i != 0){
			pvm_recv(tids[i -1],VISITEDNODES);
			pvm_upkint(visited,AMOUNTOFCITYS,1);
			pvm_upkint(&lastVisited,1,1);
			pvm_upkint(lastVisitedNodes,AMOUNTOFCITYS,1);

			if(lastVisited > 0){city = lastVisited;}
			
			
		}

		setCityYAxisArray(city);
		setCityXAxisArray(city);		

		//printf("\nValue of city in the parent is %d\n",city);
	
		pvm_initsend(PvmDataDefault);

		pvm_pkint(cityYaxis,AMOUNTOFCITYS , 1);
		/*int x = 0;
		printf("City Y Axis in parent consists of  : ");
		for(x; x < AMOUNTOFCITYS; x++){printf("%d-->",cityYaxis[x]);}
		printf("\n");*/
		pvm_pkint(cityXaxis,AMOUNTOFCITYS , 1);
		/*printf("City X Axis in parent consists of  : ");
		int xIndex = 0;
		for(xIndex; xIndex < AMOUNTOFCITYS; xIndex++){printf("%d-->",cityXaxis[xIndex]);}
		printf("\n");*/

		pvm_pkint(cityYaxisFirstCity,AMOUNTOFCITYS , 1);
		/*printf("First City Y Axis in parent consists of  : ");
		int xFIndex = 0;
		for(xFIndex; xFIndex < AMOUNTOFCITYS; xFIndex++){printf("%d-->",cityYaxisFirstCity[xFIndex]);}
		printf("\n");*/

		pvm_pkint(&city,1 , 1);

		pvm_pkint(&lastVisited,1,1);

		pvm_pkint(lastVisitedNodes,AMOUNTOFCITYS,1);

		pvm_pkint(&cityIndex,1,1);


		pvm_send(tids[i],MSGTYPE);

		
		if(i != 0){
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



int randInRange(int min, int max)
{
  srand(rand());
  int returnValue = min + (int) (rand() / (double) (RAND_MAX -1) * (max - min + 1));
  srand (returnValue + time(NULL));
  return returnValue;
}

setCityYAxisArray(int city){
  int indexYAxis = 0;
  for(indexYAxis; indexYAxis < AMOUNTOFCITYS; indexYAxis++){
	cityYaxis[indexYAxis] = a[indexYAxis][city];
	if(city == 0){
	   cityYaxisFirstCity[indexYAxis] = a[indexYAxis][city];	
	}
  }
  
}

setCityXAxisArray(int city){
  int i = 0;
  for(i; i < AMOUNTOFCITYS; i++){
	cityXaxis[i] = a[city][i];
  }
}


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

	//printf("The 23rd city is %d\n",citys[0][11]);

	//printf("The 23rd city name is %s\n",citynames[0]);
	int i,j;
	printf("\n\nThe cost list is:\n\n");
	for( i=0;i < AMOUNTOFCITYS;i++)
	{
		printf("\n\n");
		for(j=0;j < AMOUNTOFCITYS;j++)
			printf("\t%d",a[i][j]);
	}
}

/*

OUTPUT:
Enter No. of Cities: 4
Enter Cost Matrix
Enter Elements of Row # : 1
1
5
4
2
Enter Elements of Row # : 2
2
1
5
4
Enter Elements of Row # : 3
9
6
2
4
Enter Elements of Row # : 4
7
5
3
4
The cost list is:
	1	5	4	2

	2	1	5	4

	9	6	2	4

	7	5	3	4

The Path is:

1 –->4 -–>3 -–>2 -–>1
*/
