import osmnx as ox
import geopandas as gpd
import pandas as pd 
import folium 
from folium.plugins import MarkerCluster, MiniMap
import networkx as nx 
import time
import heapq
import math
import random

# HOW TO USE AT THE MOMENT 
# RUN THE CODE AND THEN GENERATE A MAP 
# ENTER THE LAT LONG OF THE LOCATION YOU WANT 
# OPEN THE map.html FILE (GO LIVE ON VSCODE) AND CLICK ON ANY TWO NODES 
# COPY THE NODE ID INTO EITHER SOURCE OR TARGET 
# RERUN THE SCRIPT AND TYPE ALG TO TIME THE ALGORITHMS 

# Node class 
class Node:
    def __init__(self, osmid, x, y, neighbours, isLandmark=False):
        self.osmid = osmid
        self.x = x
        self.y = y
        self.neighbours = neighbours
        self.isLandmark = isLandmark

    def get(self, attr_name, default=None):
        # Check if the requested attribute is one of the basic attributes
        if attr_name in ['osmid', 'x', 'y', 'neighbours']:
            return getattr(self, attr_name, default)
        # Otherwise, return from custom attributes
        return None
    
    # def getDistanceTo(self, target):
    #     return ox.distance.euclidean(self.y, self.x, target.y, target.x)
        #return math.sqrt(((self.x - target.x) ** 2) + ((target.y - self.y) ** 2))

    def getDistanceTo(self, target):
        # Haversine formula to calculate distance between two latitude/longitude points
        R = 6371000 
        dLat = math.radians(target.y - self.y)
        dLon = math.radians(target.x - self.x)
        a = (
            math.sin(dLat / 2) ** 2 +
            math.cos(math.radians(self.y)) * math.cos(math.radians(target.y)) * math.sin(dLon / 2) ** 2
        )
        c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
        distance = R * c  
        return distance

    def __repr__(self):
        return f"Node(osmid={self.osmid}, x={self.x}, y={self.y}, neighbours={self.neighbours}, isLandmark={self.isLandmark})"


# Edge class
class Edge:
    def __init__(self, nodes, osmid, name, maxspeed, highway, access, oneway, length, geometry):
        self.nodes = nodes          # List of nodes representing the polyline
        self.osmid = osmid
        self.name = name
        self.maxspeed = maxspeed
        self.highway = highway
        self.access = access
        self.oneway = oneway
        self.length = length
        self.geometry = geometry

    def get(self, attr_name, default=None):
        # Check if the requested attribute is one of the basic attributes
        if attr_name in ['osmid', 'name', 'maxspeed', 'highway', 'access', 'oneway', 'length', 'geometry']:
            return getattr(self, attr_name, default)
        # Otherwise, return from custom attributes
        return None

    def __repr__(self):
        return f"Edge(osmid={self.osmid}, name={self.name}, length={self.length}, geometry={self.geometry})"


# function to generate graph 
def generateGraph(center, radius, network_type, filename):

    map = folium.Map(location=center, zoom_start=13)

    # add a marker cluster to manage markers
    marker_cluster = MarkerCluster().add_to(map)
    MiniMap().add_to(map)

    # add a click event so we can see the latitude and longitude of the click 
    map.add_child(folium.LatLngPopup())

    # create a graph of the map with given radius and network type 
    G = ox.graph_from_point(tuple(center), dist=radius, network_type=network_type, simplify=True)

    # convert multidigraph to node and edge GeoDataFrame
    nodes, edges = ox.graph_to_gdfs(G, fill_edge_geometry=True) 

    print(f"The number of nodes is {len(nodes)} and the number of edges is {len(edges)}. The density is {nx.density(G)}.")

    folium.Circle(
        radius=radius,
        location=center,
        color='blue',
        fill=True,
        fill_color='blue',
        fill_opacity=0.1
    ).add_to(map)
   
    # Add nodes as markers
    for idx, node in nodes.iterrows():
        folium.Marker(
            location=[node['y'], node['x']],
            popup=f"Node {idx}\n",
            icon=folium.Icon(color='red', icon='info-sign')
        ).add_to(marker_cluster)
    
    # Save the map to an HTML file
    map.save(filename)

    # clean the edges GeoDataFrame - otherwise error (?)
    for col in edges.columns:
        if edges[col].dtype == 'object':
            edges[col] = edges[col].apply(lambda x: str(x) if isinstance(x, list) else x)
    
    nodes.to_file('nodes.geojson', driver='GeoJSON')
    edges.to_file('edges.geojson', driver='GeoJSON')


