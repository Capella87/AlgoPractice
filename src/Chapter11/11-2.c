// Implementation of Kruskal Algorithm

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define SUBMISSION
#define KRUSKAL
#define UNDIGRAPH

typedef struct incidence incidence;
typedef struct vertex vertex;
typedef struct edge edge;

#if PRIM
typedef struct h_node h_node;
typedef struct heap heap;
#endif

typedef struct incidence
{
    edge* edge;
    incidence* next;
    vertex* opposite;
} incidence;

// Vertex struct
typedef struct vertex
{
    incidence* outbound_header;
    incidence* inbound_header;
    int name;
    int outbound_count;
    int inbound_count;
#if defined(BFS) || defined(DFS)
    bool is_visited;
#endif

#ifdef PRIM
    int distance;
    h_node* heap_pos;
    edge* parent_edge;
#endif
} vertex;

// Edge struct
typedef struct edge
{
    vertex* source;
    vertex* destination;
    int weight;
    int num;
} edge;

// Create a new incidence to save the opposite vertex and edge information
incidence* incidence_creat(edge* edg, vertex* vert)
{
    incidence* rt = (incidence*)malloc(sizeof(incidence));
    rt->edge = edg;
    rt->next = (void*)0;
    rt->opposite = vert;

    return rt;
}

// Initialize the vertex array
void vertex_init(vertex** vert, int size)
{
    *vert = (vertex*)malloc(sizeof(vertex) * (size + 1));
    for (int i = 0; i <= size; i++)
    {
        (*vert)[i].inbound_count = (*vert)[i].outbound_count = 0;

        // Create headers
        (*vert)[i].inbound_header = incidence_creat((void*)0, (void*)0);
        (*vert)[i].outbound_header = incidence_creat((void*)0, (void*)0);
        (*vert)[i].name = 0;
#if defined(BFS) || defined(DFS)
        (*vert)[i].is_visited = false;
#endif

#ifdef PRIM
        (*vert)[i].distance = INT_MAX;
        (*vert)[i].parent_edge = (void*)0;
        (*vert)[i].heap_pos = (void*)0;
#endif
    }
}

// Initialize the edge array
void edge_init(edge** edg, int size)
{
    (*edg) = (edge*)malloc(sizeof(edge) * (size + 1));

    // Initialize information of each edge element
    for (int i = 0; i <= size; i++)
    {
        (*edg)[i].source = (*edg)[i].destination = (void*)0;
        (*edg)[i].num = -1;
        (*edg)[i].weight = 0;
    }
}

// Find the vertex which contains vert_key
int vertex_find(vertex* vert, int vert_key, int size)
{
    for (int i = 1; i <= size; i++)
        if (vert[i].name == vert_key) return i;

    // Return -1 if it could not be found or the array is full
    return -1;
}

// Find the edge which contains edge_key
int edge_find(edge* edg, int edge_key, int size)
{
    for (int i = 1; i <= size; i++)
        if (edg[i].num == edge_key) return i;

    // Return -1 if it could not be found
    return -1;
}

// Find and return the incidence node from the list
incidence* incidence_find(incidence* header, incidence** prev, int vert_key)
{
    incidence* n = header;
    incidence* pre = n;

    // Move forward while the n is not (void*)0 and the vert_key is not matching
    while (n && n->opposite->name != vert_key)
    {
        pre = n;
        n = n->next;
    }

    // Save the previous node information if prev is not (void*)0
    if (prev != (void*)0)
        *prev = pre;

    // Return the result; If it could not be found, the returned value is (void*)0
    return n;
}

// Dispose the incidence node
void incidence_remove(incidence* target, incidence* prev)
{
    if (!target) return;

    prev->next = target->next;

    free(target);
}

// Find a proper location and locate the new vertex in the array
int vertex_locate(vertex* vert, int vert_key, int vert_size)
{
    // Find a proper location to be inserted a new node
    // The vertex node which has key 0 means 
    // it is a vacant element in the array in this implementation
    int idx = vertex_find(vert, 0, vert_size);
    if (idx == -1) return -1;

    // Place a new vertex in the array
    vert[idx].name = vert_key;

    // Return the index where the new node is placed
    return idx;
}

// Find a proper location and locate the new edge in the array
int edge_locate(edge* edg, int key, int edge_size, int weight)
{
    // Find a proper location;
    // In this implementation, the vacant edge element in the array has -1 as its edge_key (or edge_num)
    int idx = edge_find(edg, -1, edge_size);
    if (idx == -1) return -1;

    // Place a new edge in the array
    edg[idx].num = (key == -2) ? idx : key;
    edg[idx].weight = weight;

    // Return the index where the new node is placed
    return idx;
}

