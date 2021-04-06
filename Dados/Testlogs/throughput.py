# code to collect throughput from cooja simulations  
 
from functools import reduce 
def add(a,b): 
    return a + b 
def next(a,source): 
    for i,w in enumerate(source): 
        if w == a: 
            return source[i+1]
def extract_node(source): 
    for i,w in enumerate(words):
            if w == "Pckt": 
                return words[i+1]  
            if w == "Tx_try":   
                return words[i+1]

def extract_tx_success(source):   
    for i,w in enumerate(words):
            if w == "Pckt": 
                return words[i+2]
def extract_rx(source): 
    for i,w in enumerate(words):
            if w == "Pckt": 
                return words[i+3]
def extract_tx_try(source):  
    for i,w in enumerate(words):
            if w == "Tx_try": 
                return words[i+2]
    
#network parameters  
numNodes = 7
slotframe_size =  9
aux_numNode = numNodes + 1

nodes = [0 for i in range(numNodes)]
# metrics per node  
Tx_per_node = [0 for i in range(0,aux_numNode)] 
Rx_per_node = [0 for i in range(0,aux_numNode)] 
Tx_try_per_node = [0 for i in range(0,aux_numNode)] 
throughput_per_node = [0 for i in range(0,aux_numNode)] 
#fill nodes  
for i in range(0,numNodes): 
    nodes.append(i) 
# total metrics 
Tx_total = 0
Tx_try_sent = 0 
Rx_total = 0  

#bandwidth  

with open('teste.txt','r') as f:
    for line in f:  
        words = line.split() 
        if "Tx_try" in words:  
            node = int(extract_node(words))
            aux_tx_try = int(extract_tx_try(words))  
            for i in range(1,6): 
                if i == node:
                    Tx_try_per_node[i] = aux_tx_try
            
            
        if "Pckt" in words:
            node = int(extract_node(words))
        
            aux_tx = int(extract_tx_success(words)) 
            aux_rx = int(extract_rx(words)) 
           
            Lrx = int(extract_rx(words))  
            for i in range(len(Rx_per_node)): 
                if i == node:   
                    if Rx_per_node[i] < Lrx:
                        Rx_per_node[i] = Lrx
            Ltx = int(extract_tx_success(words))  
            for i in range(len(Tx_per_node)): 
                if i == node:  
                    if Tx_per_node[i] < Ltx:
                        Tx_per_node[i] = Ltx
            
                

print("metrics simulation {} nodes".format(numNodes))
#print("nodes:", nodes[:]) 
 
print("Metrics for TASA") 

print("Tx per node: ",Tx_try_per_node[:]) 
print("Rx per node: ",Rx_per_node[:])
Tx_total =  reduce(lambda x, y:x+y, Tx_try_per_node) 
Rx_total =  reduce(lambda x, y:x+y, Rx_per_node) 

print("Total Tx:",Tx_total)  
print("Total Rx:",Rx_total)  
#throughput = Rx_total/slotframe_size 
#print("Throughput:",throughput)  
print("PDR(%):",(Rx_total/Tx_total)*100)

print("Metrics for TSCH-LBV:")  

print("Tx per node: ",Tx_try_per_node[:]) 
print("Rx per node: ",Rx_per_node[:])
Tx_total =  reduce(lambda x, y:x+y, Tx_try_per_node) 
Rx_total =  reduce(lambda x, y:x+y, Rx_per_node) 

print("Total Tx:",Tx_total)  
print("Total Rx:",Rx_total)  
#throughput = Rx_total/slotframe_size 
#print("Throughput:",throughput)  
print("PDR(%):",(Rx_total/Tx_total)*100)

