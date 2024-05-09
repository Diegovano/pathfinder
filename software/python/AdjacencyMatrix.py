import json

class Adjacency_Matrix:
    def __init__(self, V):
        self.matrix = []
        self.V = V
        for i in range(V):
            self.matrix.append([-1] * V)
        
        print(self.matrix)
    
    def get_weight(self, x, y):
        return self.matrix[y][x]
    
    def set_weight(self, x, y, w):
        self.matrix[y][x] = w

    def print_matrix(self):
        print(self.matrix)

    def to_json(self, fname):
        with open((fname+'.json'), 'w') as f:
            json_output = json.dumps(self, indent = 4, default=lambda o: o.__dict__) 
            f.write(json_output)

    def dec_array(self, fname):

        str_arr = str(self.matrix).replace('[','{').replace(']','}')

        with open((fname+'.txt'), 'a') as f:
            f.write(f'int myarr[{self.V}][{self.V}] = {str_arr}\n')

graph = Adjacency_Matrix(4)

