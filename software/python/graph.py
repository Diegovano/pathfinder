import json
import sys
import random
import math

class Graph:

    def __init__(self, V):
        self.x = []
        self.y = []
        self.adj = []
        self.V = V

        # Generate random coordinate pairs:
        for i in range(V):
            self.x.append(abs(random.uniform(0, 1)))
            self.y.append(abs(random.uniform(0, 1)))

        # Generate default adjacency matrix with no edges
        for i in range(V):
            self.adj.append([-1] * i + [0] + [-1] * (V - 1 - i))

        # Generate edges randomly
        for i in range(V):
            for j in range(i+1,V):
                if (round(random.uniform(0,1))):
                    w = math.sqrt((self.x[i] - self.x[j]) ** 2 + (self.y[i] - self.y[j]) ** 2)
                    self.adj[i][j] = w
                    self.adj[j][i] = w

        # Generate random index of starting vertex
        self.start = math.floor(random.uniform(0, 1) * V)
        
        # Generate random index of end vertex, different to start
        self.goal = self.start
        while(self.goal == self.start):
            self.goal = math.floor(random.uniform(0,1) * V)  
    
    def get_weight(self, x, y):
        return self.adj[y][x]
    
    def set_weight(self, x, y, w):
        self.adj[y][x] = w

    def to_json(self):
        json_out = json.dumps(self, default=lambda o: o.__dict__)
        return json_out
