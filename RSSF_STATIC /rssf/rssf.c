#include <stdio.h>
#include <stdlib.h>
#include "conf.h"
#define temp_canais 23
#define peso 1 
#define MAX_NOS 126
#define no_raiz 1

void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes);
int *alocaPacotes(int num_no); 
int *alocaPacotes2(int num_no, int **adj, int *vetor); 

int main(){    
    int tamNo;  
    tamNo = MAX_NOS - 1 ; 
    int adj[tamNo][tamNo];                  //grafo da rede
    
    int **conf ,                     //mapa do grafo de conflito pro grafo da rede
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
   // char **nome_no,             //Nome dos nós no grafo da rede
    char *nome_arq_dot = "\0";       //Nom do arquivo contendo o grafo de conflito (não usado)
    int pacotes[tamNo];               //Pacotes por nó no grafo da rede
    int node_origin, node_destin ; 
    // alocando espaco para receber o endereco 
    /*******************************************************************/ 
    // inicia arquivo  
    FILE *fl;   
    tamAresta = MAX_NOS;    
    fl = fopen("text.txt", "r"); 
    if(fl == NULL){
        printf("The file was not opened\n");
        return 0  ; 
    } 
    // matriz  

    for(int i = 0 ; i < tamNo ; i++){ 
        for(int j = 0 ; j< tamNo; j++){  
            adj[i][j] = 0 ; 
        }
    }  

    i = 0;
    while(!feof(fl)){      
        fscanf(fl,"%d %d",&node_origin, &node_destin);   
        if(node_origin < MAX_NOS && node_destin < MAX_NOS){
            if (adj[node_origin][node_destin] == 0 && node_origin != no_raiz){
                printf(" %d-> %d\n",node_origin, node_destin);    
                adj[node_origin][node_destin] = 1;
                i++;
            } 
        } 
        if(feof(fl)) break ;
    }
    tamAresta = i;

    for(int i = 1; i <= tamNo; i++){ 
        for(int j = 1 ;j <= tamNo; j++)
             printf("%d", adj[i][j]);
        printf("\n");
    }
     
    alocaPacotes2(tamNo, adj, pacotes);
    // //Mapeia os nós do grafo de conflito para os respectivos nós do grafo da rede

    // conf = mapGraphConf(adj, tamNo, tamAresta); 
    // printf("Matriz de conflito gerada"); 
    
    
    // //Gera a matriz de conflito
    // matconf = fazMatrizConf(conf, adj, tamAresta);

    // //Aloca o slotframe e o preenche com -1
    // aloca_canais = (int**) malloc(16 * sizeof(int*));
    // for(x = 0; x < 16; x++){
    //     aloca_canais[x] = (int*) malloc(temp_canais * sizeof(int));
    //     for(y = 0; y < temp_canais; y++)
    //         aloca_canais[x][y] = -1;
    
    // }

    // //Busca pelo nó raiz da rede
    // for(z = 0; z < tamNo; z++){
    //     for(i = 0; i < tamNo; i++)
    //         if(adj[z][i] != 0){
    //             flg = 0;
    //             break;
    //         }
    //     if(flg)
    //         break;
    //     else
    //         flg = 1;
    // }

    // //Por hora definimos ele manualmente
    // raiz = no_raiz;

    // //printf("\nNúmero de nós: %d \nNúmero de arestas: %d \nNome do nó raiz: %s \nNúmero do nó raiz: %d \n\n", tamNo, tamAresta, nome_no[raiz], raiz);

    // //adj[1][4] = 0;
    // //adj[2][1] = 2;
    // //adj[5][2] = 0;

    // //Mostra as arestas q representam cada nó do grafo de conflito
    // /*for(z = 0; z < tamAresta; z++)
    //     printf("[%d] = %s - > %s\n", z + 1, nome_no[conf[z][0]], nome_no[conf[z][1]]);
    // */  
    // //Guarda o total de pacotes a serem enviados pela
    // for(z = 1; z < tamNo; z++)
    //     //if(z != raiz)
    //         total_pacotes += pacotes[z];

    // printf("\nMatriz de adjacencia do grafo de conflito\n");
    // for(z = 0; z < tamAresta; z++){
    //     for(i = 0; i < tamAresta; i++)
    //         printf("%d ", matconf[z][i]);
    //     printf("\n");
    // }

    // //Mostram os pacotes contentes em cada nó da rede
    // printf("\nPacotes por nó da rede\nTempo: %d\nPacotes entregues: %d\nTotal de pacotes: %d\n", cont, pacote_entregue, total_pacotes);
    // /*
    // for(z = 0; z < tamNo; z++){
    //     printf("[%s] - > %d\n", nome_no[z], pacotes[z]);
    // }
    // printf("\n");
    // */
    // matching = DCFL(pacotes, adj, matconf, conf, tamNo, tamAresta, raiz);
    // while(pacote_entregue < total_pacotes){
    //     printf("\nMatching\n");
    //     for(x = 0; x < tamNo; x++){
    //         for(y = 0; y < tamNo; y++)
    //             printf("%d ", matching[x][y]);
    //         printf("\n");
    //     }
    //     printf("\nPacotes:\n");
    //     for(x = 1; x < tamNo ; x++)
    //         printf("Nó %d: %d pacotes\n", x, pacotes[x]);

    //     //Aloca os canais
    //     for(x = 0; x < tamNo; x ++){
    //         for(y = 0; y < tamNo; y++){
    //             if(matching[x][y]){
    //                 for(temp = 0; temp < tamAresta; temp++)
    //                     if(conf[temp][0] == x && conf[temp][1] == y)
    //                         break;
    //                 edge_selected = temp;
    //                 for(temp = 0; temp < pacotes[conf[edge_selected][0]]; temp++){
    //                     if(canal == 16)
    //                         break;
    //                     aloca_canais[canal][cont] = edge_selected; 
    //                     canal++;
    //                 }
    //             }
    //             if(canal == 16)
    //                 break;
    //         }
    //         if(canal == 16)
    //             break;
    //     }
        
    //     printf("\nCanais alocados  | |");
    //     printf("\n                \\   /");
    //     printf("\n                 \\ /\n\n");
    //     for(x = 0; x < 16; x++){
    //         for(y = 0; y < temp_canais; y++)
    //             printf("%d  ", aloca_canais[x][y] + 1);
    //         printf("\n");
    //     }
    //     printf("\n");

    //     //Executa a primeira carga de transferência
    //     executa(aloca_canais, cont, conf, &pacote_entregue, raiz, pacotes);
    //     cont++;
    //     canal = 0;
        
    //     //mostram os pacotes contentes em cada nó da rede
    //     printf("\nPacotes por nó da rede\nTempo: %d\nPac    otes entregues: %d\nTotal de pacotes: %d\n", cont, pacote_entregue, total_pacotes);
        
    //     /*for(z = 0; z < tamNo; z++){
    //         printf("[%s] - > %d\n", nome_no[z], pacotes[z]);
    //     }
    //     printf("\n");
    //     */ 
    //     matching = DCFL(pacotes, adj, matconf, conf, tamNo, tamAresta, raiz);
    
    // }

    // printf("\nCanais alocados  | |");
    // printf("\n                \\   /");
    // printf("\n                 \\ /\n\n");
    // printf(" temp_canais =  %d\n",temp_canais);
    // for(x = 0 ; x < 16; x++){
    //     for(y = 0; y < temp_canais; y++) 
    //         // linhas = tempo - coluna = canal  
    //         printf("%d  ", aloca_canais[x][y] + 1);  
             
    //     printf("\n"); 
    // } 
    // /*  
    // for(y = 0 ; y < temp_canais; y++){ 
    //     for(x = 0 ; x < 16 ; x++){ 
    //         printf("%d  ", aloca_canais[x][y] + 1);  
             
    //     printf("\n");
    //      } 

    // } 
    // */ 
    

    // //nome_arq_dot = criaGrafoConf(matconf, conf, nome_no, tamAresta);

    // printf("\nGrafo de conflito gerado: %s\n", nome_arq_dot);
    
    // free(adj);  
    // free(conf); 
    // free(matconf); 
    // free(aloca_canais); 
    // free(pacotes); 

    return 0;
}

