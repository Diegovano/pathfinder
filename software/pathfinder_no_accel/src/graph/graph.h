#include <iostream>

class Graph
{
  int **graph;

  int *dist;
  bool *inShortestPath;
  int *predecessor;

  const int NUM_VERTICES;

  public:
  Graph(int **inArr, int p_NUM_VERTICES) : NUM_VERTICES(p_NUM_VERTICES)
  {
    graph = new int*[NUM_VERTICES];
    for (int i = 0; i < NUM_VERTICES; i++) graph[i] = new int[NUM_VERTICES];

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      for (int j = 0; j < NUM_VERTICES; j++) graph[i][j] = inArr[i][j];
    }

    dist = new int[NUM_VERTICES]; 
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

  void dijkstra();

  void print() const;

  const int* shortest() const;
};