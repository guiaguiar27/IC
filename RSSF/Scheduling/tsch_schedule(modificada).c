
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
/*---------------------------------------------------------------------------*/
/** @} */


