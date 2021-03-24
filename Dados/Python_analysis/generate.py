import random
n = int(input("Num nos")) 
for i in range(1,n): 
    node1 = random.randint(1, n) 
    node2 = random.randint(1,node1) 
    while(node1 == node2): 
        node1 = random.randint(1, n) 
        node2 = random.randint(1,node1)
    print(node1, node2)
