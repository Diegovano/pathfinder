import math

class Node:
    def __init__(self, osmid, x, y, lat, lon, neighbours, isLandmark=False):
        self.osmid = osmid
        self.x   = x
        self.y   = y
        self.lat = lat
        self.lon = lon
        self.neighbours = neighbours
        self.isLandmark = isLandmark

    def get(self, attr_name, default=None):
        # Check if the requested attribute is one of the basic attributes
        if attr_name in ['osmid', 'x', 'y','lat', 'lon', 'neighbours']:
            return getattr(self, attr_name, default)
        # Otherwise, return from custom attributes
        return None
    
    def getDistanceTo(self, target, type=1):
        if type == 1:
            return math.sqrt(((self.x - target.x) ** 2) + ((target.y - self.y) ** 2))
        elif type == 2:
            return (((target.x - self.x) ** 2) + ((target.y - self.y) ** 2))
            #return (((target.x - self.x) ** 2) + ((target.y - self.y) ** 2)) * Q_rsqrt((((target.x - self.x) ** 2) + ((target.y - self.y) ** 2)))

    def __repr__(self):
        return f"Node(osmid={self.osmid}, x={self.x}, y={self.y}, neighbours={self.neighbours}, isLandmark={self.isLandmark})"
