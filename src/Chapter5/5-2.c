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

void inPlacePartition(int* arr, int left, int right, int k, int* lb, int* ub)
{
    int pivot = arr[k];
    if (k != left) swap(&arr[k], &arr[left]);

    *lb = left, *ub = right; // lower bound and upper bound
    int i = left;

    while (i <= *ub)
    {
        if (arr[i] < pivot)
            swap(&arr[i++], &arr[(*lb)++]); // Relocate arr[i] before the lower bound of pivot
        else if (arr[i] == pivot)
            i++; // It should be remained inside the boundary
        else
            swap(&arr[i], &arr[(*ub)--]); // Relocate arr[i] after the upper bound of pivot
            // But i must not be moved further 
            // because swapped element should be compared to pivot whether it is smaller or equal to the pivot.
    }
}

void quick_sort(int* arr, int left, int right)
{
    if (left >= right) return;

    int pivot_idx = findPivot(left, right);

    int lb, ub;
    inPlacePartition(arr, left, right, pivot_idx, &lb, &ub);
    quick_sort(arr, left, lb - 1);
    quick_sort(arr, ub + 1, right);
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