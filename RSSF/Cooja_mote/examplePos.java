//import org.contikios.cooja.*;
import java.util.Random; 
import java.util.*;  

//@ClassDescription("FULLTREE") 
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
    nextPos(startX,endX,startY,endY,4,ipos);
    }
  

    public void nextPos(double minRangeX, double maxRangeX,  double minRangeY, double maxRangeY, int NiveisRestantes, double[] BasePos){ 
        double ua,udx,udy, dx,dy;  
        double maxDistanceX = maxRangeX - minRangeX; 
        double maxDistanceY = maxRangeY - minRangeY; 

        // coordenadas geograficas
        ua = rand.nextInt() * (pi/2) * (-1); 
        udx = minRangeX + rand.nextInt() * maxDistanceX;  
        udy = minRangeY + rand.nextInt() * maxDistanceY;  
        
        dx = BasePos[0] + udx * Math.cos(ua); 
        dy = BasePos[1] + udy * Math.sin(ua);  
        xPos = dx;  
        yPos = dy;  
        double aux[] = new double[2]; 
        aux[0] = dx; 
        aux[1] = dy;



        if (NiveisRestantes != 1){ 
            nextPos(minRangeX, maxRangeX,minRangeY, maxRangeY, NiveisRestantes-1, aux);             
        }  


    }   
    // função de retorno para o simulador
    public double[] getNextPosition() {
        return new double[] {
            xPos,
            yPos,
            zPos};
      } 

} 



