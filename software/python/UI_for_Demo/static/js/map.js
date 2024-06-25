document.addEventListener("DOMContentLoaded", function() {
    // Get the element with the class name 'folium-map'
    var map = document.querySelector('.folium-map');

    // Ensure the map element exists and get its ID
    if (map) {
        var mapId = mapElement.id;

        // Check if the map is already initialized
        if (!mapElement._leaflet_map) {
            // Initialize the map using the retrieved ID

            L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
                attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
            }).addTo(map);

            // Save the map instance to the element to avoid reinitialization
            mapElement._leaflet_map = map;

            // Add click event listener to the map
            map.on('click', function(e) {
                var lat = e.latlng.lat;
                var lng = e.latlng.lng;

                // Send a POST request to the server with the coordinates
                fetch('/post', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify({ coordinates: [lat, lng] })
                });
            });
        }
    } else {
        console.error("Map element with class 'folium-map' not found.");
    }
});
