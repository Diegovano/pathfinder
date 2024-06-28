# Known Problems

Some of the known bugs listed here, including current methods that are used to bypass them. 

### Wrong path vector written
When the path starts and end are not the 0th node and but the shortest path goes through the 0th node, a wrong path vector can be written. Some of index of the path vector are wrongly directed to the 0th node. Further confirmation is needed to check if this error can be triggered by other situations. 

The problem appears to be bypassed by setting the 0th node as an "orphan" node not connected to anywhere else when initiating the adjacency matrix

### Manual "reset" required
The reset of the path vector and distance vector are not working properly such that the hardware is only working every other time. Every other time the hardware will always return a path vector which shows none of the nodes are connected and a shortest distance of infinite. 

It is identified that the first run after a full system reset is always wrong. 

This error is bypassable by initating a pathfinding for a size 1 matrix from a node to itself before running actual pathfinding on the full matrix, acting as a manual reset. This action is expected to only take 1 clock cycle and hence shoud not have a performance overhead.  