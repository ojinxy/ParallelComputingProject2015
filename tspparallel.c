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

int a[25][25],visited[25],n,cost=0;

void get()
{	
	
	int i,j;
	n = 25;
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

void mincost(int city)
{
    	
    	int i,ncity;
	visited[city]=1;	
	printf("%d -->",city+1);
	ncity=least(city);
	if(ncity==999)
	{
		ncity=0;
		printf("%d",ncity+1);
		cost+=a[city][ncity];
		return;
	}
	mincost(ncity);
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
	
	
	int tid,cc;                  /* my task id */
	int tids[32];		   /* slave task ids */
    	tid = pvm_mytid();	  /* enroll in pvm */
	int nhost, narch;
	char buf[100];

	struct pvmhostinfo *hostp;

	/* Set number of slaves to start */
    	pvm_config( &nhost, &narch, &hostp );

	printf("Number of hosts available %d." , nhost);
    	
	cc = pvm_spawn(LEAST, (char**)0, 0, "", 1, &tid);

	if (cc == 1) {
		cc = pvm_recv(-1, -1);
		pvm_bufinfo(cc, (int*)0, (int*)0, &tid);
		pvm_upkstr(buf);
		printf("from t%x: %s\n", tid, buf);

	} else
		printf("can't start least\n");
    	
	printf("Number of hosts available %d." , nhost);
	
	get();
	printf("\n\nThe Path is:\n\n");
	mincost(0);
	put();
}

int randInRange(int min, int max)
{
  srand(rand());
  int returnValue = min + (int) (rand() / (double) (RAND_MAX -1) * (max - min + 1));
  srand (returnValue + time(NULL));
  return returnValue;
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
