/*
	C Program for Travelling Salesman Problem using Dynamic Method
	Author: PracsPedia		www.pracspedia.com
*/
/*
* The proposed solution is to decompose the data based on the least portion of the code which has the loop.
*int least(int c)
*{
*	int i,nc=999;
*	int min=999,kmin;
*	for(i=0;i < n;i++)
*	{
*		if((a[c][i]!=0)&&(visited[i]==0))
*			if(a[c][i] < min)
*			{
*				min=a[i][0]+a[c][i];
*				kmin=a[c][i];
*				nc=i;
*			}
*	}
*	if(min!=999)
*		cost+=kmin;
*	return nc;
*}
* If we are able to split this work amoungest the amout of processors the the work should be faster. The least value can be
*broadcast to the master node to maintain the minimum value. When one city has finished the next city can be processed and parallize the least function
*again.
*/
#include<stdio.h>
#include<stdlib.h>
#include <time.h> 
#include "pvm3.h"
#define LEAST "tspleast"
#define AMOUNTOFCITYS 3
#define MSGTYPE 55
#define MSGTYPEFROMSLAVE 56
#define VISITEDNODES 57
#define PROCESSINGNODES 58

//int AMOUNTOFCITYS = 3;
int a[AMOUNTOFCITYS][AMOUNTOFCITYS],visited[AMOUNTOFCITYS],n,cost=0,city=0,cityXaxis[AMOUNTOFCITYS],
	cityYaxis[AMOUNTOFCITYS],cityYaxisFirstCity[AMOUNTOFCITYS],nproc,solution[AMOUNTOFCITYS],processing[AMOUNTOFCITYS];

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



int least(int c)
{
	int i,nc=999;
	int min=999,kmin;
	for(i=0;i < n;i++)
	{
		if((a[c][i]!=0)&&(visited[i]==0))
			if(a[c][i] < min)
			{
				min=a[i][0]+a[c][i];
				kmin=a[c][i];
				nc=i;
			}
	}
	if(min!=999)
		cost+=kmin;
	return nc;
}

void put()
{
	printf("\n\nMinimum cost:");
	printf("%d",cost);
}

void main()
{
	printf("I'm t%x\n", pvm_mytid());
	
	
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
	
	get();
	printf("\n\nThe Path is:\n\n");


	int i = 0;
	for(i ;i < AMOUNTOFCITYS; i++){
		setCityYAxisArray(i);
		setCityXAxisArray(i);
		city = i;

		if(i != 0){
			pvm_recv(tids[i -1],VISITEDNODES);
			pvm_upkint(visited,AMOUNTOFCITYS,1);
			pvm_upkint(processing,AMOUNTOFCITYS,1);
			int m = 0;
			printf("The visited nodes for parent are ");
			for(m; m < AMOUNTOFCITYS; m ++){printf("%d --> %d |",m,visited[m] );}
			printf("\n");

			pvm_recv(tids[i -1],PROCESSINGNODES);
			pvm_upkint(processing,AMOUNTOFCITYS,1);
			int z = 0;
			printf("\nThe nodes being processed in parent are : ");
			for(z; z < AMOUNTOFCITYS; z ++){printf("%d --> %d |",z,processing[z] );}
			printf("\n");
			
		}		

		printf("\nValue of city in the parent is %d\n",city);
	
		pvm_initsend(PvmDataDefault);

		pvm_pkint(cityYaxis,AMOUNTOFCITYS , 1);
		int x = 0;
		printf("City Y Axis in parent consists of  : ");
		for(x; x < AMOUNTOFCITYS; x++){printf("%d-->",cityYaxis[x]);}
		printf("\n");
		pvm_pkint(cityXaxis,AMOUNTOFCITYS , 1);
		printf("City X Axis in parent consists of  : ");
		int xIndex = 0;
		for(xIndex; xIndex < AMOUNTOFCITYS; xIndex++){printf("%d-->",cityXaxis[xIndex]);}
		printf("\n");

		pvm_pkint(cityYaxisFirstCity,AMOUNTOFCITYS , 1);
		printf("First City Y Axis in parent consists of  : ");
		int xFIndex = 0;
		for(xFIndex; xFIndex < AMOUNTOFCITYS; xFIndex++){printf("%d-->",cityYaxisFirstCity[xFIndex]);}
		printf("\n");

		pvm_pkint(&city,1 , 1);

		processing[i] = 1;

		pvm_send(tids[i],MSGTYPE);

		
		if(i != 0){
			pvm_initsend(PvmDataDefault);
			pvm_pkint(visited,AMOUNTOFCITYS,1);
			pvm_send(tids[i],VISITEDNODES);
			
			pvm_initsend(PvmDataDefault);
			pvm_pkint(visited,AMOUNTOFCITYS,1);
			pvm_send(tids[i],PROCESSINGNODES);
		}
		
		
	}

	/*Evaluate costs from slaves.*/
	int costFromSlave = 0;
	int cityFromSlave = 0;
	printf("%d-->",1);
	for(i =0; i < AMOUNTOFCITYS; i ++){
		pvm_recv(tids[i],MSGTYPEFROMSLAVE);
		pvm_upkint(&costFromSlave,1,1);
		pvm_upkint(&cityFromSlave,1,1);
		if(i != (AMOUNTOFCITYS -1)){
		printf("%d-->",cityFromSlave);
		}else{printf("%d",1);}
		cost += costFromSlave;
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
