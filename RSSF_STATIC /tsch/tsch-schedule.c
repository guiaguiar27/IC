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
 *         Atis Elsts <atis.elsts@edi.lv>
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
#include <stdio.h> 
#include <stdlib.h>
#include "conf.c"  
#define temp_canais 16
#define peso 1 
#define MAX_NOS 5
#define no_raiz 1  
#define Timeslot 16 
#define MAX_MESH_SIZE_NODE 25  
#define endereco "/home/user/contiki-ng/os/arvore.txt" 

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "TSCH Sched"
#define LOG_LEVEL LOG_LEVEL_MAC

/* Pre-allocated space for links */
MEMB(link_memb, struct tsch_link, TSCH_SCHEDULE_MAX_LINKS); 
MEMB(generic_2d_array_memb, struct generic_2d_array_element, MAX_MESH_SIZE_NODE); 
MEMB(generic_array_memb, struct generic_array_element, MAX_NOS); 

/* Pre-allocated space for slotframes */
MEMB(slotframe_memb, struct tsch_slotframe, TSCH_SCHEDULE_MAX_SLOTFRAMES); 
MEMB(adj_memb, struct 2d_Array , 1);   
MEMB(conf_memb, struct 2d_Array , 1);   
MEMB(pacotes_memb, struct 1d_Array , 1); 

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
                       uint16_t timeslot, uint16_t channel_offset, uint8_t do_remove)
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

    if(do_remove) {
      /* Start with removing the link currently installed at this timeslot (needed
       * to keep neighbor state in sync with link options etc.) */
      tsch_schedule_remove_link_by_timeslot(slotframe, timeslot, channel_offset);
    }
    if(!tsch_get_lock()) {
      LOG_ERR("! add_link memb_alloc couldn't take lock\n");
    } else {
      l = memb_alloc(&link_memb);
      if(l == NULL) {
        LOG_ERR("! add_link memb_alloc failed\n");
        tsch_release_lock();
      } else {
        struct tsch_neighbor *n;
        /* Add the link to the slotframe */ 
        //static int count = 0 ;
        list_add(slotframe->links_list, l);
        /* Initialize link */
        //l->handle = count++ ; 
        l->handle = count_lines();
        LOG_PRINT("Handle : %u\n ", l->handle);
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
              node = linkaddr_node_addr.u8[LINKADDR_SIZE - 1]
                + (linkaddr_node_addr.u8[LINKADDR_SIZE - 2] << 8);  
              node_neighbor =  l->addr.u8[LINKADDR_SIZE - 1]
                + (l->addr.u8[LINKADDR_SIZE - 2] << 8);  
              
              tsch_write_in_file(node, node_neighbor);   
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
        struct tsch_neighbor *n = tsch_queue_get_nbr(&addr);
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
tsch_schedule_remove_link_by_timeslot(struct tsch_slotframe *slotframe,
                                      uint16_t timeslot, uint16_t channel_offset)
{
  int ret = 0;
  if(!tsch_is_locked()) {
    if(slotframe != NULL) {
      struct tsch_link *l = list_head(slotframe->links_list);
      /* Loop over all items and remove all matching links */
      while(l != NULL) {
        struct tsch_link *next = list_item_next(l);
        if(l->timeslot == timeslot && l->channel_offset == channel_offset) {
          if(tsch_schedule_remove_link(slotframe, l)) {
            ret = 1;
          }
        }
        l = next;
      }
    }
  }
  return ret;
}
/*---------------------------------------------------------------------------*/
/* Looks within a slotframe for a link with a given timeslot */
struct tsch_link *
tsch_schedule_get_link_by_timeslot(struct tsch_slotframe *slotframe,
                                   uint16_t timeslot, uint16_t channel_offset)
{
  if(!tsch_is_locked()) {
    if(slotframe != NULL) {
      struct tsch_link *l = list_head(slotframe->links_list);
      /* Loop over all items. Assume there is max one link per timeslot and channel_offset */
      while(l != NULL) {
        if(l->timeslot == timeslot && l->channel_offset == channel_offset) {
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
static struct tsch_link *
default_tsch_link_comparator(struct tsch_link *a, struct tsch_link *b)
{
  if(!(a->link_options & LINK_OPTION_TX)) {
    /* None of the links are Tx: simply return the first link */
    return a;
  }

  /* Two Tx links at the same slotframe; return the one with most packets to send */
  if(!linkaddr_cmp(&a->addr, &b->addr)) {
    struct tsch_neighbor *an = tsch_queue_get_nbr(&a->addr);
    struct tsch_neighbor *bn = tsch_queue_get_nbr(&b->addr);
    int a_packet_count = an ? ringbufindex_elements(&an->tx_ringbuf) : 0;
    int b_packet_count = bn ? ringbufindex_elements(&bn->tx_ringbuf) : 0;
    /* Compare the number of packets in the queue */
    return a_packet_count >= b_packet_count ? a : b;
  }

  /* Same neighbor address; simply return the first link */
  return a;
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
            if(l->slotframe_handle != curr_best->slotframe_handle) {
              if(l->slotframe_handle < curr_best->slotframe_handle) {
                new_best = l;
              }
            } else {
              /* compare the link against the current best link and return the newly selected one */
              new_best = TSCH_LINK_COMPARATOR(curr_best, l);
            }
          } else {
            /* Select the link that has the Tx option */
            if(l->link_options & LINK_OPTION_TX) {
              new_best = l;
            }
          }

          /* Maintain backup_link */
          /* Check if 'l' best can be used as backup */
          if(new_best != l && (l->link_options & LINK_OPTION_RX)) { /* Does 'l' have Rx flag? */
            if(curr_backup == NULL || l->slotframe_handle < curr_backup->slotframe_handle) {
              curr_backup = l;
            }
          }
          /* Check if curr_best can be used as backup */
          if(new_best != curr_best && (curr_best->link_options & LINK_OPTION_RX)) { /* Does curr_best have Rx flag? */
            if(curr_backup == NULL || curr_best->slotframe_handle < curr_backup->slotframe_handle) {
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
    memb_init(&generic_2d_array_memb);  
    memb_init(&adj_memb); 
    memb_init(&conf_memb);
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
      0, 0, 1);
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

/*---------------------------------------------------------------------------*/


int **mapGraphConf(struct 2d_Array *mat, int tam_no, int tam_aresta){
    /*
    * alocado: matriz de duas posições que informa os nós de cada aresta da matriz de conflito
    * x, y: índices da matriz
    * noConf: representa o nó DO grafo de conflito
    */ 
   struct 2d_Array *alocado = memb_allloc(&conf_memb);  
    int **alocado, x = 0 , y = 0 ;
    int noConf = 0;

    //Aloca a matriz   

    for(int x = 0 ; i < tam_aresta ;i++){ 
      for(int j = 0 ; j < 2; j++){  
        struct generic_2d_array_element *el = NULL;
        el = memb_alloc(&conf_memb); 
        list_add(alocado->Internal_list, el); 
        el->colunm = j;  
        el->line = i;  
        el->value = 0 ;     
        memb_free(&conf_memb, el);
         
      }
    }      
    
    //"Captura" as arestas e armazena
    for(x = 1; x < tam_no; x++)
        for(y = 1; y < tam_no; y++) 
             for(struct generic_2d_array_element *el_aux = list_head(adj->Internal_list); el_aux != NULL; el_aux = list_item_next(el_aux)){ 
               if(el_aux->line == x && el_aux->colunm == y){ 
                 if(el_aux->value != 0 ){
                   for(struct generic_2d_array_element *el_alocado = list_head(alocado->Internal_list); el_aux != NULL; el_aux = list_item_next(el_aux))
                      if(el_alocado->line == noConf && el_alocado->colunm == 0 ){ 
                          el_alocado->value = x ;
                      } 
                      else if(el_alocado->line == noConf && el_alocado->colunm == 1 ){ 
                          el_alocado->value = y; 
                      }
                      noConf++; 
                }
               }
               
             }
            

    return alocado;
}
/*---------------------------------------------------------------------------*/

void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes){
    int  i;
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

/*------------------------------------------------------------------------------------------------------------*/
struct 1d_Array *alocaPacotes(int num_no, struct 2d_Array *adj){
  int qtd_pacotes = 0; 
    struct 1d_Array *pre_pacotes = memb_alloc(&pacotes_memb);  
    LIST_STRUCT_INIT(pre_pacotes,list_packages_node); 
    

    //Percorre o vetor de pacotes
    for(struct generic_array_element *el_aux = list_head(adj->Internal_list); el_aux != NULL; el_aux = list_item_next(el_aux)){  
        if(el_aux->value == 1){
                qtd_pacotes = peso; 
            } 
        if(qtd_pacotes){ 
          struct generic_array_element *el = NULL;
          el = memb_alloc(&generic_array_memb);  
          list_add(pre_pacotes->list_packages_node, el); 
          el->line = el_aux->line;  
          el->value = qtd_pacotes;
          //vetor[x] = qtd_pacotes;
          }
        else{ 
          struct generic_array_element *el = NULL;
          el = memb_alloc(&generic_array_memb);  
          list_add(pre_pacotes->list_packages_node, el); 
          el->line = el_aux->line;  
          el->value = 0 ;
        }

        qtd_pacotes = 0;
      }  
    
    return pre_pacotes; 
    }  



/*-----------------------------------------------------------------------------------------------------*/
 // Return the number of nodes defined for this network     
int tsch_num_nos(){ 
  int i = MAX_NOS; 
  return i; 
}  
/*---------------------------------------------------------------------------*/
void tsch_write_in_file(int n_origin, int n_destin){ 
  FILE *file; 
  file = fopen(endereco, "a");
  if(file == NULL){
        printf("The file was not opened\n");
        return ; 
  } 
  fprintf(file, "%d %d\n",n_origin,n_destin);
  fclose(file);
} 
/*---------------------------------------------------------------------------*/
int count_lines() 
{ 
    FILE *fp; 
    int count = 1;    
    char c;  
    fp = fopen(endereco, "r"); 
    if (fp == NULL) return 0; 
    for (c = getc(fp); c != EOF; c = getc(fp)) 
        if (c == '\n') 
            count = count + 1; 
    fclose(fp); 
    return count+1; 
}      
/*---------------------------------------------------------------------------*/
 
void SCHEDULE_static(){  
  FILE *fl;      
  //int i = 0 ;   
  //int node_origin, node_destin;  
  struct 2d_Array *adj = memb_alloc(&adj_memb);   
  struct 2d_Array *conf = NULL; 
  //int  tamAresta = MAX_NOS;     
  //int numNo = MAX_NOS - 1;   
//  struct 1D_Array *pacotes = NULL;  

  if(tsch_get_lock()){    
      fl = fopen(endereco, "r"); 
      if(fl == NULL){
          printf("The file was not opened\n");
          return ; 
      }    
    LIST_STRUCT_INIT(adj, Internal_list); 
    for(int i = 0 ; i < MAX_NOS ;i++){ 
      for(int j = 0 ; j < MAX_NOS; j++){  
        struct generic_2d_array_element *el = NULL;
        el = memb_alloc(&generic_2d_array_memb); 
        list_add(adj->Internal_list, el); 
        el->colunm = j;  
        el->line = i;  
        el->value = 0 ;     
        printf("el->line: %u el->colunm: %u el->value: %u\n", el->colunm, el->line, el->value); 
        memb_free(&generic_2d_array_memb, el);
         
      }
    } 

      // //read the topology 
      while(!feof(fl)){      
              fscanf(fl,"%d %d",&node_origin, &node_destin);       
              if(node_origin < MAX_NOS && node_destin < MAX_NOS){
          
                      for(struct generic_2d_array_element *el_aux = list_head(adj->Internal_list); el_aux != NULL; el_aux = list_item_next(el_aux)) {
                      if (el_aux->line == node_origin && el_aux->colunm == node_destin ){
                          if(el_aux->value == 0 && node_origin != no_raiz) 
                            el_aux->value = 1 ; 
                            printf("value: %u",el_aux->value);
                            printf("%d-> %d\n",node_origin, node_destin); 
                            i++; 
                          }   
                         
                      }
                  } 
                if(feof(fl)) break ;
              }     
      tamAresta = i ;  
      printf("TAmAresta: %d\n", tamAresta); 
      for(struct generic_2d_array_element *el_aux = list_head(adj->Internal_list); el_aux != NULL; el_aux = list_item_next(el_aux)) {
        printf("el->line: %u el->colunm: %u el->value: %u\n", el_aux->line , el_aux->colunm, el_aux->value);
      }  
      pacotes = alocaPacotes(numNo,adj);  
      for(struct generic_2d_array_element *el_aux = list_head(pacotes->list_packages_node); el_aux != NULL; el_aux = list_item_next(el_aux)) {
        printf("el->line: %u el->value: %u\n",el_aux->line, el_aux->value);
      } 
      conf = mapGraphConf(adj, tamNo, tamAresta); 




      }  


 


} 

/*---------------------------------------------------------------------------*/
  
int sort_node_to_create_link(int n){ 
 
  unsigned short  random_node;    
  int aux_n = n - 1 ;  
  random_node = random_rand() % aux_n;
  int final_sorted_node = (int) random_node;
  
  
  return final_sorted_node; 
 
 }



/** @} */
