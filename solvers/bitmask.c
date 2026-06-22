#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "solver.h"

int BitEmpty(unsigned long long mask)
{
    return __builtin_popcountll(mask);
}

void initBitTrack(int **tab, int N, int M, BitTrack *bt)
{
    memset(bt, 0, sizeof(BitTrack));

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (tab[i][j] != 0)
            {
                int num   = tab[i][j];
                int block = (i / M) * M + (j / M);
                unsigned long long bit = 1ULL << num;

                bt->rows[i] |= bit;
                bt->columns[j] |= bit;
                bt->blocks[block] |= bit;
            }
        }
    }
}

// Heurística House Density: célula com mais restrições (mais vizinhos preenchidos)
bool HouseDensity(int **tab, int N, int M, BitTrack *pointer, int *bestI, int *bestJ)
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
                unsigned long long density = pointer->blocks[b] | pointer->rows[i] | pointer->columns[j];
                
                int filledCount = BitEmpty(density);

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

/* -----------------------------------------------------------------------
   VERSÃO 0: Bitmask com scan linear (sem heurística, sem hash)
   ----------------------------------------------------------------------- */
bool solveBitmaskStandard(int **tab, int N, int M, BitTrack *pointer)
{
    int i = -1, j = -1;
    unsigned long long mask;
    bool foundEmpty = false;

    for (int tempI = 0; (tempI < N) && !(foundEmpty); tempI++)
    {
        for (int tempJ = 0; (tempJ < N) && !(foundEmpty); tempJ++)
        {
            if (tab[tempI][tempJ] == 0)
            {
                i = tempI;
                j = tempJ;
                foundEmpty = true;
            }
        }
    }
    if (!foundEmpty)
        return true;

    int b = (i / M) * M + (j / M);
    mask = pointer->blocks[b] | pointer->rows[i] | pointer->columns[j];

    unsigned long long free = ~mask;
    free &= ((1ULL << N) - 1) << 1;   /* mantém apenas bits 1 até N */

    if (free == 0)
        return false;

    while (free)
    {
        /* FIX: ffsll retorna índice+1 → subtrai 1 para obter o número real */
        int num = __builtin_ffsll(free) - 1;
        unsigned long long bit = 1ULL << num;   

        tab[i][j] = num;

        pointer->rows[i] |= bit;
        pointer->columns[j] |= bit;
        pointer->blocks[b] |= bit;

        if (solveBitmaskStandard(tab, N, M, pointer))
            return true;

        tab[i][j] = 0;

        pointer->rows[i] &= ~bit;
        pointer->columns[j] &= ~bit;
        pointer->blocks[b] &= ~bit;
        free &= ~bit;
    }

    return false;
}

/* -----------------------------------------------------------------------
   VERSÃO 1: Bitmask com House Density (sem hash)
   ----------------------------------------------------------------------- */
bool solveSudokuBitmaskH(int **tab, int N, int M, BitTrack *pointer)
{
    int i, j;
    unsigned long long mask;

    if (!HouseDensity(tab, N, M, pointer, &i, &j))
    {
        return true;
    }
    int b = (i / M) * M + (j / M);
    mask = pointer->blocks[b] | pointer->rows[i] | pointer->columns[j];

    unsigned long long free = ~mask;
    free &= ((1ULL << N) - 1) << 1;

    while (free)
    {
        int num = __builtin_ffsll(free) - 1;
        unsigned long long bit = 1ULL << num;

        tab[i][j] = num;

        pointer->rows[i] |= bit;
        pointer->columns[j] |= bit;
        pointer->blocks[b] |= bit;

        if (solveSudokuBitmaskH(tab, N, M, pointer))
        {
            return true;
        }

        tab[i][j] = 0;

        pointer->rows[i] &= ~bit;
        pointer->columns[j] &= ~bit;
        pointer->blocks[b] &= ~bit;
        free &= ~bit;
    }

    return false;
}

/* -----------------------------------------------------------------------
   VERSÃO 2: Bitmask com House Density + Zobrist Hash
   ----------------------------------------------------------------------- */
bool solveBitmaskHash(int **tab, int N, int M, BitTrack *pointer)
{
    int i, j;
    unsigned long long mask;

    if (!HouseDensity(tab, N, M, pointer, &i, &j))
    {
        return true;
    }

    int b = (i / M) * M + (j / M);
    mask = pointer->blocks[b] | pointer->rows[i] | pointer->columns[j];

    unsigned long long free = ~mask;
    free &= ((1ULL << N) - 1) << 1;

    while (free)
    {
        int num = __builtin_ffsll(free) - 1;
        unsigned long long bit = 1ULL << num;

        currentBoardHash ^= zobristTable[i][j][num];
        if (isStateFailed(currentBoardHash))
        {
            currentBoardHash ^= zobristTable[i][j][num];
            free &= ~bit;
            continue;
        }

        tab[i][j] = num;

        pointer->rows[i] |= bit;
        pointer->columns[j] |= bit;
        pointer->blocks[b] |= bit;

        if (solveBitmaskHash(tab, N, M, pointer))
        {
            return true;
        }
        saveFailedState(currentBoardHash);

        tab[i][j] = 0;

        pointer->rows[i] &= ~bit;
        pointer->columns[j] &= ~bit;
        pointer->blocks[b] &= ~bit;

        currentBoardHash ^= zobristTable[i][j][num];
        free &= ~bit;
    }

    return false;
}

/* Seletor central: 0 = nenhuma, 1 = House Density, 2 = Density + Zobrist */
bool solveSudokuBitmask(int **tab, int N, int M, BitTrack *pointer, int optimization)
{
    switch (optimization)
    {
        case 0:  
            return solveBitmaskStandard(tab, N, M, pointer);
        case 1:  
            return solveSudokuBitmaskH(tab, N, M, pointer);
        case 2:  
            return solveBitmaskHash(tab, N, M, pointer);
        default: 
            return solveBitmaskStandard(tab, N, M, pointer);
    }
}
