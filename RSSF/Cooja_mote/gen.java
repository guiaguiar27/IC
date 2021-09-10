//import org.contikios.cooja.*;
import java.util.Random; 
import java.util.*;  

//@ClassDescription("FULLTREE") 
public class gen { 
    private double startX, startY, startZ;
    private int addedInX, addedInY, addedInZ;
    private int numberInX, numberInY, numberInZ;
    private double xInterval, yInterval, zInterval;
    private double pi = 3.14159265358979323846;  
    private double[] BasePos = new double[2];   
    private ArrayList<float[]> Nodes = new ArrayList<float[]>(); // lista de nós. Cada elemento é uma lista de dois nós  



    Random rand = new Random(); 
    public void generate_full_binary_tree_network(int minRange, int maxRange){ 
        int NiveisRestantes = 10;  
        nextPos(minRange=0, maxRange, NiveisRestantes);
        
    } 
    
    public void nextPos(int minRange, int maxRange, int NiveisRestantes){ 
        double ua,ud, dx,dy;  
        int maxDistance = maxRange - minRange;  

        // coordenadas geograficas
        ua = rand.nextInt() * (pi/2) * (-1); 
        ud = minRange + rand.nextInt() * maxDistance;  
        dx = BasePos[0] + ud * Math.cos(ua); 
        dy = BasePos[1] + ud * Math.sin(ua);  
        
        //System.out.println("Node: " + dx + " " + dy); 
        float Arr[] = new float[2]; 
        Arr[0] = (float) dx;
        Arr[1] = (float) dy;
        Nodes.add(Arr); 

        if (NiveisRestantes != 1){ 
            nextPos(minRange, maxDistance, NiveisRestantes-1);             
        }  


    }  
    public void show(){ 

        for( float i[]: Nodes){ 
            System.out.println("Node 1: "+i[0] + " Node 2: " + i[1]); 
        }
        } 
    public static void main(String[] args){ 
        gen g = new gen(); 
        g.generate_full_binary_tree_network(0, 10);  
        g.show();
    } 



} 



