#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/mac/tsch/tsch.h"
#include "lib/random.h"
#include "sys/node-id.h"

#include <stdio.h>
#include <stdlib.h>
#include "rgraph.h"
#include "conf.h"
#include "criaDOT.h"
#define temp_canais 23
#define peso 1

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_PORT	8765
#define SEND_INTERVAL		  (60 * CLOCK_SECOND) 


void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes);
int *alocaPacotes(int num_no); 
char *colect_addres(char *ex);


 PROCESS(node_process, "TSCH Schedule Node"); 
 AUTOSTART_PROCESSES(&node_process);  


#define APP_SLOTFRAME_HANDLE 1
#define APP_UNICAST_TIMESLOT 1
 

// funções auxiliares para o funcionamento do nó  
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
//-------------------------------------------------------------- 
 // funcoes internas para a função de proximo link ativo 

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

 
// cria um  unico slotframe slotframe   
static  void 
initialize_tsch_schedule_global(void){ 
  
  struct tsch_slotframe *sf_common = tsch_schedule_add_slotframe(APP_SLOTFRAME_HANDLE, APP_SLOTFRAME_SIZE);
  uint16_t slot_offset;
  uint16_t channel_offset;

  /* A "catch-all" cell at (0, 0) */
  slot_offset = 0;
  channel_offset = 0;
  tsch_schedule_add_link(sf_common,
      LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED,
      LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
      slot_offset, channel_offset);
 
    
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
    int addr_integer = 0 ; 
    
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
                           addr_integer = colect_addres(nome_no[conf[aloca_canais[canal][cont]][aux_no]]) - '0';  
                           // atribui o id para o novo endereco                
                           for(j = 0; j < sizeof(addr); j += 2) {
                                addr.u8[j + 1] = addr_integer & 0xff;
                                addr.u8[j + 0] = addr_integer >> 8;
                                }
                            link_options =  LINK_OPTION_TX; 
                            // cria um novo link
                            tsch_schedule_add_link(sf_min, link_options, LINK_TYPE_NORMAL, &addr ,aux_timeslot,aux_channel_offset); 
                             
                            aux_no++; 
                            addr_integer = 0 ;     
                        } 
                        else if(aux_no == 1 ){  
                            addr_integer = colect_addres(nome_no[conf[aloca_canais[canal][cont]][aux_no]]) - '0';  
                            for(j = 0; j < sizeof(addr); j += 2) {
                                addr.u8[j + 1] = addr_integer & 0xff;
                                addr.u8[j + 0] = addr_integer >> 8;
                            }
                            link_options =  LINK_OPTION_RX; 
                            tsch_schedule_add_link(sf_min, link_options, LINK_TYPE_NORMAL, &addr ,aux_timeslot,aux_channel_offset); 
                              aux_no++; 
                            addr_integer = 0 ;     
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
static void
        rx_packet(struct simple_udp_connection *c,
                const uip_ipaddr_t *sender_addr,
                uint16_t sender_port,
                const uip_ipaddr_t *receiver_addr,
                uint16_t receiver_port,
                const uint8_t *data,
                uint16_t datalen)
{
uint32_t seqnum;

if(datalen >= sizeof(seqnum)) {
    memcpy(&seqnum, data, sizeof(seqnum));
        LOG_INFO("Received from ");
        LOG_INFO_6ADDR(sender_addr);
        LOG_INFO_(", seqnum %" PRIu32 "\n", seqnum);
        }
} 

PROCESS_THREAD(node_process, ev, data)
{
  static struct simple_udp_connection udp_conn;
  static struct etimer periodic_timer;
  static uint32_t seqnum;
  uip_ipaddr_t dst;

  PROCESS_BEGIN();

  initialize_tsch_schedule_global();

  /* Initialization; `rx_packet` is the function for packet reception */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, rx_packet);
  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  // colocar no ulitmo como nó raiz  
  if(node_id == 1) {  /* Running on the root? */
    NETSTACK_ROUTING.root_start(); 
    initialize_tsch_schedule_root(); 

  }

  /* Main loop */
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    if(NETSTACK_ROUTING.node_is_reachable()
       && NETSTACK_ROUTING.get_root_ipaddr(&dst)) {
      /* Send network uptime timestamp to the network root node */
      seqnum++;
      LOG_INFO("Send to ");
      LOG_INFO_6ADDR(&dst);
      LOG_INFO_(", seqnum %" PRIu32 "\n", seqnum);
      simple_udp_sendto(&udp_conn, &seqnum, sizeof(seqnum), &dst);
    }
    etimer_set(&periodic_timer, SEND_INTERVAL);
  }

  PROCESS_END();
}


