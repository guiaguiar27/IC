#include <stdio.h>
#include <stdlib.h>

#define MAX_NOS 6
#define MAX_NEIGHBORS 3  
#define SLOTFRAME_SIZE 6//   deve ser o mesmo tamanho do nó generico e de slots de tempo


typedef struct NetworkGraph {
    unsigned char mat_adj[MAX_NOS][MAX_NOS];
} ng;
