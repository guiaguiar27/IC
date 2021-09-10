
import sys
import json
from json import encoder 
import random
from math   import floor, sqrt, pi, sin, cos 

def generate_full_binary_tree_network(numLvls, maxRange, minRange=0, ipos=(0,0)):
    def add_next(nodes, isParentOf, basePos, lvlsRemaining, maxRange):
        pid = len(nodes) - 1
        maxDistance = maxRange - minRange
        # up child
        ua = random.random() * (pi/2)
        ud = minRange + random.random() * maxDistance
        ux = basePos[0] + ud * cos(ua)
        uy = basePos[1] + ud * sin(ua)
        nodes.append((ux, uy))                 # adding node to list
        isParentOf.append([])                  # list for the sons
        isParentOf[pid].append(len(nodes) -1)  # adding topology reference to it
        if lvlsRemaining != 1:
            add_next(nodes, isParentOf, (ux, uy), lvlsRemaining - 1, maxRange)
        # down child
        da = random.random() * (pi/2) * (-1)
        dd = minRange + random.random() * maxDistance
        dx = basePos[0] + dd * cos(da)
        dy = basePos[1] + dd * sin(da)
        nodes.append((dx, dy))                 # adding node to list
        isParentOf.append([])                  # list for the sons
        isParentOf[pid].append(len(nodes) -1)  # adding topology reference to it
        if lvlsRemaining != 1:
            add_next(nodes, isParentOf, (dx, dy), lvlsRemaining - 1, maxRange)

    nodes       = []
    isParentOf  = []
    # adding initial node 
    nodes.append(ipos)
    isParentOf.append([])
    add_next(nodes, isParentOf, ipos, numLvls - 1, maxRange)
    topo = []
    for i in range(len(isParentOf)):
        for j in isParentOf[i]: 
            # tirar o zero como no incluso
            i+=1 
            j+=1  
            #j is the parent of i 
            topo.append((j, i))
    return nodes, topo

# generate a network where the sink is connected to the root of a full binary 
# tree network.
#       S ----- (FBT)
# def generate_unbalanced_binary_tree_network(numLvls, maxRange, minRange=0, 
#                                             ipos=(0,0)):
#     # adding initial node
#     maxDistance = maxRange - minRange
#     na = (pi/2) + (random.random() * pi)
#     nd = minRange + random.random() * maxDistance
#     nx = 0 + nd * cos(na)
#     ny = 0 + nd * sin(na)
#     #
#     nodes, topo = generate_full_binary_tree_network(numLvls, maxRange, minRange, 
#                                                     ipos)
#     # adding the first node and updating the network topology
#     nodes.insert(0, (nx, ny))
#     ntopo = []
#     ntopo.append((0,1))
#     # updating indexes
#     for relation in topo:
#         ntopo.append((relation[0] + 1, relation[1] + 1))
#     return nodes, ntopo


# def generate_random_tree_network(numNodes, maxRange, maxChildren, minRange=0, 
#                                  minChildren=1, ipos=(0,0)):
#     nodes = []
#     topo  = []
#     nexts = []
#     maxDistance = maxRange - minRange
#     # first node
#     nodes.append(ipos)
#     nid = 0
#     nexts.append(nid)
#     while len(nodes) < numNodes:
#         parent = nexts.pop(0)
#         numChild = random.randint(minChildren, maxChildren)
#         # limitating the number of children by the remaining number of nodes
#         numChild = min(numChild, (numNodes - len(nodes)))
#         # generating children positions
#         for i in range(numChild):
#             na = (-pi/2) + (random.random() * pi)  # from -90 to 90 degrees
#             nd = minRange + random.random() * maxDistance
#             nx = nodes[parent][0] + nd * cos(na)
#             ny = nodes[parent][1] + nd * sin(na)
#             nodes.append((nx, ny))
#             cid = len(nodes) - 1
#             topo.append((parent, cid))
#             nexts.append(cid)
#     return nodes, topo



numNet   = 1
minRange = 2
nrange   = None
nlevels  = None
outName  = None

for i in range(1, len(sys.argv), 2):
    if sys.argv[i] == "-l":
        nlevels = int(sys.argv[i+1])
    elif sys.argv[i] == "-d":
        nrange = float(sys.argv[i+1])
    elif sys.argv[i] == "-m":
        minRange = float(sys.argv[i+1])
    elif sys.argv[i] == "-o":
        outName = sys.argv[i+1]
    elif sys.argv[i] == "-t":
        numNet = int(sys.argv[i+1])

if nlevels is None:
    print("Missing number of levels (-l [NUMBER])")
    exit(1)
if nrange is None:
    print("Missing nodes max distance (-d [DISTANCE])")
    exit(1)
if outName is None:
    print("Missing output file (-o [FILE])")
    exit(1)

data = {}

for i in range(numNet):
    nodes, topo = generate_full_binary_tree_network(nlevels, nrange, 
                                                          minRange)

with open(outName, "w") as outFile:
    for index, j in enumerate(topo): 
        el = j[0] 
        el2 = j[1]
        #print("{} ->{}".format(el,el2))
        asc = "{} {}\n".format(el,el2)
        outFile.write(asc)

