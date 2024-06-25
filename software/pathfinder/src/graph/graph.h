#include <iostream>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include "../DMA/DMA.h"
#include "../StarAccelerator/StarAccelerator.h"
#include "system.h"
// #include <cstdio>
// #include <cstddef>
// #include "system.h"
// #include "io.h"
// #include "sys/alt_irq.h"
// #include "alt_types.h"
// #include "sys/alt_timestamp.h"
// #include "sys/alt_cache.h"
// #include <unistd.h>

extern float __builtin_custom_fnff(int a, float b, float c);
extern int __builtin_custom_inff(int a, float b, float c);

#pragma GCC target("custom-fcmpnes=226")
#pragma GCC target("custom-fcmpeqs=227")
#pragma GCC target("custom-fcmpges=228")
#pragma GCC target("custom-fcmpgts=229")
#pragma GCC target("custom-fcmples=230")
#pragma GCC target("custom-fcmplts=231")
#pragma GCC target("custom-fmuls=252")
#pragma GCC target("custom-fadds=253")
#pragma GCC target("custom-fsubs=254")
#pragma GCC target("custom-fdivs=255")

#define ALT_CI_HW_DIJKSTRA_2(n,A,B) __builtin_custom_fnii(ALT_CI_HW_DIJKSTRA_0_N+(n&ALT_CI_HW_DIJKSTRA_0_N_MASK),(A),(B))
#define ALT_CI_HW_DIJKSTRA_2_N 0x0
#define ALT_CI_HW_DIJKSTRA_2_N_MASK ((1<<4)-1)

#define ALT_CI_DIJKSTRA_CHECK_STEP_1(A,B) __builtin_custom_fnff(ALT_CI_DIJKSTRA_ACCEL_0_N,(A),(B))

struct request
{
  int target;
  float newDist;
  int pred;
};

struct Node {
    float x;
    float y;
};

class Graph
{
  float **graph;
  float *dist;
  int *predecessor;

  const int NUM_VERTICES;
  const int start;
  const int end;

  bool *inShortestPath;

  std::set<int> lightEdges, heavyEdges;
  std::map<int, std::set<int>> buckets;
  int deltaVal;

  std::vector<request> findRequests(std::set<int> vertx, bool isLight);
  void relax(request req);

  std::vector<Node> nodes;

  float getDistance(DMA& dma, StarAccelerator& accel, const Node& n1, const Node& n2);

  public:
  Graph(float **inArr, int p_NUM_VERTICES, int p_start, int p_end) : NUM_VERTICES(p_NUM_VERTICES), start(p_start), end(p_end), deltaVal(1)
  {
    graph = new float*[NUM_VERTICES];
    for (int i = 0; i < NUM_VERTICES; i++) graph[i] = new float[NUM_VERTICES];

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      for (int j = 0; j < NUM_VERTICES; j++) graph[i][j] = inArr[i][j] < 0 ? INFINITY : inArr[i][j];
    }

    dist = new float[NUM_VERTICES]; 
    inShortestPath = new bool[NUM_VERTICES];
    predecessor = new int[NUM_VERTICES];

    for (int i = 0; i < NUM_VERTICES; i++)
    {
      dist[i] = INFINITY;
      inShortestPath[i] = false;
    }

    dist[start] = 0; // dist to source is 0
    predecessor[start] = 0;
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
  void HW_dijkstra(float *mem_address, DMA &dma);
  void delta(int p_delta = 1);
  void astar(DMA& dma, StarAccelerator& accel);

  void reset();

  void printAdj() const;
  void print() const;

  int* shortest() const;
};
