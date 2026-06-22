#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "solver.h"

/* Heurística House Density via vetores: célula com mais números já bloqueados */
bool VectorDensity(int **tab, int N, int M, Track *pointer, int *bestI, int *bestJ)
{
    int maxDensity = -1;
    bool found = false;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (tab[i][j] == 0)
            {
                int b = (i / M) * M + (j / M);
                int filledCount = 0;

                for (int num = 1; num <= N; num++)
                {
                    if (pointer->rows[i][num] || pointer->columns[j][num] || pointer->blocks[b][num])
                        filledCount++;
                }

                if (filledCount > maxDensity)
                {
                    maxDensity = filledCount;
                    *bestI = i;
                    *bestJ = j;
                    found  = true;
                }
            }
        }
    }
    return found;
}

void initTrackVectors(int **tab, int N, int M, Track *pointer)
{
    for (int i = 0; i < N; i++)
    {
        for (int num = 0; num <= N; num++)
        {
            pointer->rows[i][num] = 0;
            pointer->columns[i][num] = 0;
            pointer->blocks[i][num] = 0;
        }
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (tab[i][j] != 0)
            {
                int num = tab[i][j];
                int b = (i / M) * M + (j / M);

                pointer->rows[i][num] = 1;
                pointer->columns[j][num] = 1;
                pointer->blocks[b][num] = 1;
            }
        }
    }
}
// Solve vector iterativo (extra)
bool solveSudokuVectorsIterative(int **tab, int N, int M, Track *pointer)
{
    int *emptyI = malloc(N * N * sizeof(int));   
    int *emptyJ = malloc(N * N * sizeof(int));
    int totalEmpty = 0;
    int k = 0;
    int i, j, b, num;
    bool flag;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (tab[i][j] == 0)
            {
                emptyI[totalEmpty] = i;
                emptyJ[totalEmpty] = j;
                totalEmpty++;
            }
        }
    }

    if (totalEmpty == 0)
    {
        free(emptyI);
        free(emptyJ);
        return true;  
    }

    while ((k >= 0) && (k < totalEmpty))
    {
        i = emptyI[k];
        j = emptyJ[k];
        b = (i / M) * M + (j / M);

        int oldNumber = tab[i][j];
        flag = false;

        /* Desfaz o número anterior antes de tentar o próximo */
        if (oldNumber != 0)
        {
            (*pointer).rows[i][oldNumber] = 0;
            (*pointer).columns[j][oldNumber] = 0;   
            (*pointer).blocks[b][oldNumber] = 0;   
        }

        num = oldNumber + 1;

        while (num <= N)
        {
            if (((*pointer).rows[i][num] == 0) &&
                ((*pointer).columns[j][num] == 0) &&
                ((*pointer).blocks[b][num] == 0)) 
            {
                tab[i][j] = num;

                (*pointer).rows[i][num] = 1;
                (*pointer).columns[j][num] = 1;   
                (*pointer).blocks[b][num] = 1;   

                flag = true;
                break;
            }
            num++;  
        }

        if (flag)
        {
            k++;
        }
        else
        {
            tab[i][j] = 0;
            k--;
        }
    }   

    free(emptyI);
    free(emptyJ);
    return (k == totalEmpty);   
}

/* -----------------------------------------------------------------------
   VERSÃO 0: Recursivo padrão com Track
   ----------------------------------------------------------------------- */
bool solveSudokuVectorsRecursive(int **tab, int N, int M, Track *pointer)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (tab[i][j] == 0)
            {
                int b = (i / M) * M + (j / M);

                for (int num = 1; num <= N; num++)
                {
                    if ((pointer->rows[i][num] == 0) &&
                        (pointer->columns[j][num] == 0) &&
                        (pointer->blocks[b][num] == 0))
                    {
                        tab[i][j] = num;

                        pointer->rows[i][num] = 1;
                        pointer->columns[j][num] = 1;
                        pointer->blocks[b][num] = 1;

                        if (solveSudokuVectorsRecursive(tab, N, M, pointer))
                            return true;

                        tab[i][j] = 0;

                        pointer->rows[i][num] = 0;
                        pointer->columns[j][num] = 0;
                        pointer->blocks[b][num] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

/* -----------------------------------------------------------------------
   VERSÃO 1 (otimização 1): House Density com Track
   ----------------------------------------------------------------------- */
bool solveVectorsDensity(int **tab, int N, int M, Track *pointer)
{
    int i, j;

    if (!VectorDensity(tab, N, M, pointer, &i, &j))
    {
        return true;
    }
    int b = (i / M) * M + (j / M);

    for (int num = 1; num <= N; num++)
    {
        if ((pointer->rows[i][num] == 0) &&
            (pointer->columns[j][num] == 0) &&
            (pointer->blocks[b][num] == 0))
        {
            tab[i][j] = num;

            pointer->rows[i][num] = 1;
            pointer->columns[j][num] = 1;
            pointer->blocks[b][num] = 1;

            if (solveVectorsDensity(tab, N, M, pointer))
                return true;

            tab[i][j] = 0;

            pointer->rows[i][num] = 0;
            pointer->columns[j][num] = 0;
            pointer->blocks[b][num] = 0;
        }
    }
    return false;
}

/* -----------------------------------------------------------------------
   VERSÃO 2 (otimização 2): House Density + Zobrist Hash com Track
   ----------------------------------------------------------------------- */
bool solveVectorsDensityHash(int **tab, int N, int M, Track *pointer)
{
    int i, j;

    if (!VectorDensity(tab, N, M, pointer, &i, &j))
    {
        return true;
    }
    int b = (i / M) * M + (j / M);

    for (int num = 1; num <= N; num++)
    {
        if ((pointer->rows[i][num] == 0) &&
            (pointer->columns[j][num] == 0) &&
            (pointer->blocks[b][num] == 0))
        {
            currentBoardHash ^= zobristTable[i][j][num];
            if (isStateFailed(currentBoardHash))
            {
                currentBoardHash ^= zobristTable[i][j][num];
                continue;
            }

            tab[i][j] = num;

            pointer->rows[i][num] = 1;
            pointer->columns[j][num] = 1;
            pointer->blocks[b][num] = 1;

            if (solveVectorsDensityHash(tab, N, M, pointer))
            {
                return true;
            }
            saveFailedState(currentBoardHash);

            tab[i][j] = 0;

            pointer->rows[i][num] = 0;
            pointer->columns[j][num] = 0;
            pointer->blocks[b][num] = 0;

            currentBoardHash ^= zobristTable[i][j][num];
        }
    }
    return false;
}

/* Seletor central: 0 = nenhuma, 1 = House Density, 2 = Density + Zobrist */
bool solveSudokuVectors(int **tab, int N, int M, Track *pointer, int optimization)
{
    switch (optimization)
    {
        case 0:  
            return solveSudokuVectorsRecursive(tab, N, M, pointer);
        case 1:  
            return solveVectorsDensity(tab, N, M, pointer);
        case 2:  
            return solveVectorsDensityHash(tab, N, M, pointer);
        default: 
            return solveSudokuVectorsRecursive(tab, N, M, pointer);
    }
}
