#pragma once

#include "../ArduinoJson-v6.21.5.h"
using namespace ArduinoJson;

#include <algorithm> // for min
#include "system.h" // for ALT_CPU_*CACHE_SIZE

const int NUM_NODES = 219;
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
  const int size;
  int start, end;
  int averageOver;

  GraphFormat(int numberVertices) : size(numberVertices)
  {
    x = new float[size];
    y = new float[size];

    adj = new float*[size];
    for (int i = 0; i < size; i++) adj[i] = new float[size]; 
  }

  ~GraphFormat()
  {
    delete[] x;
    delete[] y;

    for (int i = 0; i < size; i++) delete[] adj[i];
    delete[] adj;
  }
};

struct ResultFormat
{
  int *shortest;
  int start, end;
  float pathfindAvg;
};

// if return not "", then returns error details
std::string deserialiseGraph(std::string &input, GraphFormat &graph);

// if return not "", then returns error details
std::string serialiseResult(ResultFormat res, std::string &output);

void GraphFormat::Full_HW_dijkstra_reshape(float* HWDijkstra_uncached_matrix);
