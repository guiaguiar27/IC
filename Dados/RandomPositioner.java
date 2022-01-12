package org.contikios.cooja.positioners;
import java.util.*;

import org.contikios.cooja.*;

//@ClassDescription("FULLTREE")
@ClassDescription("Random positioning")
public class RandomPositioner extends Positioner{

    double startX, endX, startY, endY, startZ, endZ;
    private double xPos, yPos, zPos;
    private int TotalNumNodes;
    private int node = 0;
    private int index;

    List<Double> nodesX = new ArrayList<>();
    List<Double> nodesY = new ArrayList<>();
    List<Double> Xpos = new ArrayList<>();
    List<Double> Ypos = new ArrayList<>();
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
    Gen(0);
    //Generate_full_binary_tree(this.level, this.startX, this.startY);
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

    public void Gen(int level){
        double ux,uy;
        int expo = level;
        int limit = (int) Math.pow(2,expo);
        System.out.println("limit: "+ limit);

        if(level == 0){
            uy = this.startY;
            ux = this.startX;
            this.nodesX.add(ux);
            this.nodesY.add(uy);
            this.Xpos.add(ux);
            this.Ypos.add(uy);
        }
        else{
            int counter_pos, aux_for_y;
            counter_pos = 0;
            aux_for_y = 3;
            if(level%2 != 0){
                System.out.println("level impar: " + level);
                for(int j = 1 ; j <=limit;j++){
                    if(level == 1){

                    uy = this.nodesY.get(level-1) + 30.0;

                    }
                    else{
                      System.out.println("aux for y: "+ aux_for_y);
                      if(aux_for_y%2 != 0){

                        uy = this.nodesY.get(aux_for_y) + 40.0;

                        System.out.println(" val get: " + this.nodesY.get(aux_for_y) );
                        System.out.println("par "+ j + " val: " + uy);
                      }
                      else{
                        uy = this.nodesY.get(aux_for_y) - 40.0;

                        System.out.println(" val get: " + this.nodesY.get(aux_for_y) );
                        System.out.println("impar "+ j + " val: " + uy);
                        }
                      System.out.println("Get coordinate Y = "+ j);
                      if(j%2==0){
                        aux_for_y += 1;
                      }
                      //else {
                    //    uy = this.nodesY.get(aux_for_y) - 20;
                  //      System.out.println("Get coordinate Y(impar) "+ j);

                    //  }

                  }
                  this.nodesY.add(uy);
                  this.Ypos.add(uy);

                }

                for(int j = 1 ; j <= limit ; j++){

                    //if(j > 1)
                  //      ux = this.Xpos.get(level -1)+30*j - 30;
                //    else {
                //        ux = this.Xpos.get(level-1) - 30.0;
                //        this.Xpos.add(ux);
              //      }

                    if(j%2==0){

                      ux = this.Xpos.get(counter_pos)+28;
                      System.out.println("Get coordinate(par) "+ j);
                      counter_pos +=1;
                    }
                    else {

                      ux = this.Xpos.get(counter_pos)-28;
                      System.out.println("Get coordinate(impar) "+ j);

                    }
                    this.nodesX.add(ux);
                }
            }
            else{
                this.Xpos.clear();
                int half_limit = limit/2;
                System.out.println("--Definicao do eixo x-- ");
                for(int j = 1; j <= half_limit; j++ ){
                    System.out.println("X(primeira metade):"+ level);
                    ux = this.nodesX.get(0) + 30.0*level;
                    this.nodesX.add(ux);
                    this.Xpos.add(ux);
                }
                for(int j = half_limit +1 ; j <= limit; j++){
                    System.out.println("X(segunda metade):" + level);
                    ux = this.nodesX.get(0) - 30.0*level;
                    this.nodesX.add(ux);
                    this.Xpos.add(ux);
                }

                for(int j = 1; j <= limit ; j++){
                    System.out.println("Y(Parte unica)");
                    if(j%2 == 0)
                        uy = this.Ypos.get(level-1) - 30.0;
                    else
                        uy = this.Ypos.get(level-1) + 30.0;
                    this.nodesY.add(uy);
                }
            }
        }

        if(level <  this.level -1){
            level++;
            Gen(level);

        }

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

        // coordenada y é apenas o nível (fazer a modificação necessária)

        ua = Math.random() * (Math.PI/2);
        double udX =  Math.random() * MaxdistanceX;
        double udY =  Math.random() * MaxdistanceX;
        ux = nodeX + udX * Math.cos(ua);
        uy = nodeY + udY * Math.sin(ua);
        this.nodesX.add(ux);
        this.nodesY.add(uy);

        if (lv >=  1){
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
        if (lv >= 1){
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
