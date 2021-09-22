package org.contikios.cooja.positioners;
import java.util.Random;

import org.contikios.cooja.*;

//@ClassDescription("FULLTREE")  
@ClassDescription("Random positioning")
public class RandomPositioner extends Positioner { 
    
    double startX, endX, startY, endY, startZ, endZ;
    private double pi = 3.14159265358979323846;  
    private double xPos, yPos, zPos;  
    private int TotalNumNodes; 
    private int inTest = 0;


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
    this.TotalNumNodes = totalNumberOfMotes; 
    System.out.println("Total nodes: " + this.TotalNumNodes);
     // executa uma vez para inicializar o random
    }
  

    public void nextPos(double minRangeX, double maxRangeX,  double minRangeY, double maxRangeY, double[] BasePos){ 
        System.out.println("*****RandomPositioner.nextPos()");
        double ua,udx,udy, dx,dy;  
        double maxDistanceX = maxRangeX - minRangeX; 
        double maxDistanceY = maxRangeY - minRangeY; 

        // coordenadas geograficas
        ua = rand.nextInt() * (pi/2) * (-1); 
        udx = minRangeX + rand.nextInt() * maxDistanceX;  
        udy = minRangeY + rand.nextInt() * maxDistanceY;  
        

        dx =  BasePos[0] + udx * Math.cos(ua); 
        dy =  BasePos[1] + udy * Math.sin(ua);  
        xPos = dx;  
        yPos = dy;  
        double aux[] = new double[2]; 
        aux[0] = dx; 
        aux[1] = dy;




    }   
    // função de retorno para o simulador
    public double[] getNextPosition() {   
        this.inTest++;  
        System.out.println("*****RandomPositioner.getNextPosition() - test: " + this.inTest);
        double ipos[] = new double[2];  
        ipos[0] = 0.0;  
        ipos[1] = 0.0;
     
        nextPos(startX,endX,startY,endY,ipos);
        return new double[] { 
            xPos, 
            yPos,  
            startZ + rand.nextDouble() * (endZ - startZ)
        };
      } 

} 



