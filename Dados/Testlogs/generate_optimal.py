 
total_nodes = [i for i in range(5,21)]
total_th = [0 for i in range(5,21)] 
total_pdr = [0 for i in range(5,21)]
for i in range(5,21):
    num_nodes = i 
    slots = num_nodes - 1 
    # consider only 1 slotframe
    packets_received = [0 for i in range(num_nodes)]   
    packets_sent = [1 for i in range(num_nodes)]   
    packets_sent[0] = 1  
    packets_sent[1] = 0
    for j in range(1,num_nodes): 
         packets_received.append(1)
    
    PDR_r = 0 
    for j in packets_received: 
        PDR_r += j   
    PDR_s = 0
    for j in packets_sent: 
        PDR_s += j   
    total_nodes.append(i) 
    print(i)
    print("PDR(%):",(PDR_r/PDR_s)*100) 
    throughput_optimal = PDR_r/slots 
    print("Throughput_optimal:",throughput_optimal*100);
    print("--------------------------------")
    total_th.append(throughput_optimal*100) 
    total_pdr.append((PDR_r/PDR_s)*100) 
print("nodes:",total_nodes[:])
print("PDR(%):",total_pdr[:]) 
print("Throughput:",total_th[:]) 
    