#include "net/mac/tsch/tsch.h" 
#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "lib/random.h"
#include "sys/node-id.h"
#include "sys/log.h"

#include <stdio.h>
#include <stdlib.h>
#include "rgraph.h"
#include "conf.h"
#include "criaDOT.h"
#define temp_canais 23
#define peso 1 

void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes);
int *alocaPacotes(int num_no); 
char *colect_addres(char *ex); 
 static void initialize_tsch_schedule_root();