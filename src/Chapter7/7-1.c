#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct node
{
    struct node* parent;
    struct node* lChild;
    struct node* rChild;
    int key;
} node;

node* creat_node(int key)
{
    node* rt = (node*)malloc(sizeof(node));
    rt->parent = rt->lChild = rt->rChild = (void*)0;
    rt->key = key;

    return rt;
}

bool isExternal(node* n)
{
    return !n->lChild && !n->rChild ? true : false;
}

bool isInternal(node* n)
{
    return n->lChild || n->rChild ? true : false;
}

void insertItem(node** n, node* parent, int key)
{
    if (isExternal(*n))
    {
        *n = creat_node(key);
        (*n)->lChild = creat_node(-1);
        (*n)->rChild = creat_node(-1);
        (*n)->lChild->parent = (*n)->rChild->parent = *n;
        (*n)->parent = parent;

        return;
    }

    if (key < (*n)->key)
        insertItem(&((*n)->lChild), *n, key);
    else if (key == (*n)->key)
        return;
    else insertItem(&((*n)->rChild), *n, key);
}

int findElement(node* n, int key)
{
    if (isExternal(n))
    {
        puts("X");
        return -1;
    }

    if (key < n->key)
        return findElement(n->lChild, key);
    else if (key == n->key)
    {
        printf("%d\n", key);
        return key;
    }
    else return findElement(n->rChild, key);
}

node* treeSearch(node* n, int key)
{
    if (isExternal(n)) return n;

    if (key < n->key)
        return treeSearch(n->lChild, key);
    else if (key == n->key)
        return n;
    else return treeSearch(n->rChild, key);
}

node* sibling(node* n)
{
    if (!n->parent)
        return (void*)0;

    if (n->parent->lChild == n)
        return n->parent->rChild;
    else return n->parent->lChild;
}

void reduceExternal(node** parent, node* external, node** root)
{
    node* ano = sibling(external);
    
    if (!(*parent)->parent)
    {
        *root = ano;
        (*root)->parent = (void*)0;
    }
    else
    {
        node** pparent = &((*parent)->parent); // To preserve modifications
        ano->parent = *pparent;
        if (*parent == (*pparent)->lChild)
            (*pparent)->lChild = ano;
        else
            (*pparent)->rChild = ano;
    }

    free(*parent);
    free(external);
}

node* inOrderSucc(node* n)
{
    while (isInternal(n->lChild))
        n = n->lChild;
    return n;
}

int removeElement(node** root, int key)
{
    node* n = treeSearch(*root, key);
    
    if (isExternal(n))
    {
        puts("X");
        return -1;
    }
    
    int e = n->key;
    
    node* t = n->lChild;
    if (!isExternal(t))
        t = n->rChild;
    if (isExternal(t))
        reduceExternal(&n, t, root);
    else
    {
        node* successor = inOrderSucc(n->rChild);
        t = successor->lChild;
        
        node** nn = &n; // To preserve modifications
        (*nn)->key = successor->key;
        reduceExternal(&successor, t, root);
    }

    printf("%d\n", e);
    return e;
}

void free_all(node* n)
{
    if (!n) return;

    if (n->lChild) free_all(n->lChild);
    if (n->rChild) free_all(n->rChild);
    free(n);
}

void traversal(node* n, void (*way)(node*))
{
    way(n);
    putchar('\n');
}

void preorder(node* n)
{
    if (isExternal(n))
        return;

    printf(" %d", n->key);
    if (!isExternal(n->lChild))
        preorder(n->lChild);
    if (!isExternal(n->rChild))
        preorder(n->rChild);
}

int main(void)
{
    char command;
    int key;
    node* root = creat_node(-1);

    while (1)
    {
        command = getchar();

        if (command == 'q') break;
        getchar();
        switch (command)
        {
        case 'i':
            scanf("%d", &key);
            getchar();

            insertItem(&root, (void*)0, key);
            break;

        case 'd':
            scanf("%d", &key);
            getchar();

            removeElement(&root, key);
            break;

        case 's':
            scanf("%d", &key);
            getchar();

            findElement(root, key);
            break;

        case 'p':
            traversal(root, preorder);
            break;
        }
    }

    return 0;
}