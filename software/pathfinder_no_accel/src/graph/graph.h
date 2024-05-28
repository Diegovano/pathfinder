#include <iostream>

class Graph
{
  float **graph;

  float *dist;
  bool *inShortestPath;
  int *predecessor;

  const int NUM_VERTICES;

  public:
  Graph(float **inArr, int p_NUM_VERTICES) : NUM_VERTICES(p_NUM_VERTICES)
  {
    graph = new float*[NUM_VERTICES];
    for (int i = 0; i < NUM_VERTICES; i++) graph[i] = new float[NUM_VERTICES];

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      for (int j = 0; j < NUM_VERTICES; j++) graph[i][j] = inArr[i][j];
    }

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

  ~Graph()
  {
    for (int i = 0; i < NUM_VERTICES; i++) delete[] graph[i];
    delete[] graph;

    delete[] dist;
    delete[] inShortestPath;
    delete[] predecessor;
  }

  void dijkstra();

  void print() const;

  int* shortest() const;
};