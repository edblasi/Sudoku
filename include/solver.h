#ifndef SOLVER_H
#define SOLVER_H

#include <stdbool.h>
#include "config.h"

#define NMAX 50

/* ==========================================
            ESTRUTURAS DE DADOS
   ========================================== */

/* Tracking via vetores/matrizes */
typedef struct
{
    int rows[NMAX][NMAX];
    int columns[NMAX][NMAX];
    int blocks[NMAX][NMAX];
} Track;

/* Tracking via Bitmask */
typedef struct 
{
    unsigned long long int rows[NMAX];
    unsigned long long int columns[NMAX];
    unsigned long long int blocks[NMAX];
} BitTrack;

/* Nó da tabela de transposição Zobrist */
typedef struct {
    unsigned long long int key;
    bool isFailed;
} HashNode;

/* ==========================================
            PROTÓTIPOS DE FUNÇÕES
   ========================================== */

/* Interface (interface.c) */
Config lobby(void);
void printTab(int **tab, int N);
void playGame(int **tab, int N, int M);
void ld_Campaign(int **tab, int N);
void ld_Custom(int **tab, int N, int M);

/* Validação (bruteforceBacktr.c) */
bool ismovValid(int **tab, int i, int j, int num, int N, int M);
bool win(int **tab, int N);

/* Brute Force (bruteforceBacktr.c) */
bool BruteForceDensity(int **tab, int N, int *bestI, int *bestJ, int M);
bool solveBruteStandard(int **tab, int N, int M);
bool solveBruteIterative(int **tab, int N, int M);
bool solveBruteDensity(int **tab, int N, int M);
bool solveBruteDensityHash(int **tab, int N, int M);
bool solveSudoku(int **tab, int N, int M, int optimization);

/* Bitmask (bitmask.c) */
int BitEmpty(unsigned long long mask);
bool HouseDensity(int **tab, int N, int M, BitTrack *pointer, int *bestI, int *bestJ);
void initBitTrack(int **tab, int N, int M, BitTrack *bt);
bool solveBitmaskStandard(int **tab, int N, int M, BitTrack *pointer);
bool solveSudokuBitmaskH(int **tab, int N, int M, BitTrack *pointer);
bool solveBitmaskHash(int **tab, int N, int M, BitTrack *pointer);
bool solveSudokuBitmask(int **tab, int N, int M, BitTrack *pointer, int optimization);

/* Vetores (vectorBacktr.c) */
bool VectorDensity(int **tab, int N, int M, Track *pointer, int *bestI, int *bestJ);
void initTrackVectors(int **tab, int N, int M, Track *pointer);
bool solveSudokuVectorsIterative(int **tab, int N, int M, Track *pointer);
bool solveSudokuVectorsRecursive(int **tab, int N, int M, Track *pointer);
bool solveVectorsDensity(int **tab, int N, int M, Track *pointer);
bool solveVectorsDensityHash(int **tab, int N, int M, Track *pointer);
bool solveSudokuVectors(int **tab, int N, int M, Track *pointer, int optimization);

/* Zobrist Hash (zobristhash.c) */
void initZobrist(int N);
unsigned long long int computeInitialHash(int **tab, int N);
bool isStateFailed(unsigned long long int hash);
void saveFailedState(unsigned long long int hash);
unsigned long long mix(unsigned long long x);

/* Globais definidas em zobristhash.c */
extern unsigned long long int currentBoardHash;
extern unsigned long long int zobristTable[NMAX][NMAX][NMAX + 1];

#endif
