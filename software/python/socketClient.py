import socket
import generateMap as mg

HOST = "172.20.10.9"
PORT = 80

_, graph, _, _, _  = mg.generateGraph(10)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(bytes(str(graph)+"\n\n", 'utf8'))
    data = s.recv(1024)
s.close()

print(f"Received {str(data)!r}")