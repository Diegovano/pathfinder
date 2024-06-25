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
