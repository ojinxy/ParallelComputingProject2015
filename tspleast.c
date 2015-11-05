
static char rcsid[] =
	"$Id: hello_other.c,v 1.3 2009/01/23 01:12:48 pvmsrc Exp $";



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

//int AMOUNTOFCITYS = 3;
int visited[AMOUNTOFCITYS],cost=0,cityYaxis[AMOUNTOFCITYS],cityYaxisFirstCity[AMOUNTOFCITYS],cityXaxis[AMOUNTOFCITYS],city=0,
	solution[AMOUNTOFCITYS],parentTid,tids[AMOUNTOFCITYS],lastVisited,cityIndex,lastVisitedNodes[AMOUNTOFCITYS];

main()
{
	printf("Starting Main TSP Least");
	
	

	int i,ncity;
	int visited[25];
	int mytid = pvm_mytid();       /* my task id */
	parentTid = pvm_parent();
	
	pvm_recv(parentTid,MSGTYPE);
	pvm_upkint(cityYaxis,AMOUNTOFCITYS,1);
	/*printf("City X Axis in child consists of  : ");
	int xIndex = 0;
	for(xIndex; xIndex < AMOUNTOFCITYS; xIndex++){printf("%d-->",cityXaxis[xIndex]);}
	printf("\n");*/
	pvm_upkint(cityXaxis,AMOUNTOFCITYS,1);
	/*int x = 0;
	printf("City Y Axis in child consists of  : ");
	for(x; x < AMOUNTOFCITYS; x++){printf("%d-->",cityYaxis[x]);}
	printf("\n");*/
	pvm_upkint(cityYaxisFirstCity,AMOUNTOFCITYS,1);
	/*printf("First City Y Axis in child consists of  : ");
	int xFIndex = 0;
	for(xFIndex; xFIndex < AMOUNTOFCITYS; xFIndex++){printf("%d-->",cityYaxisFirstCity[xFIndex]);}
	printf("\n");*/	

	pvm_upkint(&city,1,1);

	pvm_upkint(&lastVisited,1,1);
	pvm_upkint(lastVisitedNodes,AMOUNTOFCITYS,1);
	pvm_upkint(&cityIndex,1,1);

	int m = 0;
	/*printf("\nThe visited nodes are : ");
	for(m; m < AMOUNTOFCITYS; m ++){printf("%d --> %d |",m,lastVisitedNodes[m] );}
	printf("\n");*/
	//printf("\nThe last visited node from child is %d\n",lastVisited);

	//int z = 0;
	/*printf("\nThe nodes being processed in child main function are : ");
	for(z; z < AMOUNTOFCITYS; z ++){printf("%d --> %d |",z,processing[z] );}
	printf("\n");*/
	

	//printf("\nThe city being processed is %d\n", city);

	mincost(city);
	pvm_exit();

}

mincost(int city)
{
    	
    	int i,ncity;

	if(city != 0){
		//printf("Unpacking Visited citys!\n");
		pvm_recv(parentTid,VISITEDNODES);
		pvm_upkint(visited,AMOUNTOFCITYS,1);

	}
		
	visited[city]=1;
	int m = 0;
	//printf("The visited nodes for child are ");
	//for(m; m < AMOUNTOFCITYS; m ++){printf("%d --> %d |",m,visited[m] );}
	printf("\n");
	//printf("The city is : %d",city+1);
	//printf("Processing City");
	//printf("The cost is %d\n", cost);
	//printf("%d -->",city+1);
	solution[city] = city + 1;
	ncity=least(city);
	//printf("\nThe least city is %d\n",ncity);
	
	
	if(ncity==999)
	{
		ncity=0;
		//printf("finished");
		cost+= cityYaxis[ncity];
		//return;
	}
	//mincost(ncity);
	
	lastVisited = ncity;
	lastVisitedNodes[lastVisited] = 1;
	pvm_initsend(PvmDataDefault);
	pvm_pkint(visited,AMOUNTOFCITYS,1);
	pvm_pkint(&lastVisited,1,1);
	pvm_pkint(lastVisitedNodes,AMOUNTOFCITYS,1);
	pvm_send(parentTid,VISITEDNODES);


}

int least()
{
	int i,nc=999;
	int min=999,kmin;

	/*for(i=0;i < n;i++)
	{
		if((a[c][i]!=0)&&(visited[i]==0))
			if(a[c][i] < min)
			{
				min=a[i][0]+a[c][i];
				kmin=a[c][i];
				nc=i;
			}
	}*/
	for(i=0;i < AMOUNTOFCITYS;i++)
	{
		//printf("City at y axis %d",cityYaxis[i]);
		if((cityXaxis[i]!=0)&&(checkIfCanidateForSelection(i) == 1))
		{
			
			if(cityXaxis[i] < min)
			{
				//min=cityYaxisFirstCity[i]+cityXaxis[i];
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
		cityFromSlave = nc + 1;
	}else{costFromSlave =cityXaxis[0];cityFromSlave=1;}
	//printf("\nCost %d being returned for city %d with city index of %d\n",costFromSlave,cityFromSlave,cityIndex);
	pvm_pkint(&costFromSlave,1,1);
	pvm_pkint(&cityFromSlave,1,1);
	pvm_send(parentTid,MSGTYPEFROMSLAVE);

	return nc;
}

int checkIfCanidateForSelection(int i){
	int candidate = 0;
	/*printf("\nCity being processed %d\n",city);
	printf("Checking if City %d is a candidate for selection.\n",i);
	printf("Last visited is %d.\n",lastVisited);
	printf("Visted is %d\n",visited[i]);
	printf("Processing is %d\n",processing[i]);*/


	if(visited[i] == 0 && city != i){
		candidate = 1;
	} else if(lastVisitedNodes[i] != 1){
		candidate = 1;
	}

	/*int m = 0;
	printf("\nThe nodes being processed in child are : ");
	for(m; m < AMOUNTOFCITYS; m ++){printf("%d --> %d |",m,processing[m] );}
	printf("\n");*/
	
	/*printf("\nThe city{%d} for index i {%d} had candidate value of {%d}\n",city,i,candidate);*/
	//printf("Candidate for Selection %d\n",candidate);
	return candidate;
}
