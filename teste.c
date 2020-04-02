#include "teste.h"   

void map_topology(char *addr) 
{ 
    int tam_addr , tam_to ;    
    char no_pai[20] , no_filho[20] ;  
    int aux = 0 ;  
    int k = 0 ;  
    char addr_aux[] = {"\0"};   
    addr_aux[0] = &addr ;  
    char to[] = "to";    
    tam_addr  = strlen(addr_aux); 
    tam_to = strlen(to);   
    
    FILE *arq_dot; 
    if((arq_dot = fopen("top", "wt")) == NULL){ 
        printf("O arquivo nao foi aberto\n");
    }   
    fprintf(arq_dot, "digraph g{\n"); 
    // escrever as coordenadas 
    //    tem de estar num while   
    while(strcmp(no_filho," ") != NULL)
    {   // varia os nos pais e filhos 
        for(int i = 0 ; i < tam_addr ; i++){  
        for(int j = 0 ; j<tam_to; j++){ 
            if(addr_aux[i] = to[j] ){  
                //encontra a particula to  
                // fragmenta antes do to e dps do to   
                //percorre o to para antes e atribui a string para o no pai  
                for(k = i ; k <= i; k--){  
                    strcpy(no_pai[aux],addr_aux[k]); 
                    aux ++ ;  
                    }  
                    k = 0 ; 
                    aux = i + 2 ; // passou o to     
                    //percorre dps do to e atribui a string para o no filho  
                 while(addr_aux[aux] != "\n"){ 
                     strcpy(no_filho[k],addr_aux[k]); 
                     aux ++ ;  
                 } 
                 fprintf(arq_dot, "    %s -- %s\n",no_pai,no_filho); 
                 // so vai ser o pai se cont == 0  
            }

        } 
    }  
    }    
    // escrever as coordenadas  
    fprintf(arq_dot,"}"); 
    fclose(arq_dot);   

 }  
