from flask import Flask, render_template, request, jsonify
import folium
from folium.plugins import MarkerCluster, MiniMap
import networkx as nx 
import osmnx as ox

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

    print(f"The original node is {orig_node} and the destination node is {dest_node}.")

    if orig_node == dest_node:
        return jsonify({'error': 'The start and end nodes are the same. Please choose different locations.'}), 400

    # Step 3: Project the graph 
    G = ox.project_graph(G)
    
    # Step 4: Calculate the shortest path
    if algorithm == 'Dijkstra':
        shortest_path = nx.shortest_path(G, orig_node, dest_node, weight='length')
    elif algorithm == 'A*':
        shortest_path = nx.astar_path(G, orig_node, dest_node, weight='length')
    else:
        return jsonify({'error': 'Unsupported algorithm'}), 400

    # Step 5: Extract detailed street data for the path
    street_data = []
    for i, node in enumerate(shortest_path[:-1]):
        edge_data = G.get_edge_data(node, shortest_path[i + 1])[0]
        street_data.append({
            'from': node,
            'to': shortest_path[i + 1],
            'street_name': edge_data.get('name', 'Unnamed street'),
            'length': edge_data['length']
        })

    # Step 6: Calculate total distance
    total_distance = sum(edge['length'] for edge in street_data)

    response = {
        'status': 'success',
        'path': street_data,
        'total_distance': total_distance,
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
