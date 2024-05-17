import osmnx as ox
import geopandas as gpd
import pandas as pd 
import folium 
from folium.plugins import MarkerCluster, MiniMap
import networkx as nx 
import time
import heapq

# HOW TO USE AT THE MOMENT 
# RUN THE CODE AND THEN GENERATE A MAP 
# ENTER THE LAT LONG OF THE LOCATION YOU WANT 
# OPEN THE map.html FILE (GO LIVE ON VSCODE) AND CLICK ON ANY TWO NODES 
# COPY THE NODE ID INTO EITHER SOURCE OR TARGET 
# RERUN THE SCRIPT AND TYPE ALG TO TIME THE ALGORITHMS 

# CURRENTLY ONLY IMPLEMENTED DIJKSTRA (+ LIBRARY VERSION)

# Node class 
class Node:
    def __init__(self, osmid, x, y, geometry, neighbours, isLandmark=False):
        self.osmid = osmid
        self.x = x
        self.y = y
        self.geometry = geometry
        self.neighbours = neighbours
        self.isLandmark = isLandmark

    def get(self, attr_name, default=None):
        # Check if the requested attribute is one of the basic attributes
        if attr_name in ['osmid', 'x', 'y', 'geometry', 'neighbours']:
            return getattr(self, attr_name, default)
        # Otherwise, return from custom attributes
        return None

    def __repr__(self):
        return f"Node(osmid={self.osmid}, x={self.x}, y={self.y}, geometry={self.geometry}, neighbours={self.neighbours})"


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
            popup=f"Node {idx}",
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


# uses networkx to find the shortest path 
def benchmarkAlgo(graph, source, target):
    # graph is a MultiDiGraph which is needed for the nx.shortest_path
    # https://networkx.org/documentation/stable/reference/classes/multidigraph.html
    start_time = time.time()
    shortest_path = nx.shortest_path(graph, source=source, target=target, weight='length')
    end_time = time.time()
    return shortest_path, (end_time - start_time)


# function for Dijkstra algorithm 
def dijkstra(graph, source, target):
    node_dict = {}
    edge_dict = {}

    # create Node objects and append to the dictionary 
    for node in graph.nodes:
        current_node = graph.nodes[node]
        x = current_node.get('x', None)
        y = current_node.get('y', None)
        geometry = current_node.get('geometry', None)
        osmid = current_node.get('osmid', None)
        neighbours = list(graph.neighbors(node))
        node_obj = Node(osmid, x, y, geometry, neighbours)
        node_dict[node] = node_obj

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

    start_time = time.time()
    
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
    return path, (end_time - start_time)


# function for the user to select an algorithm to test
def algorithmSelection(choice):
    # read the nodes and the edges from the map 
    nodes = gpd.read_file('nodes.geojson')
    edges = gpd.read_file('edges.geojson')

    # default source and target for now 
    source_node = 6682140872
    target_node = 248926515

    # ensure edges have a MultiIndex for (u, v, key)
    if not isinstance(edges.index, pd.MultiIndex):
        edges.set_index(['u', 'v', 'key'], inplace=True)

    graph = ox.graph_from_gdfs(nodes, edges)
    
    if choice == 1:
        path, time_elapsed = dijkstra(graph, source_node, target_node)
        print(f"Shortest path using Dijkstra: {path}\nTime Elapsed: {time_elapsed}")
    elif choice == 2:
        pass
    elif choice == 3:
        pass
    else:
        # default to benchmark pathfinder
        shrt_path, time_elapsed = benchmarkAlgo(graph, source_node, target_node)
        print(f"Shortest path: {shrt_path}\nTime elapsed: {time_elapsed}")


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
        print(" 4. NetworkX (library)\n")
        algo_choice = int(input("Please select an algorithm to test: "))
        algorithmSelection(algo_choice)
    else:
        continue