void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes){
    int i;

    for(i = 0; i < 16; i++){
        if(aloca_canal[i][tempo] == -1)
            continue;
        if(pacotes[mapa_graf_conf[aloca_canal[i][tempo]][0]] > 0){
            pacotes[mapa_graf_conf[aloca_canal[i][tempo]][0]] -= peso;
            pacotes[mapa_graf_conf[aloca_canal[i][tempo]][1]] += peso;
        }
        if(mapa_graf_conf[aloca_canal[i][tempo]][1] == raiz)
            (*pacote_entregue) += peso;
    }
}

int *alocaPacotes(int num_no){
    int *vetor, x;
    vetor = (int*) malloc(num_no * sizeof(int));
    for(x = 0; x < num_no; x++)
        vetor[x] = peso;
    return vetor;
}  

int *alocaPacotes2(int num_no, int **adj, int *vetor){
    int  x, y, qtd_pacotes = 0;
    //Percorre o vetor de pacotes
    for(x = 0; x < num_no; x++){
        //Percorre a linha da matriz para saber se o nó X está conectado à alguém
        for(y = 0; y < num_no; y++)
            //Se sim, adiciona um pacote
            if(adj[x][y]){
                qtd_pacotes = peso;
                break;
            }

        if(qtd_pacotes)
            vetor[x] = qtd_pacotes;
        else
            vetor[x] = 0;

        //Reseta o contador
        qtd_pacotes = 0;
    }
    
    return 0;
}   
