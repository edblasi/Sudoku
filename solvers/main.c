#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "solver.h"
#include "config.h"

int main(void)
{
    Config game = lobby();

    int M = (game.N == 4)  ? 2 :
            (game.N == 9)  ? 3 :
            (game.N == 16) ? 4 :
            (game.N == 25) ? 5 :
            (game.N == 36) ? 6 : 7;

    int **tab = (int **) malloc(game.N * sizeof(int *));
    for (int i = 0; i < game.N; i++)
    {
        tab[i] = (int *) malloc(game.N * sizeof(int));
    }

    if (game.mode == 1)
    {
        ld_Campaign(tab, game.N);
        printf("Loading board for size %d\n", game.N);
    }
    else
    {
        ld_Custom(tab, game.N, M);
        printf("Loading custom mode\n");
    }

    printTab(tab, game.N);

    if (game.pmode == 0)
    {
        printf("Good luck!\n");
        playGame(tab, game.N, M);
    }
    else
    {
        printf("Processing via AI Solver...\n");
        bool solved = false;

        /* Inicializa Zobrist apenas quando a otimização exige hash (opção 2) */
        if (game.optimization == 2)
        {
            initZobrist(game.N);
            currentBoardHash = computeInitialHash(tab, game.N);
        }

        switch (game.algo)
        {
            case 0: /* Brute Force */
                solved = solveSudoku(tab, game.N, M, game.optimization);
                break;

            case 1: /* Bitmask */
            {
                BitTrack bitTrack;
                initBitTrack(tab, game.N, M, &bitTrack);
                solved = solveSudokuBitmask(tab, game.N, M, &bitTrack, game.optimization);
            }
            break;

            case 2: /* Vector Tracking */
            {
                Track vectorTrack;
                initTrackVectors(tab, game.N, M, &vectorTrack);
                solved = solveSudokuVectors(tab, game.N, M, &vectorTrack, game.optimization);
            }
            break;

            default:
                solved = solveSudoku(tab, game.N, M, game.optimization);
                break;
        }

        if (solved)
        {
            printf("\nSolution Found:\n");
            printTab(tab, game.N);
            printf("Completed\n");
        }
        else
        {
            printf("The AI could not find a solution for this board configuration.\n");
        }
    }

    for (int i = 0; i < game.N; i++)
    {
        free(tab[i]);
    }
    free(tab);

    return 0;
}
