/*

	Author:         Delano Gaskin <delanogaskin@gmail.com>
	Supported by:    Oneal Anguin <onealanguin@gmail.com>,
	Andre Graham <graham.andre.t@gmail.com>,
	Imam Idris <imamidris01@gmail.com>

	Lecturer:        Dr Simon U Ewedafe

	Program:        s_s.c

	Description:    slave program for implementation of Strassen's Matrix Multiplication Algorithm in C using PVM

	Adopted/
	Extended from:    https://github.com/welon/MatrixMultiply/blob/master/Strassen.cpp

	GitHub Account    Wei Li <liwei.day@gmail.com>
	owner:

	Influence on    The C++ implementation of the Srassen algorithm was converted to C and then parallelized
	work:

	Given an assignment to implement Strassen's algorithm for matrix multiplication, the first step was to obtain such an algorithm.
	The next phase was to convert the algorithm to its implementation platform PVM/C.
	The recursive nature of the solution required current states and child and children of children states to be monitored
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#include "pvm3.h"


int N = 0;


//matrix addition
void Matrix_Add(int n, int X[][N], int Y[][N], int Z[][N]) {
    
    int i , j = 0;
    
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            Z[i][j] = X[i][j] + Y[i][j];
        }
    }
}

//matrix subtraction
void Matrix_Sub(int n, int X[][N], int Y[][N], int Z[][N]) {
    
    int i , j = 0;
    
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            Z[i][j] = X[i][j] - Y[i][j];
        }
    }
}

//normal matrix multiplication A * B = C
void Matrix_Multiply(int A[][N], int B[][N], int C[][N]) {
    
    int i , j, t = 0;
    
    for(i = 0; i < 2; i++) {
        for(j = 0; j < 2; j++) {
            C[i][j] = 0;
            for(t = 0; t < 2; t++) {
                C[i][j] = C[i][j] + A[i][t]*B[t][j];
            }
        }
    }
}


/**
Stranssen algorithm - utilizes recursive calls
*/
void Strassen(int n, int A[][N], int B[][N], int C[][N]) {
    int A11[N][N], A12[N][N], A21[N][N], A22[N][N];
    int B11[N][N], B12[N][N], B21[N][N], B22[N][N];
    int C11[N][N], C12[N][N], C21[N][N], C22[N][N];
    int M1[N][N], M2[N][N], M3[N][N], M4[N][N], M5[N][N], M6[N][N], M7[N][N];
    int AA[N][N], BB[N][N];
    
    int M1x[N][N];
    
    int i , j = 0;
    
    int stid1 = 0;
    int stid2 = 0;
    int stid3 = 0;
    int stid4 = 0;
    int stid5 = 0;
    int stid6 = 0;
    int stid7 = 0;
    
    int rcode ,
    mtype = 1,
    half_n,
    main_flag = 0;
    
    
    
    if(n == 2) {   //2-order
        Matrix_Multiply(A, B, C);
        } else {
        //Decomposing the matrix into 8 smaller matrices
        for(i = 0; i < n / 2; i++) {
            for(j = 0; j < n / 2; j++) {
                A11[i][j] = A[i][j];
                A12[i][j] = A[i][j + n / 2];
                A21[i][j] = A[i + n / 2][j];
                A22[i][j] = A[i + n / 2][j + n / 2];
                
                B11[i][j] = B[i][j];
                B12[i][j] = B[i][j + n / 2];
                B21[i][j] = B[i + n / 2][j];
                B22[i][j] = B[i + n / 2][j + n / 2];
            }
        }
        
        
        half_n = n / 2;
        
        //M1 = (A0 + A3) × (B0 + B3)
        Matrix_Add(n / 2, A11, A22, AA);
        Matrix_Add(n / 2, B11, B22, BB);
        
        rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &stid1);
        rcode = pvm_initsend(PvmDataDefault);
        rcode = pvm_pkdouble(&N, 1, 1);
        rcode = pvm_pkdouble(&half_n, 1, 1);
        rcode = pvm_pkdouble(&AA, half_n * half_n, 1);
        rcode = pvm_pkdouble(&BB, half_n * half_n, 1);
        rcode = pvm_pkdouble(&M1x,half_n * half_n, 1);
        rcode = pvm_send(stid1, 1);
        
        
        //M2 = (A2 + A3) × B0
        Matrix_Add(n / 2, A21, A22, AA);
        
        rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &stid2);
        rcode = pvm_initsend(PvmDataDefault);
        rcode = pvm_pkdouble(&N, 1, 1);
        rcode = pvm_pkdouble(&half_n, 1, 1);
        rcode = pvm_pkdouble(&AA, half_n * half_n, 1);
        rcode = pvm_pkdouble(&B11, half_n * half_n, 1);
        rcode = pvm_pkdouble(&M2,half_n * half_n, 1);
        rcode = pvm_send(stid2, 1);
        
        
        //M3 = A0 × (B1 - B3)
        Matrix_Sub(n / 2, B12, B22, BB);
        
        rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &stid3);
        rcode = pvm_initsend(PvmDataDefault);
        rcode = pvm_pkdouble(&N, 1, 1);
        rcode = pvm_pkdouble(&half_n, 1, 1);
        rcode = pvm_pkdouble(&A11, half_n * half_n, 1);
        rcode = pvm_pkdouble(&BB, half_n * half_n, 1);
        rcode = pvm_pkdouble(&M3,half_n * half_n, 1);
        rcode = pvm_send(stid3, 1);
        
        
        //M4 = A3 × (B2 - B0)
        Matrix_Sub(n / 2, B21, B11, BB);
        
        rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &stid4);
        rcode = pvm_initsend(PvmDataDefault);
        rcode = pvm_pkdouble(&N, 1, 1);
        rcode = pvm_pkdouble(&half_n, 1, 1);
        rcode = pvm_pkdouble(&A22, half_n * half_n, 1);
        rcode = pvm_pkdouble(&BB, half_n * half_n, 1);
        rcode = pvm_pkdouble(&M4,half_n * half_n, 1);
        rcode = pvm_send(stid4, 1);
        
        
        //M5 = (A0 + A1) × B3
        Matrix_Add(n / 2, A11, A12, AA);
        
        rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &stid5);
        rcode = pvm_initsend(PvmDataDefault);
        rcode = pvm_pkdouble(&N, 1, 1);
        rcode = pvm_pkdouble(&half_n, 1, 1);
        rcode = pvm_pkdouble(&AA, half_n * half_n, 1);
        rcode = pvm_pkdouble(&B22, half_n * half_n, 1);
        rcode = pvm_pkdouble(&M5,half_n * half_n, 1);
        rcode = pvm_send(stid5, 1);
        
        
        //M6 = (A2 - A0) × (B0 + B1)
        Matrix_Sub(n / 2, A21, A11, AA);
        Matrix_Add(n / 2, B11, B12, BB);
        
        rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &stid6);
        rcode = pvm_initsend(PvmDataDefault);
        rcode = pvm_pkdouble(&N, 1, 1);
        rcode = pvm_pkdouble(&half_n, 1, 1);
        rcode = pvm_pkdouble(&AA, half_n * half_n, 1);
        rcode = pvm_pkdouble(&BB, half_n * half_n, 1);
        rcode = pvm_pkdouble(&M6,half_n * half_n, 1);
        rcode = pvm_send(stid6, 1);
        
        
        //M7 = (A1 - A3) × (B2 + B3)
        Matrix_Sub(n / 2, A12, A22, AA);
        Matrix_Add(n / 2, B21, B22, BB);
        
        rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &stid7);
        rcode = pvm_initsend(PvmDataDefault);
        rcode = pvm_pkdouble(&N, 1, 1);
        rcode = pvm_pkdouble(&half_n, 1, 1);
        rcode = pvm_pkdouble(&AA, half_n * half_n, 1);
        rcode = pvm_pkdouble(&BB, half_n * half_n, 1);
        rcode = pvm_pkdouble(&M7,half_n * half_n, 1);
        rcode = pvm_send(stid7, 1);
        
        
        
        
        
        int recv_cnt = 0; //variable used to keep track of how many slaves have processed and returned
        
        
        //this loop activates and listens using nonblocking receives to accept results from slaves
        do
        {
            rcode = pvm_nrecv(stid1, 1);          //* wait to receive message from slave
            if (rcode)
            {
                recv_cnt++;
                pvm_upkdouble(&M1, half_n * half_n,  1);   //* contents of M1
            }
            
            rcode = pvm_nrecv(stid2, 1);          //* wait to receive message from slave
            if (rcode)
            {
                recv_cnt++;
                pvm_upkdouble(&M2, half_n * half_n,  1);   //* contents of M2
            }
            
            rcode = pvm_nrecv(stid3, 1);          //* wait to receive message from slave
            if (rcode)
            {
                recv_cnt++;
                pvm_upkdouble(&M3, half_n * half_n,  1);   //* contents of M3
            }
            
            rcode = pvm_nrecv(stid4, 1);          //* wait to receive message from slave
            if (rcode)
            {
                recv_cnt++;
                pvm_upkdouble(&M4, half_n * half_n,  1);   //* contents of M4
            }
            
            rcode = pvm_nrecv(stid5, 1);          //* wait to receive message from slave
            if (rcode)
            {
                recv_cnt++;
                pvm_upkdouble(&M5, half_n * half_n,  1);   //* contents of M5
            }
            
            rcode = pvm_nrecv(stid6, 1);          //* wait to receive message from slave
            if (rcode)
            {
                recv_cnt++;
                pvm_upkdouble(&M6, half_n * half_n,  1);   //* contents of M6
            }
            
            rcode = pvm_nrecv(stid7, 1);          //* wait to receive message from slave
            if (rcode)
            {
                recv_cnt++;
                pvm_upkdouble(&M7, half_n * half_n,  1);   //* contents of M7
            }
            
        }
        while (rcode < 6);
        
        
        
        
        
        //C0 = M1 + M4 - M5 + M7
        Matrix_Add(n / 2, M1, M4, AA);
        Matrix_Sub(n / 2, M7, M5, BB);
        Matrix_Add(n / 2, AA, BB, C11);
        
        //C1 = M3 + M5
        Matrix_Add(n / 2, M3, M5, C12);
        
        //C2 = M2 + M4
        Matrix_Add(n / 2, M2, M4, C21);
        
        //C3 = M1 - M2 + M3 + M6
        Matrix_Sub(n / 2, M1, M2, AA);
        Matrix_Add(n / 2, M3, M6, BB);
        Matrix_Add(n / 2, AA, BB, C22);
        
        for(i = 0; i < n / 2; i++) {
            for(j = 0; j < n / 2; j++) {
                C[i][j] = C11[i][j];
                C[i][j + n / 2] = C12[i][j];
                C[i + n / 2][j] = C21[i][j];
                C[i + n / 2][j + n / 2] = C22[i][j];
            }
        }
    }
}


