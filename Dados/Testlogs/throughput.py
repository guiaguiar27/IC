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
    
nodes = []
Tx_total = 0
Tx_try_sent = 0 
Rx_total = 0  
slotframe_size = 8
with open('teste.txt','r') as f:
    for line in f:  
        words = line.split() 
        if "Tx_try" in words: 
            aux_tx_try = int(extract_tx_try(words)) 
            if Tx_try_sent < aux_tx_try: 
                Tx_try_sent = aux_tx_try 
        if "Pckt" in words:
            nodes.append(extract_node(words)) 
            aux_tx = int(extract_tx_success(words)) 
            if Tx_total < aux_tx: 
                Tx_total = aux_tx 
            aux_rx = int(extract_rx(words)) 
            if Rx_total < aux_rx: 
                Rx_total = aux_rx 
                
numNodes = len(nodes)      

print("metrics simulation {} nodes".format(numNodes))
#print("nodes:", nodes[:]) 
print("Total Tx:",Tx_total)  
print("Total Tx try:", Tx_try_sent) 
throughput = Tx_total/Tx_try_sent 
print("Throughput:",throughput) 
print("Throughput (packet_receive/numSlots):", Tx_total/slotframe_size)
print("Total Rx:",Rx_total) 