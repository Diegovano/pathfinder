The main chunk of this code is taken from the repo https://github.com/Property404/dijkstra-verilog

## TODO

The edge cache currently stores a complete row of edges, up to maximum of nodes declared as a constant. Modify this to support up to unknown number of nodes, with hit and miss. Assume the graph follows linear traversal access pattern. there is also no concept of cacheline access to memory

It is not exactly acting as a cache