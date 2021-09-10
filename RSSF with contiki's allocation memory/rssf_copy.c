#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conf.h"
#define temp_canais 1600
#define peso 1 
#define no_raiz 1 
#define endereco "text.txt" 


void executa(int num_aresta, int num_no, int (*aloca_canal)[16][temp_canais], int tempo, int (*mapa_graf_conf)[num_aresta][2], int *pacote_entregue, int raiz, int (*pacotes)[num_no]);
int *alocaPacotes(int num_no); 
void alocaPacotes2(int num_no, struct NetworkGraph *adj, int (*vetor)[num_no]);

int main(){    
    int tamNo; 
    //int **adj = (int**)malloc(MAX_NOS * sizeof(int*));                  //grafo da rede
    ng adj;
    
    int tamAresta,                  //Nº de arestas da rede
    z, i;                       //Variáveis temporárias
    int pacote_entregue = 0, 
    total_pacotes = 0, 
    raiz,                       //Nó raiz do grafo da rede
    flg = 1;                    //Variável temporária
    int cont = 0;               //Time do slotframe
    int aloca_canais[16][temp_canais],         //Slotframe
    x, y, canal = 0,            //Variáveis temporárias
    edge_selected, temp;        //Variáveis temporárias
   // char **nome_no,             //Nome dos nós no grafo da rede
    char *nome_arq_dot = "\0";       //Nom do arquivo contendo o grafo de conflito (não usado)
    int node_origin, node_destin ; 
    int total_canais_alocados = 0;
    int tempo = 0;
    // alocando espaco para receber o endereco 
    /*******************************************************************/ 
    // inicia arquivo  
    FILE *fl;  
    tamNo = MAX_NOS ;  
    tamAresta = MAX_NOS;    
    tempo = clock();
    fl = fopen(endereco, "r"); 
    if(fl == NULL){
        printf("The file was not opened\n");
        return 0  ; 
    } 
    // matriz  

    for(int i = 0 ; i < MAX_NOS ; i++){ 
        for(int j = 0 ; j< MAX_NOS; j++){  
            adj.mat_adj[i][j] = 0 ; 
        }
    }  

    i = 0;
    printf("Enter here!\n");
    while(!feof(fl)){      
        fscanf(fl,"%d %d",&node_origin, &node_destin);   
        // printf(" %d-> %d\n",node_origin, node_destin);    
        if(node_origin < MAX_NOS && node_destin < MAX_NOS){
            if (adj.mat_adj[node_origin][node_destin] == 0 && node_origin != no_raiz){
                adj.mat_adj[node_origin][node_destin] = 1;
                i++;
            }
        }
    }
    tamAresta = i;
    fclose(fl);

    
    int pacotes[tamNo];               //Pacotes por nó no grafo da rede
    alocaPacotes2(tamNo, &adj, &pacotes);


    int conf[tamAresta][2];
    mapGraphConf(&adj, tamNo, tamAresta, &conf);
    //Gera a matriz de conflito
    int matconf[tamAresta][tamAresta];
    fazMatrizConf(tamAresta, &conf, &matconf);

    //Preenche o slotframe com -1
    for(x = 0; x < 16; x++){
        for(y = 0; y < temp_canais; y++)
            aloca_canais[x][y] = -1;
    }

    //Busca pelo nó raiz da rede
    for(z = 0; z < tamNo; z++){
        for(i = 0; i < tamNo; i++)
            if(adj.mat_adj[z][i] != 0){
                flg = 0;
                break;
            }
        if(flg)
            break;
        else
            flg = 1;
    }

    //Por hora definimos ele manualmente
    raiz = no_raiz;

    //Guarda o total de pacotes a serem enviados pela
    for(z = 0; z < tamNo; z++)
        if(z != raiz)
            total_pacotes += pacotes[z];

    ng matching;
    DCFL(tamAresta, tamNo, &pacotes, &matconf, &conf, raiz, &matching);
    while(pacote_entregue < total_pacotes){

        //Aloca os canais
        for(x = 0; x < tamNo; x ++){
            for(y = 0; y < tamNo; y++){
                if(matching.mat_adj[x][y]){
                    for(temp = 0; temp < tamAresta; temp++)
                        if(conf[temp][0] == x && conf[temp][1] == y)
                            break;
                    edge_selected = temp;
                    aloca_canais[canal][cont] = edge_selected; 
                    canal++;
                }
                if(canal == 16)
                    break;
            }
            if(canal == 16)
                break;
        }
        

        //Executa a primeira carga de transferência
        executa(tamAresta, tamNo, &aloca_canais, cont, &conf, &pacote_entregue, raiz, &pacotes);
        cont++;
        canal = 0;
        
        DCFL(tamAresta, tamNo, &pacotes, &matconf, &conf, raiz, &matching);
        total_canais_alocados++;
    }

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
    /*  
    for(y = 0 ; y < temp_canais; y++){ 
        for(x = 0 ; x < 16 ; x++){ 
            printf("%d  ", aloca_canais[x][y] + 1);  
             
        printf("\n");
         } 

    } 
    */ 
    

    //nome_arq_dot = criaGrafoConf(matconf, conf, nome_no, tamAresta);

    FILE *novo;
    novo = fopen("res.txt", "a");
    fprintf(novo,"\nTASA timeslot:%d\n",total_canais_alocados);
    fclose(novo);

    // printf("Relacao numero de nos da rede por numero de timeslots alocados: %f", (float)tamNo/(float)total_canais_alocados);
    // printf("\nTempo de execucao total: %.2fs", ((double)(clock() - tempo)/(double)CLOCKS_PER_SEC));
    // printf("\nGrafo de conflito gerado: %s\n", nome_arq_dot);

    return 0;
}

void executa(int num_aresta, int num_no, int (*aloca_canal)[16][temp_canais], int tempo, int (*mapa_graf_conf)[num_aresta][2], int *pacote_entregue, int raiz, int (*pacotes)[num_no]){
    int i;

    for(i = 0; i < 16; i++){
        if((*aloca_canal)[i][tempo] == -1)
            continue;
        if((*pacotes)[(*mapa_graf_conf)[(*aloca_canal)[i][tempo]][0]] > 0){
            (*pacotes)[(*mapa_graf_conf)[(*aloca_canal)[i][tempo]][0]] -= peso;
            (*pacotes)[(*mapa_graf_conf)[(*aloca_canal)[i][tempo]][1]] += peso;
        }
        if((*mapa_graf_conf)[(*aloca_canal)[i][tempo]][1] == raiz)
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

void alocaPacotes2(int num_no, ng *adj, int (*vetor)[num_no]){
    int x, y, qtd_pacotes = 0;
    //Percorre o vetor de pacotes
    for(x = 0; x < num_no; x++){
        //Percorre a linha da matriz para saber se o nó X está conectado à alguém
        for(y = 0; y < num_no; y++)
            //Se sim, adiciona um pacote
            if(adj->mat_adj[x][y]){
                qtd_pacotes = peso;
                break;
            }

        if(qtd_pacotes)
            (*vetor)[x] = qtd_pacotes;
        else
            (*vetor)[x] = 0;

        //Reseta o contador
        qtd_pacotes = 0;
    }
    
}   
