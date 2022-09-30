#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void merge(int* arr, int* buffer, int start, int mid, int end)
{
    int buf_idx = start;
    int a_idx = start, b_idx = mid + 1;
    
    while (a_idx <= mid && b_idx <= end)
    {
        if (arr[a_idx] > arr[b_idx])
            buffer[buf_idx++] = arr[b_idx++];
        else buffer[buf_idx++] = arr[a_idx++];
    }
    while (a_idx <= mid)
        buffer[buf_idx++] = arr[a_idx++];
    while (b_idx <= end)
        buffer[buf_idx++] = arr[b_idx++];
    for (int i = start; i <= end; i++)
        arr[i] = buffer[i];
}

void mergeSort(int* arr, int* buffer, int start, int end)
{
    if (start == end) return;
    
    int mid = (start + end) / 2;
    
    mergeSort(arr, buffer, start, mid);
    mergeSort(arr, buffer, mid + 1, end);
    merge(arr, buffer, start, mid, end);
}

void print_array(int* arr, int n)
{
    for (int i = 0; i < n; i++)
        printf(" %d", arr[i]);
    putchar('\n');
}

int main(void)
{
    int n;
    int* l, *buffer;

    scanf("%d", &n);
    l = (int*)malloc(sizeof(int) * n);
    buffer = (int*)malloc(sizeof(int) * n);

    for (int i = 0; i < n; i++)
        scanf("%d", &l[i]);

    mergeSort(l, buffer, 0, n - 1);
    print_array(l, n);

    free(l);
    free(buffer);
    return 0;
}