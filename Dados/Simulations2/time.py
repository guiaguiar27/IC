import re
import scipy.stats
import numpy as np
from functools import reduce   

def extract_tx_widerband(source):   
    for i,w in enumerate(words):
            if w == "increased_Bandwidth": 
                return words[i+2]  
def average(list): 
    return sum(list)/len(list)

def confident_interval_data(X, confidence = 0.95, sigma = -1):
    def S(X): #funcao para calcular o desvio padrao amostral
        s = 0
        for i in range(0,len(X)):
            s = s + (X[i] - np.mean(X))**2
        s = np.sqrt(s/(len(X)-1))
        return s
    n = len(X) # numero de elementos na amostra
    Xs = np.mean(X) # media amostral
    s = S(X) # desvio padrao amostral
    zalpha = abs(scipy.stats.norm.ppf((1 - confidence)/2))
    if(sigma != -1): # se a variancia eh conhecida
        IC1 = Xs - zalpha*sigma/np.sqrt(n)
        IC2 = Xs + zalpha*sigma/np.sqrt(n)
    else: # se a variancia eh desconhecida
        if(n >= 50): # se o tamanho da amostra eh maior do que 50
            # Usa a distribuicao normal
            IC1 = Xs - zalpha*s/np.sqrt(n)
            IC2 = Xs + zalpha*s/np.sqrt(n)
        else: # se o tamanho da amostra eh menor do que 50
            # Usa a distribuicao t de Student
            talpha = scipy.stats.t.ppf((1 + confidence) / 2., n-1)
            IC1 = Xs - talpha*s/np.sqrt(n)
            IC2 = Xs + talpha*s/np.sqrt(n)
    return [IC1, IC2]

def justTime(string): 
    return int(string) 
general_band = [] 

for j in range(5,19):      
      
    max_band = 1
    numNodes = j  
    time_list = []  
    
    for i in range(1,11):    
        flag = 0    
        count = 0 

        path = "{}nodes/N{}log{}.txt".format(j,i,numNodes) 
        #bandwidth   
        with open(path,'r') as f:
            for line in f:  
                
                words = line.split()  
                if "Pckt" in words:   
                    if count == 0:
                        itime = words[0]  
                        #quando  é dado em milisegundos
                        init_time = justTime(itime)
                        count+=1         
            
        time_list.append(init_time) 
                        

    print("metrics simulation {} nodes".format(numNodes))
    print(time_list[:])   
    print("Avarege: {}".format(average(time_list)))  
    aux = average(time_list)
    general_band.append(aux)
    print("---------------------------------------------------------------------------")  

print(" Genral Avarege: {}".format(average(general_band))) 
print("Interval_conf_MaxBand_PDR: {} \n".format(confident_interval_data(general_band,0.95))) 
     
    


