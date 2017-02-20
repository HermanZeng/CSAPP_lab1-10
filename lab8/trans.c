/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
 /*
*name: Zeng Heming
*ID: 5140219405
*/
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, ii, jj, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7; //tmp8, tmp9;
    if (M == 32){
        for (i = 0; i < 25; i += 8) {
		for (j = 0; j < 32; j++) {
			tmp0 = A[i][j];
			tmp1 = A[i + 1][j];
			tmp2 = A[i + 2][j];
			tmp3 = A[i + 3][j];
			tmp4 = A[i + 4][j];
            tmp5 = A[i + 5][j];
            tmp6 = A[i + 6][j];
            tmp7 = A[i + 7][j];
            /*tmp8 = A[i + 8][j];
            tmp9 = A[i + 9][j];*/
			B[j][i] = tmp0;
			B[j][i + 1] = tmp1;
			B[j][i + 2] = tmp2;
			B[j][i + 3] = tmp3;
			B[j][i + 4] = tmp4;
            B[j][i + 5] = tmp5;
            B[j][i + 6] = tmp6;
            B[j][i + 7] = tmp7;
            /*B[j][i + 8] = tmp8;
            B[j][i + 9] = tmp9;*/
		}
	}
    /*for(j = 0; j < 32; j++){
        tmp0 = A[28][j];
        tmp1 = A[29][j];
        tmp2 = A[30][j];
        tmp3 = A[31][j];
        B[j][28] = tmp0;
        B[j][29] = tmp1;
        B[j][30] = tmp2;
        B[j][31] = tmp3;
    }*/

    }
    if (M == 64){
		for (i = 0; i < 57; i += 8) {
			for (j = 0; j < 57; j += 8) {
				for (ii = 0; ii < 4; ii++)
				{
					tmp0 = A[i + ii][j];
					tmp1 = A[i + ii][j + 1];
					tmp2 = A[i + ii][j + 2];
					tmp3 = A[i + ii][j + 3];
					B[j][i + ii] = tmp0;
					B[j+1][i + ii] = tmp1;
					B[j+2][i + ii] = tmp2;
					B[j+3][i + ii] = tmp3;
				}
				for (ii = 0; ii < 4; ii++)
				{
					tmp4 = A[i + ii][j + 4];
					tmp5 = A[i + ii][j + 5];
					tmp6 = A[i + ii][j + 6];
					tmp7 = A[i + ii][j + 7];
					B[j][i + 4 + ii] = tmp4;
					B[j + 1][i + 4 + ii] = tmp5;
					B[j + 2][i + 4 + ii] = tmp6;
					B[j + 3][i + 4 + ii] = tmp7;
				}
				for (jj = 0; jj < 4; jj++)
				{
					tmp0 = B[j + jj][i + 4];
					tmp1 = B[j + jj][i + 5];
					tmp2 = B[j + jj][i + 6];
					tmp3 = B[j + jj][i + 7];
					tmp4 = A[i + 4][j + jj];
					tmp5 = A[i + 5][j + jj];
					tmp6 = A[i + 6][j + jj];
					tmp7 = A[i + 7][j + jj];
					B[j + jj][i + 4] = tmp4;
					B[j + jj][i + 5] = tmp5;
					B[j + jj][i + 6] = tmp6;
					B[j + jj][i + 7] = tmp7;
					B[j + 4 + jj][i] = tmp0;
					B[j + 4 + jj][i + 1] = tmp1;
					B[j + 4 + jj][i + 2] = tmp2;
					B[j + 4 + jj][i + 3] = tmp3;
				}
				for (ii = 0; ii < 4; ii++)
				{
					tmp0 = A[i + 4][j + 4 + ii];
					tmp1 = A[i + 5][j + 4 + ii];
					tmp2 = A[i + 6][j + 4 + ii];
					tmp3 = A[i + 7][j + 4 + ii];
					B[j + 4 + ii][i + 4] = tmp0;
					B[j + 4 + ii][i + 5] = tmp1;
					B[j + 4 + ii][i + 6] = tmp2;
					B[j + 4 + ii][i + 7] = tmp3;
				}
			}
		}
	}
    if (M == 61){
        for (i = 0; i < 57; i += 8) {
            for (j = 0; j < 61; j++) {
                tmp0 = A[i][j];
			    tmp1 = A[i + 1][j];
			    tmp2 = A[i + 2][j];
			    tmp3 = A[i + 3][j];
			    tmp4 = A[i + 4][j];
                tmp5 = A[i + 5][j];
                tmp6 = A[i + 6][j];
                tmp7 = A[i + 7][j];
                /*tmp8 = A[i + 8][j];
                tmp9 = A[i + 9][j];*/
			    B[j][i] = tmp0;
			    B[j][i + 1] = tmp1;
			    B[j][i + 2] = tmp2;
			    B[j][i + 3] = tmp3;
			    B[j][i + 4] = tmp4;
                B[j][i + 5] = tmp5;
                B[j][i + 6] = tmp6;
                B[j][i + 7] = tmp7;
                /*B[j][i + 8] = tmp8;
                B[j][i + 9] = tmp9;*/
            }
        }
        for(j = 0; j < 67; j++){
        tmp0 = A[64][j];
        tmp1 = A[65][j];
        tmp2 = A[66][j];
        B[j][64] = tmp0;
        B[j][65] = tmp1;
        B[j][66] = tmp2;
    }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7; //tmp8, tmp9;
    if (M == 32){
        for (i = 0; i < 25; i += 8) {
		for (j = 0; j < 32; j++) {
			tmp0 = A[i][j];
			tmp1 = A[i + 1][j];
			tmp2 = A[i + 2][j];
			tmp3 = A[i + 3][j];
			tmp4 = A[i + 4][j];
            tmp5 = A[i + 5][j];
            tmp6 = A[i + 6][j];
            tmp7 = A[i + 7][j];
            /*tmp8 = A[i + 8][j];
            tmp9 = A[i + 9][j];*/
			B[j][i] = tmp0;
			B[j][i + 1] = tmp1;
			B[j][i + 2] = tmp2;
			B[j][i + 3] = tmp3;
			B[j][i + 4] = tmp4;
            B[j][i + 5] = tmp5;
            B[j][i + 6] = tmp6;
            B[j][i + 7] = tmp7;
            /*B[j][i + 8] = tmp8;
            B[j][i + 9] = tmp9;*/
		}
	}
    /*for(j = 0; j < 32; j++){
        tmp0 = A[28][j];
        tmp1 = A[29][j];
        tmp2 = A[30][j];
        tmp3 = A[31][j];
        B[j][28] = tmp0;
        B[j][29] = tmp1;
        B[j][30] = tmp2;
        B[j][31] = tmp3;
    }*/

    }
    if (M == 64){
        for (i = 0; i < 61; i += 4) {
            for (j = 0; j < 64; j++) {
                tmp0 = A[i][j];
			    tmp1 = A[i + 1][j];
			    tmp2 = A[i + 2][j];
			    tmp3 = A[i + 3][j];
			    /*tmp4 = A[i + 4][j];
                tmp5 = A[i + 5][j];
                tmp6 = A[i + 6][j];
                tmp7 = A[i + 7][j];
                tmp8 = A[i + 8][j];
                tmp9 = A[i + 9][j];*/
			    B[j][i] = tmp0;
			    B[j][i + 1] = tmp1;
			    B[j][i + 2] = tmp2;
			    B[j][i + 3] = tmp3;
			    /*B[j][i + 4] = tmp4;
                B[j][i + 5] = tmp5;
                B[j][i + 6] = tmp6;
                B[j][i + 7] = tmp7;
                B[j][i + 8] = tmp8;
                B[j][i + 9] = tmp9;*/
            }
        }
    }
    if (M == 61){
        for (i = 0; i < 61; i ++) {
            for (j = 0; j < 67; j++) {
                tmp0 = A[i][j];
			    B[j][i] = tmp0;
            }
        }
    }
    //printf("%d\n", is_transpose(M,N,A,B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

