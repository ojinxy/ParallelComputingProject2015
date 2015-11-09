//https://github.com/welon/MatrixMultiply/blob/master/Strassen.cpp

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
 
#include "pvm3.h"


int N = 0; 


//matrix printing
void print(int n, int C[][N]) {

    int i , j = 0;
    puts("——————————— SLAVE ————————");
    puts("——————————————————————————");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("%.4d ", C[i][j]);
        }puts("");
    }puts("");
}
 
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
 
 	
    int i , j = 0;
    
    int tid1 , tid2 , tid3 , tid4 , tid5 ,tid6 , tid7, rcode, mtype;


    tid1 = tid2 = tid3 = tid4 = tid5 = tid6 = tid7 = 0;

    if(n == 2) {   //2-order
        Matrix_Multiply(A, B, C);
    } else {
        //The matrices A and B into the same order of four sub- matrix, that ideological divide and conquer .
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
        
        int half_n = n / 2;
 
        //M1 = (A0 + A3) × (B0 + B3)
        Matrix_Add(n / 2, A11, A22, AA);
        Matrix_Add(n / 2, B11, B22, BB);
//        Strassen(n / 2, AA, BB, M1);

	rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &tid1);
	
	rcode = pvm_initsend(PvmDataDefault);
	rcode = pvm_pkint(&half_n, 1, 1); 
	rcode = pvm_pkdouble(&AA, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&BB, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&M1, half_n * half_n, 1); 
	rcode = pvm_send(tid1, mtype);
	
	
 
        //M2 = (A2 + A3) × B0
        Matrix_Add(n / 2, A21, A22, AA);
//        Strassen(n / 2, AA, B11, M2);
	rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &tid2);
	
	rcode = pvm_initsend(PvmDataDefault);
	rcode = pvm_pkint(&half_n, 1, 1); 
	rcode = pvm_pkdouble(&AA, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&B11, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&M2, half_n * half_n, 1); 
	rcode = pvm_send(tid2, mtype);

 
        //M3 = A0 × (B1 - B3)
        Matrix_Sub(n / 2, B12, B22, BB);
//        Strassen(n / 2, A11, BB, M3);
 	rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &tid3);
	
	rcode = pvm_initsend(PvmDataDefault);
	rcode = pvm_pkint(&half_n, 1, 1); 
	rcode = pvm_pkdouble(&A11, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&BB, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&M3, half_n * half_n, 1); 
	rcode = pvm_send(tid3, mtype);

        //M4 = A3 × (B2 - B0)
        Matrix_Sub(n / 2, B21, B11, BB);
//        Strassen(n / 2, A22, BB, M4);
	rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &tid4);
	
	rcode = pvm_initsend(PvmDataDefault);
	rcode = pvm_pkint(&half_n, 1, 1); 
	rcode = pvm_pkdouble(&A22, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&BB, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&M4, half_n * half_n, 1); 
	rcode = pvm_send(tid4, mtype);

 
        //M5 = (A0 + A1) × B3
        Matrix_Add(n / 2, A11, A12, AA);
//        Strassen(n / 2, AA, B22, M5);
	rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &tid5);
	
	rcode = pvm_initsend(PvmDataDefault);
	rcode = pvm_pkint(&half_n, 1, 1); 
	rcode = pvm_pkdouble(&AA, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&B22, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&M5, half_n * half_n, 1); 
	rcode = pvm_send(tid5, mtype);

 
        //M6 = (A2 - A0) × (B0 + B1)
        Matrix_Sub(n / 2, A21, A11, AA);
        Matrix_Add(n / 2, B11, B12, BB);
//        Strassen(n / 2, AA, BB, M6);
	rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &tid6);
	
	rcode = pvm_initsend(PvmDataDefault);
	rcode = pvm_pkint(&half_n, 1, 1); 
	rcode = pvm_pkdouble(&AA, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&BB, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&M6, half_n * half_n, 1); 
	rcode = pvm_send(tid6, mtype);

 
        //M7 = (A1 - A3) × (B2 + B3)
        Matrix_Sub(n / 2, A12, A22, AA);
        Matrix_Add(n / 2, B21, B22, BB);
//        Strassen(n / 2, AA, BB, M7);
 	rcode = pvm_spawn("s_s", NULL, PvmTaskDefault, "", 1, &tid7);
	
	rcode = pvm_initsend(PvmDataDefault);
	rcode = pvm_pkint(&half_n, 1, 1); 
	rcode = pvm_pkdouble(&AA, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&BB, half_n * half_n, 1); 
	rcode = pvm_pkdouble(&M7, half_n * half_n, 1); 
	rcode = pvm_send(tid7, mtype);


	rcode = pvm_recv(tid1, 1);          /* wait to receive message from slave*/	
	rcode = pvm_upkdouble(&M1, half_n * half_n, 1);   /* contents of matrix B */

	rcode = pvm_recv(tid2, 1);          /* wait to receive message from slave*/	
	rcode = pvm_upkdouble(&M2, half_n * half_n, 1);   /* contents of matrix B */

	rcode = pvm_recv(tid3, 1);          /* wait to receive message from slave*/	
	rcode = pvm_upkdouble(&M3, half_n * half_n, 1);   /* contents of matrix B */

	rcode = pvm_recv(tid4, 1);          /* wait to receive message from slave*/	
	rcode = pvm_upkdouble(&M4, half_n * half_n, 1);   /* contents of matrix B */

	rcode = pvm_recv(tid5, 1);          /* wait to receive message from slave*/	
	rcode = pvm_upkdouble(&M5, half_n * half_n, 1);   /* contents of matrix B */

	rcode = pvm_recv(tid6, 1);          /* wait to receive message from slave*/	
	rcode = pvm_upkdouble(&M6, half_n * half_n, 1);   /* contents of matrix B */

	rcode = pvm_recv(tid7, 1);          /* wait to receive message from slave*/	
	rcode = pvm_upkdouble(&M7, half_n * half_n, 1);   /* contents of matrix B */

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


	
	int stid = pvm_mytid();
	
	int mtype = 1;                              /* set message type */
	int mtid = pvm_parent();                    /* get task id for master process */
	int rcode = pvm_recv(mtid, mtype);          /* wait to receive message from master*/	
	rcode = pvm_upkdouble(&N, 1, 1);           /* contents of matrix A */
	
	int A[N][N],B[N][N],C[N][N];

	
	rcode = pvm_upkdouble(&A, N*N, 1);           /* contents of matrix A */
	rcode = pvm_upkdouble(&B, N*N, 1);           /* contents of matrix A */
	rcode = pvm_upkdouble(&C, N*N, 1);           /* contents of matrix A */

	


    puts("Matrix A Slave：");
    print(N, A);

    puts("Matrix B Slave：");
    print(N, B);

    puts("Matrix C Slave：");
    print(N, C);

	Strassen(N, A, B, C);

/*
	int i, j;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {

            C[i][j] *= -1;
        }
    }
 
*/    
	rcode = pvm_initsend(PvmDataDefault);
	rcode = pvm_pkdouble(&C, N*N, 1); 
	rcode = pvm_send(pvm_parent(), 1);
	rcode = pvm_exit();

	return 0;
}
