#include <stdio.h>

int H[101];
int n = 0;

void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void downHeap(int i)
{
    if (i > n) return;

    int child_idx = i * 2;
    int largest = i;

    if (child_idx <= n && H[child_idx] > H[largest])
        largest = child_idx;
    if (++child_idx <= n && H[child_idx] > H[largest])
        largest = child_idx;
    if (i != largest)
    {
        swap(&H[i], &H[largest]);
        downHeap(largest);
    }
}

// Non-recursive buildHeap
void buildHeap(void)
{
    for (int i = n / 2; i >= 1; i--)
        downHeap(i);
}

// Recursive buildHeap
void rBuildHeap(int i)
{
    if (i > n) return;

    rBuildHeap(i * 2);
    rBuildHeap(i * 2 + 1);
    downHeap(i);
}

void printHeap()
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
    buildHeap();
    printHeap();

    return 0;
}