// Find a proper location and locate the new incidence node in the array
void incidence_locate(incidence* header, edge* edge_target, vertex* opposite)
{
    incidence* node = header;

    // Move while node->next is not null and node->next->vert_key is smaller than the argument vert_key
    while (node->next && node->next->opposite->name < opposite->name)
        node = node->next;

    // Create a new incidence node
    incidence* n = incidence_creat(edge_target, opposite);

    // Place the node
    n->next = node->next;
    node->next = n;
}

// Insert incidence nodes in two vertice
void incidence_insert(vertex* vert, edge* edg, int source_idx, int destination_idx,
    int edge_num)
{
    // Insert a new incidence node in the outbound incidence list which is located in the source vertex
    incidence_locate(vert[source_idx].outbound_header, &edg[edge_num], &vert[destination_idx]);
    vert[source_idx].outbound_count++;

#ifdef UNDIGRAPH
    // If this implementation is not for a digraph, inbound incidence list in the source vertex must also contain the same information
    // But they are sharing the same edge
    incidence_locate(vert[source_idx].inbound_header, &edg[edge_num], &vert[destination_idx]);
    vert[source_idx].inbound_count++;
#endif

#ifdef UNDIGRAPH
    // For the self-loop to prevent duplicate information
    if (source_idx == destination_idx) return;
#endif

    // Insert a new incidence node in the inbound incidence list of the destination vertex
    incidence_locate(vert[destination_idx].inbound_header, &edg[edge_num], &vert[source_idx]);
    vert[destination_idx].inbound_count++;

#ifdef UNDIGRAPH
    // If this implementation is not for a digraph, outbound incidence list in the destination vertex must also have the same information
    // But they are sharing the same edge
    incidence_locate(vert[destination_idx].outbound_header, &edg[edge_num], &vert[source_idx]);
    vert[destination_idx].outbound_count++;
#endif
}

// Get rid of incidence nodes to disconnect two vertice and returns the obsolete edge 
edge* incidence_disconnect(vertex* vert, edge* edg, int source_idx, int destination_idx)
{
    // Find the incidence node to be deleted in the source vertex
    // prev node to help the removal work
    incidence* prev;
    incidence* target = incidence_find(vert[source_idx].outbound_header, &prev, vert[destination_idx].name);
    // Returns NULL if the targeted one is not found
    if (!target) return (void*)0;

    // Remove the node
    incidence_remove(target, prev);
    vert[source_idx].outbound_count--;
#ifdef UNDIGRAPH
    // In the undirected graph implemetation, inbound information must also be deleted
    target = incidence_find(vert[source_idx].inbound_header, &prev, vert[destination_idx].name);
    if (!target) return (void*)0;

    incidence_remove(target, prev);
    vert[source_idx].inbound_count--;
#endif

    // Find the incidence node to be deleted in the inbound incidence list of destination vertex
    target = incidence_find(vert[destination_idx].inbound_header, &prev, vert[source_idx].name);
    if (!target) return (void*)0;

    // Get the edge node to be removed
    edge* rt = target->edge;

    // Remove the found node
    incidence_remove(target, prev);
    vert[destination_idx].inbound_count--;

#ifdef UNDIGRAPH
    // Find the incidence node to be deleted in the outbound incidence list of destination vertex
    target = incidence_find(vert[destination_idx].outbound_header, &prev, vert[source_idx].name);
    if (!target) return (void*)0;

    incidence_remove(target, prev);
    vert[destination_idx].outbound_count--;
#endif

    // Return the edge
    return rt;
}

// Make a new connection between two vertice
void connect(vertex* vert, edge* edg, int src, int dest, int vert_arr_size, int edge_arr_size, int weight)
{
    // Exit this function if any argument is invalid
    if (src < 1 || dest < 1) return;

    // Find the location of vertex which contains the value of argument in the array
    int src_idx = vertex_find(vert, src, vert_arr_size);
    int dest_idx = vertex_find(vert, dest, vert_arr_size);

    // Get a proper edge location
    int edg_idx = edge_locate(edg, -2, edge_arr_size, weight);

    // if any vertice were not found or the edge slot is full, Terminate this call
    if (src_idx == -1 || dest_idx == -1 || edg_idx == -1)
        return;

    // Make a new connection
    incidence_insert(vert, edg, src_idx, dest_idx, edg_idx);

    // Make a new edge
    edg[edg_idx].source = vert + src_idx;
    edg[edg_idx].destination = vert + dest_idx;
}

