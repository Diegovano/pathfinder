#include "graph.h"

void Graph::dijkstra()
{
  for (int step = 0; step < NUM_VERTICES - 1; step++)
  {
    // get min distance node

    int min = __INT_MAX__;
    int min_index = -1;

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      if (!inShortestPath[i] && dist[i] <= min) 
      {
        min = dist[i], min_index = i;
      }
    }

    inShortestPath[min_index] = true;

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      if (!inShortestPath[i] && graph[min_index][i] > 0 && dist[min_index] != __INT_MAX__ && dist[min_index] + graph[min_index][i] < dist[i]) 
      {
        // printf("new dist[%d]: dist[%d] %f + graph[%d][%d] %f\n", i, min_index, dist[min_index], min_index, i, graph[min_index][i]);
        dist[i] = dist[min_index] + graph[min_index][i];
        predecessor[i] = min_index;
      }
    }
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
    dist[i] = __INT_MAX__;
    inShortestPath[i] = false;
  }

  dist[0] = 0; // dist to source is 0 (0 is bottom left)
  predecessor[0] = 0;
}

void Graph::print() const
{
  printf("\n\ndistances from bottom left:\n");

  for (int i = 0; i < NUM_VERTICES; i++)
  {
    printf("%d: %f\n", i, dist[i]);
  }

  printf("path from top right: ");

  int vert_id = NUM_VERTICES - 1;

  printf("%d ", NUM_VERTICES - 1);

  int i = 0;

  do
  {
    printf("%d ", predecessor[vert_id]);

    vert_id = predecessor[vert_id];

  } while (predecessor[vert_id] != 0 && i < 1000);

  printf("0");
}

int* Graph::shortest() const
{
  return predecessor;
}
