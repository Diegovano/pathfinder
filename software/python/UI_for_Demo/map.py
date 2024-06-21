from flask import Flask, render_template, request, jsonify
import folium
from folium.plugins import MarkerCluster, MiniMap
import networkx as nx 

coordinates_array = []
processing = False

app = Flask(__name__)


def process_coordinates(coordinates):
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

    print((lat1, lng1), (lat2, lng2))

    return jsonify({'status': 'success'}), 200


@app.route('/post', methods=['POST'])
def post():
    if processing: return

    coordinates = request.json['coordinates']
    
    if len(coordinates_array) >= 2:
        coordinates_array.clear()
        
    coordinates_array.append(coordinates)
    
    return jsonify({'status': 'success'}), 200

if __name__ == "__main__":
    # Generate the map once when the server starts
    # create_map()
    app.run(debug=True)