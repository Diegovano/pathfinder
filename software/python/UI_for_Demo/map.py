from flask import Flask, render_template, request, jsonify
import folium
from folium.plugins import MarkerCluster, MiniMap
import networkx as nx 
import osmnx as ox
import json

coordinates_array = []
processing = False

app = Flask(__name__)

def process_coordinates(coordinates):
    global processing
    processing = True
    print(coordinates)
    processing = False

def create_map():
    folium_map = folium.Map(location=[51.4988, -0.1749], zoom_start=14)
    folium_map.add_child(folium.ClickForMarker(popup='Potential Location'))

    # Save the map to an HTML file
    folium_map.save('templates/map.html')


def formatData():
    with open('nodes.geojson') as f:
        nodes_data = json.load(f)

    with open('edges.geojson') as f:
        edges_data = json.load(f)   

    # Extract nodes and edges from the geojson data
    nodes_list = nodes_data['features']
    edges_list = edges_data['features']

    # Create a mapping from node IDs to sequential indices
    node_id_to_index = {node['properties']['osmid']: i for i, node in enumerate(nodes_list)}

    # Prepare the adjacency list using the sequential indices
    adj_list = {i: [] for i in range(len(nodes_list))}

    for edge in edges_list:
        start_node_id = edge['properties']['u']
        end_node_id = edge['properties']['v']
        weight = edge['properties']['length']
        
        start_node_index = node_id_to_index[start_node_id]
        end_node_index = node_id_to_index[end_node_id]
        
        adj_list[start_node_index].append((end_node_index, weight))

    adj_list_formatted = {"adjList": []}

    for start_node_index, connections in adj_list.items():
        start_node = nodes_list[start_node_index]
        start_coord = start_node['geometry']['coordinates']
        connection_data = {
            "coords": start_coord,
            "neighbours": []
        }

        for end_node_index, weight in connections:
            end_node = nodes_list[end_node_index]
            end_coord = end_node['geometry']['coordinates']
            connection_data["neighbours"].append({
                "index": end_node_index,
                "length": weight,
                "coords": end_coord
            })
        
        adj_list_formatted["adjList"].append(connection_data)

    adj_list_json = json.dumps(adj_list_formatted, indent=2)

    with open('adj_list.json', 'w') as f:
        f.write(adj_list_json)

    return node_id_to_index, adj_list_json

@app.route('/')
def index():
    # Serve the existing HTML file
    return render_template('map.html')

@app.route('/pathfind', methods=['POST'])
def pathfind():
    lat1 = request.json['lat1']
    lng1 = request.json['lng1']
    lat2 = request.json['lat2']
    lng2 = request.json['lng2']
    algorithm = request.json['algorithm']

    # Step 1: Get the map data and center it around the start coordinates
    G = ox.graph_from_point((lat1, lng1), dist=2000, network_type='drive', simplify=True, retain_all=False)
    
    # Step 2: Get the nearest nodes to the start and end coordinates
    orig_node = ox.nearest_nodes(G, X=lng1, Y=lat1)
    dest_node = ox.nearest_nodes(G, X=lng2, Y=lat2)

    if orig_node == dest_node:
        return jsonify({'error': 'The start and end nodes are the same. Please choose different locations.'}), 400

    # Step 3: Project the graph 
    G = ox.project_graph(G)

    # Step 4: process the graph data
    nodes, edges = ox.graph_to_gdfs(G, fill_edge_geometry=True)
    for col in edges.columns:
        if edges[col].dtype == 'object':
            edges[col] = edges[col].apply(lambda x: str(x) if isinstance(x, list) else x)

    nodes.to_file('nodes.geojson', driver='GeoJSON')
    edges.to_file('edges.geojson', driver='GeoJSON')

    # Step 5: Convert the data to suitable format 
    osmid_id_dict, formattedData = formatData()

    # Step 6: Calculate the shortest path
    if algorithm == 'Dijkstra':
        print('Dijkstra')
    elif algorithm == 'A*':
        print('A*')
    elif algorithm == 'Delta Stepping':
        print('Delta Stepping')
    else:
        return jsonify({'error': 'Unsupported algorithm'}), 400
    
    total_distance = 0
    total_time = 0
    response = {
        'status': 'success',
        'total_distance': total_distance,
        'total_time': total_time,
        'info': f'Path from ({lat1}, {lng1}) to ({lat2}, {lng2}) found, using algorithm {algorithm}'
    }

    return jsonify(response), 200

@app.route('/post', methods=['POST'])
def post():
    global processing

    if processing:
        return jsonify({'status': 'error', 'message': 'Processing in progress'}), 400

    coordinates = request.json['coordinates']
    
    if len(coordinates_array) >= 2:
        coordinates_array.clear()
        
    coordinates_array.append(coordinates)
    
    return jsonify({'status': 'success'}), 200

if __name__ == "__main__":
    # Generate the map once when the server starts
    #create_map()
    app.run(debug=True)
