#include<stdio.h>
#include<stdlib.h>
#include "/home/oneal/pvm3/include/pvm3.h"

#define SIZEOFMATRIX 2
#define AMOUNTOFTASKS 7
#define SLAVE "strassen"
#define MESSAGETYPE_CALCUATE 55
#define MESSAGETYPE_RESULTS 54

int matrixa[SIZEOFMATRIX][SIZEOFMATRIX], matrixb[SIZEOFMATRIX][SIZEOFMATRIX];
int tid, tids[AMOUNTOFTASKS],nhost,narch,Q[AMOUNTOFTASKS],matrixc[SIZEOFMATRIX][SIZEOFMATRIX];

void setUpMatrix(){
	int i,j;	
	for(i = 0;i < SIZEOFMATRIX; i++){
		for(j = 0;j < SIZEOFMATRIX; j++){
			matrixa[i][j] = randInRange(0,100);
			matrixb[i][j] = randInRange(0,100);
		}
	}
}

void main(){
	printf("\nStarting program\n");
	/*Set up random values in matrix*/
	setUpMatrix();

	/*Print out matrix A and B*/
	printmatrix(matrixa,"A");
	printmatrix(matrixb, "B");

	/*Configure PVM to do tasks.*/
	struct pvmhostinfo *hostp;
	
	/* Set number of slaves to start */
    	pvm_config( &nhost, &narch, &hostp );

	printf("\nNumber of hosts available %d.\n" , nhost);

	pvm_spawn(SLAVE, (char**)0, 0, "", AMOUNTOFTASKS, tids);
	
	/*int value1 = 0;
	int value2 = 0;
	int value3 = 0;
	int value4 = 0;
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&value1,1,1);
	pvm_pkint(&value2,1,1);
	pvm_pkint(&value3,1,1);
	pvm_pkint(&value4,1,1);
	pvm_pkint(&value4,1,1);
	pvm_send(tids[0],MESSAGETYPE_CALCUATE);*/
	
	//getQn(0);
	
	int index;
	int value1 = 0;		
	int value2 = 0;
	int value3 = 0;
	int value4 = 0;
	for(index =0; index < AMOUNTOFTASKS; index++){
		
		if(index == 0){
		value1 = matrixa[0][0];
		value2 = matrixb[0][1];
		value3 = matrixb[1][1];
		value4 = 0;
		}else if(index == 1){
			value1 = matrixa[0][0];
			value2 = matrixa[0][1];
			value3 = matrixb[1][1];
			value4 = 0;
		}else if(index == 2){
			value1 = matrixa[1][0];
			value2 = matrixa[1][1];
			value3 = matrixb[0][0];
			value4 = 0;
		}else if(index == 3){
			value1 = matrixa[1][1];
			value2 = matrixb[1][0];
			value3 = matrixb[0][0];
			value4 = 0;
		}else if(index == 4){
			value1 = matrixa[0][0];
			value2 = matrixa[1][1];
			value3 = matrixb[0][0];
			value4 = matrixb[1][1];
		}else if(index == 5){
			value1 = matrixa[0][1];
			value2 = matrixa[1][1];
			value3 = matrixb[1][0];
			value4 = matrixb[1][1];
		}else if(index == 6){
			value1 = matrixa[0][0];
			value2 = matrixa[1][0];
			value3 = matrixb[0][0];
			value4 = matrixb[0][1];
		}
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&value1,1,1);
		pvm_pkint(&value2,1,1);
		pvm_pkint(&value3,1,1);
		pvm_pkint(&value4,1,1);
		pvm_pkint(&index,1,1);
		pvm_send(tids[index],MESSAGETYPE_CALCUATE);
	}

	/*Get Results from the slave nodes*/
	int indexResults = 0;
	for(indexResults; indexResults < AMOUNTOFTASKS; indexResults ++){
		pvm_recv(tids[indexResults],MESSAGETYPE_RESULTS);
		pvm_upkint(&Q[indexResults],AMOUNTOFTASKS,1);
	}

	/*Calculate Results in C matrix*/
	matrixc[0][0] = Q[4] + Q[3] - Q[1] + Q[5];
	matrixc[1][0] = Q[2] + Q[3];
	matrixc[0][1] = Q[0] + Q[1];
	matrixc[1][1] = Q[0] + Q[4] - Q[2] - Q[6];

	/*print result*/
	printmatrix(matrixc, "C");
	pvm_exit();
	
}

/*This function will be called recursively several times to come up with
	Q1..Q2...Q7*/
int getQn(int index){
	printf("\nIn getQN Function with index = %d\n",index);
	if(index < 7){
		return 0;
	}

	int value1 = 0;
	int value2 = 0;
	int value3 = 0;
	int value4 = 0;

	if(index == 0){
		value1 = matrixa[0][0];
		value2 = matrixb[0][1];
		value3 = matrixb[1][1];
		value4 = 0;
	}else if(index == 1){
		value1 = matrixa[0][0];
		value2 = matrixa[0][1];
		value3 = matrixb[1][1];
		value4 = 0;
	}else if(index == 2){
		value1 = matrixa[1][0];
		value2 = matrixa[1][1];
		value3 = matrixb[0][0];
		value4 = 0;
	}else if(index == 3){
		value1 = matrixa[1][1];
		value2 = matrixb[1][0];
		value3 = matrixb[0][0];
		value4 = 0;
	}else if(index == 4){
		value1 = matrixa[0][0];
		value2 = matrixa[1][1];
		value3 = matrixb[0][0];
		value4 = matrixb[1][1];
	}else if(index == 5){
		value1 = matrixa[0][1];
		value2 = matrixa[1][1];
		value3 = matrixb[1][0];
		value4 = matrixb[1][1];
	}else if(index == 6){
		value1 = matrixa[0][0];
		value2 = matrixa[1][0];
		value3 = matrixb[0][0];
		value4 = matrixb[0][1];
	}

	/*Send Data to Salve node*/
	pvm_initsend(PvmDataDefault);
	
	pvm_pkint(&value1,1,1);
	pvm_pkint(&value2,1,1);
	pvm_pkint(&value3,1,1);
	pvm_pkint(&value4,1,1);
	pvm_pkint(&index,1,1);

	pvm_send(tids[index],MESSAGETYPE_CALCUATE);
	
	getQn(index + 1);

}


int randInRange(int min, int max)
{
  srand(rand());
  int returnValue = min + (int) (rand() / (double) (RAND_MAX -1) * (max - min + 1));
  srand (returnValue + time(NULL));
  return returnValue;
}

int printmatrix(int matrixtoprint[SIZEOFMATRIX][SIZEOFMATRIX], char matrixname[10]){
	int i =0,j=0;	
	printf("\nMatrix %s:",matrixname);
	for( i=0;i < SIZEOFMATRIX;i++)
	{	
		printf("\n");
		for(j=0;j < SIZEOFMATRIX;j++)
			printf("\t%d",matrixtoprint[i][j]);
	}
}
