// BFS : Breadth First Search

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct qnode
{
    struct qnode* next;
    struct qnode* prev;
    int data;
} qnode;

typedef struct queue
{
    struct qnode* header;
    struct qnode* trailer;
    int count;
} queue;

int n, m, s;

qnode* creat_qnode(int data)
{
    qnode* rt = (qnode*)malloc(sizeof(qnode));
    rt->data = data;
    rt->next = rt->prev = NULL;

    return rt;
}

queue* queue_new(void)
{
    queue* rt = (queue*)malloc(sizeof(queue));
    rt->header = creat_qnode(-1);
    rt->trailer = creat_qnode(-1);

    rt->header->next = rt->trailer;
    rt->trailer->prev = rt->header;
    rt->count = 0;

    return rt;
}

bool queue_is_empty(queue* q)
{
    return !q->count ? true : false;
}

// Insert a node before the trailer of queue;
void queue_enqueue(queue* q, int data)
{
    qnode* n = creat_qnode(data);

    n->next = q->trailer;
    n->prev = q->trailer->prev;
    q->trailer->prev->next = n;
    q->trailer->prev = n;
    q->count++;
}

int queue_dequeue(queue* q)
{
    if (queue_is_empty(q))
    {
        puts("X");
        return -1;
    }

    qnode* target = q->header->next;
    q->header->next = target->next;
    target->next->prev = q->header;

    int rt = target->data;
    free(target);
    q->count--;

    return rt;
}

int queue_front(queue* q)
{
    if (queue_is_empty(q))
    {
        puts("X");
        return -1;
    }

    return q->header->next->data;
}

int queue_back(queue* q)
{
    if (queue_is_empty(q))
    {
        puts("X");
        return -1;
    }

    return q->trailer->prev->data;
}

void queue_free(queue* q)
{
    if (!queue_is_empty(q))
        queue_dequeue(q);
    free(q->header);
    free(q->trailer);
    free(q);
}

void free_all(int** arr, bool* is_visited)
{
    for (int i = 0; i <= n; i++)
        free(arr[i]);
    free(arr);
    free(is_visited);
}

void bfs(int** arr, bool* status, int start)
{
    queue* q = queue_new();

    status[start] = true;
    queue_enqueue(q, start);
    while (!queue_is_empty(q))
    {
        int vertex = queue_dequeue(q);
        printf("%d\n", vertex);
        for (int i = 1; i <= n; i++)
        {
            if (arr[vertex][i] == 1 && !status[i])
            {
                queue_enqueue(q, i);
                status[i] = true;
            }
        }
    }

    queue_free(q);
}

int main(void)
{
    int src, dest;

    scanf("%d %d %d", &n, &m, &s);
    int** arr = (int**)malloc(sizeof(int*) * (n + 1));
    bool* is_visited = (bool*)malloc(sizeof(bool) * (n + 1));
    for (int i = 0; i <= n; i++)
    {
        arr[i] = (int*)calloc(n + 1, sizeof(int));
        for (int j = 0; j <= n; j++)
        is_visited[i] = false;
    }

    for (int i = 0; i < m; i++)
    {
        scanf("%d %d", &src, &dest);
        arr[src][dest] = 1;
        arr[dest][src] = 1;
    }

    bfs(arr, is_visited, s);

    free_all(arr, is_visited);
    return 0;
}