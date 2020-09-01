#define MAX_NOS 100 
typedef struct { 
    int **MADJ; 
    int Num_nos ;  
    int num_arestas ;  

}MatrizAdj ;
void init(MatrizAdj *Matriz){ 
     
    Matriz->MADJ = (int**)malloc(MAX_NOS  * sizeof(int*)); 
    for(int j = 0 ; j< MAX_NOS; j++){ 
        Matriz->MADJ[j] = (int*)malloc(MAX_NOS *sizeof(int)); 
    }
     
    for(int i = 0 ; i < MAX_NOS ; i++){ 
        for(int j = 0 ; j< MAX_NOS; j++){ 
            Matriz->MADJ = 0 ; 
        }
    }  
    Matriz-> Num_nos = 0 ; 
    Matriz -> num_arestas = 0 ; 
    
}   
void matriz_adj(MatrizAdj *Matriz, uint16_t node_id_own, const linkaddr_t *address_node_param){ 
   // no1 emissor  
   // no2 receptor   
   uint16_t node_id_param = 0; 
   node_id_param = address_node_param.u8[LINKADDR_SIZE - 1]
            + (address_node_param.u8[LINKADDR_SIZE - 2] << 8);
    
    int qnt_no_dest , qnt_no_emis ; 
    if(node_id_own > node_id_param){ 
        if(node_id_own > Matriz->Num_nos){ 
            Matriz->Num_nos = node_id_own;
        } 
    }   
    else { 
        if(node_id_param > Matriz->Num_nos){ 
            Matriz->Num_nos = node_id_param; 
        }
    } 
    for(int i = 0 ; i < Matriz->Num_nos;i++){ 
        for(int j= 0 ; j< Matriz-> Num_nos;j++){ 
            if(i == node_id_onw){ 
                if(j == node_id_param){ 
                    Matriz->MADJ[i][j] = 1 ;  
                }
            }
        }
    }

}    
// dentro da tsch_queue 
without_name(int node_id){ 
    LOG_INFO_("\n----ENTROU----\n"); 
    int j; 
    struct tsch_neighbor *n = NULL;
    linkaddr_t addr;  
    //linkaddr_t neighbor_addr ;   
    
    for(j = 0; j < sizeof(addr); j += 2) {
      addr.u8[j + 1] = node_id & 0xff;
      addr.u8[j + 0] = node_id >> 8; 
    }  
    
    // obtem o tsch neighbor 
    n = tsch_queue_get_nbr(&addr);  
    // copia o endereco obtido para o endereco generico do neighbor  
    //linkaddr_copy(&neighbor_addr);   
    LOG_INFO_LLADDR(tsch_queue_get_nbr_address(n));
    LOG_INFO_(" -> ");
    LOG_INFO_LLADDR(&neighbor_addr);
}