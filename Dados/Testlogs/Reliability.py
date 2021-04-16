count = 0 
def next(source): 
    for i,w in enumerate(source): 
        if w == "LBV_Total" or w == "TASATotal" or w == "LBVPacket_lost" or w == "TASAPacket_lost": 
            return source[i+1]  

Ftotal = Itotal = 0  
total = [] 
lost = []

with open('tasaRel.txt','r') as f:
    #multply number of packets by 10/ pckt/slots 
    for line in f:  
        words = line.split() 
        if "LBV_Total" in words:  
            total.append(int(next(words)))     
        
        if "TASATotal" in words:  
            total.append(int(next(words))) 
        if "LBVPacket_lost" in words: 
            lost.append(int(next(words))) 
        if "TASAPacket_lost" in words: 
            lost.append(int(next(words)))
print(total[:]) 
print(lost[:]) 
for i in range(len(total)): 
    print((1 - lost[i]/total[i]),",")