/*
  Ryan Scherbarth
  November 2024
  NETID: 101968169
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
  int i, j, tmp, diag, k, l;
  int nb;

  /*
    Cases to determine M,N combinations. We're allowed to specifically and separately optimize
    For the differnet cases. I am just leting any case outside of the 3 given use CASE -1. which
    will fall through to case 3.
  */
  int CASE = -1;
  if (M == 32 && N == 32)
  {
    CASE = 1;
  }
  else if (M == 64 && N == 64)
  {
    CASE = 2;
  }
  else if (M == 61 && N == 67)
  {
    CASE = 3;
  }

  // Transpose based on CASE
  if (CASE == 1)
  {
    /*
      - block size = 8
      - 32x32 matrix split into 8x8 blocks is \frac{32}{8} * \frac{32}{8} = 4 * 4 = 16 total blocks.
      - each block has 8 * 8 = 64 elements

      with nb=8, a single row of 8 elements fits into one cache line, minimizing the number of misses and evictions.


      Cache Calculations:
      Int size: 4 bytes
      cache block size (B) = 4 * 8 = 32
      num sets (S) = \frac{C}{B*E} = \frac{1024}{32*1} = 32
      cache size (C) = S * E * B = 32 * 1 * 32 = 1024
      associativity (E) = direct mapped = 1
      address size (m) = 32 bits
      block offset (b) = log_2(B) = 5 bits
      set index (s) = log_2(S) = 5 bits
      tag (t) = m - (s+b) = 32 - (5+5) = 22 bits
    */

    nb = 8;

    // Access the rows in row major ordering so our cache calculations above actually do what we expect.
    for (k = 0; k < N; k += nb) // Covers starting x index of the nb
    {
      for (l = 0; l < M; l += nb) // Covers starting y index of the nb
      {
        for (i = k; i < k + nb && i < N; i++) // Covers rows
        {
          for (j = l; j < l + nb && j < M; j++) // Covers cols
          {
            /*
              Saving diagonal values to process after.

              when i=j we are on the diag. Each time we hit a value on the diag we end up doing
              a read and write to the same location which causes a cache miss each time for the
              m values.
            */
            if (i != j)
            {
              B[j][i] = A[i][j];
            }
            else
            {
              diag = i;
              tmp = A[i][j];
            }
          }
          /*
            To minimize cache misses on the diag, save the diag until the end. Every memory location on the
            diag will result in a miss. Since we know that for sure, the miss there along the diag causes us
            to have another miss for every value diag+1 after the first miss.

            Save the diag values until the end so that we don't have all those extra misses.
          */
          if (k == l)
          {
            B[diag][diag] = tmp;
          }
        }
      }
    }
  }
  else if (CASE == 2)
  {
    nb = 4;

    for (int i = 0; i < N; i += nb) // row of blocks
    {
      for (int j = 0; j < M; j += nb) // col of blocks
      {
        for (int k = i; k < i + nb && k < N; k++) // row in blcok
        {
          for (int l = j; l < j + nb && l < M; l++) // col in block
          {
            /*
              Just transpose everyting in order
              1890 misses
            */
            //  B[diag][diag] = tmp;

            /*
              Skips the diag until the end again.
              1790 misses
            */
            if (k != l)
            {
              B[l][k] = A[k][l];
            }
            else
            {
              diag = k;
              tmp = A[k][l];
            }
          }
          if (i == j)
          {
            B[diag][diag] = tmp;
          }
        }
      }
    }
  }

  else
  {
    /*
      Same as case 1 but using nb=16
    */
    nb = 16;

    for (k = 0; k < N; k += nb) // Covers starting x index of the nb
    {
      for (l = 0; l < M; l += nb) // Covers starting y index of the nb
      {
        for (i = k; i < k + nb && i < N; i++) // Covers rows
        {
          for (j = l; j < l + nb && j < M; j++) // Covers cols
          {
            /*
              Saving diagonal values to process after.

              when i=j we are on the diag. Each time we hit a value on the diag we end up doing
              a read and write to the same location which causes a cache miss each time for the
              m values.
            */
            if (i != j)
            {
              B[j][i] = A[i][j];
            }
            else
            {
              diag = i;
              tmp = A[i][j];
            }
          }
          if (k == l)
          {
            B[diag][diag] = tmp;
          }
        }
      }
    }
  }
}

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, tmp;

  for (i = 0; i < N; i++)
  {
    for (j = 0; j < M; j++)
    {
      tmp = A[i][j];
      B[j][i] = tmp;
    }
  }
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

  for (i = 0; i < N; i++)
  {
    for (j = 0; j < M; ++j)
    {
      if (A[i][j] != B[j][i])
      {
        return 0;
      }
    }
  }
  return 1;
}