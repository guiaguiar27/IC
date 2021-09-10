
import random  
import math as m  

from binarytree import build

def random_list(dimension):
    Gen = [] 
    for i in range(1,dimension+1): 
        Gen.append(i)  
    result = []  
    print(Gen[:])
    result = random.sample(Gen,dim) 
    return result 
def change(listNodes):  
    aux = listNodes[0]
    for i in listNodes: 
        if i == 1:
            i = aux 
            listNodes[0] = 1  
def get_topology(listNodes): 
    dim = len(listNodes)
    for i in range(dim-1):  
        if i%2 == 0: 
            print("{} -> {}".format(listNodes[i], listNodes[i+2]))
        else: 
            print("{} -> {}".format(listNodes[i], listNodes[i+1]))
def set_root(nodes): 
    position = 0   
    for i in range(len(nodes)): 
        if nodes[i] == 1: 
            position = i  
    nodes.pop(position) 
    nodes.insert(0,1)
        

class node:
    def __init__(self, num):
        self.value = num
        self.left = None
        self.right = None
        self.height = 1

class AVL:

    def height(self, Node):
        if Node is None:
            return 0
        else:
            return Node.height

    def balance(self, Node):
        if Node is None:
            return 0
        else:
            return self.height(Node.left) - self.height(Node.right)

    def MinimumValueNode(self, Node):
        if Node is None or Node.left is None:
            return Node
        else:
            return self.MinimumValueNode(Node.left)

    def rotateR(self, Node):
        a = Node.left
        b = a.right
        a.right = Node
        Node.left = b
        Node.height = 1 + max(self.height(Node.left), self.height(Node.right))
        a.height = 1 + max(self.height(a.left), self.height(a.right))
        return a

    def rotateL(self, Node):
        a = Node.right
        b = a.left
        a.left = Node
        Node.right = b
        Node.height = 1 + max(self.height(Node.left), self.height(Node.right))
        a.height = 1 + max(self.height(a.left), self.height(a.right))
        return a

    def insert(self, val, root):
        if root is None:
            return node(val)
        elif val <= root.value:
            root.left = self.insert(val, root.left)
        elif val > root.value:
            root.right = self.insert(val, root.right)
        root.height = 1 + max(self.height(root.left), self.height(root.right))
        balance = self.balance(root)
        if balance > 1 and root.left.value > val:
            return self.rotateR(root)
        if balance < -1 and val > root.right.value:
            return self.rotateL(root)
        if balance > 1 and val > root.left.value:
            root.left = self.rotateL(root.left)
            return self.rotateR(root)
        if balance < -1 and val < root.right.value:
            root.right = self.rotateR(root.right)
            return self.rotateL(root)
        return root

    def preorder(self, root):
        if root is None:
            return
        print(root.value)
        self.preorder(root.left)
        self.preorder(root.right)
    def postorder(self, root,listNodes): 
        if root is None: 
            return  
        self.postorder(root.left,listNodes) 
        self.postorder(root.right,listNodes)  
        listNodes.append(root.value)
        print(root.value) 

    def delete(self, val, Node):
        if Node is None:
            return Node
        elif val < Node.value:
            Node.left = self.delete(val, Node.left)
        elif val > Node.value:
            Node.right = self.delete(val, Node.right)
        else:
            if Node.left is None:
                lt = Node.right
                Node = None
                return lt
            elif Node.right is None:
                lt = Node.left
                Node = None
                return lt
            rgt = self.MinimumValueNode(Node.right)
            Node.value = rgt.value
            Node.right = self.delete(rgt.value, Node.right)
        if Node is None:
            return Node
        Node.height = 1 + max(self.height(Node.left), self.height(Node.right))
        balance = self.balance(Node)
        if balance > 1 and self.balance(Node.left) >= 0:
            return self.rotateR(Node)
        if balance < -1 and self.balance(Node.right) <= 0:
            return self.rotateL(Node)
        if balance > 1 and self.balance(Node.left) < 0:
            Node.left = self.rotateL(Node.left)
            return self.rotateR(Node)
        if balance < -1 and self.balance(Node.right) > 0:
            Node.right = self.rotateR(Node.right)
            return self.rotateL(Node)
        return Node

Tree = AVL()
rt = None 


dim = int(input("Dim:"))    
nodes = random_list(dim)   

print(nodes[:])
set_root(nodes) 


for i in nodes: 
    rt = Tree.insert(i,rt)

print("POSTORDER") 
nodesPO = []
Tree.postorder(rt,nodesPO)  
print(nodesPO[:])    
#get_topology(nodesPO) 



# root = build(nodesPO)
# print(root) 
# print(root.is_balanced)