# function to add the path to the map 
def add_path_to_map(map, graph, nodes, path, filename):
    marker_cluster = MarkerCluster().add_to(map)

    for idx, node in nodes.iterrows():
        folium.Marker(
            location=[node['y'], node['x']],
            popup=f"Node {idx}\n",
            icon=folium.Icon(color='red', icon='info-sign')
        ).add_to(marker_cluster)
    
    nodes_dict = createDict(graph)[0]
    path_coordinates = [(nodes_dict[node_id].y, nodes_dict[node_id].x) for node_id in path]
    folium.PolyLine(
        path_coordinates, 
        color="blue", 
        weight=2.5, 
        opacity=1
    ).add_to(map)
    
    map.save(filename)

    
# function for the heuristic used in the benchmark algo 
def heuristic(node, target, node_dict):
    # Haversine formula to calculate the distance between two nodes
    R = 6371000 
    node_data = node_dict[node]
    target_data = node_dict[target]
    
    dLat = math.radians(target_data.y - node_data.y)
    dLon = math.radians(target_data.x - node_data.x)
    a = (
        math.sin(dLat / 2) ** 2 +
        math.cos(math.radians(node_data.y)) * math.cos(math.radians(target_data.y)) * math.sin(dLon / 2) ** 2
    )
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
    distance = R * c  
    return distance
    #return math.sqrt(((node_dict[node].x - node_dict[target].x) ** 2) + ((node_dict[target].y - node_dict[node].y) ** 2))


# uses networkx to find the shortest path 
def benchmarkAlgo(graph, source, target):
    # graph is a MultiDiGraph which is needed for the nx.shortest_path
    # https://networkx.org/documentation/stable/reference/classes/multidigraph.html
    node_dict = createDict(graph)[0]
    selection = int(input("Would you like 1) Dijkstra or 2) A*? "))
    if selection == 1:
        start_time = time.time()
        shortest_path = nx.shortest_path(graph, source=source, target=target, weight='length')
        end_time = time.time()
    elif selection == 2:
        start_time = time.time()
        shortest_path = nx.astar_path(graph, source=source, target=target, heuristic=lambda u, v: heuristic(u, v, node_dict), weight='length')
        end_time = time.time()
    return shortest_path, (end_time - start_time)


# function to create the dictionaries for the nodes and edges 
# key of node dictionary is node number then info about node
# key fo edge dictonary is (start node, end node) of the egde then info about the edge 
def createDict(graph):
    
    node_dict = {}
    edge_dict = {}

    #create the node object and append to a node dictionary 
    for _, node_data in graph.nodes(data=True):
        if node_data:
            osmid = node_data['osmid']
            x = node_data['x']
            y = node_data['y']
            #neighbours = list(graph.neighbors(osmid))
            neighbours = list(graph.successors(osmid))
            node_obj = Node(osmid, x, y, neighbours)
            node_dict[osmid] = node_obj
        else:
            continue

    # create Edge objects and append to the dictionary 
    for u, v, _, data in graph.edges(keys=True, data=True):
        osmid = data.get('osmid', None)
        name = data.get('name', None)
        maxspeed = data.get('maxspeed', None)
        highway = data.get('highway', None)
        access = data.get('access', None)
        oneway = data.get('oneway', None)
        length = data.get('length', None)
        geometry = data.get('geometry', None)
        
        # Extract the nodes forming the polyline of the edge
        if geometry:
            edge_nodes = list(geometry.coords)
        else:
            edge_nodes = [(u, v)]  # set to just the start and end nodes if no geometry
        
        edge_obj = Edge(edge_nodes, osmid, name, maxspeed, highway, access, oneway, length, geometry)
        edge_dict[(u, v)] = edge_obj
        if not oneway: 
            edge_dict[(v, u)] = edge_obj

    return node_dict, edge_dict


