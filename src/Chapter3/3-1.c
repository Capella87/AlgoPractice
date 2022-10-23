#include <stdio.h>

int H[101];
int n = 0;

void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 루트 노드가 제거되고 마지막 노드를 루트에 삽입한 후 힙 조건을 만족하기 위한 작업
void downHeap(int i)
{
    if (i > n) return;
    
    // 자식 노드 위치
    int child_idx = i * 2;
    int target_idx = i;

    // 왼쪽 자식이 더 큰 경우
    if (child_idx <= n && H[target_idx] < H[child_idx])
        target_idx = child_idx;

    // 오른쪽 자식이 더 큰 경우
    if (++child_idx <= n && H[target_idx] < H[child_idx])
        target_idx = child_idx;

    // 자식 노드와 바꿔야 하는 경우
    if (target_idx != i)
    {
        // 교환
        swap(&H[target_idx], &H[i]);
        // 교환된 값을 힙 조건을 만족할 때까지 재귀 호출
        downHeap(target_idx);
    }
}

// 마지막 노드를 만들어 삽입한 후 힙 조건을 만족하기 위한 작업
void upHeap(int i)
{
    if (i <= 1) return;

    int parent_idx = i / 2;
    
    // 부모 노드보다 현재 노드가 더 큰 경우
    if (H[parent_idx] < H[i])
    {
        // 값 교환
        swap(&H[parent_idx], &H[i]);
        // 교환된 위치에서 힙 조건 만족시까지 재귀 호출
        upHeap(parent_idx);
    }
}

// 힙에 삽입
void insertItem(const int key)
{
    if (n >= 100) return;

    H[++n] = key;
    upHeap(n);
    printf("0\n");
}

// 루트 노드 제거
int removeMax()
{
    if (n < 1) return -1;

    int rt = H[1];
    H[1] = H[n--];
    downHeap(1);
    
    return rt;
}

// 힙 내 요소 출력
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