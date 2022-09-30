#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct node
{
    struct node* next;
    int data;
} node;

typedef struct list
{
    node* header;
    node* last;
    node* last_prev;
    int count;
} list;

list* init_list(void)
{
    list* rt = (list*)malloc(sizeof(list));
    rt->count = 0;
    rt->header = NULL;
    rt->last = NULL;
    rt->last_prev = NULL;

    return rt;
}

node* creat_node(const int data)
{
    node* rt = (node*)malloc(sizeof(node));
    rt->next = NULL;
    rt->data = data;
    
    return rt;
}

void add(list** lst, const int data)
{
    node* n = creat_node(data);

    if (!(*lst)->header) (*lst)->header = n;
    else (*lst)->last->next = n;

    (*lst)->last_prev = (*lst)->last;
    (*lst)->last = n;
    (*lst)->count++;
}

int pop(list** lst)
{
    if (!(*lst)->count) return -1;

    int rt = (*lst)->header->data;
    
    node* target = (*lst)->header;
    node* nxt = target->next;
    (*lst)->header = nxt;
    (*lst)->count--;

    free(target);
    return rt;
}

node* extract(list** lst)
{
    if (!(*lst)->count) return NULL;

    node* target = (*lst)->header;
    node* nxt = target->next;
    (*lst)->header = nxt;
    (*lst)->count--;

    return target;
}

void insert(list** lst, node* node)
{
    if (!(*lst)->header)
        (*lst)->header = node;
    else
        (*lst)->last->next = node;
    (*lst)->last_prev = (*lst)->last;
    (*lst)->last = node;
    (*lst)->count++;
}

void free_all(list* lst)
{
    while (lst->count)
        pop(&lst);
    free(lst);
}

void Partition(list** lst, list** a, list** b)
{
    if ((*lst)->count <= 1) return;

    int count = (*lst)->count;

    int mid = count / 2;
    int idx = 0;
    node* cur = (*lst)->header;
    node* cur_prev = (*lst)->header;
    node* cur_pp = (*lst)->header;

    while (idx < mid)
    {
        cur_prev = cur;
        cur = cur->next;
        idx++;
    }

    (*a)->header = (*lst)->header;
    (*a)->count = idx;
    (*a)->last = cur_prev;
    cur_prev->next = NULL;

    (*lst)->header = NULL;
    (*lst)->count = 0;
    (*lst)->last = (*lst)->last_prev = NULL;

    (*b)->header = cur;
    (*b)->count = count - mid;
}

void Merge(list** original, list** a, list** b, list** buffer)
{
    while ((*a)->count > 0 && (*b)->count > 0)
    {
        if ((*a)->header->data < (*b)->header->data)
            insert(buffer, extract(a));
        else insert(buffer, extract(b));
    }
    while ((*a)->count > 0)
        insert(buffer, extract(a));
    while ((*b)->count > 0)
        insert(buffer, extract(b));
    
    while ((*buffer)->count)
        insert(original, extract(buffer));
}

void mergeSort(list** l, list** buffer, int size)
{
    if (size <= 1) return;

    list* a = init_list();
    list* b = init_list();
    
    Partition(l, &a, &b);
    mergeSort(&a, buffer, a->count);
    mergeSort(&b, buffer, b->count);
    Merge(l, &a, &b, buffer);

    free_all(a);
    free_all(b);
}

void print_list(list* lst)
{
    node* cur = lst->header;

    while (cur)
    {
        printf(" %d", cur->data);
        cur = cur->next;
    }
    putchar('\n');
}

int main(void)
{
    int n, input;
    list* lst = init_list();
    list* buffer = init_list();

    scanf("%d", &n);
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &input);
        add(&lst, input);
    }
    mergeSort(&lst, &buffer, n);
    print_list(lst);

    free_all(buffer);
    free_all(lst);
    return 0;
}