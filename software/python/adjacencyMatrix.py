import json
import sys

class Adjacency_Matrix:
    def __init__(self, V):
        self.matrix = []
        self.V = V
        for i in range(V):
            self.matrix.append([-1] * i + [0] + [-1] * (V - 1 - i))
    
    def get_weight(self, x, y):
        return self.matrix[y][x]
    
    def set_weight(self, x, y, w):
        self.matrix[y][x] = w

    def print_matrix(self):
        str_arr = str(self.matrix).replace('[','{').replace(']','}')
        print(f'int myArr[{self.V}][{self.V}] = {str_arr};\n')

    def to_json(self, fname):
        with open((fname+'.json'), 'w') as f:
            json_output = json.dumps(self, indent = 4, default=lambda o: o.__dict__) 
            f.write(json_output)

    def dec_array(self, fname):

        str_arr = str(self.matrix).replace('[','{').replace(']','}')

        with open((fname+'.txt'), 'a') as f:
            f.write(f'int myarr[{self.V}][{self.V}] = {str_arr};\n')

if len(sys.argv) > 1:
    size = int(sys.argv[1])
else:
    size = 4

graph = Adjacency_Matrix(size)
graph.print_matrix()

while True:
    start_id = int(input("Enter node id: "))
    end_id = int(input("now the other one: "))
    w = int(input("weight: "))

    if start_id != end_id:
        graph.matrix[min(max(start_id, 0), size - 1)][min(max(end_id, 0), size - 1)] = w
        graph.matrix[min(max(end_id, 0), size - 1)][min(max(start_id, 0), size - 1)] = w

    graph.print_matrix()

