#include <stdio.h> 
#include <string.h> 
#define BUFF 18 
int main(){ 
    char **str; 
    char colect[5];   
    char aux[BUFF];  
    strcpy(aux,"fd00__201_1_1_1"); 
    int cont = 0, cont_aux = 0 ; 
    printf("%s",aux);
    
    for(int i=0; i < BUFF; i++) {   

        // encontra o ponto chave  
        if(strcmp(&aux[i],"-")){  
            // cria um indice na mesma posição do i para dar saltos no if sem ter que correr laços para que o indice acompanhe a copia dentro do while 
            cont_aux = i ;  
            while(strcmp(&colect[cont]," ") != 0) { 
                strcpy(colect[cont],aux[cont_aux]); 
                if(strcmp(&aux[i],"-")){ 
                    strcpy(&colect[cont]," "); 
                    break; 
                } 
                cont++ ; 
                cont_aux++ ; 
            }    
           }
           printf("%s",colect);
           i =  cont_aux ;
        }
    
    return 0  ; 
}