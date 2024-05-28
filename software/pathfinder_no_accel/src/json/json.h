#include "../ArduinoJson-v6.21.5.h"
using namespace ArduinoJson;

const int NUM_NODES = 100;
const int CAPACITY = 2 * JSON_OBJECT_SIZE(NUM_NODES) + 1 * JSON_OBJECT_SIZE(NUM_NODES ^ 2) + 3 * JSON_OBJECT_SIZE(1);

struct GraphFormat
{
  float *x, *y, **adj;
  const int size;
  int start, end;

  GraphFormat(int numberVertices) : size(numberVertices)
  {
    x = new float[size];
    y = new float[size];

    adj = new float*[size];
    for (int i = 0; i < size; i++) adj[i] = new float[size]; 
  }
};

struct ResultFormat
{
  int *shortest;
  int start, end;
};

std::string deserialiseGraph(std::string input, GraphFormat &graph);

void serialiseResult(ResultFormat res, std::string &output);