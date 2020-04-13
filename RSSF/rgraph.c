#include "rgraph.h"

    /*
    * vetor: Vetor contendo o nome de cada no para identificacao
    * arquivo: nome do arquivo que contem o grafo em DOT language
    * numNos: variavel q armazenara o nº de nos do grafo
    * numArest: variavel que armazenara o nº de arestas do grafo
    */

int **leDOT(char *arquivo, int *numNos, int *numArest, char ***vetor){
    Agraph_t *g;        //Grafo a ser lido
    Agnode_t *n1, *n2;  //Aux
    Agedge_t *e;        //Aux
    FILE *f;            //Aux
    int z, i;           //Aux
    int **matriz;       //Potencial matriz de adjacencia (caso de td certo, e claro)
    
    f = fopen(arquivo, "r");
    
    //Le o arquivo DOT e armazena o grafo em 'g'
    g = agread(f, NULL);

    //Pega a qtd de nos do grafo e armazena em "numNos"
    *numNos = agnnodes(g);

    //Pega a qtd de arestas do grafo e armazena em "numArest"
    *numArest = agnedges(g);

    //Aloca a matriz e o "vetor" dinamicamente
    *vetor = (char**) malloc(*numNos * sizeof(char*));
    matriz = (int**) malloc(*numNos * sizeof(int*));
    for(z = 0; z < *numNos; z++){
        (*vetor)[z] = (char*) malloc(10 * sizeof(char));
        matriz[z] = (int*) malloc(*numNos * sizeof(int));
    }

    //Preenche o Vetor
    z = 0;
    n1 = agfstnode(g);
    while(n1 != NULL){
        strcpy((*vetor)[z], agnameof(n1));
        z++;
        n1 = agnxtnode(g, n1);
    }

    for(z = 0; z < *numNos; z++)
        printf("%s[%d]\n", (*vetor)[z], z + 1);

    //Preenche a matriz de adjacencia com 0's
    for(z = 0; z < *numNos; z++)
        for(i = 0; i < *numNos; i++)
            matriz[z][i] = 0;

    //Varre o Grafo afim de preencher a Matriz
    n1 = agfstnode(g);                  //Pega o primeiro no do grafo
    while(n1 != NULL){   
        e = agfstout(g,n1);             //Pega a primeira aresta de saida do nó 'n'
        z = busca(*vetor, agnameof(n1), *numNos);
        while(e != NULL){               //Pega tds as arestas de saida de 'n'
            n2 = aghead(e);
            i = busca(*vetor, agnameof(n2), *numNos);
            matriz[z][i] = 1;
            e = agnxtout(g,e);          //Pega o proxima aresta de saida de 'n'
        }
        n1 = agnxtnode(g, n1);          //Pega proximo no de 'g'
    }

    for(z = 0; z < *numNos; z++){
        for(i = 0; i < *numNos; i++)
            printf("%d ", matriz[z][i]);
        printf("\n");
    }

    fclose(f);

    i = agwrite(g, stdout);

    agclose(g);

    return matriz;
}

int busca(char **vet, char *work, int tam){
    int z;
    for(z = 0; z < tam; z++)
        if(!strcmp(work, vet[z]))
            return z;
    return -1;
}




