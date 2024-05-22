import socket
import graph
import time

HOST = "192.168.137.114"
PORT = 80

while(True):

    g = graph.Graph(3)
    graphSerial = g.to_json()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(bytes(str(graphSerial)+"\n\n", 'ascii'))
        data = s.recv(1024).decode('utf8')
    s.close()

    print(f"Received {data}")