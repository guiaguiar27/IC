#include "schedule.h"  

void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes){
    int i;

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

char *colect_addres(char *ex){      
    printf("Entrou");  
    int tam1, i, idx = 0;   
    tam1 = strlen(ex);  
    char *colect = (char*) malloc(100 * sizeof(char)); 
    colect[0] = '\0';
    for(i = 0 ; i < tam1 ; i++){   
        //printf("%s\n",colect );
        if(ex[i] == '_'){ 
            i++; 
            while(ex[i] != '_' && ex[i] != '\0'){         
                colect[idx] = ex[i];
                idx++;
                colect[idx] = '\0';
                i++; 
            //printf("%s\n",colect );  
            //if(colect == "_") break ; 
            } 
            //i--;
        }           
        if(ex[i] == '\0') break ;  
    }
    
    while(colect[i] != '\0'){  
        if(colect[i] == ' '){ 
            colect[i] = 'p' ;
        }         
        i++;
        if(colect[i] == '\0') break ; 

    } 
    
    //printf("%s\n",colect);
    return (char *)colect;  
}  

static void 
initialize_tsch_schedule_root()
{
  // variaveis padrão do contiki_ng rodando tsch    
    struct tsch_slotframe *sf_min = tsch_schedule_add_slotframe(APP_SLOTFRAME_HANDLE, APP_SLOTFRAME_SIZE);
    uint16_t aux_timeslot; 
    uint16_t aux_channel_offset; 
    linkaddr_t addr;  
    uint8_t link_options; 
    //char addr_aux ; 
    
    int  i,aux_no = 0 ; 
    // coleta do id do nó  
    //char *colect = (char*) malloc(100 * sizeof(char));  
    //colect[0] = '\0'; 


    int **adj,                  //grafo da rede
    **conf,                     //mapa do grafo de conflito pro grafo da rede
    **matconf,                  //matriz de conflito
    tamNo,                      //Nº de nós da rede
    tamAresta,                  //Nº de arestas da rede
    z, j ;                       //Variáveis temporárias
    int **matching,             //Matching da rede
    pacote_entregue = 0, 
    total_pacotes = 0, 
    raiz,                       //Nó raiz do grafo da rede
    flg = 1;                    //Variável temporária
    int cont = 0;               //Time do slotframe timeslot  
    int **aloca_canais,         //Slotframe
    x, y, canal = 0,            //Variáveis temporárias
    edge_selected, temp;        //Variáveis temporárias
    char **nome_no;              //Nome dos nós no grafo da rede
    //char  *nome_arq_dot = "\0";       //Nom do arquivo contendo o grafo de conflito (não usado)
    int *pacotes; 

    adj = leDOT("arvre.dot", &tamNo, &tamAresta, &nome_no);  
    //Mapeia os nós do grafo de conflito para os respectivos nós do grafo da rede
    conf = mapGraphConf(adj, tamNo, tamAresta);
    //Gera a matriz de conflito
    matconf = fazMatrizConf(conf, adj, tamAresta);   
    pacotes = alocaPacotes(tamNo);
    // inicia a alocaçao de canais  
    aloca_canais = (int**) malloc(16 * sizeof(int*));
    for(x = 0; x < 16; x++){
        aloca_canais[x] = (int*) malloc(temp_canais * sizeof(int));
        for(y = 0; y < temp_canais; y++)
            aloca_canais[x][y] = -1;
    } 
    // encontra o no raiz  
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

    // encontra o total de pacotes  
     
    for(z = 0; z < tamNo; z++)
        if(z != raiz)
            total_pacotes = total_pacotes+ pacotes[z]; 

    matching = DCFL(pacotes, adj, matconf, conf, tamNo, tamAresta, raiz);

     while(pacote_entregue < total_pacotes){
        LOG_INFO("\nMatching\n");
        
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
                        
                        // aloca_canais[canal][cont] representa o link em questão
                        aux_timeslot = cont ;     
                        aux_channel_offset = canal + 11 ;  
                        LOG_PRINT("----- Passagem de informações-----\n"); 
                        // quantidade de links      
                        //nome_no[conf[edge_selected][0]]; ----> no q envia  
                        //nome_no[conf[edge_selected[1]]];--------> no q recebe

                           
                            // cada enlace desse for deve ser um link distinto 
                // tenho q descobrir como passar a informação do link como parametro 
                // para quem vai e pra quem recebe a mensagem  
                
                        if(aux_no ==  0){    
                           //addr_aux =  - '0';  
                           // atribui o id para o novo endereco                
                           for(j = 0; j < sizeof(addr); j += 2) {
                                addr.u8[j + 1] = colect_addres(nome_no[conf[aloca_canais[canal][cont]][aux_no]]) & 0xff;
                                addr.u8[j + 0] = colect_addres(nome_no[conf[aloca_canais[canal][cont]][aux_no]]) >> 8;
                                }
                            link_options =  LINK_OPTION_TX; 
                            // cria um novo link
                            tsch_schedule_add_link(sf_min, link_options, LINK_TYPE_NORMAL, &addr ,aux_timeslot,aux_channel_offset); 
                             
                            aux_no++; 
                            //strcpy(addr_aux , " ");     
                        } 
                        else if(aux_no == 1 ){  
                            //addr_aux = colect_addres(nome_no[conf[aloca_canais[canal][cont]][aux_no]]) - '0';  
                            for(j = 0; j < sizeof(addr); j += 2) {
                                addr.u8[j + 1] = colect_addres(nome_no[conf[aloca_canais[canal][cont]][aux_no]]) & 0xff;
                                addr.u8[j + 0] = colect_addres(nome_no[conf[aloca_canais[canal][cont]][aux_no]]) >> 8;
                            }
                            link_options =  LINK_OPTION_RX; 
                            tsch_schedule_add_link(sf_min, link_options, LINK_TYPE_NORMAL, &addr ,aux_timeslot,aux_channel_offset); 
                              aux_no++; 
                            //strcpy(addr_aux, " ")    
                        }
                            
                        canal++;    
                    }
                }
                if(canal == 16)
                    break;
            }
            if(canal == 16)
                break;
        }
        //Executa a primeira carga de transferência
        executa(aloca_canais, cont, conf, &pacote_entregue, raiz, pacotes);
        cont++;
        canal = 0;
        
        //mostram os pacotes contentes em cada nó da rede
        
        matching = DCFL(pacotes, adj, matconf, conf, tamNo, tamAresta, raiz);
    }     


}  

/* 
 caso precise incluir no makefile
PROJECT_SOURCEFILES += rsa_get_size.c 
MODULES += ./tomcrypt

*/