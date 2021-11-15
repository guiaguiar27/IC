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
    private int index;
    
    List<Double> nodesX = new ArrayList<>(); 
    List<Double> nodesY = new ArrayList<>();
    
    private int levelCheck;   
    private double level;  

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
    this.level = getLevel(totalNumberOfMotes); 

    Generate_full_binary_tree(this.level, this.startX, this.startY); 
    for(int i = 0 ; i < this.nodesX.size(); i++){ 
        System.out.print(this.nodesX.get(i)+  " "); 
        System.out.print(this.nodesY.get(i)); 
        System.out.println();
    } 
    System.out.println("Tamanho do vetor: " + this.nodesX.size()); 
    System.out.println("Quantidade de nos: " + this.TotalNumNodes); 
    
    


    } 
    public double getLevel(int nodes){ 
    int aux_node = nodes +1; 
    double level = Math.log(aux_node) / Math.log(2);  
    System.out.println("Levels: " + level);
    return level; 

    } 


    public void Generate_full_binary_tree(double levels, double nodeX, double nodeY){   

        double lv = levels;  
        // half  
        // double MaxdistanceX = (this.endX - this.startX)/2; 
        // double MaxdistanceY = (this.endY - this.startY)/2; 
        
        double MaxdistanceX = (this.endX - this.startX); 
        double MaxdistanceY = (this.endY - this.startY);    
        double ua,ud,ux,uy;  

        // up child 
        ua = Math.random() * (Math.PI/2);  
        double udX =  Math.random() * MaxdistanceX;   
        double udY =  Math.random() * MaxdistanceX;  
        ux = nodeX + udX * Math.cos(ua); 
        uy = nodeY + udY * Math.sin(ua);  
        this.nodesX.add(ux);
        this.nodesY.add(uy);
        
        if (lv <=  1){  
            lv --; 
            Generate_full_binary_tree(lv, ux,uy);
        }
        // down child   
        double da,dd,dx,dy; 
        da = Math.random() * (Math.PI/2) * (-1);  
        double ddX =  Math.random() * MaxdistanceX;   
        double ddY =  Math.random() * MaxdistanceX;  
        dx = nodeX + ddX * Math.cos(ua); 
        dy = nodeY + ddY * Math.sin(ua);  
        this.nodesX.add(dx);
        this.nodesY.add(dy); 
        if (lv <= 1){  
            lv --; 
            Generate_full_binary_tree(lv, dx,dy);
        }

         
  }

    // função de retorno para o simulador
    public double[] getNextPosition() {   
        
        int index = this.node;
        this.node++;    
        System.out.println("index: " + index);
    
        System.out.println("*****RandomPositioner.getNextPosition() - test: " + this.node);
        
        //getFinal(this.endX,0, this.node); 
        
        return new double[] { 
            this.nodesX.get(index), 
            this.nodesY.get(index),
            startZ + rand.nextDouble() * (endZ - startZ)
        };
      } 

} 



