#include "conf.h"

    /*
    * matriz: matriz de adjacência do grafo da rede
    * graf_conf: grafo de conflito
    * mapa_graf_conf: Mapa do grafo de conflito
    *   - Mapeia os nós da rede pros nós do grafo de conflito
    * num_no: número de nós do grafo da rede
    * num_aresta: nº de arestas do grafo da rede
    */
void DCFL(uint16_t num_aresta, uint16_t num_no, uint16_t (*pacotes)[num_no], uint16_t (*graf_conf)[num_aresta][num_aresta], uint16_t (*mapa_graf_conf)[num_aresta][2], uint16_t raiz, ng *matching, uint16_t (*vetor)[num_aresta][2]){
    /*
    * x, y: índices de acesso à matriz
    * no_atual: último nó folha usado para iniciar o matching
    */
    uint16_t x ; 
    uint16_t no_atual;
    srand(time(NULL));
    
    //Seleciona o nó com maior carga pra ser transmitida
    no_atual = rand() % num_no;   
    //printtf("sorted nó:%d\n",no_atual);
    while(no_atual == raiz){ 
        no_atual = rand() % num_no;
    }
    for(x = 0; x < num_no; x++){
        if((*pacotes)[x] > (*pacotes)[no_atual] && x != raiz)
            no_atual = x; 
    }
    
    //Encontra qual nó do grafo de conflitos representa a aresta do nó folha selecionado 
    for(x = 0; x < num_aresta; x++)
        if((*mapa_graf_conf)[x][0] == no_atual){
            geraMaching(num_aresta, num_no, pacotes, graf_conf, mapa_graf_conf, x, matching, vetor );
            return;
        }

    printf("Caímos no pior caso\n");
}


    /*
    *   mat_adj: matriz de adjacência do grafo da rede
    *   graf_conf: matriz de ajacência do grafo de conflito
    *   tam: tamanho do grafo de conflito
    *   tam_rede: tamanho do grafo da rede
    *   node: aresta selecionada para iniciar o matching
    */
void geraMaching(uint16_t tam, uint16_t tam_rede, uint16_t (*pacotes)[tam_rede], uint16_t (*graf_conf)[tam][tam], uint16_t (*mapa_graf_conf)[tam][2], uint16_t node, ng *resultado, uint16_t (*vetor)[tam][2]){
    
    /*
    * x, y: usado para percorrer o vetor
    * vetor: Usada para saber quais arestas já foram aceitas ou rejeitadas pro matching
    *       - Possui duas colunas, uma é preenchida com 0's e a outra com 1's.
    *       - A primeira coluna representa os conflitos; se houver conflito entre esta aresta e
    *         alguma outra no mapa do grafo de conflito, esta aresta recebe 1 no vetor.
    *       - A segunda coluna representa as arestas que já foram verificados; se esta aresta já
    *         foi "olhada" (foi rejeitada ou aceita pro matching) ela é marcada com 0, para não 
    *         ser "olhada" novamente.
    * resultado: matriz de adjacência do matching
    */
    uint16_t x, y, maior_peso = -1, cont = 1, flg = 1;

    //Preenchendo com zeros a matriz do matching
    for(x = 0; x < tam_rede; x++){ 
        for(y = 0; y < tam_rede; y++)
            resultado->mat_adj[x][y] = 0;
    }

    //Preenchendo com 0's e 1's o vetor que informa quais nós da matriz de conflito geram conflito com o node
    for(x = 0; x < tam; x++){
        (*vetor)[x][0] = 0;
        (*vetor)[x][1] = 1;
    }

    //Pesquisa os nós que geram conflito com o node
    for(x = 0; x < tam; x++)
        if((*graf_conf)[node][x] != 0){
            (*vetor)[x][0] = 1;
            (*vetor)[x][1] = 0;
        }

    (*vetor)[node][0] = 0;
    (*vetor)[node][1] = 0;
    
    //Pesquisa quais outras arestas do grafo de conflito podem ser transimtidos com o node
    while(cont){
        //Escolhe a aresta com maior peso...
        for(x = 0; x < tam; x++){
            if(!(*vetor)[x][0] && (*vetor)[x][1]){
                if(flg){
                    maior_peso = x;
                    flg = 0;
                }
                else
                    if((*pacotes)[(*mapa_graf_conf)[x][0]] > (*pacotes)[(*mapa_graf_conf)[maior_peso][0]])
                        maior_peso = x;
            }
        }
        //...e caso haja uma aresta com maior peso...
        if(maior_peso != -1){
            (*vetor)[maior_peso][1] = 0;
            //...retira as arestas que geram conflito com ela
            for(x = 0; x < tam; x++)
                if((*graf_conf)[maior_peso][x] != 0 && x != node){
                    (*vetor)[x][0] = 1;
                    (*vetor)[x][1] = 0;
                }
            maior_peso = -1;
        }
        cont = 0;
        for(x = 0; x < tam; x++)
            if((*vetor)[x][1])
                cont = 1;
        flg = 1;
    }

    //Preenche a matriz de adjacência com as arestas que podem transmitir ao mesmo tempo
    for(x = 0; x < tam; x++){
        if((*vetor)[x][0] == 0 && (*pacotes)[(*mapa_graf_conf)[x][0]] > 0){
            resultado->mat_adj[(*mapa_graf_conf)[x][0]][(*mapa_graf_conf)[x][1]] = 1;
        }
    }
}

