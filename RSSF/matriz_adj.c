#define MAX_NOS 100 
typedef struct { 
    int **MADJ; 
    int Num_nos ;  
    int num_arestas ;  

}MADJ; 
void init(MADJ *Matriz){ 
     
    Matriz->MADJ = (int**)malloc(MAX_NOS  * sizeof(int*)); 
    for(int i = 0 ; i < MAX_NOS ; i++){ 
        for(int j = 0 ; j< MAX_NOS; j++){ 
            Matriz->MADJ = (int*)malloc(MAX_NOS *sizeof(int)); 
        }
    } 
    for(int i = 0 ; i < MAX_NOS ; i++){ 
        for(int j = 0 ; j< MAX_NOS; j++){ 
            Matriz->MADJ = 0 ; 
        }
    }  
    Matriz-> Num_nos = 0 ; 
    Matriz -> num_arestas = 0 ; 
    
}  
void matriz_adj(MADJ *Matriz, int no1, int no2){ 
   // no1 emissor  
   // no2 receptor 
    int qnt_no_dest , qnt_no_emis ; 
    if(no1 > no2){ 
        if(no1> Matriz->Num_nos){ 
            Matriz->Num_nos = no1;
        } 
    }   
    else { 
        if(no2 > Matriz->Num_nos){ 
            Matriz->Num_nos = no2; 
        }
    } 
    for(int i = 0 ; i < Matriz->Num_nos;i++){ 
        for(int j= 0 ; j< Matriz-> Num_nos;j++){ 
            if(i == no1){ 
                if(j == no2){ 
                    Matriz->MADJ[i][j] = 1 ;  
                }
            }
        }
    }

} 
void without_name(int node_id){ 
    int j; 
    linkaddr_t addr;  
    linkaddr_t neighbor_addr ;   
    
    for(j = 0; j < sizeof(addr); j += 2) {
      addr.u8[j + 1] = node_id & 0xff;
      addr.u8[j + 0] = node_id >> 8; 
    }  
    struct tsch_neighbor *n = NULL;
    // obtem o 
    n = tsch_queue_get_nbr(addr);  
    // copia o endereco obtido para o endereco generico do neighbor  
    linkaddr_cmp(neighbor_addr, tsch_queue_get_nbr_address(n);  
    LOG_INFO_LLADDR(addr);
    LOG_INFO_(" -> ");
    LOG_INFO_LLADDR(neighbor_addr);


}