int main(int argc, char *argv[] )
{
    
    
    int current_n = 0;
    int is_called_from_main = 0;
    int use_this_n = 0;
    
    int stid = pvm_mytid();
    
    int mtype = 1;                              /* set message type */
    int mtid = pvm_parent();                    /* get task id for master process */
    int rcode = pvm_recv(-1, mtype);          /* wait to receive message from master*/
    
    rcode = pvm_upkdouble(&N, 1, 1);           /* Original matrix size */
    rcode = pvm_upkdouble(&current_n, 1, 1);           /* size of matrix being used in this slave */
    
    int A[N][N],B[N][N],C[N][N];
    
    
    rcode = pvm_upkdouble(&A, current_n * current_n, 1);           /* contents of matrix A */
    rcode = pvm_upkdouble(&B, current_n * current_n, 1);           /* contents of matrix B */
    rcode = pvm_upkdouble(&C, current_n * current_n, 1);           /* contents of matrix C */
    
    Strassen(current_n, A, B, C);
    
    //Send below return the result of computation to the task that spawned this slave
    rcode = pvm_initsend(PvmDataDefault);
    rcode = pvm_pkdouble(&C, N*N, 1);
    rcode = pvm_send(pvm_parent(), 1);
    rcode = pvm_exit();
    exit(0);
    
    return 0;
}
