#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int findPivot(int min, int max)
{
    srand((unsigned)time(NULL));
    return min + rand() % (max - min);
}

int inPlacePartition(int* arr, int left, int right, int k)
{
    int pivot = arr[k];
    if (k != right) swap(&arr[k], &arr[right]);

    int i = left, j = right - 1;
    while (i <= j)
    {
        while (i <= j && arr[i] <= pivot)
            i++;
        while (i <= j && pivot <= arr[j])
            j--;
        if (i < j) swap(&arr[i], &arr[j]);
    }
    swap(&arr[i], &arr[right]);

    return i;
}

void quick_sort(int* arr, int left, int right)
{
    if (left >= right) return;

    int pivot_idx = findPivot(left, right);
   
    pivot_idx = inPlacePartition(arr, left, right, pivot_idx);
    quick_sort(arr, left, pivot_idx - 1);
    quick_sort(arr, pivot_idx + 1, right);
}

void print_array(int* arr, int size)
{
    for (int i = 0; i < size; i++)
        printf(" %d", arr[i]);
    putchar('\n');
}

int main(void)
{
    int n;
    
    scanf("%d", &n);
    int* arr = (int*)malloc(sizeof(int) * n);
    
    for (int i = 0; i < n; i++)
        scanf("%d", &arr[i]);
    
    quick_sort(arr, 0, n - 1);
    print_array(arr, n);
    
    free(arr);
    return 0;
}