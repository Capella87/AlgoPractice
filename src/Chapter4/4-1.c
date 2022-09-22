#include <stdio.h>
#include <stdlib.h>

int H[101];
int n;

void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void downHeap(int target_idx, int size)
{
    if (!size) return;

    int largest = target_idx;
    int child_idx = target_idx * 2;

    if (child_idx <= size && H[largest] < H[child_idx])
        largest = child_idx;
    if (++child_idx <= size && H[largest] < H[child_idx])
        largest = child_idx;
    if (target_idx != largest)
    {
        swap(&H[target_idx], &H[largest]);
        downHeap(largest, size);
    }
}

void buildHeap(void)
{
    for (int i = n / 2; i >= 1; i--)
        downHeap(i, n);
}

void removeHeap(void)
{
    for (int i = n; i >= 1; i--)
    {
        swap(&H[1], &H[i]);
        downHeap(1, i - 1);
    }
}

void inPlaceHeapSort(void)
{
    buildHeap();
    removeHeap();
}

void printArray(void)
{
    for (int i = 1; i <= n; i++)
        printf(" %d", H[i]);
    putchar('\n');
}


int main(void)
{
    scanf("%d", &n);
    for (int i = 1; i <= n; i++)
        scanf("%d", &H[i]);
    inPlaceHeapSort();
    printArray();

    return 0;
}