# required imports
import matplotlib.pyplot as plt
import random
import math
from queue import PriorityQueue 
import time 

plt.ion()

# Node class with:
#             co-ordinates,
#             visited flag,
#             current shortest distance to this node,
#             index of the last node visited before this one

# Node class 
class Node:
    def __init__(self, x, y, visited=False, search_dist=999, search_last=999, isLandmark=False):
        self.x = x
        self.y = y
        self.visited = visited
        self.search_dist = search_dist
        self.search_last = search_last
        self.isLandmark = isLandmark

    def distance_to(self, other):
        return math.sqrt((self.x - other.x)**2 + (self.y - other.y)**2)


# function to generate the graph 
def generateGraph(size):
    
    start = None
    goal  = None
    nodes = []
    edges = []
    dist_matrix = []

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

    print(f"The start node is node with index {start} and the goal node has index {goal}")
    return nodes, dist_matrix, edges, start, goal


# function for heuristic calculation for A*
def heuristic(n1, n2):
    return math.sqrt(((n1.x - n2.x) ** 2) + ((n2.y - n1.y) ** 2))
    #return abs(n1.x - n2.x) + abs(n1.y - n2.y)


# pathfinder for A* algorithm 
def findPathAStar(nodes, dist_matrix, start, goal):
    pq = PriorityQueue()                                                       # has the format of (f_score, node)
    pq.put((0,start))                                                          # start of the queue is the start node
    came_from = {start: None}                                                  # start a dictionary of where the node came from
    g_score   = {start: 0}                                                     # start a dictionary of the gscore of the node
    f_score   = {start: heuristic(nodes[start], nodes[goal])}                  # start a dictionary of the fscore of the node
    closed_set = set()                                                         # set of all the closed nodes 
    visited_nodes = set()                                                      # set of visited nodes
    
    # whilst the queue is not empty 
    while not pq.empty():
        _, current = pq.get()        # current is now the node with the lowest f score 
        
        if current in closed_set:    # if current has been explored already then skip and dequeue next
            continue
        closed_set.add(current)      # if current not explored since we are now exploring mark as explored
        visited_nodes.add(current)   # add current node to visited nodes
        
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
            plotGraph(True, 2, "A*", edges, nodes, solution, start, goal, time_taken, visited_nodes)
            return solution

        for i in range(len(nodes)):
            if dist_matrix[current][i] == 999 or i in closed_set:
                # if the neighbour is distance inf or neighbour already explored skip it
                continue
            
            # gscore of this node is the gscore of current + distance to neighbour 
            temp_g_score = g_score[current] + dist_matrix[current][i]

            # if the neighbour is not in the g_score dict or the calculated g_score is less than 
            # its current g_score 
            if i not in g_score or temp_g_score < g_score[i]: 
                came_from[i] = current                                # the neighbour came from current node
                g_score[i]   = temp_g_score                           # store g_score
                f_score[i]   = temp_g_score + heuristic(nodes[i], nodes[goal])   # calc and store f_score
                pq.put((f_score[i], i))                               # add to priority queue to be explored 
    
    print("No solution to the ALT Algorithm")
    return None


# pathefinder for Dijkstra algorithm 
def findPathDijkstra(nodes, dist_matrix, start, goal):
    visiting_node = start
    visiting_dist = 0

    size = len(nodes)

    search_prios = [999] * size
    search_prios[start] = 0

    for _ in range(size):
        visiting_node = search_prios.index(min(search_prios))
        visiting_dist = nodes[visiting_node].search_dist

        if visiting_node == goal or nodes[visiting_node].visited:
            break

        for i in range(size):
            if not nodes[i].visited:
                new_dist = visiting_dist + dist_matrix[visiting_node][i] 

                if new_dist < nodes[i].search_dist:
                    nodes[i].search_dist = new_dist
                    search_prios[i] = new_dist
                    nodes[i].search_last = visiting_node

        nodes[visiting_node].visited = True
        search_prios[visiting_node] = 999
    
    solution = []
    current_node = goal
    while current_node != start and current_node != 999:
        prev_node = nodes[current_node].search_last
        solution.append([prev_node, current_node])
        current_node = prev_node

    solution.reverse()
    print(f"The result from Dijkstra is {solution}")
    return solution 


# function to get distance (TODO: move to Node class) 
def getDistance(n1, n2):
    return math.sqrt(((n1.x - n2.x) ** 2) + ((n2.y - n1.y) ** 2)) 


