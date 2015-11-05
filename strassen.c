#include<stdio.h>
#include<stdlib.h>
#include "/home/oneal/pvm3/include/pvm3.h"

#define MESSAGETYPE_CALCUATE 55
#define MESSAGETYPE_RESULTS 54

int value1 = 0,value2 = 0,value3 = 0,value4 = 0,currentindex = 0, Q = 0;

void main(){
	printf("\nIn Slave!!!!\n");
	/*Get parent TID*/
	int parenttid = pvm_parent();
	int mytid = pvm_mytid();

	printf("\nParent TID %d\n",parenttid);
	printf("\nMy TID %d\n",mytid);

	/*Get data from master*/
	pvm_recv(parenttid,MESSAGETYPE_CALCUATE);

	/*Unpack data being send from parent*/
	pvm_upkint(&value1,1,1);
	pvm_upkint(&value2,1,1);
	pvm_upkint(&value3,1,1);
	pvm_upkint(&value4,1,1);
	pvm_upkint(&currentindex,1,1);

	if(currentindex == 0){
		Q = (value1) * (value2 - value3);
	}else if(currentindex == 1){
		Q = (value1 + value2) * (value3);
	}else if(currentindex == 2){
		Q = (value1 + value2) * (value3);
	}else if(currentindex == 3){
		Q = (value1) * (value2 - value3);
	}else if(currentindex == 4){
		Q = (value1 + value2) * (value3 + value4);
	}else if(currentindex == 5){
		Q = (value1 - value2) * (value3 + value4);
	}else if(currentindex == 6){
		Q = (value1 - value2) * (value3 + value4);
	}
	

	/*Return results to the master*/
	pvm_initsend(PvmDataDefault);

	pvm_pkint(&Q,1,1);
	
	pvm_send(parenttid,MESSAGETYPE_RESULTS);

	pvm_exit();	
}
