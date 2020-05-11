
#include "contiki.h"
#include "dev/leds.h"
#include "lib/memb.h"
#include "net/nbr-table.h"
#include "net/packetbuf.h"
#include "net/queuebuf.h"
#include "net/mac/tsch/tsch.h"
#include "net/mac/framer/frame802154.h"
#include "sys/process.h"
#include "sys/rtimer.h"
#include <string.h> 

#include "sys/log.h"
#define LOG_MODULE "TSCH Sched"
#define LOG_LEVEL LOG_LEVEL_MAC

// includes syphony   
#include "rgraph.h"
#include "conf.h"
#include "criaDOT.h"
#define temp_canais 23
#define peso 1

/* Pre-allocated space for links */
MEMB(link_memb, struct tsch_link, TSCH_SCHEDULE_MAX_LINKS);
/* Pre-allocated space for slotframes */
MEMB(slotframe_memb, struct tsch_slotframe, TSCH_SCHEDULE_MAX_SLOTFRAMES);
/* List of slotframes (each slotframe holds its own list of links) */
LIST(slotframe_list);  
// inicia a construção do slotframe  

struct tsch_slotframe *
tsch_schedule_add_slotframe(uint16_t handle, uint16_t size)
{
  if(size == 0) {
    return NULL;
  }

  if(tsch_schedule_get_slotframe_by_handle(handle)) {
    /* A slotframe with this handle already exists */
    return NULL;
  }

  if(tsch_get_lock()) {
    struct tsch_slotframe *sf = memb_alloc(&slotframe_memb);
    if(sf != NULL) {
      /* Initialize the slotframe */
      sf->handle = handle;
      TSCH_ASN_DIVISOR_INIT(sf->size, size);
      LIST_STRUCT_INIT(sf, links_list);
      /* Add the slotframe to the global list */
      list_add(slotframe_list, sf);
    }
    LOG_INFO("add_slotframe %u %u\n",
           handle, size);
    tsch_release_lock();
    return sf;
  }
  return NULL;
}  

// ___________________________________________________- 
// remove todos os slotframes tornando o escalonamento vazio 
int
tsch_schedule_remove_all_slotframes(void)
{
  struct tsch_slotframe *sf;
  while((sf = list_head(slotframe_list))) {
    if(tsch_schedule_remove_slotframe(sf) == 0) {
      return 0;
    }
  }
  return 1;
}  
// --------------------------------------------------  
// remove slotframe e retorna 1 para sucesso e 0 para falha 
int
tsch_schedule_remove_slotframe(struct tsch_slotframe *slotframe)
{
  if(slotframe != NULL) {
    /* Remove all links belonging to this slotframe */
    struct tsch_link *l;
    while((l = list_head(slotframe->links_list))) {
      tsch_schedule_remove_link(slotframe, l);
    }

    /* Now that the slotframe has no links, remove it. */
    if(tsch_get_lock()) {
      LOG_INFO("remove slotframe %u %u\n", slotframe->handle, slotframe->size.val);
      memb_free(&slotframe_memb, slotframe);
      list_remove(slotframe_list, slotframe);
      tsch_release_lock();
      return 1;
    }
  }
  return 0;
} 

