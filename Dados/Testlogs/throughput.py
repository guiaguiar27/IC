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
def exctract_tx_success(source):   
    for i,w in enumerate(words):
            if w == "Pckt": 
                return words[i+2]
def extract_rx(source): 
    for i,w in enumerate(words):
            if w == "Pckt": 
                return words[i+3]
def extract_tx_try(source): 
    pass  
nodes = [] 
Tx_success = [] 
Rx_success = []
with open('teste.txt','r') as f:
    for line in f:  
        words = line.split() 
        if "Pckt" in words:
            nodes.append(extract_node(words)) 
            Tx_success.append(exctract_tx_success(words)) 
            Rx_success.append(extract_rx(words)) 
        
numNodes = len(nodes)        
print("metrics simulation {} nodes".format(numNodes))
print("nodes:", nodes[:]) 
Tx_final = list(map(int, Tx_success)) 
Rx_final = list(map(int, Rx_success))
Tx_total = reduce(add, Tx_final)
print("tx_success:", Tx_final[:]) 
print("Total Tx:",Tx_total) 
print("rx_success:",Rx_final[:])