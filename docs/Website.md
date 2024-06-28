
# Pathfinding Website Documentation

## Table of Contents
- [Introduction](#introduction)
- [Key Features](#key-features)
- [Installation](#installation)
    - [Prerequisites](#prerequisites)
    - [Install Dependencies](#install-dependencies)
- [Running the Application](#running-the-application)
- [Code Explanation](#code-explanation)
- [Templates](#templates)
- [Note](#note)


This documentation supports a Flask web application that provides an interface for pathfinding on a map, using geographical data. The application uses Folium for map rendering and OSMnx for obtaining street network data. It includes functionality for processing coordinates, creating a map, formatting data, and communicating with an ESP32 device.

### Key Features

- **Interactive Map Interface**: Users can select start and end points on a map.
- **Path Visualisation**: The shortest path between the points is visualised on the map.
- **Data Fetching**: The website fetches necessary map data to ensure accurate and up-to-date information.
- **Data Transmission**: Sends the selected path data to an ESP32 microcontroller for additional processing and use.

## Prerequisites

Before proceeding with the installation, ensure you meet the following requirements:

- **Python 3.12.4 or higher**: This application requires Python 3.12.4 or newer. You can check your Python version by running the following command in your terminal:

```bash
python --version
# or if you have multiple Python versions installed:
python3 --version
```

If you do not have Python 3.12.4 or higher, please visit [Python's official website](https://www.python.org/downloads/) to download and install the latest version.

## Installation
This installation guide assumes that the repository has already been cloned. 
## Install Dependencies

Before you run the application, you'll need to install the necessary Python libraries. Use the following commands in your terminal:

```bash
# Use pip or pip3 depending on your Python installation
pip install Flask folium osmnx 
# or
pip3 install Flask folium osmnx 
```

Here are the links to the official documentation for each library, which you might find helpful:

- [Flask](https://flask.palletsprojects.com/en/2.2.x/) - A lightweight WSGI web application framework.
- [Folium](https://python-visualization.github.io/folium/) - A library for creating interactive maps with Python.
- [OSMnx](https://osmnx.readthedocs.io/en/stable/) - A package to retrieve, model, analyse, and visualise street networks from OpenStreetMap.
- [Socket](https://docs.python.org/3/library/socket.html) - Low-level networking interface (note: `socket` is part of Python's standard library, so no installation is required for this one).

Ensure you have these libraries installed to proceed with running the application.

## Running the Application

### Generate the Initial Map

Uncomment the `create_map()` line in the `if __name__ == "__main__":` block of `app.py` and run the script once to generate `map.html`.

```python
if __name__ == "__main__":
    create_map()
    app.run(debug=True)
```

Then comment it back to avoid regenerating the map on each run. 

### Run the Flask Application

```bash
python app.py
```

### Open in Browser

Open your web browser and navigate to `http://127.0.0.1:5000` to see the map.

## Code Explanation

### `app.py`

#### Imports and Initial Setup

```python
from flask import Flask, render_template, request, jsonify
import folium
import osmnx as ox
import json
import socket
```

- `Flask`, `render_template`, `request`, and `jsonify` are imported from Flask to handle web requests and responses.
- `folium` and its plugins are used for creating and rendering maps.
- `osmnx` is used to fetch the node and edge data associated with the map.
- `json` and `socket` handle JSON data and socket communication respectively.

#### Global Variables

```python
coordinates_array = []
```

This variable is used to store coordinates inputted on the front-end.

#### Flask App Initialisation

```python
app = Flask(__name__)
```

#### Helper Functions

- `create_map()`: Function to generate and store an initial map with interactive markers. 
    ### Explanation

    1. **Initialise Map**:
        - The function initialises a Folium map centred at the specified coordinates with a given zoom level.
        ```python
        folium_map = folium.Map(location=[51.4988, -0.1749], zoom_start=14)
        ```

    2. **Add Clickable Marker**:
        - Adds a feature to the map that allows users to click and add markers. Each marker will have a popup with the text 'Potential Location'.
        ```python
        folium_map.add_child(folium.ClickForMarker(popup='Potential Location'))
        ```

    3. **Save the Map**:
        - Saves the created map to an HTML file named `map.html` in the `templates` directory.
        ```python
        folium_map.save('templates/map.html')
        ```


- `formatData(algorithm, start, end)`: Formats the graph data into a JSON structure suitable for transmission to the ESP32. 
    ### Explanation

    1. **Loading Data**:
        - The function begins by loading node and edge data from `nodes.geojson` and `edges.geojson` files using the `json.load()` method.
        ```python
        with open('nodes.geojson') as f:
            nodes_data = json.load(f)
        with open('edges.geojson') as f:
            edges_data = json.load(f)
        ```

    2. **Mapping Node IDs to Indices**:
        - It creates a dictionary `node_id_to_index` that maps each node's OSMID (`osmid`) to its index in the `nodes_list`.
        ```python
        node_id_to_index = {node['properties']['osmid']: i for i, node in enumerate(nodes_list)}
        ```

    3. **Initialising the Adjacency List**:
        - An empty adjacency list `adj_list` is initialised for all nodes in the graph.
        ```python
        adj_list = {i: [] for i in range(len(nodes_list))}
        ```

    4. **Populating the Adjacency List**:
        - The function iterates over each edge in the `edges_list`, retrieves the start and end node IDs, their corresponding indices, and the edge weight (length).
        - It then populates the adjacency list with tuples containing the end node index and the edge weight.
        ```python
        for edge in edges_list:
            start_node_id = edge['properties']['u']
            end_node_id = edge['properties']['v']
            weight = edge['properties']['length']
            start_node_index = node_id_to_index[start_node_id]
            end_node_index = node_id_to_index[end_node_id]
            adj_list[start_node_index].append((end_node_index, weight))
        ```

    5. **Extracting Coordinates**:
        - The function extracts the x and y coordinates for each node and stores them in `x_coordinates` and `y_coordinates` lists.
        ```python
        x_coordinates = []
        y_coordinates = []

        for start_node_index, connections in adj_list.items():
            coordinates = nodes_list[start_node_index]['geometry']['coordinates']
            x_coordinate = coordinates[0]
            y_coordinate = coordinates[1]
            x_coordinates.append(x_coordinate)
            y_coordinates.append(y_coordinate)
        ```

    6. **Formatting Data for JSON**:
        - The adjacency list and coordinates are formatted into a dictionary `adj_list_formatted` that includes:
            - `adjList`: The adjacency list with edge weights.
            - `x`: List of x coordinates for each node.
            - `y`: List of y coordinates for each node.
            - `size`: Total number of nodes.
            - `start`: Index of the start node.
            - `end`: Index of the end node.
            - `avgCount`: Number of times the pathfinding algorithm should run on the FPGA.
        ```python
        adj_list_formatted = {
            "adjList": [[(end_node_index, length) for end_node_index, length in connections] for connections in adj_list.values()],
            "x": x_coordinates,
            "y": y_coordinates,
            "size": len(nodes_list),
            "start": node_id_to_index[start],
            "end": node_id_to_index[end],
            "avgCount": 100
        }
        ```

    7. **Converting to JSON**:
        - The formatted dictionary is converted to a JSON string `adj_list_json` using `json.dumps()`.
        ```python
        adj_list_json = json.dumps(adj_list_formatted, indent=None, separators=(',', ':'),)
        ```

    8. **Return Values**:
        - The function returns the `node_id_to_index` dictionary and the formatted JSON string.
        ```python
        return node_id_to_index, adj_list_json
        ```
- `send_receive_esp32(json_data)`: Transmits JSON data to an ESP32 device, waits for the response, and then returns the parsed data for further processing.
    ### Explanation
    1. **Establish Connection**:
        - The function establishes a socket connection to the ESP32 device using the specified IP address and port.
        ```python
        esp32_ip = "192.168.1.10"
        esp32_port = 12345
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((esp32_ip, esp32_port))
        ```
    2. **Send Data**:
        - It sends the JSON data to the ESP32 device, encoding it as UTF-8.
        ```python
        s.sendall(json_data.encode('utf-8'))
        ```
    3. **Receive Response**:
        - The function waits to receive the response from the ESP32 device, decoding it from UTF-8.
        ```python
        received_data = s.recv(1024).decode('utf-8')
        ```
    4. **Return Parsed Data**:
        - Finally, it parses the received data from JSON format and returns it.
        ```python
        return json.loads(received_data)
        ```

#### Routes

- `index()`: Renders the initial map.
- `pathfind()`: Handles pathfinding requests, generates and formats the graph data, and sends it to the ESP32.
- `post()`: Accepts coordinates for processing.

#### Running the App

```python
if __name__ == "__main__":
    app.run(debug=True)
```

Starts the Flask application.



## Templates

### `templates/map.html`

This HTML file contains the structure for rendering the map and the user interface for interacting with the pathfinding feature. Here are the key elements and their functions:

#### HTML Structure 
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Pathfinder</title>
    <style>
        /* CSS styles for map and UI elements */
    </style>
    <script>
        /* JavaScript for handling map interactions and loading screen */
    </script>
</head>
<body>
    <div id="loadingScreen" class="loading-screen">
        <img src="{{ url_for('static', filename='images/loading.png') }}" alt="Loading" />
    </div>
    <div class="folium-map" id="map_e752e21955fbc03701e3fb6487e3e6fb"></div>
    <div id='container' style="position: fixed; top:20px;right:20px; z-index:1000">
        <div class="side-box">
            <!-- UI elements for route selection and path details -->
        </div>
    </div>
    <script src="https://cdn.jsdelivr.net/npm/leaflet@1.9.3/dist/leaflet.js"></script>
    <script src="https://code.jquery.com/jquery-3.7.1.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.2/dist/js/bootstrap.bundle.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/Leaflet.awesome-markers/2.0.2/leaflet.awesome-markers.js"></script>
    <script>
        // JavaScript for map initialisation and marker handling
    </script>        
</body>
</html>
```

#### CSS Styles
The CSS in the `<style>` section defines the appearance of the map and UI elements. This includes styles for the loading screen, map container, and various UI components such as buttons and text boxes.

#### Javascript Functionality 
- **Loading Screen**: The loading screen is displayed when the page loads and is hidden with an animation when clicked, or a specific time has elapsed.
- **Map Initialisation**: The Leaflet Map is initialised with OpenStreetMap tiles.
- **Marker Handling**: Functions for adding and removing markers, drawing a circle indicating the radius of computation, drawing polylines for the path, and fetching place names using the Nominatim API.

#### Interactive Elements 
- **Route Selection**: Users can select start and end points on the map, which are marked with different coloured markers.
- **Calculate Button**: Triggers the pathfinding process.

## Note

- The `distance` parameter can be adjusted to increase the number of nodes and edges being sent to the ESP32. This, however, was limited due to the memory capacity of the ESP32.
