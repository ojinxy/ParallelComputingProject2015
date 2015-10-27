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
#define SLAVENAME "slave1"
//#include<conio.h>
int a[10][10],visited[10],n,cost=0;

void get()
{	
	
	int i,j;
	//printf("Enter No. of Cities: ");
	//scanf("%d",&n);
	n = 35;
	printf("No. of Cities travelling salesman program is %d: ",n);
	//printf("\nEnter Cost Matrix\n");
	printf("\nRandomizing Cost Matrix\n");
	for( i=0;i < n;i++)
	{
		//printf("\nEnter Elements of Row # : %d\n",i+1);
		/*Code added by OA 23 Oct 2015 - Use random numbers to generate distance between points.*/
		for( j=0;j < n;j++)
			a[i][j] = randInRange(1,100);
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
	//clrscr();
	get();
	printf("\n\nThe Path is:\n\n");
	mincost(0);
	put();
	//getch();
}

int randInRange(int min, int max)
{
  srand(rand());
  int returnValue = min + (int) (rand() / (double) (RAND_MAX -1) * (max - min + 1));
  srand (returnValue + time(NULL));
  return returnValue;
}

/* Returns an integer in the range [0, n).
 *
 * Uses rand(), and so is affected-by/affects the same seed.
 */
int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Chop off all of the values that would cause skew...
    long end = RAND_MAX / n; // truncate skew

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
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
