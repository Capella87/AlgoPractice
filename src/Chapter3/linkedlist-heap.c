#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node
{
    struct node* left;
    struct node* right;
    struct node* parent;
    bool is_leaf;
    int data;
} node;

typedef struct tree
{
    node* root;
    node* last_node;
    int count;
} tree;

node* new_node(const int data, bool is_leaf)
{
    node* rt = (node*)malloc(sizeof(node));
    rt->left = rt->parent = rt->right = NULL;
    rt->data = data;
    rt->is_leaf = is_leaf;

    return rt;
}

tree* init_tree(void)
{
    tree* rt = (tree*)malloc(sizeof(tree));
    rt->root = new_node(0, true);
    rt->count = 0;
    rt->last_node = rt->root;
    
    return rt;
}

void swap(node** first, node** second)
{
    int temp = (*first)->data;
    (*first)->data = (*second)->data;
    (*second)->data = temp;
}

void up_heap(node* target)
{
    while (target->parent && target->parent->data < target->data)
    {
        swap(&target->parent, &target);
        target = target->parent;
    }
}

void down_heap(node* target)
{
    while (!target->is_leaf)
    {
        node* t = target;
        if (!target->left->is_leaf && target->left->data > t->data)
            t = target->left;
        if (!target->right->is_leaf && target->right->data > t->data)
            t = target->right;
        if (target != t)
        {
            swap(&t, &target);
            target = t;
        }
        else break;
    }
}

node* find_location(tree* tree)
{
    node* pos = tree->last_node;

    while (pos->parent)
    {
        if (pos->parent->left == pos)
        {
            pos = pos->parent->right;
            break;
        }
        else pos = pos->parent;
    }

    while (!pos->is_leaf && pos->left)
    {
        pos = pos->left;
    }

    return pos;
}

node* refresh_last_node(node* n, bool is_switched)
{
    node* pos = n;
    if (pos->parent && !is_switched)
    {
        if (pos->parent->right == pos)
            pos = refresh_last_node(pos->parent->left, true);
        else if (pos->parent->left == pos)
            pos = refresh_last_node(pos->parent, is_switched);
    }
    else
    {
        if (pos->right && !pos->right->is_leaf)
            pos = refresh_last_node(pos->right, is_switched);
    }

    return pos;
}

void insert(tree** tr, const int data)
{
    // Find location;
    node* location = find_location(*tr);
    // replace leaf node in the location to new node;
    node* new_elem = new_node(data, false);
    new_elem->parent = location->parent;
    if (location->parent && location->parent->left == location)
        location->parent->left = new_elem;
    else if (location->parent && location->parent->right == location)
        location->parent->right = new_elem;
    else (*tr)->root = new_elem;

    location->parent = new_elem;
    new_elem->left = location;
    location = new_elem;

    // former leaf node to be left child of the new node and make right child leaf node.
    location->right = new_node(0, true);
    location->right->parent = location;

    // refresh last_node;
    (*tr)->last_node = location;
    
    // Calibrate the heap
    (*tr)->count++;
    up_heap(location);
}

int pop(tree** tr)
{
    if (!(*tr)->count)
    {
        printf("No element in the heap.");
        return -1;
    }

    int rt = (*tr)->root->data;

    if ((*tr)->root != (*tr)->last_node)
    {
        (*tr)->root->data = (*tr)->last_node->data;

        // Refresh last node information
        node* rem_target = (*tr)->last_node;
        (*tr)->last_node = refresh_last_node((*tr)->last_node, false);

        rem_target->left->parent = rem_target->parent;
        if (rem_target->parent && rem_target->parent->left == rem_target)
            rem_target->parent->left = rem_target->left;
        else if (rem_target->parent && rem_target->parent->right == rem_target)
            rem_target->parent->right = rem_target->left;

        // Remove last node and its right assistant node
        free(rem_target->right);
        free(rem_target);

        // DownHeap
        down_heap((*tr)->root);
    }
    else
    {
        node* empty_node = (*tr)->last_node->left;
        empty_node->parent = NULL;

        free((*tr)->last_node->right);
        free((*tr)->last_node);
        (*tr)->last_node = (*tr)->root = empty_node;
    }

    // Decrement count
    (*tr)->count--;

    return rt;
}

void free_all_nodes(node* target)
{
    if (target->left)
        free_all_nodes(target->left);
    if (target->right)
        free_all_nodes(target->right);
    free(target);
}

void free_all(tree* tr)
{
    free_all_nodes(tr->root);
    free(tr);
}

int main(void)
{
    tree* tr = init_tree();

    insert(&tr, 12);
    insert(&tr, 20);
    insert(&tr, 40);

    while (tr->count)
        printf("%d ", pop(&tr));
    putchar('\n');

    insert(&tr, 541);
    printf("%d\n", pop(&tr));
    
    free_all(tr);
    return 0;
}