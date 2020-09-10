// tsch types 
struct MatrizAdj { 
    int **MADJ; 
    int Num_nos;  
    int num_arestas ;  
    
};  
// tsch schedule  

void init(struct MatrizAdj *Matriz); 
void matriz_adj(struct MatrizAdj *Matriz, uint16_t node_id_own, uint16_t node_id_param); 
void init(struct MatrizAdj *Matriz){ 
    
    Matriz->MADJ = calloc (MAX_NOS, sizeof(int*));
    Matriz->MADJ[0] = NULL;
    for (int i = 1; i < MAX_NOS; i++) Matriz->MADJ = calloc (i, sizeof(int));
    
     
    for(int i = 0 ; i < MAX_NOS ; i++){ 
        for(int j = 0 ; j< MAX_NOS; j++){  
            Matriz->MADJ = 0 ; 
        }
    }   
    Matriz-> Num_nos = 0 ; 
    Matriz -> num_arestas = 0 ; 
    LOG_PRINT("----- MATRIZ DE ADJACENCIA INCIADA -----\n");
}    
void matriz_adj(struct MatrizAdj *Matriz, uint16_t node_id_own, uint16_t node_id_param){ 
   // no1 emissor  
   // no2 receptor   
     
    if(node_id_own > node_id_param){ 
        if(node_id_own > Matriz->Num_nos){ 
            Matriz->Num_nos = node_id_own; 
            Matriz ->MADJ[node_id_own][node_id_param] = 1 ;   
            Matriz ->MADJ[node_id_param][node_id_own] = 1 ; 
            LOG_PRINT("----- ARESTA ADICIONADA EM [%u][%u]  -----\n",node_id_own, node_id_param); 
        }

    }   
    else { 
        if(node_id_param > Matriz->Num_nos){ 
            Matriz->Num_nos = node_id_param;    
            Matriz ->MADJ[node_id_own][node_id_param] = 1 ;   
            Matriz ->MADJ[node_id_param][node_id_own] = 1 ; 
            LOG_PRINT("----- ARESTA ADICIONADA EM [%u][%u]  -----\n",node_id_own, node_id_param);  
        }
    } 

}  
// **************** final function of scheduler algorithm  
void 
sort_links(int **coordenadas){   
  LOG_PRINT("----- ENTROU -----\n"); 
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
        

// ************************************ 
// tsch queue  
void  tsch_neighbour_maping(void); 
void  tsch_neighbour_maping(void) 
{ 
    LOG_INFO_("\n----ENTROU----\n"); 
    int j, generate_node_id, Max; 
    generate_node_id = 1 ;  
    Max = 10 ;    
    struct tsch_neighbor *n = NULL ;   
    //(struct tsch_neighbor *)nbr_table_head(tsch_neighbors);
    linkaddr_t addr;  
    //linkaddr_t neighbor_addr ;   
    while(generate_node_id < Max){ 

      for(j = 0; j < sizeof(addr); j += 2){
        addr.u8[j + 1] = generate_node_id & 0xff;
        addr.u8[j + 0] = generate_node_id >> 8;   
      }  
      
      n = tsch_queue_get_nbr(&addr);
      
      if(n!= NULL){  
        if(tsch_get_lock()) {     
          LOG_INFO_LLADDR(&addr);
          LOG_INFO_(" -> "); 
          LOG_INFO_LLADDR(&n->addr);  
        }
      }   

      if(generate_node_id == 20) break;  
      else generate_node_id++ ;  

    }
    // obtem o tsch neighbor  
    
    // copia o endereco obtido para o endereco generico do neighbor  
    //linkaddr_copy(&neighbor_addr,);   

    // take the first neighbor  
       
    
   }
 