#include <stdio.h>
#include <stdlib.h>

void sort(int* arr, const int size)
{
    for (int i = 1; i < size; i++)
    {
        int temp = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > temp)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
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