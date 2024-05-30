#include <tuple>
#include <map>
#include <vector>
#include <cmath>
#include <queue>
#include <algorithm>
#include <iostream>
#include <set>

// Class for a Node
class Node {
    public:
        Node() = default;
        Node(long long id, float x, float y, std::vector<long long> neighbours) {
            this->id = id;
            this->x = x;
            this->y = y;
            this->neighbours = neighbours;
        }

        long long getId() const {
            return id;
        }

        float getX() const {
            return x;
        }

        float getY() const {
            return y;
        }

        std::vector<long long> getNeighbours() const {
            return neighbours;
        }

        float getDistanceTo(Node node) {
            const float R = 6371000; // Radius of the earth in meters
            float lat1 = this->y * M_PI / 180.0;
            float lon1 = this->x * M_PI / 180.0;
            float lat2 = node.getY() * M_PI / 180.0;
            float lon2 = node.getX() * M_PI / 180.0;

            float dLat = lat2 - lat1;
            float dLon = lon2 - lon1;

            float a = std::sin(dLat / 2) * std::sin(dLat / 2) +
                    std::cos(lat1) * std::cos(lat2) *
                    std::sin(dLon / 2) * std::sin(dLon / 2);
            float c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

            float distance = R * c;
            return distance;
        }

    private:
        long long id;
        float x;
        float y;
        std::vector<long long> neighbours;
};

// Class for an Edge
class Edge {
    public:
        Edge() = default;
        Edge(long long id, float length) {
            this->id = id;
            this->length = length;
        }

        long long getId() const {
            return id;
        }

        float getLength() const {
            return length;
        }

    private:
        long long id;
        float length;
};

