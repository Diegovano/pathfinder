#include "graph.h"

#define HW_ACCEL true

bool operator< (const request& a, const request& b) // required for std::set<request>
{
  return a.target < b.target;
}

void Graph::swDijkstra()
{
  for (int step = 0; step < NUM_VERTICES - 1; step++)
  {
    // for (int i = 0; i < NUM_VERTICES; i++) printf("%d:\t%c %f\t", i, inShortestPath[i] ? 'y' : 'n', dist[i]);    // get min distance node

    float min = INFINITY;
    int min_index = -1;

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      if (!inShortestPath[i] && dist[i] <= min)
      {
        min = dist[i], min_index = i;
      }
    }
    if (min_index == -1) return;

    inShortestPath[min_index] = true;

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      if(!inShortestPath[i] && graph[min_index][i] > 0 && dist[min_index] != INFINITY && dist[min_index] + graph[min_index][i] < dist[i])
      {
        // printf("new dist[%d]: dist[%d] %f + graph[%d][%d] %f\n", i, min_index, dist[min_index], min_index, i, graph[min_index][i]);
        dist[i] = dist[min_index] + graph[min_index][i];
        predecessor[i] = min_index;
      }
    }
  }
}

void Graph::dijkstra()
{
  for (int step = 0; step < NUM_VERTICES - 1; step++)
  {
    // for (int i = 0; i < NUM_VERTICES; i++) printf("%d:\t%c %f\t", i, inShortestPath[i] ? 'y' : 'n', dist[i]);    // get min distance node

    float min = INFINITY;
    int min_index = -1;

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      if (!inShortestPath[i] && ALT_CI_LEF(dist[i], min))
      {
        min = dist[i], min_index = i;
      }
    }
    if (min_index == -1) return;

    inShortestPath[min_index] = true;

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      float newDist = ALT_CI_DIJKSTRA_CHECK_STEP_1(dist[min_index], graph[min_index][i]);

      // if (!inShortestPath[i] && newDist < dist[i]) 
      if (!inShortestPath[i] && ALT_CI_LTF(newDist, dist[i]))
      {
        // printf("new dist[%d]: dist[%d] %f + graph[%d][%d] %f\n", i, min_index, dist[min_index], min_index, i, graph[min_index][i]);
        dist[i] = newDist;
        predecessor[i] = min_index;
      }
    }
  }
}

void Graph::delta(int p_delta)
{
  deltaVal = p_delta;

  buckets[0] = std::set<int>({start});

  predecessor[start] = start;

  while (true)
  {
    std::set<int> del;

    int firstNonEmpty = -1;
    for (unsigned int i = 0; i < buckets.size(); i++) 
      if (buckets.count(i) && buckets[i].size()) // check set exists and is non-empty
      {
        firstNonEmpty = i;
        break;
      }

    if (firstNonEmpty == -1) break;

    while(buckets.at(firstNonEmpty).size())
    {
      std::vector<request> reqs = findRequests(buckets.at(firstNonEmpty), true);
      del.insert(buckets.at(firstNonEmpty).begin(), buckets.at(firstNonEmpty).end());
      buckets.at(firstNonEmpty) = std::set<int>();

      for (request req : reqs) relax(req);
    }

    std::vector<request> reqs = findRequests(del, false);
    for (request req : reqs) relax(req);
  }
}

std::vector<request> Graph::findRequests(std::set<int> vertx, bool isLight)
{
  std::vector<request> light, heavy;

  for (int vert : vertx)
  {
    for (int i = 0; i < NUM_VERTICES; i++)
    {
      if(vert != i && graph[vert][i] <= deltaVal) // double check comparaison, optimal may be to split evenly when weight is equal to delta
        light.push_back({i, dist[vert] + graph[vert][i], vert});
      else if (vert != i) heavy.push_back({i, dist[vert] + graph[vert][i], vert});
    }
  }

  // printf("Made %s requests\n", isLight ? "light" : "heavy");

  return isLight ? light : heavy;
}

void Graph::relax(request req)
{
  if (req.newDist < dist[req.target])
  {
    // printf("relaxing %d, new dist: %f prev %f\n", req.target, req.newDist, dist[req.target]);
    int ida = floor(dist[req.target] / deltaVal), idb = floor(req.newDist / deltaVal);

    if (buckets.count(ida)) buckets[ida].erase(req.target);
    if (buckets.count(idb)) buckets[idb].insert(req.target);
    else buckets[idb] = std::set<int>({req.target});

    dist[req.target] = req.newDist;
    predecessor[req.target] = req.pred;
  }
}

float Graph::getDistance(DMA& dma, StarAccelerator& accel, const Node& n1, const Node& n2)
{
  accel.input[0] = n1.x;
  accel.input[1] = n2.x;
  accel.input[2] = n1.y;
  accel.input[3] = n2.y;

  dma.copy((void*)(accel.input), (void*)(accel.base() + 4), 16);

  accel.exec();

  dma.copy((void*)(accel.base() + 36), (void*)(accel.res), 4);

  return *(accel.res);
}

