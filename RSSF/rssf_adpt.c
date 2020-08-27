/* 
na schedule já há includes básicos 
#include <stdio.h>
#include <stdlib.h>
*/
#include "rgraph.h"
#include "conf.h"
#include "criaDOT.h"
#define temp_canais 23
#define peso 1

void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes);
int *alocaPacotes(int num_no);

int gera_matriz(){     
    int tamNo; 
    int **adj,                  //grafo da rede
    **conf,                     //mapa do grafo de conflito pro grafo da rede
    **matconf,                      //Nº de nós da rede
    tamAresta,                  //Nº de arestas da rede
    z, i;                       //Variáveis temporárias
    int **matching,             //Matching da rede
    pacote_entregue = 0, 
    total_pacotes = 0, 
    raiz,                       //Nó raiz do grafo da rede
    flg = 1;                    //Variável temporária
    int cont = 0;               //Time do slotframe
    int **aloca_canais,         //Slotframe
    x, y, canal = 0,            //Variáveis temporárias
    edge_selected, temp;        //Variáveis temporárias
    char **nome_no,             //Nome dos nós no grafo da rede
    *nome_arq_dot = "\0";       //Nom do arquivo contendo o grafo de conflito (não usado)
    int *pacotes;               //Pacotes por nó no grafo da rede

    // alocando espaco para receber o endereco 

    //Lê o arquivo .dot
    adj = leDOT("arvre.dot", &tamNo, &tamAresta, &nome_no);
    //Aloca os pacotes para cada nó
    pacotes = alocaPacotes(tamNo);
    //Mapeia os nós do grafo de conflito para os respectivos nós do grafo da rede
    conf = mapGraphConf(adj, tamNo, tamAresta);
    //Gera a matriz de conflito
    matconf = fazMatrizConf(conf, adj, tamAresta);
    //Aloca o slotframe e o preenche com -1
    aloca_canais = (int**) malloc(16 * sizeof(int*));
    for(x = 0; x < 16; x++){
        aloca_canais[x] = (int*) malloc(temp_canais * sizeof(int));
        for(y = 0; y < temp_canais; y++)
            aloca_canais[x][y] = -1;
    }

    //Busca pelo nó raiz da rede
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

    
    //Guarda o total de pacotes a serem enviados pela
    for(z = 0; z < tamNo; z++)
        if(z != raiz)
            total_pacotes += pacotes[z];

    //Mostram os pacotes contentes em cada nó da rede
    matching = DCFL(pacotes, adj, matconf, conf, tamNo, tamAresta, raiz);
    
    while(pacote_entregue < total_pacotes){    
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
                if(canal == 16) break;
            }
            if(canal == 16) break;
        }
        //Executa a primeira carga de transferência
        executa(aloca_canais, cont, conf, &pacote_entregue, raiz, pacotes);
        cont++; canal = 0; 
        //mostram os pacotes contentes em cada nó da rede
        matching = DCFL(pacotes, adj, matconf, conf, tamNo, tamAresta, raiz);
    } return 0; 
    //nome_arq_dot = criaGrafoConf(matconf, conf, nome_no, tamAresta);
    
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





