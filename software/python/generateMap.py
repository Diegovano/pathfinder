## Python Version of map generator from https://github.com/MerelyLogical/blog/blob/main/src/mapgen.js


import matplotlib.pyplot as plt
import random
import math
from queue import PriorityQueue 
import time 

# Node class with:
#             co-ordinates,
#             visited flag,
#             current shortest distance to this node,
#             index of the last node visited before this one

class Node:
    def __init__(self, x, y, visited=False, search_dist=999, search_last=999, isLandmark=False):
        self.x = x
        self.y = y
        self.visited = visited
        self.search_dist = search_dist
        self.search_last = search_last
        self.isLandmark = isLandmark


size = 99           # number of nodes in graph; has to be >= 10
nodes = []          # contains the instances of Node
edges = []          # holds pair of indicies representing the edges between two nodes 
dist_matrix = []    # matrix of distances between every two pairs of nodes in the graph (dist_matrix[i][j] = dist between node i and node j)
solution = []

start = None        # index of the start node
goal = None         # index of the goal node


plt.ion()

num_landmarks = 4   # number of landmarks can change this number to be whatever we want
landmarks = []

def setLandmarks():
    for i in range(num_landmarks):
        index = random.randint(0,len(nodes))
        while(nodes[index].isLandmark == False):
            if(nodes[index].isLandmark == True):
                index = random.randint(0,len(nodes))
            else:
                nodes[index].isLandmark = True
                landmarks.append(index)

def generateGraph():
    global start, goal

    # Generate the nodes of the graph, each with random coordinates between 0 and 1
    for i in range(0, size):
        randx = abs(random.uniform(0, 1))  
        randy = abs(random.uniform(0, 1)) 
        nodes.append(Node(randx, randy))  # Create a Node object and add it to the list of nofes

        # init a list to represent the distances from this node to all other nodes
        dists = [999] * size
        # calcualte the distance between this node and all previously created nodes
        for j in range(0, i):
            dist = math.sqrt(((randx - nodes[j].x) ** 2) + ((randy - nodes[j].y) ** 2))
            dists[j] = dist  

        # append this node's distances to the distance matrix
        dist_matrix.append(dists)

    for i in range(0, size):
        connections = 0  # count for the number of connections this node has
        for j in range(0, i):
            # connection is made based on a random roll and a threshold (?)
            roll = random.uniform(0, 1) / 2.5  
            threshold = (dist_matrix[i][j] / math.sqrt(2)) + (connections / size)
            if roll > threshold:
                edges.append([i, j])  # create edge between nodes i and j
                dist_matrix[j][i] = dist_matrix[i][j]  # symmetric distance
                connections += 1
            else:
                dist_matrix[i][j] = 999  # set as max (inf)

        dist_matrix[i][i] = 0  # node has 0 distance to itself

    # Randomly select a starting node
    start = math.floor(random.uniform(0, 1) * size)
    nodes[start].search_dist = 0
    nodes[start].search_last = start  

    # select a goal node and make sure its not the start node
    goal = math.floor(random.uniform(0, 1) * (size - 1))
    if goal >= start:
        goal += 1

# Dijkstra Algo Basic
def findPath():
    global start, goal  

    # start from start node
    visiting_node = start
    visiting_dist = 0

    # priority queue for 'dijkstra's' algorithm
    # starting distances are set to infinity (999) except for the start node
    search_prios = [999] * size
    search_prios[start] = 0

    #print(f"Start, Goal: {start}, {goal}") 

    # iterate through all nodes to find the shortest path
    for cnt in range(size):
        # Find the node with the smallest distance that hasn't been visited
        visiting_node = search_prios.index(min(search_prios))
        visiting_dist = nodes[visiting_node].search_dist
        #print(f"Visiting: {visiting_node}, {visiting_dist}")  

        # if current visiting node is the goal or has already been visited, exit the loop
        if visiting_node == goal or nodes[visiting_node].visited:
            break

        # loop through neighbours of the current visiting node
        for i in range(size):
            if not nodes[i].visited:
                # calc the new distance to this neighbour
                new_dist = visiting_dist + dist_matrix[visiting_node][i]
                #print(f"Exploring: {i}, {new_dist}, {nodes[i].search_dist}") 

                # if new distance shorter than the known distance update the distance and the path
                if new_dist < nodes[i].search_dist:
                    nodes[i].search_dist = new_dist
                    search_prios[i] = new_dist
                    nodes[i].search_last = visiting_node 

        # mark current node as visited
        nodes[visiting_node].visited = True
        search_prios[visiting_node] = 999 # make sure not visited again

    #print(nodes) 

def heuristic(n1, n2):
    return math.sqrt(((n1.x - n2.x) ** 2) + ((n2.y - n1.y) ** 2))

