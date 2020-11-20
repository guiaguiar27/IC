#include <stdio.h>
#include <stdlib.h>




int **geraMaching(int *pacotes, int **mat_adj, int **graf_conf, int **mapa_graf_conf, int tam, int tam_rede, int node);
void selecao(int **conf, int pai, int tam);
int **mapGraphConf(int **mat, int tam_no, int tam_aresta);
int **fazMatrizConf(int **mapConf, int **graf_rede, int tam_arest);    //Tam é nº de arestas do grafo da rede

