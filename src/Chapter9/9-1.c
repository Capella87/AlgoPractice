#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SUBMISSION
#define UNDIGRAPH

typedef struct incidence incidence;
typedef struct vertex vertex;
typedef struct edge edge;

typedef struct incidence
{
    edge* edge;
    incidence* next;
    int vert_key;
} incidence;

// Vertex struct
typedef struct vertex
{
    incidence* outbound_header;
    incidence* inbound_header;
    int name;
    int outbound_count;
    int inbound_count;
#ifdef FIRSTSEARCH
    bool is_visited;
#endif
#ifdef TOPOSORT
    int degree;
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
incidence* incidence_creat(edge* edg, int vert_key)
{
    incidence* rt = (incidence*)malloc(sizeof(incidence));
    rt->edge = edg;
    rt->next = (void*)0;
    rt->vert_key = vert_key;

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
        (*vert)[i].inbound_header = incidence_creat((void*)0, 0);
        (*vert)[i].outbound_header = incidence_creat((void*)0, 0);
        (*vert)[i].name = 0;
#ifdef FIRSTSEARCH
        (*vert)[i].is_visited = false;
#endif
#ifdef TOPOSORT
        (*vert)[i].degree = 0;
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

    // Move forward while the n is not null and the vert_key is not matching
    while (n && n->vert_key != vert_key)
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

// // Find a proper location and locate the new incidence node in the array
void incidence_locate(incidence* header, edge* edge_target, int vert_key)
{
    incidence* node = header;

    // Move while node->next is not null and node->next->vert_key is smaller than the argument vert_key
    while (node->next && node->next->vert_key < vert_key)
        node = node->next;

    // Create a new incidence node
    incidence* n = incidence_creat(edge_target, vert_key);

    // Place the node
    n->next = node->next;
    node->next = n;
}

// Insert incidence nodes in two vertice
void incidence_insert(vertex* vert, edge* edg, int source_idx, int destination_idx,
    int edge_num)
{
    // Insert a new incidence node in the outbound incidence list which is located in the source vertex
    incidence_locate(vert[source_idx].outbound_header, &edg[edge_num], vert[destination_idx].name);
    vert[source_idx].outbound_count++;

#ifdef UNDIGRAPH
    // If this implementation is not for a digraph, inbound incidence list in the source vertex must also contain the same information
    // But they are sharing the same edge
    incidence_locate(vert[source_idx].inbound_header, &edg[edge_num], vert[destination_idx].name);
    vert[source_idx].inbound_count++;
#endif

#ifdef UNDIGRAPH
    // For the self-loop to prevent duplicate information
    if (source_idx == destination_idx) return;
#endif

    // Insert a new incidence node in the inbound incidence list of the destination vertex
    incidence_locate(vert[destination_idx].inbound_header, &edg[edge_num], vert[source_idx].name);
    vert[destination_idx].inbound_count++;

#ifdef UNDIGRAPH
    // If this implementation is not for a digraph, outbound incidence list in the destination vertex must also have the same information
    // But they are sharing the same edge
    incidence_locate(vert[destination_idx].outbound_header, &edg[edge_num], vert[source_idx].name);
    vert[destination_idx].outbound_count++;
#endif

#ifdef TOPOSORT
    // Sync the number of degree of the destincation vertex
    vert[destination_idx].degree++;
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
#ifdef TOPOSORT
    // Sync between the degree and the inbound count
    vert[destination_idx].degree--;
#endif
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

#ifdef SUBMISSION

// Build the graph according to the assignment
void build_graph(vertex** vert, edge** edg, int vert_arr_size, int edge_arr_size)
{
    vertex_init(vert, vert_arr_size);
    edge_init(edg, edge_arr_size);

    for (int i = 1; i <= vert_arr_size; i++)
        vertex_locate(*vert, i, vert_arr_size);

    connect(*vert, *edg, 1, 2, vert_arr_size, edge_arr_size, 1);
    connect(*vert, *edg, 1, 3, vert_arr_size, edge_arr_size, 1);
    connect(*vert, *edg, 1, 4, vert_arr_size, edge_arr_size, 1);
    connect(*vert, *edg, 2, 3, vert_arr_size, edge_arr_size, 1);
    connect(*vert, *edg, 3, 5, vert_arr_size, edge_arr_size, 4);
    connect(*vert, *edg, 5, 5, vert_arr_size, edge_arr_size, 4);
    connect(*vert, *edg, 5, 6, vert_arr_size, edge_arr_size, 3);
    connect(*vert, *edg, 1, 6, vert_arr_size, edge_arr_size, 2);
}

// Show the connection and weight information of vertex
void a(vertex* vert, edge* edg, int vert_key, int vert_arr_size, int edge_arr_size)
{
    // Find the vertex in the array
    int target_idx = vertex_find(vert, vert_key, vert_arr_size);
    if (target_idx == -1)
    {
        puts("-1");
        return;
    }

    // Print information
    incidence* pos = vert[target_idx].outbound_header->next;
    while (pos)
    {
        printf(" %d %d", pos->vert_key, pos->edge->weight);
        pos = pos->next;
    }
    putchar('\n');
}

// Modify, add or remove the connection
void m(vertex* vert, edge* edg, int src, int dest, int new_weight, int vert_arr_size, int edge_arr_size)
{
    // Validate the existence of vertex
    if (src == -1 || dest == -1)
    {
        puts("-1");
        return;
    }

    int src_idx = vertex_find(vert, src, vert_arr_size);
    int dest_idx = vertex_find(vert, dest, vert_arr_size);

    if (src_idx == -1 || dest_idx == -1)
    {
        puts("-1");
        return;
    }

    // Removal
    if (!new_weight)
    {
        disconnect(vert, edg, src, dest, vert_arr_size, edge_arr_size);
        return;
    }

    // Modify weight of edge or make a new edge connection
    incidence* result = incidence_find(vert[src_idx].outbound_header, (void*)0, vert[dest_idx].name);
    if (!result)
    {
        connect(vert, edg, src, dest, vert_arr_size, edge_arr_size, new_weight);
        return;
    }
    edge* edge_target = result->edge;
    edge_target->weight = new_weight;
}

#endif

int main(void)
{
    // n for vertice, em for edges
    int n = 6, em = 21;
    char command;
    int src, dest, weight;
    vertex* vert = (void*)0;
    edge* edg = (void*)0;

    build_graph(&vert, &edg, n, em);

    while (1)
    {
        command = getchar();
        getchar();
        switch (command)
        {
        case 'a':
            scanf("%d", &src);
            getchar();
            
            a(vert, edg, src, n, em);
            break;

        case 'm':
            scanf("%d %d %d", &src, &dest, &weight);
            getchar();

            m(vert, edg, src, dest, weight, n, em);
            break;

        case 'q': // Exit the loop
            goto exit;
        }
    }
exit:
    free_all(vert, edg, n, em);
    return 0;
}