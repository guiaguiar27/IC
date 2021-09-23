package org.contikios.cooja.positioners;
import java.util.*;  

import org.contikios.cooja.*;

//@ClassDescription("FULLTREE")  
@ClassDescription("Random positioning")
public class RandomPositioner extends Positioner { 
    
    double startX, endX, startY, endY, startZ, endZ;
    private double xPos, yPos, zPos;  
    private int TotalNumNodes; 
    private int inTest = 0;     
    private double [][] nodes = new double[2][];  

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
    //nodes[0][0] = 0.0; 
    //nodes[1][0] = 0.0;
    }
  
    public void getFinal(double maxRange, double minRange, int node){  
        System.out.println("Enter in get Final");
        
        double Maxdistance = maxRange - minRange;  
        
        if(node%2 != 0){
            double ua = rand.nextDouble() * (Math.PI/2); 
            double ud = minRange + rand.nextDouble() * Maxdistance;  
            double ux = this.nodes[0][node-1] + ud * Math.cos(ua); 
            double uy = this.nodes[1][node-1] + ud * Math.sin(ua); 
            this.nodes[0][node] = ux; 
            this.nodes[1][node] = uy;  
            xPos = ux;  
            yPos = uy;  
        
              
        }
        if(node%2 == 0){ 

            double da = rand.nextDouble() * (Math.PI/2) *(-1); 
            double dd = minRange + rand.nextDouble() * Maxdistance;  
            double dx = this.nodes[0][node-1] + dd * Math.cos(da); 
            double dy = this.nodes[1][node-1] + dd * Math.sin(da); 
            this.nodes[0][node] = dx; 
            this.nodes[1][node] = dy;  
            xPos = dx;  
            yPos = dy;  
        
              
        } 
          
    }

    // public void nextPos(double minRangeX, double maxRangeX,  double minRangeY, double maxRangeY, double[] BasePos, int node){ 

    //     double ua,udx,udy, dx,dy;  
    //     double maxDistanceX = maxRangeX - minRangeX; 
    //     double maxDistanceY = maxRangeY - minRangeY; 

    //     // coordenadas geograficas
    //     ua = rand.nextInt() * (Math.PI/2) * (-1); 
    //     udx = minRangeX + rand.nextInt() * maxDistanceX;  
    //     udy = minRangeY + rand.nextInt() * maxDistanceY;  
        

    //     dx =  BasePos[0] + udx * Math.cos(ua); 
    //     dy =  BasePos[1] + udy * Math.sin(ua);  
    //     xPos = dx;  
    //     yPos = dy;  
    //     double aux[] = new double[2]; 
    //     aux[0] = dx; 
    //     aux[1] = dy;




    // }   
    // função de retorno para o simulador
    public double[] getNextPosition() {   
        this.inTest++;  
        System.out.println("*****RandomPositioner.getNextPosition() - test: " + this.inTest);
        double ipos[] = new double[2];  
        ipos[0] = 0.0;  
        ipos[1] = 0.0;
        getFinal(this.endX,0, this.inTest); 

        return new double[] { 
            xPos, 
            yPos,  
            startZ + rand.nextDouble() * (endZ - startZ)
        };
      } 

} 



