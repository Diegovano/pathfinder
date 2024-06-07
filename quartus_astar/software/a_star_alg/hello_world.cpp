#include <vector>
#include <cmath>
#include <queue>
#include <iostream>
#include <set>
#include <algorithm>
#include <cstdio>

#include <sys/alt_cache.h>
#include <sys/times.h>
#include <sys/alt_timestamp.h>
#include <system.h>

#define ALT_CI_FP_ADD_0(A,B) __builtin_custom_fnff(ALT_CI_FP_ADD_0_N,(A),(B))
#define ALT_CI_FP_ADD_0_N 0x1
#define ALT_CI_FP_MULT_0(A,B) __builtin_custom_fnff(ALT_CI_FP_MULT_0_N,(A),(B))
#define ALT_CI_FP_MULT_0_N 0x2
#define ALT_CI_FP_SQRT_0(A) __builtin_custom_fnf(ALT_CI_FP_SQRT_0_N,(A))
#define ALT_CI_FP_SQRT_0_N 0x3
#define ALT_CI_FP_SUB_0(A,B) __builtin_custom_fnff(ALT_CI_FP_SUB_0_N,(A),(B))
#define ALT_CI_FP_SUB_0_N 0x0

constexpr double PI = 3.14159265358979323846;
const float MAX_FLOAT = 3.40282e+38;

// Struct for a Node
struct Node {
    float x;
    float y;
};

// Struct for an Edge
struct Edge {
    int target;
    float length;
};

// Function to calculate the Euclidean distance between two nodes
float getDistance(const Node& node1, const Node& node2) {
	float node_x = node1.x;
	float dx     = ALT_CI_FP_SUB_0(node_x, node1.x);
	float xsq    = ALT_CI_FP_MULT_0(dx,dx);

	float node_y = node1.y;
	float dy     = ALT_CI_FP_SUB_0(node_y,node2.y);
	float ysq    = ALT_CI_FP_MULT_0(dy,dy);

	float addition_part = ALT_CI_FP_ADD_0(xsq, ysq);

	float distance = ALT_CI_FP_SQRT_0(addition_part);
	return distance;
    //return sqrt((node1.x - node2.x) * (node1.x - node2.x) + (node1.y - node2.y) * (node1.y - node2.y));
}

std::vector<int> findPath(const std::vector<Node>& nodes, const std::vector<std::vector<Edge>>& adjList, int source, int target) {

    // g_value is a vector of floats which stores the cost of the cheapest path to a node
    std::vector<float> g_value(nodes.size(), MAX_FLOAT);

    // f_value is a vector of floats which stores the estimated total cost from the start node to the goal node
    std::vector<float> f_value(nodes.size(), MAX_FLOAT);

    // came_from is a vector of ints which stores the parent node of a node
    std::vector<int> came_from(nodes.size(), -1);

    // closed_nodes is a set of ints which stores the nodes that have been visited
    std::set<int> closed_nodes;

    // initialise the g value of the source node to 0
    g_value[source] = 0;
    // initialise the f value of the source node to the heuristic value
    f_value[source] = getDistance(nodes[source], nodes[target]);

    // create a priority queue of pairs of floats and ints which represents the f value and the node
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> queue;
    // push the source node onto the queue
    queue.push({0.0f, source});

    // while the queue is not empty
    while (!queue.empty()) {
        // get the node with the smallest f value
        int current_node = queue.top().second;
        // remove the node from the queue
        queue.pop();

        // if the node is in the closed nodes set, skip the rest of the loop
        if (closed_nodes.count(current_node)) continue;
        // if the node is the target node, we have found the path
        if (current_node == target) break;

        // add the node to the closed nodes set
        closed_nodes.insert(current_node);

        // for each neighbour of the current node
        for (auto& edge : adjList[current_node]) {
            // get the neighbour node
            int neighbour = edge.target;
            // if the neighbour node is in the closed nodes set, skip the rest of the loop
            if (closed_nodes.count(neighbour)) continue;

            // calculate the tentative g value
            float temp_g_score = g_value[current_node] + edge.length;
            //float temp_g_score = ALT_CI_FP_ADD_0(g_value[current_node],edge.length);

            // if the tentative g value is less than the g value of the neighbour node
            if (temp_g_score < g_value[neighbour]) {
                // update the came from, g value and f value of the neighbour node
                came_from[neighbour] = current_node;
                g_value[neighbour] = temp_g_score;
                f_value[neighbour] = temp_g_score + getDistance(nodes[neighbour], nodes[target]);
                //f_value[neighbour] = ALT_CI_FP_ADD_0(temp_g_score,getDistance(nodes[neighbour], nodes[target]));
                // push the f value and the neighbour node to the queue
                queue.push({f_value[neighbour], neighbour});
            }
        }
    }

    // reconstruct the path
    std::vector<int> path;
    // start from the target node
    for (int current_node = target; current_node != -1; current_node = came_from[current_node]) {
        path.push_back(current_node);
    }
    std::reverse(path.begin(), path.end());

    return path;
}

