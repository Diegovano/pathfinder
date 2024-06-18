#include "graph.h"

#define HW_ACCEL true

bool operator< (const request& a, const request& b) // required for std::set<request>
{
  return a.target < b.target;
}

void Graph::dijkstra()
{
  for (int step = 0; step < NUM_VERTICES - 1; step++)
  {
    // get min distance node

    // int min = __INT_MAX__;
    float min = INFINITY;
    int min_index = -1;

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      #if HW_ACCEL
      if (!inShortestPath[i] && ALT_CI_LEF_1(dist[i], min))
      #else
      if (!inShortestPath[i] && dist[i] <= min)
      #endif
      {
        min = dist[i], min_index = i;
      }
    }

    inShortestPath[min_index] = true;

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      #if HW_ACCEL
      #ifdef __INTELLISENSE__
      #pragma diag_suppress 20 // ignore missing __builtin_stwio etc...
      #endif
      
      float newDist = ALT_CI_DIJKSTRA_CHECK_STEP_0(dist[min_index], graph[min_index][i]);
      // if (!inShortestPath[i] && newDist < dist[i]) 

      if (!inShortestPath[i] && ALT_CI_LTF_1(newDist, dist[i])) 
      #ifdef __INTELLISENSE__
      #pragma diag_default 20 // restore default behaviour
      #endif
      #else
      if(!inShortestPath[i] && graph[min_index][i] > 0 && dist[min_index] != INFINITY && dist[min_index] + graph[min_index][i] < dist[i])
      #endif
      {
        // printf("new dist[%d]: dist[%d] %f + graph[%d][%d] %f\n", i, min_index, dist[min_index], min_index, i, graph[min_index][i]);
        #if HW_ACCEL
        dist[i] = newDist;
        #else
        dist[i] = dist[min_index] + graph[min_index][i];
        #endif
        predecessor[i] = min_index;
      }
    }
  }
}

void Graph::delta(int p_delta)
{
  deltaVal = p_delta;

  for (int i = 0; i < 9; i++) buckets.push_back(std::set<int>());

  buckets.at(0).insert(start); // starting node

  predecessor[start] = start;

  while (true)
  {
    std::set<int> del;

    int firstNonEmpty = -1;
    for (unsigned int i = 0; i < buckets.size(); i++) 
      if (buckets.at(i).size()) 
      {
        firstNonEmpty = i;
        break;
      }

    if (firstNonEmpty == -1) break;

    while(buckets.at(firstNonEmpty).size())
    {
      std::set<request> reqs = findRequests(buckets.at(firstNonEmpty), true);
      del.insert(buckets.at(firstNonEmpty).begin(), buckets.at(firstNonEmpty).end());
      buckets.at(firstNonEmpty) = std::set<int>();

      for (request req : reqs) relax(req);
    }

    std::set<request> reqs = findRequests(del, false);
    for (request req : reqs) relax(req);
  }
}

std::set<request> Graph::findRequests(std::set<int> vertx, bool isLight)
{
  std::set<request> light, heavy;

  for (int vert : vertx)
  {
    for (int i = 0; i < NUM_VERTICES; i++)
    if(vert != i && graph[vert][i] <= deltaVal) // double check comparaison, optimal may be to split evenly when weight is equal to delta
      light.insert({i, dist[vert] + graph[vert][i], vert});
    else if (vert != i) heavy.insert({i, dist[vert] + graph[vert][i], vert});
  }

  return isLight ? light : heavy;
}

void Graph::relax(request req)
{
  if (req.newDist != __INT_MAX__ && req.newDist < dist[req.target])
  {
    buckets.at(floor(dist[req.target] / deltaVal)).erase(req.target);
    buckets.at(floor(req.newDist/deltaVal)).insert(req.target);

    dist[req.target] = req.newDist;
    predecessor[req.target] = req.pred;
  }
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

  } while (predecessor[vert_id] != 0 && i < 1000);

  printf("%d", start);
}


int* Graph::shortest() const
{
  return predecessor;
}
