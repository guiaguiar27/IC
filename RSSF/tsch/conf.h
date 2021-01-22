#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "network-graph.h" 
#include "lib/random.h" 

void DCFL(uint16_t num_aresta, uint16_t num_no, uint16_t (*pacotes)[num_no], uint16_t (*graf_conf)[num_aresta][num_aresta], uint16_t (*mapa_graf_conf)[num_aresta][2], uint16_t raiz, ng *matching, uint16_t (*vetor)[num_aresta][2]); 
void geraMaching(uint16_t tam, uint16_t tam_rede, uint16_t (*pacotes)[tam_rede], uint16_t (*graf_conf)[tam][tam], uint16_t (*mapa_graf_conf)[tam][2], uint16_t node, ng *resultado, uint16_t (*vetor)[tam][2]);
void selecao(uint16_t **conf, uint16_t pai, uint16_t tam);
void mapGraphConf(ng *mat, uint16_t tam_no, uint16_t tam_aresta, uint16_t (*alocado)[tam_aresta][2]);
void fazMatrizConf(uint16_t tam_arest, uint16_t (*mapConf)[tam_arest][2], uint16_t (*grafoconf)[tam_arest][tam_arest]);    //Tam é nº de arestas do grafo da rede

