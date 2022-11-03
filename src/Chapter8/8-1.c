#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    struct node* next;
    int key;
} node;

typedef struct htable
{
    node* chain;
    int count;
} htable;

int m;

htable* init_table(void)
{
    htable* rt = (htable*)malloc(sizeof(htable) * m);
    
    for (int i = 0; i < m; i++)
    {
        rt[i].chain = (void*)0;
        rt[i].count = 0;
    }

    return rt;
}

node* creat_node(int key)
{
    node* rt = (node*)malloc(sizeof(node));
    rt->next = (void*)0;
    rt->key = key;

    return rt;
}

int get_hash(int key)
{
    return key % m;
}

node* find_element(htable* table, node** prev, int* r_value, int* r_seq, int key)
{
    int value = get_hash(key);

    if (r_value) *r_value = value;

    if (!table[value].count) return (void*)0;

    node* pos = table[value].chain;
    node* ppos = pos;
    int seq = 1;
    while (pos && pos->key != key)
    {
        ppos = pos;
        pos = pos->next;
        seq++;
    }

    if (r_seq) *r_seq = seq;
    if (prev) *prev = ppos;
    
    return pos;
}

void insert_element(htable** table, int key)
{
    int value = 0;
    node* n = find_element(*table, (void*)0, &value, (void*)0, key);

    if (n) return;

    n = creat_node(key);

    // Insert
    if ((*table)[value].count)
    {
        node* t = (*table)[value].chain;
        n->next = t;
    }
    (*table)[value].chain = n;
    (*table)[value].count++;
}

void remove_element(htable** table, int key)
{
    node* prev = (void*)0;
    int value = 0;
    int seq = 0;
    node* n = find_element(*table, &prev, &value, &seq, key);

    if (!n)
    {
        puts("0");
        return;
    }

    if ((*table)[value].chain == n)
        (*table)[value].chain = n->next;
    else prev->next = n->next;

    (*table)[value].count--;
    free(n);
    printf("%d\n", seq);
}

void search_element(htable* table, int key)
{
    int seq = 0;
    node* n = find_element(table, (void*)0, (void*)0, &seq, key);

    if (!n)
    {
        puts("0");
        return;
    }

    printf("%d\n", seq);
}

void print_element(htable* table)
{
    for (int i = 0; i < m; i++)
    {
        node* pos = table[i].chain;
        while (pos)
        {
            printf(" %d", pos->key);
            pos = pos->next;
        }
    }
    putchar('\n');
}

void free_all(htable* table)
{
    if (!table) return;

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < table[i].count; j++)
        {
            node* t = table[i].chain;
            table[i].chain = t->next;
            free(t);
        }
    }

    free(table);
}

int main(void)
{
    int x;

    scanf("%d", &m);
    getchar();
    htable* ht = init_table();

    while (1)
    {
        char c = getchar();
        getchar();
        
        switch (c)
        {
        case 'e':
            goto exit;

        case 'i':
            scanf("%d", &x);
            getchar();
            
            insert_element(&ht, x);
            break;

        case 'd':
            scanf("%d", &x);
            getchar();

            remove_element(&ht, x);
            break;

        case 's':
            scanf("%d", &x);
            getchar();

            search_element(ht, x);
            break;

        case 'p':
            print_element(ht);
            break;
        }
    }
    
exit:
    free_all(ht);
    return 0;
}