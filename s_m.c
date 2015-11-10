<<<<<<< HEAD
//https://github.com/welon/MatrixMultiply/blob/master/Strassen.cpp
=======
/*

	Author:     	Delano Gaskin <delanogaskin@gmail.com>
	Supported by:   Oneal Anguin <onealanguin@gmail.com>,
					Andre Graham <graham.andre.t@gmail.com>,
					Imam Idris <imamidris01@gmail.com>

	Lecturer:    	Dr Simon U Ewedafe

	Program:    	s_m.c

	Description:    master program for implementation of Strassen's Matrix Multiplication Algorithm in C using PVM

	Adopted/
	Extended from:  https://github.com/welon/MatrixMultiply/blob/master/Strassen.cpp

	GitHub Account  Wei Li <liwei.day@gmail.com>
	owner:

	Influence on    The C++ implementation of the Srassen algorithm was converted to C and then parallelized
	work:

	Given an assignment to implement Strassen's algorithm for matrix multiplication, the first step was to obtain such an algorithm.
	The next phase was to convert the algorithm to its implementation platform PVM/C.
	The recursive nature of the solution required current states and child and children of children states to be monitored
*/

>>>>>>> matrix

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <time.h>
<<<<<<< HEAD
 
#include "pvm3.h"


 
const int N = 2; 
 
//matrix printing
void print(int n, int C[][N]) {

=======

#include "pvm3.h"


int N = 4;

//matrix printing
void print(int n, int C[][N]) {
    
>>>>>>> matrix
    int i , j = 0;
    puts("——————————————————————————");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%.4d ", C[i][j]);
        }puts("");
    }puts("");
}
<<<<<<< HEAD
 

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
=======



int main(int argc, char *argv[]) {
    
    //Argument accepted from command line, should be a number, a power of 2
    if (argc == 2)
    N = atoi(argv[1]);
    
    //Create 3 matrix containers, A * B = C
    int A[N][N],B[N][N],C[N][N];
    
    int i, j , random_num,  rcode, tid = 0;
    int mtype = 99999;
    int main_n = N; // This holds the matrix size on recursive calls as the matrix size decrease
    
    srand(time(NULL));
    
    puts("In Delano's Strassen");
    
    //A and B matrix assignment
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            
            random_num = rand();
            
            A[i][j] = random_num % 10; //Matrix element A is a random number
            
            B[i][j] =   (A[i][j] * 3) % 10;  //Matrix element B is a the modulus of A * 3 when divided by 10
            C[i][j] = A[i][j] + B[i][j] ; // C is initially set to equivalent element of A + B
        }
    }
    
    
    int mtid = pvm_mytid(); //Obtain current task id
    int main_flag = 1;
    
    printf("n masters id is %d n", mtid); //printing master task id
    
    rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &tid); // first slave spawned
    
    rcode = pvm_initsend(PvmDataDefault);
    rcode = pvm_pkdouble(&N, 1, 1); //Initial size of matrix
    rcode = pvm_pkdouble(&main_n, 1, 1);  //Size of recursive matrix size is the same as original size for first run
    
    rcode = pvm_pkdouble(&A, N*N, 1); //sending matix A
    rcode = pvm_pkdouble(&B, N*N, 1); //sending matrix B
    rcode = pvm_pkdouble(&C, N*N, 1); //sending matrix C... although not necessay, staying true to original serial algorithm
    rcode = pvm_send(tid, 1);    // sending task to slave
    
    rcode = pvm_recv(tid, 1);          /* wait to receive message from slave*/
    rcode = pvm_upkdouble(&C, N*N, 1);   /* contents of matrix C */
    
    puts("Matrix A：");
    print(N, A);    // printing matrix A
    puts("Matrix B: ");
    print(N, B);    //printing matrix B
    puts("Result of A * B = C：");
    print(N, C); //printing matrix C = A*B
    
    rcode = pvm_exit(); //Tells the local pvmd that this process is leaving PVM.
    exit(0); //cause normal process termination
    return 0; //terminates the execution of a function and returns control to the calling function
>>>>>>> matrix
}
