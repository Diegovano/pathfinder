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
      #if HW_ACCEL
      float newDist = ALT_CI_DIJKSTRA_CHECK_STEP_1(dist[min_index], graph[min_index][i]);

      // if (!inShortestPath[i] && newDist < dist[i]) 
      if (!inShortestPath[i] && newDist < dist[i])
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

  buckets[0] = std::set<int>({start});

  predecessor[start] = start;

  int iter = 0;

  while (true)
  {
    printf("Starting iteration %d\n", ++iter);
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