int main() {

    std::map<long long, Node> nodes_dict;
    std::map<std::vector<long long>, Edge> edge_dict;

    // example data
    nodes_dict[276548]     = Node(276548, -0.1744846, 51.5000732, {26389375, 3764277424});
    nodes_dict[25291708]   = Node(25291708, -0.1755578, 51.5002184, {1691189781, 3764277424});
    nodes_dict[25291709]   = Node(25291709, -0.1755345, 51.499965,  {3764277424, 25291708});
    nodes_dict[25473589]   = Node(25473589, -0.1734311, 51.4981251, {26389442, 25473592, 25473590});
    nodes_dict[25473590]   = Node(25473590, -0.1716726, 51.4982613, {25473589});
    nodes_dict[25473592]   = Node(25473592, -0.1715166, 51.4980503, {25473589});
    nodes_dict[26389375]   = Node(26389375, -0.1743629, 51.4997905, {276548, 34519894, 32618386});
    nodes_dict[26389434]   = Node(26389434, -0.1716958, 51.4985293, {26389442});
    nodes_dict[26389442]   = Node(26389442, -0.1734544, 51.4982211, {34519894, 25473589, 26389434});
    nodes_dict[32618386]   = Node(32618386, -0.1717465, 51.5000341, {26389375});
    nodes_dict[34519894]   = Node(34519894, -0.1740421, 51.4981715, {26389442, 26389375});
    nodes_dict[1691189781] = Node(1691189781, -0.1760478, 51.500665, {25291708});
    nodes_dict[3764277424] = Node(3764277424, -0.1752505, 51.4999865, {25291709, 276548});

    edge_dict[{276548, 26389375}]     = Edge(2693172, 33.532);
    edge_dict[{26389375, 276548}]     = Edge(2693172, 33.532);
    edge_dict[{276548, 3764277424}]   = Edge(299032751, 53.887);
    edge_dict[{3764277424, 276548}]   = Edge(299032751, 53.887);
    edge_dict[{26389375, 34519894}]   = Edge(2693172, 181.396);
    edge_dict[{34519894, 26389375}]   = Edge(2693172, 181.396);
    edge_dict[{26389375, 32618386}]   = Edge(23106603, 183.129);
    edge_dict[{32618386, 26389375}]   = Edge(23106603, 183.129);
    edge_dict[{3764277424, 25291709}] = Edge(299032751, 19.803);
    edge_dict[{25291709, 3764277424}] = Edge(299032751, 19.803);
    edge_dict[{25291708, 1691189781}] = Edge(372864575, 62.337);
    edge_dict[{1691189781, 25291708}] = Edge(372864575, 62.337);
    edge_dict[{25291708, 3764277424}] = Edge(372864578, 33.73);
    edge_dict[{25291709, 25291708}]   = Edge(8304508, 28.225);
    edge_dict[{25473589, 26389442}]   = Edge(5038288, 10.796);
    edge_dict[{26389442, 25473589}]   = Edge(5038288, 10.796);
    edge_dict[{25473589, 25473592}]   = Edge(5038288, 154.774);
    edge_dict[{25473592, 25473589}]   = Edge(5038288, 154.774);
    edge_dict[{25473589, 25473590}]   = Edge(673383050, 122.668);
    edge_dict[{25473590, 25473589}]   = Edge(673383050, 122.668);
    edge_dict[{26389442, 34519894}]   = Edge(5038287, 41.055);
    edge_dict[{34519894, 26389442}]   = Edge(5038287, 41.055);
    edge_dict[{26389442, 26389434}]   = Edge(5038288, 143.458);
    edge_dict[{26389434, 26389442}]   = Edge(5038288, 143.458);

    // random source and target
    int source = 25473590;
    int target = 34519894;

    std::map<int, int> g_value;
    std::map<int, float> f_value;
    std::map<long long, long long> came_from;

    g_value[source] = 0;

    std::set<long long> closed_nodes;

    // prioirty queue of vectors of long longs (f_value, node) in ascending order of f_value 
    std::priority_queue<std::vector<long long>, std::vector<std::vector<long long>>, std::greater<std::vector<long long>>> queue;

    // push the source node to the queue
    queue.push({0ll, source});

    // while the queue is not empty
    while(!queue.empty()) {
        // get the top element of the queue
        std::vector<long long> top = queue.top();
        // remove the top element from the queue
        queue.pop();

        // get the current node 
        long long current_node = top[1];

        
        // if the current node is in the closed nodes set, continue
        if (closed_nodes.count(current_node) > 0) {
            continue;
        }

        // if the current node is the target node, break
        if (current_node == target) {
            break;
        }

        // add the current node to the closed nodes set
        closed_nodes.insert(current_node);

        // get the neighbours of the current node
        std::vector<long long> cn_neighbours = nodes_dict[current_node].getNeighbours();

        // for each neighbour of the current node
        for (int neighbour : cn_neighbours) {
            // if the neighbour is in the closed nodes set, continue
            if (closed_nodes.count(neighbour) > 0) {
                continue;
            }

            // get the edge between the current node and the neighbour
            Edge edge = edge_dict[{current_node, neighbour}];

            // if the edge length is not 0
            if (edge.getLength() != 0) {
                // calculate the temp g score
                float temp_g_score = g_value[current_node] + edge.getLength();
                // if the neighbour is not in the g value map or the temp g score is less than the g value of the neighbour
                if (g_value.find(neighbour) == g_value.end() || temp_g_score < g_value[neighbour]) {
                    // update the came from map, g value map and f value map
                    came_from[neighbour] = current_node;
                    g_value[neighbour] = temp_g_score;
                    f_value[neighbour] = temp_g_score + nodes_dict[current_node].getDistanceTo(nodes_dict[target]);
                    // push the f value and the neighbour to the queue
                    std::vector<long long> vec; 
                    vec.push_back(f_value[neighbour]);
                    vec.push_back(neighbour);
                    queue.push(vec);
                }
            }

        }

    }

    // get the path
    std::vector<int> path;
    int current_node = target;
    while (came_from[current_node] != 0) {
        path.push_back(current_node);
        current_node = came_from[current_node];
    }
    path.push_back(source);
    std::reverse(path.begin(), path.end());

    // print out the path
    // in this example it should be [25473590, 25473589, 26389442, 34519894]
    for (int node : path) {
        std::cout << node << " ";
    }
    std::cout << std::endl;

    return 0;
}
