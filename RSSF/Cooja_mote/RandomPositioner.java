package org.contikios.cooja.positioners;
import java.util.Random;

import org.contikios.cooja.*;

//@ClassDescription("FULLTREE")  
@ClassDescription("Random positioning")
public class RandomPositioner extends Positioner { 
    
    double startX, endX, startY, endY, startZ, endZ;
    private double pi = 3.14159265358979323846;  
    private double xPos, yPos, zPos; 


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
    ipos[0] = 0.0;  
    ipos[1] = 0.0;
    nextPos(startX,endX,startY,endY,ipos);
    }
  

    public void nextPos(double minRangeX, double maxRangeX,  double minRangeY, double maxRangeY, double[] BasePos){ 
        double ua,udx,udy, dx,dy;  
        double maxDistanceX = maxRangeX - minRangeX; 
        double maxDistanceY = maxRangeY - minRangeY; 

        // coordenadas geograficas
        ua = rand.nextInt() * (pi/2) * (-1); 
        udx = minRangeX + rand.nextInt() * maxDistanceX;  
        udy = minRangeY + rand.nextInt() * maxDistanceY;  
        

        dx =  this.startX +  rand.nextDouble() + udx * Math.cos(ua); 
        dy =  this.startY + rand.nextDouble() + udy * Math.sin(ua);  
        xPos = dx;  
        yPos = dy;  
        double aux[] = new double[2]; 
        aux[0] = dx; 
        aux[1] = dy;




    }   
    // função de retorno para o simulador
    public double[] getNextPosition() {
        return new double[] {startX + rand.nextDouble()*(endX - startX),
            startY + rand.nextDouble()*(endY - startY),
            startZ + rand.nextDouble()*(endZ - startZ)};
      } 

} 



