//https://github.com/welon/MatrixMultiply/blob/master/Strassen.cpp

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <time.h>
 
#include "pvm3.h"


 
const int N = 2; 
 
//matrix printing
void print(int n, int C[][N]) {

    int i , j = 0;
    puts("——————————————————————————");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%.4d ", C[i][j]);
        }puts("");
    }puts("");
}
 

int randInRange(int min, int max)
{
  srand(rand());
  int returnValue = min + (int) (rand() / (double) (RAND_MAX -1) * (max - min + 1));
  srand (returnValue + time(NULL));
  return returnValue;
}

int main() {
    int A[N][N],B[N][N],C[N][N];

	int i, j , random_num,  rcode, tid = 0;
	int mtype = 1;


	srand(time(NULL));
//	int r = rand();

    puts("In Delano's Strassen");

	  

    //A and B matrix assignment 
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
	   
	    
            random_num = rand();

            A[i][j] = random_num % 10;

            B[i][j] =   (A[i][j] * 3) % 10;
            C[i][j] = A[i][j] + B[i][j] ;
        }
    }
 

	int mtid = pvm_mytid();


	rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &tid);
	
	rcode = pvm_initsend(PvmDataDefault);
	rcode = pvm_pkdouble(&N, 1, 1); 
	rcode = pvm_pkdouble(&A, N*N, 1); 
	rcode = pvm_pkdouble(&B, N*N, 1); 
	rcode = pvm_pkdouble(&C, N*N, 1); 
	rcode = pvm_send(tid, mtype);

	rcode = pvm_recv(-1, -1);          /* wait to receive message from slave*/	
	rcode = pvm_upkdouble(&C, N*N, 1);   /* contents of matrix B */	
	
    puts("Matrix A：");
    print(N, A);
    puts("Matrix B: ");
    print(N, B);
    puts("Result of A * B = C：");
    print(N, C);

	rcode = pvm_exit();
    return 0;
}
