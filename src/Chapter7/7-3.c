#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Node definition
typedef struct node
{
    struct node* parent;
    struct node* lChild;
    struct node* rChild;
    int height;
    int key;
} node;

// Create a node and return it
node* creat_node(int key)
{
    node* rt = (node*)malloc(sizeof(node));
    rt->parent = rt->lChild = rt->rChild = (void*)0;
    rt->key = key;
    rt->height = 0;

    return rt;
}

// Create two external nodes
void expandExternal(node* n)
{
    if (!n) return;

    n->lChild = creat_node(-1);
    n->rChild = creat_node(-1);
    n->lChild->parent = n->rChild->parent = n;
}

// Get the height of node
int get_height(node* target)
{
    return !target ? 0 : target->height;
}

// Get a balance index by subtraction
int get_diff(node* n)
{
    if (!n) return 0;

    return get_height(n->lChild) - get_height(n->rChild);
}

// Return whether the node is balanced or not
bool isBalanced(node* n)
{
    if (!n) return false;

    int diff = get_diff(n);

    return diff < -1 || diff > 1 ? false : true;
}

// Return true if the node is an external node
bool isExternal(node* n)
{
    if (!n) return false;

    return !n->lChild && !n->rChild ? true : false;
}

// Return true if the node is an internal node
bool isInternal(node* n)
{
    if (!n) return false;

    return n->lChild || n->rChild ? true : false;
}

// Update the height of node
bool updateHeight(node* n)
{
    if (!n) return false;

    int orig = n->height;
    n->height = 1 + MAX(get_height(n->lChild), get_height(n->rChild));
    return orig == n->height ? false : true;
}

// Make child node a root node by right rotation
node* right_rotation(node* target, node* child)
{
    // To update child information for the parent node of target
    int direction = -1;
    // The direction variable will 1 if the right node of parent is the target
    if (target->parent)
        direction = target->parent->rChild == target ? 1 : 0;

    // Rotation
    node* child_right = child->rChild;
    child->rChild = target;
    child->parent = target->parent;
    target->parent = child;
        
    target->lChild = child_right;
    child_right->parent = target;

    // Update height information
    updateHeight(target);
    updateHeight(child);

    // Update the child information for the parent
    switch (direction)
    {
    case 1:
        child->parent->rChild = child;
        break;
    case 0:
        child->parent->lChild = child;
        break;
    }

    return child;
}

// Make child node a root node by right rotation
node* left_rotation(node* target, node* child)
{
    // To update child information for the parent node of target
    int direction = -1;
    // The direction variable will 1 if the right node of parent is the target
    if (target->parent)
        direction = target->parent->rChild == target ? 1 : 0;

    // Rotation
    node* child_left = child->lChild;
    child->lChild = target;
    child->parent = target->parent;
    target->parent = child;

    target->rChild = child_left;
    child_left->parent = target;

    // Update height information
    updateHeight(target);
    updateHeight(child);

    // Update the child information for the parent
    switch (direction)
    {
    case 1:
        child->parent->rChild = child;
        break;
    case 0:
        child->parent->lChild = child;
        break;
    }

    return child;
}

// Rebalance the tree
node* restructure(node* target, node* child, node* grand_child)
{
    //    (target)
    //      /
    //   (child)
    if (child->key < target->key)
    {
        //    (target)
        //      /
        //   (child)
        //    /
        // (grand_child)
        if (grand_child->key < child->key)
            return right_rotation(target, child);
        //    (target)
        //      /
        //   (child)
        //       \
        //    (grand_child)
        else
        {
            target->lChild = left_rotation(child, grand_child);
            return right_rotation(target, target->lChild);
        }
    }

    // (target)
    //   \
    //   (child)
    else
    {
        // (target)
        //   \
        //   (child)
        //      \
        //     (grand_child)
        if (child->key < grand_child->key)
            return left_rotation(target, child);

        // (target)
        //   \
        //   (child)
        //     /
        // (grand_child)
        else
        {
            target->rChild = right_rotation(child, grand_child);
            return left_rotation(target, target->rChild);
        }
    }
}

// Find a suitable position for the key
node* treeSearch(node* n, int key)
{
    // Returns the external node when it finds the place to put (Precisely it reached to the end of the tree)
    if (isExternal(n)) return n;

    if (key < n->key) return treeSearch(n->lChild, key); // Recurse; Move to a left child
    else if (key == n->key) return n; // Returns the duplicate node when key is already put
    else return treeSearch(n->rChild, key); // Recurse; Move to a right child
}

// Search the imbalance and fix them from the bottom
void searchAndFixAfterInsertion(node** root, node* target)
{
    node* pos = target;

    // Start an inspection from the new node;
    while (pos)
    {
        updateHeight(pos);

        if (pos->parent == (void*)0) *root = pos; // When pos reached the root node, update the pointer variable of root
        if (!isBalanced(pos)) break; // Exit the loop when an imbalance is detected. it only works the depth of the tree is bigger or equal than 3
        pos = pos->parent; // Move to the parent node
    }
    if (!pos) return; // Exit the function; it means there's no imbalance

    // Preparing to fix the found imbalance
    int diff = get_diff(pos);
    node* child = diff < -1 ? pos->rChild : pos->lChild;
    int second_diff = get_diff(child);
    
    // Prioritize less rotation
    node* grand_child;
    if (child == pos->lChild)
        grand_child = second_diff >= 0 ? child->lChild : child->rChild;
    else
        grand_child = second_diff <= 0 ? child->rChild : child->lChild;

    // Fix
    pos = restructure(pos, child, grand_child);
    // Update height information of remainder nodes from the pos to the root
    while (pos)
    {
        updateHeight(pos);
        if (pos->parent == (void*)0) *root = pos;
        pos = pos->parent;
    }
}

