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
numNodes = 19
slotframe_size = 17 


nodes = [0 for i in range(numNodes)]
# metrics per node  
Tx_per_node = [0 for i in range(numNodes)] 
Rx_per_node = [0 for i in range(numNodes)] 
Tx_try_per_node = [0 for i in range(numNodes)] 
throughput_per_node = [0 for i in range(numNodes)] 
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
            for i in range(len(Tx_try_per_node)): 
                if i == nodes:  
                    if Tx_try_per_node[i] < aux_tx_try:
                        Tx_try_per_node[i] = aux_tx_try
            
            if Tx_try_sent < aux_tx_try: 
                Tx_try_sent = aux_tx_try  

        if "Pckt" in words:
            node = int(extract_node(words))
        
            aux_tx = int(extract_tx_success(words)) 
            if Tx_total < aux_tx: 
                Tx_total = aux_tx 
            aux_rx = int(extract_rx(words)) 
            if Rx_total < aux_rx: 
                Rx_total = aux_rx  

            Lrx = int(extract_rx(words))  
            for i in range(len(Rx_per_node)): 
                if i == nodes:  
                    if Rx_per_node[i] < Lrx:
                        Rx_per_node[i] = Lrx
            Ltx = int(extract_tx_success(words))  
            for i in range(len(Tx_per_node)): 
                if i == nodes:  
                    if Tx_per_node[i] < Ltx:
                        Tx_per_node[i] = Ltx
            
                

print("metrics simulation {} nodes".format(numNodes))
#print("nodes:", nodes[:]) 
print("Total Tx:",Tx_total)  
print("Total Tx try:", Tx_try_sent)  
throughput = Tx_total/Tx_try_sent 
print("Throughput:",throughput) 
print("Throughput (packet_receive/numSlots):", Tx_total/slotframe_size)
print("Total Rx:",Rx_total)  

print("Tx per node: ",Tx_per_node[:]) 
print("Rx per node: ",Rx_per_node[:])