//Armazena as arestas de um grafo e os respectivos nós q o compõem
    /*
    * mat: matriz de adjacência da rede
    * tam: nº de nós do grafo da rede
    */
void mapGraphConf(ng *mat, uint16_t tam_no, uint16_t tam_aresta, uint16_t (*alocado)[tam_aresta][2]){
    /*
    * alocado: matriz de duas posições que informa os nós de cada aresta da matriz de conflito
    * x, y: índices da matriz
    * noConf: representa o nó DO grafo de conflito
    */
    uint16_t x, y;
    uint16_t noConf = 0;

    //"Captura" as arestas e armazena
    for(x = 0; x < tam_no; x++)
        for(y = 0; y < tam_no; y++)
            if(mat->mat_adj[x][y] != 0){
                (*alocado)[noConf][0] = x;
                (*alocado)[noConf][1] = y;
                noConf++;
            }
}
    /*
    * matriz[noConf][0] = Posição do primeiro nó da aresta representada por noConf
    * matriz[noConf][1] = Posição do segundo nó da aresta representada por noConf
    * noConf (Representa o nó q será usado na matriz de conflito)
    * o sentido das arestas são:
    * alocado[noConf][0] -> alocado[noConf][1]
    */


//Gera a matriz de adjacência do grafo de conflito
   /*
   * mapConf: Matriz que mapeia os nós do grafo de conflito pros nós do grafo da rede
   * tam: nº de arestas do grafo da rede
   */
void fazMatrizConf(uint16_t tam_arest, uint16_t (*mapConf)[tam_arest][2], uint16_t (*grafoconf)[tam_arest][tam_arest]){
    uint16_t x, y, z, i;

    //Preenche a matriz de conflito
    for(x = 0; x < tam_arest; x++){
        for(y = 0; y < tam_arest; y++)
            (*grafoconf)[x][y] = 0;
    }

    //Percorre mapConf: se algum nó que compõe essa aresta...
    //...esta presente em outra aresta, o conflito é marcado em grafoconf
    for(x = 0; x < tam_arest; x++)
        for(y = x; y < tam_arest; y++)
            for(z = 0; z < 2; z++){
                if((*mapConf)[x][z] == (*mapConf)[y][0]){
                    (*grafoconf)[x][y] = 1;
                    (*grafoconf)[y][x] = 1;
                    for(i = 0; i < tam_arest; i++)
                        if((*mapConf)[i][0] == (*mapConf)[y][1] || (*mapConf)[i][1] == (*mapConf)[y][1]){
                            (*grafoconf)[x][i] = 1;
                            (*grafoconf)[i][x] = 1;
                        }
                }
                if((*mapConf)[x][z] == (*mapConf)[y][1]){
                    (*grafoconf)[x][y] = 1;
                    (*grafoconf)[y][x] = 1;
                    for(i = 0; i < tam_arest; i++)
                        if((*mapConf)[i][0] == (*mapConf)[y][0] || (*mapConf)[i][1] == (*mapConf)[y][0]){
                            (*grafoconf)[x][i] = 1;
                            (*grafoconf)[i][x] = 1;
                        }
                }
            }

    for(x = 0; x < tam_arest; x++)
        (*grafoconf)[x][x] = 0;
}