# function for A* Algorithm 
def a_star(graph, source, target):

    node_dict, edge_dict = createDict(graph)

    # dictionary to store the g value and f value of the node 
    g_value   = {source: 0}
    f_value   = {source: node_dict[source].getDistanceTo(node_dict[target])}
    came_from = {source: None}

    closed_nodes = set()

    start_time = time.time()

    for _ in range(10):

        queue = []
        heapq.heappush(queue, (0,source))

        while queue:

            _, current_node = heapq.heappop(queue)

            if current_node in closed_nodes:
                continue

            if current_node == target:
                break

            closed_nodes.add(current_node) 

            # for neighbour in node_dict[current_node].neighbours:
            for neighbour in node_dict[current_node].neighbours:
                if neighbour in closed_nodes:
                    continue
                edge = edge_dict.get((current_node, neighbour))
                if edge is not None:
                    temp_g_score = g_value[current_node] + edge.length
                    if neighbour not in g_value or temp_g_score < g_value[neighbour]:
                        came_from[neighbour] = current_node
                        g_value[neighbour]   = temp_g_score
                        f_value[neighbour]   = temp_g_score + node_dict[current_node].getDistanceTo(node_dict[target])
                        heapq.heappush(queue, (f_value[neighbour], neighbour))

        path = []
        current_node = target
        while came_from[current_node] is not None:
            path.append(current_node)
            current_node = came_from[current_node]
        path.append(source)
        path.reverse()
    
    end_time = time.time()
    return path, (end_time - start_time)/10


# function for bidirectional A*
def a_star_bi(graph, source, target):
    pass


# function for Dijkstra algorithm 
def dijkstra(graph, source, target):

    node_dict, edge_dict = createDict(graph)

    start_time = time.time()
    
    for _ in range(10):
        queue = []                                                     # priority queue to store not to be explored
        heapq.heappush(queue, (0, source))                             # push to the queue the source node
        distances = {node: float('inf') for node in node_dict}         # dictionary to store the shortest distance from source to each node 
        distances[source] = 0
        previous_nodes = {node: None for node in node_dict}            # dictionary to store the previous node in the optimal path for each node

        while queue:                                                   # whilst the queue is not empty 
            current_distance, current_node = heapq.heappop(queue)      # get the current distanec and the current node from queue 

            if current_node == target:                                 # break if current is target (found)
                break

            for neighbour in node_dict[current_node].neighbours:       # for every neighbour of the current node
                edge = edge_dict.get((current_node, neighbour))        # get the edge connecting the two
                if edge is not None:                                   # if there is in fact an edge 
                    edge_length = edge.length                          # get the length of the edge 
                    distance = current_distance + edge_length          # calcualte the new distance from the source 
                    if distance < distances[neighbour]:                # if distance is less than current known distance from neighbour
                        distances[neighbour] = distance                # update the distance 
                        previous_nodes[neighbour] = current_node       # add to our optimal path previous nodes dict     
                        heapq.heappush(queue, (distance, neighbour))   # push the neighbour to queue to be explored

        path = []                                                      # empty path 
        current_node = target                                          # set current node to target
        while previous_nodes[current_node] is not None:                # while there are still previous nodes
            path.append(current_node)                                  # add the current node to the path 
            current_node = previous_nodes[current_node]                # set the new current node to the next node
        path.append(source)                                            # appened to the source to the path 
        path.reverse()                                                 # reverse the list (source to target)
    
    end_time = time.time()
    return path, (end_time - start_time)/10


# function for Bidirectional Dijkstra 
def dijkstra_bi(graph, source, target):
    
    node_dict, edge_dict = createDict(graph)

    start_time = time.time()

    path = []
    end_time = time.time()
    return path, (end_time - start_time) / 10


# function for selecting the landmarks - furthest logic (can implement random as well)
def selectLandmarks(nodes, num_landmarks):
    if num_landmarks > len(nodes):
        raise ValueError("Number of landmarks cannot exceed number of nodes")
    
    landmarks = []  # stores the node objects that are now landmarks 

    # randomise the first node to be a landmark
    keys = list(nodes.keys())
    random_index = random.randint(0, len(keys) - 1)
    nodes[keys[random_index]].isLandmark = True
    landmarks.append(nodes[keys[random_index]])
    
    landmark_count = 1

    # loop through all other nodes and find the node that is furthest away from the current node
    while landmark_count < num_landmarks:
        max_dist = -1
        best_candidate = None

        for idx in range(len(keys)):
            if not nodes[keys[idx]].isLandmark:
                min_dist_to_landmark = min(nodes[keys[idx]].getDistanceTo(landmarks[i]) for i in range(len(landmarks)))
                if min_dist_to_landmark > max_dist:
                    max_dist = min_dist_to_landmark
                    best_candidate = idx
        
        if best_candidate is not None:
            nodes[keys[best_candidate]].isLandmark = True
            landmarks.append(nodes[keys[best_candidate]])
            landmark_count += 1

    return nodes


