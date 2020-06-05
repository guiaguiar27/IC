//	     ||
//linha 11   \/
char *colect_addres(char *ex);
//ao invés de receber uma matriz de caracteres (um vetor de strings) coloquei para receber só a string desejada.

//	     ||
//linha 205  \/
char *colect_addres(char *ex){      
    printf("Entrou");  
    int tam1, i, idx = 0;   
    tam1 = strlen(ex);  
    char *colect = (char*) malloc(100 * sizeof(char)); 
    colect[0] = '\0';
    for(i = 0 ; i < tam1 ; i++){   
        printf("%s\n",colect );
        if(ex[i] == '_'){ 
            i++; 
            while(ex[i] != '_' && ex[i] != '\0'){         
                colect[idx] = ex[i];
                idx++;
                colect[idx] = '\0';
                i++; 
                printf("%s\n",colect );  
                //if(colect == "_") break ; 
            } 
            i--;
        }           
        if(ex[i] == '\0') break ;  
    }
    printf("%s\n",colect);
    return colect;  
} 
//função modificada
