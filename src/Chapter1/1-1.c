// The implementation of character list ADT

#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    struct node* next;
    struct node* prev;
    char elem;
} node;

typedef struct list
{
    struct node* head;
    struct node* trailer;
    int count;
} list;

node* create_node(const char e)
{
    node* new_node = (node*)malloc(sizeof(node));
    new_node->next = new_node->prev = NULL;
    new_node->elem = e;

    return new_node;
}

list* init_list(void)
{
    list* l = (list*)malloc(sizeof(list));
    l->head = create_node(0);
    l->trailer = create_node(0);
    
    l->head->next = l->trailer;
    l->trailer->prev = l->head;
    l->count = 0;

    return l;
}

// Find a proper location to insert
node* find_location(list* l, const int r)
{
    if (l->count + 1 < r) return NULL;

    node* pos = l->head;
    int idx = 0;

    while (idx < r && pos->next != NULL)
    {
        pos = pos->next;
        idx++;
    }

    return pos;
}

void add(list* l, const int r, const char e)
{
    node* location = find_location(l, r);
    if (location == NULL)
    {
        printf("invalid position\n");
        return;
    }

    node* target = create_node(e);
    target->next = location;
    target->prev = location->prev;
    location->prev->next = target;
    location->prev = target;
    l->count++;
}

void delete(list* l, const int r)
{
    node* location = find_location(l, r);

    if (location == NULL || location == l->trailer || location == l->head)
    {
        printf("invalid position\n");
        return;
    }

    location->prev->next = location->next;
    location->next->prev = location->prev;
    free(location);
    l->count--;
}

void get_entry(list* l, const int r)
{
    node* target = find_location(l, r);
    
    if (target == NULL || target == l->head || target == l->trailer)
    {
        printf("invalid position\n");
        return;
    }

    printf("%c\n", target->elem);
}

void print(list* l)
{
    node* pos = l->head->next;
    
    while (pos != l->trailer)
    {
        printf("%c", pos->elem);
        pos = pos->next;
    }
    putchar('\n');
}

int main(void)
{
    int n, r;
    char command, target;

    scanf("%d", &n);
    getchar();

    list* l = init_list();
    while (n > 0)
    {
        scanf("%c", &command);
        if (command == 'P')
            print(l);
        else if (command == 'A')
        {
            getchar();
            scanf("%d %c", &r, &target);
            add(l, r, target);
        }
        else if (command == 'D')
        {
            getchar();
            scanf("%d", &r);
            delete(l, r);
        }
        else if (command == 'G')
        {
            getchar();
            scanf("%d", &r);
            get_entry(l, r);
        }
        getchar();
        n--;
    }

    return 0;
}