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
    int target_idx = i;
    if (child_idx <= n && H[target_idx] < H[child_idx])
        target_idx = child_idx;
    if (++child_idx <= n && H[target_idx] < H[child_idx])
        target_idx = child_idx;

    if (target_idx != i)
    {
        swap(&H[target_idx], &H[i]);
        downHeap(target_idx);
    }
}

void upHeap(int i)
{
    if (i <= 1) return;

    int parent_idx = i / 2;
    
    if (H[parent_idx] < H[i])
    {
        swap(&H[parent_idx], &H[i]);
        upHeap(parent_idx);
    }
}

void insertItem(const int key)
{
    if (n >= 100) return;

    H[++n] = key;
    upHeap(n);
    printf("0\n");
}

int removeMax()
{
    if (n < 1) return -1;

    int rt = H[1];
    H[1] = H[n--];
    downHeap(1);
    
    return rt;
}

void printHeap()
{
    if (n < 1) return;

    for (int i = 1; i <= n; i++)
        printf(" %d", H[i]);
    putchar('\n');
}

int main(void)
{
    char command;
    int target;

    while (1)
    {
        scanf("%c", &command);
        getchar();
        if (command == 'i')
        {
            scanf("%d", &target);
            getchar();
            insertItem(target);
        }
        else if (command == 'p')
            printHeap();
        else if (command == 'd')
            printf("%d\n", removeMax());
        else if (command == 'q')
            break;
    }

    return 0;
}