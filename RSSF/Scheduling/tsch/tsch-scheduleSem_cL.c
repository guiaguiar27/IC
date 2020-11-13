/*
 * Copyright (c) 2014, SICS Swedish ICT.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         IEEE 802.15.4 TSCH MAC schedule manager.
 * \author
 *         Simon Duquennoy <simonduq@sics.se>
 *         Beshr Al Nahas <beshr@sics.se>
 */

/**
 * \addtogroup tsch
 * @{
*/

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
#include <stdlib.h>
#include "conf.h" 
#define temp_canais 23
#define peso 1 
#define MAX_NOS 11
#define no_raiz 1 
#define endereco "/home/user/contiki-ng/os/arvore.txt" 

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "TSCH Sched"
#define LOG_LEVEL LOG_LEVEL_MAC

/* Pre-allocated space for links */
MEMB(link_memb, struct tsch_link, TSCH_SCHEDULE_MAX_LINKS);
/* Pre-allocated space for slotframes */
MEMB(slotframe_memb, struct tsch_slotframe, TSCH_SCHEDULE_MAX_SLOTFRAMES);
/* List of slotframes (each slotframe holds its own list of links) */
LIST(slotframe_list);

/* Adds and returns a slotframe (NULL if failure) */
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
/*---------------------------------------------------------------------------*/
/* Removes all slotframes, resulting in an empty schedule */
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
/*---------------------------------------------------------------------------*/
/* Removes a slotframe Return 1 if success, 0 if failure */
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
/*---------------------------------------------------------------------------*/
/* Looks for a slotframe from a handle */
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
/*---------------------------------------------------------------------------*/
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
/*---------------------------------------------------------------------------*/
/* Adds a link to a slotframe, return a pointer to it (NULL if failure) */
struct tsch_link *
tsch_schedule_add_link(struct tsch_slotframe *slotframe,
                       uint8_t link_options, enum link_type link_type, const linkaddr_t *address,
                       uint16_t timeslot, uint16_t channel_offset)
{
  struct tsch_link *l = NULL; 
  uint16_t node_neighbor, node;
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
          linkaddr_copy(&n->addr_neighbor, &linkaddr_node_addr);
           
          /* We have a tx link to this neighbor, update counters */
          if(n != NULL) {
            n->tx_links_count++;
 
            if(!(l->link_options & LINK_OPTION_SHARED)) {
            // escrve topologia em um arquivo  
              n->dedicated_tx_links_count++; 
              node = linkaddr_node_addr.u8[LINKADDR_SIZE - 1]
                + (linkaddr_node_addr.u8[LINKADDR_SIZE - 2] << 8);  
              node_neighbor =  n->addr.u8[LINKADDR_SIZE - 1]
                + (n->addr.u8[LINKADDR_SIZE - 2] << 8);  
              
              tsch_queue_write_in_file(node, node_neighbor); 
            }
          }
        }
      }
    }
  }
  return l;
}
/*---------------------------------------------------------------------------*/
/* Removes a link from slotframe. Return 1 if success, 0 if failure */
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
/*---------------------------------------------------------------------------*/
/* Looks within a slotframe for a link with a given timeslot */
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
/* Returns the next active link after a given ASN, and a backup link (for the same ASN, with Rx flag) */
struct tsch_link *
tsch_schedule_get_next_active_link(struct tsch_asn_t *asn, uint16_t *time_offset,
    struct tsch_link **backup_link)
{
  uint16_t time_to_curr_best = 0;
  struct tsch_link *curr_best = NULL;
  struct tsch_link *curr_backup = NULL; /* Keep a back link in case the current link
  turns out useless when the time comes. For instance, for a Tx-only link, if there is
  no outgoing packet in queue. In that case, run the backup link instead. The backup link
  must have Rx flag set. */
  if(!tsch_is_locked()) {
    struct tsch_slotframe *sf = list_head(slotframe_list);
    /* For each slotframe, look for the earliest occurring link */
    while(sf != NULL) {
      /* Get timeslot from ASN, given the slotframe length */
      uint16_t timeslot = TSCH_ASN_MOD(*asn, sf->size);
      struct tsch_link *l = list_head(sf->links_list);
      while(l != NULL) {
        uint16_t time_to_timeslot =
          l->timeslot > timeslot ?
          l->timeslot - timeslot :
          sf->size.val + l->timeslot - timeslot;
        if(curr_best == NULL || time_to_timeslot < time_to_curr_best) {
          time_to_curr_best = time_to_timeslot;
          curr_best = l;
          curr_backup = NULL;
        } else if(time_to_timeslot == time_to_curr_best) {
          struct tsch_link *new_best = NULL;
          /* Two links are overlapping, we need to select one of them.
           * By standard: prioritize Tx links first, second by lowest handle */
          if((curr_best->link_options & LINK_OPTION_TX) == (l->link_options & LINK_OPTION_TX)) {
            /* Both or neither links have Tx, select the one with lowest handle */
            if(l->slotframe_handle < curr_best->slotframe_handle) {
              new_best = l;
            }
          } else {
            /* Select the link that has the Tx option */
            if(l->link_options & LINK_OPTION_TX) {
              new_best = l;
            }
          }

          /* Maintain backup_link */
          if(curr_backup == NULL) {
            /* Check if 'l' best can be used as backup */
            if(new_best != l && (l->link_options & LINK_OPTION_RX)) { /* Does 'l' have Rx flag? */
              curr_backup = l;
            }
            /* Check if curr_best can be used as backup */
            if(new_best != curr_best && (curr_best->link_options & LINK_OPTION_RX)) { /* Does curr_best have Rx flag? */
              curr_backup = curr_best;
            }
          }

          /* Maintain curr_best */
          if(new_best != NULL) {
            curr_best = new_best;
          }
        }

        l = list_item_next(l);
      }
      sf = list_item_next(sf);
    }
    if(time_offset != NULL) {
      *time_offset = time_to_curr_best;
    }
  }
  if(backup_link != NULL) {
    *backup_link = curr_backup;
  }
  return curr_best;
}
/*---------------------------------------------------------------------------*/
/* Module initialization, call only once at startup. Returns 1 is success, 0 if failure. */
int
tsch_schedule_init(void)
{
  if(tsch_get_lock()) {
    memb_init(&link_memb);
    memb_init(&slotframe_memb);
    list_init(slotframe_list);
    tsch_release_lock();
    return 1;
  } else {
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
/* Create a 6TiSCH minimal schedule */
void
tsch_schedule_create_minimal(void)
{
  struct tsch_slotframe *sf_min;
  /* First, empty current schedule */
  tsch_schedule_remove_all_slotframes();
  /* Build 6TiSCH minimal schedule.
   * We pick a slotframe length of TSCH_SCHEDULE_DEFAULT_LENGTH */
  sf_min = tsch_schedule_add_slotframe(0, TSCH_SCHEDULE_DEFAULT_LENGTH);
  /* Add a single Tx|Rx|Shared slot using broadcast address (i.e. usable for unicast and broadcast).
   * We set the link type to advertising, which is not compliant with 6TiSCH minimal schedule
   * but is required according to 802.15.4e if also used for EB transmission.
   * Timeslot: 0, channel offset: 0. */
  tsch_schedule_add_link(sf_min,
      (LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING),
      LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
      0, 0);
}
/*---------------------------------------------------------------------------*/
struct tsch_slotframe *
tsch_schedule_slotframe_head(void)
{
  return list_head(slotframe_list);
}
/*---------------------------------------------------------------------------*/
struct tsch_slotframe *
tsch_schedule_slotframe_next(struct tsch_slotframe *sf)
{
  return list_item_next(sf);
}
/*---------------------------------------------------------------------------*/
/* Prints out the current schedule (all slotframes and links) */
void
tsch_schedule_print(void)
{
  if(!tsch_is_locked()) {
    struct tsch_slotframe *sf = list_head(slotframe_list);

    LOG_PRINT("----- start slotframe list -----\n");

    while(sf != NULL) {
      struct tsch_link *l = list_head(sf->links_list);

      LOG_PRINT("Slotframe Handle %u, size %u\n", sf->handle, sf->size.val);

      while(l != NULL) {
        LOG_PRINT("* Link Options %02x, type %u, timeslot %u, channel offset %u, address %u\n",
               l->link_options, l->link_type, l->timeslot, l->channel_offset, l->addr.u8[7]);
        l = list_item_next(l);
      }

      sf = list_item_next(sf);
    }

    LOG_PRINT("----- end slotframe list -----\n");
  }
}
/*---------------------------------------------------------------------------------------------------*/
/** @} */ 

void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes){
    int  i;

    for(i = 0; i < 16; i++){
        if(aloca_canal[i][tempo] == -1)
            continue;
        if(pacotes[mapa_graf_conf[aloca_canal[i][tempo]][0]] > 0){
            pacotes[mapa_graf_conf[aloca_canal[i][tempo]][0]]+=peso;
            pacotes[mapa_graf_conf[aloca_canal[i][tempo]][1]]-=peso;
        }
        if(mapa_graf_conf[aloca_canal[i][tempo]][1] == raiz)
            (*pacote_entregue)++;
    }
}

/*------------------------------------------------------------------------------------------------------------*/
int *alocaPacotes(int num_no, int **adj){
 int *vetor, x, y, qtd_pacotes = 0;
    vetor = (int*) malloc(num_no * sizeof(int));
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
    
    return vetor;}  

/*----------------------------------------------------------------------------------------------------------------*/

void 
sort_links(int **coordenadas){   
  LOG_PRINT("----- ENTROU SORTI_LINKS -----\n"); 
  //escreve_dot();
    int i, j = 0 ;          
    int total_timeslot = 3, total_channel_of = 25 ; 
    /*int **coordenadas = (int**)malloc(total_channel_of *sizeof(int*)); 
    for(i = 0; i< total_channel_of; i++) {
      coordenadas[i] = (int *) malloc(total_timeslot * sizeof(int));
    }  
    */ 
     
    // linhas = channel_offset  
    // colunas = time slot
    // initialize example matrix
    /*
    coordenadas[0][0] = 3 ;
    coordenadas[0][1] = 5 ; 
    coordenadas[1][0] = 7 ; 
    coordenadas[1][1] = 1 ; 
    */ 
    // generate random integers 
    /*srand(time(NULL));
    for(i = 0 ; i<total_channel_of ; i++){ 
       for(j = 0 ; j < total_timeslot ;j++){ 
          coordenadas[i][j] = rand()%10;   
      }
    }  
    */  
    if(!tsch_is_locked()) { 
    struct tsch_slotframe *sf = list_head(slotframe_list);
    while(sf != NULL) {
      struct tsch_link *l = list_head(sf->links_list);
      /* Loop over all items. Assume there is max one link per timeslot */
      
        for(i = 0 ; i<total_channel_of ; i++){ 
          for(j = 0 ; j < total_timeslot ;j++){ 
            //coordenadas[i][j] = rand()%16  ;  
            if(coordenadas[i][j] == l->handle){   
              LOG_PRINT("---------------------------\n"); 
              LOG_PRINT("----HANDLE: %d-----\n", l-> handle); 
              LOG_PRINT("----TIMESLOT: %d-----\n", l-> timeslot); 
              LOG_PRINT("----CHANNEL: %d-----\n", l-> channel_offset);   
              l-> timeslot = i; 
              l-> channel_offset = j ;  
              LOG_PRINT("----CHANGE-----\n"); 
              LOG_PRINT("----TIMESLOT: %d-----\n", l-> timeslot); 
              LOG_PRINT("----CHANNEL: %d-----\n", l-> channel_offset); 
              LOG_PRINT("-----------------------------\n");   
              l = list_item_next(l); 
            }
      }
      }    
        
       
      
      sf = list_item_next(sf);
    }
  } 
}
        

/*-----------------------------------------------------------------------------------------------------*/
int SCHEDULE(int **adj){      
   LOG_PRINT("Entrou SCHEDULE\n");
    int tamNo; 
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
                                 //Nome dos nós no grafo da rede
                                 //Nom do arquivo contendo o grafo de conflito (não usado)
    int *pacotes;               //Pacotes por nó no grafo da rede
    int node_origin, node_destin ; 
    /*******************************************************************/ 
    // inicia arquivo 
    FILE *fl;  
    if(!tsch_get_lock()){  
      LOG_PRINT("----- TSCH LOCK -----\n");
      tamNo = MAX_NOS ;  
      tamAresta = MAX_NOS;   
      fl = fopen(endereco, "r"); 
      if(fl == NULL){
          printf("The file was not opened\n");
          return 0  ; 
      } 
      // matriz  
      adj = (int**)malloc(MAX_NOS * sizeof(int*)); 
      for(int i = 0; i< MAX_NOS; i++) {
          adj[i] = (int *)malloc(MAX_NOS * sizeof(int));
      }
      for(int i = 0 ; i < MAX_NOS ; i++){ 
          for(int j = 0 ; j< MAX_NOS; j++){  
              adj[i][j] = 0 ; 
          }
      }  

      i = 0;
      while(!feof(fl)){      
          fscanf(fl,"%d %d",&node_origin, &node_destin);   
          if(node_origin < MAX_NOS && node_destin < MAX_NOS){
              if (adj[node_origin][node_destin] == 0 && node_origin != no_raiz){
                  adj[node_origin][node_destin] = 1;
                  i++;
              } 
          } 
          if(feof(fl)) break ;
      }
      tamAresta = i;

      pacotes = alocaPacotes(tamNo, adj);
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

      //Por hora definimos ele manualmente
      raiz = no_raiz;

      //Guarda o total de pacotes a serem enviados pela
      for(z = 0; z < tamNo; z++)
          if(z != raiz)
              total_pacotes += pacotes[z];


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
    sort_links(aloca_canais);
  }
    return 0;
    }  