# function to set the landmarks (random method)
def setLandmarksRandom(nodes, num_landmarks):
    landmarks = []
    max_attempts = len(nodes) * 2 
    attempts = 0
    while len(landmarks) < num_landmarks and attempts < max_attempts:
        index = random.randint(0, len(nodes) - 1)
        if not nodes[index].isLandmark:
            nodes[index].isLandmark = True
            landmarks.append(index)
        attempts += 1
    return landmarks


# function to set the landmarks (furthest away method)
def setLandmarksFurthest(nodes, num_landmarks):
    if num_landmarks > len(nodes):
        raise ValueError("Number of landmarks cannot exceed number of nodes")

    landmarks = []                                          # array holding the indices of the landmarks
    initial_landmark = random.randint(0, len(nodes) - 1)
    landmarks.append(initial_landmark)                      # select a random node to be the first node
    nodes[initial_landmark].isLandmark = True

    while len(landmarks) < num_landmarks:
        max_dist = -1
        best_candidate = None

        for idx, node in enumerate(nodes):
            if not node.isLandmark:
                min_dist_to_landmarks = min(getDistance(node, nodes[landmarks[i]]) for i in range(len(landmarks)))
                if min_dist_to_landmarks > max_dist:
                    max_dist = min_dist_to_landmarks
                    best_candidate = idx

        if best_candidate is not None:
            nodes[best_candidate].isLandmark = True
            landmarks.append(best_candidate)

    return landmarks


# function to return a matrix of distances from nodes to landmarks           
def getDistanceToLandmarks(nodes, landmarks):
    # matrix which stores the distance from the a node and the landmark 
    # eg lm_dist_matrix[1,2] is the distance from the first node to the 2nd landmark 
    # lm_dist_matrix = [[0 for _ in range(num_landmarks)] for _ in range(size)]
    lm_dist_matrix = []
    for node in nodes:
        lm_distance_row = []
        for landmark in landmarks:
            lm_distance_row.append(math.sqrt((node.x - nodes[landmark].x) ** 2 + (node.y - nodes[landmark].y) ** 2))
        lm_dist_matrix.append(lm_distance_row)
    return lm_dist_matrix


# function for Heuristic calculation for ALT algorithm 
def altHeuristic(n_index, goal_index, lm_dist_matrix, landmarks):
    h_estimate = 0
    for l in range(len(landmarks)):
        h_estimate = max(h_estimate, abs(lm_dist_matrix[goal_index][l] - lm_dist_matrix[n_index][l]))
    return h_estimate


# pathfinder for ALT algorithm 
def findPathALT(nodes, dist_matrix, start, goal, landmarks, lm_dist_matrix):
    pq = PriorityQueue()
    pq.put((0, start))
    came_from = {start: None}
    g_score   = {start: 0}
    f_score   = {start: altHeuristic(start, goal, lm_dist_matrix, landmarks)}

    closed_set = set()

    while not pq.empty():
        _, current = pq.get()

        if current in closed_set:
            continue
        closed_set.add(current)

        # if current node is the goal then we have a solution 
        if current == goal:
            solution = []  
            while current in came_from:
                prev = came_from[current]
                if prev is not None:
                    solution.append([prev, current])
                current = prev
            solution.reverse()
            print(f"The result from ALT is {solution}")
            return solution
            
        
        for neighbour in range(len(nodes)):
            if dist_matrix[current][neighbour] == 999 or neighbour in closed_set:
                continue

            temp_g_score = g_score[current] + dist_matrix[current][neighbour]

            if neighbour not in g_score or temp_g_score < g_score[neighbour]:
                came_from[neighbour] = current
                g_score[neighbour]   = temp_g_score
                f_score[neighbour]   = temp_g_score + altHeuristic(neighbour, goal, lm_dist_matrix, landmarks)
                pq.put((f_score[neighbour], neighbour))
    
    print("No solution to the ALT Algorithm")
    return None 


