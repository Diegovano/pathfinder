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
        dist[i] = dist[min_index] + graph[min_index][i];
        predecessor[i] = min_index;
      }
    }
  }
}

void Graph::print() const
{
  printf("distances from bottom left:\n");

  for (int i = 0; i < NUM_VERTICES; i++)
  {
    printf("%d: %d\n", i, dist[i]);
  }

  printf("path from top right: ");

  int vert_id = NUM_VERTICES - 1;

  printf("%d ", NUM_VERTICES - 1);

  do
  {
    printf("%d ", predecessor[vert_id]);

    vert_id = predecessor[vert_id];

  } while (predecessor[vert_id] != 0);

  printf("0");
}

const int* Graph::shortest() const
{
  return predecessor;
}