// Remove the connection between two vertice
void disconnect(vertex* vert, edge* edg, int src, int dest, int vert_arr_size, int edge_arr_size)
{
    // Exit this function if any argument is invalid
    if (src < 1 || dest < 1) return;

    // Find each node's location
    int source_idx = vertex_find(vert, src, vert_arr_size);
    int destination_idx = vertex_find(vert, dest, vert_arr_size);

    // Cut the connection and get the obsolete edge
    edge* edge = incidence_disconnect(vert, edg, src, dest);
    if (!edge) return;

    // Remove the useless edge
    edge->source = edge->destination = (void*)0;
    edge->num = -1;
    edge->weight = 0;
}

// Release all resources of graph
void free_all(vertex* vert, edge* edg, int vert_arr_size, int edge_ver_size)
{
    for (int i = 0; i <= vert_arr_size; i++)
    {
        // Remove all incidence nodes
        while (vert[i].inbound_count > 0)
        {
            incidence_remove(vert[i].inbound_header->next, vert[i].inbound_header);
            vert[i].inbound_count--;
        }
        free(vert[i].inbound_header);

        while (vert[i].outbound_count > 0)
        {
            incidence_remove(vert[i].outbound_header->next, vert[i].outbound_header);
            vert[i].outbound_count--;
        }
        free(vert[i].outbound_header);
    }

    free(vert);
    free(edg);
}

#ifdef KRUSKAL

typedef struct heap_node
{
    struct heap_node* left;
    struct heap_node* right;
    struct heap_node* parent;
    bool is_leaf;
    edge* data;
} heap_node;

typedef struct heap_tree
{
    heap_node* root;
    heap_node* last_node;
    int count;
} heap;

heap_node* heap_new_node(edge* data, bool is_leaf)
{
    heap_node* rt = (heap_node*)malloc(sizeof(heap_node));
    rt->left = rt->parent = rt->right = (void*)0;
    rt->data = data;
    rt->is_leaf = is_leaf;

    return rt;
}

heap* heap_init(void)
{
    heap* rt = (heap*)malloc(sizeof(heap));
    rt->root = heap_new_node(0, true);
    rt->count = 0;
    rt->last_node = rt->root;

    return rt;
}

void heap_swap_node(heap_node** first, heap_node** second)
{
    edge* temp = (*first)->data;
    (*first)->data = (*second)->data;
    (*second)->data = temp;
}

void heap_up_heap(heap_node* target)
{
    while (target->parent && target->parent->data->weight > target->data->weight)
    {
        heap_swap_node(&target->parent, &target);
        target = target->parent;
    }
}

void heap_down_heap(heap_node* target)
{
    while (!target->is_leaf)
    {
        heap_node* t = target;
        if (!target->left->is_leaf && target->left->data->weight < t->data->weight)
            t = target->left;
        if (!target->right->is_leaf && target->right->data->weight < t->data->weight)
            t = target->right;
        if (target != t)
        {
            heap_swap_node(&t, &target);
            target = t;
        }
        else break;
    }
}

