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
    return  talpha*s/np.sqrt(n) 


for j in range(3,17):      
      
    max_band = 1
    numNodes = j  
    Band_list = []

    for i in range(1,11):    
        flag = 0   

        path = "{}nos/N{}log{}.txt".format(j,i,numNodes) 
        #bandwidth   
        with open(path,'r') as f:
            for line in f:  
                words = line.split()  
                if "increased_Bandwidth" in words:   
 
                    Bw = int(extract_tx_widerband(words))  
                    #garante que não haja canais usados sem no minimo 1 canal computado
                    if Bw == 0: 
                        Bw = 1 
                    elif Bw > 1 and Bw%2 != 0: 
                        Bw += 1   
                    if Bw > max_band:
                        max_band = Bw
        Band_list.append(max_band) 
                        

    print("metrics simulation {} nodes".format(numNodes))
    print(Band_list[:])  
    print("max_band: {}".format(max_band)) 
    print("Avarege: {}".format(average(Band_list)))
    print("Interval_conf_MaxBand_PDR: {} \n".format(confident_interval_data(Band_list,0.95)))
    
    print("---------------------------------------------------------------------------") 