// Search the imbalance and fix them from the bottom after removal; Similar to searchAndFixAfterRemoval
void searchAndFixAfterRemoval(node** root, node* target)
{
    node* pos = target;

    // Start an inspection from the new node;
    while (pos)
    {
        updateHeight(pos);

        if (pos->parent == (void*)0) *root = pos; // When pos reached the root node, update the pointer variable of root
        if (!isBalanced(pos)) // Exit the loop when an imbalance is detected. it only works the depth of the tree is bigger or equal than 3
        {
            // Preparing to fix the found imbalance
            int diff = get_diff(pos);
            node* child = diff < -1 ? pos->rChild : pos->lChild;
            int second_diff = get_diff(child);

            // Prioritize less rotation
            node* grand_child;
            if (child == pos->lChild)
                grand_child = second_diff >= 0 ? child->lChild : child->rChild;
            else
                grand_child = second_diff <= 0 ? child->rChild : child->lChild;

            // Fix
            pos = restructure(pos, child, grand_child);
            if (!pos->parent) *root = pos; // Update *root information if updated pos is root
        }
        pos = pos->parent; // Move to the parent node
    }
    // Exit the function; it means there's no imbalance
}

// Insert a new key
void insertItem(node** root, int key)
{
    node* target = treeSearch(*root, key);

    // Exit the function if the key already exists
    if (target->key == key) return;

    // Transform the external node to internal node
    target->key = key;
    target->height = 1;
    expandExternal(target);

    // 'Maintenance'
    searchAndFixAfterInsertion(root, target);
    return;
}

// Find a key in the tree
int findElement(node* n, int key)
{
    // Not found
    if (isExternal(n))
    {
        puts("X");
        return -1;
    }

    // Use recursion
    if (key < n->key)
        return findElement(n->lChild, key);
    // Print the key if it exists
    else if (key == n->key)
    {
        printf("%d\n", key);
        return key;
    }
    else return findElement(n->rChild, key);
}

// Get the sibling node of a node
node* sibling(node* n)
{
    if (!n->parent)
        return (void*)0;

    return n->parent->lChild == n ? n->parent->rChild : n->parent->lChild;
}

// Remove an external node which is no longer needed
void reduceExternal(node** parent, node* external, node** root)
{
    node* ano = sibling(external); // Use sibling node to replace the position

    if (!(*parent)->parent) // In case of root node
    {
        *root = ano;
        (*root)->parent = (void*)0;
    }
    else
    {
        node** pparent = &((*parent)->parent); // To preserve modifications
        ano->parent = *pparent; // Set sibling's new parent because the old parent will be removed
        if (*parent == (*pparent)->lChild) // Update *pparent child information
            (*pparent)->lChild = ano;
        else
            (*pparent)->rChild = ano;
    }

    // Remove an external node and the targeted node to be deleted
    free(*parent);
    free(external);
}

// Find the next successor of the deleted element
node* inOrderSucc(node* n)
{
    while (isInternal(n->lChild))
        n = n->lChild;
    return n;
}

// Remove an element from the key if it is found
int removeElement(node** root, int key)
{
    node* n = treeSearch(*root, key);

    if (isExternal(n))
    {
        puts("X");
        return -1;
    }

    int e = n->key;

    node* begin = (void*)0; // A pointer variable for maintenance
    node* t = n->lChild;
    if (!isExternal(t))
        t = n->rChild;
    if (isExternal(t)) // In case of no children or only one child
    {
        begin = n->parent;
        reduceExternal(&n, t, root);
    }
    else // In case of two children
    {
        node* successor = inOrderSucc(n->rChild); // Find a successor for the vacant node
        t = successor->lChild;

        node** nn = &n; // To preserve modifications
        (*nn)->key = successor->key;
        begin = successor->parent;
        reduceExternal(&successor, t, root); // Remove obsolete original successor node
    }
    printf("%d\n", e);

    searchAndFixAfterRemoval(root, begin); // Fix imbalance in the tree
    return e;
}

// Free all elements in the tree before the termination
void free_all(node* n)
{
    if (!n) return;

    if (n->lChild) free_all(n->lChild);
    if (n->rChild) free_all(n->rChild);
    free(n);
}

// Traverse a tree and show its elements
void traversal(node* n, void (*way)(node*))
{
    way(n);
    putchar('\n');
}

// Preorder traversal
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

        // Quit
        if (command == 'q') break;
        getchar();
        switch (command)
        {
        // Insertion
        case 'i':
            scanf("%d", &key);
            getchar();

            insertItem(&root, key);
            break;

        // Remove a key from the tree
        case 'd':
            scanf("%d", &key);
            getchar();

            removeElement(&root, key);
            break;

        // Search a key from the tree
        case 's':
            scanf("%d", &key);
            getchar();

            findElement(root, key);
            break;

        // Print all elements in the tree
        case 'p':
            traversal(root, preorder);
            break;
        }
    }

    free_all(root);
    return 0;
}