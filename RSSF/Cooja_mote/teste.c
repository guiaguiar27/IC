#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>  
#define buffer 100
typedef struct geo{ 
    int x; 
    int y;  
}geo;   

// typedef strcut geo_fig{ 
    
//     int quantidade_lados; 
//     int perimetro;  
//     int area;   

// }geo_fig;   

int troca(int a, int b){ 
    // locais  
    int aux = 10;
    a = 10 - b; 
    //a = 0    

    printf("val_local = %d\n",a);
    return a;
}  
int dist(geo p1, geo p2){ 
    int distX, distY; 
    
    distX = p1.x - p2.x; 
    distY = p1.y - p2.y; 
    printf("dist x : %d - dist y: %d\n",distX,distY); 
    return 0; 

} 

int main(){     

    char *s = (char*)malloc(buffer*sizeof(char));
    fgets(s,buffer, stdin);
    printf("%s",s);
   return 0; 

}