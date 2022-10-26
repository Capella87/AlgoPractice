#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <Windows.h>

// 두 값을 바꾸는 함수
void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 랜덤으로 수를 생성해서 만든 배열에 넣고 그 배열을 반환
int* buildList(int n, int min, int max)
{
    int* rt = (int*)malloc(sizeof(int) * n);
    srand((unsigned)time(NULL)); // 아무튼 시간을 시드로 넣는다.
    
    for (int i = 0; i < n; i++) // rand()가 short만 반환해서 이런 걸 써야 한다.
        rt[i] = ((((long)rand() << 15) | rand()) % (max - min)) + min; // 출처: 이전 시간 실습
    
    return rt;
}

// 리스트 내 요소 출력하는 함수
void writeList(int* arr, int size)
{
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    putchar('\n');
}

// 힙을 형성하는 함수 O(n)
void heapify(int* arr, int size, int idx)
{
    if (idx >= size) return;

    // 이것은 최소 힙이다.
    int smallest = idx;
    int child_idx = idx * 2 + 1; // 자식 노드; 여기서는 왼쪽 먼저
    
    // 왼쪽 자식이 더 작은 경우
    if (child_idx < size && arr[smallest] > arr[child_idx])
        smallest = child_idx;
    // 오른쪽 자식이 더 작은 경우
    if (++child_idx < size && arr[smallest] > arr[child_idx])
        smallest = child_idx;
    // 현재 노드보다 더 작은 자식이 있는 경우
    if (smallest != idx)
    {
        swap(&arr[smallest], &arr[idx]); // 해당 노드와 선택한 자식 노드간 값 교환
        heapify(arr, size, smallest); // 해당 노드가 올바른 위치를 찾을 때까지 재귀
    }
}

// 힙에서 가장 작은 수를 빼는 함수
int pop(int* heap, int size)
{
    if (!size) return -1;

    int rt = heap[0];
    // 이것은 min heap이기 때문에 최솟값이 나온다.
    heap[0] = heap[size-- - 1]; // 여기서는 빼고 나서 가장 나중 노드에 있는 것을 맨 앞으로 가져 온다.
    heapify(heap, size - 1, 0); // 힙 성질 회복

    return rt; // 뺀 수 반환
}

// 기존의 배열을 복사해서 힙을 만들어 반환하는 함수
int* buildHeap(int* original, int size)
{
    int* copied = (int*)malloc(sizeof(int) * size);
    memcpy(copied, original, sizeof(int) * size); // 메모리 복사

    // 힙을 만든다.
    for (int i = size / 2; i >= 0; i--)
        heapify(copied, size, i);
    
    // 만든 것 반환
    return copied;
}

// k번째로 작은 수 반환하는 함수
int findKthSmallest(int* l, int size, int k)
{
    // 값을 복사해 힙을 만든다.
    int* heap = buildHeap(l, size);
    
    int rt = 0; // K번째로 작은 수가 나올 때까지 계속 값을 뺀다.
    for (int i = 0; i < k; i++)
        rt = pop(heap, size - i);

    // 힙을 동적 할당 해제하고 결과값을 반환
    free(heap);
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

int main(void)
{
    // 리스트 만들기
    int* L = buildList(10, 1, 100);
    int output[3];

    // 리스트 생성하기
    writeList(L, 10);
    for (int k = 1; k <= 3; k++)
        output[k - 1] = findKthSmallest(L, 10, k);
    // 결과값 출력
    writeList(output, 3);
    free(L);

    // 시간 측정하는 것
    L = buildList(100000, 1, 1000000);
    int karray[] = { 1, 100, 99900, 99999 };

    LARGE_INTEGER ticksPerSec, start, end;
    for (int i = 0; i <= 3; i++)
    {
        // 시간 측정 시작
        QueryPerformanceFrequency(&ticksPerSec);
        QueryPerformanceCounter(&start);

        // K번째로 작은 수 반환
        int e = findKthSmallest(L, 100000, karray[i]);

        QueryPerformanceCounter(&end);
        // 결과값 및 소요 시간 출력
        printf("%d, Execution time: %.12f microseconds\n", e, GetExecutionTime(&start, &end, &ticksPerSec));
    }

    return 0;
}