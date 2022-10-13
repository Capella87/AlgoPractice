#include <stdio.h>
#include <stdlib.h>

int search_recursive(int* arr, int left, int right, int key)
{
    if (left == right)
    {
        if (arr[left] <= key) return left;
        else if (!left) return -1;
        else return left - 1;
    }

    int mid = (left + right) / 2;

    if (arr[mid] == key) return mid;
    else if (arr[mid] < key) return search_recursive(arr, mid + 1, right, key);
    else return search_recursive(arr, left, mid, key);
}

int main(void)
{
    int n, k;

    scanf("%d %d", &n, &k);
    int* arr = (int*)malloc(sizeof(int) * n);

    for (int i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    printf(" %d\n", search_recursive(arr, 0, n - 1, k));

    free(arr);
    return 0;
}