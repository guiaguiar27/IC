package org.contikios.cooja.positioners;
import java.util.Random;

import org.contikios.cooja.*;

//@ClassDescription("FULLTREE")  
@ClassDescription("Random positioning")
public class RandomPositioner extends Positioner { 
    
    double startX, endX, startY, endY, startZ, endZ;
    private double pi = 3.14159265358979323846;  
    private double xPos, yPos, zPos;  
    private TotalNumNodes; 


    Random rand = new Random(); 
    
    public RandomPositioner(int totalNumberOfMotes,
			  double startX, double endX,
			  double startY, double endY,
			  double startZ, double endZ) {
    this.startX = startX;
    this.endX = endX;
    this.startY = startY;
    this.endY = endY;
    this.startZ = startZ;
    this.endZ = endZ;   
    double ipos[] = new double[2]; 
    double finalPos = new double[totalNumberOfMotes];  
    ipos[0] = 0.0;  
    ipos[1] = 0.0;  
    nextPos(startX, endX, startY, endY, ipos, 0, totalNumberOfMotes, finalPos); 
    
    }
  

    public void nextPos(double minRangeX, double maxRangeX,  double minRangeY, double maxRangeY, double[] BasePos, int count, int total, double Final){ 
        double ua,udx,udy, dx,dy;  
        double maxDistanceX = maxRangeX - minRangeX; 
        double maxDistanceY = maxRangeY - minRangeY; 

        // coordenadas geograficas
        ua = rand.nextInt() * (pi/2) * (-1); 
        udx = minRangeX + rand.nextInt() * maxDistanceX;  
        udy = minRangeY + rand.nextInt() * maxDistanceY;  
        

        dx =  BasePos[0]+ udx * Math.cos(ua); 
        dy =  BasePos[1]+ udy * Math.sin(ua);  
        xPos = dx;  
        yPos = dy;  
        double aux[] = new double[2]; 
        aux[0] = dx; 
        aux[1] = dy;   

        Final[count] = aux[0]; 
        count++; 
        Final[count] = aux[1];
        if count <= this.TotalNumNodes){ 
            nextPos(minRangeX, maxRangeX, minRangeY, maxRangeY, aux, count, Final); 
        }




    }   
    // função de retorno para o simulador
    public double[] getNextPosition() { 
        double ipos[] = new double[2];  
        ipos[0] = 0.0;  
        ipos[1] = 0.0;
     
        //nextPos(startX,endX,startY,endY,ipos);
        return new double[] { 
            xPos, 
            yPos,  
            startZ + rand.nextDouble() * (endZ - startZ)
        };
      } 

} 



