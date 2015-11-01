
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
#define AMOUNTOFCITYS 3
#define MSGTYPE 55
#define MSGTYPEFROMSLAVE 56
#define VISITEDNODES 57
#define PROCESSINGNODES 58

//int AMOUNTOFCITYS = 3;
int visited[AMOUNTOFCITYS],cost=0,cityYaxis[AMOUNTOFCITYS],cityYaxisFirstCity[AMOUNTOFCITYS],cityXaxis[AMOUNTOFCITYS],city=0,
	solution[AMOUNTOFCITYS],parentTid,tids[AMOUNTOFCITYS],processing[AMOUNTOFCITYS];

main()
{
	printf("Starting Main TSP Least");
	
	

	int i,ncity;
	int visited[25];
	int mytid = pvm_mytid();       /* my task id */
	parentTid = pvm_parent();
	
	pvm_recv(parentTid,MSGTYPE);
	pvm_upkint(cityXaxis,AMOUNTOFCITYS,1);
	printf("City X Axis in child consists of  : ");
	int xIndex = 0;
	for(xIndex; xIndex < AMOUNTOFCITYS; xIndex++){printf("%d-->",cityXaxis[xIndex]);}
	printf("\n");
	pvm_upkint(cityYaxis,AMOUNTOFCITYS,1);
	int x = 0;
	printf("City Y Axis in child consists of  : ");
	for(x; x < AMOUNTOFCITYS; x++){printf("%d-->",cityYaxis[x]);}
	printf("\n");
	pvm_upkint(cityYaxisFirstCity,AMOUNTOFCITYS,1);
	printf("First City Y Axis in child consists of  : ");
	int xFIndex = 0;
	for(xFIndex; xFIndex < AMOUNTOFCITYS; xFIndex++){printf("%d-->",cityYaxisFirstCity[xFIndex]);}
	printf("\n");	

	pvm_upkint(&city,1,1);

	int z = 0;
	printf("\nThe nodes being processed in child main function are : ");
	for(z; z < AMOUNTOFCITYS; z ++){printf("%d --> %d |",z,processing[z] );}
	printf("\n");
	

	printf("\nThe city being processed is %d\n", city);

	mincost(city);
	pvm_exit();

}

mincost(int city)
{
    	
    	int i,ncity;

	if(city != 0){
		printf("Unpacking Visited citys!\n");
		pvm_recv(parentTid,VISITEDNODES);
		pvm_upkint(visited,AMOUNTOFCITYS,1);

		printf("Unpacking Processed Nodes!\n");
		pvm_recv(parentTid,PROCESSINGNODES);
		pvm_upkint(processing,AMOUNTOFCITYS,1);
	}
		
	visited[city]=1;
	int m = 0;
	printf("The visited nodes for child are ");
	for(m; m < AMOUNTOFCITYS; m ++){printf("%d --> %d |",m,visited[m] );}
	printf("\n");
	//printf("The city is : %d",city+1);
	printf("Processing City");
	//printf("The cost is %d\n", cost);
	//printf("%d -->",city+1);
	solution[city] = city + 1;
	ncity=least(city);
	//printf("\nThe least city is %d\n",ncity);
	
	
	if(ncity==999)
	{
		ncity=0;
		printf("finished");
		cost+= cityYaxis[ncity];
		//return;
	}
	//mincost(ncity);
	
	
	pvm_initsend(PvmDataDefault);
	pvm_pkint(visited,AMOUNTOFCITYS,1);
	pvm_send(parentTid,VISITEDNODES);

	pvm_initsend(PvmDataDefault);
	processing[city] = 0;
	pvm_pkint(processing,AMOUNTOFCITYS,1);
	pvm_send(parentTid,PROCESSINGNODES);

}

int least()
{
	int i,nc=999;
	int min=999,kmin;
	for(i=0;i < AMOUNTOFCITYS;i++)
	{
		printf("City at y axis %d",cityYaxis[i]);
		if((cityYaxis[i]!=0)&&(checkIfCanidateForSelection(i) == 1))
			if(cityYaxis[i] < min)
			{
				min=cityYaxisFirstCity[i]+cityYaxis[i];
				kmin=cityYaxis[i];
				nc=i;
			}
	}
	if(min!=999)
		cost+=kmin;

	pvm_initsend(PvmDataDefault);
	int costFromSlave;
	int cityFromSlave;
	if(nc != 999){
		costFromSlave = kmin;
		cityFromSlave = nc + 1;
	}else{costFromSlave =0;cityFromSlave=1;}
	pvm_pkint(&costFromSlave,1,1);
	pvm_pkint(&cityFromSlave,1,1);
	pvm_send(parentTid,MSGTYPEFROMSLAVE);

	return nc;
}

int checkIfCanidateForSelection(int i){
	int candidate = 0;

	if(visited[i] == 0 && city != i){
		candidate = 1;
	}

	if(visited[i] == 1 && processing[i] == 1 && city != i){
		candidate = 1;
	}

	int m = 0;
	printf("\nThe nodes being processed in child are : ");
	for(m; m < AMOUNTOFCITYS; m ++){printf("%d --> %d |",m,processing[m] );}
	printf("\n");
	
	printf("\nThe city{%d} for index i {%d} had candidate value of {%d}\n",city,i,candidate);

	return candidate;
}
