#include <stdio.h>
#include <stdlib.h>

#define MAX_NOS 19
#define MAX_NEIGHBORS 5 

typedef struct NetworkGraph {
    unsigned char mat_adj[MAX_NOS-1][MAX_NOS-1];
} ng;
