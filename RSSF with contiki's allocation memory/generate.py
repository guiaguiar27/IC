import random
import sys
sorteados = []
link_1 = []
n = int(sys.argv[1]) 
arq = open('res.txt', 'w')
for i in range(1,n): 
    node1 = random.randint(1, n) 
    node2 = random.randint(1,node1) 
    while(node1 == node2): 
        node1 = random.randint(1, n) 
        node2 = random.randint(1,node1)
    
    if node2 not in sorteados and node2 not in link_1 and node2 != 1:
        link_1.append(node2)
    if node1 not in sorteados:
        sorteados.append(node1)
    if node2 not in sorteados:
        sorteados.append(node2)
    if node1 in link_1:
        link_1.remove(node1)
    
    arq.writelines(str(node1) + ' ' + str(node2) + '\n')
    print(node1, node2)

for i in link_1:
    if i == link_1[len(link_1) - 1]:
        arq.writelines(str(i) + ' 1')
    else:
        arq.writelines(str(i) + ' 1' + '\n')
    print(i, '1')
arq.close()
