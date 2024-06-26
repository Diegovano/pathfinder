from flask import Flask, render_template, request, jsonify
import folium
from folium.plugins import MarkerCluster, MiniMap
import networkx as nx 
import osmnx as ox
import json
import socket

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


def formatData(algorithm, start, end):

   with open('nodes.geojson') as f:
       nodes_data = json.load(f)
   with open('edges.geojson') as f:
       edges_data = json.load(f)   

   nodes_list = nodes_data['features']
   edges_list = edges_data['features']
   node_id_to_index = {node['properties']['osmid']: i for i, node in enumerate(nodes_list)}

   print("Start Osmid: ", start)
   print("End Osmid: ", end)

   adj_list = {i: [] for i in range(len(nodes_list))}

   for edge in edges_list:
       start_node_id = edge['properties']['u']
       end_node_id = edge['properties']['v']
       weight = edge['properties']['length']
       start_node_index = node_id_to_index[start_node_id]
       end_node_index = node_id_to_index[end_node_id]
       adj_list[start_node_index].append((end_node_index, weight))

   x_coordinates = []
   y_coordinates = []

   for start_node_index, connections in adj_list.items():
       coordinates = nodes_list[start_node_index]['geometry']['coordinates']
       x_coordinate = coordinates[0]
       y_coordinate = coordinates[1]
       x_coordinates.append(x_coordinate)
       y_coordinates.append(y_coordinate)


   adj_list_formatted = {
       "adjList": [[(end_node_index, length) for end_node_index, length in connections] for connections in adj_list.values()],
       "x": x_coordinates,
       "y": y_coordinates,
       "size": len(nodes_list),
       "start": node_id_to_index[start],
       "end": node_id_to_index[end],
       "avgCount": 100
   }

   adj_list_json = json.dumps(adj_list_formatted, indent=None, separators=(',', ':'),)

   with open('adj_list.json', 'w') as f:
       f.write(adj_list_json)

   return node_id_to_index, adj_list_json


def send_receive_esp32(json_data):
    HOST = "192.168.137.94"
    PORT = 80
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            s.sendall(bytes(str(json_data)+"\n\n", 'utf8'))
            print("Sent GRAPH")
            response = s.recv(2048).decode('utf8')
            print(response)
            data = json.loads(response)
    except Exception as e:
        print(f"An error occurred: {e}")
        data = None
    return data
    # data = '{"sht": [73, 108, 162, 71, 68, 63, 60, 61, 156, 112, 15, 113], "max_iterations":0,"iCache":2048, "dCache":2048,"sw_astar_time": 24435,"hybrid_astar_time": 3062,"sw_dj_time": 373,"hybrid_dj_time": 132,"hw_dj_time": 25,"delta_time": "inf","hybrid_delta_time": "inf","distance": 956.6,"note":""}'    
    # data = json.loads(data)
    # return data


@app.route('/')
def index():
    # Serve the existing HTML file
    return render_template('map.html')

@app.route('/pathfind', methods=['POST'])
def pathfind():
    centerlat = request.json['centerlat']
    centerlng = request.json['centerlng']
    lat1 = request.json['lat1']
    lng1 = request.json['lng1']
    lat2 = request.json['lat2']
    lng2 = request.json['lng2']
    algorithm = request.json['algorithm']

    distance = 800
    coordinates_array =  (centerlat, centerlng) # (51.4988, -0.1749) 
    G = ox.graph_from_point(coordinates_array, dist=distance, network_type='drive', simplify=True, retain_all=False)    
    orig_node = ox.nearest_nodes(G, X=lng1, Y=lat1)
    dest_node = ox.nearest_nodes(G, X=lng2, Y=lat2)

    if orig_node == dest_node:
        return jsonify({'error': 'The start and end nodes are the same. Please choose different locations.'}), 400

    G = ox.project_graph(G)
    with open('graph_matrix.txt', 'w') as f:
        f.write(str(nx.adjacency_matrix(G)))
    # print(nx.adjacency_matrix(G))
    nodes, edges = ox.graph_to_gdfs(G, fill_edge_geometry=True)

    for col in edges.columns:
        if edges[col].dtype == 'object':
            edges[col] = edges[col].apply(lambda x: str(x) if isinstance(x, list) else x)

    nodes.to_file('nodes.geojson', driver='GeoJSON')
    edges.to_file('edges.geojson', driver='GeoJSON')
 
    node_id_dict, json_data = formatData(algorithm, orig_node, dest_node)
    received_data = send_receive_esp32(json_data)

    inverted_node_id_dict = {v: k for k, v in node_id_dict.items()}
    path_osmids = [inverted_node_id_dict[id] for id in received_data['sw_dj_sht']]
    path_coords = [[nodes.loc[osmid].lat, nodes.loc[osmid].lon] for osmid in path_osmids if osmid in nodes.index]

    # times = [received_data['sw_astar_time'], received_data['hybrid_astar_time'], received_data['sw_dj_time'], received_data['hybrid_dj_time'], received_data['hw_dj_time'], received_data['delta_time'], received_data['hybrid_delta_time']]
    times = [0, 0, received_data['sw_dj_time'], received_data['hybrid_dj_time'], 0, 0, 0]
    
    response = {
        'status': 'success',
        #'total_distance': received_data['distance'],
        'runtimes': times,
        'path': path_coords,
        'algorithm': algorithm,
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
    #create_map() 
    app.run(debug=True)
