package org.contikios.cooja.positioners;
import java.util.*;  

import org.contikios.cooja.*;

//@ClassDescription("FULLTREE")  
@ClassDescription("Random positioning")
public class RandomPositioner extends Positioner { 
    
    double startX, endX, startY, endY, startZ, endZ;
    private double pi = 3.14159265358979323846;  
    private double xPos, yPos, zPos;  
    private int TotalNumNodes; 
    private int inTest = 0;     
    double [][] nodes = new double[2][];  

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
    nodes[0][0] = 0.0; 
    nodes[1][0] = 0.0;

    System.out.println("Total nodes: " + this.TotalNumNodes); 
    for (int i = 0; i < nodes.length; i++)
    {
           System.out.print(nodes[ i ] [ 0 ] + ": ");
           for (int j = 1; j < nodes[ i ].length; j++)
           {
                      System.out.print(nodes[ i ][ j ] + " ");
           }
          System.out.println( );
    }
     // executa uma vez para inicializar o random
    }
  
    public void getFinal(double maxRange, double minRange, int node){  
        System.out.println("Enter in get Final");
        int pid = nodes.length; 
        pid = pid - 1; 
        double Maxdistance = maxRange - minRange;  
        if(node%2 != 0){
            double ua = rand.nextDouble() * (Math.PI/2); 
            double ud = minRange + rand.nextDouble() * Maxdistance;  
            double ux = nodes[0][node-1] + ud * Math.cos(ua); 
            double uy = nodes[1][node-1] + ud * Math.sin(ua); 
            nodes[0][node] = ux; 
            nodes[1][node] = uy;  
            xPos = ux;  
            yPos = uy;  
        
              
        }
        if(node%2 == 0){ 

            double ua = rand.nextDouble() * (Math.PI/2); 
            double ud = minRange + rand.nextDouble() * Maxdistance;  
            double ux = nodes[0][node-1] + ud * Math.cos(ua); 
            double uy = nodes[1][node-1] + ud * Math.sin(ua); 
            nodes[0][node] = ux; 
            nodes[1][node] = uy;  
            xPos = ux;  
            yPos = uy;  
        
              
        } 
       
        return;   
    }

    public void nextPos(double minRangeX, double maxRangeX,  double minRangeY, double maxRangeY, double[] BasePos, int node){ 

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
        getFinal(this.endX,0, this.inTest);
        return new double[] { 
            xPos, 
            yPos,  
            startZ + rand.nextDouble() * (endZ - startZ)
        };
      } 

} 



