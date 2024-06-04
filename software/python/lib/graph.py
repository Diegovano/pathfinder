import json
import sys
import random
import math

class Graph:

    def __init__(self, size):
        self.x = []
        self.y = []
        self.adj = []
        self.size = size

        # Generate random coordinate pairs:
        for i in range(size):
            self.x.append(abs(random.uniform(0, 1)))
            self.y.append(abs(random.uniform(0, 1)))

        # Generate default adjacency matrix with no edges
        for i in range(size):
            self.adj.append([-1] * i + [0] + [-1] * (size - 1 - i))

        # Generate edges randomly
        for i in range(size):
            for j in range(i+1,size):
                if (round(random.uniform(0,1))):
                    w = math.sqrt((self.x[i] - self.x[j]) ** 2 + (self.y[i] - self.y[j]) ** 2)
                    self.adj[i][j] = w
                    self.adj[j][i] = w

        # Generate random index of starting vertex
        self.start = math.floor(random.uniform(0, 1) * size)
        
        # Generate random index of end vertex, different to start
        self.end = self.start
        while(self.end == self.start):
            self.end = math.floor(random.uniform(0,1) * size)  
    
    def get_weight(self, x, y):
        return self.adj[y][x]
    
    def set_weight(self, x, y, w):
        self.adj[y][x] = w

    def to_json(self):
        json_out = json.dumps(self, default=lambda o: o.__dict__)
        return json_out
