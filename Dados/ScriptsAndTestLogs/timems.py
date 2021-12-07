count = 0 
def next(source): 
    for i,w in enumerate(source): 
        if w == "i" or w == "f" : 
            return source[i+1]  
def th(source): 
    for i,w in enumerate(source): 
        if w == "i" or w == "f" : 
            return source[i+2]  
Ftotal = Itotal = 0 
with open('newThroughput.txt','r') as f:
    for line in f:  
        words = line.split()
        if "i" in words:  
             time = next(words) 
             aux = time.split('.') 
             Iminutes = int(aux[0])  
             totalMilMin = Iminutes*60000 
             Iseconds = int(aux[1])  
             totalMilSeconds = Iseconds*1000  
             Iml = int(aux[2])  
             Itotal = totalMilMin + totalMilSeconds + Iml
            
        if "f" in words:   
            time = next(words) 
            aux = time.split('.') 
            Iminutes = int(aux[0])  
            totalMilMin = Iminutes*60000 
            Iseconds = int(aux[1])  
            totalMilSeconds = Iseconds*1000  
            Iml = int(aux[2])  
            Ftotal = totalMilMin + totalMilSeconds + Iml
            print(Ftotal,"- ",Itotal,"time in miliseconds: ",Ftotal - Itotal) 
            print("---------------------------------------------------------------------------")   
        