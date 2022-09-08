#include <stdio.h>
#include <stdlib.h>

void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void sort(int* arr, const int size)
{
    int idx = arr[size - 1];

    for (int i = size - 1; i > 0; i--)
    {
        int idx = i;

        for (int j = i - 1; j >= 0; j--)
            if (arr[j] > arr[idx])
                idx = j;
        if (idx != i) swap(&arr[idx], &arr[i]);
    }
}

int main(void)
{
    int n;

    scanf("%d", &n);
    int* arr = (int*)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
        scanf("%d", &arr[i]);
    sort(arr, n);

    for (int i = 0; i < n; i++)
        printf(" %d", arr[i]);
    putchar('\n');

    free(arr);
    return 0;
}