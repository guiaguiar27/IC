#include <stdio.h>
#include <stdlib.h>

#define MAX_NOS 5 

#define temp_canais 16 
#define Timeslot 16

typedef struct NetworkGraph {
    char mat_adj[MAX_NOS][MAX_NOS];
} ng;
