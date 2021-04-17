# code to collect throughput from cooja simulations  
 

from functools import reduce 
def average(list): 
    return sum(list)/len(list)

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
            if w == "increased_Bandwidth": 
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

def extract_tx_widerband(source):   
    for i,w in enumerate(words):
            if w == "increased_Bandwidth": 
                return words[i+2]
    
# for j in range(5,19)      
PDR_TASA = [] 
PDR_LBV = []
PRR_TASA = [] 
PRR_LBV = [] 

for i in range(1,11):     
    print("Simulation {}".format(i))
    path = "N{}log{}.txt".format(i,13) 
    #network parameters  
    numNodes = 13
    slotframe_size = 13+ 2 
    aux_numNode = numNodes + 1

    nodes = [0 for i in range(numNodes)]
    # metrics per node  
    Tx_per_node = [0 for i in range(0,aux_numNode)] 
    Rx_per_node = [0 for i in range(0,aux_numNode)] 
    Tx_try_per_node = [0 for i in range(0,aux_numNode)] 
    throughput_per_node = [0 for i in range(0,aux_numNode)]  
    Wider_band = [1 for i in range(0,aux_numNode)] 
    #fill nodes  
    for i in range(0,numNodes): 
        nodes.append(i) 
    # total metrics 
    Tx_total = 0
    Tx_try_sent = 0 
    Rx_total = 0  

    #bandwidth   

    with open(path,'r') as f:
        for line in f:  
            words = line.split() 
            if "Tx_try" in words:  
                node = int(extract_node(words))
                aux_tx_try = int(extract_tx_try(words))  
                for i in range(1,aux_numNode): 
                    if i == node:
                        Tx_try_per_node[i] = aux_tx_try
                
                
            if "Pckt" in words:
                node = int(extract_node(words))
            
                aux_tx = int(extract_tx_success(words)) 
                aux_rx = int(extract_rx(words)) 
            
                Lrx = int(extract_rx(words))  
                for i in range(1,aux_numNode): 
                    if i == node:   
                        if Rx_per_node[i] < Lrx:
                            Rx_per_node[i] = Lrx
                Ltx = int(extract_tx_success(words))  
                for i in range(1,aux_numNode): 
                    if i == node:  
                        if Tx_per_node[i] < Ltx:
                            Tx_per_node[i] = Ltx 
            if "increased_Bandwidth" in words:   

                node = int(extract_node(words)) 
                Bw = int(extract_tx_widerband(words))  
                for i in range(1,aux_numNode): 
                    if i == node:     
                        if Wider_band[i] <= Bw: 
                            Wider_band[i] = Bw

                
                    

    print("metrics simulation {} nodes".format(numNodes))
    #print("nodes:", nodes[:]) 

    print("Metrics for TASA") 

    #print("Tx per node: ",Tx_try_per_node[:]) 
    #print("Rx per node: ",Rx_per_node[:])
    Tx_total =  reduce(lambda x, y:x+y, Tx_try_per_node) 
    Rx_total =  reduce(lambda x, y:x+y, Rx_per_node) 

    #print("Total Tx:",Tx_total)  
    #print("Total Rx:",Rx_total)  
    #throughput = Rx_total/slotframe_size 
    #print("Throughput:",throughput)   
    prr = Tx_total - Rx_total 
    prr = prr/Tx_total
    aux_tasa =  (Rx_total/Tx_total)*100 
     
    PDR_TASA.append(aux_tasa)

    print("PDR(%):",aux_tasa) 
    print("PRR(%)",prr) 


    #print("****************************************************************************")
    print("Metrics for TSCH-LBV:")   

    #print("Wider bandwidth:",Wider_band[:])
    #print("Tx per node: ",Tx_try_per_node[:]) 
    #print("Rx per node: ",Rx_per_node[:]) 
    Tx_total =  reduce(lambda x, y:x+y, Tx_try_per_node) 
    #multiplica os pacotes apenas daqueles que conseguiram mandar pacotes  

    for i in range(0, aux_numNode): 
        Tx_per_node[i] = Tx_per_node[i] * Wider_band[i]   
    Rx_total =  reduce(lambda x, y:x+y, Tx_per_node)  

    #print("****************************************************************************")
    #print("Wider Tx and Rx")
    for i in range(0, aux_numNode): 
        Tx_try_per_node[i] = Tx_try_per_node[i] * Wider_band[i]   

    Tx_total =  reduce(lambda x, y:x+y, Tx_try_per_node) 
    #print("Total Tx:",Tx_total)  
    #print("Total Rx:",Rx_total)  
    #throughput = Rx_total/slotframe_size 
    #print("Throughput:",throughput) 
    prr = Tx_total - Rx_total    
    prr = prr/Tx_total
    aux_lbv = (Rx_total/Tx_total)*100
    print("PDR(%):",aux_lbv)  
    print("PRR(%):",prr)
    PDR_LBV.append(aux_lbv)
    print("---------------------------------------------------------------------------") 
print("PDR average TASA:",average(PDR_TASA)) 
print("PDR average LBV:",average(PDR_LBV))