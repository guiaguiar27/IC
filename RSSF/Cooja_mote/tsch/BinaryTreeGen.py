import random 
from binarytree import build


##
# create node's list 
## 
dim = int(input("Dim:")) 
nodes = [] 
nodes.append(1) 
root = nodes[0]
for i in range(dim): 
    value = random.randint(2, dim)  
    if value in nodes:
        value = random.randint(2, dim)  
             
    nodes.append(value) 

print(nodes)  

root = build(nodes)
print(root) 
print(root.values)