/*
 * Copyright (c) 2012-2014, Thingsquare, http://www.thingsquare.com/.
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
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
* \addtogroup rpl-lite
* @{
*
* \file
*         DAG root utility functions for RPL.
*/

#include "contiki.h"
#include "contiki-net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net/routing/rpl-lite/rpl.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/uip-sr.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "RPL"
#define LOG_LEVEL LOG_LEVEL_RPL
#define endereco "/home/user/contiki-ng/os/arvre.dot"

/*---------------------------------------------------------------------------*/
void escreve_dot(void){  
  LOG_INFO(".DOT EM PROCESSAMENTO\n");

  if(rpl_dag_root_is_root()) {
    if(uip_sr_num_nodes() > 0) {
      FILE *fl = fopen(endereco, "w");
      fprintf(fl, "digraph g{\n");
      fclose(fl);
      uip_sr_node_t *link;
      /* Our routing links */
      
      link = uip_sr_node_head();
      while(link != NULL) {
        char buf[100];
        uip_sr_link_snprint(buf, sizeof(buf), link);
        
	      escreve_arq(buf);
        link = uip_sr_node_next(link);
      }
      
      fl = fopen(endereco, "a");
      fprintf(fl, "}");
      fclose(fl);
    }
  }
} 
void
rpl_dag_root_print_links(const char *str)
{
  if(rpl_dag_root_is_root()) {
    if(uip_sr_num_nodes() > 0) {
      FILE *fl = fopen(endereco, "w");
      fprintf(fl, "digraph g{\n");
      fclose(fl);
      uip_sr_node_t *link;
      /* Our routing links */
      LOG_INFO("links: %u routing links in total (%s)\n", uip_sr_num_nodes(), str)
      ;
      link = uip_sr_node_head();
      while(link != NULL) {
        char buf[100];
        uip_sr_link_snprint(buf, sizeof(buf), link);
        LOG_INFO("links: %s\n", buf);
	      escreve_arq(buf);
        link = uip_sr_node_next(link);
      }
      LOG_INFO("links: end of list\n");
      fl = fopen(endereco, "a");
      fprintf(fl, "}");
      fclose(fl);
    } else {
      LOG_INFO("No routing links\n");
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
set_global_address(uip_ipaddr_t *prefix, uip_ipaddr_t *iid)
{
  static uip_ipaddr_t root_ipaddr;
  const uip_ipaddr_t *default_prefix;
  int i;
  uint8_t state;

  default_prefix = uip_ds6_default_prefix();

  /* Assign a unique local address (RFC4193,
     http://tools.ietf.org/html/rfc4193). */
  if(prefix == NULL) {
    uip_ip6addr_copy(&root_ipaddr, default_prefix);
  } else {
    memcpy(&root_ipaddr, prefix, 8);
  }
  if(iid == NULL) {
    uip_ds6_set_addr_iid(&root_ipaddr, &uip_lladdr);
  } else {
    memcpy(((uint8_t*)&root_ipaddr) + 8, ((uint8_t*)iid) + 8, 8);
  }

  uip_ds6_addr_add(&root_ipaddr, 0, ADDR_AUTOCONF);

  LOG_INFO("IPv6 addresses:\n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      LOG_INFO("-- ");
      LOG_INFO_6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      LOG_INFO_("\n");
    }
  }
}
/*---------------------------------------------------------------------------*/
void
rpl_dag_root_set_prefix(uip_ipaddr_t *prefix, uip_ipaddr_t *iid)
{
  static uint8_t initialized = 0;

  if(!initialized) {
    set_global_address(prefix, iid);
    initialized = 1;
  }
}
/*---------------------------------------------------------------------------*/
int
rpl_dag_root_start(void)
{
  struct uip_ds6_addr *root_if;
  int i;
  uint8_t state;
  uip_ipaddr_t *ipaddr = NULL;

  rpl_dag_root_set_prefix(NULL, NULL);

  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       state == ADDR_PREFERRED &&
       !uip_is_addr_linklocal(&uip_ds6_if.addr_list[i].ipaddr)) {
      ipaddr = &uip_ds6_if.addr_list[i].ipaddr;
    }
  }

  root_if = uip_ds6_addr_lookup(ipaddr);
  if(ipaddr != NULL || root_if != NULL) {

    rpl_dag_init_root(RPL_DEFAULT_INSTANCE, ipaddr,
      (uip_ipaddr_t *)rpl_get_global_address(), 64, UIP_ND6_RA_FLAG_AUTONOMOUS);
    rpl_dag_update_state();

    LOG_INFO("created a new RPL DAG\n");
    return 0;
  } else {
    LOG_ERR("failed to create a new RPL DAG\n");
    return -1;
  }
}
/*---------------------------------------------------------------------------*/
int
rpl_dag_root_is_root(void)
{
  return curr_instance.used && curr_instance.dag.rank == ROOT_RANK;
}
/*---------------------------------------------------------------------------*/
void escreve_arq(const char *buf){
  FILE *fl;
  char copia[100];
  int x;
  strcpy(copia, buf);
  for(x = 0; x < 100; x++){
    if(copia[x] == 't'){
      copia[x] = '-';
      copia[++x]= '>';
    }
    else if(copia[x] == '('){
      copia[x] = '\0';
      break;
    }
    else if(copia[x] == ':')
      copia[x] = '_';
  }
  fl = fopen(endereco, "a");
  fprintf(fl, " %s\n", copia);
  fclose(fl);
}
/** @} */