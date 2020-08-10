#include "criaDOT.h"

char *criaGrafoDir(int **grafo, int tam){
    int x, y, dec = 48, uni = 48;
    char *nome, nos[tam][4], aresta = 65;
    FILE *file;
    srand(time(NULL));
    nome = (char*) malloc(12 * sizeof(char));

    for(x = 0; x < tam; x++){
        nos[x][0] = aresta;
        nos[x][1] = dec;
        nos[x][2] = uni;
        nos[x][3] = '\0';
        (aresta == 90) ? aresta = 65 : aresta++;
        (uni == 9) ? uni = 48, dec++ : uni++;
    }

    for(x = 0; x < 7; x++)
        nome[x] = (rand() % 26) + 65;
    nome[7] = '\0';
    strcat(nome, ".txt");
    
    file = fopen(nome, "wt");
    if(file == NULL)
        return NULL;
    
    fprintf(file, "digraph g{\n");
    for(x = 0; x < tam; x++)
        for(y = 0; y < tam; y++)
            if(grafo[x][y] != 0)
                fprintf(file, "%s -> %s\n", nos[x], nos[y]);
    
    fprintf(file, "}");

    fclose(file);

    return nome;
}

char *criaGrafoNDir(int **grafo, int tam){
    int x, y, dec = 48, uni = 48;
    char *nome, nos[tam][4], aresta = 65;
    FILE *file;
    srand(time(NULL));

    nome = (char*) malloc(12 * sizeof(char));

    for(x = 0; x < tam; x++){
        nos[x][0] = aresta;
        nos[x][1] = dec;
        nos[x][2] = uni;
        nos[x][3] = '\0';
        (aresta == 90) ? aresta = 65 : aresta++;
        (uni == 9) ? uni = 48, dec++ : uni++;
    }

    for(x = 0; x < 7; x++)
        nome[x] = (rand() % 26) + 65;
    nome[7] = '\0';
    strcat(nome, ".txt");
    
    file = fopen(nome, "wt");
    if(file == NULL)
        return NULL;
    
    fprintf(file, "graph g{\n");
    for(x = 0; x < tam; x++)
        for(y = 0; y < tam; y++)
            if(grafo[x][y] != 0)
                fprintf(file, "%s -- %s\n", nos[x], nos[y]), grafo[y][x] = 0;

    fprintf(file, "}");

    fclose(file);

    return nome;
}


    /*
    * grafo: grafo de conflito
    * mapa_grafo_conf: mapeia os nós do "grafo", pros nós do grafo da rede
    * nome_no: nome em caracter dos nós do grafo da rede
    * tam: tamanho de "grafo"
    */
char *criaGrafoConf(int **grafo, int **mapa_graf_conf, char **nome_no, int tam){
    int x, y;
    char *nome, nome_no_conf1[20], nome_no_conf2[20];
    FILE *file;
    srand(time(NULL));

    nome = (char*) malloc(12 * sizeof(char));

    for(x = 0; x < 7; x++)
        nome[x] = (rand() % 26) + 65;
    nome[7] = '\0';
    strcat(nome, ".txt");
    
    file = fopen(nome, "wt");
    if(file == NULL)
        return NULL;
    
    fprintf(file, "graph g{\n");
    for(x = 0; x < tam; x++)
        for(y = x; y < tam; y++)
            if(grafo[x][y] != 0){
                strcpy(nome_no_conf1, nome_no[mapa_graf_conf[x][0]]);
                strcat(nome_no_conf1, nome_no[mapa_graf_conf[x][1]]);

                strcpy(nome_no_conf2, nome_no[mapa_graf_conf[y][0]]);
                strcat(nome_no_conf2, nome_no[mapa_graf_conf[y][1]]);

                fprintf(file, "    %s -- %s\n", nome_no_conf1, nome_no_conf2);
            }

    fprintf(file, "}");

    fclose(file);

    return nome;
}









