#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "solver.h"

bool ismovValid(int **tab, int i, int j, int num, int N, int M)
{
    for (int y = 0; y < N; y++)
    {
        if ((num == tab[i][y]) && (y != j))
            return false;
    }

    for (int x = 0; x < N; x++)
    {
        if ((num == tab[x][j]) && (x != i))
            return false;
    }

    int bgi = (i / M) * M;
    int bgj = (j / M) * M;

    for (int x = bgi; x < bgi + M; x++)
    {
        for (int y = bgj; y < bgj + M; y++)
        {
            if ((tab[x][y] == num) && !(y == j && x == i))
                return false;
        }
    }

    return true;
}

bool win(int **tab, int N)
{

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (tab[i][j] == 0)
                return false;
        }
    }

    return true;
}

/* Heurística MRV: célula com menos opções disponíveis */
bool BruteForceDensity(int **tab, int N, int *bestI, int *bestJ, int M)
{
    int minOptions = N + 1;
    bool found = false;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (tab[i][j] == 0)
            {
                int optionsCount = 0;
                for (int num = 1; num <= N; num++)
                {
                    if (ismovValid(tab, i, j, num, N, M))
                        optionsCount++;
                }

                if (optionsCount < minOptions)
                {
                    minOptions = optionsCount;
                    *bestI = i;
                    *bestJ = j;
                    found = true;
                }
            }
        }
    }
    return found;
}

/* Versão 0: Backtracking iterativo (sem heurística) */
bool solveBruteIterative(int **tab, int N, int M)
{
    int *emptyI = malloc(N * N * sizeof(int));
    int *emptyJ = malloc(N * N * sizeof(int));
    int totalEmpty = 0;
    int k = 0;

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
        int i   = emptyI[k];
        int j   = emptyJ[k];
        int num = tab[i][j] + 1;
        bool flag = false;

        while (num <= N)
        {
            if (ismovValid(tab, i, j, num, N, M))
            {
                tab[i][j] = num;
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

/* Versão 1: Backtracking recursivo padrão*/
bool solveBruteStandard(int **tab, int N, int M)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (tab[i][j] == 0)
            {
                for (int num = 1; num <= N; num++)
                {
                    if (ismovValid(tab, i, j, num, N, M))
                    {
                        tab[i][j] = num;

                        if (solveBruteStandard(tab, N, M))
                        {
                            return true;
                        }
                        tab[i][j] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

/* Versão 2 (otimização 1): House Density */
bool solveBruteDensity(int **tab, int N, int M)
{
    int i, j;

    if (!BruteForceDensity(tab, N, &i, &j, M))
    {
        return true;
    }
    for (int num = 1; num <= N; num++)
    {
        if (ismovValid(tab, i, j, num, N, M))
        {
            tab[i][j] = num;

            if (solveBruteDensity(tab, N, M))
            {
                return true;
            }
            tab[i][j] = 0;
        }
    }
    return false;
}

/* Versão 3 (otimização 2): House Density + Zobrist Hash */
bool solveBruteDensityHash(int **tab, int N, int M)
{
    int i, j;

    if (!BruteForceDensity(tab, N, &i, &j, M))
    {
        return true;
    }
    for (int num = 1; num <= N; num++)
    {
        if (ismovValid(tab, i, j, num, N, M))
        {
            currentBoardHash ^= zobristTable[i][j][num];
            if (isStateFailed(currentBoardHash))
            {
                currentBoardHash ^= zobristTable[i][j][num];
                continue;
            }

            tab[i][j] = num;

            if (solveBruteDensityHash(tab, N, M))
            { 
                return true;
            }
            saveFailedState(currentBoardHash);

            tab[i][j] = 0;

            currentBoardHash ^= zobristTable[i][j][num];
        }
    }

    return false;
}

/* Seletor central: 0 = nenhuma, 1 = House Density, 2 = Density + Zobrist */
bool solveSudoku(int **tab, int N, int M, int optimization)
{
    switch (optimization)
    {
        case 0:  
            return solveBruteStandard(tab, N, M);
        case 1:  
            return solveBruteDensity(tab, N, M);
        case 2:  
            return solveBruteDensityHash(tab, N, M);
        default: 
            return solveBruteStandard(tab, N, M);
    }
}