# function for ALT algorithm 
def alt(graph, source, target, landmarks=None):
    node_dict, edge_dict = createDict(graph)

    if landmarks is None:
        num_lm = int(input("How many landmarks would you like? "))
    else:
        num_lm = landmarks

    # need to select some landmarks 
    node_dict = selectLandmarks(node_dict, num_lm)
    
    # then need to find the distance of each node from each landmark 
    landmark_distances = {lm: {} for lm in node_dict if node_dict[lm].isLandmark}
    for landmark in landmark_distances:
        for node in node_dict:
            landmark_distances[landmark][node] = heuristic(node, landmark, node_dict)
            # cannot use the one below since some nodes are not reachable 
            #landmark_distances[landmark][node] = nx.shortest_path_length(graph, source=landmark, target=node, weight='length')

    def alt_heuristic(node, target):
        return max(abs(landmark_distances[landmark][target] - landmark_distances[landmark][node]) for landmark in landmark_distances)

    # dictionary to store the g value and f value of the node 
    g_value   = {source: 0}
    f_value   = {source: alt_heuristic(source, target)}
    came_from = {source: None}

    closed_nodes = set()

    start_time = time.time()

    for _ in range(10):

        queue = []
        heapq.heappush(queue, (0,source))

        while queue:

            _, current_node = heapq.heappop(queue)

            if current_node in closed_nodes:
                continue

            if current_node == target:
                break

            closed_nodes.add(current_node) 

            # for neighbour in node_dict[current_node].neighbours:
            for neighbour in node_dict[current_node].neighbours:
                if neighbour in closed_nodes:
                    continue
                edge = edge_dict.get((current_node, neighbour))
                if edge is not None:
                    temp_g_score = g_value[current_node] + edge.length
                    if neighbour not in g_value or temp_g_score < g_value[neighbour]:
                        came_from[neighbour] = current_node
                        g_value[neighbour]   = temp_g_score
                        f_value[neighbour]   = temp_g_score + alt_heuristic(neighbour, target)
                        heapq.heappush(queue, (f_value[neighbour], neighbour))

        path = []
        current_node = target
        while came_from[current_node] is not None:
            path.append(current_node)
            current_node = came_from[current_node]
        path.append(source)
        path.reverse()
    
    end_time = time.time()

    return path, (end_time - start_time)/10


# function to return the distance of the found path
def returnDistance(path, graph):
    _, edges_dict = createDict(graph)
    distance = 0

    for i in range(len(path)-1):
        distance += edges_dict[((path[i], path[i+1]))].length
    return distance 


# if we were properly doing this then we cant use bidirectional 
# algorithms since they would be on the other side of the road
# function for bidirectional algorithms
def bidirectional(graph, source, target):

    print("\nBidirectional Algorithm Selection: ")
    print(" 1. Dijkstra")
    print(" 2. A* ")
    algo_choice = int(input("Please select a bidirectional algorithm to test: "))

    if algo_choice == 1:
        path, time_elapsed = dijkstra_bi(graph, source, target)
        return path, time_elapsed, "bidirectional Dijkstra"
    elif algo_choice == 2:
        path, time_elapsed = a_star_bi(graph, source, target)
        return path, time_elapsed, "bidirectional A*"


# function to select random nodes
def select_random_nodes(graph, num_pairs=5):
    valid_nodes = [node for node in graph.nodes() if list(graph.neighbors(node))]
    random_pairs = [(random.choice(valid_nodes), random.choice(valid_nodes)) for _ in range(num_pairs)]
    return random_pairs

