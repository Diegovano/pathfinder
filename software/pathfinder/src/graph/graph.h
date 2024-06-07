#include <iostream>
#include <cmath>

#define ALT_CI_DIJKSTRA_CHECK_STEP_0(A,B) __builtin_custom_fnff(ALT_CI_DIJKSTRA_CHECK_STEP_0_N,(A),(B))
#define ALT_CI_DIJKSTRA_CHECK_STEP_0_N 0x0
#define ALT_CI_LEF_0(A,B) __builtin_custom_inff(ALT_CI_LEF_0_N,(A),(B))
#define ALT_CI_LEF_0_N 0x1
#define ALT_CI_LTF_0(A,B) __builtin_custom_inff(ALT_CI_LTF_0_N,(A),(B))
#define ALT_CI_LTF_0_N 0x2

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
      dist[i] = INFINITY;
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

  void reset();

  void print() const;

  int* shortest() const;
};