void Graph::astar(DMA& dma, StarAccelerator& accel)
{

  // g_value is a vector of floats which stores the cost of the cheapest path to a node
  std::vector<float> g_value(NUM_VERTICES, INFINITY);

  // f_value is a vector of floats which stores the estimated total cost from the start node to the goal node
  std::vector<float> f_value(NUM_VERTICES, INFINITY);

  // came_from is a vector of ints which stores the parent node of a node
  std::vector<int> came_from(NUM_VERTICES, -1);

  // closed_nodes is a set of ints which stores the nodes that have been visited
  std::set<int> closed_nodes;

  // initialise the g value of the source node to 0
  g_value[start] = 0;
  // initialise the f value of the source node to the heuristic value
  f_value[start] = getDistance(dma, accel, nodes[start], nodes[end]);

  // create a priority queue of pairs of floats and ints which represents the f value and the node
  std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> queue;
  // push the source node onto the queue
  queue.push({0.0f, start});

  while (!queue.empty())
  {
    // get the node with the smallest f value
    int current_node = queue.top().second;
    queue.pop();

    // if the node is in the closed nodes set, skip the rest of the loop
    if (closed_nodes.count(current_node)) continue;
    // if the node is the target node, we have found the path
    if (current_node == end) break;

    // add the node to the closed nodes set
    closed_nodes.insert(current_node);

    // for each neighbour of the current node
    for (int i = 0; i < NUM_VERTICES; i++)
    {
      // if the neighbour node is in the closed nodes set, skip the rest of the loop
      if (closed_nodes.count(i)) continue;

      // calculate the tentative g value
//            float temp_g_score = g_value[current_node]+edge.length;
      float temp_g_score = ALT_CI_ADDF(g_value[current_node], graph[current_node][i]);

      // if the tentative g value is less than the g value of the neighbour node
      if (ALT_CI_LTF(temp_g_score, g_value[i]))
      {
          // update the came from, g value and f value of the neighbour node
          came_from[i] = current_node;
          g_value[i] = temp_g_score;
//                f_value[neighbour] = temp_g_score+get_distance(dma, accel, nodes[neighbour], nodes[target]);
          f_value[i] = ALT_CI_ADDF(temp_g_score, getDistance(dma, accel, nodes[i], nodes[end]));
          // push the f value and the neighbour node to the queue
          queue.push({f_value[i], i});
      }
    }
  }

  // reconstruct the path
  std::vector<int> path;
  // start from the target node
  for (int current_node = end; current_node != -1; current_node = came_from[current_node]) {
    path.push_back(current_node);
  }
  // std::reverse(path.begin(), path.end());
  
  predecessor = path.data();
}

void Graph::reset()
{
  delete[] dist;
  delete[] inShortestPath;
  delete[] predecessor;

  dist = new float[NUM_VERTICES]; 
  inShortestPath = new bool[NUM_VERTICES];
  predecessor = new int[NUM_VERTICES];

  for (int i = 0; i < NUM_VERTICES; i++)
  {
    dist[i] = INFINITY;
    inShortestPath[i] = false;
  }

  dist[start] = 0; // dist to source is 0 (0 is bottom left)
  predecessor[start] = 0;
}

void Graph::printAdj() const
{
  printf("[");
  for (int i = 0; i < NUM_VERTICES; i++)
  {
    printf("[");
    for (int j = 0; j < NUM_VERTICES; j++)
    {
      printf("%f", graph[i][j]);
      if (j < NUM_VERTICES - 1) printf(",\t");
    }
    printf("]");
    if (i < NUM_VERTICES - 1) printf(",\n");
  }
  printf("]\n\n");
}

void Graph::print() const
{
  printf("\n\ndistances from bottom %d:\n", start);

  for (int i = 0; i < NUM_VERTICES; i++)
  {
    printf("%d: %f\n", i, dist[i]);
  }

  printf("path from %d: ", end);

  int vert_id = end;

  printf("%d ", end);

  int i = 0;

  do
  {
    printf("%d ", predecessor[vert_id]);

    vert_id = predecessor[vert_id];

  } while (predecessor[vert_id] != start && predecessor[vert_id] < NUM_VERTICES && predecessor[vert_id] >= 0 && i++ < 1000);

  printf("%d", start);
}


int* Graph::shortest() const
{
  return predecessor;
}

float Graph::distance(int index) const
{
  return dist[index];
}

void Graph::HW_dijkstra(float *mem_address, DMA &dma)
{
  const unsigned int cache_address = HW_DIJKSTRA_0_BASE;

   dma.copy((void*)mem_address, (void*)(cache_address), 65536);

 	ALT_CI_HW_DIJKSTRA_2(2, 0, 0); //a manual software reset cuz hardware reset buggy
	long dataa = (end << 16) + start;
	long datab = NUM_VERTICES;
	ALT_CI_HW_DIJKSTRA_2(2, dataa, datab);
}
