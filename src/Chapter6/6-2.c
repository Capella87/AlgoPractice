#include <stdio.h>
#include <stdlib.h>

int search_nonrecursive(int* arr, int left, int right, int key)
{
    int size = right + 1;

    while (left < right)
    {
        int mid = (left + right) / 2;
        
        if (arr[mid] == key)
        {
            left = mid;
            break;
        }
        else if (arr[mid] < key) left = mid + 1;
        else right = mid;
    }

    return key <= arr[left] ? left : size;
}

int main(void)
{
    int n, k;

    scanf("%d %d", &n, &k);
    int* arr = (int*)malloc(sizeof(int) * n);

    for (int i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    printf(" %d\n", search_nonrecursive(arr, 0, n - 1, k));

    free(arr);
    return 0;
}