#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define chunksize 2
#define minN 1
#define maxN 10
#define N (maxN - minN + 1)
#define SIZE 10

int digits[SIZE];

int main() {
    omp_set_dynamic(0);
    omp_set_num_threads(4);

#pragma omp parallel for schedule(static, chunksize)
    for (int i = 0; i < SIZE; i++) {
        srand(time(NULL) ^ i);
        int r = rand() % (N) + minN;
        digits[i] = r;
    }

    int count[maxN + 1] = {0};
#pragma omp parallel for shared(count)
    for (int i = 0; i < SIZE; i++)
#pragma omp atomic
        count[digits[i]]++;

    for (int i = 0; i < SIZE; i++) printf("%i ", digits[i]);
    printf("\n");
    for (int i = 0; i < maxN + 1; i++) printf("%i ", count[i]);
    printf("\n");

    int win = 0;
#pragma omp parallel for reduction(+ : win) schedule(static, chunksize)
    for (int i = 0; i < maxN + 1; i++) {
        if (count[i] > 1) win++;
    }

    printf("Wins: %i", win);
}
