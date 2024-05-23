import socket
import graph
import time
import json

HOST = "192.168.137.13"
PORT = 80

while(True):

    g = graph.Graph(3)
    graphSerial = g.to_json()
    #print(graphSerial+'\n')

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(bytes(str(graphSerial)+"\n\n", 'utf8'))
        print("Sent GRAPH")
        data = s.recv(1024).decode('utf8')
    s.close()


    #shortestPath = json.loads(data)
    #print(shortestPath)

    print(f"Received {data}")

    time.sleep(10)