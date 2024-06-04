import socket
import lib.graph as graph
import time
import json
import lib.logger as logger

HOST = "192.168.137.13"
PORT = 80

logger = logger.Logger()
logger.loadLog('31_05_2024.csv')

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

    print(f"Received {data}")
    logger.logEntry(data)
    logger.saveLog('31_05_2024.csv')


    time.sleep(10)