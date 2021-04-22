import scipy.stats
import numpy as np

def lbv(source): 
    for i,w  in enumerate(source): 
        if w == "n": 
            return int(source[i+1])
def tasa(source): 
    for i,w in enumerate(source): 
        if w =="n": 
            return int(source[i+2])
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
    return talpha*s/np.sqrt(n) 

Slbv = [] 
Stasa = [] 
count = 100
with open("tmAllc.txt","r") as f: 
    for line in f:  
            words = line.split()  
            if "n" in words:  
                Slbv.append(lbv(words)) 
                Stasa.append(tasa(words)) 
                if len(Slbv) == 10 and len(Stasa) == 10: 
                    print("Simulaçao",count)
                    print("LBVMEDIA = {}".format(np.mean(Slbv)))
                    print("LBVCONF = {}".format(confident_interval_data(Slbv,0.95)))
                    print("TASAMEDIA = {}".format(np.mean(Stasa)))
                    print("TASACONF ={}".format(confident_interval_data(Stasa,0.95)))
                    count +=100  
                    Slbv = [] 
                    Stasa = []
                    print("------------------------------------")