// --------------------------------------------------  
// busca slotframes a partir do handle  
struct tsch_slotframe *
tsch_schedule_get_slotframe_by_handle(uint16_t handle)
{
  if(!tsch_is_locked()) {
    struct tsch_slotframe *sf = list_head(slotframe_list);
    while(sf != NULL) {
      if(sf->handle == handle) {
        return sf;
      }
      sf = list_item_next(sf);
    }
  }
  return NULL;
}  
// _--------------------------------------------------_-__--___--_ 
/* Looks for a link from a handle */
struct tsch_link *
tsch_schedule_get_link_by_handle(uint16_t handle)
{
  if(!tsch_is_locked()) {
    struct tsch_slotframe *sf = list_head(slotframe_list);
    while(sf != NULL) {
      struct tsch_link *l = list_head(sf->links_list);
      /* Loop over all items. Assume there is max one link per timeslot */
      while(l != NULL) {
        if(l->handle == handle) {
          return l;
        }
        l = list_item_next(l);
      }
      sf = list_item_next(sf);
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
static const char *
print_link_options(uint16_t link_options)
{
  static char buffer[20];
  unsigned length;

  buffer[0] = '\0';
  if(link_options & LINK_OPTION_TX) {
    strcat(buffer, "Tx|");
  }
  if(link_options & LINK_OPTION_RX) {
    strcat(buffer, "Rx|");
  }
  if(link_options & LINK_OPTION_SHARED) {
    strcat(buffer, "Sh|");
  }
  length = strlen(buffer);
  if(length > 0) {
    buffer[length - 1] = '\0';
  }

  return buffer;
}
/*---------------------------------------------------------------------------*/
static const char *
print_link_type(uint16_t link_type)
{
  switch(link_type) {
  case LINK_TYPE_NORMAL:
    return "NORMAL";
  case LINK_TYPE_ADVERTISING:
    return "ADV";
  case LINK_TYPE_ADVERTISING_ONLY:
    return "ADV_ONLY";
  default:
    return "?";
  }
}   
// ------------------------------------------------------------------------------------------- 
// add link para um slotframe  
struct tsch_link *
tsch_schedule_add_link(struct tsch_slotframe *slotframe,
                       uint8_t link_options, enum link_type link_type, const linkaddr_t *address,
                       uint16_t timeslot, uint16_t channel_offset)
{
  struct tsch_link *l = NULL;
  if(slotframe != NULL) {
    /* We currently support only one link per timeslot in a given slotframe. */

    /* Validation of specified timeslot and channel_offset */
    if(timeslot > (slotframe->size.val - 1)) {
      LOG_ERR("! add_link invalid timeslot: %u\n", timeslot);
      return NULL;
    }

    /* Start with removing the link currently installed at this timeslot (needed
     * to keep neighbor state in sync with link options etc.) */
    tsch_schedule_remove_link_by_timeslot(slotframe, timeslot);
    if(!tsch_get_lock()) {
      LOG_ERR("! add_link memb_alloc couldn't take lock\n");
    } else {
      l = memb_alloc(&link_memb);
      if(l == NULL) {
        LOG_ERR("! add_link memb_alloc failed\n");
        tsch_release_lock();
      } else {
        static int current_link_handle = 0;
        struct tsch_neighbor *n;
        /* Add the link to the slotframe */
        list_add(slotframe->links_list, l);
        /* Initialize link */
        l->handle = current_link_handle++;
        l->link_options = link_options;
        l->link_type = link_type;
        l->slotframe_handle = slotframe->handle;
        l->timeslot = timeslot;
        l->channel_offset = channel_offset;
        l->data = NULL;
        if(address == NULL) {
          address = &linkaddr_null;
        }
        linkaddr_copy(&l->addr, address);

        LOG_INFO("add_link sf=%u opt=%s type=%s ts=%u ch=%u addr=",
                 slotframe->handle,
                 print_link_options(link_options),
                 print_link_type(link_type), timeslot, channel_offset);
        LOG_INFO_LLADDR(address);
        LOG_INFO_("\n");
        /* Release the lock before we update the neighbor (will take the lock) */
        tsch_release_lock();

        if(l->link_options & LINK_OPTION_TX) {
          n = tsch_queue_add_nbr(&l->addr);
          /* We have a tx link to this neighbor, update counters */
          if(n != NULL) {
            n->tx_links_count++;
            if(!(l->link_options & LINK_OPTION_SHARED)) {
              n->dedicated_tx_links_count++;
            }
          }
        }
      }
    }
  }
  return l;
} 
// ------------------------------------------------------------------------  
// 
int
tsch_schedule_remove_link(struct tsch_slotframe *slotframe, struct tsch_link *l)
{
  if(slotframe != NULL && l != NULL && l->slotframe_handle == slotframe->handle) {
    if(tsch_get_lock()) {
      uint8_t link_options;
      linkaddr_t addr;

      /* Save link option and addr in local variables as we need them
       * after freeing the link */
      link_options = l->link_options;
      linkaddr_copy(&addr, &l->addr);

      /* The link to be removed is scheduled as next, set it to NULL
       * to abort the next link operation */
      if(l == current_link) {
        current_link = NULL;
      }
      LOG_INFO("remove_link sf=%u opt=%s type=%s ts=%u ch=%u addr=",
               slotframe->handle,
               print_link_options(l->link_options),
               print_link_type(l->link_type), l->timeslot, l->channel_offset);
      LOG_INFO_LLADDR(&l->addr);
      LOG_INFO_("\n");

      list_remove(slotframe->links_list, l);
      memb_free(&link_memb, l);

      /* Release the lock before we update the neighbor (will take the lock) */
      tsch_release_lock();

      /* This was a tx link to this neighbor, update counters */
      if(link_options & LINK_OPTION_TX) {
        struct tsch_neighbor *n = tsch_queue_add_nbr(&addr);
        if(n != NULL) {
          n->tx_links_count--;
          if(!(link_options & LINK_OPTION_SHARED)) {
            n->dedicated_tx_links_count--;
          }
        }
      }

      return 1;
    } else {
      LOG_ERR("! remove_link memb_alloc couldn't take lock\n");
    }
  }
  return 0;
} 
/*---------------------------------------------------------------------------*/
/* Removes a link from slotframe and timeslot. Return a 1 if success, 0 if failure */
int
tsch_schedule_remove_link_by_timeslot(struct tsch_slotframe *slotframe, uint16_t timeslot)
{
  return slotframe != NULL &&
         tsch_schedule_remove_link(slotframe, tsch_schedule_get_link_by_timeslot(slotframe, timeslot));
}
// --------------------------------------------------------------------------------------
struct tsch_link *
tsch_schedule_get_link_by_timeslot(struct tsch_slotframe *slotframe, uint16_t timeslot)
{
  if(!tsch_is_locked()) {
    if(slotframe != NULL) {
      struct tsch_link *l = list_head(slotframe->links_list);
      /* Loop over all items. Assume there is max one link per timeslot */
      while(l != NULL) {
        if(l->timeslot == timeslot) {
          return l;
        }
        l = list_item_next(l);
      }
      return l;
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/ 
// funçoes anteriores necessarias 
void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes);
int *alocaPacotes(int num_no);
// -------------------------------------------------------------------------------




struct tsch_link *
tsch_schedule_get_next_active_link(struct tsch_asn_t *asn, uint16_t *time_offset,
    struct tsch_link **backup_link)
{  // sera necessario passar alguns parametros para o modelo do tsch 
    // o algoritmo "Symphony" não pode ser configurado sem os TADs do TSCH no contiki   
    // por isso a criação de links e dos slotframes  
    /*  
    TAD TSCH_link  
    composição : 
    struct tsch_link {
 
        struct tsch_link *next; 
        uint16_t handle;
        linkaddr_t addr; 
        uint16_t slotframe_handle; 
        uint8_t handle;
        uint16_t timeslot;
        uint16_t channel_offset;
        uint8_t link_options; 
        enum link_type link_type;
        void *data;
    };  
*/ 
    uint16_t time_to_curr_best = 0;
    struct tsch_link *curr_best = NULL;
    struct tsch_link *curr_backup = NULL; 

    int **adj,                  //grafo da rede
    **conf,                     //mapa do grafo de conflito pro grafo da rede
    **matconf,                  //matriz de conflito
    tamNo,                      //Nº de nós da rede
    tamAresta,                  //Nº de arestas da rede
    z, i;                       //Variáveis temporárias
    int **matching,             //Matching da rede
    pacote_entregue = 0, 
    total_pacotes = 0, 
    raiz,                       //Nó raiz do grafo da rede
    flg = 1;                    //Variável temporária
    int cont = 0;               //Time do slotframe timeslot  
    int **aloca_canais,         //Slotframe
    x, y, canal = 0,            //Variáveis temporárias
    edge_selected, temp;        //Variáveis temporárias
    char **nome_no,             //Nome dos nós no grafo da rede
    *nome_arq_dot = "\0";       //Nom do arquivo contendo o grafo de conflito (não usado)
    int *pacotes;   


    adj = leDOT("arvre.dot", &tamNo, &tamAresta, &nome_no);  
    //Mapeia os nós do grafo de conflito para os respectivos nós do grafo da rede
    conf = mapGraphConf(adj, tamNo, tamAresta);
    
    //Gera a matriz de conflito
    matconf = fazMatrizConf(conf, adj, tamAresta);  
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
    LOG_INFO("\nNúmero de nós: %d \nNúmero de arestas: %d \nNome do nó raiz: %s \nNúmero do nó raiz: %d \n\n", tamNo, tamAresta, nome_no[raiz], raiz); 

    //Guarda o total de pacotes a serem enviados pela
    for(z = 0; z < tamNo; z++)
        if(z != raiz)
            total_pacotes += pacotes[z]; 

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
                        canal++;
                    }
                }
                if(canal == 16)
                    break;
            }
            if(canal == 16)
                break;
        }
        LOG_INFO("\nCanais alocados  | |");
        

        //Executa a primeira carga de transferência
        executa(aloca_canais, cont, conf, &pacote_entregue, raiz, pacotes);
        cont++;
        canal = 0;
        
        //mostram os pacotes contentes em cada nó da rede
        LOG_INFO("\nPacotes por nó da rede\nTempo: %d\nPac    otes entregues: %d\nTotal de pacotes: %d\n", cont, pacote_entregue, total_pacotes);

        matching = DCFL(pacotes, adj, matconf, conf, tamNo, tamAresta, raiz);
    } 

    // passar os canais alocados para o TAD do contiki  
    // ao invez de printar  
     for(x = 0; x < 16; x++){
        for(y = 0; y < temp_canais; y++) 
            // linhas = tempo - coluna = canal 
            printf("%d  ", aloca_canais[x][y] + 1);  
             
        printf("\n"); 
    } 

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