def findPathAStar():
    # implement something similar to this https://www.redblobgames.com/pathfinding/a-star/introduction.html
    global start, goal, nodes, dist_matrix, solution
    pq = PriorityQueue()                                       # has the format of (f_score, node)
    pq.put((0,start))                                          # start of the queue is the start node
    came_from = {start: None}                                  # start a dictionary of where the node came from
    g_score   = {start: 0}                                     # start a dictionary of the gscore of the node
    f_score   = {start: heuristic(nodes[start], nodes[goal])}  # start a dictionary of the fscore of the node

    closed_set = set()                                         # set of all the closed nodes 

    # whilst the queue is not empty 
    while not pq.empty():
        _, current = pq.get()    # current is now the node with the lowest f score 
        
        # if current node is the goal then we have a solution 
        if current == goal:
            solution = []  
            while current in came_from:
                prev = came_from[current]
                if prev is not None:
                    solution.append([prev, current])
                current = prev
            solution.reverse()
            print(f"The result from A* is {solution}")
            return solution

        closed_set.add(current)

        for i in range(size):
            if dist_matrix[current][i] == 999 or i in closed_set:
                # if the neighbour is distance inf or neighbour already explored skip it
                continue
            
            # gscore of this node is the gscore of current + distance to neighbour 
            temp_g_score = g_score[current] + dist_matrix[current][i]

            # if the neighbour is not in the g_score dict or the calculated g_score is less than 
            # its current g_score 
            if i not in g_score or temp_g_score < g_score[i]: 
                came_from[i] = current                                        # the neighbour came from current node
                g_score[i]   = temp_g_score                                   # store g_score
                f_score[i]   = temp_g_score + heuristic(nodes[i], nodes[goal])# calc and store f_score
                if i not in closed_set:
                    # if neighbour not in closed set (has not been fully explored)
                    pq.put((f_score[i], i)) # add to priority queue to be explored 
    
    print("No solution to A* Algorithm")
    return [] 

# A* Algorithm ALT Method - in progress
def preprocess():
    global landmarks

    # matrix which stores the distance from the a node and the landmark 
    # eg lm_dist_matrix[1,2] is the distance from the first node to the 2nd landmark 
    lm_dist_matrix = [[0 for _ in range(num_landmarks)] for _ in range(size)]

    for i in range(size):
        print(lm_dist_matrix[i])

    # we need to loop through every node and calculate the distance to the landmark 

def findPathALT():
    global start, goal, nodes, dist_matrix, solution
    pq = PriorityQueue()                                       # has the format of (f_score, node)
    pq.put((0,start))                                          # start of the queue is the start node
    came_from = {start: None}                                  # start a dictionary of where the node came from
    g_score   = {start: 0}                                     # start a dictionary of the gscore of the node
    f_score   = {start: heuristic(nodes[start], nodes[goal])}  # start a dictionary of the fscore of the node

    closed_set = set()                                         # set of all the closed nodes 

    preprocess()

    return 0 

def connectPath():
    global solution 
    current_node = goal

    while current_node != start and current_node != 999:
        prev_node = nodes[current_node].search_last
        solution.append([prev_node, current_node])
        current_node = prev_node
    
    solution.reverse()
    print(f"The result from Dijkstra is {solution}")

def plotGraph(showPath=True, figureId=1):
    # https://www.w3schools.com/python/matplotlib_plotting.asp

    plt.figure(figureId, figsize=(5, 5))
    plt.clf()

    for edge in edges:
        # for every edge then plot the line
        x_values = [nodes[edge[0]].x, nodes[edge[1]].x]
        y_values = [nodes[edge[0]].y, nodes[edge[1]].y]
        plt.plot(x_values, y_values, 'gray', lw=0.5)

    # loop through nodes and indices in nodes array
    for idx, node in enumerate(nodes):
        if idx == start:
            # make green start
            plt.plot(node.x, node.y, 'go', markersize=8, label='Start')
        elif idx == goal:
            # make red end
            plt.plot(node.x, node.y, 'ro', markersize=8, label='Goal')
        elif node.isLandmark:
            plt.plot(node.x, node.y, 'ko')    # black is the landmark
        else:
            # other nodes are blue and smaller 
            plt.plot(node.x, node.y, 'bo', markersize=4)

    if showPath and solution:
        for path in solution:
            # for each path in the solutuon then we plot the line
            x_values = [nodes[path[0]].x, nodes[path[1]].x]
            y_values = [nodes[path[0]].y, nodes[path[1]].y]
            plt.plot(x_values, y_values, 'cyan')

    plt.xlabel("X")
    plt.ylabel("Y")
    plt.title("Graph with Path")
    plt.legend()
    plt.draw()
    
# Generate the graph and plot    
generateGraph()
setLandmarks()  # for ALT 
print(f"The start node is {start} and the end node is {goal}\n")
plotGraph(False, figureId = 1)

# Dijkstra Algorithm and plot path
start_time = time.time()
findPath()
end = time.time()
print(f"Elapsed Time for Dijkstra is {end-start_time}\n")
connectPath()
plotGraph(True, figureId = 2)

# A* Algorithm and plot path
start_time = time.time()
findPathAStar()
end = time.time()
print(f"\nElapsed Time for A* is {end-start_time}\n")
plotGraph(True, figureId = 3)

# ALT Algorithm and plot path
start_time = time.time()
findPathALT()
end = time.time()
print(f"\nElapsed Time for ALT is {end-start_time}\n")
plotGraph(True, figureId=4)

# display all plots
plt.show(block=True)