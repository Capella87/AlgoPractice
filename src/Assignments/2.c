#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <Windows.h>

int n = 100000;
const int limits[4] = { 1, 100, 500, 1000 };
typedef enum mode { deterministic1, deterministic3, randomized1, randomized3 } mode;

// rand()를 확장한 난수 생성기
int get_random(int min, int max)
{
    return ((((long)rand() << 15) | rand()) % (max - min)) + min;
}

// 난수로 채운 배열 생성 및 반환
int* createArray(void)
{
    int* rt = (int*)malloc(sizeof(int) * n);

    srand((unsigned)time(NULL));
    for (int i = 0; i < n; i++)
        rt[i] = get_random(1, n);

    return rt;
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

// 배열을 복사해 반환하는 함수
int* copy_array(int* original, int size)
{
    int* rt = (int*)malloc(sizeof(int) * size);
    memcpy(rt, original, sizeof(int) * size);

    return rt;
}

// 리스트 내 요소 출력하는 함수
void writeList(int* arr, int size)
{
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    putchar('\n');
}

// 삽입 정렬; limit 밑으로 내려가면 시행된다.
void insertionSort(int* arr, int n)
{
    for (int i = 1; i < n; i++)
    {
        int temp = arr[i], j = i - 1;

        while (j >= 0 && arr[j] >= temp)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

// 두 값을 서로 바꾸는 함수
inline void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 제자리 분할 함수; 중복 숫자를 지원한다.
void inPlacePartition(int* arr, int l, int r, int k, int* lb, int* ub)
{
    // i -> 탐색할 때 사용되는 것
    int i = l;
    *lb = l, *ub = r; // pivot과 같은 수의 왼쪽 경계와 오른쪽 경계

    // pivot으로 지목된 수를 맨 앞으로 옮긴다
    swap(&arr[l], &arr[k]);
    // pivot의 값을 편의를 위해 따로 저장
    int pivot = arr[l];

    while (i <= *ub)
    {
        // 요약: pivot의 범위를 일단 길이 전체로 잡고 pivot보다 작거나 큰 수는 그 범위 바깥으로 이동시키고 범위를 줄이는 방식으로 구현
        if (arr[i] < pivot) // pivot보다 작은 경우
            swap(&arr[i++], &arr[(*lb)++]); // lb 자리로 값을 이동시키고 i와 *lb 값 증가
        else if (pivot < arr[i]) // pivot보다 큰 경우
            swap(&arr[i], &arr[(*ub)--]); // pivot보다 큰 경우 ub 자리로 값을 이동시키고 *ub 값 감소
        else i++; // pivot과 같은 값은 pivot 범위에 있어야 하므로 그냥 i만 증가
    }
}

// 삼항 연산자를 이용한 중간값을 반환하는 함수
int findIndexOfMedianOfThree(int* arr, int a, int b, int c)
{
    return arr[a] > arr[b] ? (arr[b] > arr[c] ? c : (arr[a] > arr[c] ? c : a))
        : (arr[a] > arr[c] ? a : (arr[b] > arr[c] ? c : b));
}

// pivot을 조건에 따라 잡아 그 index를 반환하는 함수
int findPivot(int* arr, int l, int r, mode mod)
{
    // rand()를 위한 것
    srand((unsigned)time(NULL));

    // 그냥 맨 오른쪽 것을 반환
    if (mod == deterministic1)
        return r;

    // l, r간 한 수를 랜덤으로 잡아서 반환
    if (mod == randomized1)
    {
        int rt = get_random(l, r);
        return rt;
    }

    // 두 수만 범위에 있는 경우 l을 반환
    if (r - l == 1)
        return l;

    // 그 외의 경우
    int a, b, c;
    switch (mod)
    {
    case deterministic3: // 세 index를 각각 l, 중간, r로 잡는다
        a = l;
        b = (l + r) / 2;
        c = r;
        break;
    case randomized3: // 세 index를 랜덤으로 잡는다
        a = get_random(l, r);
        b = get_random(l, r);
        c = get_random(l, r);
        break;
    }

    // 위의 두가지 경우 세 index의 중간값을 가진 index를 반환
    return findIndexOfMedianOfThree(arr, a, b, c);
}

// 재귀 방식의 퀵 정렬
void rQuickSort(int* arr, int l, int r, mode mod, int limit)
{
    // 구간 limit보다 넓거나 같을 때까지만 퀵 정렬을 수행
    if (r - l >= limit)
    {
        int k = findPivot(arr, l, r, mod); // pivot 반환
        int a, b;
        inPlacePartition(arr, l, r, k, &a, &b); // 제자리 분할
        rQuickSort(arr, l, a - 1, mod, limit); // 왼쪽 범위 퀵 정렬
        rQuickSort(arr, b + 1, r, mod, limit); // 오른쪽 범위 퀵 정렬
    }
}

// 퀵 정렬; 정확히는 limit 전까지만 퀵 정렬을 수행하고 그 미만 범위는 삽입 정렬로 처리
void quickSort(int* arr, int size, mode mod, int limit)
{
    rQuickSort(arr, 0, size - 1, mod, limit); // 퀵 정렬 수행
    if (limit > 1) // limit이 1보다 큰 경우에만 삽입 정렬 수행
        insertionSort(arr, n);
}

int main(void)
{
    int* arr = createArray(); // 배열 생성

    LARGE_INTEGER ticksPerSec, start, end;
    printf("Limit\t결정적1\t\t결정적3\t\t무작위1\t\t무작위3\n");
    for (int i = 0; i < 4; i++)
    {
        printf("%d\t", limits[i]); // limit 인쇄
        for (int j = 0; j < 4; j++)
        {
            int* clone = copy_array(arr, n); // 배열 복사

            StartTimer(&ticksPerSec, &start); // 시작
            quickSort(clone, n, j, limits[i]); // 정렬 수행
            EndTimer(&end); // 종료

            printf("%.08lf\t", GetExecutionTime(&start, &end, &ticksPerSec)); // 실행 시간 수행(ms)
            free(clone); // 정렬된 배열 할당 해제
        }
        putchar('\n');
    }

    free(arr);
    return 0;
}