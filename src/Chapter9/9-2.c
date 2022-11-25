#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SUBMISSION

typedef struct edge
{
    int weight;
} edge;

typedef struct graph
{
    edge** matrix;
    int vertex_count;
    int edge_count;
    int max_edge_count;
} graph;

// Make a matrix for the graph
graph* graph_init(int vert_arr_size, int edge_arr_size)
{
    graph* rt = (graph*)malloc(sizeof(graph));

    // Matrix initialization
    rt->matrix = (edge**)malloc(sizeof(edge*) * (vert_arr_size + 1));
    rt->vertex_count = vert_arr_size;
    rt->max_edge_count = edge_arr_size;
    rt->edge_count = 0;
    for (int i = 0; i <= vert_arr_size; i++)
    {
        rt->matrix[i] = (edge*)malloc(sizeof(edge) * (vert_arr_size + 1));
        for (int j = 0; j <= vert_arr_size; j++)
            rt->matrix[i][j].weight = 0;
    }

    return rt;
}

// Make a new connection
void connect(graph* gh, int src, int dest, int weight)
{
    if (gh->max_edge_count == gh->edge_count) return;

    gh->matrix[src][dest].weight = weight;
    gh->matrix[dest][src].weight = weight;
    gh->edge_count++;
}

// Close the connection between src and dest vertex
void disconnect(graph* gh, int src, int dest)
{
    if (!gh->edge_count) return;

    gh->matrix[src][dest].weight = 0;
    gh->matrix[dest][src].weight = 0;
    gh->edge_count--;
}

// Return true if the edge is exist
bool edge_is_exist(graph* gh, int src, int dest)
{
    return gh->matrix[src][dest].weight != 0 && gh->matrix[dest][src].weight != 0 ? true : false;
}

// Update weight
void update(graph* gh, int src, int dest, int weight)
{
    gh->matrix[src][dest].weight = weight;
    gh->matrix[dest][src].weight = weight;
}

#ifdef SUBMISSION
// Show graph information
void a(graph* gh, int src, int vert_arr_size)
{
    // Non-existence vertex
    if (src < 1 || src > vert_arr_size)
    {
        puts("-1");
        return;
    }
    
    // Print information
    for (int i = 1; i <= vert_arr_size; i++)
    {
        if (gh->matrix[src][i].weight != 0)
            printf(" %d %d", i, gh->matrix[src][i].weight);
    }
    putchar('\n');
}

// Update, Add or delete edge information
void m(graph* gh, int src, int dest, int new_weight, int vert_arr_size, int edge_arr_size)
{
    if (src < 1 || dest < 1 || src > vert_arr_size || dest > vert_arr_size)
    {
        puts("-1");
        return;
    }

    // new_weight is zero and edge is exist; disconnect
    if (edge_is_exist(gh, src, dest) && !new_weight)
    {
        disconnect(gh, src, dest);
        return;
    }

    // new_weight is not zero and the edge is not exist; connect
    if (!edge_is_exist(gh, src, dest) && new_weight)
    {
        connect(gh, src, dest, new_weight);
        return;
    }
    // Update weight information
    else if (edge_is_exist(gh, src, dest))
    {
        update(gh, src, dest, new_weight);
    }
}

// Build graph according to the assignment
void graph_build(graph** gh, int vert_arr_size, int edge_arr_size)
{
    *gh = graph_init(vert_arr_size, edge_arr_size);

    connect(*gh, 1, 2, 1);
    connect(*gh, 1, 3, 1);
    connect(*gh, 1, 4, 1);
    connect(*gh, 1, 6, 2);
    connect(*gh, 2, 3, 1);
    connect(*gh, 3, 5, 4);
    connect(*gh, 5, 5, 4);
    connect(*gh, 5, 6, 3);
}
#endif

// Dispose all resources
void free_all(graph* gh, int vert_arr_size)
{
    for (int i = 0; i <= vert_arr_size; i++)
        free(gh->matrix[i]);
    free(gh->matrix);
    free(gh);
}

int main(void)
{
    // n for vertice, em for edges
    int n = 6, em = 21;

    char command;
    int src, dest, weight;
    graph* gh;
    graph_build(&gh, n, em);

    while (1)
    {
        command = getchar();
        getchar();
        switch (command)
        {
        case 'a':
            scanf("%d", &src);
            getchar();

            a(gh, src, n);
            break;

        case 'm':
            scanf("%d %d %d", &src, &dest, &weight);
            getchar();

            m(gh, src, dest, weight, n, em);
            break;

        case 'q': // Exit the loop
            goto exit;
        }
    }
exit:
    free_all(gh, n);
    return 0;
}