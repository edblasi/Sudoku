#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "solver.h"

Config lobby(void)
{
    Config Lconfig;

    enum
    {
        SIZE,
        MODE,
        PLAYER,
        OPTIMIZATION,
        ALGO,
        DONE
    } state = SIZE;

    while (state != DONE)
    {
        if (state == SIZE)
        {
            printf("========================================\n");
            printf("                SUDOKU                  \n");
            printf("========================================\n");
            printf("Choose the sudoku size:\n");
            printf("[4]  Mini Sudoku (4x4)\n");
            printf("[9]  Traditional Sudoku (9x9)\n");
            printf("[16] Big Sudoku (16x16)\n");
            printf("[25] Giant Sudoku (25x25)\n");
            printf("[36] Supreme Sudoku (36x36)\n");
            printf("[49] Ultimate Sudoku (49x49)\n");

            scanf("%d", &Lconfig.N);

            if (Lconfig.N == 4 || Lconfig.N == 9 || Lconfig.N == 16 ||
                Lconfig.N == 25 || Lconfig.N == 36 || Lconfig.N == 49)
            {
                state = MODE;
            }
            else
            {
                printf("Invalid size!\n");
            }
        }
        else if (state == MODE)
        {
            printf("========================================\n");
            printf("               GAME MODE                \n");
            printf("========================================\n");
            printf("Custom Mode (0)\n");
            printf("Campaign Mode (1)\n");

            scanf("%d", &Lconfig.mode);

            if (Lconfig.mode == 0 || Lconfig.mode == 1)
            {
                state = PLAYER;
            }
            else
            {
                printf("Invalid mode!\n");
            }
        }
        else if (state == PLAYER)
        {
            printf("========================================\n");
            printf("                 PLAYER                 \n");
            printf("========================================\n");
            printf("Human Mode (0)\n");
            printf("AI Mode (1)\n");

            scanf("%d", &Lconfig.pmode);

            if (Lconfig.pmode == 0)
            {
                Lconfig.optimization = 0;
                Lconfig.algo = 0;
                state = DONE;
            }
            else if (Lconfig.pmode == 1)
            {
                state = OPTIMIZATION;
            }
            else
            {
                printf("Invalid player mode!\n");
            }
        }
        else if (state == OPTIMIZATION)
        {
            printf("========================================\n");
            printf("             AI OPTIMIZATION            \n");
            printf("========================================\n");
            printf("None (0)\n");
            printf("House Density Heuristic (1)\n");
            printf("House Density + Zobrist Hash Table (2)\n");

            scanf("%d", &Lconfig.optimization);

            if (Lconfig.optimization >= 0 && Lconfig.optimization <= 2)
            {
                state = ALGO;
            }
            else
            {
                printf("Invalid optimization mode!\n");
            }
        }
        else if (state == ALGO)
        {
            printf("========================================\n");
            printf("              ALGORITHM                 \n");
            printf("========================================\n");
            printf("BruteForce (0)\n");
            printf("BitMask (1)\n");
            printf("VectorTracking (2)\n");

            scanf("%d", &Lconfig.algo);

            if (Lconfig.algo >= 0 && Lconfig.algo <= 2)
            {
                state = DONE;
            }
            else
            {
                printf("Invalid algorithm choice!\n");
            }
        }
    }

    return Lconfig;
}

void printTab(int **tab, int N)
{
    int M = (N == 4) ? 2 :
            (N == 9) ? 3 :
            (N == 16) ? 4 :
            (N == 25) ? 5 :
            (N == 36) ? 6 : 7;

    printf("\n");
    printf("     ");

    for (int j = 0; j < N; j++)
    {
        if (j > 0 && j % M == 0)
        {
            printf("  ");
        }
        printf("%2d ", j);
    }
    printf("\n");

    for (int i = 0; i < N; i++)
    {
        if (i > 0 && i % M == 0)
        {
            printf("\n     ");
            for (int j = 0; j < N; j++)
            {
                printf("---");
                if ((j + 1) % M == 0 && j != N - 1)
                {
                    printf("+");
                }
            }
            printf("\n");
        }

        printf("%2d | ", i);

        for (int j = 0; j < N; j++)
        {
            if (tab[i][j] == 0)
            {
                printf(" . ");
            }
            else
            {
                printf("%2d ", tab[i][j]);
            }

            if ((j + 1) % M == 0 && j != N - 1)
            {
                printf("| ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void ld_Custom(int **tab, int N, int M)
{
    int i, j, num;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            tab[i][j] = 0;
        }
    }

    printTab(tab, N);

    while (1)
    {
        printf("Enter the value: (Row, Column, Value)\n");

        if (scanf("%d %d %d", &i, &j, &num) != 3)
        {
            break;
        }

        if (i < 0 || i >= N || j < 0 || j >= N)
        {
            printf("Invalid position\n");
            continue;
        }

        if (ismovValid(tab, i, j, num, N, M))
        {
            tab[i][j] = num;
            printTab(tab, N);
        }
        else
        {
            printf("Invalid move\n");
        }
    }
}

/* Carrega o tabuleiro de um arquivo .txt — um puzzle por arquivo */
void ld_Campaign(int **tab, int N)
{
    FILE *file;

    switch (N)
    {
        case 4:
            file = fopen("sheet4.txt", "r");
            break;
        case 9:
            file = fopen("sheet9.txt", "r");
            break;
        case 16:
            file = fopen("sheet16.txt", "r");
            break;
        case 25:
            file = fopen("sheet25.txt", "r");
            break;
        case 36:
            file = fopen("sheet36.txt", "r");
            break;
        case 49:
            file = fopen("sheet49.txt", "r");
            break;
        default:
            printf("Error: unsupported board size %d\n", N);
            exit(1);
    }

    if (file == NULL)
    {
        printf("Error: could not open sheet file for size %d\n", N);
        exit(1);
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (fscanf(file, "%d", &tab[i][j]) != 1)
            {
                printf("Error: failed to read position [%d][%d]\n", i, j);
                fclose(file);
                exit(1);
            }
        }
    }

    fclose(file);
}

void playGame(int **tab, int N, int M)
{
    int i, j, num;

    while (1)
    {
        printf("Enter the value: (Row, Column, Value)\n");

        if (scanf("%d %d %d", &i, &j, &num) != 3)
        {
            int c;
            printf("Invalid value, try again\n");
            while ((c = getchar()) != '\n' && c != EOF)
            {}
            continue;
        }

        if (i < 0 || i >= N || j < 0 || j >= N)
        {
            printf("Invalid position\n");
            continue;
        }

        if ((tab[i][j] == 0) && (ismovValid(tab, i, j, num, N, M)))
        {
            tab[i][j] = num;
            printTab(tab, N);
        }
        else
        {
            printf("That number cannot be placed there! Try again\n");
        }

        if (win(tab, N))
        {
            printf("Congratulations, you won!\n");
            break;
        }
    }
}
