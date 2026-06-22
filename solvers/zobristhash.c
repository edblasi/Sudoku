#include <stdio.h>
#include <stdlib.h>
#include "solver.h"

/* ~1M entradas ≈ 16MB — adequado para puzzles até 49x49 */
#define HASH_TABLE_SIZE (1 << 20)

/* Tabela Zobrist e hash global (declarados como extern em solver.h) */
unsigned long long int zobristTable[NMAX][NMAX][NMAX + 1];
unsigned long long int currentBoardHash = 0;

/* Tabela de transposição — estática pois é detalhe de implementação */
static HashNode hashTable[HASH_TABLE_SIZE];

/*
 * Gera um número pseudo-aleatório de 64 bits.
 * rand() garante apenas 15 bits (RAND_MAX mínimo), então combinamos 4 blocos.
 */
static unsigned long long int random64(void)
{
    unsigned long long int b1 = rand();
    unsigned long long int b2 = rand();
    unsigned long long int b3 = rand();
    unsigned long long int b4 = rand();
    return (b1 << 45) | (b2 << 30) | (b3 << 15) | b4;
}

/* Inicializa a tabela Zobrist com valores aleatórios e limpa o hash table */
void initZobrist(int N)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        hashTable[i].key = 0;
        hashTable[i].isFailed = false;
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int num = 1; num <= N; num++)
            {
                zobristTable[i][j][num] = random64();
            }
        }
    }
}

/* Calcula o hash inicial do tabuleiro (células pré-preenchidas) */
unsigned long long int computeInitialHash(int **tab, int N)
{
    unsigned long long int h = 0;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (tab[i][j] != 0)
            {
                h ^= zobristTable[i][j][tab[i][j]];
            }
        }
    }
    return h;
}

/* Função de mistura de bits para distribuição uniforme dos índices(Algoritmo do claude[IA] pra evitar o máximo de colisões) */
unsigned long long mix(unsigned long long x)
{
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    x =  x ^ (x >> 31);
    return x;
}

/* Verifica se o estado com dado hash já foi marcado como falho */
bool isStateFailed(unsigned long long int hash)
{
    int index = (int)(mix(hash) & (HASH_TABLE_SIZE - 1));

    if ((hashTable[index].key == hash) && hashTable[index].isFailed)
        return true;

    return false;
}

/* Salva o estado atual como falho na tabela de transposição */
void saveFailedState(unsigned long long int hash)
{
    int index = (int)(mix(hash) & (HASH_TABLE_SIZE - 1));
    hashTable[index].key = hash;
    hashTable[index].isFailed = true;
}
