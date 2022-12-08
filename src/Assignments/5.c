#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

#include <Windows.h>

#define MIN(a, b) (((a) > (b)) ? (b) : (a))

const int MAX = 30;

// Get value by using Dynamic Programming
int airtelDP(int* fare, int* hotel_price, int* cache, int src, int dest)
{
    for (int i = src + 1; i <= dest; i++)
    {
        // No stopover
        cache[i] = cache[src] + fare[i - src];

        // Stopover
        for (int j = src + 1; j < i; j++)
        {
            int cost = cache[j] + hotel_price[j] + fare[i - j];
            cache[i] = MIN(cache[i], cost);
        }
    }

    return cache[dest];
}

// Get value by using Divide and Conquer (Forward)
int airtelDC(int* fare, int* hotel_price, int src, int dest)
{
    if (dest == src) return 0;

    // No stopover
    int min_cost = airtelDC(fare, hotel_price, src, src) + fare[dest - src];

    // Stopover
    for (int i = src + 1; i < dest; i++)
        min_cost = MIN(min_cost, airtelDC(fare, hotel_price, src, i) + hotel_price[i] + fare[dest - i]);
    
    return min_cost;
}

// 측정 시간을 마이크로세컨드 단위로 반환하는 함수
double GetExecutionTime(LARGE_INTEGER* start, LARGE_INTEGER* end, LARGE_INTEGER* ticksPerSec)
{
    LARGE_INTEGER diff;
    diff.QuadPart = end->QuadPart - start->QuadPart;

    // 1000을 곱해 ms 단위로 변환
    return 1000 * ((double)diff.QuadPart / (double)ticksPerSec->QuadPart);
}

// 시간 측정 시작하는 함수
void StartTimer(LARGE_INTEGER* ticksPerSec, LARGE_INTEGER* start)
{
    QueryPerformanceFrequency(ticksPerSec);
    QueryPerformanceCounter(start);
}

// 시간 측정을 끝내는 함수
void EndTimer(LARGE_INTEGER* end)
{
    QueryPerformanceCounter(end);
}

void PrintCPUTime(int n, int s, int d, int mincost, const char* version, LARGE_INTEGER* start, LARGE_INTEGER* end, LARGE_INTEGER* ticksPerSec)
{
    printf("%d\t%d\t%d\t%d\t\t%s\t\t%.08lf\n", n, s, d, mincost, version, GetExecutionTime(start, end, ticksPerSec));
}

void ResetCache(int* cache, int size)
{
    for (int i = 0; i < size; i++)
        cache[i] = 0;
}

int main(void)
{
    int sd[3][2] = { {0, 4}, {2, 5}, {2, 4} };
    int s, d, n;

    int* A = (int*)calloc(MAX, sizeof(int));
    A[1] = 1;
    for (int i = 2; i < MAX; i++)
        A[i] = A[i - 1] + A[i - 1] % 5 + 3;

    int* H = (int*)calloc(MAX, sizeof(int));
    H[1] = 5;
    for (int i = 2; i < MAX; i++)
        H[i] = (H[i - 1] + i) % 9 + 1;
    int* cache = (int*)calloc(MAX, sizeof(int));

    printf("n\ts\td\tmincost\t\tversion\t\tcputime\n");

    n = 6;
    int result = 0;
    LARGE_INTEGER start, end, ticksPerSec;
    for (int i = 0; i < 3; i++)
    {
        StartTimer(&ticksPerSec, &start);
        result = airtelDC(A, H, sd[i][0], sd[i][1]);
        EndTimer(&end);
        PrintCPUTime(n, sd[i][0], sd[i][1], result, "DC", &start, &end, &ticksPerSec);

        StartTimer(&ticksPerSec, &start);
        result = airtelDP(A, H, cache, sd[i][0], sd[i][1]);
        EndTimer(&end);
        PrintCPUTime(n, sd[i][0], sd[i][1], result, "DP", &start, &end, &ticksPerSec);

        ResetCache(cache, MAX);
    }

    n = MAX;
    s = 1, d = n - 2;

    StartTimer(&ticksPerSec, &start);
    result = airtelDC(A, H, s, d);
    EndTimer(&end);
    PrintCPUTime(n, s, d, result, "DC", &start, &end, &ticksPerSec);

    StartTimer(&ticksPerSec, &start);
    result = airtelDP(A, H, cache, s, d);
    EndTimer(&end);
    PrintCPUTime(n, s, d, result, "DP", &start, &end, &ticksPerSec);

    puts("\nNote: cputime is ms");

    free(A);
    free(H);
    free(cache);
    return 0;
}