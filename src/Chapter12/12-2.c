// Implementation of Bellman-Ford Algorithm

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define SUBMISSION
#define BELLMAN_FORD
#define DIGRAPH

typedef struct incidence incidence;
typedef struct vertex vertex;
typedef struct edge edge;

#define MIN(a, b) (((a) > (b)) ? (b) : (a))

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

    // Move forward while the n is not null and the vert_key is not matching
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

#ifdef BELLMAN_FORD

// 블럭 단위로 쪼개서 주석을 단다.

// Initialize distance array
int* bf_distance_init(int vert_arr_size)
{
    int* rt = (int*)malloc(sizeof(int) * (vert_arr_size + 1));
    
    for (int i = 0; i <= vert_arr_size; i++)
        rt[i] = INT_MAX;

    return rt;
}

// Print distance information
void print_distance(int* distance_arr, int size, int begin_vert_key)
{
    for (int i = 1; i <= size; i++)
    {
        if (i == begin_vert_key || distance_arr[i] >= INT_MAX)
            continue;
        printf("%d %d\n", i, distance_arr[i]);
    }
}

// Implementation of Bellman-Ford Algorithm
void bellman_ford(vertex* vert_arr, edge* edge_arr, int vert_arr_size, int edge_arr_size, vertex* begin)
{
    int* distance_arr = bf_distance_init(vert_arr_size);

    distance_arr[begin->name] = 0;
    
    // Warning : This code does not guarantee working properly when there are any unused edges in the edge_arr.
    // Later, It should be marked as unused using extra variable
    // TODO : Future Release; unused edge and vertex handling; remove edge or vertex feature.
    for (int i = 1; i <= vert_arr_size; i++)
    {
        for (int j = 1; j <= edge_arr_size; j++)
        {
            // Prevent overflow problem
            if (distance_arr[edge_arr[j].source->name] == INT_MAX)
                continue;
            else if (distance_arr[edge_arr[j].destination->name] == INT_MAX)
                distance_arr[edge_arr[j].destination->name] = edge_arr[j].weight + distance_arr[edge_arr[j].source->name];
            else distance_arr[edge_arr[j].destination->name] = MIN(distance_arr[edge_arr[j].destination->name],
                edge_arr[j].weight + distance_arr[edge_arr[j].source->name]);
        }
    }

    print_distance(distance_arr, vert_arr_size, begin->name);
    free(distance_arr);
}
#endif


int main(void)
{
    int n, m, src, dest, weight, s;
    vertex* vert;
    edge* edg;

    scanf("%d %d %d", &n, &m, &s);
    vertex_init(&vert, n);
    for (int i = 1; i <= n; i++)
        vertex_locate(vert, i, n);
    edge_init(&edg, m);
    for (int i = 0; i < m; i++)
    {
        scanf("%d %d %d", &src, &dest, &weight);
        connect(vert, edg, src, dest, n, m, weight);
    }
    bellman_ford(vert, edg, n, m, &vert[s]);

    free_all(vert, edg, n, m);
    return 0;
}

/*
5 7 1
1 2 1
1 4 5
5 1 -2
3 5 3
3 4 1
1 3 2
3 2 -3

3 2 3
1 2 -123
2 3 -1222
*/