int main() {
    // create nodes list which is a vector of Node objects
    std::vector<Node> nodes(13);

    // create adjacency list which is a vector of vectors of Edge objects
    std::vector<std::vector<Edge>> adjList(13);

    // Initialise the nodes list with the coordinates of the nodes
    nodes[0]  = {5709222.9834205909, 696108.62011866691}; // Node 276548
    nodes[1]  = {5709191.8782056486, 696118.27943041269}; // Node 25291708
    nodes[2]  = {5709211.2912476128, 696055.84373104712}; // Node 25291709
    nodes[3]  = {5709236.2511749854, 696033.52349726949}; // Node 25473589
    nodes[4]  = {5709284.5927414894, 695997.60383067257}; // Node 25473590
    nodes[5]  = {5709208.1398356995, 696036.22818386171}; // Node 25473592
    nodes[6]  = {5709009.2122519007, 696190.09544731071}; // Node 26389375
    nodes[7]  = {5709019.8233349705, 696188.06609560957}; // Node 26389434
    nodes[8]  = {5709006.0310580395, 696323.27681472222}; // Node 26389442
    nodes[9]  = {5709029.0720731048, 696311.54391213995}; // Node 32618386
    nodes[10] = {5709012.7329801535, 696147.49481777672}; // Node 34519894
    nodes[11] = {5709225.9788216352, 696298.79496300314}; // Node 1691189781
    nodes[12] = {5709058.8068370707, 696308.78188286372}; // Node 3764277424

    // Initialise the adjacency list with the edges
    adjList[0]  = {{6, 33.532}, {12, 53.887}};
    adjList[1]  = {{11, 62.337}, {12, 33.73}};
    adjList[2]  = {{12, 19.803}, {1, 28.225}};
    adjList[3]  = {{8, 10.796}, {5, 154.774}, {4, 122.668}};
    adjList[4]  = {{3, 122.668}};
    adjList[5]  = {{3, 154.774}};
    adjList[6]  = {{0, 33.532}, {10, 181.396}, {9, 183.129}};
    adjList[7]  = {{8, 143.458}};
    adjList[8]  = {{10, 41.055}, {3, 10.796}, {7, 143.458}};
    adjList[9]  = {{6, 183.129}};
    adjList[10] = {{8, 41.055}, {6, 181.396}};
    adjList[11] = {{1, 62.337}};
    adjList[12] = {{0, 53.887}, {2, 19.803}};

    // source and target nodes
    int source = 4;
    int target = 10;

    int iterations = 100;
    std::vector<int> result;

    // start the timer
    //clock_t exec_t1, exec_t2, total_time = 0;

    alt_64 proc_ticks = 0;
	alt_u64 time1 = 0;
	alt_u64 time3 = 0;

	alt_timestamp_start();

    for (int i = 0; i < iterations; ++i) {
        //exec_t1 = times(NULL);
    	alt_icache_flush_all();
    	alt_dcache_flush_all();

    	time1 = alt_timestamp();
    	result = findPath(nodes, adjList, source, target);
        time3 = alt_timestamp();
    	//exec_t2 = times(NULL);
    	//total_time += (exec_t2 - exec_t1);
        proc_ticks += (time3 - time1);
    }

    int k = alt_timestamp_freq() * 1e-6 * iterations;
    double proc_us = (double)proc_ticks / (double)k;

    printf("Profiling Results: %i iteration(s), \nproc_ticks: %lld,\tproc_us: %f\tavg: %f\n",
                iterations, proc_ticks, proc_us, proc_us);

    // Print the final path and the average duration
    std::cout << "Final path: ";
    for (int node : result) {
        std::cout << node << " ";
    }
    std::cout << std::endl;

//    double average_time = (double)total_time / iterations;
//    printf("Average processing time = %f ticks\n", average_time);

    return 0;
}