heap_node* heap_find_location(heap* tree)
{
    heap_node* pos = tree->last_node;

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

heap_node* heap_refresh_last_node(heap_node* n, bool is_switched)
{
    heap_node* pos = n;
    if (pos->parent && !is_switched)
    {
        if (pos->parent->right == pos)
            pos = heap_refresh_last_node(pos->parent->left, true);
        else if (pos->parent->left == pos)
            pos = heap_refresh_last_node(pos->parent, is_switched);
    }
    else
    {
        if (pos->right && !pos->right->is_leaf)
            pos = heap_refresh_last_node(pos->right, true); // Changed
    }

    return pos;
}

void heap_insert(heap** tr, edge* data)
{
    // Find location;
    heap_node* location = heap_find_location(*tr);
    // replace leaf node in the location to new node;
    heap_node* new_elem = heap_new_node(data, false);
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
    location->right = heap_new_node((void*)0, true);
    location->right->parent = location;

    // refresh last_node;
    (*tr)->last_node = location;

    // Calibrate the heap
    (*tr)->count++;
    heap_up_heap(location);
}

edge* heap_pop(heap** tr)
{
    if (!(*tr)->count) return (void*)0;

    edge* rt = (*tr)->root->data;

    if ((*tr)->root != (*tr)->last_node)
    {
        (*tr)->root->data = (*tr)->last_node->data;

        // Refresh last node information
        heap_node* rem_target = (*tr)->last_node;
        (*tr)->last_node = heap_refresh_last_node((*tr)->last_node, false);

        rem_target->left->parent = rem_target->parent;
        if (rem_target->parent && rem_target->parent->left == rem_target)
            rem_target->parent->left = rem_target->left;
        else if (rem_target->parent && rem_target->parent->right == rem_target)
            rem_target->parent->right = rem_target->left;

        // Remove last node and its right assistant node
        free(rem_target->right);
        free(rem_target);

        // DownHeap
        heap_down_heap((*tr)->root);
    }
    else
    {
        heap_node* empty_node = (*tr)->last_node->left;
        empty_node->parent = (void*)0;

        free((*tr)->last_node->right);
        free((*tr)->last_node);
        (*tr)->last_node = (*tr)->root = empty_node;
    }

    // Decrement count
    (*tr)->count--;

    return rt;
}

void heap_free_all_nodes(heap_node* target)
{
    if (target->left)
        heap_free_all_nodes(target->left);
    if (target->right)
        heap_free_all_nodes(target->right);
    free(target);
}

void heap_free_all(heap* tr)
{
    heap_free_all_nodes(tr->root);
    free(tr);
}

// Disjoint Set

typedef struct disjoint_set_element
{
    vertex* data;
    int root_idx;
} ds_elem;

typedef struct disjoint_set
{
    ds_elem* set;
    int set_count;
    int size;
} disjoint_set;

void disjoint_set_init(disjoint_set** set, vertex* vert, int size)
{
    *set = (disjoint_set*)malloc(sizeof(disjoint_set));
    (*set)->set_count = (*set)->size = size;

    (*set)->set = (ds_elem*)malloc(sizeof(ds_elem) * (size + 1));

    for (int i = 0; i <= size; i++)
    {
        (*set)->set[i].data = &vert[i];
        (*set)->set[i].root_idx = -1;
    }
}

int disjoint_set_find(disjoint_set* set, int vert_idx)
{
    if (vert_idx < 1 || vert_idx > set->size)
        return 0;
    else if (set->set[vert_idx].root_idx < 0) return vert_idx;

    return set->set[vert_idx].root_idx = disjoint_set_find(set, set->set[vert_idx].root_idx);
}

// Merge b to a
void disjoint_set_union(disjoint_set* set, int a, int b)
{
    if (!set || !a || !b || !set->set) return;

    int a_root_idx = disjoint_set_find(set, a);
    int b_root_idx = disjoint_set_find(set, b);

    // If set a is bigger than b (deeper depth) : merge to a
    if (set->set[a_root_idx].root_idx < set->set[b_root_idx].root_idx)
    {
        set->set[b_root_idx].root_idx = a_root_idx;
    }
    // Set b is bigger than or equal to a : merge to b
    else
    {
        if (set->set[b_root_idx].root_idx == set->set[a_root_idx].root_idx)
            set->set[b_root_idx].root_idx--;
        set->set[a_root_idx].root_idx = b_root_idx;
    }
}

// Kruskal Algorithm

int kruskal(vertex* vert, edge* edg, int vert_arr_size, int edge_arr_size)
{
    heap* edge_heap = heap_init();
    for (int i = 1; i <= edge_arr_size; i++)
        heap_insert(&edge_heap, edg + i);

    disjoint_set* vert_ds;
    disjoint_set_init(&vert_ds, vert, vert_arr_size);

    ds_elem* mst = (void*)0;
    int total_weight = 0;

    while (edge_heap->count)
    {
        edge* e = heap_pop(&edge_heap);

        int src = e->source->name < e->destination->name ? e->source->name : e->destination->name;
        int dest = (src == e->source->name) ? e->destination->name : e->source->name;

        if (disjoint_set_find(vert_ds, src) != disjoint_set_find(vert_ds, dest))
        {
            printf(" %d", e->weight);
            disjoint_set_union(vert_ds, src, dest);
            total_weight += e->weight;
        }
    }
    putchar('\n');

    heap_free_all(edge_heap);
    free(vert_ds->set);
    free(vert_ds);

    return total_weight;
}
#endif

int main(void)
{
    int n, m, src, dest, weight;
    vertex* vert;
    edge* edg;

    scanf("%d %d", &n, &m);
    vertex_init(&vert, n);
    for (int i = 1; i <= n; i++)
        vertex_locate(vert, i, n);
    edge_init(&edg, m);

    for (int i = 0; i < m; i++)
    {
        scanf("%d %d %d", &src, &dest, &weight);
        connect(vert, edg, src, dest, n, m, weight);
    }
    printf("%d\n", kruskal(vert, edg, n, m));

    free_all(vert, edg, n, m);
    return 0;
}

/*
6 9
1 2 3
1 3 20
2 4 25
2 5 17
3 4 34
3 5 1
3 6 12
4 5 5
5 6 37
*/