# function to ploth the graph
def plotGraph(showPath, figureId, algorithm, edges, nodes, solution, start, goal, time_taken, visited_nodes=None):
    plt.figure(figureId, figsize=(5, 5))
    plt.clf()

    for edge in edges:
        x_values = [nodes[edge[0]].x, nodes[edge[1]].x]
        y_values = [nodes[edge[0]].y, nodes[edge[1]].y]
        plt.plot(x_values, y_values, 'gray', lw=0.5)

    for idx, node in enumerate(nodes):
        if idx == start:
            plt.plot(node.x, node.y, 'go', markersize=8, label='Start')
        elif idx == goal:
            plt.plot(node.x, node.y, 'ro', markersize=8, label='Goal')
        elif node.isLandmark:
            plt.plot(node.x, node.y, 'ko', markersize=6, label='Landmark' if idx == 0 else "")
        else:
            plt.plot(node.x, node.y, 'bo', markersize=4)

        if visited_nodes and idx in visited_nodes:
            plt.plot(node.x, node.y, 'yo', markersize=6)
        
        # Annotate the node with its index
        plt.text(node.x, node.y, str(idx), fontsize=8, verticalalignment='bottom', horizontalalignment='right')
        

    if showPath and solution:
        for path in solution:
            x_values = [nodes[path[0]].x, nodes[path[1]].x]
            y_values = [nodes[path[0]].y, nodes[path[1]].y]
            plt.plot(x_values, y_values, 'cyan')

    plt.xlabel("X")
    plt.ylabel("Y")
    plt.title(f"Graph of {algorithm} with {'path' if showPath else 'no path'}\nTime taken: {time_taken:.10f} seconds")
    plt.legend()
    plt.show()


# main function for alt algorithm 
def alt_algo(nodes, dist_matrix, edges, start, goal):
    resetLandmarks(nodes)
    
    num_landmarks = int(input("Please enter the number of landmarks: "))
    
    landmark_selection = input("Which landmark selection do you want to try? (rand = random, far = furthest) ")
    if landmark_selection == "rand":
        landmarks = setLandmarksRandom(nodes, num_landmarks)
    else:
        landmarks = setLandmarksFurthest(nodes, num_landmarks)

    lm_dist_matrix = getDistanceToLandmarks(nodes, landmarks)
    start_time = time.time()
    path = findPathALT(nodes, dist_matrix, start, goal, landmarks, lm_dist_matrix)
    end_time = time.time()
    time_taken = end_time - start_time
    print(f"The ALT algorithm took {time_taken}")
    plotGraph(True, 1, "ALT", edges, nodes, path, start, goal, time_taken)
    plt.show()


# main function for Dijkstra algorithm 
def dijkstra(nodes, dist_matrix, edges, start, goal):
    start_time = time.time()
    path = findPathDijkstra(nodes, dist_matrix, start, goal)
    end_time = time.time()
    time_taken = end_time - start_time
    print(f"The Dijkstra algorithm took {time_taken}")
    plotGraph(True, 3, "Dijkstra", edges, nodes, path, start, goal, time_taken)
    plt.show()


# main function for A* algorithm
def a_star(nodes, dist_matrix, edges, start, goal):
    start_time = time.time()
    path = findPathAStar(nodes, dist_matrix, start, goal)
    end_time = time.time()
    time_taken = end_time - start_time
    print(f"The A* algorithm took {time_taken}")
    plotGraph(True, 2, "A*", edges, nodes, path, start, goal, time_taken)
    plt.show()


# main function for bidirectional A*
def bidirect_a_star():
    return 0


# function to reset the landmarks (in case we want to change graph whilst program running)
def resetLandmarks(nodes):
    for node in nodes:
        node.isLandmark = False

# main call 
try:
    graph_gen = False
    proceed = False
    while True:

        if(graph_gen == False):
            num_nodes = int(input("How many nodes do you want (>10)? "))
            while not proceed:
                nodes, dist_matrix, edges, start, goal = generateGraph(num_nodes)
                plotGraph(False, 4, "", edges, nodes, [], start, goal, 0)
                if input("Do you want to proceed with the given map? (y - Yes, n - No): ") == 'y':
                    proceed = True
                else:
                    plt.close()
                    proceed = False
                graph_gen = True

        print("\nSelect an Algorithm to Test:")
        print("1. Dijkstra")
        print("2. A* Algorithm")
        print("3. ALT Algorithm")
        print("4. New Graph")
        print("5. Exit\n")

        choice = int(input("Please enter your choice (1-5): "))
        if choice == 1:
            dijkstra(nodes, dist_matrix, edges, start, goal)
        elif choice == 2:
            a_star(nodes, dist_matrix, edges, start, goal)
        elif choice == 3:
            alt_algo(nodes, dist_matrix, edges, start, goal)
        elif choice == 4:
            graph_gen = False
            proceed = False
            continue
        else:
            plt.close()
            break

except KeyboardInterrupt:
    plt.close()
    print("Program Stopped By User")
