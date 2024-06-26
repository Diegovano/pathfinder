#pragma once

#include "../ArduinoJson-v6.21.5.h"
using namespace ArduinoJson;

#include <algorithm> // for min
#include "system.h" // for ALT_CPU_*CACHE_SIZE

const int NUM_NODES = 1024 * 2 * 4;
// const int CAPACITY = 2 * JSON_OBJECT_SIZE(NUM_NODES) + 1 * JSON_OBJECT_SIZE(NUM_NODES ^ 2) + 4 * JSON_OBJECT_SIZE(1);
const int CAPACITY = 2*JSON_ARRAY_SIZE(0) + 50*JSON_ARRAY_SIZE(1) + 2*NUM_NODES*JSON_ARRAY_SIZE(2) + 88*JSON_ARRAY_SIZE(3) + 8*JSON_ARRAY_SIZE(4) + 3*JSON_ARRAY_SIZE(NUM_NODES) + JSON_OBJECT_SIZE(7);

struct Edge {
    int target;
    float length;
};
struct GraphFormat
{
  bool adjMatx;
  float *x, *y, **adj;
  int size;
  int start, end;
  int averageOver;

  GraphFormat()
  { }

  ~GraphFormat()
  {
    // delete[] x;
    // delete[] y;

    // for (int i = 0; i < size; i++) if(adj[i]) delete[] adj[i];
    // delete[] adj;
  }

  void matrixReshapeHWDijkstra(float *HWDijkstra_uncached_matrix);
};

struct ResultFormat
{
  int *shortest;
  int *swDijkstraShortest = nullptr, *dijkstraShortest = nullptr, *swAstarShortest = nullptr, *astarShortest = nullptr, *deltaShortest = nullptr;
  int start, end;
  float pathfindAvg;
  float swDijkstraAvg = 0, dijkstraAvg = 0, swAstarAvg = 0, astarAvg = 0, deltaAvg = 0, hwDijkstraAvg = 0;
  float distance;
};

// if return not "", then returns error details
std::string deserialiseGraph(std::string &input, GraphFormat *graph);

// if return not "", then returns error details
std::string serialiseResult(ResultFormat res, std::string &output);