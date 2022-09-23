package org.contikios.cooja.positioners;
import java.util.*;  

import org.contikios.cooja.*;

//@ClassDescription("FULLTREE")  
@ClassDescription("Random positioning")
public class RandomPositioner extends Positioner { 
    
    double startX, endX, startY, endY, startZ, endZ;
    private double xPos, yPos, zPos;  
    private int TotalNumNodes; 
    private int node = 0; 
    private double IposX, IposY;  // para ser o nó pai de cada dois nós.
    //private double [][] nodes = new double[2][];
    private int levelCheck;   

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
  

    // função de retorno para o simulador
    public double[] getNextPosition() {   
        this.node++;  
        double MaxdistanceX = (endX - startX)/2; 
        double MaxdistanceY = (endX - startY)/2;    
        double ua,ux,uy;  

        if(this.node%2 != 0){
            ua = Math.random() * (Math.PI/2); 
            double udX = startX + Math.random() * MaxdistanceX;   
            double udY = startY + Math.random() * MaxdistanceX;  
            ux = this.IposX + udX * Math.cos(ua); 
            uy = this.IposY + udY * Math.sin(ua); 
            IposX = ux; 
            IposY = uy; 
        } 
        else { 
            ua = Math.random() * (Math.PI/2) * (-1);
            double udX = startX + Math.random() * MaxdistanceX;   
            double udY = startY + Math.random() * MaxdistanceX;  
            ux = this.IposX + udX * Math.cos(ua); 
            uy = this.IposY + udY * Math.sin(ua); 
            IposX = ux; 
            IposY = uy; 
        }    
            
    
        System.out.println("*****RandomPositioner.getNextPosition() - test: " + this.node);
        
        //getFinal(this.endX,0, this.node); 

        return new double[] { 
            ux, 
            uy,  
            startZ + rand.nextDouble() * (endZ - startZ)
        };
      } 

} 


