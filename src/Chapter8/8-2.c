#include <stdio.h>
#include <stdlib.h>

// Hash table
typedef struct htable
{
    int data;
} htable;

int m, n;

htable* init_table(void)
{
    htable* rt = (htable*)malloc(sizeof(htable) * m);

    for (int i = 0; i < m; i++)
    {
        rt[i].data = 0;
    }

    return rt;
}

// Hash function
int get_hash(int key)
{
    return key % m;
}

// 존재하는 경우 그 위치를 찾아 반환한다.
int find_element(htable* table, int key)
{
    if (!table) return -1;

    int value = get_hash(key);
    
    if (!table[value].data) return -1;
    else if (table[value].data == key) return value;

    int idx = (value + 1) % m;
    while (table[idx].data && table[idx].data != key)
    {
        idx = ++idx % m;
    }

    return table[idx].data == key ? idx : -1;
}

int find_place(htable* table, int* ccount, int key)
{
    int value = get_hash(key);

    if (!table[value].data) return value;
    
    int idx = (value + 1) % m;
    *ccount = 1;
    while (table[idx].data && idx != value)
    {
        (*ccount)++;
        idx = ++idx % m;
    }

    return idx;
}

void insert_element(htable** table, int key)
{
    if (!table) return;

    int value = find_element(*table, key);

    if (value != -1) return;

    int collisions = 0;
    value = find_place(*table, &collisions, key);

    (*table)[value].data = key;

    for (int i = 0; i < collisions; i++)
        putchar('C');
    printf("%d\n", value);
}

void search_element(htable* table, int key)
{
    int value = find_element(table, key);

    if (value == -1)
    {
        puts("-1");
        return;
    }

    printf("%d %d\n", value, key);
}

int main(void)
{
    int x;

    scanf("%d %d", &m, &n);
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

        case 's':
            scanf("%d", &x);
            getchar();

            search_element(ht, x);
            break;
        }
    }

exit:
    free(ht);
    return 0;
}