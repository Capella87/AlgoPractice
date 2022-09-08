#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node
{
    struct node* left;
    struct node* right;
    int data;
} node;

node* traverse(node* parent, const int target)
{
    if (!parent) return parent;
    node* rt = parent;

    if (parent->left)
    {
        rt = traverse(parent->left, target);
        if (rt->data == target) return rt;
    }
    if (parent->data == target) return rt = parent;
    if (parent->right)
    {
        rt = traverse(parent->right, target);
        if (rt->data == target) return rt;
    }

    return rt;
}

node* create_node(const int value)
{
    node* rt = (node*)malloc(sizeof(node));
    rt->data = value;
    rt->left = rt->right = NULL;

    return rt;
}

void insert(node** root, const int parent, const int left, const int right)
{
    if (*root == NULL)
        *root = create_node(parent);
    if (left != 0)
        (*root)->left = create_node(left);
    if (right != 0)
        (*root)->right = create_node(right);
}

void free_all(node* root)
{
    if (root->left != NULL)
        free_all(root->left);
    if (root->right != NULL)
        free_all(root->right);
    free(root);
}

int main(void)
{
    int n, e;
    int left, right, parent;
    char input[101] = "";
    node* root = NULL;

    scanf("%d", &n);
    scanf("%d %d %d", &parent, &left, &right);
    insert(&root, parent, left, right);
    --n;
    while (n > 0)
    {
        scanf("%d %d %d", &parent, &left, &right);
        node* location = traverse(root, parent);
        insert(&location, parent, left, right);
        n--;
    }

    scanf("%d", &e);
    getchar();
    while (e > 0)
    {
        scanf("%[^\n]", input);
        getchar();

        node* pos = root;
        size_t len = strlen(input);
        for (int i = 0; i < len; i++)
        {
            printf(" %d", pos->data);
            if (input[i] == 'L')
                pos = pos->left;
            else if (input[i] == 'R')
                pos = pos->right;
        }
        printf(" %d\n", pos->data);
        e--;
    }

    free_all(root);
    return 0;
}