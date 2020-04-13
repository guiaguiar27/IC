#include <stdio.h>
#include <stdlib.h>
#include "rgraph.h"
#include "conf.h"
#include "criaDOT.h"
#define temp_canais 23
#define peso 1

void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes);
int *alocaPacotes(int num_no);

int main(){
    int **adj, **conf, **matconf, tamNo, tamAresta, z, i, no_atual = 5;
    int **matching, pacote_entregue = 0, total_pacotes = 0, raiz, flg = 1;
    int cont = 0;
    int **aloca_canais, x, y, canal = 0, edge_selected, temp;
    char **nome_no, *nome_arq_dot = "\0";
    int *pacotes;

    adj = leDOT("arvre.dot", &tamNo, &tamAresta, &nome_no);
    pacotes = alocaPacotes(tamNo);
    conf = mapGraphConf(adj, tamNo, tamAresta);
    matconf = fazMatrizConf(conf, adj, tamAresta);

    aloca_canais = (int**) malloc(16 * sizeof(int*));
    for(x = 0; x < 16; x++){
        aloca_canais[x] = (int*) malloc(temp_canais * sizeof(int));
        for(y = 0; y < temp_canais; y++)
            aloca_canais[x][y] = -1;
    }

    for(z = 0; z < tamNo; z++){
        for(i = 0; i < tamNo; i++)
            if(adj[z][i] != 0){
                flg = 0;
                break;
            }
        if(flg)
            break;
        else
            flg = 1;
    }
    raiz = z;

    printf("\nNúmero de nós: %d\nNúmero de arestas: %d\nNó raiz: %s\n\n", tamNo, tamAresta, nome_no[raiz]);

    //adj[1][4] = 0;
    //adj[2][1] = 2;
    //adj[5][2] = 0;

    for(z = 0; z < tamAresta; z++)
        printf("[%d] = %s - > %s\n", z + 1, nome_no[conf[z][0]], nome_no[conf[z][1]]);

    //Guarda o total de pacotes a serem enviados pela
    for(z = 0; z < tamNo; z++)
        if(z != raiz)
            total_pacotes += pacotes[z];

    printf("\nMatriz de adjacencia do grafo de conflito\n");
    for(z = 0; z < tamAresta; z++){
        for(i = 0; i < tamAresta; i++)
            printf("%d ", matconf[z][i]);
        printf("\n");
    }

    //mostram os pacotes contentes em cada nó da rede
    printf("\nPacotes por nó da rede\nTempo: %d\nPacotes entregues: %d\nTotal de pacotes: %d\n", cont, pacote_entregue, total_pacotes);
    for(z = 0; z < tamNo; z++){
        printf("[%s] - > %d\n", nome_no[z], pacotes[z]);
    }
    printf("\n");
    matching = DCFL(pacotes, adj, matconf, conf, tamNo, tamAresta, raiz);
    
    while(pacote_entregue < total_pacotes){
        printf("\nMatching\n");
        for(x = 0; x < tamNo; x++){
            for(y = 0; y < tamNo; y++)
                printf("%d ", matching[x][y]);
            printf("\n");
        }

        //Aloca os canais
        for(x = 0; x < tamNo; x ++){
            for(y = 0; y < tamNo; y++){
                if(matching[x][y]){
                    for(temp = 0; temp < tamAresta; temp++)
                        if(conf[temp][0] == x && conf[temp][1] == y)
                            break;
                    edge_selected = temp;
                    for(temp = 0; temp < pacotes[conf[edge_selected][0]]; temp++){
                        if(canal == 16)
                            break;
                        aloca_canais[canal][cont] = edge_selected;
                        canal++;
                    }
                }
                if(canal == 16)
                    break;
            }
            if(canal == 16)
                break;
        }
        
        printf("\nCanais alocados  | |");
        printf("\n                \\   /");
        printf("\n                 \\ /\n\n");
        for(x = 0; x < 16; x++){
            for(y = 0; y < temp_canais; y++)
                printf("%d  ", aloca_canais[x][y] + 1);
            printf("\n");
        }
        printf("\n");

        //Executa a primeira carga de transferência
        executa(aloca_canais, cont, conf, &pacote_entregue, raiz, pacotes);
        cont++;
        canal = 0;
        
        //mostram os pacotes contentes em cada nó da rede
        printf("\nPacotes por nó da rede\nTempo: %d\nPacotes entregues: %d\nTotal de pacotes: %d\n", cont, pacote_entregue, total_pacotes);
        for(z = 0; z < tamNo; z++){
            printf("[%s] - > %d\n", nome_no[z], pacotes[z]);
        }
        printf("\n");
        matching = DCFL(pacotes, adj, matconf, conf, tamNo, tamAresta, raiz);
    }

    printf("\nCanais alocados  | |");
    printf("\n                \\   /");
    printf("\n                 \\ /\n\n");
    for(x = 0; x < 16; x++){
        for(y = 0; y < temp_canais; y++)
            printf("%d  ", aloca_canais[x][y] + 1);
        printf("\n");
    }

    //nome_arq_dot = criaGrafoConf(matconf, conf, nome_no, tamAresta);

    printf("\nGrafo de conflito gerado: %s\n", nome_arq_dot);

    return 0;
}

void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes){
    int x, y, z, i;

    for(i = 0; i < 16; i++){
        if(aloca_canal[i][tempo] == -1)
            continue;
        if(pacotes[mapa_graf_conf[aloca_canal[i][tempo]][0]] > 0){
            pacotes[mapa_graf_conf[aloca_canal[i][tempo]][0]]--;
            pacotes[mapa_graf_conf[aloca_canal[i][tempo]][1]]++;
        }
        if(mapa_graf_conf[aloca_canal[i][tempo]][1] == raiz)
            (*pacote_entregue)++;
    }
}

int *alocaPacotes(int num_no){
    int *vetor, x;
    vetor = (int*) malloc(num_no * sizeof(int));
    for(x = 0; x < num_no; x++)
        vetor[x] = peso;
    return vetor;
}