# function used to test all algorithms
def test_all_algorithms():
    # generate the graph 
    radius = [250, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000]
    center = [51.4988, -0.1749]
    type_net = "drive"

    num_pairs = 5

    results = []

    for rad in radius:
        print(f"Generating map with radius {rad}")
        
        G = ox.graph_from_point(tuple(center), dist=rad, network_type=type_net, simplify=True)
        nodes, edges = ox.graph_to_gdfs(G, fill_edge_geometry=True)
        
        for col in edges.columns:
            if edges[col].dtype == 'object':
                edges[col] = edges[col].apply(lambda x: str(x) if isinstance(x, list) else x)
        
        nodes.to_file('nodes.geojson', driver='GeoJSON')
        edges.to_file('edges.geojson', driver='GeoJSON')

        nodes = gpd.read_file('nodes.geojson')
        edges = gpd.read_file('edges.geojson')

        if not isinstance(edges.index, pd.MultiIndex):
            edges.set_index(['u', 'v', 'key'], inplace=True)

        graph = ox.graph_from_gdfs(nodes, edges)

        print("Graph Generated \n")

        num_nodes = len(nodes)
        num_edges = len(edges)
        density = nx.density(graph)
        random_pairs = select_random_nodes(graph, num_pairs)

        for source, target in random_pairs:
            while True:
                try:
                    dijkstra_path, dijkstra_time = dijkstra(graph, source, target)
                    astar_path, astar_time = a_star(graph, source, target)
                    #alt_path, alt_time = alt(graph, source, target, min(int(len(nodes)/50), 70))
                    dijkstra_distance = returnDistance(dijkstra_path, graph)
                    astar_distance = returnDistance(astar_path, graph)
                    break
                except:
                    source, target = random.choice(select_random_nodes(graph,1))

            result = {
                'radius': rad,
                'density': density,
                'num_nodes': num_nodes,
                'num_edges': num_edges,
                'source': source,
                'target': target,
                'dijkstra_distance': dijkstra_distance,
                'astar_distance': astar_distance,
                'dijkstra_time': dijkstra_time,
                'astar_time': astar_time,
            }

            with open("test_results.txt", "a") as file:
                file.write(str(result) + ",\n")

            results.append(result)
    
    return results

# function for the user to select an algorithm to test
def algorithmSelection(choice):
    # read the nodes and the edges from the map 
    print("\nReading Files....")
    nodes = gpd.read_file('nodes.geojson')
    edges = gpd.read_file('edges.geojson')
    print("Files Read\n")

    # default source and target for now 
    source_node = int(input("Please enter the OSMID of the source node: "))
    target_node = int(input("Please enter the OSMID of the target node: "))

    # ensure edges have a MultiIndex for (u, v, key)
    if not isinstance(edges.index, pd.MultiIndex):
        edges.set_index(['u', 'v', 'key'], inplace=True)

    graph = ox.graph_from_gdfs(nodes, edges)
    
    if choice == 1:
        path, time_elapsed = dijkstra(graph, source_node, target_node)
        print(f"Shortest path using Dijkstra: {path}\nTime Elapsed: {time_elapsed}")
    elif choice == 2:
        path, time_elapsed = a_star(graph, source_node, target_node)
        print(f"Shortest path using A*: {path}\nTime Elapsed: {time_elapsed}")
    elif choice == 3:
        path, time_elapsed = alt(graph, source_node, target_node)
        print(f"Shortest path using ALT: {path}\nTime Elapsed: {time_elapsed}")
    elif choice == 4:
        path, time_elapsed, method = bidirectional(graph, source_node, target_node)
        print(f"Shortest path using {method}: {path}\nTime Elapsed: {time_elapsed}")
    else:
        # default to benchmark pathfinder
        path, time_elapsed = benchmarkAlgo(graph, source_node, target_node)
        print(f"Shortest path: {path}\nTime elapsed: {time_elapsed}")
    
    try:
        print(f"The distance of the route is {returnDistance(path, graph)}m")
    except Exception as ex:
        print(f"Error with excpetion {ex}")
    
    # Load the previously generated map
    map = folium.Map(location=[nodes.iloc[0]['y'], nodes.iloc[0]['x']], zoom_start=13)
    add_path_to_map(map, graph, nodes, path, 'map.html')


# main loop
while True:
    choice = input("Would you like to generate a map (gen) or time an algorithm (alg)? ")
    if choice == "gen":
        lat = float(input("Enter the latitude of the center (default 51.4988): "))
        long = float(input("Enter the longitude of the center (default -0.1749): "))
        mapCenter = [lat, long] 
        radius = int(input("Enter the radius of the graph coverage: "))
        network_type = input("Place enter the network type (all, all_public, bike, drive, drive_service, walk): ")
        map_filename = 'map.html'
        generateGraph(mapCenter, radius, network_type, map_filename)
    elif choice == "alg":
        
        print("\nAlgorithm Selection: ")
        print(" 1. Dijkstra")
        print(" 2. A* ")
        print(" 3. ALT")
        print(" 4. Bidirectional Algorithms")
        print(" 5. NetworkX (library)")
        print(" 6. Testing\n")
        algo_choice = int(input("Please select an algorithm to test: "))
        if algo_choice == 6:
            print(test_all_algorithms())
        else:
            algorithmSelection(algo_choice)
    else:
        continue